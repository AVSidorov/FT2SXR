
"""****************** File CtrlSdram.h *************************
*
*  Definitions of user application interface
*    structures and constants
*    for BRD_ctrl : SDRAM section
*
* (C) Instrumental Systems
*
* Created: by Dorokhin A. Feb, 2005
* 17.02.2005 - add BRDctrl_SDRAM_...DLG
* 04.03.2005 - add 'BRDctrl_SDRAM_GETCFG'
*
************************************************************
"""

from .ctrl import *

brdSdramCode = \
    {
        'BRDctrl_SDRAM_SETREADMODE'        : brdBaseCode['BRDctrl_SDRAM'] + 0,    # Set SDRAM read mode (auto or non-auto) (U32)
        'BRDctrl_SDRAM_GETREADMODE'        : brdBaseCode['BRDctrl_SDRAM'] + 1,    # Get SDRAM read mode (auto or non-auto) (U32)
        'BRDctrl_SDRAM_SETMEMSIZE'        : brdBaseCode['BRDctrl_SDRAM'] + 2,    # Set memory size for acqusition data (32-bit words) (U32)
        'BRDctrl_SDRAM_GETMEMSIZE'        : brdBaseCode['BRDctrl_SDRAM'] + 3,    # Get memory size for acqusition data (32-bit words) (U32)
        'BRDctrl_SDRAM_SETSTARTADDR'        : brdBaseCode['BRDctrl_SDRAM'] + 4,    # Set start address for acqusition data (32-bit words) (U32)
        'BRDctrl_SDRAM_GETSTARTADDR'        : brdBaseCode['BRDctrl_SDRAM'] + 5,    # Get start address for acqusition data (32-bit words) (U32)
        'BRDctrl_SDRAM_SETREADADDR'        : brdBaseCode['BRDctrl_SDRAM'] + 6,    # Set start address for read data (32-bit words) (used by non-auto mode) (U32)
        'BRDctrl_SDRAM_GETREADADDR'        : brdBaseCode['BRDctrl_SDRAM'] + 7,    # Get start address for read data (32-bit words) (used by non-auto mode) (U32)
        'BRDctrl_SDRAM_SETPOSTTRIGER'        : brdBaseCode['BRDctrl_SDRAM'] + 8,    # Set post-trigger size (32-bit words) (used by pretrigger mode) (U32)
        'BRDctrl_SDRAM_GETPOSTTRIGER'        : brdBaseCode['BRDctrl_SDRAM'] + 9,    # Get post-trigger size (32-bit words) (used by pretrigger mode) (U32)
        'BRDctrl_SDRAM_SETFIFOMODE'        : brdBaseCode['BRDctrl_SDRAM'] + 10,    # Set FIFO mode (U32)
        'BRDctrl_SDRAM_GETFIFOMODE'        : brdBaseCode['BRDctrl_SDRAM'] + 11,    # Get FIFO mode (U32)
        'BRDctrl_SDRAM_GETCFG'            : brdBaseCode['BRDctrl_SDRAM'] + 13,    # Get SDRAM configuration (PBRD_SdramCfg)
        'BRDctrl_SDRAM_SETSEL'            : brdBaseCode['BRDctrl_SDRAM'] + 14,    # Set select source & destination for SDRAM (U32)
        'BRDctrl_SDRAM_GETSEL'            : brdBaseCode['BRDctrl_SDRAM'] + 15,    # Get select source & destination for SDRAM (U32)
        'BRDctrl_SDRAM_GETCFGEX'            : brdBaseCode['BRDctrl_SDRAM'] + 17,    # Get extension SDRAM configuration (PBRD_SdramCfgEx)
        'BRDctrl_SDRAM_SETTESTSEQ'        : brdBaseCode['BRDctrl_SDRAM'] + 18,    # Set SDRAM test sequence (U32)
        'BRDctrl_SDRAM_GETTESTSEQ'        : brdBaseCode['BRDctrl_SDRAM'] + 19,    # Get SDRAM test sequence (U32)
        
        #'BRDctrl_SDRAM_ENDPAGESDLG'        : brdBaseCode['BRDctrl_SDRAM'] + 124,    # destroy property pages for SDRAM (NULL)
        #'BRDctrl_SDRAM_SETPROPDLG'        : brdBaseCode['BRDctrl_SDRAM'] + 125,    # Display setting property dialog for SDRAM (ULONG)
        #'BRDctrl_SDRAM_GETPROPDLG'        : brdBaseCode['BRDctrl_SDRAM'] + 126,    # Display getting only property dialog for SDRAM (ULONG)
        'BRDctrl_SDRAM_GETDLGPAGES'        : brdBaseCode['BRDctrl_SDRAM'] + 127,    # get property pages of SDRAM for dialog (PBRD_PropertyList)
        
        'BRDctrl_SDRAM_FIFORESET'            : brdBaseCode['BRDctrl_SDRAM'] + 128,    # SDRAM FIFO reset (NULL)
        'BRDctrl_SDRAM_ENABLE'            : brdBaseCode['BRDctrl_SDRAM'] + 129,    # Enable/disable write to SDRAM (U32)
        'BRDctrl_SDRAM_FIFOSTATUS'        : brdBaseCode['BRDctrl_SDRAM'] + 130,    # Get SDRAM FIFO status (U32)
        'BRDctrl_SDRAM_GETDATA'            : brdBaseCode['BRDctrl_SDRAM'] + 131,    # Get data buffer from SDRAM (BRD_DataBuf)
        'BRDctrl_SDRAM_READENABLE'        : brdBaseCode['BRDctrl_SDRAM'] + 132,    # Enable/disable read to SDRAM (used by non-auto mode) (U32)
        'BRDctrl_SDRAM_GETACQSIZE'        : brdBaseCode['BRDctrl_SDRAM'] + 133,    # Get real acqusition data size (32-bit words) (U32)
        'BRDctrl_SDRAM_ISACQCOMPLETE'        : brdBaseCode['BRDctrl_SDRAM'] + 134,    # Get acqusition complete status (U32)
        'BRDctrl_SDRAM_ISPASSMEMEND'        : brdBaseCode['BRDctrl_SDRAM'] + 135,    # Get passage memory end (cycling) status by acqusition data (U32)
        'BRDctrl_SDRAM_GETPRETRIGEVENT'    : brdBaseCode['BRDctrl_SDRAM'] + 136,    # Get real start event by pretrigger mode (32-bit words) (U32)
        
        'BRDctrl_SDRAM_GETSRCSTREAM'        : brdBaseCode['BRDctrl_SDRAM'] + 138,    # Get source address for stream (U32)
        'BRDctrl_SDRAM_IRQACQCOMPLETE'    : brdBaseCode['BRDctrl_SDRAM'] + 139,    # Enable/disable acqusition complete interrupt (U32)
        'BRDctrl_SDRAM_WAITACQCOMPLETE'    : brdBaseCode['BRDctrl_SDRAM'] + 140,    # Wait of acqusition complete interrupt (U32)
        'BRDctrl_SDRAM_WAITACQCOMPLETEEX'    : brdBaseCode['BRDctrl_SDRAM'] + 141,    # Wait of acqusition complete interrupt (PBRD_WaitEvent)
        
        'BRDctrl_SDRAM_FLAGCLR'            : brdBaseCode['BRDctrl_SDRAM'] + 144,    # SDRAM FIFO status flag clear (NULL)
        'BRDctrl_SDRAM_PUTDATA'            : brdBaseCode['BRDctrl_SDRAM'] + 145    # Put data buffer into SDRAM (BRD_DataBuf)
}

