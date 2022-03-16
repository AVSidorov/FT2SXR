import sys
from ui.MainWindowUIDesign import Ui_MainWindow
from ui.ADCUI import ADCUIWidget
from PyQt5 import QtWidgets, QtCore
from core.sxr_protocol import packet_init
from core.sxr_protocol_pb2 import MainPacket


class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):

    channel0 = QtCore.pyqtSignal(bytes)  # For uplink
    channel1 = QtCore.pyqtSignal(bytes)  # For downlink

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setupUi(self)
        self.actionADC.triggered.connect(self.action_adc_set)
        self.buttonStart.clicked.connect(self.start_adc)

    def action_adc_set(self):
        adcSettings = ADCUIWidget(self)
        adcSettings.channel0.connect(self.channel0)  # make uplink for child widgets
        self.channel1.connect(adcSettings.channel0_slot)  # downlink from system to children

        win = self.mdiArea.addSubWindow(adcSettings)
        win.show()
        request = packet_init(1, adcSettings.address)
        request.command = 0
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def start_adc(self):
        request = packet_init(1, 0)
        request.command = 2
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        self.channel1.emit(data)


def main():
    app = QtWidgets.QApplication(sys.argv)
    mv = MainWindow()
    mv.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()