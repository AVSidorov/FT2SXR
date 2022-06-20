from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, AmpStatus
from core.fileutils import work_dir
import os
import csv


class Amplifier(Dev):
    def __init__(self, parent=None):
        super().__init__(parent, SystemStatus.AMP)

        self.state = AmpStatus()

        self.state.gainA = 0.0
        self.state.gainB = 0.0
        self.state.tail = 0b0000
        with open(os.path.join(work_dir(), 'amp_last.csv'), newline='') as file:
            cal = csv.DictReader(file, delimiter=',')
            last_file = {}
            for i in cal:
                last_file = i
            self.state.gainA = float(last_file['gainA'])
            self.state.gainB = float(last_file['gainB'])
            self.state.tail = int(last_file['switch_state'])

    def snapshot(self, request: MainPacket = None, response: MainPacket = None):
        hf, amp = super().snapshot(request, response)
        amp.attrs['name'] = 'Shaping amplifier for Amptek FastSDD detector'
        for field in self.state.ListFields():
            fname = field[0].name
            val = field[1]
            dset = amp.create_dataset(f'{fname}', data=val, track_times=True)
            amp.attrs[fname] = val
            if fname in ('gainA', 'gainB'):
                dset.attrs['units'] = 'wheel units'
            elif fname in ('tail',):
                dset.attrs['units'] = 'switchers position'

        filename = hf.filename
        hf.close()

        self._response(response, filename.encode())

        return filename

    def get_status(self, response: MainPacket = None):
        self._response(response, self.state.SerializeToString())

    def set_settings(self, request: MainPacket = None, response: MainPacket = None):
        if isinstance(request, MainPacket):
            self.state.ParseFromString(request.data)

        if isinstance(request, AmpStatus):
            self.state = request

        self._response(response, self.state.SerializeToString())

        with open(os.path.join(work_dir(), 'amp_last.csv'), 'w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(('gainA', 'gainB', 'switch_state'))
            writer.writerow((self.state.gainA, self.state.gainB, self.state.tail))
