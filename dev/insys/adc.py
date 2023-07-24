import paramiko
import time
import datetime
from scp import SCPClient
import configparser
import os
import numpy as np
import h5py
from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, AdcStatus, SystemStatus, Commands
from core.sxr_protocol import packet_init
from threading import Thread
from dev.insys.bardy.getCodesNames import *
from insys.EXAM.exam_adc.exam_protocol_pb2 import BRD_ctrl
from core.netmanagers import NetManagerSimple
from core.fileutils import work_dir, today_dir


class Channel:
    def __init__(self, gain=1., bias=0., on=False, void=False, name='No name'):
        self.gain = gain
        self.bias = bias
        self.on = on
        self.data = np.ndarray((0,))
        self.void = void
        self.name = name


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


class ADC(Dev):
    def __init__(self, parent=None, nboards=1, connect=True, wdir=None):
        super().__init__(parent, SystemStatus.ADC, AdcStatus())

        self.boards = [Board() for _ in range(nboards)]

        self.connected = False
        self.client = None
        self.ssh = None
        self.ssh_timeout = 0.5
        self.file_base = 'data_0'
        # self.wdir = os.path.abspath('./')
        self.wdir = work_dir()
        self.scp = None
        self.config = None
        self.isAcqComplete = False
        self.started = False

        config = configparser.ConfigParser(inline_comment_prefixes=(';', '//'))
        # TODO check size of SamplesPerChannel, that defines internal buffer size.
        # full buffer size = SamplesPerChannel*2*num_chan shouldn't exceed 4Mb
        config.optionxform = str

        # read config from directory of launch
        if os.path.exists(os.path.join(self.wdir, 'dev/insys/adc.ini')):
            config.read(os.path.join(self.wdir, 'dev/insys/adc.ini'), "utf-8")
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

        self.status = AdcStatus()
        self.get_status()

        # make directory whit current date, to store adc memory dumps and  cfg.ini for sending
        if wdir is None:
            self.wdir = work_dir()

        if connect:
            self.connection_watch = Thread(name='Thread-ADC_connection-watchdog', target=self.watchdog, daemon=True)
            self.connection_watch.start()

        adc_watcher = NetManagerSimple(self)
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

            self.response.data = 'ADC connected'.encode()
            if self.response.IsInitialized() and self.parent is not None:
                self.channel0.emit(self.response.SerializeToString())
            self.reset_packets()
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
        with open(os.path.join(self.wdir, 'dev/insys/temp/adc.ini'), 'w') as f:
            config.write(f)
        if self.connected:
            self.scp.put(os.path.join(self.wdir, 'dev/insys/temp/adc.ini'), '/home/embedded/examples/exam_adc.ini')

    def start(self, response: bool = False):
        # separate function for ability start ADC(thread) "manually", not only by command in packet
        self.send_config()
        thrd = Thread(name='Thread-adc-start', target=self.run, args=(response,), daemon=True)
        thrd.start()

    def run(self, response: bool = False):
            self.started = True
            self.isAcqComplete = False
            if os.path.exists(os.path.join(self.wdir, 'dev/insys/temp/'+self.file_base+'.bin')):
                os.remove(os.path.join(self.wdir, 'dev/insys/temp/'+self.file_base+'.bin'))

            #TODO remove using ssh shell. Use exec_command instead

            # if connected run exam_adc
            if self.connected:
                self.ssh.send('/home/embedded/examples/exam_adc\n')
            else:
                self.started = False
                self.isAcqComplete = True

            # wait for exam_adc ending or stopping by user
            while self.started:
                time.sleep(1)

            # if data acquired get adc memory dump file
            if self.isAcqComplete:
                if self.connected:
                    # if connected copy from adc
                    self.scp.get('/home/embedded/examples/data_0.bin', self.wdir)

                    # wait for file transfer completes
                    while not(os.path.exists(os.path.join(self.wdir, 'dev/insys/temp/'+self.file_base+'.bin'))):
                        time.sleep(0.1)
                else:
                    self.generate_data()
                # read dump from disk (generated or loaded from adc)
                dump = np.fromfile(os.path.join(self.wdir, 'dev/insys/temp/'+self.file_base + '.bin'), dtype=np.int16)

            else:
                dump = np.ndarray((0,))

            dump = dump.reshape((-1, self.boards[0].n_active_ch)).T
            # cols = (_ for _ in dump)

            n = 0
            for ch in self.boards[0].channels:
                if ch.on:
                    # ch.data = next(cols)
                    ch.data = dump[n]
                    n += 1
                else:
                    ch.data = np.ndarray((0,))

            self._response(response)

            self.request.address = SystemStatus.SXR
            self.request.sender = self.address
            self.request.command = Commands.STOP
            if self.request.IsInitialized():
                self.channel0.emit(self.request.SerializeToString())

            self.request.address = SystemStatus.SXR
            self.request.sender = self.address
            self.request.command = Commands.SNAPSHOT
            if self.request.IsInitialized():
                self.channel0.emit(self.request.SerializeToString())

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

    def get_status(self, response: bool = False):

        name = self.get_cfg_item('Option', 'AdcServiceName')
        if name is not None:
            self.state.name = name

        self.state.connected = self.connected

        rate = self.get_cfg_item('device0_fm814x250m0', 'SamplingRate')
        if rate is not None:
            self.state.sampling_rate = int(rate)

        samples = self.get_cfg_item('Option', 'MemSamplesPerChan')
        if samples is not None:
            self.state.samples = int(samples)

        start = self.get_cfg_item('device0_fm814x250m0', 'StartSource')
        if start is not None:
            if int(start) == 0:
                self.state.start = AdcStatus.IN0
            elif int(start) == 2:
                self.state.start = AdcStatus.EXTSTART
            elif int(start) == 3:
                self.state.start = AdcStatus.SOFTSTART

        stop = self.get_cfg_item('device0_fm814x250m0', 'StopSource')
        if stop is not None:
            if int(stop) == 0:
                self.status.stop = AdcStatus.SOFTSTART

        stop = self.get_cfg_item('device0_fm814x250m0', 'StopSource')
        if stop is not None:
            if int(stop) == 0:
                self.state.stop = AdcStatus.SOFTSTART

        clock = self.get_cfg_item('device0_fm814x250m0', 'ClockSource')
        if clock is not None:
            if int(clock, 16) == 0:
                self.state.clock_source = AdcStatus.CLOCKOFF
            elif int(clock, 16) == 1:
                self.state.clock_source = AdcStatus.CLOCKINT
            elif int(clock, 16) == 2:
                self.state.clock_source = AdcStatus.CLOCKEXT

        memory = self.get_cfg_item('Option', 'DaqIntoMemory')
        if memory is not None:
            if int(memory) == 0:
                self.state.memory_type = AdcStatus.MEMHOST
            elif int(memory) == 1:
                self.state.memory_type = AdcStatus.MEMINT
            elif int(memory) == 2:
                self.state.memory_type = AdcStatus.MEMFIFO

        ch_mask = self.get_cfg_item('device0_fm814x250m0', 'ChannelMask')
        if ch_mask is not None:
            if len(self.state.board_status) < 1:
                self.state.board_status.add()
            self.state.board_status[0].channel_mask = self.boards[0].channel_mask.to_bytes(1, 'big')

        config = configparser.ConfigParser()
        with open(os.path.join(self.wdir, 'dev/insys/adc_additional.ini'), 'r') as f:
            config.read_file(f)

        for ch_n in range(len(self.boards[0].channels)):
            bias = self.get_cfg_item('device0_fm814x250m0', f'Bias{ch_n}')
            if len(self.state.board_status) < 1:
                self.state.board_status.add()
            if bias is not None:
                if len(self.state.board_status[0].channel_status) < ch_n+1:
                    self.state.board_status[0].channel_status.add()
                self.state.board_status[0].channel_status[ch_n].enabled = self.boards[0].channels[ch_n].on
                self.state.board_status[0].channel_status[ch_n].bias = float(bias)
                self.state.board_status[0].channel_status[ch_n].name = config['adc_additional'][f'name{ch_n+1}']
                self.state.board_status[0].channel_status[ch_n].void = bool((int(config['adc_additional']['void_mask'], 2) >> (len(self.boards[0].channels) - ch_n - 1)) & 1)

        del config

        self._response(response, self.state.SerializeToString())

    def get_cfg_item(self, sec, key):
        if sec in self.config:
            if key in self.config[sec]:
                return self.config[sec][key]
            else:
                return None
        else:
            return None

    def set_settings(self, state: MainPacket = None, response: bool = False):
        super().set_settings(state=state, response=response)
        status = self.state

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
                self.boards[0].channels[n_ch].name = ch_status.name
                self.boards[0].channels[n_ch].void = ch_status.void

        if status.start == status.SOFTSTART:
            self.config['device0_fm814x250m0']['StartSource'] = '3'
            self.config['device0_fm814x250m0']['StartBaseSource'] = '0'
        elif status.start == status.EXTSTART:
            self.config['device0_fm814x250m0']['StartSource'] = '2'
            self.config['device0_fm814x250m0']['StartBaseSource'] = '7'
        elif status.start == status.IN0:
            self.config['device0_fm814x250m0']['StartSource'] = '0'
            self.config['device0_fm814x250m0']['StartBaseSource'] = '7'

        with open(os.path.join(self.wdir, 'dev/insys/adc.ini'), 'w') as f:
            self.config.write(f)

        config = configparser.ConfigParser()
        with open(os.path.join(self.wdir, 'dev/insys/adc_additional.ini'), 'r') as f:
            config.read_file(f)
        void_mask = 0
        if len(status.board_status) > 0:
            n_ch = -1
            for ch_status in status.board_status[0].channel_status:
                n_ch += 1
                if ch_status.void:
                    void_mask += 1
                void_mask = void_mask << 1
                config['adc_additional'][f'name{n_ch+1}'] = ch_status.name
            void_mask = void_mask >> 1
            config['adc_additional']['void_mask'] = str(bin(void_mask))
        with open(os.path.join(self.wdir, 'dev/insys/adc_additional.ini'), 'w') as f:
            config.write(f)

        if response is not None:
            self.get_status(response)

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
        with open(os.path.join(self.wdir, 'dev/insys/temp/'+self.file_base + '.bin'), 'w') as f:
            dump.tofile(f)
        return dump

    def stop(self, response: bool = False):
        if self.started:
            if self.connected:
                self.ssh.send(b'\x1B')
        self.started = False
        self._response(response)
    
    def reboot(self, response: bool = False):
        if self.connected and os.path.exists(os.path.join(work_dir(), 'root_key')):
            client = paramiko.SSHClient()
            client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            client.connect(hostname="192.168.0.242", username="root", key_filename='root_key', password='root')
            ssh = client.invoke_shell()
            ssh.send('reboot\n')
            self._response(response)
        else:
            response = packet_init(0, self.address)
            response.command = Commands.INFO
            response.data = 'ADC disconnected or key file not found'.encode()
            if response.IsInitialized():
                self.channel0.emit(response.SerializeToString())

    def watchdog(self, timeout=2):
        while True:
            if self.connected:
                transp = self.client.get_transport()
                transp.set_keepalive(timeout*2)
                while self.connected:
                    self.connected = transp.is_alive()
                    time.sleep(timeout)

            self.response.data = 'ADC disconnected'.encode()
            if self.response.IsInitialized() and self.parent is not None:
                self.channel0.emit(self.response.SerializeToString())
            self.reset_packets()
            self.make_connection()
            time.sleep(timeout)

    def snapshot(self, request: MainPacket = None, response: bool = False):
        hf, adc = super().snapshot(request, response)

        adc.attrs['name'] = 'InSys FM814X250M'
        adc.attrs['bit'] = 14
        adc.attrs['max_count'] = 2**15
        adc.attrs['min_count'] = -2**15

        if 'config' in adc:
            cfg = adc['config']
        else:
            cfg = adc.create_group('config')

        for sec in self.config:
            if len(self.config[sec].keys()) > 0:
                cfg.create_group(sec)
                for key in self.config[sec]:
                    cfg[sec][key] = self.config[sec][key]
                    cfg.attrs[key] = self.config[sec][key]

        adc.attrs['num_active_ch'] = self.boards[0].n_active_ch
        for ch in self.boards[0].channels:
            if ch.on and not ch.void:
                dset = adc.create_dataset(f'channel{self.boards[0].channels.index(ch):02d}', shape=ch.data.shape,  compression="gzip", compression_opts=1, data=ch.data)
                dset.attrs['units'] = 'adc counts'
                dset.attrs['name'] = ch.name

        filename = hf.filename
        hf.close()

        self._response(response, filename.encode())

        return filename
