import sys
from PyQt5 import QtWidgets
from WarningMiniX2UIDesign import Ui_WarningMiniX2WidgetDesign


class WaningMiniX2UI (QtWidgets.QWidget, Ui_WarningMiniX2WidgetDesign):
    def __init__(self):
        super().__init__()
        self.setupUi(self)

        WaningMiniX2UI.setFixedSize(self, 281, 90)

    def accept(self):
        self.close()

    def reject(self):
        pass


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = WaningMiniX2UI()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()