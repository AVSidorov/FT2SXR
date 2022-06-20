import sys
from PyQt5 import QtWidgets, QtCore
from math import trunc
from ui.ADCUIDesign import Ui_ADCWidgetDesign
from core.sxr_protocol_pb2 import MainPacket, AdcStatus, SystemStatus, Commands
from core.sxr_protocol import packet_init


class ADCUIWidget (QtWidgets.QWidget, Ui_ADCWidgetDesign):
    channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)

        self.address = 11

        self.status = AdcStatus()  # Object - message for storage ADC state
        self.status.board_status.add()
        for _ in range(8):
            self.status.board_status[0].channel_status.add()

        self.ch_names = ['' for i in range(8)]
        self.void_ch = 8
        eval(f'self.ch{self.void_ch}_checkBox.setDisabled(True)')
        eval(f'self.ch{self.void_ch}_name_lineEdit.setDisabled(True)')
        eval(f'self.ch8_name_lineEdit.setText("void")')

        # signals
        for ch_n in range(1, 9):
            eval(f'self.ch{ch_n}_checkBox.stateChanged.connect(self.ui2status)')
            eval(f'self.ch{ch_n}_name_lineEdit.textChanged.connect(self.ui2status)')
        self.frec_spinBox.valueChanged.connect(self.ui2status)
        self.source_comboBox.currentIndexChanged.connect(self.ui2status)
        self.delay_spinBox.valueChanged.connect(self.ui2status)
        self.interval_spinBox.valueChanged.connect(self.ui2status)
        self.bias_doubleSpinBox.valueChanged.connect(self.ui2status)

        self.ch_comboBox.currentTextChanged.connect(self.show_channel)

        self.statusbar = QtWidgets.QStatusBar(self)
        self.statusbar.setMaximumSize(16777215, 22)
        self.verticalLayout.addWidget(self.statusbar)
        self.statusbar.show()

        self.ui2status()
        self.status2ui()

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        request = MainPacket()
        request.ParseFromString(data)
        if request.sender == SystemStatus.ADC:  # 1 is reserved address for ADC
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.blockSignals(True)
                self.status = request.data
                self.status2ui()
                self.blockSignals(False)

    def status2ui(self):
        status = self.status

        if status is None:
            return

        if isinstance(status, bytes):
            data = status
            status = AdcStatus()
            status.ParseFromString(data)

        self.status = status

        last_ch = None
        n_ch = 0
        if len(status.board_status) > 0:
            # store ch_comboBox state
            if self.ch_comboBox.count() > 0:
                last_ch = self.ch_comboBox.currentText()
            self.ch_comboBox.clear()
            for ch_n in range(len(status.board_status[0].channel_status)):
                eval(f'self.ch{ch_n+1}_checkBox.setChecked(status.board_status[0].channel_status[ch_n].enabled)')
                if status.board_status[0].channel_status[ch_n].enabled:
                    self.ch_comboBox.addItem(str(ch_n+1))
                    self.show_channel()
                    n_ch += 1

        self.frec_spinBox.blockSignals(True)
        self.frec_spinBox.setValue(int(status.sampling_rate/1e6))
        self.frec_spinBox.blockSignals(False)

        if n_ch > 0:
            max_time = trunc(536870912/(2*status.sampling_rate*n_ch)*1e3)
            self.interval_spinBox.blockSignals(True)
            self.interval_spinBox.setMaximum(trunc(max_time))
            self.interval_val_label.setText(f'0...{max_time}, мс')
            self.interval_spinBox.blockSignals(False)
        else:
            self.interval_spinBox.blockSignals(True)
            self.interval_spinBox.setMaximum(16777215)
            self.interval_val_label.setText(f'0..., мс')
            self.interval_spinBox.blockSignals(False)

        self.interval_spinBox.blockSignals(True)
        self.interval_spinBox.setValue(int(status.samples/status.sampling_rate*1e3))
        self.interval_spinBox.blockSignals(False)

        self.statusbar.showMessage('Connected' if status.connected else 'Disconnected')
        self.statusbar.show()

        self.ch_comboBox.blockSignals(True)
        if status.start == status.SOFTSTART:
            self.source_comboBox.setCurrentIndex(0)
        elif status.start == status.EXTSTART:
            self.source_comboBox.setCurrentIndex(1)
        self.ch_comboBox.blockSignals(False)
        self.ch_comboBox.blockSignals(False)

        self.interval_spinBox.blockSignals(True)
        self.interval_spinBox.setValue(int(status.samples / status.sampling_rate * 1e3))
        self.interval_spinBox.blockSignals(False)

        if self.ch_comboBox.count() > 0:
            if last_ch is not None:
                ind = self.ch_comboBox.findText(last_ch)
                if ind > -1:
                    self.ch_comboBox.setCurrentIndex(ind)
                else:
                    self.ch_comboBox.setCurrentIndex(0)
            else:
                self.ch_comboBox.setCurrentIndex(0)

    def ui2status(self):
        if len(self.status.board_status) < 1:
            self.status.board_status.add()

        self.status.board_status[0].channel_mask = b''
        for ch_n in range(8):
            if len(self.status.board_status[0].channel_status) < ch_n+1:
                self.status.board_status[0].channel_status.add()
            exec(f'self.status.board_status[0].channel_status[ch_n].enabled = self.ch{ch_n+1}_checkBox.isChecked()')

        n_enabled = 0  # void channel
        for ch_n in range(8):
            if self.status.board_status[0].channel_status[ch_n].enabled and self.ch_names[ch_n] != 'void':
                n_enabled += 1
        if n_enabled % 2 != 0:
            print('нечетный')
            self.ch8_checkBox.setChecked(True)
        else:
            print('четный')
            self.ch8_checkBox.setChecked(False)

        self.status.sampling_rate = self.frec_spinBox.value() * int(1e6)

        if self.source_comboBox.currentIndex() == 0:
            self.status.start = self.status.SOFTSTART
        elif self.source_comboBox.currentIndex() == 1:
            self.status.start = self.status.EXTSTART

        self.status.samples = int(self.interval_spinBox.value()/1e3 * self.status.sampling_rate)

        if self.ch_comboBox.count() > 0:
            self.status.board_status[0].channel_status[int(self.ch_comboBox.currentText())-1].bias = self.bias_doubleSpinBox.value()

        for i in range(8):  # channel names
            eval(f'self.ch_names.pop({i})')
            eval(f'self.ch_names.insert({i}, self.ch{i+1}_name_lineEdit.text())')

        self.status2ui()

        request = packet_init(SystemStatus.ADC, self.address)
        request.command = Commands.SET
        if self.status.IsInitialized():
            request.data = self.status.SerializeToString()
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def show_channel(self):
        if self.status is None:
            self.status = AdcStatus()

        if not isinstance(self.status, AdcStatus):
            self.status = AdcStatus()

        if len(self.status.board_status) < 1:
            self.status.board_status.add()

        for ch_n in range(8):
            if len(self.status.board_status[0].channel_status) < ch_n+1:
                self.status.board_status[0].channel_status.add()

        if self.ch_comboBox.count() > 0:
            self.bias_doubleSpinBox.blockSignals(True)
            self.bias_doubleSpinBox.setValue(self.status.board_status[0].channel_status[int(self.ch_comboBox.currentText())-1].bias)
            self.bias_doubleSpinBox.blockSignals(False)


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = ADCUIWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
