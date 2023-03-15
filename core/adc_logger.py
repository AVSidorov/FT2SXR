import io
from PyQt5 import QtCore, QtWidgets
from PyQt5.QtGui import QColor
import datetime
from core.core import Core
from dev.insys.bardy.brderr import *
from dev.insys.bardy.getCodesNames import *
from dev.insys.bardy.ctrladc import *
from insys.EXAM.exam_adc.exam_protocol_pb2 import BRD_ctrl


class ADCLogger(Core):
    def __init__(self, out=None, parent=None):
        super().__init__(parent)
        self.out = out

    @QtCore.pyqtSlot(bytes)
    def channel2_slot(self, data: bytes):
        head = f'[{datetime.datetime.now().strftime("%d.%m.%Y %H:%M:%S")}] '
        lvl = None
        # get packet length
        pkt = BRD_ctrl()
        pkt.command = 0
        pkt.out = 0
        pkt.status = 0
        pktSize = pkt.ByteSize()

        if len(data) == pktSize:
            pkt.ParseFromString(data)
            if pkt.IsInitialized():
                data = ''
                # data += head + \
                #         f"{f'cmd: 0x{pkt.command:08X}':<64s}" \
                #         f"{f'out: 0x{pkt.out:08X}':<10s}" \
                #         f"{f'status: 0x{pkt.status:08X}':<s}\n"

                cmd = getCmdName(pkt.command)
                if cmd is None:
                    cmd = f'0x{pkt.command:04X}'

                out = f'0x{pkt.out:04X}'

                status = getErrName(pkt.status)
                if status is None:
                    status = f'0x{pkt.status:04X}'

                data += head + \
                                f'cmd: {cmd:<32}' \
                                f'out: {out:<16}' \
                                f'status: {status}'

                lvl = extrErrLvl(pkt.status)
                if lvl in BRDerrLvl:
                    lvl = f'{BRDerrLvl[lvl]}'
                else:
                    lvl = f'0b{lvl:02b}'

                src = extrErrSrc(pkt.status)
                if src in BRDerrSrc:
                    src = f'from {BRDerrSrc[src]}'
                else:
                    src = f'src unknown 0b{src:03b}'

                code = extrErrCode(pkt.status)
                dev = getBaseName(code)
                code = f'{dev} error code {code & 0x00FF:03d}'
                # data += head + \
                #         f"{lvl:<64s}" \
                #         f"{src:<10s}" \
                #         f"{code:<s}"
            else:
                data = head + ' Bad Packet'
        else:
            data = head + f'data len is {len(data)}'

        if self.out is not None:
            if isinstance(self.out, io.TextIOBase):
                self.out.write(data+'\n')
            elif isinstance(self.out, str):
                with open(self.out, 'at') as f:
                    f.write(str(data)+'\n')
            elif isinstance(self.out, QtWidgets.QTextBrowser):
                if lvl is not None:
                    if lvl in ('Success', 'Information'):
                        self.out.setTextColor(QColor(0x000000))
                    elif lvl == 'Warning':
                        self.out.setTextColor(QColor(0x0000ff))
                    elif lvl == 'Error':
                        self.out.setTextColor(QColor(0xffbb44))
                    elif lvl == 'Fatal Error':
                        self.out.setTextColor(QColor(0xff0000))
                    else:
                        self.out.setTextColor(QColor(0x00ff00))
                self.out.append(data)
