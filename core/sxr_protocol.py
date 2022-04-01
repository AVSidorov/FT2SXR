from core.sxr_protocol_pb2 import MainPacket
import warnings

def packet_init(address, sender):
    request = MainPacket()
    request.address = address
    request.sender = sender
    return request


def isPacketOfType(data, packetT):
    pkt = packetT()
    with warnings.catch_warnings(record=True) as w:
        warnings.simplefilter("always")
        pkt.ParseFromString(data)
    if len(w) == 0:
        return True
    else:
        return False
