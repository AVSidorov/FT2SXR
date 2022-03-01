import sys
from PyQt5 import QtWidgets
from MiniX2UIDesign import Ui_MiniX2WidgetDesign


class MiniX2Widget(QtWidgets.QWidget, Ui_MiniX2WidgetDesign):

    def __init__(self):
        super().__init__()
        self.setupUi(self)
        MiniX2Widget.setFixedSize(self, 300, 170)

        # MiniX2 initial values
        self.enabled = 0  # 0/1 or True/False
        self.voltage = 0
        self.current = 0

        # signals
        self.enable_pushButton.clicked.connect(self.setenabled)
        self.voltage_doubleSpinBox.valueChanged.connect(self.setvoltage)
        self.current_doubleSpinBox.valueChanged.connect(self.setcurrent)

    def setenabled(self):
        if self.enabled == 0:
            self.enabled = 1
            self.enable_pushButton.setText('Enabled')
            self.enable_pushButton.setStyleSheet('color: red')
        else:
            self.enabled = 0
            self.enable_pushButton.setText('Disabled')
            self.enable_pushButton.setStyleSheet('color: black')

    def setvoltage(self):
        self.voltage = self.voltage_doubleSpinBox.value()

    def setcurrent(self):
        self.current = self.current_doubleSpinBox.value()


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = MiniX2Widget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()