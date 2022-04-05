"""
****************** File ctrlsysmon.h *************************
*
*  Definitions of user application interface
*    structures and constants
*    for BRD_ctrl : CMN section
*
* (C) InSys by Dorokhin Andrey Mar 2012
*
************************************************************
"""


from .ctrl import *

brdSysMonCode = \
    {
        'BRDctrl_SYSMON_GETTEMP'      : brdBaseCode['BRDctrl_CMN'] + 0,        # read temperatures (PBRD_SysMonVal)
        'BRDctrl_SYSMON_GETVCCINT'    : brdBaseCode['BRDctrl_CMN'] + 1,        # read voltages of VCCint(PBRD_SysMonVal)
        'BRDctrl_SYSMON_GETVCCAUX'    : brdBaseCode['BRDctrl_CMN'] + 2,        # read voltages of VCCaux (PBRD_SysMonVal)
        'BRDctrl_SYSMON_GETVREFP'     : brdBaseCode['BRDctrl_CMN'] + 3,        # read voltage of Vrefp (REAL64*)
        'BRDctrl_SYSMON_GETVREFN'     : brdBaseCode['BRDctrl_CMN'] + 4,        # read voltage of Vrefn (REAL64*)
        'BRDctrl_SYSMON_GETSTATUS'    : brdBaseCode['BRDctrl_CMN'] + 5,        # read system monitor status (U32)
        'BRDctrl_SYSMON_GETVNOMINALS' : brdBaseCode['BRDctrl_CMN'] + 6,        # get voltage nominals (PBRD_VoltNominals)
        'BRDctrl_SYSMON_GETVN7S'      : brdBaseCode['BRDctrl_CMN'] + 7,        # get voltage nominals for 7-series FPGA (PBRD_VoltNominals7s)
        'BRDctrl_SYSMON_GETVCCBRAM'   : brdBaseCode['BRDctrl_CMN'] + 8        # read voltages of VCCbram for 7-series FPGA (PBRD_SysMonVal)
    }

"""print('BRDctrl_CMN = \\')
print('{')
for key in brdSysMonCode:
    if brdSysMonCode[key] is not None:
        print(f"    0x{brdSysMonCode[key]:04X}: '{key}',")
print('}')
"""
BRDctrl_CMN = \
    {
        0x0900: 'BRDctrl_SYSMON_GETTEMP',
        0x0901: 'BRDctrl_SYSMON_GETVCCINT',
        0x0902: 'BRDctrl_SYSMON_GETVCCAUX',
        0x0903: 'BRDctrl_SYSMON_GETVREFP',
        0x0904: 'BRDctrl_SYSMON_GETVREFN',
        0x0905: 'BRDctrl_SYSMON_GETSTATUS',
        0x0906: 'BRDctrl_SYSMON_GETVNOMINALS',
        0x0907: 'BRDctrl_SYSMON_GETVN7S',
        0x0908: 'BRDctrl_SYSMON_GETVCCBRAM'
    }