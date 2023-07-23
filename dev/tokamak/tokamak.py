import configparser
import os
import gc
from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, TokamakStatus
from core.fileutils import work_dir


class Tokamak(Dev):
    def __init__(self, parent=None):
        super().__init__(parent, SystemStatus.TOKAMAK, TokamakStatus())

        config = configparser.ConfigParser(inline_comment_prefixes=(';', '//', '#'))
        config.read(os.path.join(work_dir(), os.path.normpath('dev/tokamak/tok_settings.ini')))

        self.state.density = float(config['tokamak']['density'])
        self.state.current = int(config['tokamak']['current'])
        self.state.shotType = int(config['tokamak']['shotType'])
        self.state.power = int(config['tokamak']['power'])

        del config

        gc.collect(generation=2)

    def set_settings(self, state: MainPacket = None, response: bool = False):
        super().set_settings(state, response)
        config = configparser.ConfigParser(inline_comment_prefixes=(';', '//', '#'))
        config.read(os.path.join(work_dir(), os.path.normpath('dev/tokamak/tok_settings.ini')))

        config['tokamak']['density'] = str(self.state.density)
        config['tokamak']['current'] = str(self.state.current)
        config['tokamak']['shotType'] = str(self.state.shotType)
        config['tokamak']['power'] = str(self.state.power)

        with open(os.path.join(work_dir(), os.path.normpath('dev/tokamak/tok_settings.ini')), 'w') as configfile:
            config.write(configfile)

        del config

        gc.collect(generation=2)

    def snapshot(self, request: MainPacket = None, response: bool = False):
        hf, jour = super().snapshot(request, response)
        jour.attrs['name'] = 'Main tokamak parameters'
        for field in self.state.ListFields():
            fname = field[0].name
            val = field[1]
            dset = jour.create_dataset(f'{fname}', data=val, track_times=True)
            jour.attrs[fname] = val
            if fname in ('density',):
                dset.attrs['units'] = 'lines'
            elif fname in ('current',):
                dset.attrs['units'] = 'kA'
            elif fname in ('shotType',):
                dset.attrs['units'] = 'Heating type'

        filename = hf.filename
        hf.close()

        self._response(response, filename.encode())

        return filename