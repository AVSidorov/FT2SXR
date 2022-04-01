import signal
from socket import socket, AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_REUSEADDR


if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    sock = socket(AF_INET, SOCK_DGRAM)
    sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    sock.bind(("0.0.0.0", 9009))
    ex = False
    while not ex:
        data, addr = sock.recvfrom(1024)
        print(f'data length {len(data)}')
        print(data)
