import paramiko
import time
import datetime
from scp import SCPClient
import configparser
import os
import numpy as np
import h5py
from core.core import Core
from core.sxr_protocol_pb2 import MainPacket, AdcStatus, SystemStatus
from core.sxr_protocol import packet_init
from threading import Thread
from dev.insys.bardy.getCodesNames import *
from insys.EXAM.exam_adc.exam_protocol_pb2 import BRD_ctrl
from core.netmanager_simple import NetManagerSimple
from core.fileutils import work_dir, today_dir


class Channel:
    def __init__(self, gain=1., bias=0., on=False):
        self.gain = gain
        self.bias = bias
        self.on = on
        self.data = np.ndarray((0,))


class Board:
    def __init__(self, nch=8):
        self.channels = [Channel() for _ in range(nch)]

    @property
    def n_active_ch(self):
        n = 0
        for ch in self.channels:
            if ch.on:
                n += 1
        return n

    @property
    def channel_mask(self):
        bit = 1
        mask = 0
        for ch in self.channels:
            if ch.on:
                mask += bit
            bit <<= 1
        return mask

    @channel_mask.setter
    def channel_mask(self, mask):
        for ch in self.channels:
            ch.on = bool(mask & 1)
            mask >>= 1


class ADC(Core):
    def __init__(self, parent=None, nboards=1, connect=True, wdir=None):
        super().__init__(parent)
        self.address = 1

        self.boards = [Board() for _ in range(nboards)]

        self.connected = False
        self.client = None
        self.ssh = None
        self.ssh_timeout = 0.5
        self.file_base = 'data_0'
        self.wdir = os.path.abspath('./')
        self.scp = None
        self.config = None
        self.isAcqComplete = False
        self.started = False

        config = configparser.ConfigParser(inline_comment_prefixes=(';', '//'))
        # TODO check size of SamplesPerChannel, that defines internal buffer size.
        # full buffer size = SamplesPerChannel*2*num_chan shouldn't exceed 4Mb
        config.optionxform = str

        # read config from directory of launch
        if os.path.exists(os.path.join(self.wdir, 'adc.ini')):
            config.read(os.path.join(self.wdir, 'adc.ini'), "utf-8")
            self.config = config

            mask = self.get_cfg_item('device0_fm814x250m0', 'ChannelMask')
            if mask is not None:
                mask = int(mask, 16)
            else:
                mask = 0

            self.boards[0].channel_mask = mask

            for ch_n in range(len(self.boards[0].channels)):
                bias = self.get_cfg_item('device0_fm814x250m0', f'Bias{ch_n}')
                if bias is not None:
                    self.boards[0].channels[ch_n].bias = float(bias)

        # make directory whit current date, to store adc memory dumps and  cfg.ini for sending
        if wdir is None:
            self.wdir = today_dir()

        if connect:
            self.make_connection()

        adc_watcher = NetManagerSimple(self)
        adc_watcher.channel0.connect(self.channel1)
        adc_watcher.channel0.connect(self.channel2_slot)

    def make_connection(self):
        client = paramiko.SSHClient()
        self.client = client
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        try:
            client.connect(hostname="192.168.0.242", username="adc_user", password="adc_user", look_for_keys=False,
                           allow_agent=False)
            self.connected = True
            ssh = client.invoke_shell()
            self.ssh = ssh
            ssh.send('cd /home/embedded/examples\n')
            self.ssh_output(0.5)

            scp = SCPClient(client.get_transport())
            self.scp = scp
            self.send_config()

            connection_watch = Thread(name='Thread-connection-watchdog', target=self.watchdog, daemon=True)
            connection_watch.start()

            response = packet_init(0, self.address)
            response.command = 0xFFFFFFFF
            response.data = 'ADC connected'.encode()
            if response.IsInitialized():
                self.channel0.emit(response.SerializeToString())
        except:
            self.connected = False

    def ssh_output(self, timeout=None):
        if timeout is None:
            timeout = self.ssh_timeout
        time.sleep(timeout)

        while self.ssh.recv_ready():
            try:
                page = self.ssh.recv(3000)
                self.channel2.emit(page)
                time.sleep(self.ssh_timeout)
            except:
                break

    def send_config(self):
        config = self.config
        with open(os.path.join(self.wdir, 'cfg.ini'), 'w') as f:
            config.write(f)
        if self.connected:
            self.scp.put(os.path.join(self.wdir, 'cfg.ini'), '/home/embedded/examples/exam_adc.ini')

    def start(self, response=None):
        # separate function for ability start ADC(thread) "manually", not only by command in packet
        self.send_config()
        thrd = Thread(name='Thread-adc-start', target=self.run, args=(response,), daemon=True)
        thrd.start()

    def run(self, response=None):
            self.started = True
            self.isAcqComplete = False
            if os.path.exists(os.path.join(self.wdir, self.file_base+'.bin')):
                os.remove(os.path.join(self.wdir, self.file_base+'.bin'))

            #TODO remove using ssh shell. Use exec_command instead

            # if connected run exam_adc
            if self.connected:
                self.ssh.send('/home/embedded/examples/exam_adc\n')
            else:
                self.started = False
                self.isAcqComplete = True

            # wait for exam_adc ending or stopping by user
            while self.started:
                pass

            # if data acquired get adc memory dump file
            if self.isAcqComplete:
                if self.connected:
                    # if connected copy from adc
                    self.scp.get('/home/embedded/examples/data_0.bin', self.wdir)

                    # wait for file transfer completes
                    while not(os.path.exists(os.path.join(self.wdir, self.file_base+'.bin'))):
                        pass
                else:
                    self.generate_data()
                # read dump from disk (generated or loaded from adc)
                dump = np.fromfile(os.path.join(self.wdir, self.file_base + '.bin'), dtype=np.int16)

            else:
                dump = np.ndarray((0,))

            dump = dump.reshape((-1, self.boards[0].n_active_ch)).T
            cols = (_ for _ in dump)

            for ch in self.boards[0].channels:
                if ch.on:
                    ch.data = next(cols)
                else:
                    ch.data = np.ndarray((0,))

            filename = self.snapshot()

            if response is not None:
                response.data = filename.encode()
                if response.IsInitialized():
                    self.channel0.emit(response.SerializeToString())

    def channel0_slot(self, data: bytes):
        request = MainPacket()
        request.ParseFromString(data)
        if request.address == self.address:
            response = packet_init(request.sender, self.address)
            response.command = request.command

            if request.command == 0:
                self.status_message(response)
            elif request.command == 1:
                self.status_to_config(request.data, response)
            elif request.command == 2:
                self.start(response)
            elif request.command == 3:
                self.stop()
            elif request.command == 4:
                self.reboot()
            elif request.command == 5:
                self.make_connection()

    def channel2_slot(self, data: bytes):
        pkt = BRD_ctrl()
        pkt.command = 0
        pkt.out = 0
        pkt.status =0
        pktSize = pkt.ByteSize()

        if len(data) == pktSize:
            pkt.ParseFromString(data)
            if pkt.IsInitialized():
                cmd = getCmdName(pkt.command)
                if cmd == 'BRDctrl_SDRAM_ISACQCOMPLETE':
                    self.isAcqComplete = bool(pkt.out)
                elif cmd == 'BRDctrl_STREAM_CBUF_FREE':
                    self.started = False

    def status_message(self, response=None):
        status = AdcStatus()

        name = self.get_cfg_item('Option', 'AdcServiceName')
        if name is not None:
            status.name = name

        status.connected = self.connected

        rate = self.get_cfg_item('device0_fm814x250m0', 'SamplingRate')
        if rate is not None:
            status.sampling_rate = int(rate)

        samples = self.get_cfg_item('Option', 'MemSamplesPerChan')
        if samples is not None:
            status.samples = int(samples)

        start = self.get_cfg_item('device0_fm814x250m0', 'StartSource')
        if start is not None:
            if int(start) == 0:
                status.start = AdcStatus.IN0
            elif int(start) == 2:
                status.start = AdcStatus.EXTSTART
            elif int(start) == 3:
                status.start = AdcStatus.SOFTSTART

        stop = self.get_cfg_item('device0_fm814x250m0', 'StopSource')
        if stop is not None:
            if int(stop) == 0:
                status.stop = AdcStatus.SOFTSTART

        stop = self.get_cfg_item('device0_fm814x250m0', 'StopSource')
        if stop is not None:
            if int(stop) == 0:
                status.stop = AdcStatus.SOFTSTART

        clock = self.get_cfg_item('device0_fm814x250m0', 'ClockSource')
        if clock is not None:
            if int(clock, 16) == 0:
                status.clock_source = AdcStatus.CLOCKOFF
            elif int(clock, 16) == 1:
                status.clock_source = AdcStatus.CLOCKINT
            elif int(clock, 16) == 2:
                status.clock_source = AdcStatus.CLOCKEXT

        memory = self.get_cfg_item('Option', 'DaqIntoMemory')
        if memory is not None:
            if int(memory) == 0:
                status.memory_type = AdcStatus.MEMHOST
            elif int(memory) == 1:
                status.memory_type = AdcStatus.MEMINT
            elif int(memory) == 2:
                status.memory_type = AdcStatus.MEMFIFO

        ch_mask = self.get_cfg_item('device0_fm814x250m0', 'ChannelMask')
        if ch_mask is not None:
            if len(status.board_status) < 1:
                status.board_status.add()
            status.board_status[0].channel_mask = self.boards[0].channel_mask.to_bytes(1, 'big')

        for ch_n in range(len(self.boards[0].channels)):
            bias = self.get_cfg_item('device0_fm814x250m0', f'Bias{ch_n}')
            if len(status.board_status) < 1:
                status.board_status.add()
            if bias is not None:
                if len(status.board_status[0].channel_status) < ch_n+1:
                    status.board_status[0].channel_status.add()
                status.board_status[0].channel_status[ch_n].enabled = self.boards[0].channels[ch_n].on
                status.board_status[0].channel_status[ch_n].bias = float(bias)

        if response is None:
            return status.SerializeToString()
        else:
            response.data = status.SerializeToString()
            if response.IsInitialized():
                self.channel0.emit(response.SerializeToString())

    def get_cfg_item(self, sec, key):
        if sec in self.config:
            if key in self.config[sec]:
                return self.config[sec][key]
            else:
                return None
        else:
            return None

    def status_to_config(self, status, response=None):
        if isinstance(status, bytes):
            data = status
            status = AdcStatus()
            status.ParseFromString(data)

        self.config['Option']['DaqIntoMemory'] = str(status.memory_type)
        self.config['Option']['MemSamplesPerChan'] = str(status.samples)

        if len(status.board_status) > 0:
            if len(status.board_status[0].channel_mask) > 0:
                self.config['device0_fm814x250m0']['ChannelMask'] =\
                    f'0x{int().from_bytes(status.board_status[0].channel_mask, "big"):02X}'
                self.boards[0].channel_mask = int().from_bytes(status.board_status[0].channel_mask, "big")
            else:
                for ch, ch_ in zip(status.board_status[0].channel_status, self.boards[0].channels):
                    ch_.on = ch.enabled
                self.config['device0_fm814x250m0']['ChannelMask'] = f'0x{self.boards[0].channel_mask:02X}'

        if status.clock_source == status.CLOCKOFF:
            self.config['device0_fm814x250m0']['ClockSource'] = '0x0'
        elif status.clock_source == status.CLOCKINT:
            self.config['device0_fm814x250m0']['ClockSource'] = '0x1'
        elif status.clock_source == status.CLOCKEXT:
            self.config['device0_fm814x250m0']['ClockSource'] = '0x81'
        self.config['device0_fm814x250m0']['SamplingRate'] = str(status.sampling_rate)

        if len(status.board_status) > 0:
            n_ch = -1
            for ch_status in status.board_status[0].channel_status:
                n_ch += 1
                self.config['device0_fm814x250m0'][f'Bias{n_ch}'] = f'{ch_status.bias:4.2f}'
                self.boards[0].channels[n_ch].bias = ch_status.bias

        if status.start == status.SOFTSTART:
            self.config['device0_fm814x250m0']['StartSource'] = '3'
            self.config['device0_fm814x250m0']['StartBaseSource'] = '0'
        elif status.start == status.EXTSTART:
            self.config['device0_fm814x250m0']['StartSource'] = '2'
            self.config['device0_fm814x250m0']['StartBaseSource'] = '7'
        elif status.start == status.IN0:
            self.config['device0_fm814x250m0']['StartSource'] = '0'
            self.config['device0_fm814x250m0']['StartBaseSource'] = '7'

        if response is not None:
            self.status_message(response)

    def generate_data(self):
        samples = self.get_cfg_item('Option', 'MemSamplesPerChan')
        if samples is None:
            samples = int(1e4)
        else:
            samples = int(samples)

        dump = np.ndarray((samples, self.boards[0].n_active_ch), dtype=np.int16)
        col = 0
        for ch in self.boards[0].channels:
            if ch.on:
                ch.data = ((np.random.normal(ch.bias*2**15/100, 4*ch.gain, samples)) // 4 * 4).astype(np.int16)
                dump[:, col] = ch.data
                col += 1

        dump = dump.reshape((-1, 1)).squeeze()
        with open(os.path.join(self.wdir, self.file_base + '.bin'), 'w') as f:
            dump.tofile(f)
        return dump

    def stop(self):
        if self.connected:
            self.ssh.send(b'\x1B')
        self.started = False
    
    def reboot(self):
        if self.connected and os.path.exists(os.path.join(work_dir(),'root_key')):
            client = paramiko.SSHClient()
            client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            client.connect(hostname="192.168.0.242", username="root", key_filename='root_key', password='root')
            ssh = client.invoke_shell()
            ssh.send('reboot\n')
        else:
            response = packet_init(0, self.address)
            response.command = 0xFFFFFFFF
            response.data = 'ADC disconnected or key file not found'.encode()
            if response.IsInitialized():
                self.channel0.emit(response.SerializeToString())

    def watchdog(self, timeout=2):
        transp = self.client.get_transport()
        transp.set_keepalive(timeout*2)
        while self.connected:
            self.connected = transp.is_alive()
            time.sleep(timeout)

        response = packet_init(0, self.address)
        response.command = 0xFFFFFFFF
        response.data = 'ADC disconnected'.encode()
        if response.IsInitialized():
            self.channel0.emit(response.SerializeToString())
        self.make_connection()

    def snapshot(self, file: str = None):
        timestamp = time.time()
        if file is None:
            curtime = datetime.datetime.fromtimestamp(timestamp)
            file = f'adc{curtime.year-2000:02d}{curtime.month:02d}{curtime.day:02d}' \
                     f'{curtime.hour:02d}{curtime.minute:02d}{curtime.second:02d}.h5'
            file = os.path.join(self.wdir, file)
        if os.path.exists(file):
            hf = h5py.File(file, 'r+')
        else:
            hf = h5py.File(file, 'w')
            hf.create_dataset('timestamp', data=timestamp)
            hf['/'].attrs['timestamp'] = timestamp
        if 'ADC' in hf:
            adc = hf['ADC']
        else:
            adc = hf.create_group('ADC')

        adc.attrs['name'] = 'InSys FM814X250M'
        adc.attrs['bit'] = 14
        adc.attrs['max_count'] = 2**15
        adc.attrs['min_count'] = -2**15

        if 'config' in adc:
            cfg = adc['config']
        else:
            cfg = adc.create_group('config')
            cfg.attrs['timestamp'] = timestamp

        for sec in self.config:
            if len(self.config[sec].keys()) > 0:
                cfg.create_group(sec)
                for key in self.config[sec]:
                    cfg[sec][key] = self.config[sec][key]

        for ch in self.boards[0].channels:
            if ch.on:
                dset = adc.create_dataset(f'channel{self.boards[0].channels.index(ch):02d}', shape=ch.data.shape,  compression="gzip", compression_opts=1, data=ch.data)
                dset.attrs['units'] = 'adc counts'
                dset.attrs['timestamp'] = timestamp

        hf.close()
        return file