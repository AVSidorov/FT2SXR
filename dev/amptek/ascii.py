import numpy as np
import re
import os
from dev.amptek.protocol import idByDev, devById


devsCfg = ('D', 'P', 'G', 'M', 'T', 'X')


def idByCfg (dev: str) -> int:
    return ([devsCfg.index(_) for _ in (dev,) if _ in devsCfg] + [None, ])[0]


def ascii_cfg_load(fname: str = '', structured=False):
    if len(fname) == 0:
        fname = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'px5_ascii.csv')

    if not os.path.exists(fname):
        return None

    if structured:
        names = np.genfromtxt(fname, dtype=str, delimiter=',', comments=None)[0, :]
        ascii_cfg = np.genfromtxt(fname, dtype=[(name, '<U255') for name in names],
                                  delimiter=',', comments=None, skip_header=1)
    else:
        ascii_cfg = np.genfromtxt(fname, dtype=str, delimiter=',', comments=None, skip_header=1)

    return ascii_cfg


def ascii_req_split(req: str) -> np.ndarray:
    return np.array([cmd.split('=') for cmd in req.rstrip(';').split(';') if len(cmd.split('='))==2])


def ascii_req_full(cfg: np.ndarray) -> str:
    cfg = cfg[np.argsort(cfg[:, 6])]    # sort by order column
    req = '=?;'.join(cfg[:, 0]).replace(' ', '') + '=?;'
    return ascii_rm_fields(req, 'RESC')


def ascii_resp(req: str, cfg: np.ndarray) -> str:
    req = ascii_req_split(req)
    resp = np.array([(cmd, cfg[cmd == cfg[:, 0], 1][0]) for cmd in req[:, 0] if cmd in cfg[:, 0]])
    if 'RESC' in resp[:, 0]:
        resp[resp[:, 0] == 'RESC', 1] = '?'
    return ';'.join(f'{cmd[0]}={cmd[1]}' for cmd in resp)


def ascii_rm_fields(req: str, fields) -> str:
    return ';'.join([_ for _ in req.rstrip(';').split(';') if not _.split('=')[0] in fields]) + ';'


def pack_txt_cfg(req: str, obj=None) -> str:
    cfg = None
    if obj is None:
        cfg = PX5Configuration()
        cfg = cfg.cfg

    if isinstance(obj, np.ndarray):
        cfg = obj

    if hasattr(obj, 'ascii_cfg'):
        if isinstance(obj.ascii_cfg, PX5Configuration):
            cfg = obj.ascii_cfg.cfg
        elif isinstance(obj.ascii_cfg, np.ndarray):
            cfg = obj.ascii_cfg

    if hasattr(obj, 'cfg'):
        if isinstance(obj.cfg, PX5Configuration):
            cfg = obj.cfg.cfg
        elif isinstance(obj.cfg, np.ndarray):
            cfg = obj.cfg

    if cfg is None:
        return ''

    return ascii_resp(req, cfg)


def ver2str(ver: int) -> str:
    return '.'.join([f'{_:02d}' for _ in [(ver >> 8), (ver & 0b11110000) >> 4, ver & 0b1111]])


def str2ver(version: str = '', limits=False, prefix='FW'):
    ver_min = 0
    ver_max = 0xfff
    match = re.search(r"(?:^|"+prefix+r"|\s)(?:\d{1,2}\.?){2,3}", version)
    if match is not None:
        ver = sum([int(_) << 4 * (2 - ind) for ind, _ in enumerate(match[0].replace(prefix, '').split('.'))])
        ver_min = ver
        if version.find('prior') >= 0:
            ver_max = ver - 1
            ver_min = 0
    else:
        ver = 0
    if limits:
        return ver_min, ver_max
    else:
        return ver


def parse_optionset(options: str) -> np.ndarray:
    return np.array(re.findall(r'(?:\[(?P<options>.+?)\])+(?:\((?P<modifier>DP5|PX5)\))?\|?', options))


def parse_option(option: str) -> list:
    optional = list()
    obligatory = option

    optional_pos = option.find('{')
    if optional_pos >= 0:
        optional = option[option.find('{')+1:option.find('}')].split('|')
        obligatory = option[:option.find('{')]+option[option.find('}')+1:]

    if optional_pos == 0:
        return [opt_part+obligatory for opt_part in ['', ]+optional]
    else:
        return [obligatory+opt_part for opt_part in ['', ] + optional]


def parse_options(options: str) -> list:
    options_full = list()
    for opt in re.findall(r'(?:(?:[^\|\{\}]+)|(?:\{.+?\})){1,2}', options.rstrip('|')+'|'):
        options_full += parse_option(opt)
    return options_full


def check_option_value(value: str, options: list) -> bool:
    if value in options:
        return True
    for opt in options:
        if '#' in opt:
            if re.match(r'^'+r'\d+'.join(re.split(r'#+', opt.replace('+',r'\+')))+r'$', value) is not None:
                return True
    return False


class PX5Configuration:
    def __new__(cls, cfg=None, obj=None, fw=None):
        cfg_obj = super().__new__(cls)

        if cfg is None:
            cfg = ascii_cfg_load()
        elif isinstance(cfg, str):
            cfg = ascii_cfg_load(fname=cfg)
        elif isinstance(cfg, np.ndarray):
            cfg = cfg

        if obj is not None:
            dev = obj.DeviceID
        else:
            dev = idByDev('PX5')

        if obj is not None:
            fw = obj.fw
        elif fw is not None:
            if isinstance(fw, str):
                fw = str2ver(fw)
            elif isinstance(fw, int) and fw <= 4095:
                fw = fw
        else:
            fw = 4095

        cfg_new = np.ndarray((0, cfg.shape[1]))
        for row in cfg:
            ver_min, ver_max = str2ver(row[-3], limits=True)
            if all((any((len(row[-2]) == 0, devsCfg[dev] in row[-2].split(';'))),
                     all((ver_min <= fw, fw <= ver_max)))):
                # check duplicates
                if cfg_new.shape[0] > 0:
                    if row[0] in cfg_new[:, 0]:
                        mask = cfg_new[:, 0] == row[0]
                        ver_min0, ver_max0 = str2ver(cfg_new[mask, -3][0], limits=True)
                        if ver_min > ver_min0:
                            cfg_new[mask, :] = row
                    else:
                        cfg_new = np.vstack((cfg_new, row))
                else:
                    cfg_new = np.vstack((cfg_new, row))

        if cfg_new.size <= 0:
            return None

        # options cleanup
        for row in cfg_new:
            options_set = parse_optionset(row[3])
            row[3] = options_set[np.logical_or(options_set[:, 1] == devById(dev), options_set[:, 1] == ''), 0][0]

        cfg_obj.cfg = cfg_new
        return cfg_obj

    def __call__(self):
        return self.cfg

    def get_options(self, cmd: str) -> list:
        if cmd in self.cfg[:, 0]:
            return parse_options(self.cfg[self.cfg[:, 0] == cmd, 3][0])

    def check_option_value(self, cmd: str, value: str) -> bool:
        if cmd in self.cfg[:, 0]:
            return check_option_value(value, self.get_options(cmd))
        else:
            return None

    def full_req(self):
        return ascii_req_full(self.cfg)
