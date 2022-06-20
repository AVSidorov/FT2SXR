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

    def __init__(self, parent=None, address=0):
        super().__init__(parent)
        self.address = address
        self.request = MainPacket()
        self.response = MainPacket()
        self.response.address = 0
        self.response.sender = self.address
        self.response.command = Commands.INFO

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
    def __init__(self,parent=None, address=0, state=None):
        super().__init__(parent=parent, address=address)
        self.state = state

    @property
    def name(self):
        if self.address > 0:
            if self.address in SystemStatus.EnumDev.values():
                return SystemStatus.EnumDev.Name(self.address)
        else:
            return None

    def _response(self, response: bool = False, data: bytes = None):
        self.response.address = self.request.sender
        self.response.command = self.request.command ^ 0xFFFFFFFF
        if response:
            if data is not None:
                self.response.data = data
            if self.request.command in Commands.values():
                if self.response.IsInitialized():
                    self.channel0.emit(self.response.SerializeToString())
            # reset request
            # set command to INFO_ACK so response couldn't be emitted
            self.request.command = Commands.INFO ^ 0xFFFFFFFF
        else:
            return data

    def get_status(self, response: bool = False):
        if self.state is not None:
            if hasattr(self.state, 'SerializeToString'):
                if callable(self.state.SerializeToString):
                    self._response(response, self.state.SerializeToString())
        else:
            self._response(response)

    def get_settings(self, response: bool = False):
        self._response(response)

    def set_settings(self, state: MainPacket = None, response: bool = False):
        if response:
            if state is None:
                state = self.request

        if state is None:
            return

        if isinstance(state, MainPacket):
            self.state.ParseFromString(state.data)
        elif isinstance(state, type(self.state)):
            self.state = state

        self._response(response, self.state.SerializeToString())

    def start(self, response: bool = False):
        self._response(response)

    def stop(self, response: bool = False):
        self._response(response)

    def reboot(self, response: bool = False):
        self._response(response)

    def connect(self, response: bool = False):
        self._response(response)

    def snapshot(self, request: MainPacket = None, response: bool = False):
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
            if request.command == Commands.STATUS:
                self.get_status(response=True)
            elif request.command == Commands.SET:
                self.set_settings(response=True)
            elif request.command == Commands.START:
                self.start(response=True)
            elif request.command == Commands.STOP:
                self.stop(response=True)
            elif request.command == Commands.REBOOT:
                self.reboot(response=True)
            elif request.command == Commands.CONNECT:
                self.make_connection(response=True)
            elif request.command == Commands.SNAPSHOT:
                self.snapshot(request, response=True)