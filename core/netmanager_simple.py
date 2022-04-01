from core.core import Core
from socket import socket, inet_aton, AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_BROADCAST, SO_REUSEADDR
from threading import Thread


class NetManagerSimple(Core):
    """
    Class only receive and send data from/to UDP socket
    """

    def __init__(self, parent=None, ip="0.0.0.0", port=9009):
        super().__init__(parent)
        self.address = 9
        self.port = port
        self.sock = socket(AF_INET, SOCK_DGRAM)
        self.sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
        self.sock.bind((ip, port))
        self.ip = self.sock.getsockname()[0]

        self.clients = set()
        self.actv = True
        self.thrd = Thread(name='Thread-netmanager-simple', target=self.run, daemon=True)
        self.thrd.start()

    def run(self):
        while self.actv:
            data, addr = self.sock.recvfrom(1024)
            self.clients.add(addr)
            # self.channel0.emit(data)
            self.channel0.emit(f'New data from {addr[0]}:{addr[1]}'.encode('utf8'))
            print(data)

    def channel0_slot(self, data: bytes):
        self.sendToClients(data)

    def sendToClients(self, data):
        sock = socket(AF_INET, SOCK_DGRAM)
        for addr in self.clients:
            sock.sendto(data, addr)
        sock.close()

    def broadcast(self, data):
        sock = socket(AF_INET, SOCK_DGRAM)
        sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
        ip = self.ip[:self.ip.rfind('.')+1]+"255"
        sock.sendto(data, (ip, self.port))
        sock.close()