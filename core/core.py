from PyQt5 import QtCore
from core.sxr_protocol_pb2 import MainPacket


class Core(QtCore.QObject):
    channel0 = QtCore.pyqtSignal(bytes)
    channel1 = QtCore.pyqtSignal(bytes)
    channel2 = QtCore.pyqtSignal(bytes)

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        pass

    @QtCore.pyqtSlot(bytes)
    def channel1_slot(self, data: bytes):
        pass

    @QtCore.pyqtSlot(bytes)
    def channel2_slot(self, data: bytes):
        pass

    def get_origin_core(self):
        if self.parent() is not None:
            if isinstance(self.parent(), Core):
                return self.parent().get_origin_core()
            else:
                return self
        else:
            return self


class Dev(Core):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.address = -1

    def start(self, response: MainPacket = None):
        if response is not None:
            if response.IsInitialized():
                self.channel0.emit(response.SerializeToString())

    def stop(self, response: MainPacket = None):
        if response is not None:
            if response.IsInitialized():
                self.channel0.emit(response.SerializeToString())

    def snapshot(self, file: str = None):
        pass
