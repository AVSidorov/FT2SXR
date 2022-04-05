from .ctrl import *
from .ctrladc import *
from .brderr import *
from .ctrlstrm import *
from .ctrlsysmon import *
from .ctrlsdram import *

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
    base = getBaseName(status)
    sourceDict = 'BRDerr'
    if base is not None:
        sourceDict += base[base.find('_'):]

    if eval(f'status & ~0x00070000 in [_ & ~0x00070000 for _ in {sourceDict}]'):
        return eval(f'{sourceDict}[status & ~0x00070000]')
    elif eval(f'status & 0xFFFF in {sourceDict}'):
        return eval(f'{sourceDict}[status & 0xFFFF]')
    else:
        return None
