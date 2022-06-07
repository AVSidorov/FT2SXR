from socket import socket, AF_INET, SOCK_DGRAM
from threading import Thread
import sys
import io

from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus
from core.sxr_protocol import packet_init
from dev.amptek.protocol import Protocol
from dev.amptek.ascii import *
from dev.amptek.netfinder import Packet as Netfinder_packet
from dev.amptek.netfinder import netfinder_response


class PX5(Dev):
    def __init__(self, parent=None, px5_ip='192.168.0.239', mtu=520):
        self.address = SystemStatus.PX5
        super().__init__(parent)

        self.px5_ip = px5_ip
        self.px5_port = 10001
        self.mtu = mtu

        self.udp_socket = socket(AF_INET, SOCK_DGRAM)
        self.protocol = Protocol()

    def channel0_slot(self, data: bytes):
        request = MainPacket()
        request.ParseFromString(data)
        if request.address == self.address:
            response = packet_init(request.sender, self.address)
            response.command = request.command

            if request.command == 1:
                thrd = Thread(name='Thread-px5send', target=self.send_to_px5, args=(request.data, response),
                              daemon=True)
                thrd.start()

    def send_to_px5(self, data, response=None):
        # TODO check that data is amptek packet
        self.udp_socket.sendto(data, (self.px5_ip, self.px5_port))
        ex = False
        while not ex:
            ack, _ = self.udp_socket.recvfrom(self.mtu)
            if self.protocol(ack) is not None:
                ex = True
        if response is None:
            return self.protocol.request


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


