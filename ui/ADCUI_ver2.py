import sys
from PyQt5 import QtWidgets, QtCore
from ADCUI_design_ver2 import Ui_ADC_widget


class ADCUI_widget (QtWidgets.QWidget, Ui_ADC_widget):
    # channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.ch_list = [1, 2, 3, 4, 5, 6, 7, 8]
        self.enable_ch_list = [0, 0, 0, 0, 0, 0, 0, 0]
        self.enable_all_flag = 0
        self.enable_ch()
        self.bias_list = [0., 0., 0., 0., 0., 0., 0., 0.]
        self.current_channel = 1
        self.frec = 0
        self.source = ''
        self.delay = 0
        self.interval = 0

        self.ch1_checkBox.clicked.connect(self.enable_ch)
        self.ch2_checkBox.clicked.connect(self.enable_ch)
        self.ch3_checkBox.clicked.connect(self.enable_ch)
        self.ch4_checkBox.clicked.connect(self.enable_ch)
        self.ch5_checkBox.clicked.connect(self.enable_ch)
        self.ch6_checkBox.clicked.connect(self.enable_ch)
        self.ch7_checkBox.clicked.connect(self.enable_ch)
        self.ch8_checkBox.clicked.connect(self.enable_ch)
        self.frec_spinBox.valueChanged.connect(self.setfrec)
        self.source_comboBox.currentTextChanged.connect(self.setsource)
        self.delay_spinBox.valueChanged.connect(self.setdelay)
        self.interval_spinBox.valueChanged.connect(self.setinterval)
        self.ch_comboBox.currentTextChanged.connect(self.showchannel)
        self.bias_doubleSpinBox.valueChanged.connect(self.setbias)

    def enable_ch(self):
        self.enable_ch_list[0] = bool(self.ch1_checkBox.checkState())
        self.enable_ch_list[1] = bool(self.ch2_checkBox.checkState())
        self.enable_ch_list[2] = bool(self.ch3_checkBox.checkState())
        self.enable_ch_list[3] = bool(self.ch4_checkBox.checkState())
        self.enable_ch_list[4] = bool(self.ch5_checkBox.checkState())
        self.enable_ch_list[5] = bool(self.ch6_checkBox.checkState())
        self.enable_ch_list[6] = bool(self.ch7_checkBox.checkState())
        self.enable_ch_list[7] = bool(self.ch8_checkBox.checkState())

        self.ch_list = []
        for i in range(len(self.enable_ch_list)):
            if self.enable_ch_list[i]:
                self.ch_list.append(i+1)
        self.ch_comboBox.clear()
        for i in range(len(self.ch_list)):
            self.ch_comboBox.addItem(str(self.ch_list[i]))

        self.enable_all_flag = False if sum(self.enable_ch_list) == 0 else True

        self.frec_spinBox.setEnabled(self.enable_all_flag)
        self.source_comboBox.setEnabled(self.enable_all_flag)
        self.delay_spinBox.setEnabled(self.enable_all_flag)
        self.interval_spinBox.setEnabled(self.enable_all_flag)
        self.bias_doubleSpinBox.setEnabled(self.enable_all_flag)
        self.ch_comboBox.setEnabled(self.enable_all_flag)

        # self.channel0.emit
    def setfrec(self):
        self.frec = int(self.frec_spinBox.value())

    def setsource(self):
        self.source = self.source_comboBox.currentText()

    def setdelay(self):
        self.delay = self.delay_spinBox.value()
        print(self.delay)

    def setinterval(self):
        self.interval = self.interval_spinBox.value()
        print(self.interval)

    def showchannel(self):
        if self.ch_comboBox.currentText() != '':
            self.current_channel = int(self.ch_comboBox.currentText())
        self.bias_doubleSpinBox.setValue(self.bias_list[self.current_channel-1])

    def setbias(self):
        self.bias_list[self.current_channel-1] = self.bias_doubleSpinBox.value()


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = ADCUI_widget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
