from PyQt5 import QtWidgets, QtCore
from ui.CentralWidgetUIDesign import Ui_MainWidgetDesign
from core.sxr_protocol import packet_init
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands


class MainWidget(QtWidgets.QWidget, Ui_MainWidgetDesign):

    channel0 = QtCore.pyqtSignal(bytes)  # For uplink
    channel1 = QtCore.pyqtSignal(bytes)  # For downlink

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)

        self.address = 17

        self.manual_pushButton.clicked.connect(self.start_adc)
        self.stop_pushButton.clicked.connect(self.stop_adc)
        self.request = packet_init(SystemStatus.SXR, 12)

    def start_adc(self):
        self.request.command = Commands.START
        if self.request.IsInitialized():
            self.channel0.emit(self.request.SerializeToString())

    def stop_adc(self):
        self.request.command = Commands.STOP
        if self.request.IsInitialized():
            self.channel0.emit(self.request.SerializeToString())

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        self.channel1.emit(data)

