import configparser
import gc
from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, JournalStatus
from core.fileutils import work_dir, today_dir
import os
import time
import datetime


class Journal(Dev):
    def __init__(self, parent=None):
        super().__init__(parent, SystemStatus.JOURNAL, JournalStatus())

        config = configparser.ConfigParser()
        config.read(os.path.join(work_dir(), os.path.normpath('dev/journal/jou_settings.ini')))

        if len(os.listdir(today_dir())) == 0:
            self.state.SXRshot = 0
            self.state.TOKAMAKshot = 0
            timestamp = time.time()
            curtime = datetime.datetime.fromtimestamp(timestamp)
            file = f'SXR{curtime.year - 2000:02d}{curtime.month:02d}{curtime.day:02d}_{str(self.state.SXRshot).zfill(3)}'
            self.state.filename = file
            self.state.comment = ''

            config['journal']['sxrshot'] = str(self.state.SXRshot)
            config['journal']['tokamakshot'] = str(self.state.TOKAMAKshot)
            config['journal']['filename'] = str(self.state.filename)
            config['journal']['comment'] = str(self.state.comment)

            with open(os.path.join(work_dir(), os.path.normpath('dev/journal/jou_settings.ini')), 'w') as configfile:
                config.write(configfile)

            gc.collect(2)

        else:
            self.state.SXRshot = int(config['journal']['sxrshot'])
            self.state.TOKAMAKshot = int(config['journal']['tokamakshot'])
            self.state.filename = config['journal']['filename']
            self.state.comment = config['journal']['comment']

    def set_settings(self, state: MainPacket = None, response: bool = False):
        super().set_settings(state, response)
        config = configparser.ConfigParser()
        config.read(os.path.join(work_dir(), os.path.normpath('dev/journal/jou_settings.ini')))

        config['journal']['sxrshot'] = str(self.state.SXRshot)
        config['journal']['tokamakshot'] = str(self.state.TOKAMAKshot)
        config['journal']['filename'] = str(self.state.filename)
        config['journal']['comment'] = str(self.state.comment)

        with open(os.path.join(work_dir(), os.path.normpath('dev/journal/jou_settings.ini')), 'w') as configfile:
            config.write(configfile)

        gc.collect(2)

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

        self.state.SXRshot += 1
        self.state.TOKAMAKshot += 1
        self.state.comment = ''
        timestamp = time.time()
        curtime = datetime.datetime.fromtimestamp(timestamp)
        file = f'SXR{curtime.year - 2000:02d}{curtime.month:02d}{curtime.day:02d}_{str(self.state.SXRshot).zfill(3)}'
        self.state.filename = file

        config = configparser.ConfigParser()
        config.read(os.path.join(work_dir(), os.path.normpath('dev/journal/jou_settings.ini')))

        config['journal']['sxrshot'] = str(self.state.SXRshot)
        config['journal']['tokamakshot'] = str(self.state.TOKAMAKshot)
        config['journal']['filename'] = str(self.state.filename)
        config['journal']['comment'] = str(self.state.comment)

        with open(os.path.join(work_dir(), os.path.normpath('dev/journal/jou_settings.ini')), 'w') as configfile:
            config.write(configfile)

        gc.collect(2)

        return filename
