import numpy as np
import h5py
from os import path, listdir
from core.core import Core
import configparser


class Reader(Core):
    def __init__(self, parent=None, data_file=None):
        super().__init__(parent=parent)

        self.data = None
        self.meta = None

    def read(self, data_file=None):
        if path.isabs(data_file):
            if path.isdir(data_file):
                print('not a file')

            elif path.isfile(data_file):

                if path.splitext(data_file)[1] == '.bin':
                    if 'cfg.ini' and 'data_0.bin' in listdir(path.dirname(data_file)):
                        try:
                            conf_dict = self.parse_ini(path.join(path.dirname(data_file), 'cfg.ini'))
                        except KeyError:
                            return

                        n_ch = conf_dict['mask'].count("1")
                        measurements = np.fromfile(data_file, dtype=np.int16)
                        self.data = measurements.reshape((-1, n_ch)).T

                        meta = []
                        str_mask = conf_dict['mask'][2:]
                        for i in range(1, len(str_mask)+1):
                            if str_mask[-i] == '1':
                                meta.append('shotid')
                                meta.append(i)
                                meta.append(conf_dict['samples'])
                                meta.append(conf_dict['rate'])
                        meta = np.array(np.array(meta)).reshape((-1, n_ch)).T
                        self.meta = meta

    def clear(self):
        self.data = None

    def parse_ini(self, ini_path=None):
        if path.isabs(ini_path):
            config = configparser.ConfigParser()
            config.read(ini_path)

            samples = int(config['Option']['MemSamplesPerChan'])
            device_section = config.sections().copy()
            device_section.remove('Option')
            device_section = device_section[0]
            rate = int(config[device_section]['SamplingRate'])
            mask = bin(eval(config[device_section]['ChannelMask']))

            return {'samples': samples,
                    'rate': rate,
                    'mask': mask}
        else:
            return None