"""
print('BRDctrl_SDRAM = \\')
print('{')
for key in brdSdramCode:
    if brdSdramCode[key] is not None:
        print(f"    0x{brdSdramCode[key]:04X}: '{key}',")
print('}')
"""
BRDctrl_SDRAM = \
    {
        0x1200: 'BRDctrl_SDRAM_SETREADMODE',
        0x1201: 'BRDctrl_SDRAM_GETREADMODE',
        0x1202: 'BRDctrl_SDRAM_SETMEMSIZE',
        0x1203: 'BRDctrl_SDRAM_GETMEMSIZE',
        0x1204: 'BRDctrl_SDRAM_SETSTARTADDR',
        0x1205: 'BRDctrl_SDRAM_GETSTARTADDR',
        0x1206: 'BRDctrl_SDRAM_SETREADADDR',
        0x1207: 'BRDctrl_SDRAM_GETREADADDR',
        0x1208: 'BRDctrl_SDRAM_SETPOSTTRIGER',
        0x1209: 'BRDctrl_SDRAM_GETPOSTTRIGER',
        0x120A: 'BRDctrl_SDRAM_SETFIFOMODE',
        0x120B: 'BRDctrl_SDRAM_GETFIFOMODE',
        0x120D: 'BRDctrl_SDRAM_GETCFG',
        0x120E: 'BRDctrl_SDRAM_SETSEL',
        0x120F: 'BRDctrl_SDRAM_GETSEL',
        0x1211: 'BRDctrl_SDRAM_GETCFGEX',
        0x1212: 'BRDctrl_SDRAM_SETTESTSEQ',
        0x1213: 'BRDctrl_SDRAM_GETTESTSEQ',
        0x127F: 'BRDctrl_SDRAM_GETDLGPAGES',
        0x1280: 'BRDctrl_SDRAM_FIFORESET',
        0x1281: 'BRDctrl_SDRAM_ENABLE',
        0x1282: 'BRDctrl_SDRAM_FIFOSTATUS',
        0x1283: 'BRDctrl_SDRAM_GETDATA',
        0x1284: 'BRDctrl_SDRAM_READENABLE',
        0x1285: 'BRDctrl_SDRAM_GETACQSIZE',
        0x1286: 'BRDctrl_SDRAM_ISACQCOMPLETE',
        0x1287: 'BRDctrl_SDRAM_ISPASSMEMEND',
        0x1288: 'BRDctrl_SDRAM_GETPRETRIGEVENT',
        0x128A: 'BRDctrl_SDRAM_GETSRCSTREAM',
        0x128B: 'BRDctrl_SDRAM_IRQACQCOMPLETE',
        0x128C: 'BRDctrl_SDRAM_WAITACQCOMPLETE',
        0x128D: 'BRDctrl_SDRAM_WAITACQCOMPLETEEX',
        0x1290: 'BRDctrl_SDRAM_FLAGCLR',
        0x1291: 'BRDctrl_SDRAM_PUTDATA'
    }

#SDRAM error codes
brdSdramErrCode = \
    {
        'BRDerr_SDRAM_NO_MEMORY'        :    brdBaseCode['BRDctrl_SDRAM'] + 0,
        'BRDerr_SDRAM_BAD_PARAMETER'    :    brdBaseCode['BRDctrl_SDRAM'] + 1,
        'BRDerr_SDRAM_NO_EQU_SPD'       :    brdBaseCode['BRDctrl_SDRAM'] + 2,
    }

"""
print('BRDerr_SDRAM = \\')
print('{')
for key in brdSdramErrCode:
    if brdSdramErrCode[key] is not None:
        print(f"    0x{brdSdramErrCode[key]:04X}: '{key}',")
print('}')
"""

BRDerr_SDRAM = \
    {
        0x1200: 'BRDerr_SDRAM_NO_MEMORY',
        0x1201: 'BRDerr_SDRAM_BAD_PARAMETER',
        0x1202: 'BRDerr_SDRAM_NO_EQU_SPD'
    }