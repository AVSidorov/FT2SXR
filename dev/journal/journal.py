from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, JournalStatus
from core.fileutils import work_dir
import os
import csv


class Journal(Dev):
    def __init__(self, parent=None):
        super().__init__(parent, SystemStatus.JOURNAL, JournalStatus())

        self.state.SXRshot = 0
        self.state.TOKAMAKshot = 0
        self.state.filename = ''
        self.state.comment = ''

    def snapshot(self, request: MainPacket = None, response: bool = False):
        hf, jour = super().snapshot(request, response)
        jour.attrs['name'] = 'Journal of experiment'
        for field in self.state.ListFields():
            fname = field[0].name
            val = field[1]
            dset = jour.create_dataset(f'{fname}', data=val, track_times=True)
            jour.attrs[fname] = val
            if fname in ('SXRshot', 'TOKAMAKshot'):
                dset.attrs['units'] = 'number of shot'

        filename = hf.filename
        hf.close()

        self._response(response, filename.encode())

        return filename
