from ui.TOKAMAK.TokamakWidgetUIDesign import Ui_tokamakWidget
from core.sxr_protocol_pb2 import MainPacket, TokamakStatus, SystemStatus, Commands
from core.sxr_protocol import packet_init
from PyQt5 import QtWidgets, QtCore, QtGui
import gc
import os


class TokamakWidget(QtWidgets.QWidget, Ui_tokamakWidget):
    channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self, win=None, parent=None):
        curdir = os.getcwd()
        os.chdir(os.path.join(curdir, 'ui', 'ControlPanel'))
        super().__init__(parent=parent)
        self.setupUi(self)
        os.chdir(curdir)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        self.win = win
        self.address = 24
        self.status = TokamakStatus()

        # signals
        self.density_doubleSpinBox.valueChanged.connect(self.setdensity)
        self.current_spinBox.valueChanged.connect(self.setcurrent)
        self.power_spinBox.valueChanged.connect(self.setpower)
        self.mode_comboBox.currentTextChanged.connect(self.setmode)
        self.install_pushButton.clicked.connect(self.install_settings)
        self.return_pushButton.clicked.connect(self.return_settings)
        self.saveclose_pushButton.clicked.connect(self.saveclose)

    def setcurrent(self):
        self.status.current = self.current_spinBox.value()

    def setdensity(self):
        self.status.density = self.density_doubleSpinBox.value()

    def setmode(self):
        if self.mode_comboBox.currentText() == 'OH':
            self.status.shotType = self.status.OH
            self.power_spinBox.setValue(0)
            self.power_spinBox.setDisabled(True)
            self.current_spinBox.setEnabled(True)
            self.density_doubleSpinBox.setEnabled(True)
        elif self.mode_comboBox.currentText() == 'RF':
            self.status.shotType = self.status.RF
            self.power_spinBox.setEnabled(True)
            self.current_spinBox.setEnabled(True)
            self.density_doubleSpinBox.setEnabled(True)
        elif self.mode_comboBox.currentText() == 'Тлеющий':
            self.status.shotType = self.status.GLOW
            self.density_doubleSpinBox.setValue(0.0)
            self.current_spinBox.setValue(0)
            self.power_spinBox.setEnabled(True)
            self.current_spinBox.setDisabled(True)
            self.density_doubleSpinBox.setDisabled(True)
        elif self.mode_comboBox.currentText() == 'OFF':
            self.status.shotType = self.status.OFF
            self.power_spinBox.setValue(0)
            self.density_doubleSpinBox.setValue(0.0)
            self.current_spinBox.setValue(0)
            self.power_spinBox.setDisabled(True)
            self.current_spinBox.setDisabled(True)
            self.density_doubleSpinBox.setDisabled(True)

    def setpower(self):
        self.status.power = self.power_spinBox.value()

    def ui2status(self):
        self.setcurrent()
        self.setdensity()
        self.setpower()
        self.setmode()

    def status2ui(self):
        self.current_spinBox.setValue(self.status.current)
        self.density_doubleSpinBox.setValue(self.status.density)
        self.power_spinBox.setValue(self.status.power)
        if self.status.shotType == self.status.OH:
            self.mode_comboBox.setCurrentText('OH')
        elif self.status.shotType == self.status.RF:
            self.mode_comboBox.setCurrentText('RF')
        elif self.status.shotType == self.status.GLOW:
            self.mode_comboBox.setCurrentText('Тлеющий')
        elif self.status.shotType == self.status.OFF:
            self.mode_comboBox.setCurrentText('OFF')

    def install_settings(self):
        request = packet_init(SystemStatus.TOKAMAK, self.address)
        request.command = Commands.SET
        if self.status.IsInitialized():
            request.data = self.status.SerializeToString()
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def return_settings(self):
        request = packet_init(SystemStatus.TOKAMAK, self.address)
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
        if request.sender == SystemStatus.TOKAMAK and request.address == self.address:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.blockSignals(True)
                self.status.ParseFromString(request.data)
                self.status2ui()
                self.blockSignals(False)

    def hideEvent(self, a0: QtGui.QHideEvent) -> None:
        gc.collect()
        self.close()

