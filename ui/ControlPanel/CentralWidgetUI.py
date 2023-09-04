import os
import time
import psutil
from PyQt5 import QtWidgets, QtCore, QtGui
from ui.ControlPanel.CentralWidgetUIDesign import Ui_MainWidgetDesign
from core.sxr_protocol import packet_init
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, AmpStatus, AdcStatus, HardwareStatus, TokamakStatus, JournalStatus, GsaStatus
from core.fileutils import today_dir, work_dir


class MainWidget(QtWidgets.QWidget, Ui_MainWidgetDesign):

    channel0 = QtCore.pyqtSignal(bytes)  # For uplink
    channel1 = QtCore.pyqtSignal(bytes)  # For downlink
    channelSettings = QtCore.pyqtSignal(int)  # To open settings windows
    channelStart = QtCore.pyqtSignal()  # Uplink to start system
    channelSnapshot = QtCore.pyqtSignal()  # Uplink to make snapshot

    def __init__(self, parent=None):
        curdir = os.getcwd()
        os.chdir(os.path.join(curdir, 'ui', 'ControlPanel'))
        super().__init__(parent=parent)
        self.setupUi(self)
        os.chdir(curdir)
        self.wannastop = False
        self.working_in_periodic = False
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

        self.start_time = time.time()
        self.working_time_timer = QtCore.QTimer(self)
        self.working_time_timer.timeout.connect(self.quick_check)
        self.working_time_timer.start(1000)

        self.work_dir = today_dir()
        self.today_files = []

        self.address = 16
        self.isInPeriodic = False

        # self.manual_pushButton.clicked.connect(self.start_btn)
        self.external_pushButton.clicked.connect(self.start_adc)
        # self.periodic_pushButton.clicked.connect(self.start_adc)
        self.changeADC_pushButton.clicked.connect(self._openADC)
        self.changeAmp_pushButton.clicked.connect(self._openAmp)
        self.changeGSA_pushButton.clicked.connect(self._openGSA)
        self.changeHard_pushButton.clicked.connect(self._openHard)
        self.changeTok_pushButton.clicked.connect(self._openTok)
        self.changeJour_pushButton.clicked.connect(self._openJour)
        self.journal_groupBox.toggled.connect(self._check_modules)
        self.adc_groupBox.toggled.connect(self._check_modules)
        self.hardware_groupBox.toggled.connect(self._check_modules)
        self.amp_groupBox.toggled.connect(self._check_modules)
        self.tokamak_groupBox.toggled.connect(self._check_modules)
        self.gsa_groupBox.toggled.connect(self._check_modules)
        # self.nas_groupBox.toggled.connect(self._check_modules)
        self.manual_pushButton.hide()
        self.periodic_manual_pushButton.hide()
        self.periodic_external_pushButton.hide()
        self.save_pushButton.clicked.connect(self._save_file)
        self.remove_pushButton.clicked.connect(self._remove_file)
        self.stop_pushButton.clicked.connect(self.stop_adc)
        self.request = packet_init(SystemStatus.SXR, self.address)

        self.periodic_manual_pushButton.setDisabled(True)
        self.periodic_external_pushButton.setDisabled(True)
        self.external_pushButton.setDisabled(True)

        self._set_system_check()

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

    def _set_working_time(self):
        delta = time.time() - self.start_time
        # if delta > 60:
        self.time_label.setText(f'{str(int(delta//3600)).rjust(2, "0")}:{str(int(delta%3600//60)).rjust(2, "0")}:{str(round(delta%3600%60)).rjust(2, "0")}')

    def _check_dir(self):
        if self.today_files != os.listdir(self.work_dir):
            self.today_files = os.listdir(self.work_dir)
            self.dir_treeWidget.clear()
            n = 0
            v = 0
            for file in os.scandir(self.work_dir):
                stat = file.stat()
                v += stat.st_size
                item = QtWidgets.QTreeWidgetItem([file.name, str(round(stat.st_size/1048576, 1)),
                                                  time.strftime('%X %d.%m.%y', time.gmtime(stat.st_ctime))])
                if file.name[-3:] == '.h5':
                    item.setIcon(0, QtGui.QIcon(
                        os.path.join(work_dir(), 'style', 'icons', 'icons8-business-report-48.png')))
                    n += 1
                elif file.name[-4:] == '.txt':
                    item.setIcon(0, QtGui.QIcon(
                        os.path.join(work_dir(), 'style', 'icons', 'icons8-document-48.png')))
                self.dir_treeWidget.addTopLevelItem(item)
            self.dir_treeWidget.resizeColumnToContents(0)
            self.dir_treeWidget.resizeColumnToContents(1)
            self.dir_treeWidget.scrollToBottom()
            self.shots_label.setText(f'Сохранено {n} разрядов ({round(v/1048576, 1)} Мб):')

    def quick_check(self):
        self._set_working_time()
        self._check_dir()

    def start_adc(self):
        if self.ADCstatus.is_in_periodic:
            self.working_in_periodic = True
        self._set_system_working()
        self.channelStart.emit()

    def stop_adc(self):
        if self.working_in_periodic:
            self.wannastop = True

        self.request.command = Commands.STOP
        if self.request.IsInitialized():
            self.channel0.emit(self.request.SerializeToString())
        self._set_system_ready()

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
        if self.JOURNALstatus.daycomment == '':
            self.day_comment_label.show()
        else:
            self.day_comment_label.hide()

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
            self.adc_status_label.setStyleSheet('color: rgb(0, 170, 0)')
        else:
            self.adc_status_label.setText('Disconnected')
            self.adc_status_label.setStyleSheet('color: rgb(255, 0, 0)')

        if self.ADCstatus.start == AdcStatus.INTSTART and not self.ADCstatus.is_in_periodic:
            self.start_label.setText('Ручной')
        elif self.ADCstatus.start == AdcStatus.EXTSTART and not self.ADCstatus.is_in_periodic:
            self.start_label.setText('Триггер')
        elif self.ADCstatus.start == AdcStatus.INTSTART and self.ADCstatus.is_in_periodic:
            self.start_label.setText('Периодический ручной')
        elif self.ADCstatus.start == AdcStatus.EXTSTART and self.ADCstatus.is_in_periodic:
            self.start_label.setText('Периодический триггер')

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

    def radio_buttons(self, ADCstatus_start = None, AdcStatus_is_in_periodic = None):
        if ADCstatus_start is not None and AdcStatus_is_in_periodic is not None:
            if ADCstatus_start == AdcStatus.INTSTART and not AdcStatus_is_in_periodic:
                self.external_pushButton.setText('Manual\nstart')
                self.external_pushButton.setIcon(QtGui.QIcon(os.path.join(work_dir(), 'style', 'icons',
                                                                          'icons8-play-48.png')))
            elif ADCstatus_start == AdcStatus.EXTSTART and not AdcStatus_is_in_periodic:
                self.external_pushButton.setText('External\nstart')
                self.external_pushButton.setIcon(QtGui.QIcon(os.path.join(work_dir(), 'style', 'icons',
                                                                          'icons8-play-48.png')))
            elif ADCstatus_start == AdcStatus.INTSTART and AdcStatus_is_in_periodic:
                self.external_pushButton.setText('Manual\nperiodic\nstart')
                self.external_pushButton.setIcon(QtGui.QIcon(os.path.join(work_dir(), 'style', 'icons',
                                                                          'icons8-fast-forward-48.png')))
            elif ADCstatus_start == AdcStatus.EXTSTART and AdcStatus_is_in_periodic:
                self.external_pushButton.setText('External\nperiodic\nstart')
                self.external_pushButton.setIcon(QtGui.QIcon(os.path.join(work_dir(), 'style', 'icons',
                                                                          'icons8-fast-forward-48.png')))

        # if command is not None:
        #     if command == Commands.START:
        #         self.external_pushButton.setDisabled(True)
        #     elif command == Commands.START ^ 0xFFFFFFFF:
        #         self.external_pushButton.setDisabled(True)
        #     elif command == Commands.STOP:
        #         self.external_pushButton.setEnabled(True)
        #     elif command == Commands.STOP ^ 0xFFFFFFFF:
        #         self.external_pushButton.setEnabled(True)

    def _check_modules(self):
        if all([self.journal_groupBox.isChecked(), self.adc_groupBox.isChecked(),
                self.tokamak_groupBox.isChecked(), self.hardware_groupBox.isChecked(),
                self.amp_groupBox.isChecked(), self.gsa_groupBox.isChecked()]):
            self._set_system_ready()
            self.journal_groupBox.setCheckable(False)
            self.adc_groupBox.setCheckable(False)
            self.tokamak_groupBox.setCheckable(False)
            self.hardware_groupBox.setCheckable(False)
            self.amp_groupBox.setCheckable(False)
            self.gsa_groupBox.setCheckable(False)

    def _save_file(self):
        self.channelSnapshot.emit()
        self._set_system_ready()

    def _remove_file(self):
        self._set_system_ready()

    def _set_system_working(self):
        self.state_label.setText('В работе. Ожидаю запуск АЦП...')
        # self.state_icon_label_1.setPixmap(QtGui.QPixmap(os.path.join(
        #     work_dir(), 'style', 'icons', 'icons8-expired-48.png'
        # )))
        # self.state_icon_label_2.setPixmap(QtGui.QPixmap(os.path.join(
        #     work_dir(), 'style', 'icons', 'icons8-expired-48.png'
        # )))
        im1 = QtGui.QMovie(os.path.join(
            work_dir(), 'style', 'icons', 'icons8-attention.gif'
        ))
        im1.start()
        self.state_icon_label_1.setMovie(im1)
        im2 = QtGui.QMovie(os.path.join(
            work_dir(), 'style', 'icons', 'icons8-attention.gif'
        ))
        im2.start()
        self.state_icon_label_2.setMovie(im1)
        self.external_pushButton.setDisabled(True)

    def _set_system_saving(self):
        # TODO saving notification
        pass

    def _set_system_downloading(self):
        pass

    def _set_system_measuring(self):
        pass

    def _set_system_check(self):
        self.state_label.setText('Проверьте все приборы и устройства. Отметьте устройства на панели "Настройки".')
        # self.state_icon_label_1.setPixmap(QtGui.QPixmap(os.path.join(
        #     work_dir(), 'style', 'icons', 'icons8-double-tick-48.png'
        # )))
        # self.state_icon_label_2.setPixmap(QtGui.QPixmap(os.path.join(
        #     work_dir(), 'style', 'icons', 'icons8-double-tick-48.png'
        # )))
        im1 = QtGui.QMovie(os.path.join(
            work_dir(), 'style', 'icons', 'icons8-done.gif'
        ))
        im1.start()
        self.state_icon_label_1.setMovie(im1)
        im2 = QtGui.QMovie(os.path.join(
            work_dir(), 'style', 'icons', 'icons8-done.gif'
        ))
        im2.start()
        self.state_icon_label_2.setMovie(im1)
        self.save_pushButton.hide()
        self.save_pushButton.setDisabled(True)
        self.remove_pushButton.hide()
        self.remove_pushButton.setDisabled(True)
        self.external_pushButton.setDisabled(True)

    def _set_system_ready(self):
        self.state_label.setText('Система готова к работе.')
        self.state_icon_label_1.setPixmap(QtGui.QPixmap(os.path.join(
            work_dir(), 'style', 'icons', 'icons8-checkmark-48.png'
        )))
        self.state_icon_label_2.setPixmap(QtGui.QPixmap(os.path.join(
            work_dir(), 'style', 'icons', 'icons8-checkmark-48.png'
        )))
        self.external_pushButton.show()
        self.external_pushButton.setEnabled(True)
        self.save_pushButton.hide()
        self.save_pushButton.setDisabled(True)
        self.remove_pushButton.hide()
        self.remove_pushButton.setDisabled(True)

    def _set_system_save(self):
        self.state_label.setText('Данные собраны. Проверьте настройки и спасите/удалите файл.')
        self.state_icon_label_1.setPixmap(QtGui.QPixmap(os.path.join(
            work_dir(), 'style', 'icons', 'icons8-save-as-48.png'
        )))
        self.state_icon_label_2.setPixmap(QtGui.QPixmap(os.path.join(
            work_dir(), 'style', 'icons', 'icons8-save-as-48.png'
        )))
        self.external_pushButton.hide()
        self.external_pushButton.setDisabled(True)
        self.save_pushButton.show()
        self.save_pushButton.setEnabled(True)
        self.remove_pushButton.show()
        self.remove_pushButton.setEnabled(True)

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

        # self._check_nas()
        self._check_pc()

    def _check_nas(self):
        pass

    def _check_pc(self):
        print(psutil.cpu_percent())
        mem = psutil.virtual_memory()
        print(round(mem.total/1024/1024/1024, 1))
        print(round(mem.available/1024/1024/1024, 1))
        disk = psutil.disk_usage(work_dir())
        print(round(disk.total/1024/1024/1024, 1))
        print(round(disk.free/1024/1024/1024, 1))

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
                    self.radio_buttons(ADCstatus_start=self.ADCstatus.start, AdcStatus_is_in_periodic = self.ADCstatus.is_in_periodic)

        elif request.sender == SystemStatus.SXR:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.SXRstatus.ParseFromString(request.data)
            elif request.command == Commands.DONE:
                self._set_system_save()
                if self.ADCstatus.is_in_periodic:
                    self.save_pushButton.click()
                    if not self.wannastop:
                        self.external_pushButton.click()
                    elif self.wannastop:
                        self.wannastop = False
                        self.working_in_periodic = False
            # self.radio_buttons(ADCstatus_start=None, command=(request.command ^ 0xFFFFFFFF))

    def showEvent(self, a0: QtGui.QShowEvent) -> None:
        self.get_settings()
        self._check_dir()
        self.show()
