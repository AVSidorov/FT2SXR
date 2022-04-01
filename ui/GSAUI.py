import sys
from PyQt5 import QtWidgets
from ui.GSAUIDesign import Ui_GSAWidgetDesign


class GSAWidget(QtWidgets.QWidget, Ui_GSAWidgetDesign):
    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)

        # GSA initial values
        self.amplitude = 0

        # signals
        self.amplitude_doubleSpinBox.valueChanged.connect(self.setamplitude)

    def setamplitude(self):
        self.amplitude = self.amplitude_doubleSpinBox.value()


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = GSAWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()