from ui.ADCUI import ADCUIWidget
from core.core import Core
from core.logger import Logger
from PyQt5 import QtWidgets
import sys


def main():
    app = QtWidgets.QApplication(sys.argv)

    core = Core(app)

    ex = ADCUIWidget()
    ex.channel0.connect(core.channel0)

    logger = Logger('log.txt', app)
    core.channel0.connect(logger.channel0_slot)

    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
