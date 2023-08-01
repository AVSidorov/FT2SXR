from PyQt5 import QtWidgets, QtCore, QtGui
from ui.ControlPanel.CentralWidgetUIDesign import Ui_MainWidgetDesign
from core.sxr_protocol import packet_init
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, AmpStatus, AdcStatus, HardwareStatus, TokamakStatus, JournalStatus, GsaStatus


class MainWidget(QtWidgets.QWidget, Ui_MainWidgetDesign):

    channel0 = QtCore.pyqtSignal(bytes)  # For uplink
    channel1 = QtCore.pyqtSignal(bytes)  # For downlink
    channelSettings = QtCore.pyqtSignal(int)  # To open settings windows
    channelStart = QtCore.pyqtSignal()  # To start system

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.AMPstatus = AmpStatus()
        self.ADCstatus = AdcStatus()
        self.SXRstatus = SystemStatus()
        self.HARDWAREstatus = HardwareStatus()
        self.TOKAMAKstatus = TokamakStatus()
        self.JOURNALstatus = JournalStatus()
        self.GSAstatus = GsaStatus()
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.get_settings)
        self.timer.start(30000)

        self.address = 16
        self.isInPeriodic = False

        # self.manual_pushButton.clicked.connect(self.start_btn)
        self.external_pushButton.clicked.connect(self.start_btn)
        # self.periodic_pushButton.clicked.connect(self.start_adc)
        self.changeADC_pushButton.clicked.connect(self._openADC)
        self.changeAmp_pushButton.clicked.connect(self._openAmp)
        self.changeGSA_pushButton.clicked.connect(self._openGSA)
        self.changeHard_pushButton.clicked.connect(self._openHard)
        self.changeTok_pushButton.clicked.connect(self._openTok)
        self.changeJour_pushButton.clicked.connect(self._openJour)
        self.manual_pushButton.hide()
        self.periodic_manual_pushButton.hide()
        self.periodic_external_pushButton.hide()
        self.stop_pushButton.clicked.connect(self.stop_adc)
        self.request = packet_init(SystemStatus.SXR, self.address)

        self.periodic_manual_pushButton.setDisabled(True)
        self.periodic_external_pushButton.setDisabled(True)

    def _openADC(self):
        self.channelSettings.emit(SystemStatus.ADC)

    def _openAmp(self):
        self.channelSettings.emit(SystemStatus.AMP)

    def _openGSA(self):
        self.channelSettings.emit(SystemStatus.GSA)

    def _openHard(self):
        self.channelSettings.emit(SystemStatus.HARDWARE)

    def _openTok(self):
        self.channelSettings.emit(SystemStatus.TOKAMAK)

    def _openJour(self):
        self.channelSettings.emit(SystemStatus.JOURNAL)

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
        # setText = 'gA: {0:4.2f}, gB: {1:4.2f}, tail: {2:s}'.format(self.AMPstatus.gainA, self.AMPstatus.gainB, tail) #del
        # self.status_tableWidget.setItem(0, 2, QTableWidgetItem(setText))
        # self.status_tableWidget.resizeColumnToContents(2)
        self.amp_mask_label.setText(tail)
        self.amp_gainA_label.setText(f'{self.AMPstatus.gainA:4.2f}')
        self.amp_gainB_label.setText(f'{self.AMPstatus.gainB:4.2f}')

    def set_hardware(self):
        self.hardware_foil_label.setText(self.HARDWAREstatus.foil)
        self.hardware_diaph_label.setText(self.HARDWAREstatus.diaphragm)
        self.hardware_angle_label.setText(f'{self.HARDWAREstatus.angle:3.1f}')
        self.hardware_Hknife_label.setText(f'{self.HARDWAREstatus.hknife:4.2f}')
        self.hardware_Vknife_label.setText(f'{self.HARDWAREstatus.vknife:4.2f}')

    def set_tokamak(self):
        self.current_label.setText(str(self.TOKAMAKstatus.current))
        self.density_label.setText(f'{self.TOKAMAKstatus.density:3.1f}')
        self.power_label.setText(str(self.TOKAMAKstatus.power))
        if self.TOKAMAKstatus.shotType == self.TOKAMAKstatus.OH:
            self.mode_label.setText('OH')
        elif self.TOKAMAKstatus.shotType == self.TOKAMAKstatus.RF:
            self.mode_label.setText('RF')
        elif self.TOKAMAKstatus.shotType == self.TOKAMAKstatus.GLOW:
            self.mode_label.setText('Тлеющий')
        elif self.TOKAMAKstatus.shotType == self.TOKAMAKstatus.OFF:
            self.mode_label.setText('OFF')

    def set_journal(self):
        self.sxrNo_label.setText(str(self.JOURNALstatus.SXRshot))
        self.tokamakNo_label.setText(str(self.JOURNALstatus.TOKAMAKshot))
        self.fileName_label.setText(str(self.JOURNALstatus.filename))
        self.comment_textBrowser.setText(self.JOURNALstatus.comment)

    def set_gsa(self):
        self.gsa_ampl_label.setText(f'{self.GSAstatus.amplitude:4.1f}')
        self.gsa_edge_label.setText(f'{self.GSAstatus.edge}')
        self.gsa_freq_label.setText(f'{self.GSAstatus.frequency}')

    def set_adc(self, connection=False):
        # if connection is True:
        rate = self.ADCstatus.sampling_rate / 1e6
        time = self.ADCstatus.samples / rate / 1000
        self.adc_freq_label.setText(f'{rate:3.0f}')
        self.adc_duration_label.setText(f'{time:4.0f}')
        if self.ADCstatus.connected:
            self.adc_status_label.setText('Connected')
        else:
            self.adc_status_label.setText('Disconnected')

        for n, ch in enumerate(self.ADCstatus.board_status[0].channel_status):
            if ch.enabled and not ch.void:
                eval(f'self.ch{n+1}_groupBox.show()')
                eval(f'self.ch{n+1}_name_label.setText(ch.name)')
                eval(f'self.ch{n+1}_bias_label.setText(f"{ch.bias:3.0f}")')
            else:
                eval(f'self.ch{n+1}_groupBox.hide()')
        # setText = 'rate: {0:3.0f}MHz, time: {1:3.0f}ms, ch: {2}'.format(rate, time, ch_en)
            # if not self.ADCstatus.enabled:
            #     setText = '*DISCON* ' + setText
                # pass
            # self.status_tableWidget.setItem(0, 1, QTableWidgetItem(setText))
            # self.status_tableWidget.resizeColumnToContents(1)
        # elif connection is False:
            # setText = '(DISCON) ' + self.status_tableWidget.item(0, 1).text()
            # self.status_tableWidget.setItem(0, 1, QTableWidgetItem(setText))
            # self.status_tableWidget.resizeColumnToContents(1)
            # pass

    def start_btn(self):
        text = self.external_pushButton.text()
        if text == 'Manual start':
            self.start_adc()
        elif text == 'External start':
            self.start_adc()
        # self.external_pushButton.setDisabled(True)

    def radio_buttons(self, command = None, ADCstatus_start = None):
        if ADCstatus_start is not None:
            if ADCstatus_start == AdcStatus.INTSTART:
                self.external_pushButton.setText('Manual start')
            elif ADCstatus_start == AdcStatus.EXTSTART:
                self.external_pushButton.setText('External start')

        if command is not None:
            if command == Commands.START:
                self.external_pushButton.setDisabled(True)
            elif command == Commands.START ^ 0xFFFFFFFF:
                self.external_pushButton.setDisabled(True)
            elif command == Commands.STOP:
                self.external_pushButton.setEnabled(True)
            elif command == Commands.STOP ^ 0xFFFFFFFF:
                self.external_pushButton.setEnabled(True)

    def get_settings(self):
        request = packet_init(SystemStatus.SXR, self.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

        request = packet_init(SystemStatus.AMP, self.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

        request = packet_init(SystemStatus.HARDWARE, self.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

        request = packet_init(SystemStatus.ADC, self.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

        request = packet_init(SystemStatus.TOKAMAK, self.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

        request = packet_init(SystemStatus.JOURNAL, self.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

        request = packet_init(SystemStatus.GSA, self.address)
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

        elif request.sender == SystemStatus.HARDWARE:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.HARDWAREstatus.ParseFromString(request.data)
                self.set_hardware()

        elif request.sender == SystemStatus.TOKAMAK:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.TOKAMAKstatus.ParseFromString(request.data)
                self.set_tokamak()

        elif request.sender == SystemStatus.JOURNAL:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.JOURNALstatus.ParseFromString(request.data)
                self.set_journal()

        elif request.sender == SystemStatus.GSA:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.GSAstatus.ParseFromString(request.data)
                self.set_gsa()

        elif request.sender == SystemStatus.ADC:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                try:
                    str_data = request.data.decode()
                    if str_data == 'ADC disconnected':
                        self.set_adc(connection=False)
                except UnicodeDecodeError:
                    self.ADCstatus.ParseFromString(request.data)
                    self.set_adc(connection=True)
                    self.radio_buttons(ADCstatus_start=self.ADCstatus.start, command=None)

        elif request.sender == SystemStatus.SXR:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.SXRstatus.ParseFromString(request.data)
            self.radio_buttons(ADCstatus_start=None, command=(request.command ^ 0xFFFFFFFF))

    def showEvent(self, a0: QtGui.QShowEvent) -> None:
        self.get_settings()
        self.show()
