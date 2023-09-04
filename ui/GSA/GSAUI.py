import sys
from PyQt5 import QtWidgets, QtCore, QtGui
import gc
import os
from ui.GSA.GSAUIDesign import Ui_GSAWidgetDesign
from core.sxr_protocol_pb2 import MainPacket, GsaStatus, SystemStatus, Commands
from core.sxr_protocol import packet_init


class GSAWidget(QtWidgets.QWidget, Ui_GSAWidgetDesign):
    channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self, win=None, parent=None):
        curdir = os.getcwd()
        os.chdir(os.path.join(curdir, 'ui', 'ControlPanel'))
        super().__init__(parent=parent)
        self.setupUi(self)
        os.chdir(curdir)

        self.win = win
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)
        self.address = 23

        # GSA initial values
        self.amplitude = 0.0
        self.edge = 0.0
        self.frequency = 0.0
        self.amplitudes_list = []
        self.status = GsaStatus()

        self.install_amplitudes_comboBox()

        # signals
        self.amp_comboBox.currentTextChanged.connect(self.setamplitude)
        self.edge_doubleSpinBox.valueChanged.connect(self.setedge)
        self.frequency_doubleSpinBox.valueChanged.connect(self.setfrequency)
        self.install_pushButton.clicked.connect(self.install_settings)
        self.return_pushButton.clicked.connect(self.return_settings)
        self.saveclose_pushButton.clicked.connect(self.saveclose)

    def setamplitude(self):
        self.amplitude = float(self.amp_comboBox.currentText())
        self.status.amplitude = self.amplitude

    def setedge(self):
        self.edge = self.edge_doubleSpinBox.value()
        self.status.edge = self.edge

    def setfrequency(self):
        self.frequency = self.frequency_doubleSpinBox.value()
        self.status.frequency = self.frequency

    def install_amplitudes_comboBox(self):
        with open('ui/GSA/amplitudes.txt') as f:
            amplitudes = [name[:-1] for name in f]
            self.amplitudes_list = amplitudes
            self.amp_comboBox.addItems(amplitudes)

    def ui2status(self):
        self.status.amplitude = self.amplitude
        self.status.edge = self.edge
        self.status.frequency = self.frequency

    def status2ui(self):
        self.edge_doubleSpinBox.setValue(self.status.edge)
        self.frequency_doubleSpinBox.setValue(self.status.frequency)
        self.amp_comboBox.setCurrentText(str(round(float(self.status.amplitude), 1)))

        self.amplitude = self.status.amplitude
        self.edge = self.status.edge
        self.frequency = self.status.frequency

    def install_settings(self):
        request = packet_init(SystemStatus.GSA, self.address)
        request.command = Commands.SET
        if self.status.IsInitialized():
            request.data = self.status.SerializeToString()
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def return_settings(self):
        request = packet_init(SystemStatus.GSA, self.address)
        request.command = Commands.STATUS
        self.channel0.emit(request.SerializeToString())

    def saveclose(self):
        self.install_settings()
        if self.win is not None:
            self.win.close()
        else:
            self.hide()

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        request = MainPacket()
        request.ParseFromString(data)
        if request.sender == SystemStatus.GSA and request.address == self.address:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.blockSignals(True)
                self.status.ParseFromString(request.data)
                self.status2ui()
                self.blockSignals(False)

    def hideEvent(self, a0: QtGui.QHideEvent) -> None:
        gc.collect()
        self.close()


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = GSAWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()