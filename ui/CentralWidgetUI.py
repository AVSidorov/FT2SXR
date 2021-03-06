from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import QTableWidgetItem
from ui.CentralWidgetUIDesign import Ui_MainWidgetDesign
from core.sxr_protocol import packet_init
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, AmpStatus, AdcStatus


class MainWidget(QtWidgets.QWidget, Ui_MainWidgetDesign):

    channel0 = QtCore.pyqtSignal(bytes)  # For uplink
    channel1 = QtCore.pyqtSignal(bytes)  # For downlink

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.AMPstatus = AmpStatus()
        self.ADCstatus = AdcStatus()

        self.address = 12

        self.manual_pushButton.clicked.connect(self.start_adc)
        self.stop_pushButton.clicked.connect(self.stop_adc)
        self.request = packet_init(SystemStatus.SXR, 12)

        self.get_status()

    def start_adc(self):
        self.request.command = Commands.START
        if self.request.IsInitialized():
            self.channel0.emit(self.request.SerializeToString())

    def stop_adc(self):
        self.request.command = Commands.STOP
        if self.request.IsInitialized():
            self.channel0.emit(self.request.SerializeToString())

    def set_amp(self):
        tail = ''
        for i in range(4):
            tail = ('1' if self.AMPstatus.tail & (1 << i) else '0') + tail
        setText = 'gA: {0:4.2f}, gB: {1:4.2f}, tail: {2:s}'.format(self.AMPstatus.gainA, self.AMPstatus.gainB, tail)
        self.status_tableWidget.setItem(0, 2, QTableWidgetItem(setText))
        self.status_tableWidget.resizeColumnToContents(2)

    def get_status(self):
        request = packet_init(SystemStatus.AMP, self.address)
        request.command = Commands.STATUS
        self.channel0.emit(request.SerializeToString())

        request = packet_init(SystemStatus.ADC, self.address)
        request.command = Commands.STATUS
        self.channel0.emit(request.SerializeToString())

    def set_adc(self):
        rate = self.ADCstatus.sampling_rate / 1e6
        time = self.ADCstatus.samples / rate / 1000
        ch_en = ''
        for ch in self.ADCstatus.board_status[0].channel_status:
            ch_en += str(int(ch.enabled))
        setText = 'rate: {0:3.0f}MHz, time: {1:3.0f}ms, ch: {2}'.format(rate, time, ch_en)
        self.status_tableWidget.setItem(0, 1, QTableWidgetItem(setText))
        self.status_tableWidget.resizeColumnToContents(1)

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        self.channel1.emit(data)

        request = MainPacket()
        request.ParseFromString(data)
        if request.sender == SystemStatus.AMP:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.AMPstatus.ParseFromString(request.data)
                self.set_amp()

        elif request.sender == SystemStatus.ADC:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.ADCstatus.ParseFromString(request.data)
                self.set_adc()
