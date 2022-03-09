import sys
from PyQt5 import QtWidgets

from ui.MainWindowUIDesignV2 import Ui_MainWindowDesign
from ui.ADCUI import ADCUIWidget
from ui.AmplifierUI import AmplifierWidget
from ui.CalibrationSettingsUI import CalibrationSettingsWidget
from ui.GSAUI import GSAWidget
from ui.MeasurementSettingsUI import MeasurementSettingsWidget
from ui.MiniX2UI import MiniX2Widget
from ui.PX5UI import PX5Widget
from ui.WarningUI import WarningWidget


class MainWindow (QtWidgets.QMainWindow, Ui_MainWindowDesign):
    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        MainWindow.setFixedSize(self, 552, 484)

        # widgets
        self.px5Settings = PX5Widget()
        self.adcSettings = ADCUIWidget()
        self.amplifierSettings = AmplifierWidget()
        self.gsaSettings = GSAWidget()
        self.measurementSettings = MeasurementSettingsWidget()
        self.calibrationSettings = CalibrationSettingsWidget()
        self.minix2Settings = MiniX2Widget()
        self.warningSettings = WarningWidget()  # "Точно хотите выйти?" - вдруг пригодится

        # signals
        self.actionPX_5.triggered.connect(self.action_px5_set)
        self.actionADC.triggered.connect(self.action_adc_set)
        self.actionGSA.triggered.connect(self.action_gsa_set)
        self.actionAmplifier.triggered.connect(self.action_amplifier_set)
        self.actionCalibration_settings.triggered.connect(self.action_calibration_set)
        self.actionMeasurement_settings.triggered.connect(self.action_measurement_set)
        self.actionMini_X2.triggered.connect(self.action_minix2_set)
        self.actionMeasureStatus.triggered.connect(self.switch_calib_measure)
        self.actionCalibStatus.triggered.connect(self.switch_calib_measure)

    def action_px5_set(self):
        self.px5Settings.show()

    def action_adc_set(self):
        self.adcSettings.show()

    def action_gsa_set(self):
        self.gsaSettings.show()

    def action_amplifier_set(self):
        self.amplifierSettings.show()

    def action_calibration_set(self):
        self.calibrationSettings.show()

    def action_measurement_set(self):
        self.measurementSettings.show()

    def action_minix2_set(self):
        self.minix2Settings.show()

    def switch_calib_measure(self):  # Набросок! Криво и косо.
        CalibText = self.actionCalibStatus.text()
        if CalibText.split()[-1] == 'Disable':
            self.actionCalibStatus.setText(CalibText.split()[0] + ' Enable')
            self.actionMeasureStatus.setText(self.actionMeasureStatus.text().split()[0] + ' Disable')
            self.manual_pushButton.setText(self.manual_pushButton.text().split('\n')[0] + '\nCalibration')
            self.periodic_pushButton.setText(self.manual_pushButton.text().split('\n')[0] + '\nCalibration')
            self.external_pushButton.setText(self.manual_pushButton.text().split('\n')[0] + '\nCalibration')
        else:
            self.actionCalibStatus.setText(CalibText.split()[0] + ' Disable')
            self.actionMeasureStatus.setText(self.actionMeasureStatus.text().split()[0] + ' Enable')
            self.manual_pushButton.setText(self.manual_pushButton.text().split('\n')[0] + '\nMeasurement')
            self.periodic_pushButton.setText(self.manual_pushButton.text().split('\n')[0] + '\nMeasurement')
            self.external_pushButton.setText(self.manual_pushButton.text().split('\n')[0] + '\nMeasurement')


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = MainWindow()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
