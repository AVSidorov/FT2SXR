import gc
import sys
import os
import csv
from PyQt5 import QtWidgets
from ui.HardwareUIDesign import Ui_HardwareWidgetDesign
from core.sxr_protocol_pb2 import MainPacket, HardwareStatus, SystemStatus, Commands
from core.sxr_protocol import packet_init
from PyQt5 import QtWidgets, QtCore, QtGui


class HardwareWidget(QtWidgets.QWidget, Ui_HardwareWidgetDesign):
    channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)
        self.address = 22

        # Hardware values
        self.hknife = 0.0
        self.vknife = 0.0
        self.foil = ''
        self.diaphragm = ''
        self.angle = 0.0
        self.status = HardwareStatus()
        self.address = 20
        self.foils_list = []
        self.diaphragms_list = []

        self.install_foils_comboBox()
        self.install_diaph_comboBox()

        self.hknife_doubleSpinBox.valueChanged.connect(self.ui2status)
        self.vknife_doubleSpinBox.valueChanged.connect(self.ui2status)
        self.foil_comboBox.currentTextChanged.connect(self.ui2status)
        self.diaph_comboBox.currentTextChanged.connect(self.ui2status)
        self.angle_doubleSpinBox.valueChanged.connect(self.ui2status)
        self.install_pushButton.clicked.connect(self.install_settings)
        self.return_pushButton.clicked.connect(self.return_settings)

        self.ui2status()
        self.status2ui()
        gc.collect()

    def install_foils_comboBox(self):
        with open('ui/foil.txt') as f:
            foils = [name[:-1] for name in f]
            self.foils_list = foils
            self.foil_comboBox.addItems(foils)

    def install_diaph_comboBox(self):
        with open('ui/diaphragm.txt') as f:
            diaphragms = [name[:-1] for name in f]
            self.diaphragms_list = diaphragms
            self.diaph_comboBox.addItems(diaphragms)

    def ui2status(self):
        self.hknife = self.hknife_doubleSpinBox.value()
        self.vknife = self.vknife_doubleSpinBox.value()
        self.foil = self.foil_comboBox.currentText()
        self.diaphragm = self.diaph_comboBox.currentText()
        self.angle = self.angle_doubleSpinBox.value()

        self.status.angle = self.angle
        self.status.hknife = self.hknife
        self.status.vknife = self.vknife
        self.status.foil = self.foil
        self.status.diaphragm = self.diaphragm

    def status2ui(self):
        self.angle = self.status.angle
        self.hknife = self.status.hknife
        self.vknife = self.status.vknife
        self.foil = self.status.foil
        self.diaphragm = self.status.diaphragm

        self.hknife_doubleSpinBox.setValue(self.hknife)
        self.vknife_doubleSpinBox.setValue(self.vknife)
        self.angle_doubleSpinBox.setValue(self.angle)

    def install_settings(self):
        request = packet_init(SystemStatus.HARDWARE, self.address)
        request.command = Commands.SET
        if self.status.IsInitialized():
            request.data = self.status.SerializeToString()
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def return_settings(self):
        request = packet_init(SystemStatus.HARDWARE, self.address)
        request.command = Commands.STATUS
        self.channel0.emit(request.SerializeToString())

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        request = MainPacket()
        request.ParseFromString(data)
        if request.sender == SystemStatus.HARDWARE and request.address == self.address:
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
    ex = HardwareWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
