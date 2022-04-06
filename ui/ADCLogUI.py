from PyQt5 import QtWidgets, QtCore
from ui.ADCLogUIDesign import Ui_AdcStatusWin
import sys
from core.sxr_protocol import packet_init


class AdcLog (QtWidgets.QWidget, Ui_AdcStatusWin):
    channel0 = QtCore.pyqtSignal(bytes)  # For uplink

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        AdcLog.setFixedSize(self, 1000, 300)
        self.textBrowser.setFontFamily('Courier')
        self.reboot_pushButton.clicked.connect(self.reboot)

    def reboot(self):
        request = packet_init(1, 0)
        request.command = 4
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())


def main():
    app = QtWidgets.QApplication(sys.argv)
    log_win = AdcLog()
    log_win.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()