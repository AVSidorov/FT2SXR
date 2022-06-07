from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands


class Amplifier(Dev):
    def __init__(self, parent = None):
        super().__init__(parent)
        self.address = SystemStatus.AMP

        self.gainA = 0.0
        self.gainB = 0.0
        self.tail = 0b0110

    def snapshot(self, request: MainPacket = None, response: MainPacket = None):
        hf, amp = super().snapshot(request, response)
        amp.attrs['name'] = 'Shaping amplifier for Amptek FastSDD detector'
        dset = amp.create_dataset('gainA', data=self.gainA, track_times=True)
        dset.attrs['units'] = 'wheel units'
        dset = amp.create_dataset('gainB', data=self.gainB, track_times=True)
        dset.attrs['units'] = 'wheel units'
        dset = amp.create_dataset('tail', data=self.tail, track_times=True)
        dset.attrs['units'] = 'switchers position'
        filename = hf.filename
        hf.close()

        self._response(response, filename.encode())

        return filename