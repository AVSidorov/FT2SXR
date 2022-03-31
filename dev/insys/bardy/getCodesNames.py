from .ctrl import BRDctrl, BRDctrl_COMMON
from .ctrladc import brdAdcCode, BRDctrl_ADC


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