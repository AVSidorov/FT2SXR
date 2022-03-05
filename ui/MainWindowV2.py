import sys
from PyQt5 import QtWidgets
from MainWindowUIDesignV2 import Ui_MainWindowDesign


class MainWindow (QtWidgets.QMainWindow, Ui_MainWindowDesign):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        MainWindow.setFixedSize(self, 552, 484)


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = MainWindow()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
