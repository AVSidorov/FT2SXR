import sys
from PyQt5 import QtWidgets, QtCore, QtGui
from ui.PX5UIDesign import Ui_PX5WidgetDesign
from core.sxr_protocol_pb2 import MainPacket, AdcStatus
from core.sxr_protocol import packet_init


class PX5Widget(QtWidgets.QWidget, Ui_PX5WidgetDesign):
    channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        # PX5 initial values
        # Режим запуска
        self.enabled = None  # 0/1/None
        self.setenable()

        self.startsource = ''
        self.spectrsource = ''
        self.valuetomeasure = ''
        self.colltime = 0
        # Установки для измерения спектров MCA
        self.numberch = 0
        self.gain = 0
        self.flattop = 0
        self.pktime = 0
        self.pileupenabled = 0  # 0/1
        # Выходные сигналы
        self.outputdac = ''
        self.dacoffset = 0
        self.aux1 = ''
        self.aux2 = ''

        # signals
        self.enable_pushButton.clicked.connect(self.setenable)
        # self.hideEvent.connect(self.closeEvent)

        statusbar = QtWidgets.QStatusBar(self)
        statusbar.setMaximumSize(16777215, 22)
        self.verticalLayout.addWidget(statusbar)
        statusbar.show()

    def setenable(self):
        if self.enabled is None:
            self.enabled = 0
            self.settings_toolBox.setItemEnabled(self.settings_toolBox.indexOf(self.mca_page), False)
            self.settings_toolBox.setItemEnabled(self.settings_toolBox.indexOf(self.start_page), False)
        elif self.enabled == 0:
            self.enabled = 1
            self.enable_pushButton.setText('Enable')
            self.enable_pushButton.setStyleSheet('color: red')
            self.settings_toolBox.setItemEnabled(self.settings_toolBox.indexOf(self.mca_page), True)
            self.settings_toolBox.setItemEnabled(self.settings_toolBox.indexOf(self.start_page), True)
        else:
            self.enabled = 0
            self.enable_pushButton.setText('Disable')
            self.enable_pushButton.setStyleSheet('color: black')
            self.settings_toolBox.setItemEnabled(self.settings_toolBox.indexOf(self.mca_page), False)
            self.settings_toolBox.setItemEnabled(self.settings_toolBox.indexOf(self.start_page), False)

    def hideEvent(self, a0: QtGui.QHideEvent) -> None:
        self.close()


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = PX5Widget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
