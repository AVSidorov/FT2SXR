import sys
import signal
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

from PyQt5.QtWidgets import QApplication
from core.adc_logger import ADCLogger
from core.netmanager_simple import NetManagerSimple
from ui.ADCLogger.ADCLogUI import AdcLog


def main():
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    app = QApplication(sys.argv)

    adc_log = AdcLog()

    netmng = NetManagerSimple(app)
    logger = ADCLogger(adc_log.textBrowser, app)

    netmng.channel0.connect(logger.channel2_slot)
    adc_log.show()

    return sys.exit(app.exec_())


if __name__ == '__main__':
    main()
