from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, GsaStatus
import configparser
import os
import gc
from core.fileutils import work_dir


class Gsa(Dev):
    def __init__(self, parent=None):
        super().__init__(parent, SystemStatus.GSA, GsaStatus())

        config = configparser.ConfigParser(inline_comment_prefixes=(';', '//', '#'))
        config.read(os.path.join(work_dir(), os.path.normpath('dev/tubl/gsa_settings.ini')))

        self.state.amplitude = float(config['gsa']['amplitude'])
        self.state.edge = float(config['gsa']['edge'])
        self.state.frequency = float(config['gsa']['frequency'])

        del config

        gc.collect(2)

    def set_settings(self, state: MainPacket = None, response: bool = False):
        super().set_settings(state, response)
        config = configparser.ConfigParser(inline_comment_prefixes=(';', '//', '#'))
        config.read(os.path.join(work_dir(), os.path.normpath('dev/tubl/gsa_settings.ini')))

        config['gsa']['amplitude'] = str(self.state.amplitude)
        config['gsa']['edge'] = str(self.state.edge)
        config['gsa']['frequency'] = str(self.state.frequency)

        with open(os.path.join(work_dir(), os.path.normpath('dev/tubl/gsa_settings.ini')), 'w') as configfile:
            config.write(configfile)

        del config

        gc.collect(2)

    def snapshot(self, request: MainPacket = None, response: bool = False):
        hf, amp = super().snapshot(request, response)
        amp.attrs['name'] = 'Standard amplitude generator settings'
        for field in self.state.ListFields():
            fname = field[0].name
            val = field[1]
            dset = amp.create_dataset(f'{fname}', data=val, track_times=True)
            amp.attrs[fname] = val
            if fname in ('amplitude',):
                dset.attrs['units'] = 'mV'
            elif fname in ('edge',):
                dset.attrs['units'] = 'ns'
            elif fname in ('frequency',):
                dset.attrs['units'] = 'Hz'

        filename = hf.filename
        hf.close()

        self._response(response, filename.encode())

        return filename