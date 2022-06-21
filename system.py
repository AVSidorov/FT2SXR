from core.core import Core
from core.logger import Logger
from core.ft2sxr import Ft2SXR
from core.netmanagers import NetManagerSimple, Netmanager
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

    net_manager0 = Netmanager(core, port=5555)

    # manager for ADC data
    net_manager_adc = NetManagerSimple(core, port=5557)
    core.channel1.connect(net_manager_adc.channel0_slot)
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
