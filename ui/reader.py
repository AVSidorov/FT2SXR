import numpy as np
from os import path, listdir
from core.core import Core
import configparser
import h5py as h5


class Reader(Core):
    def __init__(self, parent=None, data_file=None):
        super().__init__(parent=parent)

        self.data = None
        self.meta = None

    def read(self, data_file=None):
        # print(type(data_file))
        if isinstance(data_file, str) and path.isabs(data_file):
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
                                # meta.append('.\\' + data_file.split('\\')[-2] + '\\' + data_file.split('\\')[-1])
                                meta.append(path.relpath(data_file, path.dirname(__file__)))
                                meta.append(i)
                                meta.append(conf_dict['samples'])
                                meta.append(conf_dict['rate'])
                                meta.append(i)
                        meta = np.array(meta)
                        meta = meta.reshape((n_ch, -1))
                        self.meta = meta

                if path.splitext(data_file)[1] == '.h5':
                    try:
                        conf_dict, measurements = self.parse_h5(data_file)
                    except KeyError:
                        return

                    n_ch = conf_dict['mask'].count("1")
                    self.data = measurements

                    meta = []
                    str_mask = conf_dict['mask'][2:]
                    for i in range(1, len(str_mask) + 1):
                        if str_mask[-i] == '1':
                            meta.append(path.split(data_file)[1])
                            meta.append(i)
                            meta.append(conf_dict['samples'])
                            meta.append(conf_dict['rate'])
                            meta.append(conf_dict['names'][i-1])
                    meta = np.array(meta)
                    meta = meta.reshape((n_ch, -1))
                    self.meta = meta

    def clear(self):
        self.data = None
        self.meta = None

    def parse_ini(self, ini_path=None):
        if path.isabs(ini_path):
            config = configparser.ConfigParser()
            config.read(ini_path)

            samples = int(config['Option']['MemSamplesPerChan'])
            device_section = config.sections().copy()
            device_section.remove('Option')
            if 'DEFAULT' in device_section:
                device_section.remove('DEFAULT')
            device_section = device_section[0]
            rate = int(config[device_section]['SamplingRate'])
            mask = bin(eval(config[device_section]['ChannelMask']))

            return {'samples': samples,
                    'rate': rate,
                    'mask': mask,
                    'names': None}
        else:
            return None

    def parse_h5(self, h5_path):
        if path.isabs(h5_path):
            with h5.File(h5_path, 'r') as file:
                config = file['SXR']['ADC']['config']
                samples = eval(config['Option']['MemSamplesPerChan'][()])
                device_section = list(config.keys())
                device_section.remove('Option')
                if 'DEFAULT' in device_section:
                    device_section.remove('DEFAULT')
                device_section = device_section[0]
                rate = eval(config[device_section]['SamplingRate'][()])
                mask = bin(eval(config[device_section]['ChannelMask'][()]))

                channels = list(file['SXR']['ADC'].keys())
                channels.remove('config')

                names = []
                for i in channels:
                    globals()[i] = file['SXR']['ADC'][i][()]
                    names.append(file['SXR']['ADC'][i].attrs.get('name'))
                measurements = np.vstack([eval(i) for i in channels])

                return {'samples': samples,
                        'rate': rate,
                        'mask': mask,
                        'names': names}, measurements
        else:
            return None
