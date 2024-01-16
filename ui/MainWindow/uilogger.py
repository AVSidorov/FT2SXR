import io
from PyQt5 import QtCore, QtWidgets
import datetime
from core.core import Core
from core.sxr_protocol_pb2 import MainPacket, Commands, SystemStatus


class Logger(Core):
    def __init__(self, out=None, parent=None):
        super().__init__(parent)
        self.out = out
        self.prev_cmd = ''
        core = self.get_origin_core()
        if core is not None:
            core.channel0.connect(self.channel0_slot)
        self.address = 55

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        self.request.ParseFromString(data)
        pck = self.request

        head = f'[{datetime.datetime.now().strftime("%d.%m.%Y %H:%M:%S")}] '
        if pck.address in SystemStatus.EnumDev.values():
            dev = SystemStatus.EnumDev.Name(pck.address)
        else:
            dev = pck.address

        if pck.sender in SystemStatus.EnumDev.values():
            sender = SystemStatus.EnumDev.Name(pck.sender)
        else:
            sender = pck.sender

        data = ''

        if pck.command in Commands.values():
            cmd = Commands.Name(pck.command)
            if cmd == 'DONE':
                self.prev_cmd = 'DONE'
                data += f' Данные от {sender} получены'
            elif cmd == 'START' and dev == 'SXR':
                self.prev_cmd = 'START'
                data += f' ЗАПУСК ИЗМЕРЕНИЯ'
            elif cmd == 'STOP' and dev == '16':
                self.prev_cmd = 'STOP'
                data += f' Остановка измерения'
            elif cmd == 'STOP' and sender == 'ADC':
                self.prev_cmd = 'STOP'
                data += f' Остановка {sender}'
            elif cmd == 'SNAPSHOT' and dev == 'SXR':
                self.prev_cmd = 'SHAPSHOT'
                data += f' Данные сохранены'
            elif cmd == 'INFO':
                data += f' {pck.data.decode()}'
        elif pck.command ^ 0xFFFFFFFF in Commands.values():
            cmd = Commands.Name(pck.command ^ 0xFFFFFFFF)
            if cmd == 'SET':
                self.prev_cmd = 'SET'
                data += f' Настройки {sender} установлены'
            # elif cmd == 'STATUS':
            #     data += f' {sender} инициализирован'
            elif cmd == 'REBOOT':
                self.prev_cmd = 'REBOOT'
                data += f' Перезагрузка {sender}'
        else:
            cmd = pck.command

        # data = head + f' To: {dev} From: {sender} Command: {cmd} data length={len(pck.data)}'
        # if pck.command == Commands.INFO:
        #     data += '\n' + head + pck.data.decode()

        # if data[-1] != '\n':
        #     data += '\n'

        if self.out is not None and data != '':
            data = head + data
            if data[-1] != '\n':
                data += '\n'
            if isinstance(self.out, io.TextIOBase):
                self.out.write(data)
            elif isinstance(self.out, str):
                with open(self.out, 'at') as f:
                    f.write(data)
            elif isinstance(self.out, QtWidgets.QTextBrowser):
                self.out.append(data[:-1])


