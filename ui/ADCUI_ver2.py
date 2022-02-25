import sys
from PyQt5 import QtWidgets, QtCore
from ADCUI_design_ver2 import Ui_ADC_widget


class ADCUI_widget (QtWidgets.QWidget, Ui_ADC_widget):
    # channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self):
        super().__init__()
        self.setupUi(self)

        # ADC values
        self.channels_status = [0, 0, 0, 0, 0, 0, 0, 0]  # 0/1 or True/False
        self.biases = [0., 0., 0., 0., 0., 0., 0., 0.]
        self.frequency = 0
        self.source = ''
        self.delay = 0
        self.interval = 0

        # service data
        self.active_channels = [1, 2, 3, 4, 5, 6, 7, 8]
        self.enable_all_channels_flag = 0
        self.current_channel = 1

        self.enable_ch()

        # signals
        self.ch1_checkBox.clicked.connect(self.enable_ch)
        self.ch2_checkBox.clicked.connect(self.enable_ch)
        self.ch3_checkBox.clicked.connect(self.enable_ch)
        self.ch4_checkBox.clicked.connect(self.enable_ch)
        self.ch5_checkBox.clicked.connect(self.enable_ch)
        self.ch6_checkBox.clicked.connect(self.enable_ch)
        self.ch7_checkBox.clicked.connect(self.enable_ch)
        self.ch8_checkBox.clicked.connect(self.enable_ch)
        self.frec_spinBox.valueChanged.connect(self.setfreq)
        self.source_comboBox.currentTextChanged.connect(self.setsource)
        self.delay_spinBox.valueChanged.connect(self.setdelay)
        self.interval_spinBox.valueChanged.connect(self.setinterval)
        self.ch_comboBox.currentTextChanged.connect(self.showchannel)
        self.bias_doubleSpinBox.valueChanged.connect(self.setbias)

    def enable_ch(self):
        self.channels_status[0] = bool(self.ch1_checkBox.checkState())
        self.channels_status[1] = bool(self.ch2_checkBox.checkState())
        self.channels_status[2] = bool(self.ch3_checkBox.checkState())
        self.channels_status[3] = bool(self.ch4_checkBox.checkState())
        self.channels_status[4] = bool(self.ch5_checkBox.checkState())
        self.channels_status[5] = bool(self.ch6_checkBox.checkState())
        self.channels_status[6] = bool(self.ch7_checkBox.checkState())
        self.channels_status[7] = bool(self.ch8_checkBox.checkState())

        # make drop-down list
        self.active_channels = []
        for i in range(len(self.channels_status)):
            if self.channels_status[i]:
                self.active_channels.append(i + 1)
        self.ch_comboBox.clear()
        for i in range(len(self.active_channels)):
            self.ch_comboBox.addItem(str(self.active_channels[i]))

        # disable all fields
        self.enable_all_channels_flag = False if sum(self.channels_status) == 0 else True

        self.frec_spinBox.setEnabled(self.enable_all_channels_flag)
        self.source_comboBox.setEnabled(self.enable_all_channels_flag)
        self.delay_spinBox.setEnabled(self.enable_all_channels_flag)
        self.interval_spinBox.setEnabled(self.enable_all_channels_flag)
        self.bias_doubleSpinBox.setEnabled(self.enable_all_channels_flag)
        self.ch_comboBox.setEnabled(self.enable_all_channels_flag)

        # self.channel0.emit

    def setfreq(self):
        self.frequency = int(self.frec_spinBox.value())

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
        self.bias_doubleSpinBox.setValue(self.biases[self.current_channel - 1])

    def setbias(self):
        self.biases[self.current_channel - 1] = self.bias_doubleSpinBox.value()


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = ADCUI_widget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
