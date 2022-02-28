import sys
from ui.MainWindowDesign import Ui_MainWindow
from ui.ADCUI import ADCUIWidget
from PyQt5 import QtWidgets


class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setupUi(self)
        self.actionADC.triggered.connect(self.action_adc_set)

    def action_adc_set(self):
        win = self.mdiArea.addSubWindow(ADCUIWidget())
        win.show()

def main():
    app = QtWidgets.QApplication(sys.argv)
    mv = MainWindow()
    mv.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()