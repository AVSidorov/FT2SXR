from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, AmpStatus


class Amplifier(Dev):
    def __init__(self, parent = None):
        super().__init__(parent)
        self.address = SystemStatus.AMP

        self.state = AmpStatus()

        self.state.gainA = 0.0
        self.state.gainB = 0.0
        self.state.tail = 0b0110

    def snapshot(self, request: MainPacket = None, response: MainPacket = None):
        hf, amp = super().snapshot(request, response)
        amp.attrs['name'] = 'Shaping amplifier for Amptek FastSDD detector'
        for field in self.state.ListFields():
            fname = field[0].name
            val = field[1]
            dset = amp.create_dataset(f'{fname}', data=val, track_times=True)
            if fname in ('gainA', 'gainB'):
                dset.attrs['units'] = 'wheel units'
            elif fname in ('tail',):
                dset.attrs['units'] = 'switchers position'

        filename = hf.filename
        hf.close()

        self._response(response, filename.encode())

        return filename

    def get_status(self, response: MainPacket = None):
        self._response(response, self.state)
        if response is None:
            return self.state

    def set_settings(self, request: MainPacket = None, response: MainPacket = None):
        if isinstance(request, MainPacket):
            request = request.data

        if isinstance(request, AmpStatus):
            self.state = request




