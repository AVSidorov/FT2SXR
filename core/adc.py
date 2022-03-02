import sys
import paramiko
import time
import datetime
from scp import SCPClient
import configparser
import os
import numpy as np
from .core import Core
from .sxr_protocol_pb2 import MainPacket, AdcStatus
from .sxr_protocol import packet_init


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

    def set_active(self, index=0, value: bool = True):
        if all((index >= 0, index < len(self.channels))):
            self.channels[index] = value


class ADC(Core):
    def __init__(self, parent=None, nboards=1):
        super().__init__(parent)
        self.address = 1

        self.boards = [Board() for _ in range(nboards)]

        self.connected = False
        self.ssh = None
        self.ssh_timeout = 0.5
        self.dump = 'data_0.bin'
        self.wdir = os.path.abspath('./')
        self.scp = None
        self.config = None

        config = configparser.ConfigParser(inline_comment_prefixes=(';', '//'))
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
        td = datetime.date.today()
        wdir = format(td.year-2000, '02d')+format(td.month,'02d')+format(td.day, '02d')
        wdir = os.path.join(self.wdir, wdir)

        if not(os.path.exists(wdir)):
            os.mkdir(wdir)

        self.wdir = wdir

        client = paramiko.SSHClient()
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
        except:
            self.connected = False

    def ssh_output(self, timeout=None):
        if timeout is None:
            timeout = self.ssh_timeout
        time.sleep(timeout)

        output = ""
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

    def start(self):
        if self.connected:
            self.ssh.send('/home/embedded/examples/exam_adc\n')
            self.ssh_output(5)

            self.scp.get('/home/embedded/examples/data_0.bin', self.wdir)
            while not(os.path.exists(os.path.join(self.wdir, self.file_base+'.bin'))):
                pass
            dump = np.fromfile(os.path.join(self.wdir, self.file_base + '.bin'), dtype=np.int16)
        else:
            dump = np.round(np.random.normal(0, 1, int(1e5))*10)

            dump = dump.reshape((-1, self.boards[0].n_active_ch)).T
            cols = (_ for _ in dump)

        for ch in self.boards[0].channels:
            if ch.on:
                ch.data = next(cols)
            else:
                ch.data = np.ndarray((0,))

    def channel0_slot(self, data: bytes):
        request = MainPacket()
        request.ParseFromString(data)
        if request.address == self.address:
            response = packet_init(request.sender, self.address)
            response.command = request.command

            if request.command == 0:
                self.status_message(response)
            elif request.command == 1:
                self.status_to_config(request.data)
                self.status_message(response)

    def status_message(self, response=None):
        status = AdcStatus()

        name = self.get_cfg_item('Option', 'AdcServiceName')
        if name is not None:
            status.name = name

        status.connected = self.connected

        rate = self.get_cfg_item('device0_fm814x250m0', 'SamplingRate')
        if rate is not None:
            status.sampling_rate = int(rate)

        samples = self.get_cfg_item('Option', 'SamplesPerChannel')
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

    def status_to_config(self, status):
        if isinstance(status, bytes):
            data = status
            status = AdcStatus()
            status.ParseFromString(data)

        self.config['Option']['DaqIntoMemory'] = str(status.memory_type)
        self.config['Option']['SamplesPerChannel'] = str(status.samples)
        self.config['Option']['MemSamplesPerChan'] = str(status.samples)

        if len(status.board_status) > 0:
            if len(status.board_status[0].channel_mask) > 0:
                self.config['device0_fm814x250m0']['ChannelMask'] = f'0x{int().from_bytes(status.board_status[0].channel_mask, "big"):02X}'
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
        elif status.start == status.EXTSTART:
            self.config['device0_fm814x250m0']['StartSource'] = '2'
        elif status.start == status.IN0:
            self.config['device0_fm814x250m0']['StartSource'] = '0'

