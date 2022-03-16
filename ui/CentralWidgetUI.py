from PyQt5 import QtWidgets
from ui.CentralWidgetUIDesign import Ui_MainWidgetDesign


class MainWidget(QtWidgets.QWidget, Ui_MainWidgetDesign):
    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        MainWidget.setFixedSize(self, 553, 462)
