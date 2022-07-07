from ui.MainWindow import MainWindow
from core.core import Core
from core.netmanagers import NetManagerSimple, Netmanager
from PyQt5 import QtWidgets
import sys


def main():
    app = QtWidgets.QApplication(sys.argv)

    core = Core(app)

    net_manager0 = Netmanager(core, port=5556)
    # connecting to FT2_sxr (adding to clients locally)
    net_manager0.clients[("127.0.0.1", 5555)] = 0
    # and starting send keep alive packets (to get in clients on system side)
    net_manager0.startTimer(30000)

    net_manager_adc = NetManagerSimple(core, port=5558)
    net_manager_adc.channel0.connect(core.channel1)
    net_manager_adc.startTimer(30000)

    mw = MainWindow()
    mw.channel0.connect(core.channel0)          # out Main Packets (commands)
    core.channel0.connect(mw.channel0_slot)     # in Main Packets (commands)
    core.channel1.connect(mw.channel2)          # in BRD_ctrl packets (from exam_adc)

    mw.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
