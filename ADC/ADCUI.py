import sys
from PyQt5 import QtWidgets, QtCore
from ADCUI_design import UI_ADC_widget


class ADCUI_widget (QtWidgets.QWidget, UI_ADC_widget):
    channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self):
        super().__init__()
        self.setupUi(self)

        self.enable_flag = 1
        self.enable_all()

        self.enable_pushButton.clicked.connect(self.enable_all)

    def enable_all(self):
        self.channel0.emit(b'btnckiked')
        if self.enable_flag == 0:
            self.enable_pushButton.setStyleSheet("color: red")
            self.enable_pushButton.text()
            self.frec_spinBox.setEnabled(True)
            self.bias_doubleSpinBox.setEnabled(True)
            self.source_comboBox.setEnabled(True)
            self.delay_spinBox.setEnabled(True)
            self.interval_spinBox.setEnabled(True)
            self.enable_flag = 1
        else:
            self.frec_spinBox.setEnabled(False)
            self.bias_doubleSpinBox.setEnabled(False)
            self.source_comboBox.setEnabled(False)
            self.delay_spinBox.setEnabled(False)
            self.interval_spinBox.setEnabled(False)
            self.enable_pushButton.setStyleSheet("color: black")
            self.enable_flag = 0


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = ADCUI_widget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
