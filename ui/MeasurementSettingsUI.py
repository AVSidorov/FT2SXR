import sys
from PyQt5 import QtWidgets
from ui.MeasurementSettingsUIDesign import Ui_MeasurementSettingsWidgetDesign


class MeasurementSettingsWidget(QtWidgets.QWidget, Ui_MeasurementSettingsWidgetDesign):

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        MeasurementSettingsWidget.setFixedSize(self, 300, 155)


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = MeasurementSettingsWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
