from core.core import Core
from socket import socket, AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_BROADCAST
from threading import Thread, Lock


class NetManagerBase(Core):
    def __init__(self, parent=None, ip="0.0.0.0", port=22222, name='NetManager'):
        super().__init__(parent, port)

        self.port = port
        self.sock = socket(AF_INET, SOCK_DGRAM)
        self.sock.bind((ip, port))
        self.sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
        self.ip = self.sock.getsockname()[0]
        self.name = name
        self.clients = set()
        self.lock = Lock()
        self.actv = True
        self.thrd = Thread(name=f'Thread-{self.name}', target=self.run, daemon=True)
        self.thrd.start()

    def run(self):
        pass

    def channel0_slot(self, data: bytes):
        self.send_to_clients(data)

    def broadcast(self, data):
        if self.ip != "0.0.0.0":
            ip = self.ip[:self.ip.rfind('.')+1]+"255"
            n = self.sock.sendto(data, (ip, self.port))
            return n
        else:
            return None

    def send_to_clients(self, data: bytes):
        self.lock.acquire()
        for addr in self.clients:
            self.sock.sendto(data, addr)
        self.lock.release()


class NetManagerSimple(NetManagerBase):
    """
    Class only receive and send data from/to UDP socket and signal system
    """
    def __init__(self,  parent=None, ip="0.0.0.0", port=9009):
        super().__init__(parent, ip, port, 'NetManager-Simple')

    def run(self):
        while self.actv:
            data, addr = self.sock.recvfrom(1024)
            self.lock.acquire()
            self.clients.add(addr)
            self.lock.release()
            self.channel0.emit(data)


class Netmanager(NetManagerBase):
    def __init__(self, parent=None, ip="0.0.0.0", port=22222):
        super().__init__(parent, ip, port, 'NetManager-ch0')
        core = self.get_origin_core()
        if core is not None:
            self.channel0.connect(core.channel0)
            core.channel0.connect(self.channel0_slot)

    def run(self):
        while self.actv:
            data, addr = self.sock.recvfrom(1024)
            self.lock.acquire()
            self.clients.add(addr)
            self.lock.release()
            self.request.ParseFromString(data)
            self.channel0.emit(data)

    def channel0_slot(self, data: bytes):
        self.response.ParseFromString(data)
        if self.request.sender != self.response.sender:
            self.send_to_clients(data)
            self.response.sender = self.address
            self.request.sender = self.address