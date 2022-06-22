from socket import socket, AF_INET, SOCK_DGRAM
from socket import error as sockerr
from socket import timeout as socktimeout
from threading import Thread
import sys
import io

import h5py

from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands
from core.sxr_protocol import packet_init
from dev.amptek.protocol import Protocol, Packet, unpack_status, request_txt_cfg_readback
from dev.amptek.ascii import *
from dev.amptek.netfinder import Packet as Netfinder_packet
from dev.amptek.netfinder import netfinder_response


class PX5(Dev):
    def __init__(self, parent=None, px5_ip='192.168.0.239', mtu=520):
        super().__init__(parent, SystemStatus.PX5)

        self.px5_ip = px5_ip
        self.px5_port = 10001
        self.mtu = mtu

        self.udp_socket = socket(AF_INET, SOCK_DGRAM)
        self.udp_socket.settimeout(1)
        self.protocol = Protocol()
        self.cfg = PX5Configuration()

    def run_send_thread(self, request: MainPacket = None, response: MainPacket = None):
        thrd = Thread(name='Thread-px5send', target=self.send_to_px5, args=(request.data, response),
                      daemon=True)
        thrd.start()

    def send_to_px5(self, data, response: bool = False):
        # TODO check that data is amptek packet
        self.udp_socket.sendto(data, (self.px5_ip, self.px5_port))
        ex = False
        while not ex:
            try:
                ack, _ = self.udp_socket.recvfrom(self.mtu)
                if self.protocol(ack) is not None:
                    res = Packet(self.protocol.request).data
                    ex = True
            except sockerr as err:
                ex = True
                if isinstance(err, socktimeout):
                    res = 'Socket timeout'

        return self._response(response, res)

    def start(self, response: bool = False):
        return self.send_to_px5(self.protocol('request_enable_mca'), response)

    def stop(self, response: bool = False):
        return self.send_to_px5(self.protocol('request_disable_mca'), response)

    def get_status(self, response: bool = False):
        status = self.send_to_px5(self.protocol('request_status'), response)
        if isinstance(status, (bytes, bytearray)):
            return unpack_status(status)
        else:
            return status

    def get_settings(self, response: bool = False):
        #TODO split full request in two parts to avoid Serial Num reset and bad fields in response
        req = self.cfg.full_req()
        if response:
            if len(self.request.data) > 0:
                req = self.request.data
        return self.send_to_px5(request_txt_cfg_readback(req))

    def snapshot(self, request: MainPacket = None, response: bool = False):
        hf, px5_group = super().snapshot(request, response)

        data = self.send_to_px5(self.protocol('request_spectrum_status'))

        if isinstance(data, (bytes, bytearray)): # if socket error str will be returned
            spectrum, status = self.protocol.responses['response_spectrum_status'](self.protocol.request)

            if 'status' not in px5_group:
                group_status = px5_group.create_group('status', track_order=True)
            elif isinstance(px5_group['status'], h5py.Group):
                group_status = px5_group['status']

            for key in ('DeviceID', 'SerialNum'):
                px5_group.attrs[key] = status[key]
            for field in status:
                group_status.create_dataset(field, data=status[field], track_order=True)
                group_status.attrs[field] = status[field]

            if 'spectrum' not in px5_group:
                px5_group.create_dataset('spectrum', data=spectrum)
            else:
                px5_group['spectrum'] = spectrum

        req = self.cfg.full_req()
        ex = False

        count_tries = 0
        group_cfg = px5_group.create_group('config_ascii', track_order=True)
        while all((not ex, count_tries < 4)):
            count_tries += 1
            ascii_cfg = self.send_to_px5(request_txt_cfg_readback(req))
            if isinstance(ascii_cfg, (bytes, bytearray)):
                ascii_cfg = ascii_req_split(ascii_cfg.decode())
                # remove bad fields
                ascii_cfg = np.array([(row[0], row[1]) for row in ascii_cfg if self.cfg.check_option_value(row[0], row[1])])
                for field in ascii_cfg:
                    if field[0] not in group_cfg:
                        group_cfg.create_dataset(field[0], shape=(1,), data=str(field[1]))
                        group_cfg.attrs[field[0]] = str(field[1])

                if ascii_req_split(req).shape[0] > ascii_cfg.shape[0]:
                    req = ''
                    for field in ascii_req_split(self.cfg.full_req()):
                        if field[0] not in ascii_cfg[:, 0]:
                            req += f'{field[0]}=?;'
                else:
                    ex = True

        filename = hf.filename
        hf.close()

        self._response(response, filename.encode())

        return filename


