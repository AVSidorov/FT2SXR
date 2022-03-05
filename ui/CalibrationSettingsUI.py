import sys
from PyQt5 import QtWidgets
from CalibrationSettingsUIDesign import Ui_CalibrationSettingsWidgetDesign


class CalibrationSettingsWidget (QtWidgets.QWidget, Ui_CalibrationSettingsWidgetDesign):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        CalibrationSettingsWidget.setFixedSize(self, 300, 95)


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = CalibrationSettingsWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
