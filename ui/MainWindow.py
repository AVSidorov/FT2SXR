import sys
from ui.MainWindowDesign import Ui_MainWindow
from ui.ADCUI import ADCUIWidget
from PyQt5 import QtWidgets, QtCore
from core.sxr_protocol import packet_init


class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):

    channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setupUi(self)
        self.actionADC.triggered.connect(self.action_adc_set)

    def action_adc_set(self):
        adcSettings = ADCUIWidget(self)
        adcSettings.channel0.connect(self.channel0)

        win = self.mdiArea.addSubWindow(adcSettings)
        win.show()
        request = packet_init(1, adcSettings.address)
        request.command = 0
        if request.IsInitialized():
              self.channel0.emit(request.SerializeToString())

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        pass

def main():
    app = QtWidgets.QApplication(sys.argv)
    mv = MainWindow()
    mv.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()