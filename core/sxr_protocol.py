from core.sxr_protocol_pb2 import MainPacket


def packet_init(address, sender):
    request = MainPacket()
    request.address = address
    request.sender = sender
    return request

