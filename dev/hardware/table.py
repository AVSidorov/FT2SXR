import configparser
from core.core import Dev
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, HardwareStatus
from core.fileutils import work_dir
import os
import gc


class Hardware(Dev):
    def __init__(self, parent=None):
        super().__init__(parent, SystemStatus.HARDWARE, HardwareStatus())

        config = configparser.ConfigParser(inline_comment_prefixes=(';', '//', '#'))
        config.read(os.path.join(work_dir(), os.path.normpath('dev/hardware/hard_settings.ini')))

        self.state.angle = float(config['hardware']['angle'])
        self.state.vknife = float(config['hardware']['vknife'])
        self.state.hknife = float(config['hardware']['hknife'])
        self.state.foil = config['hardware']['foil']
        self.state.diaphragm = config['hardware']['diaphragm']

        del config

        gc.collect(generation=2)
        # with open(os.path.join(work_dir(), os.path.normpath('dev/hardware/hardware_last.csv')), newline='') as file:
        #     cal = csv.DictReader(file, delimiter=',')
        #     last_file = {}
        #     for i in cal:
        #         last_file = i
        #     self.state.angle = float(last_file['angle'])
        #     self.state.hknife = float(last_file['hknife'])
        #     self.state.vknife = float(last_file['vknife'])
        #     self.state.foil = last_file['foil']
        #     self.state.diaphragm = last_file['diaphragm']

    def set_settings(self, state: MainPacket = None, response: bool = False):
        super().set_settings(state, response)
        config = configparser.ConfigParser(inline_comment_prefixes=(';', '//', '#'))
        config.read(os.path.join(work_dir(), os.path.normpath('dev/hardware/hard_settings.ini')))

        config['hardware']['angle'] = str(self.state.angle)
        config['hardware']['vknife'] = str(self.state.vknife)
        config['hardware']['hknife'] = str(self.state.hknife)
        config['hardware']['foil'] = self.state.foil
        config['hardware']['diaphragm'] = self.state.diaphragm

        with open(os.path.join(work_dir(), os.path.normpath('dev/hardware/hard_settings.ini')), 'w') as configfile:
            config.write(configfile)

        del config

        gc.collect(generation=2)

    def snapshot(self, request: MainPacket = None, response: bool = False):
        hf, table = super().snapshot(request, response)
        table.attrs['name'] = 'Mechanical system of the SXR diagnostic'
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