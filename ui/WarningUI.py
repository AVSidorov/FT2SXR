import sys
from PyQt5 import QtWidgets
from ui.WarningUIDesign import Ui_WarningWidgetDesign


class WarningWidget (QtWidgets.QWidget, Ui_WarningWidgetDesign):
    def __init__(self):
        super().__init__()
        self.setupUi(self)

        WarningWidget.setFixedSize(self, 300, 90)

    def accept(self):
        pass

    def reject(self):
        pass


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = WarningWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()