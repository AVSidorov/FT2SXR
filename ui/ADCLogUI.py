from PyQt5 import QtWidgets, QtCore
from ui.ADCLogUIDesign import Ui_AdcStatusWin


class AdcLog (QtWidgets.QWidget, Ui_AdcStatusWin):

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)

        