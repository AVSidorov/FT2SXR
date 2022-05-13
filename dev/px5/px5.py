from socket import socket, AF_INET, SOCK_DGRAM
from core.core import Core
import numpy as np
import os
from core.sxr_protocol_pb2 import MainPacket, SystemStatus
from core.sxr_protocol import packet_init
from threading import Thread, Lock
import uuid
from PyQt5 import QtNetwork
import io
import sys
import re

macBytes2str = lambda mac: ':'.join([f'{_:02X}' for _ in mac[:6]])
ipBytes2str = lambda ip: '.'.join([f'{_:d}' for _ in ip[:4]])

# devices names as tuple
devs = ('DP5', 'PX5', 'DP5G', 'MCA8000D', 'TB5', 'DP5-X')
devsCfg = ('D', 'P', 'G', 'M', 'T', 'X')

# lambdas to avoid bad indexing
devById = lambda id: devs[id % len(devs)]
idByDev = lambda dev: ([devs.index(_) for _ in (dev,) if _ in devs] + [None, ])[0]
idByCfg = lambda dev: ([devsCfg.index(_) for _ in (dev,) if _ in devsCfg] + [None, ])[0]

# add aliases for lambdas
id2dev = devById
dev2id = idByDev

spec_len2pidI = lambda len_data: ((int(len_data // 3).bit_length() - 8) * 2 - 1)  # takes number of bytes
spec_len2pidB = lambda len_data: ((int(len_data // 3).bit_length() - 8) * 2 - 1).to_bytes(1, 'big')
spec_pid2len = lambda pid2: 2 ** (8 + ((pid2 - 1) // 2))  # returns number of channels


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
    return all((pkt[:2] == b'\xf5\xfa', int().from_bytes(pkt[4:6], 'big') == len(pkt) - 8, check_chksum(pkt)))


def parse_header(pkt):
    if pkt[:2] == b'\xf5\xfa':
        return int().from_bytes(pkt[4:6], 'big')
    else:
        return None


def request_status():
    return packet(b'\x01', b'\x01')


def response_status(pkt, obj=None):
    pkt = Packet(pkt)
    if pkt is None:
        return None
    if pkt.pid1 == b'\x01' and pkt.pid2 == b'\x01':
        data = pack_status(obj)
        return packet(b'\x80', b'\x01', data)
    elif pkt.pkt[2:4] == b'\x80\x01':
        return unpack_status(pkt.data, obj)
        # return True
    else:
        return None


def request_spectrum():
    return packet(b'\x02', b'\x01')


def response_spectrum(pkt, obj=None):
    pkt = Packet(pkt)
    if pkt is None:
        return None
    if pkt.pkt[2:4] == b'\x02\x01':
        data = pack_spectrum(obj)
        pid2 = spec_len2pidB(len(data))
        return packet(b'\x81', pid2, data)
    elif pkt.pid1 == b'\x81' and \
            pkt.pid2 == spec_len2pidB(len(pkt.pkt) - 8):
        return unpack_spectrum(pkt.data)
        # return True
    else:
        return None


def request_spectrum_clear():
    return packet(b'\x02', b'\x02')


def response_spectrum_clear(pkt, obj=None):
    if not check_packet(pkt):
        return None
    if pkt[2:4] == b'\x02\x02':
        data = pack_spectrum(obj)
        pid2 = spec_len2pidI(len(data))
        clear_spectrum(obj)
        return packet(b'\x81', pid2, data)
    else:
        return None


def request_spectrum_status():
    return packet(b'\x02', b'\x03')


def response_spectrum_status(pkt, obj=None):
    pkt = Packet(pkt)
    if pkt is None:
        return None
    if pkt.pkt[2:4] == b'\x02\x03':
        data = pack_spectrum(obj)
        data += pack_status(obj)
        pid2 = spec_len2pidI(len(data)) + 1
        return packet(b'\x81', pid2, data)
    elif pkt.pid1 == b'\x81' and \
            pkt.pid2 == (spec_len2pidI(len(pkt.pkt) - 8) + 1).to_bytes(1, 'big'):
        ch_num = spec_pid2len(int.from_bytes(pkt.pid2, 'big'))
        return unpack_spectrum(pkt.data[0:ch_num * 3]), unpack_status(pkt.data[ch_num * 3:])
    else:
        return None


def request_spectrum_clear_status():
    return packet(b'\x02', b'\x04')


def response_spectrum_clear_status(pkt, obj=None):
    if not check_packet(pkt):
        return None
    if not pkt[2:4] == b'\x02\x04':
        return None
    data = pack_spectrum(obj)
    data += pack_status(obj)
    pid2 = spec_len2pidI(len(data)) + 1
    clear_spectrum(obj)
    return packet(b'\x81', pid2, data)


def request_txt_cfg(ascii_req):
    return packet(b'\x20', b'\x02', ascii_req)


def request_txt_cfg_readback(ascii_req):
    return packet(b'\x20', b'\x03', ascii_req)


def response_txt_cfg(pkt, obj=None):
    if not check_packet(pkt):
        return None

    pkt = Packet(pkt)

    if pkt.pid1 == b'\x20' and pkt.pid2 == b'\x02':
        return request_ok()
    else:
        return None


def response_txt_cfg_readback(pkt, obj=None):
    if not check_packet(pkt):
        return None

    pkt = Packet(pkt)

    if pkt.pid1 == b'\x20' and pkt.pid2 == b'\x03':
        req = pkt.data.decode()
        # data = pack_txt_cfg(req, obj)
        data = b''
        if isinstance(data, (tuple, list, np.ndarray)):
            return [packet(b'\x82', b'\x07', _) for _ in data]
        else:
            return packet(b'\x82', b'\x07', data)
    elif pkt.pid1 == b'\x82' and pkt.pid2 == b'\x07':
        return True
    else:
        return None


def request_enable_mca():
    packet(b'\xf0', b'\x02')


def response_enable_mca(pkt, obj):
    pkt = Packet(pkt)
    if pkt is None:
        return None
    if pkt.pid1 == b'\xf0' and pkt.pid2 == b'\x02':
        return request_ok()
    return None


def request_disable_mca():
    packet(b'\xf0', b'\x03')


def response_disable_mca(pkt, obj):
    pkt = Packet(pkt)
    if pkt is None:
        return None
    if pkt.pid1 == b'\xf0' and pkt.pid2 == b'\x03':
        return request_ok()
    return None


def request_ok():
    return packet(b'\xff', b'\x00')


def response_ok(pkt, obj=None):
    pkt = Packet(pkt)
    if pkt is None:
        return None
    if pkt.pid1 == b'\xff' and pkt.pid2 == b'\x00':
        return True
    return None


def response_sync_error(pkt, obj=None):
    pkt = Packet(pkt)
    if pkt is None:
        return None
    if pkt.pid1 == b'\xff' and pkt.pid2 == b'\x01':
        return True
    return None


def add_int(name, nbytes=1, data=None, obj=None, k=1.0, byteorder='little', signed=False, full=False):
    if data is None:
        data = b''

    if obj is None:
        if full:
            data += np.random.randint(2 ** (8 * nbytes - 1)).to_bytes(nbytes, byteorder, signed=signed)
        else:
            data += bytes(nbytes)
    elif hasattr(obj, name):
        if int(k * getattr(obj, name)).bit_length() <= 8 * nbytes - 1:
            data += int(k * getattr(obj, name)).to_bytes(nbytes, byteorder, signed=signed)
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
    status['FastCount'] = int.from_bytes(data[0:4], 'little')  # 0-3
    status['SlowCount'] = int.from_bytes(data[4:8], 'little')  # 4-7
    status['GPCount'] = int.from_bytes(data[8:12], 'little')  # 8-11
    status['AccTimeMs'] = data[12]  # 12
    status['AccTime'] = int.from_bytes(data[13:16], 'little') * 100  # 13-15
    status['RealTime'] = int.from_bytes(data[20:24], 'little')  # 20-24
    status['FirmwareVerMajor'] = (data[24] & int('0b11110000', 2)) >> 4  # 24 D7-D4
    status['FirmwareVerMinor'] = data[24] & int('0b00001111', 2)  # 24 D3-D0
    status['FPGAVerMinor'] = data[25] & int('0b00001111', 2)
    status['FPGAVerMajor'] = (data[25] & int('0b11110000', 2)) >> 4
    status['SerialNum'] = int.from_bytes(data[26:30], 'little')
    status['HV'] = int.from_bytes(data[30:31], 'big', signed=True) * 0.5
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

    data = add_int('FastCount', 4, data, obj, full=True)  # 0-3
    data = add_int('SlowCount', 4, data, obj, full=True)  # 4-7
    data = add_int('GPCount', 4, data, obj, full=True)  # 8-11
    data = add_int('AccTimeMs', 1, data, obj)  # 12
    data = add_int('AccTime', 3, data, obj, k=1 / 100)  # 13-15
    data = add_int('LiveTime', 4, data, obj)  # 16-19 Formerly ‘Livetime’ - under development
    data = add_int('RealTime', 4, data, obj)  # 20-23
    data = add_int('FirmwareVer', 1, data, obj)  # 24
    data = add_int('FPGAVer', 1, data, obj)  # 25
    data = add_int('SerialNum', 4, data, obj)  # 26-29
    data = add_int('HV', 2, data, obj, k=2, byteorder='big', signed=True)  # 30-31
    data = add_int('DetectorTemp', 2, data, obj, k=10, byteorder='big', signed=True)  # 32-33
    data = add_int('BoardTemp', 1, data, obj, signed=True)  # 34 Board temp (1 °C/count,signed)
    data = add_int('StateFlag1', 1, data, obj)  # 35
    data = add_int('StateFlag2', 1, data, obj)  # 36
    data = add_int('FirmwareBuild', 1, data, obj)  # 37
    data = add_int('VoltageFlag', 1, data, obj)  # 38
    data = add_int('DeviceID', 1, data, obj)  # 39
    data = add_int('TECVoltage', 2, data, obj, k=758.5, byteorder='big')  # 40-41
    data = add_int('HPGeHVPSinstalled', 1, data, obj)  # 42
    data += bytes(21)
    return data


def unpack_spectrum(data, obj=None):
    # make ndarray from buffer
    data = np.frombuffer(data, dtype='S1')
    # add zeros to get 4 bytes/channel instead 3 bytes/channel
    data = np.insert(data, slice(3, data.size, 3), b'\x00')
    data = np.append(data, bytes(4 - data.size % 4))
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
        data = np.histogram(53.19 * np.random.randn(int(1e6)) + 4095, 8192)[0]

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


nf_udp_status = ('Open', 'Shared', 'Binded', 'Locked', 'USB connected')
nf_id2status = lambda sID: nf_udp_status[sID % len(nf_udp_status)]
nf_status2id = lambda status: ([nf_udp_status.index(_) for _ in (status,) if _ in status] + [None, ])[0]


def netfinder_unpack(data):
    if data[0] != 1:
        return
    desc = dict()
    desc['UDPStatus'] = nf_id2status(data[1])
    desc['RequestID'] = int.from_bytes(data[2:4], 'big')
    desc['Event1Days'] = int.from_bytes(data[4:6], 'big')
    desc['Event1Hours'] = data[6]
    desc['Event1Minutes'] = data[7]
    desc['Event2Days'] = int.from_bytes(data[8:10], 'big')
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
    return b'\x00\x00' + np.random.randint(2 ** 16 - 1).to_bytes(2, 'big') + b'\xf4\xfa'


def netfinder_response(data, mac=0, ip=0):
    # TODO desc dictionary as argument
    if not all((data[0:2] == b'\x00\x00', data[-2:] == b'\xf4\xfa', len(data) == 6)):
        return None
    req = data
    resp = b'\x01'  # prefix
    resp += b'\x00'  # UDP port status
    resp += req[2:4]  # RequestID
    resp += b'\x00\x00'  # desc['Event1Days'] = int.from_bytes(data[4:6], 'big')
    resp += b'\x00'  # desc['Event1Hours'] = data[6]
    resp += b'\x00'  # desc['Event1Minutes'] = data[7]
    resp += b'\x00\x00'  # desc['Event2Days'] = int.from_bytes(data[8:10],'big')
    resp += b'\x00'  # desc['Event2Hours'] = data[10]
    resp += b'\x00'  # desc['Event2Minutes'] = data[11]
    resp += b'\x00'  # desc['Event1Seconds'] = data[12]
    resp += b'\x00'  # desc['Event2Seconds'] = data[13]
    if isinstance(mac, int):
        mac = mac.to_bytes(6, 'big')
    resp += mac  # ['mac'] = data[14:20]
    if isinstance(ip, int):
        ip = ip.to_bytes(4, 'big')
    elif isinstance(ip, str):
        ip = QtNetwork.QHostAddress(ip).toIPv4Address().to_bytes(4, 'big')
    resp += ip  # desc['ip'] = data[20:24]
    resp += b'\xff\xff\xff\x00'  # desc['mask'] = data[24:28]
    resp += b'\x00\x00\x00\x00'  # desc['gateway'] = data[28:32]
    # descriptions = data[32:].split(b'\x00')
    resp += b'PX5 imitator\x00'  # desc['DeviceName'] = descriptions[0].decode()
    resp += b'Sid made\x00'  # desc['DeviceDesc'] = descriptions[1].decode()
    resp += b'Null1\x00'  # desc['Event1Name'] = descriptions[2].decode()
    resp += b'Null2\x00'  # desc['Event2Name'] = descriptions[3].decode()
    return resp


class Netfinder_packet:
    def __init__(self, pkt=None, **kwargs):
        if pkt is not None:
            self._pkt = bytearray(pkt)
        else:
            self._pkt = bytearray(36)
            self._pkt[0] = b'\x01'

    @property
    def prefix(self):
        return self._pkt[0]

    @prefix.setter
    def prefix(self):
        self._pkt[0] = b'\x01'

    @property
    def UDPStatus(self):
        return nf_id2status(self._pkt[1])

    @UDPStatus.setter
    def UDPStatus(self, val):
        if isinstance(val, int):
            self._pkt[1] = val.to_bytes(1, 'big')
        elif isinstance(val, (bytes, bytearray)):
            self._pkt[1] = val[-1]
        elif isinstance(val, str):
            if nf_status2id is not None:
                self._pkt[1] = nf_status2id.to_bytes(1, 'big')

    @property
    def RequestID(self):
        return int.from_bytes(self._pkt[2:4], 'big')

    @RequestID.setter
    def RequestID(self, val):
        pass

    @property
    def Event1Days(self):
        return int.from_bytes(self._pkt[4:6], 'big')

    @Event1Days.setter
    def Event1Days(self, val):
        pass

    @property
    def Event1Hours(self):
        return self._pkt[6]

    @Event1Hours.setter
    def Event1Hours(self, val):
        pass

    @property
    def Event1Minutes(self):
        return self._pkt[7]

    @Event1Minutes.setter
    def Event1Minutes(self, val):
        pass

    @property
    def Event2Days(self):
        return int.from_bytes(self._pkt[8:10], 'big')

    @Event2Days.setter
    def Event1Days(self, val):
        pass

    @property
    def Event2Hours(self):
        return self._pkt[10]

    @Event2Hours.setter
    def Event2Hours(self, val):
        pass

    @property
    def Event2Minutes(self):
        return self._pkt[11]

    @Event2Minutes.setter
    def Event2Minutes(self, val):
        pass

    @property
    def Event1Seconds(self):
        return self._pkt[12]

    @Event1Seconds.setter
    def Event1Seconds(self, val):
        pass

    @property
    def Event2Seconds(self):
        return self._pkt[13]

    @Event2Seconds.setter
    def Event2Seconds(self, val):
        pass

    @property
    def mac(self):
        return macBytes2str(self._pkt[14:20])

    @mac.setter
    def mac(self, val):
        pass

    @property
    def ip(self):
        return ipBytes2str(self._pkt[20:24])

    @ip.setter
    def ip(self, value):
        if isinstance(value, str):
            self._pkt[20:24] = QtNetwork.QHostAddress(value).toIPv4Address().to_bytes(4, 'big')
        elif isinstance(value, int):
            self._pkt[20:24] = value.to_bytes(4, 'big')
        elif isinstance(value, (bytes, bytearray)):
            self._pkt[20:24] = value

    @property
    def mask(self):
        return '.'.join([f'{_:d}' for _ in self._pkt[24:28]])

    @mask.setter
    def mask(self, value):
        pass

    @property
    def gateway(self):
        return '.'.join([f'{_:d}' for _ in self._pkt[28:32]])

    @mask.setter
    def gateway(self, value):
        pass

    @property
    def descriptions(self):
        desc_list = self._pkt[32:].split(b'\x00')
        desc = dict()
        desc['DeviceName'] = desc_list[0].decode()
        desc['DeviceDesc'] = desc_list[1].decode()
        desc['Event1Name'] = desc_list[2].decode()
        desc['Event2Name'] = desc_list[3].decode()
        return desc

    @descriptions.setter
    def descriptions(self, value):
        pass

    def __len__(self):
        return len(self._pkt)

    def __call__(self, typeOut=bytes):
        if typeOut == bytes:
            return bytes(self._pkt)
        elif typeOut == bytearray:
            return bytearray(self._pkt)
        elif typeOut == dict:
            return netfinder_unpack(self._pkt)


def ascii_cfg_load(fname: str = '', structured=False):
    if len(fname) == 0:
        fname = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'px5_ascii.csv')

    if not os.path.exists(fname):
        return None

    if structured:
        names = np.genfromtxt(fname, dtype=str, delimiter=',', comments=None)[0, :]
        ascii_cfg = np.genfromtxt(fname, dtype=[(name, '<U255') for name in names],
                                  delimiter=',', comments=None, skip_header=1)
    else:
        ascii_cfg = np.genfromtxt(fname, dtype=str, delimiter=',', comments=None, skip_header=1)

    return ascii_cfg


def ascii_req_full(cfg) -> str:
    return '=?;'.join(cfg[np.where(cfg[:, 1])[0], 0]).replace(' ', '') + '=?;'


def ascii_resp(req: str, cfg) -> str:
    return ';'.join([f'{cmd}={cfg[cfg[:, 0] == cmd, 1][0]}' \
                                        for cmd in req.replace('=', '').replace('?', '').rstrip(';').split(';')]) + ';'


def ascii_rm_fields(req: str, fields) -> str:
    return ';'.join([_ for _ in req.rstrip(';').split(';') if not _.split('=')[0] in fields]) + ';'


def pack_txt_cfg(req, obj=None):
    if obj is None:
        cfg = PX5Configuration()()
    if hasattr(obj, 'ascii_cfg'):
        cfg = obj.ascii_cfg
    if cfg is None:
        return None

    return ascii_resp(req, cfg)


def ver2str(ver: int) -> str:
    return '.'.join([f'{_:02d}' for _ in [(ver >> 8), (ver & 0b11110000) >> 4, ver & 0b1111]])


def str2ver(version: str = '', limits=False, prefix='FW'):
    ver_min = 0
    ver_max = 0xfff
    match = re.search(r"(?:^|"+prefix+r"|\s)(?:\d{1,2}\.?){2,3}", version)
    if match is not None:
        ver = sum([int(_) << 4 * (2 - ind) for ind, _ in enumerate(match[0].replace(prefix, '').split('.'))])
        ver_min = ver
        if version.find('prior') >= 0:
            ver_max = ver - 1
            ver_min = 0
    if limits:
        return ver_min, ver_max
    else:
        return ver


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
        self.buf = b''
        self.pkt_length = 0
        self.request = b''

        self.requests = dict()
        self.responses = dict()

        self.requests['request_status'] = request_status
        self.responses['response_status'] = response_status

        self.requests['request_spectrum'] = request_spectrum
        self.responses['response_spectrum'] = response_spectrum

        self.requests['request_spectrum_clear'] = request_spectrum_clear
        self.responses['response_spectrum_clear'] = response_spectrum_clear

        self.requests['request_spectrum_status'] = request_spectrum_status
        self.responses['response_spectrum_status'] = response_spectrum_status

        self.requests['request_spectrum_clear_status'] = request_spectrum_clear_status
        self.responses['response_spectrum_clear_status'] = response_spectrum_clear_status

        self.responses['response_txt_cfg'] = response_txt_cfg

        self.responses['response_txt_cfg_readback'] = response_txt_cfg_readback

        self.responses['response_ok'] = response_ok

        self.responses['response_enable_mca'] = response_enable_mca

        self.responses['response_disable_mca'] = response_disable_mca

    def __call__(self, pkt, obj=None):
        resp = None
        if isinstance(pkt, str):
            if pkt in self.requests:
                resp = self.requests[pkt]()
        elif isinstance(pkt, (bytes, bytearray)):
            pktL = parse_header(pkt)

            # if header is determined clear buffer and store full length
            if pktL is not None:
                self.pkt_length = pktL + 8
                self.buf = b''
            elif self.pkt_length == 0:  # given data with not recognized header and protocol doesn't collect data
                resp = None

            # store in buffer only if full length determined
            if self.pkt_length > 0:
                self.buf += pkt

            # if full packet acquired
            if len(self.buf) == self.pkt_length:
                self.request = self.buf
                recognized = False
                for func in self.responses:
                    resp = self.responses[func](self.buf, obj)
                    if isinstance(resp, (bytes, bytearray)):
                        print(f'[prot] {func} is made')
                        recognized = True
                        break
                    elif resp:
                        print(f'[prot] {func} received')
                        recognized = True
                        break

                if not recognized:
                    print("Don't recognized packet")
                    resp = False

                self.buf = b''
                self.pkt_length = 0

        return resp


class PX5(Core):
    def __init__(self, parent=None, px5_ip='192.168.0.239', mtu=520):
        super().__init__(parent)
        self.address = SystemStatus.PX5

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

            if request.command == 0:
                self.init_from_px5(response)
            elif request.command == 1:
                thrd = Thread(name='Thread-px5send', target=self.send_to_px5, args=(request.data, response),
                              daemon=True)
                thrd.start()

    def init_from_px5(self, response=None):
        cfg = ascii_req_full()
        req = ascii_req_full(cfg)
        req = ascii_rm_fields(req, 'GAIN')
        resp = b''
        while len(req) > 0:
            pos = req[:self.mtu - 8].rfind(';')
            pkt = self.send_to_px5(request_txt_cfg_readback(req[:pos + 1]))
            pkt = Packet(pkt)
            resp += pkt.data
            req = req[pos + 1:]

        resp = b'RESC=YES;' + resp
        while len(resp) > 0:
            pos = resp[:self.mtu - 8].rfind(b';')
            self.send_to_px5(request_txt_cfg(resp[:pos + 1]))
            resp = resp[pos + 1:]

    def send_to_px5(self, data, response=None):
        # TODO check that data is px5 packet
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

            # if not from px5 save client and send to px5
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


class PX5Configuration:
    def __new__(cls, cfg=None, obj: PX5Imitator = None, fw=None):
        cfg_obj = super().__new__(cls)

        if cfg is None:
            cfg = ascii_cfg_load()
        elif isinstance(cfg, str):
            cfg = ascii_cfg_load(fname=cfg)
        elif isinstance(cfg, np.ndarray):
            cfg = cfg

        if obj is not None:
            dev = obj.DeviceID
        else:
            dev = idByDev('PX5')

        if obj is not None:
            fw = obj.fw
        elif fw is not None:
            if isinstance(fw, str):
                fw = str2ver(fw)
            elif isinstance(fw, int) and fw< 4095:
                fw = fw
        else:
            fw = 0

        cfg_new = np.ndarray((0, cfg.shape[1]))
        for row in cfg:
            ver_min, ver_max = str2ver(row[-3], limits=True)
            if all((any((len(row[-2]) == 0, devsCfg[dev] in row[-2].split(';'))),
                     all((ver_min <= fw, fw <= ver_max)))):
                cfg_new = np.vstack((cfg_new, row))

        if cfg_new.size > 0:
            cfg_obj.cfg = cfg_new
            return cfg_obj
        else:
            return None


    def __call__(self):
        return self.cfg

