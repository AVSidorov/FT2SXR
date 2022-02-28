import sys
import paramiko
import time
import datetime
from scp import SCPClient
import configparser
import os
import numpy as np
from .core import Core
from .sxr_protocol_pb2 import MainPacket


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
            mask = 0
            if 'device0_fm814x250m0' in config:
                if 'ChannelMask' in config['device0_fm814x250m0']:
                    mask = int(config['device0_fm814x250m0']['ChannelMask'], 16)
            for ch in self.boards[0].channels:
                ch.on = bool(mask & 1)
                mask >>= 1

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
        pck = MainPacket()
        pck.ParseFromString(data)
        if pck.address == self.address:
            if pck.command == 0:
                self.start()




