from .ctrl import *
from .ctrladc import *
from .brderr import *


def getBaseName(code):
    baseCode = code & 0xFF00
    if not baseCode in BRDctrl:
        return None
    return BRDctrl[baseCode]


def getCmdName(code):
    baseName = getBaseName(code)
    if baseName is None:
        return None

    try: eval(f'{baseName}')
    except NameError: return None
    if eval(f'isinstance({baseName}, dict)'):
        if eval(f'code in {baseName}'):
            cmdName = eval(f'{baseName}[{code}]')
    else:
        cmdName = None
    return cmdName


def getErrName(status):
    if status & ~0x00070000 in [_ & ~0x00070000 for _ in BRDerr]:
        return BRDerr[status & ~0x00070000]
    elif status & ~0x00070000 in [_ & ~0x00070000 for _ in BRDerr_ADC]:
        return BRDerr_ADC[status & ~0x00070000]
    else:
        return None
