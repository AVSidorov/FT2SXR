import numpy as np

# devices names as tuple
devs = ('DP5', 'PX5', 'DP5G', 'MCA8000D', 'TB5', 'DP5-X')


# lambdas to avoid bad indexing
def devById(dev_id: int) -> str:
    return devs[dev_id % len(devs)]


def idByDev(dev: str) -> int:
    return ([devs.index(_) for _ in (dev,) if _ in devs] + [None, ])[0]


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
    pkt = Packet(pkt)
    if pkt is None:
        return None
    if pkt.pkt[2:4] == b'\x02\x02':
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
    return packet(b'\xf0', b'\x02')


def response_enable_mca(pkt, obj):
    pkt = Packet(pkt)
    if pkt is None:
        return None
    if pkt.pid1 == b'\xf0' and pkt.pid2 == b'\x02':
        return request_ok()
    return None


def request_disable_mca():
    return packet(b'\xf0', b'\x03')


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


class Packet:

    def __new__(cls, pkt=None, **kwargs):
        if pkt is not None:
            if isinstance(pkt, (bytes, bytearray)):
                if check_packet(pkt):
                    return super().__new__(cls)
        else:
            return super().__new__(cls)
        return None

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

        self.requests['request_txt_cfg_readback'] = request_txt_cfg_readback
        self.responses['response_txt_cfg_readback'] = response_txt_cfg_readback

        self.responses['response_ok'] = response_ok

        self.requests['request_enable_mca'] = request_enable_mca
        self.responses['response_enable_mca'] = response_enable_mca

        self.requests['request_disable_mca'] = request_disable_mca
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
