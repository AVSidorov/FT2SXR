import sys
import gc
from PyQt5 import QtWidgets, QtCore, QtGui
from math import trunc
from ui.ADCUIDesign import Ui_ADCWidgetDesign
from core.sxr_protocol_pb2 import MainPacket, AdcStatus, SystemStatus, Commands
from core.sxr_protocol import packet_init


class ADCUIWidget (QtWidgets.QWidget, Ui_ADCWidgetDesign):
    channel0 = QtCore.pyqtSignal(bytes)
    channelNext = QtCore.pyqtSignal()

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        self.address = 11
        self.isInPeriodic = False

        self.status = AdcStatus()  # Object - message for storage ADC state
        self.status.board_status.add()
        for _ in range(8):
            self.status.board_status[0].channel_status.add()

        self.ch_names = ['' for i in range(8)]
        self.names_without_void = ['' for i in range(8)]

        # signals
        for ch_n in range(1, 9):
            eval(f'self.ch{ch_n}_checkBox.stateChanged.connect(self.ui2status)')
            eval(f'self.ch{ch_n}_name_lineEdit.textChanged.connect(self.ui2status)')

        self.freq_spinBox.valueChanged.connect(self.ui2status)
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
                try:
                    str_data = request.data.decode()
                    if str_data == 'ADC disconnected':
                        pass
                except UnicodeDecodeError:
                    self.status.ParseFromString(request.data)
                    self.status2ui()
                self.blockSignals(False)

    def status2ui(self):
        status = self.status

        last_ch = None
        n_ch = 0
        if len(status.board_status) > 0:
            # store ch_comboBox state
            if self.ch_comboBox.count() > 0:
                last_ch = self.ch_comboBox.currentText()
            self.ch_comboBox.clear()
            for ch_n in range(len(status.board_status[0].channel_status)):
                eval(f'self.ch{ch_n+1}_checkBox.blockSignals(True)')
                eval(f'self.ch{ch_n+1}_checkBox.setChecked(status.board_status[0].channel_status[ch_n].enabled)')
                eval(f'self.ch{ch_n+1}_checkBox.blockSignals(False)')
                if status.board_status[0].channel_status[ch_n].enabled:
                    self.ch_comboBox.addItem(str(ch_n+1))
                    self.show_channel()
                    n_ch += 1

        self.freq_spinBox.blockSignals(True)
        self.freq_spinBox.setValue(int(status.sampling_rate/1e6))
        self.freq_spinBox.blockSignals(False)

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
        elif status.start == status.EXTSTART and not self.isInPeriodic:
            self.source_comboBox.setCurrentIndex(1)
        elif status.start == status.EXTSTART and self.isInPeriodic:
            self.source_comboBox.setCurrentIndex(2)
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
        for ch in range(8, 0, -1):
            if eval(f'self.ch{ch}_name_lineEdit.text() == "void"') and eval(f'self.ch{ch}_checkBox.isChecked()'):
                # eval(f'self.ch{ch}_checkBox.blockSignals(True)')
                eval(f'self.ch{ch}_checkBox.setChecked(False)')
                # eval(f'self.ch{ch}_checkBox.blockSignals(False)')
                # eval(f'self.ch{ch}_name_lineEdit.blockSignals(True))')
                eval(f'self.ch{ch}_name_lineEdit.setText(self.names_without_void[{ch-1}])')
                # eval(f'self.ch{ch}_name_lineEdit.blockSignals(False))')

        if len(self.status.board_status) < 1:
            self.status.board_status.add()

        self.status.board_status[0].channel_mask = b''
        for ch_n in range(8):
            if len(self.status.board_status[0].channel_status) < ch_n+1:
                self.status.board_status[0].channel_status.add()
            exec(f'self.status.board_status[0].channel_status[ch_n].enabled = self.ch{ch_n+1}_checkBox.isChecked()')

        n_enabled = 0  # void channels
        for ch_n in range(8):
            if self.status.board_status[0].channel_status[ch_n].enabled:
                n_enabled += 1
        # print(n_enabled)

        good_values = (0, 1, 2, 4, 8)
        n2add = 0
        if n_enabled not in good_values:
            round_up = good_values[-1]
            for i in range(len(good_values)):
                if good_values[i] > n_enabled:
                    round_up = good_values[i]
                    break
            n2add = round_up-n_enabled
            self.void_label.setText(f'Добавлено {n2add} дополнительных каналов')

        for i in range(n2add):
            for ch in range(8, 0, -1):
                if not eval(f'self.ch{ch}_checkBox.isChecked()'):
                    eval(f'self.ch{ch}_checkBox.blockSignals(True)')
                    eval(f'self.ch{ch}_checkBox.setChecked(True)')
                    eval(f'self.ch{ch}_checkBox.blockSignals(False)')
                    eval(f'self.ch{ch}_name_lineEdit.blockSignals(True)')
                    eval(f'self.ch{ch}_name_lineEdit.setText("void")')
                    eval(f'self.ch{ch}_name_lineEdit.blockSignals(False)')
                    break

        self.status.sampling_rate = self.freq_spinBox.value() * int(1e6)

        if self.source_comboBox.currentIndex() == 0:
            self.status.start = self.status.SOFTSTART
            self.isInPeriodic = False
        elif self.source_comboBox.currentIndex() == 1:
            self.status.start = self.status.EXTSTART
            self.isInPeriodic = False
        elif self.source_comboBox.currentIndex() == 2:
            self.status.start = self.status.EXTSTART
            self.isInPeriodic = True

        self.status.samples = int(self.interval_spinBox.value()/1e3 * self.status.sampling_rate)

        if self.ch_comboBox.count() > 0:
            self.status.board_status[0].channel_status[int(self.ch_comboBox.currentText())-1].bias = self.bias_doubleSpinBox.value()

        for i in range(8):  # channel names
            eval(f'self.ch_names.pop({i})')
            eval(f'self.ch_names.insert({i}, self.ch{i+1}_name_lineEdit.text())')
            if eval(f'self.ch{i+1}_name_lineEdit.text()') != 'void':
                eval(f'self.names_without_void.pop({i})')
                eval(f'self.names_without_void.insert({i}, self.ch{i + 1}_name_lineEdit.text())')

        # self.status2ui()

        if n_enabled in good_values:
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

    def hideEvent(self, a0: QtGui.QHideEvent) -> None:
        gc.collect()
        self.close()

    def closeEvent(self, a0: QtGui.QCloseEvent) -> None:
        self.channelNext.emit()


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = ADCUIWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
