import sys
from PyQt5 import QtWidgets
from ui.CalibrationSettingsUIDesign import Ui_CalibrationSettingsWidget


class CalibrationSettingsWidget (QtWidgets.QWidget, Ui_CalibrationSettingsWidget):
    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = CalibrationSettingsWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
