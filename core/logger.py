import io
from PyQt5 import QtCore
import datetime
from core import Core


class Logger(Core):
    def __init__(self, out=None, parent=None):
        super().__init__(parent)
        self.out = out


    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        data = data.decode()
        data = f'[{datetime.datetime.now().strftime("%d.%m.%Y %H:%M:%S")}] {self.sender()} '+data
        if data[-1] != '\n':
            data += '\n'

        if self.out is not None:
            if isinstance(self.out, io.TextIOBase):
                self.out.write(data)
            elif isinstance(self.out, str):
                with open(self.out, 'at') as f:
                    f.write(data)



