from ui.MainWindow import MainWindow
from ui.dump_plotter import DumpPlotter
from core.core import Core
from core.logger import Logger
from core.adc import ADC
from PyQt5 import QtWidgets
import sys


def main():
    app = QtWidgets.QApplication(sys.argv)

    core = Core(app)

    adc = ADC()
    adc.channel0.connect(core.channel0)
    core.channel0.connect(adc.channel0_slot)
    adc.channel2.connect(adc.channel2)

    mw = MainWindow()
    mw.channel0.connect(core.channel0)
    core.channel0.connect(mw.channel0_slot)

    logger = Logger('log.txt', app)
    core.channel0.connect(logger.channel0_slot)

    plotter = DumpPlotter(app)
    plotter.status = adc.status_message()
    core.channel0.connect(plotter.channel0_slot)

    mw.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
