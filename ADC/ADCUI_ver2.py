import sys
from PyQt5 import QtWidgets, QtCore
from ADCUI_design_ver2 import UI_ADC_widget


class ADCUI_widget (QtWidgets.QWidget, UI_ADC_widget):
    # channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.enable_ch_list = [0, 0, 0, 0, 0, 0, 0, 0]
        self.on_flag = 0
        self.enable_ch()

        self.ch1_checkBox.clicked.connect(self.enable_ch)
        self.ch2_checkBox.clicked.connect(self.enable_ch)
        self.ch3_checkBox.clicked.connect(self.enable_ch)
        self.ch4_checkBox.clicked.connect(self.enable_ch)
        self.ch5_checkBox.clicked.connect(self.enable_ch)
        self.ch6_checkBox.clicked.connect(self.enable_ch)
        self.ch7_checkBox.clicked.connect(self.enable_ch)
        self.ch8_checkBox.clicked.connect(self.enable_ch)

    def enable_ch(self):
        self.enable_ch_list[0] = bool(self.ch1_checkBox.checkState())
        self.enable_ch_list[1] = bool(self.ch2_checkBox.checkState())
        self.enable_ch_list[2] = bool(self.ch3_checkBox.checkState())
        self.enable_ch_list[3] = bool(self.ch4_checkBox.checkState())
        self.enable_ch_list[4] = bool(self.ch5_checkBox.checkState())
        self.enable_ch_list[5] = bool(self.ch6_checkBox.checkState())
        self.enable_ch_list[6] = bool(self.ch7_checkBox.checkState())
        self.enable_ch_list[7] = bool(self.ch8_checkBox.checkState())

        self.bias1_doubleSpinBox.setEnabled(self.enable_ch_list[0])
        self.bias2_doubleSpinBox.setEnabled(self.enable_ch_list[1])
        self.bias3_doubleSpinBox.setEnabled(self.enable_ch_list[2])
        self.bias4_doubleSpinBox.setEnabled(self.enable_ch_list[3])
        self.bias5_doubleSpinBox.setEnabled(self.enable_ch_list[4])
        self.bias6_doubleSpinBox.setEnabled(self.enable_ch_list[5])
        self.bias7_doubleSpinBox.setEnabled(self.enable_ch_list[6])
        self.bias8_doubleSpinBox.setEnabled(self.enable_ch_list[7])

        self.on_flag = False if sum(self.enable_ch_list) == 0 else True

        self.frec_spinBox.setEnabled(self.on_flag)
        self.source_comboBox.setEnabled(self.on_flag)
        self.delay_spinBox.setEnabled(self.on_flag)
        self.interval_spinBox.setEnabled(self.on_flag)

        # self.channel0.emit(b'btnclicked')


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = ADCUI_widget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
