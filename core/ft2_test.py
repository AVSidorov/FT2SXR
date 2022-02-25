import sys
import signal

from PyQt5.QtCore import QCoreApplication
from console import Console
from logger import Logger
from core import Core


def main():
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    app = QCoreApplication(sys.argv)

    core = Core(app)

    console = Console(app)
    console.channel0.connect(core.channel0)

    logger = Logger('log.txt', app)
    core.channel0.connect(logger.channel0_slot)

    echo = Logger(sys.stdout, app)
    core.channel0.connect(echo.channel0_slot)

    core.channel0.connect(console.channel0_slot)
    console.run()

    app.exec()
