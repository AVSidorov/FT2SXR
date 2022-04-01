import sys
import signal
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

from PyQt5.QtCore import QCoreApplication
from core.adc_logger import ADCLogger
from core.netmanager_simple import NetManagerSimple


def main():
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    app = QCoreApplication(sys.argv)

    netmng = NetManagerSimple(app)
    logger = ADCLogger(sys.stdout, app)

    netmng.channel0.connect(logger.channel2_slot)

    return app.exec()


if __name__ == '__main__':
    main()
