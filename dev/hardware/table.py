from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, HardwareStatus
from core.fileutils import work_dir
import os
import csv


class Hardware(Dev):
    def __init__(self, parent=None):
        super().__init__(parent, SystemStatus.HARDWARE, HardwareStatus())

        self.state.angle = 0.0
        self.state.vknife = 0.0
        self.state.hknife = 0.0
        self.state.foil = ''
        self.state.diaphragm = ''
        with open(os.path.join(work_dir(), os.path.normpath('dev/hardware/hardware_last.csv')), newline='') as file:
            cal = csv.DictReader(file, delimiter=',')
            last_file = {}
            for i in cal:
                last_file = i
            self.state.angle = float(last_file['angle'])
            self.state.hknife = float(last_file['hknife'])
            self.state.vknife = float(last_file['vknife'])
            self.state.foil = last_file['foil']
            self.state.diaphragm = last_file['diaphragm']

    def snapshot(self, request: MainPacket = None, response: bool = False):
        hf, table = super().snapshot(request, response)
        table.attrs['name'] = 'Diagnostics mechanical system'
        for field in self.state.ListFields():
            fname = field[0].name
            val = field[1]
            dset = table.create_dataset(f'{fname}', data=val, track_times=True)
            table.attrs[fname] = val
            if fname in ('vknife', 'hknife'):
                dset.attrs['units'] = 'mm'
            elif fname in ('angle',):
                dset.attrs['units'] = 'degrees'

        filename = hf.filename
        hf.close()

        self._response(response, filename.encode())

        return filename