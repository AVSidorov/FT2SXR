from ui.MainWindow import MainWindow
from core.core import Core
from core.logger import Logger
from PyQt5 import QtWidgets
import sys


def main():
    app = QtWidgets.QApplication(sys.argv)

    core = Core(app)

    mw = MainWindow()
    mw.channel0.connect(core.channel0)
    core.channel0.connect(mw.channel0_slot)

    logger = Logger('log.txt', app)
    core.channel0.connect(logger.channel0_slot)

    mw.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
