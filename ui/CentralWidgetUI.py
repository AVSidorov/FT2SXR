from PyQt5 import QtWidgets, QtCore
from ui.CentralWidgetUIDesign import Ui_MainWidgetDesign
from core.sxr_protocol import packet_init
from core.sxr_protocol_pb2 import MainPacket


class MainWidget(QtWidgets.QWidget, Ui_MainWidgetDesign):

    channel0 = QtCore.pyqtSignal(bytes)  # For uplink
    channel1 = QtCore.pyqtSignal(bytes)  # For downlink

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        MainWidget.setFixedSize(self, 553, 462)

        self.manual_pushButton.clicked.connect(self.start_adc)

    def start_adc(self):
        request = packet_init(1, 0)
        request.command = 2
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        self.channel1.emit(data)

