import sys
from ui.MainWindowUIDesign import Ui_MainWindow
from ui.CentralWidgetUI import MainWidget
from ui.ADCUI import ADCUIWidget
from ui.GSAUI import GSAWidget
from ui.PX5UI import PX5Widget
from ui.AmplifierUI import AmplifierWidget
from ui.MeasurementSettingsUI import MeasurementSettingsWidget
from ui.CalibrationSettingsUI import CalibrationSettingsWidget
from ui.MiniX2UI import MiniX2Widget
from ui.WarningUI import WarningWidget
from PyQt5 import QtWidgets, QtCore
from core.sxr_protocol import packet_init
from core.sxr_protocol_pb2 import MainPacket


class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):

    channel0 = QtCore.pyqtSignal(bytes)  # For uplink
    channel1 = QtCore.pyqtSignal(bytes)  # For downlink

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.actionADC.triggered.connect(self.action_adc_set)
        self.actionPX_5.triggered.connect(self.action_px5_set)
        self.actionGSA.triggered.connect(self.action_gsa_set)
        self.actionAmplifier.triggered.connect(self.action_amplifier_set)
        self.actionCalibration_settings.triggered.connect(self.action_calibration_set)
        self.actionMeasurement_settings.triggered.connect(self.action_measurement_set)
        self.actionMini_X2.triggered.connect(self.action_minix2_set)
        # self.actionMeasureStatus.triggered.connect(self.switch_calib_measure)
        # self.actionCalibStatus.triggered.connect(self.switch_calib_measure)

        win_main = MainWidget(self.centralwidget)
        win_main.channel0.connect(self.channel0)
        self.channel1.connect(win_main.channel0_slot)
        win_main.show()

    def action_adc_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('ADC')

        adcSettings = ADCUIWidget(win)
        adcSettings.channel0.connect(self.channel0)  # make uplink for child widgets
        self.channel1.connect(adcSettings.channel0_slot)  # downlink from system to children

        win.show()
        request = packet_init(1, adcSettings.address)
        request.command = 0
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def action_px5_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('PX-5')

        px5Settings = PX5Widget(win)
        win.show()

    def action_gsa_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('GSA')

        gsaSettings = GSAWidget(win)
        win.show()

    def action_amplifier_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('Amplifier')

        amplifierSettings = AmplifierWidget(win)
        win.show()

    def action_calibration_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('Calibration settings')

        calibrationSettings = CalibrationSettingsWidget(win)
        win.show()

    def action_measurement_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('Measurement settings')

        measurementSettings = MeasurementSettingsWidget(win)
        win.show()

    def action_minix2_set(self):
        win = QtWidgets.QDialog(self)
        win.setModal(True)
        win.setWindowTitle('Mini-X2')

        minix2Settings = MiniX2Widget(win)
        win.show()

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        self.channel1.emit(data)


def main():
    app = QtWidgets.QApplication(sys.argv)
    mv = MainWindow()
    mv.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()