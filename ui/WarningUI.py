import sys
from PyQt5 import QtWidgets
from ui.WarningUIDesign import Ui_WarningWidget


class WarningWidget (QtWidgets.QWidget, Ui_WarningWidget):
    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)

    def accept(self):
        pass

    def reject(self):
        pass


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = WarningWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()