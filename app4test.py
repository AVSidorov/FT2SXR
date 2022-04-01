from ui.MainWindow import MainWindow
from ui.dump_plotter import DumpPlotter
from core.core import Core
from core.logger import Logger
from core.adc_logger import ADCLogger
from core.adc import ADC
from core.netmanager_simple import NetManagerSimple
from PyQt5 import QtWidgets
import sys


def main():
    app = QtWidgets.QApplication(sys.argv)

    core = Core(app)

    netmanager =NetManagerSimple(app)
    netmanager.channel0.connect(core.channel2)

    adc = ADC()
    adc.channel0.connect(core.channel0)
    core.channel0.connect(adc.channel0_slot)
    # adc.channel2.connect(core.channel2)

    mw = MainWindow()
    mw.channel0.connect(core.channel0)
    core.channel0.connect(mw.channel0_slot)
    core.channel2.connect(mw.channel2)

    logger = Logger('log.txt', app)
    core.channel0.connect(logger.channel0_slot)

    plotter = DumpPlotter(app)
    plotter.status = adc.status_message()
    core.channel0.connect(plotter.channel0_slot)

    mw.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
