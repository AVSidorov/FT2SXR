import sys
import os
import signal

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

from socket import socket, AF_INET, SOCK_DGRAM
from insys.EXAM.exam_adc.exam_protocol_pb2 import BRD_ctrl as Adc_msg

if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    sock = socket(AF_INET, SOCK_DGRAM)
    iter = 0
    ex = False
    while not ex:
        pkt = Adc_msg()
        cmd = input(">")
        if cmd != 'exit':
            pkt.command = 0
        else:
            ex = True
            pkt.command = 11

        pkt.out = iter
        pkt.status = -1
        print(f"cmd: {pkt.command}, out:{pkt.out}, status:{pkt.status}")
        data = pkt.SerializeToString()
        print(f'data len {len(data)}')
        print(data)
        sock.sendto(data, ("127.0.0.3", 9009))

        iter += 1