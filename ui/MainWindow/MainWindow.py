import os
import sys
import gc
from time import time
from multiprocessing import Process
import subprocess
from ui.MainWindow.MainWindowUIDesign import Ui_MainWindow
from ui.ControlPanel.CentralWidgetUI import MainWidget
from ui.ADC.ADCUI import ADCUIWidget
from ui.GSA.GSAUI import GSAWidget
from ui.PLOTTER.PlotterUI import main as PlotterMain
from ui.NAS.nasWidget import main as NasMain
# from ui.PX5UI import PX5Widget
from ui.PX5_bigUI import PX5Widget
from ui.AMP.AmplifierUI import AmplifierWidget
from ui.MeasurementSettingsUI import MeasurementSettingsWidget
from ui.HARDWARE.HardwareUI import HardwareWidget
from ui.CalibrationSettingsUI import CalibrationSettingsWidget
from ui.MiniX2UI import MiniX2Widget
from PyQt5 import QtWidgets, QtCore
from core.sxr_protocol import packet_init
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands
from core.logger import Logger
from core.adc_logger import ADCLogger
from core.fileutils import work_dir
from ui.ADCLogger.ADCLogUI import AdcLog
from ui.PLOTTER.PlotterUI import PlotterWidget
from ui.JOURNAL.ShotSettingsUI import ShotSettings
from ui.TOKAMAK.TokamakUI import TokamakWidget


