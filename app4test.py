from ui.MainWindow.MainWindow import MainWindow
from core.core import Core
from core.logger import Logger
from core.ft2sxr import Ft2SXR
from PyQt5 import QtWidgets, QtGui
import sys
import os


def main():
    try:
        # Включите в блок try/except, если вы также нацелены на Mac/Linux
        from PyQt5.QtWinExtras import QtWin  # !!!
        myappid = 'FT2SXR'  # !!!
        QtWin.setCurrentProcessExplicitAppUserModelID(myappid)  # !!!
    except ImportError:
        pass

    app = QtWidgets.QApplication(sys.argv)
    app.setWindowIcon(QtGui.QIcon(os.path.join(os.getcwd(), 'style', 'icons', 'main_icon.png')))
    # app.setStyle('Fusion')
    # app.setStyleSheet(Path('style/Ubuntu.qss').read_text())

    core = Core(app)

    system = Ft2SXR(core)

    mw = MainWindow()
    mw.setWindowIcon(QtGui.QIcon(os.path.join(os.getcwd(), 'style', 'icons', 'main_icon.png')))
    mw.channel0.connect(core.channel0)          # out Main Packets (commands)
    core.channel0.connect(mw.channel0_slot)     # in Main Packets (commands)
    core.channel1.connect(mw.channel2)          # in BRD_ctrl packets (from exam_adc)

    logger = Logger('log.txt', core)

    mw.showMaximized()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