class PX5Imitator:
    def __init__(self):

        self.ip = '127.0.0.2'
        self.port = 10001
        self.mac = 0
        self.netmask = b'\xff\xff\xff\x00'
        self.gateway = '127.0.0.2'
        self.mtu = 520

        self.FastCount = 0
        self.SlowCount = 0
        self.GPCount = 0
        self.AccvTime = 0
        self.RealTime = 0
        self.FirmwareVer = str2ver("6.9.7") >> 4
        self.FPGAVer = str2ver("7.1") >> 4
        self.SerialNum = 1234
        self.HV = -1.0
        self.DetectorTemp = 230
        self.BoardTemp = 23
        self.PresetRealTimeReached = 0
        self.FastThresholedLocked = 1
        self.MCAEnabled = 1
        self.PresetCountReached = 0
        self.OscilloscopeReady = 0
        self.UnitIsConfigured = 0
        self.AutoInputOffsetLocked = 0
        self.MCSFinished = 0
        self.FirstAfterReboot = 1
        self.FPGAClock = 1  # 0 - 20 MHz 1 - 80 Mhz
        self.FPGAClockAuto = 0
        self.FirmwareBuild = str2ver("6.9.7") & 0b1111
        self.PC5JumperNormal = 0
        self.HVPolarity = -1
        self.PreAmpVoltage = 1  # 0 - 5V 1 - 8.5V
        self.DeviceID = 1
        self.TECVoltage = 3.4765985497692813
        self.HPGeHVPSinstalled = 0

        self.ascii_cfg = ascii_cfg_load()

        self.netfinder_thrd = Thread(name='Thread-NetFinder', target=self.netfinder_run, daemon=True)
        self.netfinder_actv = True
        self.netfinder_thrd.start()

        self.actv = True
        self.thrd = Thread(name='Thread-px5imitator', target=self.imitator_run, daemon=True)
        self.thrd.start()

        self.protocol = Protocol()

    def netfinder_run(self):
        netfinder_sock = socket(AF_INET, SOCK_DGRAM)
        netfinder_sock.bind(('0.0.0.0', 3040))
        while self.netfinder_actv:
            req, addr = netfinder_sock.recvfrom(1024)
            resp = netfinder_response(req, ip=self.ip)
            netfinder_sock.sendto(resp, addr)

    def imitator_run(self):
        sock = socket(AF_INET, SOCK_DGRAM)
        sock.bind(('0.0.0.0', self.port))
        while self.actv:
            req, addr = sock.recvfrom(self.mtu)
            resp = self.protocol(req, self)
            if resp is not None:
                for i in range(len(resp) // self.mtu + 1):
                    sock.sendto(resp[i * self.mtu:(i + 1) * self.mtu], addr)

    @property
    def FirmwareVerMajor(self):
        return (self.FirmwareVer & 0b11110000) >> 4

    @property
    def FirmwareVerMinor(self):
        return self.FirmwareVer & 0b1111

    @property
    def FPGAVerMajor(self):
        return (self.FPGAVer & 0b11110000) >> 4

    @property
    def FPGAVerMinor(self):
        return self.FPGAVer & 0b1111

    @property
    def fw(self):
        return (self.FirmwareVer << 4) +self.FirmwareBuild

    @property
    def AccTime(self):
        return int(self.AccvTime // 100)

    @property
    def AccTimeMs(self):
        return int(self.AccvTime % 100)


class Retranslator:
    def __init__(self, ip_px5='192.168.0.239', ip_this='127.0.0.2', log=sys.stdout, dump=None):
        self.ip = ip_this
        self.ip_px5 = ip_px5
        self.actv = True
        self.protocol_to = Protocol()
        self.protocol_from = Protocol()

        if log is not None:
            if isinstance(log, str):
                self.logger = open(dump, 'at')
            elif isinstance(log, io.TextIOBase):
                self.logger = log
        else:
            self.logger = None

        if dump is not None:
            if isinstance(dump, str):
                self.dump = open(dump, 'ab')
            elif isinstance(log, io.TextIOBase):
                self.dump = dump
        else:
            self.dump = None

        thrd = Thread(name='Thread-NetFinder', target=self.sock_wrap, args=(3040,), daemon=True)
        thrd.start()

        thrd = Thread(name='Thread-px5Main', target=self.sock_wrap, args=(10001,), daemon=True)
        thrd.start()

    def sock_wrap(self, port=3040):
        # create socket on choosen port
        sock = socket(AF_INET, SOCK_DGRAM)
        sock.bind(('0.0.0.0', port))

        client = ('127.0.0.1', port + 1)
        while self.actv:
            data, addr = sock.recvfrom(1024)

            # if not from amptek save client and send to amptek
            if addr[0] != self.ip_px5:
                client = addr
                req = data

                if self.protocol_to(req):
                    self.logging(self.protocol_to.request, addr)

                sock.sendto(req, (self.ip_px5, port))
            else:
                # in case netfinder change ip in packet
                if port == 3040:
                    nf_resp = Netfinder_packet(data)
                    nf_resp.ip = self.ip
                    resp = nf_resp()
                    print(resp)
                else:
                    resp = data

                if self.protocol_from(resp):
                    self.logging(self.protocol_from.request, addr)

                sock.sendto(resp, client)

    def stop(self):
        self.actv = False
        if self.logger is not None:
            if self.logger.name != '<stdout>':
                self.logger.close()
        if self.dump is not None:
            self.dump.close()

    def logging(self, data=b'', addr=('', 0)):
        if self.dump is not None:
            self.dump.write(data)
        if self.logger is not None:
            self.logger.write(f'GET {len(data)} bytes from {addr[0]}:{addr[1]}')


