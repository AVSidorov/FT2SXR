import sys
from PyQt5 import QtWidgets
from ui.AmplifierUIDesign import Ui_AmplifierWidgetDesign


class AmplifierWidget(QtWidgets.QWidget, Ui_AmplifierWidgetDesign):

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)

        # Amplifier initial values
        self.gainA = 0
        self.gainB = 0
        self.decay = 0

        # signals
        self.gainA_doubleSpinBox.valueChanged.connect(self.setgainA)
        self.gainB_doubleSpinBox.valueChanged.connect(self.setgainB)
        self.decay_doubleSpinBox.valueChanged.connect(self.setdecay)


    def setgainA(self):
        self.gainA = self.gainA_doubleSpinBox.value()

    def setgainB(self):
        self.gainB = self.gainB_doubleSpinBox.value()

    def setdecay(self):
        self.decay = self.decay_doubleSpinBox.value()


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = AmplifierWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
