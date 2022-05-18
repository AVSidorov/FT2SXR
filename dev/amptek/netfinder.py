import numpy as np
from PyQt5 import QtNetwork


def macBytes2str (mac: bytes) -> str:
    return ':'.join([f'{_:02X}' for _ in mac[:6]])


def ipBytes2str (ip: bytes) -> str:
    return '.'.join([f'{_:d}' for _ in ip[:4]])


nf_udp_status = ('Open', 'Shared', 'Binded', 'Locked', 'USB connected')


def nf_id2status(sID):
    return nf_udp_status[sID % len(nf_udp_status)]


def nf_status2id(status):
    return ([nf_udp_status.index(_) for _ in (status,) if _ in status] + [None, ])[0]


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


class Packet:
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
