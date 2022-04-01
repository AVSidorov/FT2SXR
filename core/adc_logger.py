import io
from PyQt5 import QtCore, QtWidgets
from PyQt5.QtGui import QColor
import datetime
from core.core import Core
from dev.insys.bardy.brderr import *
from dev.insys.bardy.getCodesNames import *
from dev.insys.bardy.ctrladc import *
from core.exam_protocol_pb2 import BRD_ctrl


class ADCLogger(Core):
    def __init__(self, out=None, parent=None):
        super().__init__(parent)
        self.out = out

    @QtCore.pyqtSlot(bytes)
    def channel2_slot(self, data: bytes):
        head = f'[{datetime.datetime.now().strftime("%d.%m.%Y %H:%M:%S")}]'

        lvl = None
        if len(data) == 15:
            pkt = BRD_ctrl()
            pkt.ParseFromString(data)
            if pkt.IsInitialized():
                cmd = getCmdName(pkt.command)
                if cmd is None:
                    cmd = f'0x{pkt.command:04X}'
                out = f'0x{pkt.out:X}'

                if pkt.status in BRDerr:
                    status = BRDerr[pkt.status]
                elif pkt.status in BRDerr_ADC:
                    status = BRDerr_ADC[pkt.status]
                else:
                    status = f'0x{pkt.status:X}'

                brd_str = f'cmd:{cmd} ' \
                          f'out: {out} ' \
                          f'status: {status}\n'

                lvl = extrErrLvl(pkt.status)
                src = extrErrSrc(pkt.status)
                code = extrErrCode(pkt.status)
                dev = getBaseName(code)

                if lvl in BRDerrLvl:
                    status += f' {BRDerrLvl[lvl]} '
                if src in BRDerrSrc:
                    status += f' from {BRDerrSrc[src]} '
                status += f' {dev} error code 0x{code:04X}'

                data = brd_str + status
            else:
                data = 'Bad Packet'
        else:
            head += 'ADC SSH SESSION\n'
            data = data.decode('utf-8').replace('\r', '')
        data = head + data
        if self.out is not None:
            if isinstance(self.out, io.TextIOBase):
                self.out.write(data+'\n')
            elif isinstance(self.out, str):
                with open(self.out, 'at') as f:
                    f.write(str(data)+'\n')
            elif isinstance(self.out, QtWidgets.QTextBrowser):
                if lvl is not None:
                    if lvl == 2:
                        self.out.setTextColor(QColor(0x0000ff))
                    elif lvl == 3:
                        self.out.setTextColor(QColor(0xff0000))

                self.out.append(data)
                self.out.setTextColor(QColor(0))
