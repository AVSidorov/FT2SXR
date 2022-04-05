from PyQt5 import QtWidgets
from ui.ADCLogUIDesign import Ui_AdcStatusWin
import sys


class AdcLog (QtWidgets.QWidget, Ui_AdcStatusWin):

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        AdcLog.setFixedSize(self, 1000, 300)
        self.textBrowser.setFontFamily('Courier')


def main():
    app = QtWidgets.QApplication(sys.argv)
    log_win = AdcLog()
    log_win.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()