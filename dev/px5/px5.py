from socket import socket, AF_INET, SOCK_DGRAM
from core.core import Core
import numpy as np
import os
from core.sxr_protocol_pb2 import MainPacket
from core.sxr_protocol import packet_init
from threading import Thread
import uuid
from PyQt5 import QtNetwork

# devices names as tuple
devs = ('DP5', 'PX5', 'DP5G', 'MCA8000D')

# lambdas to avoid bad indexing
devById = lambda id: devs[id % 4]
idByDev = lambda dev: ([devs.index(_) for _ in (dev,) if _ in devs] + [None,])[0]

# add aliases for lambdas
id2dev = devById
dev2id = idByDev

spec_len2pid = lambda n_ch: (int(n_ch).bit_length()-8)*2-1

def packet(pid1=b'\x00', pid2=b'\x00', data=b''):
    # make packet
    if isinstance(data, str):
        data = data.encode()
    if isinstance(pid1, int):
        pid1 = pid1.to_bytes(1, 'big')
    if isinstance(pid2, int):
        pid2 = pid2.to_bytes(1, 'big')

    pkt = b'\xf5\xfa' + pid1 + pid2 + len(data).to_bytes(2, 'big') + data
    chksum = ~sum(pkt) + 1
    # 3 bytes are necessary so sum can be larger than 2 byte signed int
    return pkt + chksum.to_bytes(3, 'big', signed=True)[-2:]


def check_chksum(pkt):
    chksumIn = pkt[-2:]
    chksum = ~sum(pkt[:-2]) + 1
    chksum = chksum.to_bytes(3, 'big', signed=True)[-2:]
    return chksumIn == chksum


def check_packet(pkt):
    return all((pkt[:2] == b'\xf5\xfa', int().from_bytes(pkt[4:6],'big') == len(pkt)-8, check_chksum(pkt)))


def request_status():
    return packet(b'\x01', b'\x01')


def response_status(pkt, obj=None):
    if not check_packet(pkt):
        return None
    if not pkt[2:4] == b'\x01\x01':
        return None
    data = pack_status(obj)
    return packet(b'\x80', b'\x01', data)


def request_spectrum():
    return packet(b'\x02', b'\x01')


