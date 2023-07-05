from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, AmpStatus
import configparser
import os
import gc
from core.fileutils import work_dir


class Amplifier(Dev):
    def __init__(self, parent=None):
        super().__init__(parent, SystemStatus.AMP, AmpStatus())

        config = configparser.ConfigParser()
        config.read(os.path.join(work_dir(), os.path.normpath('dev/tubl/amp_settings.ini')))

        self.state.gainA = float(config['amp']['gaina'])
        self.state.gainB = float(config['amp']['gainb'])
        self.state.tail = eval(config['amp']['switchstate'])

        gc.collect(2)

    def set_settings(self, state: MainPacket = None, response: bool = False):
        super().set_settings(state, response)
        config = configparser.ConfigParser()
        config.read(os.path.join(work_dir(), os.path.normpath('dev/tubl/amp_settings.ini')))

        config['amp']['gaina'] = str(self.state.gainA)
        config['amp']['gainb'] = str(self.state.gainB)
        config['amp']['switchstate'] = bin(self.state.tail)

        with open(os.path.join(work_dir(), os.path.normpath('dev/tubl/amp_settings.ini')), 'w') as configfile:
            config.write(configfile)

        gc.collect(2)

    def snapshot(self, request: MainPacket = None, response: bool = False):
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

