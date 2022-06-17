import time
from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands
from core.sxr_protocol import packet_init
from dev.insys.adc import ADC
from dev.amptek.px5 import PX5
from dev.tubl.amplifier import Amplifier
from core.fileutils import today_dir
import os


class Ft2SXR(Dev):
    def __init__(self, parent=None, wdir=None):
        super().__init__(parent, SystemStatus.SXR)
        core = self.get_origin_core()

        if wdir is None:
            self.wdir = today_dir()
        else:
            self.wdir = wdir

        self.request_to_dev = MainPacket()
        self.request_to_dev.sender = self.address

        self.devs_que = list()

        self.adc = ADC(self)
        self.px5 = PX5(self)
        self.amp = Amplifier(self)

        self.state = SystemStatus.IDLE
        self.state = SystemStatus()

        # connect devs to message system
        if core is not None:
            self.channel0.connect(core.channel0)
            core.channel0.connect(self.channel0_slot)

            self.adc.channel0.connect(core.channel0)       # out Main Packets (commands)
            self.adc.channel1.connect(core.channel1)       # out BRD_ctrl packets (from exam_adc)
            core.channel0.connect(self.adc.channel0_slot)  # in Main Packets (commands)

            self.px5.channel0.connect(core.channel0)       # out Main Packets (commands)
            core.channel0.connect(self.px5.channel0_slot)  # in Main Packets (commands)

            self.amp.channel0.connect(core.channel0)       # out Main Packets (commands)
            core.channel0.connect(self.amp.channel0_slot)  # in Main Packets (commands)

        self.state.devs.append(SystemStatus.ADC)
        self.state.devs.append(SystemStatus.AMP)
        self.state.devs.append(SystemStatus.PX5)

    def get_status(self, response: MainPacket = None):
        self._response(response, self.state)

    def set_settings(self, request: MainPacket = None, response: MainPacket = None):
        if isinstance(request, MainPacket):
            request = request.data

        if isinstance(request, SystemStatus):
            self.state = request

    def command_to_devs(self, command: Commands = None, response: MainPacket = None):
        for dev in self.state.devs:
            self.request_to_dev.address = dev
            self.request_to_dev.sender = self.address
            self.request_to_dev.command = command

            if self.request_to_dev.IsInitialized():
                self.channel0.emit(self.request_to_dev.SerializeToString())

        self._response(response)

    def command_to_dev_from_queue(self, command: Commands = None, response: MainPacket = None):
        # Command que have two "params" devs in que and command should be sent.
        # If response from one device coincide with current command in que (in self.request_to_dev packet)
        # next request (command to next device) will be sent

        if command == self.request_to_dev.command:
            if len(self.devs_que) > 0:
                dev = self.devs_que.pop()
                self.request_to_dev.address = dev

                if self.request_to_dev.IsInitialized():
                    self.channel0.emit(self.request_to_dev.SerializeToString())
            else:
                # "Clear command (INFO command require no acknowledgment)
                self.request_to_dev.command = Commands.INFO
                # return response so as full que is processed
                self._response(self.response)

    def start(self, response: MainPacket = None):
        self.command_to_devs(Commands.START)

    def stop(self, response: MainPacket = None):
        self.command_to_devs(Commands.STOP)

    def snapshot(self, request: MainPacket = None, response: MainPacket = None):
        hf, sxr = super().snapshot(f'{self.wdir}/?', response)
        sxr.attrs['name'] = 'SXR diagnostics'
        filename = os.path.abspath(os.path.join(self.wdir, hf.filename))
        hf.close()

        self.request_to_dev.command = Commands.SNAPSHOT
        self.request_to_dev.data = f'/SXR@{filename}'.encode()

        self.devs_que.extend(self.state.devs)
        self.command_to_dev_from_queue(Commands.SNAPSHOT, response)

    def channel0_slot(self, data: bytes):
        # here processed commands
        super().channel0_slot(data)
        # check for responses (ACK)
        if self.request.address == self.address:
            if self.request.command ^ 0xFFFFFFFF in Commands.values():
                self.command_to_dev_from_queue(self.request.command ^ 0xFFFFFFFF, self.response)
                # Command que have two "params" devs in que and command should be sent.
                # If response from one device coincide with current command in que (in self.request_to_dev packet)
                # next request (command to next device) will be sent

