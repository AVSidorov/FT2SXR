from PyQt5 import QtCore
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands
from core.sxr_protocol import packet_init
import time
import datetime
import os
import h5py


class Core(QtCore.QObject):
    channel0 = QtCore.pyqtSignal(bytes)
    channel1 = QtCore.pyqtSignal(bytes)
    channel2 = QtCore.pyqtSignal(bytes)

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        pass

    @QtCore.pyqtSlot(bytes)
    def channel1_slot(self, data: bytes):
        pass

    @QtCore.pyqtSlot(bytes)
    def channel2_slot(self, data: bytes):
        pass

    def get_origin_core(self):
        if self.parent() is not None:
            if isinstance(self.parent(), Core):
                return self.parent().get_origin_core()
            else:
                return self
        else:
            return self


class Dev(Core):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.request = MainPacket()
        self.response = MainPacket()
        self.response.sender = self.address

    @property
    def name(self):
        if self.address >= 0:
            return SystemStatus.EnumDev.Name(self.address)
        else:
            return None

    def _response(self, response: MainPacket = None, data: bytes = None):
        if response is not None:
            if data is not None:
                response.data = data
            if response.IsInitialized():
                self.channel0.emit(response.SerializeToString())
        else:
            return data

    def get_status(self, response: MainPacket = None):
        self._response(response)

    def get_settings(self, response: MainPacket = None):
        self._response(response)

    def set_settings(self, response: MainPacket = None):
        self._response(response)

    def start(self, response: MainPacket = None):
        self._response(response)

    def stop(self, response: MainPacket = None):
        self._response(response)

    def reboot(self, response: MainPacket = None):
        self._response(response)

    def connect(self, response: MainPacket = None):
        self._response(response)

    def snapshot(self, request: MainPacket = None, response: MainPacket = None):
        file_origin = None

        timestamp = time.time()
        curtime = datetime.datetime.fromtimestamp(timestamp)
        datetime_str = str(curtime)

        file = f'{self.name}{curtime.year-2000:02d}{curtime.month:02d}{curtime.day:02d}' \
                 f'{curtime.hour:02d}{curtime.minute:02d}{curtime.second:02d}.h5'
        group = f'/{self.name}'

        if isinstance(request, MainPacket):
            file_origin = request.data.decode()

        if isinstance(request, str):
            file_origin = request

        if file_origin is not None:
            if len(file_origin) > 0:
                file_origin = file_origin.replace('?', file)
                file_origin = file_origin.split('@')

                if len(file_origin[-1]) > 0:
                    file = file_origin[-1]

                if len(file_origin) > 1:
                    if len(file_origin[0]) > 1:
                        group = file_origin[0].rstrip('/\\') + group

        if os.path.exists(file):
            hf = h5py.File(file, 'r+')
        else:
            hf = h5py.File(file, 'w')
            hf.create_dataset('timestamp', data=timestamp)
            hf['/'].attrs['timestamp'] = timestamp
            hf.create_dataset('datetime', data=datetime_str)
            hf['/'].attrs['datetime'] = datetime_str

        if group in hf:
            if isinstance(group, str):
                group = hf[group]
        else:
            group = hf.create_group(group)

        if isinstance(group, h5py.Group):
            group.attrs['timestamp'] = timestamp
            group.attrs['datetime'] = datetime_str
            return hf, group
        else:
            return hf, None

    def channel0_slot(self, data: bytes):
        self.request.ParseFromString(data)
        request = self.request
        if request.address == self.address:
            if request.command in Commands.values():
                self.response.address = request.sender
                self.response.command = request.command ^ 0xFFFFFFFF

            if request.command == Commands.STATUS:
                self.get_status(self.response)
            elif request.command == Commands.SET:
                self.set_settings(request, self.response)
            elif request.command == Commands.START:
                self.start(self.response)
            elif request.command == Commands.STOP:
                self.stop(self.response)
            elif request.command == Commands.REBOOT:
                self.reboot(self.response)
            elif request.command == Commands.CONNECT:
                self.make_connection(self.response)
            elif request.command == Commands.SNAPSHOT:
                self.snapshot(request, self.response)