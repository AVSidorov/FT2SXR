BRD_SECTIONSIZE = 256

brdBaseCode =\
{
    'BRDctrl_DAQ'      : BRD_SECTIONSIZE*8,  # DAQ control
    'BRDctrl_CMN'      : BRD_SECTIONSIZE*9,  # common control parameters
    'BRDctrl_ADC'      : BRD_SECTIONSIZE*10,  # ADC control parameters
    'BRDctrl_DAC'      : BRD_SECTIONSIZE*11,  # DAC control parameters
    'BRDctrl_DDC'      : BRD_SECTIONSIZE*12,  # DDC control parameters
    'BRDctrl_PIO'      : BRD_SECTIONSIZE*13,  # PIO control parameters
    'BRDctrl_STREAM'   : BRD_SECTIONSIZE*14,  # STREAM control parameters
    'BRDctrl_SVEAM'    : BRD_SECTIONSIZE*15,  # SVEAM control parameters
    'BRDctrl_DIOIN'    : BRD_SECTIONSIZE*16,  # DIOIN control parameters
    'BRDctrl_DIOOUT'   : BRD_SECTIONSIZE*17,  # DIOOUT control parameters
    'BRDctrl_SDRAM'    : BRD_SECTIONSIZE*18,  # SDRAM control parameters
    'BRDctrl_DSPNODE'  : BRD_SECTIONSIZE*19,  # DSP node control parameters
    'BRDctrl_TEST'     : BRD_SECTIONSIZE*20,  # Test control parameters
    'BRDctrl_FOTR'     : BRD_SECTIONSIZE*21,  # FOTR control parameters
    'BRDctrl_QM'       : BRD_SECTIONSIZE*22,  # QM9857 control parameters
    'BRDctrl_GC5016'   : BRD_SECTIONSIZE*23,  # GC5016 control parameters
    'BRDctrl_DDS'      : BRD_SECTIONSIZE*24,  # DDS control parameters
    'BRDctrl_SYNCDAC'  : BRD_SECTIONSIZE*25,  # SyncDac control parameters
    'BRDctrl_COMMON'   : BRD_SECTIONSIZE*26,  # real common control area
    'BRDctrl_NET'      : BRD_SECTIONSIZE*27,  # net (still EMAC) control parameters
    'BRDctrl_FM402S'   : BRD_SECTIONSIZE*28,  # fm40Xs control parameters
    'BRDctrl_ILLEGAL'  : None
}

"""
print('BRDctrl = \\')
print('{')
for key in brdBaseCode:
    if brdBaseCode[key] is not None:
        print(f"    0x{brdBaseCode[key]:04X}: '{key}',")
print('}')
"""

BRDctrl = \
{
    0x0800: 'BRDctrl_DAQ',
    0x0900: 'BRDctrl_CMN',
    0x0A00: 'BRDctrl_ADC',
    0x0B00: 'BRDctrl_DAC',
    0x0C00: 'BRDctrl_DDC',
    0x0D00: 'BRDctrl_PIO',
    0x0E00: 'BRDctrl_STREAM',
    0x0F00: 'BRDctrl_SVEAM',
    0x1000: 'BRDctrl_DIOIN',
    0x1100: 'BRDctrl_DIOOUT',
    0x1200: 'BRDctrl_SDRAM',
    0x1300: 'BRDctrl_DSPNODE',
    0x1400: 'BRDctrl_TEST',
    0x1500: 'BRDctrl_FOTR',
    0x1600: 'BRDctrl_QM',
    0x1700: 'BRDctrl_GC5016',
    0x1800: 'BRDctrl_DDS',
    0x1900: 'BRDctrl_SYNCDAC',
    0x1A00: 'BRDctrl_COMMON',
    0x1B00: 'BRDctrl_NET',
    0x1C00: 'BRDctrl_FM402S'
}

#
# Real Common Commands
#
brdCommonCode =\
{
    'BRDctrl_GETTRACETEXT'   : brdBaseCode['BRDctrl_COMMON'] + 0,   # Получить буфер трассировки в виде текста
    'BRDctrl_GETTRACEPARAM'  : brdBaseCode['BRDctrl_COMMON'] + 1   # Получить буфер трассировки в виде структуры
}

BRDctrl_COMMON = \
{
    0x1A00: 'BRDctrl_GETTRACETEXT',
    0x1A01: 'BRDctrl_GETTRACEPARAM'
}

# #pragma pack(push, 1)
#
# // for set and get Parameter value of the each channel
# typedef struct _BRD_ValChan
# {
#     REAL64	value;		// Parameter value
# U32		chan;		// Channel number
# } BRD_ValChan, *PBRD_ValChan;
#
# // for set and get Parameter value of the each channel
# typedef struct _BRD_UvalChan
# {
#     U32		value;		// Parameter value
# U32		chan;		// Channel number
# } BRD_UvalChan, *PBRD_UvalChan;
#
# // for set and get Parameter value
# typedef struct _BRD_EnVal
# {
#     U32		enable;	// enable/disable parameter
# U32		value;	// value  parameter
# } BRD_EnVal, *PBRD_EnVal;
#
# typedef struct _BRD_WaitEvent
# {
#     U32		timeout;		// IN,  Timeout (msec) (0xFFFFFFFFL - INFINITE)
# HANDLE	hAppEvent;		// application event
# } BRD_WaitEvent, *PBRD_WaitEvent;
#
# typedef struct
# {
#     U32		item;			// IN:  Number of Items into pItem[] Array
# U32		itemReal;		// OUT: Number of Initialized Items or Number of Really Needed Items
# void	*pItem;			// OUT: Array with Items
# } BRD_ItemArray, *PBRD_ItemArray;
#
# typedef struct
# {
#     U32		traceId;	// IN: Number of Trace Buffer
# U32		sizeb;		// IN: Size of Trace Buffer (bytes)
# BRDCHAR	*pText;		// OUT: Trace Buffer like Text
# } BRD_TraceText, *PBRD_TraceText;
#
# typedef struct
# {
#     U32		traceId;	// IN: Number of Trace Buffer
# U32		sizeb;		// IN: Size of Trace Buffer into *pParam (bytes)
# U32		sizebReal;	// OUT: Size of Real Trace Buffer (bytes)
# void	*pParam;	// OUT: Trace Buffer like Structure
# } BRD_TraceParam, *PBRD_TraceParam;
#
# #pragma pack(pop)
#
# #endif // _CTRL_H
#
# //
# // End of file
# //
#
