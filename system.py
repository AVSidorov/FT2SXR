from core.core import Core
from core.logger import Logger
from core.ft2sxr import Ft2SXR
from core.netmanagers import NetManagerSimple, NetManager
from PyQt5.QtCore import QCoreApplication
import sys
import signal


def main():
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    app = QCoreApplication(sys.argv)

    core = Core(app)

    system = Ft2SXR(core)
    print('system ready')

    logger = Logger('log.txt', core)

    std_out = Logger(sys.stdout, core)

    # manager for main data (channel 0)
    net_manager0 = NetManager(core, port=5555)
    net_manager0.startTimer(15000)

    # manager for ADC data (channel 1)
    net_manager_adc = NetManager(core, port=5557, channel=1)
    net_manager_adc.startTimer(15000)

    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
