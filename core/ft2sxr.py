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
        super().__init__(parent)
        core = self.get_origin_core()

        self.address = SystemStatus.SXR
        if wdir is None:
            self.wdir = today_dir()
        else:
            self.wdir = wdir

        self.adc = ADC(self)
        self.px5 = PX5(self)
        self.amp = Amplifier(self)

        self.state = SystemStatus.IDLE
        self.state = SystemStatus()
        self.laststart = None

        # connect devs to message system
        if core is not None:
            self.adc.channel0.connect(core.channel0)       # out Main Packets (commands)
            self.adc.channel1.connect(core.channel1)       # out BRD_ctrl packets (from exam_adc)
            core.channel0.connect(self.adc.channel0_slot)  # in Main Packets (commands)

            self.px5.channel0.connect(core.channel0)       # out Main Packets (commands)
            core.channel0.connect(self.px5.channel0_slot)  # in Main Packets (commands)

            self.amp.channel0.connect(core.channel0)       # out Main Packets (commands)
            core.channel0.connect(self.amp.channel0_slot)  # in Main Packets (commands)

    def get_status(self, response: MainPacket = None):
        self._response(response, self.state)
        if response is None:
            return self.state

    def set_settings(self, request: MainPacket = None, response: MainPacket = None):
        if isinstance(request, MainPacket):
            request = request.data

        if isinstance(request, SystemStatus):
            self.state = request

    def start(self, response=None):
        self.laststart = time.time()

        request = packet_init(0, SystemStatus.SXR)

        for dev in self.state.devs:
            request.address = dev
            request.command = Commands.START
            if request.IsInitialized():
                self.channel0.emit(request.SerializeToString())

        if response is not None:
            if response.IsInitialized():
                self.channel0.emit(response.SerializeToString())

    def snapshot(self, request: MainPacket = None, response: MainPacket = None):
        hf, sxr = super().snapshot(f'{self.wdir}/?', response)
        sxr.attrs['name'] = 'SXR diagnostics'
        filename = os.path.abspath(os.path.join(self.wdir, hf.filename))
        hf.close()

        for dev in self.state.devs:
            request.address = dev
            request.command = Commands.SNAPSHOT
            request.data = f'/SXR@{filename}'.encode()
            if request.IsInitialized():
                self.channel0.emit(request.SerializeToString())

        self._response(response, f'/SXR@{filename}'.encode())

        return f'/SXR@{filename}'


