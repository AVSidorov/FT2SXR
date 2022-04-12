from core.core import Core
from core.sxr_protocol_pb2 import SystemStatus, MainPacket
from core.adc import ADC
from dev.px5.px5 import PX5


class Ft2SXR(Core):
    def __init__(self, parent=None):
        super().__init__(parent)
        core = None
        if parent is not None:
            if isinstance(parent, Core):
                core = parent

        self.address = 0
        self.adc = ADC(self)
        self.px5 = PX5(self)
        self.state = SystemStatus.IDLE
        self.devs = list().append(self.adc)

        if core is not None:
            self.adc.channel0.connect(core.channel0)       # out Main Packets (commands)
            self.adc.channel1.connect(core.channel1)       # out BRD_ctrl packets (from exam_adc)
            core.channel0.connect(self.adc.channel0_slot)  # in Main Packets (commands)

    def status_message(self, response=None):
        status = SystemStatus()
        status.state = self.state
        for dev in self.devs:
            if dev == self.adc:
                status.devs.append(SystemStatus.ADC)
            elif dev == self.px5:
                status.devs.append(SystemStatus.PX5)

    def status_to_config(self, status, response=None):
        self.devs = list()
        for dev in status:
            if dev == SystemStatus.ADC:
                self.devs.append(self.adc)
            elif dev == SystemStatus.PX5:
                self.devs.append(self.px5)

        if response is not None:
            self.status_message(response)

    def start(self, response=None):
        for dev in self.devs:
            dev.start(response)
