from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import QTableWidgetItem
from ui.CentralWidgetUIDesign import Ui_MainWidgetDesign
from core.sxr_protocol import packet_init
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, AmpStatus, AdcStatus


class MainWidget(QtWidgets.QWidget, Ui_MainWidgetDesign):

    channel0 = QtCore.pyqtSignal(bytes)  # For uplink
    channel1 = QtCore.pyqtSignal(bytes)  # For downlink
    channelStart = QtCore.pyqtSignal()  # To start system

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.AMPstatus = AmpStatus()
        self.ADCstatus = AdcStatus()
        self.SXRstatus = SystemStatus()
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.get_settings)
        self.timer.start(30000)

        self.address = 12

        self.manual_pushButton.clicked.connect(self.start_adc)
        self.external_pushButton.clicked.connect(self.start_adc)
        self.periodic_pushButton.clicked.connect(self.start_adc)
        self.stop_pushButton.clicked.connect(self.stop_adc)
        self.request = packet_init(SystemStatus.SXR, self.address)

        self.periodic_pushButton.setDisabled(True)

    def start_adc(self):
        self.channelStart.emit()

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

    def set_adc(self, connection=False):
        if connection is True:
            rate = self.ADCstatus.sampling_rate / 1e6
            time = self.ADCstatus.samples / rate / 1000
            ch_en = ''
            for ch in self.ADCstatus.board_status[0].channel_status:
                ch_en += str(int(ch.enabled))
            setText = 'rate: {0:3.0f}MHz, time: {1:3.0f}ms, ch: {2}'.format(rate, time, ch_en)
            if not self.ADCstatus.enabled:
                setText = '*DISCON* ' + setText
            self.status_tableWidget.setItem(0, 1, QTableWidgetItem(setText))
            self.status_tableWidget.resizeColumnToContents(1)
        elif connection is False:
            # setText = self.status_tableWidget.item(0, 1).text() + ' (DISCON)'
            # self.status_tableWidget.setItem(0, 1, QTableWidgetItem(setText))
            # self.status_tableWidget.resizeColumnToContents(1)
            pass

    def radio_buttons(self, command = None, ADCstatus_start = None):
        # if ADCstatus_start is not None:
        #     if ADCstatus_start == AdcStatus.INTSTART:
        #         self.external_pushButton.setDisabled(True)
        #         self.stop_pushButton.setDisabled(True)
        #         self.manual_pushButton.setEnabled(True)
        #     elif ADCstatus_start == AdcStatus.EXTSTART:
        #         self.external_pushButton.setEnabled(True)
        #         self.stop_pushButton.setEnabled(True)
        #         self.manual_pushButton.setDisabled(True)
        # elif command is not None:
        #     if command == Commands.START:
        #         self.manual_pushButton.setDisabled()
        if ADCstatus_start is not None and command is not None:
            if ADCstatus_start == AdcStatus.INTSTART:
                self.external_pushButton.setDisabled(True)
                self.stop_pushButton.setDisabled(True)
                if command == Commands.START:
                    self.manual_pushButton.setDisabled(True)
                elif command == Commands.STOP:
                    self.manual_pushButton.setEnabled(True)
                else:
                    self.manual_pushButton.setEnabled(True)
            elif ADCstatus_start == AdcStatus.EXTSTART:
                self.manual_pushButton.setDisabled(True)
                if command == Commands.START:
                    self.external_pushButton.setDisabled(True)
                    self.stop_pushButton.setEnabled(True)
                elif command == Commands.STOP:
                    self.external_pushButton.setEnabled(True)
                    self.stop_pushButton.setDisabled(True)
                else:
                    self.external_pushButton.setEnabled(True)
                    self.stop_pushButton.setDisabled(True)

    def get_settings(self):
        request = packet_init(SystemStatus.SXR, self.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

        request = packet_init(SystemStatus.AMP, self.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

        request = packet_init(SystemStatus.ADC, self.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

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
                try:
                    str_data = request.data.decode()
                    if str_data == 'ADC disconnected':
                        self.set_adc(connection=False)
                except UnicodeDecodeError:
                    self.ADCstatus.ParseFromString(request.data)
                    self.set_adc(connection=True)
                    self.radio_buttons(ADCstatus_start=self.ADCstatus.start, command=(request.command^ 0xFFFFFFFF))

        elif request.sender == SystemStatus.SXR:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.SXRstatus.ParseFromString(request.data)

    def showEvent(self, a0: QtGui.QShowEvent) -> None:
        self.get_settings()
        self.show()