def response_spectrum(pkt, obj=None):
    if not check_packet(pkt):
        return None
    if not pkt[2:4] == b'\x02\x01':
       return None
    data = pack_spectrum(obj)
    pid2 = spec_len2pid(len(data) // 3)
    return packet(b'\x81', pid2, data)


def request_spectrum_clear():
    return packet(b'\x02', b'\x02')


def response_spectrum_clear(pkt, obj=None):
    if not check_packet(pkt):
        return None
    if not pkt[2:4] == b'\x02\x02':
        return None
    data = pack_spectrum(obj)
    pid2 = spec_len2pid(len(data) // 3)
    clear_spectrum(obj)
    return packet(b'\x81', pid2, data)


def request_spectrum_status():
    return packet(b'\x02', b'\x03')


def response_spectrum_status(pkt, obj=None):
    if not check_packet(pkt):
        return None
    if not pkt[2:4] == b'\x02\x03':
        return None
    data = pack_spectrum(obj)
    data += pack_status(obj)
    pid2 = spec_len2pid(len(data) // 3)+1
    clear_spectrum(obj)
    return packet(b'\x81', pid2, data)

def request_spectrum_clear_status(pkt, obj=None):
    return packet(b'\x02', b'\x04')


def add_int(name,  nbytes=1, data=None, obj=None, k=1.0, byteorder='little', signed=False, full=False):
    if data is None:
        data = b''

    if obj is None:
        if full:
            data += np.random.randint(2**(8*nbytes-1)).to_bytes(nbytes, byteorder, signed=signed)
        else:
            data += bytes(nbytes)
    elif hasattr(obj, name):
        if int(k*getattr(obj, name)).bit_length() <= 2**(8*nbytes-1):
            data += int(k*getattr(obj, name)).to_bytes(nbytes, byteorder, signed=signed)
        else:
            data += bytes(nbytes)
    else:
        data += bytes(nbytes)
    return data


def unpack_status(data, obj=None):
    if obj is None:
        status = dict()
    else:
        status = obj.__dict__
    status['FastCount'] = int.from_bytes(data[0:4], 'little')
    status['SlowCount'] = int.from_bytes(data[4:8], 'little')
    status['GPCount'] = int.from_bytes(data[8:12], 'little')
    status['AccTimeMs'] = data[12]
    status['AccTime'] = int.from_bytes(data[13:16], 'little') * 100
    status['RealTime'] = int.from_bytes(data[20:24], 'little')
    status['FirmwareVerMajor'] = (data[24] & int('0b11110000', 2)) >> 4
    status['FirmwareVerMinor'] = data[24] & int('0b00001111', 2)
    status['FPGAVerMinor'] = data[25] & int('0b00001111', 2)
    status['FPGAVerMajor'] = (data[25] & int('0b11110000', 2)) >> 4
    status['SerialNum'] = int.from_bytes(data[26:30], 'little')
    status['HV'] = int.from_bytes(data[30:32], 'big', signed=True) * 0.5
    status['DetectorTemp'] = int.from_bytes(data[32:34], 'big', signed=True) * 0.1
    status['BoardTemp'] = int.from_bytes(data[34].to_bytes(1, 'big'), 'big', signed=True)
    status['PresetRealTimeReached'] = (data[35] & 128) >> 7
    status['FastThresholedLocked'] = (data[35] & 64) >> 6
    status['MCAEnabled'] = (data[35] & 32) >> 5
    status['PresetCountReached'] = (data[35] & 16) >> 4
    status['OscilloscopeReady'] = (data[35] & 4) >> 2
    status['UnitIsConfigured'] = (data[35] & 1)
    status['AutoInputOffsetLocked'] = (data[36] & 128) >> 7
    status['MCSFinished'] = (data[36] & 64) >> 6
    status['FirstAfterReboot'] = (data[36] & 32) >> 5
    status['FPGAClock'] = 80 if (data[36] & 2) >> 1 else 20
    status['FPGAClockAuto'] = (data[36] & 1)
    status['FirmwareBuild'] = data[37] & 15
    status['PC5JumperNormal'] = (data[38] & 128) >> 7
    status['HVPolarity'] = 1 if (data[38] & 64) >> 6 else -1
    status['PreAmpVoltage'] = 8.5 if (data[38] & 32) >> 5 else 5
    status['DeviceID'] = devById(data[39])
    status['TECVoltage'] = int.from_bytes(data[40:42], 'big') / 758.5
    status['HPGeHVPSinstalled'] = data[42]
    if obj is None:
        return status


def pack_status(obj=None):
    data = b''

    data = add_int('FastCount', 4, data, obj, full=True)    # 0-3
    data = add_int('SlowCount', 4, data, obj, full=True)    # 4-7
    data = add_int('GPCount', 4, data, obj, full=True)      # 8-11
    data = add_int('AccTimeMs', 1, data, obj)               # 12
    data = add_int('AccTime', 3, data, obj, k=1/100)        # 13-15
    data = add_int('LiveTime', 4, data, obj)                # 16-19 Formerly ‘Livetime’ - under development
    data = add_int('RealTime', 4, data, obj)                # 20-23
    data = add_int('FirmwareVer', 1, data, obj)             # 24
    data = add_int('FPGAVer', 1, data, obj)                 # 25
    data = add_int('SerialNum', 4, data, obj)               # 26-29
    data = add_int('HV', 2, data, obj, k=2, byteorder='big', signed=True)              # 30-31
    data = add_int('DetectorTemp', 2, data, obj, k=10, byteorder='big', signed=True)   # 32-33
    data = add_int('BoardTemp', 1, data, obj, signed=True)                             # 34 Board temp (1 °C/count,signed)
    data = add_int('StateFlag1', 1, data, obj)              # 35
    data = add_int('StateFlag2', 1, data, obj)              # 36
    data = add_int('FirmwareBuild', 1, data, obj)           # 37
    data = add_int('VoltageFlag', 1, data, obj)             # 38
    data = add_int('DeviceID', 1, data, obj)                # 39
    data = add_int('TECVoltage', 2, data, obj, k=758.5, byteorder='big')    #40-41
    data = add_int('HPGeHVPSinstalled', 1, data, obj)       # 42
    data += bytes(21)
    return data


def unpack_spectrum(data, obj=None):
    # make ndarray from buffer
    data = np.frombuffer(data, dtype='S1')
    # add zeros to get 4 bytes/channel instead 3 bytes/channel
    data = np.insert(data, slice(3, data.size, 3), b'\x00')
    data = np.append(data, bytes(4-data.size % 4))
    # make from bytes array array of integers
    data.dtype = '<i4'
    return data


def clear_spectrum(obj=None):
    if obj is not None:
        if hasattr(obj, 'mca'):
            if hasattr(obj.mca, 'spec'):
                if isinstance(obj.mca.spec, np.ndarray):
                    obj.mca.spec.fill(0)


def pack_spectrum(obj=None):
    data = None
    if obj is not None:
        if isinstance(obj, np.ndarray):
            data = obj.copy()
        elif hasattr(obj, 'mca'):
            if hasattr(obj.mca, 'spec'):
                if isinstance(obj.mca.spec, np.ndarray):
                    data = obj.mca.spec.copy()

    if data is None:
        data = np.histogram(53.19*np.random.randn(int(1e6))+4095, 8192)[0]

    data = data.astype('<i4')
    # represent as sequence of bytes
    data.dtype = 'S1'
    # remove ever 4th byte for representing numbers in 3 bytes/channel
    data = np.delete(data, slice(3, data.size, 4))
    return data.tobytes()


def unpack_eth_settings(pkt):
    if not check_packet(pkt):
        return None
    if not pkt[2:4] == b'\x03\x04':
            return None

    data = pkt[6:70]
    eth_settings = dict()
    eth_settings['DHCP'] = data[0]
    eth_settings['ip'] = data[1:5]
    eth_settings['mask'] = data[5:9]
    eth_settings['gateway'] = data[9:13]
    eth_settings['port'] = int.from_bytes(data[17:19], 'big')
    eth_settings['mac'] = data[19:25]
    return eth_settings


def netfinder_unpack(data):
    if data[0] != 1:
        return
    desc = dict()
    desc['UDPStatus'] = ('Open', 'Shared', 'Binded', 'Locked', 'USB connected')[data[1]]
    desc['RequestID'] = int.from_bytes(data[2:4], 'big')
    desc['Event1Days'] = int.from_bytes(data[4:6], 'big')
    desc['Event1Hours'] = data[6]
    desc['Event1Minutes'] = data[7]
    desc['Event2Days'] = int.from_bytes(data[8:10],'big')
    desc['Event2Hours'] = data[10]
    desc['Event2Minutes'] = data[11]
    desc['Event1Seconds'] = data[12]
    desc['Event2Seconds'] = data[13]
    desc['mac'] = data[14:20]
    desc['ip'] = data[20:24]
    desc['mask'] = data[24:28]
    desc['gateway'] = data[28:32]
    descriptions = data[32:].split(b'\x00')
    desc['DeviceName'] = descriptions[0].decode()
    desc['DeviceDesc'] = descriptions[1].decode()
    desc['Event1Name'] = descriptions[2].decode()
    desc['Event2Name'] = descriptions[3].decode()
    return desc


def netfinder_req():
    return b'\x00\x00' + np.random.randint(2**16-1).to_bytes(2,'big') + b'\xf4\xfa'


def netfinder_response(data, mac=0, ip=0):
    #TODO desc dictionary as argument
    if not all((data[0:2] == b'\x00\x00', data[-2:] == b'\xf4\xfa', len(data) == 6)):
        return None
    req = data
    resp = b'\x01'       # prefix
    resp += b'\x00'      # UDP port status
    resp += req[2:4]     # RequestID
    resp += b'\x00\x00'  # desc['Event1Days'] = int.from_bytes(data[4:6], 'big')
    resp += b'\x00'      # desc['Event1Hours'] = data[6]
    resp += b'\x00'      # desc['Event1Minutes'] = data[7]
    resp += b'\x00\x00'  # desc['Event2Days'] = int.from_bytes(data[8:10],'big')
    resp += b'\x00'      # desc['Event2Hours'] = data[10]
    resp += b'\x00'      # desc['Event2Minutes'] = data[11]
    resp += b'\x00'      # desc['Event1Seconds'] = data[12]
    resp += b'\x00'      # desc['Event2Seconds'] = data[13]
    if isinstance(mac, int):
        mac = mac.to_bytes(6, 'big')
    resp += mac          # ['mac'] = data[14:20]
    if isinstance(ip, int):
        ip = ip.to_bytes(4, 'big')
    elif isinstance(ip, str):
        ip = QtNetwork.QHostAddress(ip).toIPv4Address().to_bytes(4,'big')
    resp += ip           # desc['ip'] = data[20:24]
    resp += b'\xff\xff\xff\x00'  # desc['mask'] = data[24:28]
    resp += b'\x00\x00\x00\x00'  # desc['gateway'] = data[28:32]
    # descriptions = data[32:].split(b'\x00')
    resp += b'PX5 imitator\x00'  # desc['DeviceName'] = descriptions[0].decode()
    resp += b'Sid made\x00'  # desc['DeviceDesc'] = descriptions[1].decode()
    resp += b'Null1\x00'  # desc['Event1Name'] = descriptions[2].decode()
    resp += b'Null2\x00'  # desc['Event2Name'] = descriptions[3].decode()
    return resp


def acsii_cfg_load(fname='', structured=False):
    if len(fname) == 0:
        fname = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'px5_ascii.csv')

    if not os.path.exists(fname):
        return None

    if structured:
        names = np.genfromtxt(fname, dtype=str, delimiter=',', comments=None)[0, :]
        ascii_cfg = np.genfromtxt(fname, dtype=[(name,'<U255') for name in names],
                                  delimiter=',', comments=None,skip_header=1)
    else:
        ascii_cfg = np.genfromtxt(fname, dtype=str, delimiter=',', comments=None, skip_header=1)

    return ascii_cfg


class Packet:

    def __new__(cls, pkt=None, **kwargs):
        obj = super().__new__(cls)
        if pkt is not None:
            if isinstance(pkt, (bytes, bytearray)):
                if not check_packet(pkt):
                    obj = None
        return obj

    def __init__(self, pkt=None, *, pid1=b'\x00', pid2=b'\x00', data=b''):
        if pkt is None:
            self.pkt = packet(pid1, pid2, data)
        else:
            self.pkt = pkt

    @property
    def pid1(self):
        return self.pkt[2].to_bytes(1, 'big')

    @property
    def pid2(self):
            return self.pkt[3].to_bytes(1, 'big')

    @property
    def data(self):
        return self.pkt[6:-2]

    @property
    def len(self):
        return len(self.data).to_bytes(2, 'big')

    @property
    def chk_sum(self):
        return self.pkt[-2:]

    def __call__(self, *args, **kwargs):
        return self.pkt


class Protocol:
    def __init__(self):
        self.requests=dict()
        self.responses = dict()

        self.requests['request_status'] = request_status
        self.responses['response_status'] = response_status

        self.requests['request_spectrum'] = request_spectrum
        self.responses['response_spectrum'] = response_spectrum

        self.requests['request_spectrum_clear'] = request_spectrum_clear
        self.responses['response_spectrum_clear'] = response_spectrum_clear

        self.requests['request_spectrum_status'] = request_spectrum_status
        self.responses['response_spectrum_status'] = response_spectrum_status

    def __call__(self, pkt, obj=None):
        resp = None
        if isinstance(pkt, str):
            if pkt in self.requests:
                resp = self.requests[pkt]()
        elif isinstance(pkt, (bytes, bytearray)):
            for func in self.responses:
                resp = self.responses[func](pkt, obj)
                if resp is not None:
                    break

        return resp


class PX5(Core):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.address = 2

        self.udp_socket = socket(AF_INET, SOCK_DGRAM)
        self.address_ip = ('192.168.0.239', 10001)
        self.ascii_cmd = acsii_cfg_load()

    def channel0_slot(self, data: bytes):
        request = MainPacket()
        request.ParseFromString(data)
        if request.address == self.address:
            response = packet_init(request.sender, self.address)
            response.command = request.command

            if request.command == 3:
                self.init_from_px5(response)
            elif request.command == 4:
                self.get_status(response)

    def init_from_px5(self, response=None):
        cfg_to = self.get_ascii_cfg()
        self.send_acsii_cmd(cfg_to, response)

    def get_status(self, response=None):
        pkt = packet(b'\x01', b'\x01')
        self.udp_socket.sendto(pkt, self.addr)
        ack, _ = self.udp_socket.recvfrom(1024)
        if response is not None:
            response.data = ack
            self.channel0.emit(response)
        else:
            return unpack_status(ack)

    def get_ascii_cfg(self, cfg=None, response=None):
        if cfg is None:
            cfg = '=?;'.join(self.ascii_cmd[np.where(self.ascii_cmd[:, 1])[0], 0]).replace(' ', '') + '=?;'
        pkt = packet(b'\x20', b'\x03', cfg)
        self.udp_socket.sendto(pkt, self.addr)
        ack, _ = self.udp_socket.recvfrom(1024)
        if response is not None:
            response.data = ack
            self.channel0.emit(response)
        else:
            return ack[6:-2]

    def send_acsii_cmd(self, cfg, response=None):
        pkt = packet(b'\x20', b'\x02', cfg)
        self.udp_socket.sendto(pkt, self.addr)
        ack, _ = self.udp_socket.recvfrom(1024)
        if response is not None:
            response.data = ack
            self.channel0.emit(response)
        else:
            return ack


class PX5Imitator:
    def __init__(self):

        self.ip = '127.0.0.2'
        self.port = 10001
        self.mac = 0
        self.netmask = b'\xff\xff\xff\x00'
        self.gateway = '127.0.0.2'

        self.FastCount = 0
        self.SlowCount = 0
        self.GPCount = 0
        self.AccvTime = 0
        self.RealTime = 0
        self.FirmwareVer = 1
        self.FPGAVer = 2
        self.SerialNum = 1234
        self.HV = -130
        self.DetectorTemp = 230
        self.BoardTemp = 23
        self.PresetRealTimeReached = 0
        self.FastThresholedLocked = 1
        self.MCAEnabled = 0
        self.PresetCountReached = 0
        self.OscilloscopeReady = 0
        self.UnitIsConfigured = 0
        self.AutoInputOffsetLocked = 0
        self.MCSFinished = 0
        self.FirstAfterReboot = 1
        self.FPGAClock = 1  # 0 - 20 MHz 1 - 80 Mhz
        self.FPGAClockAuto = 0
        self.FirmwareBuild = 8
        self.PC5JumperNormal = 0
        self.HVPolarity = -1
        self.PreAmpVoltage = 1  # 0 - 5V 1 - 8.5V
        self.DeviceID = 1
        self.TECVoltage = 0.021
        self.HPGeHVPSinstalled = 0

        self.ascii_cfg = acsii_cfg_load()

        self.netfinder_addr = ('0.0.0.0', 3040)
        self.netfinder_thrd = Thread(name='Thread-NetFinder', target=self.netfinder_run, daemon=True)
        self.netfinder_actv = True
        self.netfinder_thrd.start()

        self.actv = True
        self.thrd = Thread(name='Thread-px5imitator', target=self.imitator_run, daemon=True)
        self.thrd.start()

        self.protocol = Protocol()

    def netfinder_run(self):
        netfinder_sock = socket(AF_INET, SOCK_DGRAM)
        netfinder_sock.bind(self.netfinder_addr)
        while self.netfinder_actv:
            req, addr = netfinder_sock.recvfrom(1024)
            resp = netfinder_response(req, ip=self.ip)
            netfinder_sock.sendto(resp, addr)

    def imitator_run(self):
        sock = socket(AF_INET, SOCK_DGRAM)
        sock.bind((self.ip, self.port))
        while self.actv:
            req, addr = sock.recvfrom(1024)
            print(req)
            resp = self.protocol(req, self)
            if resp is not None:
                sock.sendto(resp, addr)

    @property
    def FirmwareVerMajor(self):
        return self.FirmwareVer & int('0b11110000', 2) >> 4

    @property
    def FirmwareVerMinor(self):
        return self.FirmwareVer & int('0b00001111', 2)

    @property
    def AccTime(self):
        return int(self.AccvTime // 100)

    @property
    def AccTimeMs(self):
        return int(self.AccvTime % 100)
