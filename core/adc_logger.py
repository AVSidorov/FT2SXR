import io
from PyQt5 import QtCore, QtWidgets
import datetime
from .core import Core


class ADCLogger(Core):
    def __init__(self, out=None, parent=None):
        super().__init__(parent)
        self.out = out

    @QtCore.pyqtSlot(bytes)
    def channel2_slot(self, data: bytes):
        head = f'[{datetime.datetime.now().strftime("%d.%m.%Y %H:%M:%S")}] ADC SSH SESSION\n'
        data = head + data.decode('utf-8').replace('\r', '') + '\n\n'

        if self.out is not None:
            if isinstance(self.out, io.TextIOBase):
                self.out.write(data)
            elif isinstance(self.out, str):
                with open(self.out, 'at') as f:
                    f.write(str(data))
            elif isinstance(self.out, QtWidgets.QTextBrowser):
                self.out.append(data)
