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
        # MainWidget.resize(self, w, h)
        # self.groupBox_3.setGeometry(QtCore.QRect(10, 10, int(w*0.28), int(h-30)))
        # self.groupBox_2.setGeometry(QtCore.QRect(int(w*0.29)+10, int(0.61*h)+10, int(w*0.68), int(h-30)))
        # self.groupBox.setGeometry(QtCore.QRect(int(w*0.29)+10, 10, int(w*0.68), int(0.6*h)))

        self.manual_pushButton.clicked.connect(self.start_adc)
        self.stop_pushButton.clicked.connect(self.stop_adc)

    def start_adc(self):
        request = packet_init(1, 0)
        request.command = 2
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def stop_adc(self):
        request = packet_init(1, 0)
        request.command = 3
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        self.channel1.emit(data)

