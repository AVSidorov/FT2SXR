import numpy as np
from os import path, listdir, stat
# from core.core import Core
import configparser
import h5py as h5
import re
# from time import time


# class Reader(Core):
class Reader:
    def __init__(self):
        # super().__init__(parent=parent)
        super().__init__()

        self.data = None
        self.meta = None
        self.stat = None

    def read(self, data_file=None):
        # print(type(data_file))
        if isinstance(data_file, str) and path.isabs(data_file):
            if path.isdir(data_file):
                return

            elif path.isfile(data_file):

                if path.splitext(data_file)[1] == '.bin':
                    if 'adc.ini' and 'data_0.bin' in listdir(path.dirname(data_file)):
                        try:
                            if 'adc_additional.ini' in listdir(path.dirname(data_file)):
                                conf_dict = self.parse_ini(ini_path=path.join(path.dirname(data_file), 'adc.ini'),
                                                           additional=path.join(path.dirname(data_file), 'adc_additional.ini'))
                            else:
                                conf_dict = self.parse_ini(ini_path=path.join(path.dirname(data_file), 'adc.ini'))
                        except KeyError:
                            return

                        n_ch = conf_dict['mask'].count("1")
                        measurements = np.fromfile(data_file, dtype=np.int16)
                        # self.data = measurements.reshape((-1, n_ch)).T
                        self.data = np.transpose(np.reshape(measurements, (-1, n_ch)))
                        if conf_dict['void_mask'] is not None:
                            str_mask = conf_dict['void_mask'][2:]
                            for i in range(1, len(str_mask) + 1):
                                if str_mask[-i] == '1':
                                    self.data = np.delete(self.data, i-1)
                            conf_dict['mask'] = bin(eval(conf_dict['mask']) ^ eval(conf_dict['void_mask']))
                            n_ch = conf_dict['mask'].count("1")

                        meta = []
                        str_mask = conf_dict['mask'][2:]
                        for i in range(1, len(str_mask)+1):
                            if str_mask[-i] == '1':
                                # meta.append('.\\' + data_file.split('\\')[-2] + '\\' + data_file.split('\\')[-1])
                                meta.append(path.relpath(data_file, path.dirname(__file__)))
                                meta.append(i)
                                meta.append(conf_dict['samples'])
                                meta.append(conf_dict['rate'])
                                if conf_dict['void_mask'] is not None:
                                    meta.append(conf_dict['names'][i - 1])
                                else:
                                    meta.append(i)
                        meta = np.array(meta)
                        meta = meta.reshape((n_ch, -1))
                        self.meta = meta
                        self.stat = stat(data_file)

                elif path.splitext(data_file)[1] == '.h5':
                    try:
                        conf_dict, self.data = self.parse_h5(data_file)
                    except KeyError:
                        return

                    n_ch = conf_dict['mask'].count("1")
                    # self.data = measurements

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
                    self.stat = stat(data_file)

    def clear(self):
        del self.data
        self.data = None
        del self.meta
        self.meta = None
        del self.stat
        self.stat = None

    def parse_ini(self, ini_path=None, additional=None):
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

            void_mask = None
            names = None
            if additional is not None:
                config.read(additional)
                names = []
                void_mask = bin(eval(config['adc_additional']['void_mask']))
                real_mask = bin(eval(mask) ^ eval(void_mask))
                str_mask = real_mask[2:]
                for i in range(1, len(str_mask) + 1):
                    if str_mask[-i] == '1':
                        names.append(config['adc_additional'][f'name{i}'])

            return {'samples': samples,
                    'rate': rate,
                    'mask': mask,
                    'names': names,
                    'void_mask': void_mask}
        else:
            return None

    def parse_h5(self, h5_path):
        if path.isabs(h5_path):
            with h5.File(h5_path, 'r') as file:
                config = file['SXR']['ADC']['config']
                samples = eval(config['Option']['MemSamplesPerChan'][()])
                device_section = list(config.keys())
                # device_section.remove('Option')
                # if 'DEFAULT' in device_section:
                #     device_section.remove('DEFAULT')
                for section in device_section:
                    if re.match(r'device', section):
                        device_section = section
                        break
                # device_section = device_section[0]
                rate = eval(config[device_section]['SamplingRate'][()])
                mask = bin(eval(config[device_section]['ChannelMask'][()]))

                channels = list(file['SXR']['ADC'].keys())
                channels.remove('config')

                names = []

                for i in channels:
                    globals()[i] = file['SXR']['ADC'][i][()]
                    names.append(file['SXR']['ADC'][i].attrs.get('name'))
                measurements = np.vstack([np.array(eval(i), dtype=np.int16) for i in channels])
                return {'samples': samples,
                        'rate': rate,
                        'mask': mask,
                        'names': names}, measurements
        else:
            return None