class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):
    channel0 = QtCore.pyqtSignal(bytes)  # For uplink
    channel1 = QtCore.pyqtSignal(bytes)  # For downlink
    channel2 = QtCore.pyqtSignal(bytes)  # For ADC Log

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.address = 16

        self.actionADC.triggered.connect(self.action_adc_set)
        # self.actionPX_5.triggered.connect(self.action_px5_set)
        self.actionGSA.triggered.connect(self.action_gsa_set)
        self.actionAmplifier.triggered.connect(self.action_amplifier_set)
        # self.actionCalibration_settings.triggered.connect(self.action_calibration_set)
        # self.actionMeasurement_settings.triggered.connect(self.action_measurement_set)
        # self.actionMini_X2.triggered.connect(self.action_minix2_set)
        self.actionShow_log.triggered.connect(self.action_adclog)
        self.actionHardware.triggered.connect(self.action_hardware_set)
        self.actionShot.triggered.connect(self.action_shot_set)
        self.actionTokamak.triggered.connect(self.action_tokamak_set)

        self.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        self.plotter_process = None
        self.nas_process = None
        self.actionOpen_SXR_file.triggered.connect(self.open_sxr)
        self.actionOpen_HDF5_file.triggered.connect(self.open_hdf5)

        width = QtWidgets.QApplication.desktop().screenGeometry().width()
        height = QtWidgets.QApplication.desktop().screenGeometry().height()
        self.w = int(self.size().width() * width / 1366)
        self.h = int(self.size().height() * height / 768)
        self.resize(self.w, self.h)

        request = packet_init(SystemStatus.ADC, self.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

        win_main = MainWidget(self.centralwidget)
        win_main.channel0.connect(self.channel0)
        win_main.channelSettings.connect(self.settings_from_centralWidget)
        win_main.channelSnapshot.connect(self.channelSnapshot_slot)
        win_main.channelNas.connect(self.open_nas)
        self.channel1.connect(win_main.channel0_slot)
        win_main.channelStart.connect(self.channelStart_slot)
        self.verticalLayout = QtWidgets.QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName("verticalLayout")
        self.verticalLayout.addWidget(win_main)


        logger = Logger(win_main.log_textBrowser, self)
        self.channel1.connect(logger.channel0_slot)

        # self.current_plotter_win = None

        win_main.show()

    @QtCore.pyqtSlot(int)
    def settings_from_centralWidget(self, toOpen: int):
        if toOpen == SystemStatus.ADC:
            self.action_adc_set()
        elif toOpen == SystemStatus.AMP:
            self.action_amplifier_set()
        elif toOpen == SystemStatus.GSA:
            self.action_gsa_set()
        elif toOpen == SystemStatus.TOKAMAK:
            self.action_tokamak_set()
        elif toOpen == SystemStatus.HARDWARE:
            self.action_hardware_set()
        elif toOpen == SystemStatus.JOURNAL:
            self.action_shot_set()

    def action_adc_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('ADC')
        win.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        adcSettings = ADCUIWidget(win)
        adcSettings.channel0.connect(self.channel0)  # make uplink for child widgets
        self.channel1.connect(adcSettings.channel0_slot)  # downlink from system to children

        win.verticalLayout = QtWidgets.QVBoxLayout(win)
        win.verticalLayout.addWidget(adcSettings)

        win.show()
        gc.collect()

        request = packet_init(SystemStatus.ADC, adcSettings.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def action_px5_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('PX-5')
        win.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        px5Settings = PX5Widget(win)
        win.verticalLayout = QtWidgets.QVBoxLayout(win)
        win.verticalLayout.addWidget(px5Settings)

        win.show()
        gc.collect()

    def action_gsa_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('GSA')
        win.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        gsaSettings = GSAWidget(win)
        gsaSettings.channel0.connect(self.channel0)
        self.channel1.connect(gsaSettings.channel0_slot)

        win.verticalLayout = QtWidgets.QVBoxLayout(win)
        win.verticalLayout.addWidget(gsaSettings)

        win.show()
        gc.collect()

        request = packet_init(SystemStatus.GSA, gsaSettings.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def action_amplifier_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('Amplifier')
        win.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        amplifierSettings = AmplifierWidget(win)
        amplifierSettings.channel0.connect(self.channel0)
        self.channel1.connect(amplifierSettings.channel0_slot)

        win.verticalLayout = QtWidgets.QVBoxLayout(win)
        win.verticalLayout.addWidget(amplifierSettings)

        win.show()
        gc.collect()

        request = packet_init(SystemStatus.AMP, amplifierSettings.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def action_hardware_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('Стол')
        win.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        hardwareSettings = HardwareWidget(win)
        hardwareSettings.channel0.connect(self.channel0)
        self.channel1.connect(hardwareSettings.channel0_slot)

        win.verticalLayout = QtWidgets.QVBoxLayout(win)
        win.verticalLayout.addWidget(hardwareSettings)

        win.show()
        gc.collect()

        request = packet_init(SystemStatus.AMP, hardwareSettings.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def action_shot_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('Нумерация')
        win.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        shotSettings = ShotSettings(win)
        shotSettings.channel0.connect(self.channel0)
        self.channel1.connect(shotSettings.channel0_slot)

        win.verticalLayout = QtWidgets.QVBoxLayout(win)
        win.verticalLayout.addWidget(shotSettings)

        win.show()
        gc.collect()

        request = packet_init(SystemStatus.JOURNAL, shotSettings.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def action_tokamak_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('Параметры токамака')
        win.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        tokamakSettings = TokamakWidget(win)
        tokamakSettings.channel0.connect(self.channel0)
        self.channel1.connect(tokamakSettings.channel0_slot)

        win.verticalLayout = QtWidgets.QVBoxLayout(win)
        win.verticalLayout.addWidget(tokamakSettings)

        win.show()
        gc.collect()

        request = packet_init(SystemStatus.TOKAMAK, tokamakSettings.address)
        request.command = Commands.STATUS
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def action_calibration_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('Calibration settings')
        win.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        calibrationSettings = CalibrationSettingsWidget(win)
        win.verticalLayout = QtWidgets.QVBoxLayout(win)
        win.verticalLayout.addWidget(calibrationSettings)

        win.show()

    def action_measurement_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('Measurement settings')
        win.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        measurementSettings = MeasurementSettingsWidget(win)
        win.verticalLayout = QtWidgets.QVBoxLayout(win)
        win.verticalLayout.addWidget(measurementSettings)

        win.show()

    def action_minix2_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('Mini-X2')
        win.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        minix2Settings = MiniX2Widget(win)
        win.verticalLayout = QtWidgets.QVBoxLayout(win)
        win.verticalLayout.addWidget(minix2Settings)

        win.show()

    def action_adclog(self):
        win = QtWidgets.QDialog(self)
        win.setModal(False)
        win.setWindowTitle('ADC feedback')

        adc_log = AdcLog(win)
        adc_logger = ADCLogger(adc_log.textBrowser, self)
        self.channel2.connect(adc_logger.channel2_slot)  # make downlink for BRD_ctrl messages
        adc_log.channel0.connect(self.channel0)  # make uplink for reboot
        win.show()
        gc.collect()

    def open_sxr(self, data_file=None):
        gc.collect(generation=2)
        # win = QtWidgets.QMainWindow(self)
        # win.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        if data_file is None or data_file is False:
            data_file = QtWidgets.QFileDialog.getOpenFileName(self,
                                                              "Select one or more files to open",
                                                              ".",
                                                              "SXR Files (*.h5 *.bin)")[0]

        # if data_file != '':
        #     try:
        #         if self.current_plotter_win is not None:
        #             self.current_plotter_win.close()
        #     except:
        #         pass
        #
        #     win.setWindowTitle('SXR Plotter')
        #     win._main = QtWidgets.QWidget()
        #     win.setCentralWidget(win._main)
        #     layout = QtWidgets.QVBoxLayout(win._main)
        #     self.current_plotter_win = win
        #
        #     sxr_pltSettings = PlotterWidget(data_file=data_file, x_unit='msec')
        #     layout.addWidget(sxr_pltSettings)
        #     win.show()

        if data_file != '':
            try:
                if self.plotter_process is not None:
                    self.plotter_process.terminate()
            except:
                pass
            self.plotter_process = Process(target=PlotterMain, args=(data_file, 'ms'),
                           daemon=True)
            self.plotter_process.start()

            gc.collect(generation=2)

    def open_hdf5(self, data_file=None):
        if data_file is None or data_file is False:
            data_file = QtWidgets.QFileDialog.getOpenFileName(self,
                                                              "Select one or more files to open",
                                                              ".",
                                                              "HDF5 files (*.h5)")[0]
        if data_file != '':
            # os.system(f"{os.path.join(os.getcwd(), 'hdfview', 'HDFview', 'HDFView.exe')} -root "
            #           f"{os.path.split(data_file)[0]} {data_file}")
            subprocess.Popen(f"{os.path.join(os.getcwd(), 'hdfview', 'HDFview', 'HDFView.exe')} -root "
                      f"{os.path.split(data_file)[0]} {data_file}")

    @QtCore.pyqtSlot()
    def open_nas(self):
        try:
            if self.nas_process is not None:
                self.nas_process.terminate()
        except:
            pass
        self.nas_process = Process(target=NasMain, daemon=True)
        self.nas_process.start()
        gc.collect(generation=2)

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        self.channel1.emit(data)

        request = MainPacket()
        request.ParseFromString(data)
        if request.sender == SystemStatus.ADC:
            if request.command == Commands.SNAPSHOT ^ 0xFFFFFFFF:
                if isinstance(request.data.decode('utf-8'), str):
                    # data_file = os.path.join(
                    #     os.path.split(os.path.join(os.path.abspath('./'), request.data.decode('utf-8')))[0],
                    #     'data_0.bin')
                    data_file = request.data.decode('utf-8')
                    self.open_sxr(data_file=data_file)
        elif request.sender == SystemStatus.SXR:
            if request.command == Commands.DONE:
                self.open_sxr(data_file=os.path.join(work_dir(), 'dev', 'insys', 'temp', 'data_0.bin'))

    @QtCore.pyqtSlot()
    def channelStart_slot(self):
        request = packet_init(SystemStatus.SXR, self.address)
        request.command = Commands.START
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    @QtCore.pyqtSlot()
    def channelSnapshot_slot(self):
        request = packet_init(SystemStatus.SXR, self.address)
        request.command = Commands.SNAPSHOT
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

def main():
    app = QtWidgets.QApplication(sys.argv)
    mv = MainWindow()
    mv.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
