from ui.MainWindow import MainWindow
from ui.dump_plotter import DumpPlotter
from core.core import Core
from core.logger import Logger
from core.adc_logger import ADCLogger
from core.ft2sxr import Ft2SXR
from PyQt5 import QtWidgets
import sys


def main():
    app = QtWidgets.QApplication(sys.argv)

    core = Core(app)

    system = Ft2SXR(core)

    mw = MainWindow()
    mw.channel0.connect(core.channel0)          # out Main Packets (commands)
    core.channel0.connect(mw.channel0_slot)     # in Main Packets (commands)
    core.channel1.connect(mw.channel2)          # in BRD_ctrl packets (from exam_adc)

    logger = Logger('log.txt', app)
    core.channel0.connect(logger.channel0_slot)

    # plotter = DumpPlotter(app)
    # plotter.status = system.adc.status_message()
    # core.channel0.connect(plotter.channel0_slot)

    mw.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
