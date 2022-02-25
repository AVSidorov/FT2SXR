from PyQt5 import QtCore


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
    def channel1_slot(self, data: bytes):
        pass
