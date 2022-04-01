from socket import socket, AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_REUSEADDR
import sys
import os
from socket import socket, AF_INET, SOCK_DGRAM

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

from core.sxr_protocol import MainPacket, isPacketOfType
from core.exam_protocol_pb2 import BRD_ctrl as Adc_msg

if __name__ == '__main__':
    sock = socket(AF_INET, SOCK_DGRAM)
    sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    sock.bind(("0.0.0.0", 9009))
    ex = False
    while not ex:
        data, addr = sock.recvfrom(1024)
        if isPacketOfType(data, Adc_msg):
            pkt = Adc_msg()
            pkt.ParseFromString(data)
            print(f"cmd: {pkt.command}, out:{pkt.out}, status:{pkt.status}")
        if isPacketOfType(data, MainPacket):
            pkt = MainPacket()
            pkt.ParseFromString(data)
            print(f"cmd: {pkt.command}, to:{pkt.address}, from:{pkt.sender} with data {len(pkt.data)} bytes")

        if pkt.command == 11:
            ex = True
