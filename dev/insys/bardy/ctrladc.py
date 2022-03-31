from dev.insys.bardy.ctrl import brdBaseCode

"""
****************** File CtrlAdc.h *************************
*
*  Definitions of user application interface
*	structures and constants
*	for BRD_ctrl : ADC section
*
* (C) InSys by Dorokhin Andrey Jan, 2002
*
*	27.11.2003 - add 'BRDctrl_ADC_SETINPRANGE', 'BRDctrl_ADC_GETINPRANGE'
*	28.02.2005 - add getting ADC configuration
*
************************************************************
"""
#
# #include "ctrladmpro.h"
#
brdAdcCode = \
{
    'BRDctrl_ADC_SETCHANMASK'       :   brdBaseCode['BRDctrl_ADC'] + 0,  # Set mask of ADC channels (U32)
    'BRDctrl_ADC_GETCHANMASK'       :   brdBaseCode['BRDctrl_ADC'] + 1,  # Get mask of ADC channels (U32)
    'BRDctrl_ADC_SETGAIN'           :   brdBaseCode['BRDctrl_ADC'] + 2,  # Set gain of the ADC channel (BRD_ValChan)
    'BRDctrl_ADC_GETGAIN'           :   brdBaseCode['BRDctrl_ADC'] + 3,  # Get gain of the ADC channel (BRD_ValChan)
    'BRDctrl_ADC_SETINPRANGE'       :   brdBaseCode['BRDctrl_ADC'] + 4,  # Set input range of the ADC channel in Volt (BRD_ValChan)
    'BRDctrl_ADC_GETINPRANGE'       :   brdBaseCode['BRDctrl_ADC'] + 5,  # Get input range of the ADC channel in Volt (BRD_ValChan)
    'BRDctrl_ADC_SETBIAS'           :   brdBaseCode['BRDctrl_ADC'] + 6,  # Set bias of the ADC channel (BRD_ValChan)
    'BRDctrl_ADC_GETBIAS'           :   brdBaseCode['BRDctrl_ADC'] + 7,  # Get bias of the ADC channel (BRD_ValChan)
    'BRDctrl_ADC_SETFORMAT'         :   brdBaseCode['BRDctrl_ADC'] + 8,  # Set format (8/16/24/32-bit) data for ADC (U32)
    'BRDctrl_ADC_GETFORMAT'         :   brdBaseCode['BRDctrl_ADC'] + 9,  # Get format (8/16/24/32-bit) data for ADC (U32)
    'BRDctrl_ADC_SETRATE'           :   brdBaseCode['BRDctrl_ADC'] + 10,  # Set ADC sampling rate in Hz (REAL64)
    'BRDctrl_ADC_GETRATE'           :   brdBaseCode['BRDctrl_ADC'] + 11,  # Get ADC sampling rate in Hz (REAL64)
    'BRDctrl_ADC_SETCLKMODE'        :   brdBaseCode['BRDctrl_ADC'] + 12,  # Set ADC clock mode (BRD_ClkMode)
    'BRDctrl_ADC_GETCLKMODE'        :   brdBaseCode['BRDctrl_ADC'] + 13,  # Get ADC clock mode (BRD_ClkMode)
    'BRDctrl_ADC_SETSYNCMODE'       :   brdBaseCode['BRDctrl_ADC'] + 14,  # Set ADC synchronization mode (BRD_SyncMode)
    'BRDctrl_ADC_GETSYNCMODE'       :   brdBaseCode['BRDctrl_ADC'] + 15,  # Get ADC synchronization mode (BRD_SyncMode)
    'BRDctrl_ADC_SETMASTER'         :   brdBaseCode['BRDctrl_ADC'] + 16,  # Set ADC master/single/slave mode (U32)
    'BRDctrl_ADC_GETMASTER'         :   brdBaseCode['BRDctrl_ADC'] + 17,  # Get ADC master/single/slave mode (U32)
    'BRDctrl_ADC_SETSTARTMODE'      :   brdBaseCode['BRDctrl_ADC'] + 18,  # Set ADC start mode (BRD_StartMode)
    'BRDctrl_ADC_GETSTARTMODE'      :   brdBaseCode['BRDctrl_ADC'] + 19,  # Set ADC start mode (BRD_StartMode)
    'BRDctrl_ADC_SETSTDELAY'        :   brdBaseCode['BRDctrl_ADC'] + 20,  # Set ADC start delay counter (BRD_EnVal)
    'BRDctrl_ADC_GETSTDELAY'        :   brdBaseCode['BRDctrl_ADC'] + 21,  # Get ADC start delay counter (BRD_EnVal)
    'BRDctrl_ADC_SETACQDATA'        :   brdBaseCode['BRDctrl_ADC'] + 22,  # Set ADC acquisition data counter (BRD_EnVal)
    'BRDctrl_ADC_GETACQDATA'        :   brdBaseCode['BRDctrl_ADC'] + 23,  # Get ADC acquisition data counter (BRD_EnVal)
    'BRDctrl_ADC_SETSKIPDATA'       :   brdBaseCode['BRDctrl_ADC'] + 24,  # Set ADC skipped data counter (BRD_EnVal)
    'BRDctrl_ADC_GETSKIPDATA'       :   brdBaseCode['BRDctrl_ADC'] + 25,  # Get ADC skipped data counter (BRD_EnVal)
    'BRDctrl_ADC_SETINPRESIST'      :   brdBaseCode['BRDctrl_ADC'] + 26,  # Set input resistance of the ADC channel (BRD_ValChan)
    'BRDctrl_ADC_GETINPRESIST'      :   brdBaseCode['BRDctrl_ADC'] + 27,  # Get input resistance of the ADC channel (BRD_ValChan)
    'BRDctrl_ADC_SETDCCOUPLING'     :   brdBaseCode['BRDctrl_ADC'] + 28,  # Set DC coupling ADC input (BRD_ValChan)
    'BRDctrl_ADC_GETDCCOUPLING'     :   brdBaseCode['BRDctrl_ADC'] + 29,  # Get DC coupling ADC input (BRD_ValChan)
    'BRDctrl_ADC_SETINPSRC'         :   brdBaseCode['BRDctrl_ADC'] + 30,  # Set input source select
    'BRDctrl_ADC_GETINPSRC'         :   brdBaseCode['BRDctrl_ADC'] + 31,  # Get input source select
    'BRDctrl_ADC_SETINPMUX'         :   brdBaseCode['BRDctrl_ADC'] + 32,  # Set input multiplexer (BRD_InpMux)
    'BRDctrl_ADC_GETINPMUX'         :   brdBaseCode['BRDctrl_ADC'] + 33,  # Get input multiplexer (BRD_InpMux)
    'BRDctrl_ADC_SETDIFMODE'        :   brdBaseCode['BRDctrl_ADC'] + 34,  # Set differential mode of inputs (U32)
    'BRDctrl_ADC_GETDIFMODE'        :   brdBaseCode['BRDctrl_ADC'] + 35,  # Get differential mode of inputs (U32)
    'BRDctrl_ADC_SETSAMPLEHOLD'     :   brdBaseCode['BRDctrl_ADC'] + 36,  # Set sample/hold enable (U32)
    'BRDctrl_ADC_GETSAMPLEHOLD'     :   brdBaseCode['BRDctrl_ADC'] + 37,  # Get sample/hold enable (U32)
    'BRDctrl_ADC_SETGAINTUNING'     :   brdBaseCode['BRDctrl_ADC'] + 38,  # Set gain tuning (BRD_ValChan)
    'BRDctrl_ADC_GETGAINTUNING'     :   brdBaseCode['BRDctrl_ADC'] + 39,  # Get gain tuning (BRD_ValChan)
    'BRDctrl_ADC_SETTARGET'         :   brdBaseCode['BRDctrl_ADC'] + 40,  # Set ADC data target (U32)
    'BRDctrl_ADC_GETTARGET'         :   brdBaseCode['BRDctrl_ADC'] + 41,  # Get ADC data target (U32)
    'BRDctrl_ADC_SETCLKPHASE'       :   brdBaseCode['BRDctrl_ADC'] + 42,  # Set ADC clock phase tuning (BRD_ValChan)
    'BRDctrl_ADC_GETCLKPHASE'       :   brdBaseCode['BRDctrl_ADC'] + 43,  # Get ADC clock phase tuning (BRD_ValChan)
    'BRDctrl_ADC_SETDBLCLK'         :   brdBaseCode['BRDctrl_ADC'] + 44,  # Set ADC double clocking mode (U32)
    'BRDctrl_ADC_GETDBLCLK'         :   brdBaseCode['BRDctrl_ADC'] + 45,  # Get ADC double clocking mode (U32)
    'BRDctrl_ADC_SELFCLBR'          :   brdBaseCode['BRDctrl_ADC'] + 46,  # Execute self-calibration (None)
    'BRDctrl_ADC_SETPRETRIGMODE'    :   brdBaseCode['BRDctrl_ADC'] + 48,  # Set ADC pretrigger mode (BRD_PretrigMode)
    'BRDctrl_ADC_GETPRETRIGMODE'    :   brdBaseCode['BRDctrl_ADC'] + 49,  # Set ADC pretrigger mode (BRD_PretrigMode)
    #'BRDctrl_ADC_SETCLKTUNING'      :   brdBaseCode['BRDctrl_ADC'] + 50,  # Set ADC clock frequency tuning in % (REAL64)
    #'BRDctrl_ADC_GETCLKTUNING'      :   brdBaseCode['BRDctrl_ADC'] + 51,  # Get ADC clock frequency tuning in % (REAL64)
    'BRDctrl_ADC_SETCLKTHR'         :   brdBaseCode['BRDctrl_ADC'] + 50,  # Set ADC threshold clock (REAL64) - clock frequency tuning in % for ADC28X800M
    'BRDctrl_ADC_GETCLKTHR'         :   brdBaseCode['BRDctrl_ADC'] + 51,  # Get ADC threshold clock (REAL64) - clock frequency tuning in % for ADC28X800M
    'BRDctrl_ADC_SETCODE'           :   brdBaseCode['BRDctrl_ADC'] + 52,  # Set code type (0 - two's complement, 1 - floating point, 2 - straight binary, 7 - Gray code) data for ADC (U32)
    'BRDctrl_ADC_GETCODE'           :   brdBaseCode['BRDctrl_ADC'] + 53,  # Get code type (0 - two's complement, 1 - floating point, 2 - straight binary, 7 - Gray code) data for ADC (U32)
    'BRDctrl_ADC_GETCFG'            :   brdBaseCode['BRDctrl_ADC'] + 55,  # Get ADC configuration (PBRD_AdcCfg)
    'BRDctrl_ADC_WRITEINIFILE'      :   brdBaseCode['BRDctrl_ADC'] + 56,  # Write all ADC properties into ini file (PBRD_IniFile)
    'BRDctrl_ADC_READINIFILE'       :   brdBaseCode['BRDctrl_ADC'] + 57,  # Read all ADC properties from ini file (PBRD_IniFile)
    'BRDctrl_ADC_SETTESTMODE'       :   brdBaseCode['BRDctrl_ADC'] + 58,  # Set ADC test mode (U32)
    'BRDctrl_ADC_GETTESTMODE'       :   brdBaseCode['BRDctrl_ADC'] + 59,  # Get ADC test mode (U32)
    'BRDctrl_ADC_SETSPECIFIC'       :   brdBaseCode['BRDctrl_ADC'] + 60,  # Set ADC specific parameters ()
    'BRDctrl_ADC_GETSPECIFIC'       :   brdBaseCode['BRDctrl_ADC'] + 61,  # Get ADC specific parameters ()
    'BRDctrl_ADC_SETCLKLOCATION'    :   brdBaseCode['BRDctrl_ADC'] + 62,  # Set ADC clock source location (U32)
    'BRDctrl_ADC_GETCLKLOCATION'    :   brdBaseCode['BRDctrl_ADC'] + 63,  # Get ADC clock source location (U32)
    'BRDctrl_ADC_SETEXTCLKTHR'      :   brdBaseCode['BRDctrl_ADC'] + 64,  # Set ADC external clock threshold (REAL64)
    'BRDctrl_ADC_GETEXTCLKTHR'      :   brdBaseCode['BRDctrl_ADC'] + 65,  # Get ADC external clock threshold (REAL64)
    'BRDctrl_ADC_SETTITLEMODE'      :   brdBaseCode['BRDctrl_ADC'] + 66,  # Set Title mode (BRD_EnVal)
    'BRDctrl_ADC_GETTITLEMODE'      :   brdBaseCode['BRDctrl_ADC'] + 67,  # Get Title mode (BRD_EnVal)
    'BRDctrl_ADC_SETTITLEDATA'      :   brdBaseCode['BRDctrl_ADC'] + 68,  # Set data for title programming register (U32)
    'BRDctrl_ADC_GETTITLEDATA'      :   brdBaseCode['BRDctrl_ADC'] + 69,  # Get data for title programming register (U32)
    'BRDctrl_ADC_SETCLKINV'         :   brdBaseCode['BRDctrl_ADC'] + 70,  # Set ADC clock invert (U32)
    'BRDctrl_ADC_GETCLKINV'         :   brdBaseCode['BRDctrl_ADC'] + 71,  # Get ADC clock invert (U32)
    'BRDctrl_ADC_SETINPFILTER'      :   brdBaseCode['BRDctrl_ADC'] + 72,  # Set input LPF enable/disable (BRD_ValChan)
    'BRDctrl_ADC_GETINPFILTER'      :   brdBaseCode['BRDctrl_ADC'] + 73,  # Get input LPF enable/disable (BRD_ValChan)
    'BRDctrl_ADC_SETMU'             :   brdBaseCode['BRDctrl_ADC'] + 74,  # Set MU (subunit specific)
    'BRDctrl_ADC_GETMU'             :   brdBaseCode['BRDctrl_ADC'] + 75,  # Get MU (subunit specific)
    'BRDctrl_ADC_SETSTARTSLAVE'     :   brdBaseCode['BRDctrl_ADC'] + 76,  # Set Start from Master (U32)
    'BRDctrl_ADC_GETSTARTSLAVE'     :   brdBaseCode['BRDctrl_ADC'] + 77,  # Get Start from Master (U32)
    'BRDctrl_ADC_SETCLOCKSLAVE'     :   brdBaseCode['BRDctrl_ADC'] + 78,  # Set Clock from Master (U32)
    'BRDctrl_ADC_GETCLOCKSLAVE'     :   brdBaseCode['BRDctrl_ADC'] + 79,  # Get Clock from Master (U32)
    'BRDctrl_ADC_GETCHANORDER'      :   brdBaseCode['BRDctrl_ADC'] + 80,  # Get Order of ChanNumbers into FIFO
    'BRDctrl_ADC_SETTESTSEQ'        :   brdBaseCode['BRDctrl_ADC'] + 82,  # Set ADC test sequence (U32)
    'BRDctrl_ADC_GETTESTSEQ'        :   brdBaseCode['BRDctrl_ADC'] + 83,  # Get ADC test sequence (U32)
    'BRDctrl_ADC_WRDELAY'           :   brdBaseCode['BRDctrl_ADC'] + 84,  # Write delay value (BRD_StdDelay)
    'BRDctrl_ADC_RDDELAY'           :   brdBaseCode['BRDctrl_ADC'] + 85,  # Read delay value (BRD_StdDelay)
    'BRDctrl_ADC_RSTDELAY'          :   brdBaseCode['BRDctrl_ADC'] + 86,  # Reset delay value (BRD_StdDelay)
    #'BRDctrl_ADC_ENDPAGESDLG'      :   brdBaseCode['BRDctrl_ADC'] + 124, # destroy property pages for ADC (NULL)
    #'BRDctrl_ADC_SETPROPDLG'       :   brdBaseCode['BRDctrl_ADC'] + 125, # Display setting property dialog for ADC (ULONG)
    #'BRDctrl_ADC_GETPROPDLG'       :   brdBaseCode['BRDctrl_ADC'] + 126, # Display getting only property dialog for ADC (ULONG)
    'BRDctrl_ADC_GETDLGPAGES'       :   brdBaseCode['BRDctrl_ADC'] + 127, # get property pages of ADC for dialog (PBRD_PropertyList)
    'BRDctrl_ADC_FIFORESET'         :   brdBaseCode['BRDctrl_ADC'] + 128,  # ADC FIFO reset (NULL)
    'BRDctrl_ADC_ENABLE'            :   brdBaseCode['BRDctrl_ADC'] + 129,  # ADC enable / disable (U32)
    'BRDctrl_ADC_FIFOSTATUS'        :   brdBaseCode['BRDctrl_ADC'] + 130,  # Get ADC FIFO status (U32)
    'BRDctrl_ADC_GETDATA'           :   brdBaseCode['BRDctrl_ADC'] + 131,  # Get data buffer from ADC FIFO (BRD_DataBuf)
    'BRDctrl_ADC_ISBITSOVERFLOW'    :   brdBaseCode['BRDctrl_ADC'] + 132,  # Check overflow of ADC bits (U32)
    'BRDctrl_ADC_GETPRETRIGEVENT'   :   brdBaseCode['BRDctrl_ADC'] + 136,  # Get real start event by pretrigger mode (32-bit words) (U32)
    'BRDctrl_ADC_GETPREVENTPREC'    :   brdBaseCode['BRDctrl_ADC'] + 137,  # Get sample number for precision real start event by pretrigger mode (U32)
    'BRDctrl_ADC_GETSRCSTREAM'      :   brdBaseCode['BRDctrl_ADC'] + 138,  # Get source address for stream (U32)
    'BRDctrl_ADC_GETBLKCNT'         :   brdBaseCode['BRDctrl_ADC'] + 139,  # Get block number by restart mode (U32)
    'BRDctrl_ADC_ISCOMPLEX'         :   brdBaseCode['BRDctrl_ADC'] + 140,  # Check if real (0) or complex (1) ADC signal (U32)
    'BRDctrl_ADC_SETFC'             :   brdBaseCode['BRDctrl_ADC'] + 141,  # Set Fc if complex ADC signal (BRD_ValChan)
    'BRDctrl_ADC_GETFC'             :   brdBaseCode['BRDctrl_ADC'] + 142,  # Get Fc if complex ADC signal (BRD_ValChan)
    'BRDctrl_ADC_PREPARESTART'      :   brdBaseCode['BRDctrl_ADC'] + 143,  # Called before first 'BRDctrl_ADC_ENABLE' (NULL)
    'BRDctrl_ADC_CLRBITSOVERFLOW'   :   brdBaseCode['BRDctrl_ADC'] + 144,  # Clear ADC bit overflow flags (U32)
    'BRDctrl_ADC_ILLEGAL': None
}

"""
print('BRDctrl_ADC = \\')
print('{')
for key in brdAdcCode:
    if brdAdcCode[key] is not None:
        print(f"    0x{brdAdcCode[key]:04X}: '{key}',")
print('}')
"""

BRDctrl_ADC = \
{
    0x0A00: 'BRDctrl_ADC_SETCHANMASK',
    0x0A01: 'BRDctrl_ADC_GETCHANMASK',
    0x0A02: 'BRDctrl_ADC_SETGAIN',
    0x0A03: 'BRDctrl_ADC_GETGAIN',
    0x0A04: 'BRDctrl_ADC_SETINPRANGE',
    0x0A05: 'BRDctrl_ADC_GETINPRANGE',
    0x0A06: 'BRDctrl_ADC_SETBIAS',
    0x0A07: 'BRDctrl_ADC_GETBIAS',
    0x0A08: 'BRDctrl_ADC_SETFORMAT',
    0x0A09: 'BRDctrl_ADC_GETFORMAT',
    0x0A0A: 'BRDctrl_ADC_SETRATE',
    0x0A0B: 'BRDctrl_ADC_GETRATE',
    0x0A0C: 'BRDctrl_ADC_SETCLKMODE',
    0x0A0D: 'BRDctrl_ADC_GETCLKMODE',
    0x0A0E: 'BRDctrl_ADC_SETSYNCMODE',
    0x0A0F: 'BRDctrl_ADC_GETSYNCMODE',
    0x0A10: 'BRDctrl_ADC_SETMASTER',
    0x0A11: 'BRDctrl_ADC_GETMASTER',
    0x0A12: 'BRDctrl_ADC_SETSTARTMODE',
    0x0A13: 'BRDctrl_ADC_GETSTARTMODE',
    0x0A14: 'BRDctrl_ADC_SETSTDELAY',
    0x0A15: 'BRDctrl_ADC_GETSTDELAY',
    0x0A16: 'BRDctrl_ADC_SETACQDATA',
    0x0A17: 'BRDctrl_ADC_GETACQDATA',
    0x0A18: 'BRDctrl_ADC_SETSKIPDATA',
    0x0A19: 'BRDctrl_ADC_GETSKIPDATA',
    0x0A1A: 'BRDctrl_ADC_SETINPRESIST',
    0x0A1B: 'BRDctrl_ADC_GETINPRESIST',
    0x0A1C: 'BRDctrl_ADC_SETDCCOUPLING',
    0x0A1D: 'BRDctrl_ADC_GETDCCOUPLING',
    0x0A1E: 'BRDctrl_ADC_SETINPSRC',
    0x0A1F: 'BRDctrl_ADC_GETINPSRC',
    0x0A20: 'BRDctrl_ADC_SETINPMUX',
    0x0A21: 'BRDctrl_ADC_GETINPMUX',
    0x0A22: 'BRDctrl_ADC_SETDIFMODE',
    0x0A23: 'BRDctrl_ADC_GETDIFMODE',
    0x0A24: 'BRDctrl_ADC_SETSAMPLEHOLD',
    0x0A25: 'BRDctrl_ADC_GETSAMPLEHOLD',
    0x0A26: 'BRDctrl_ADC_SETGAINTUNING',
    0x0A27: 'BRDctrl_ADC_GETGAINTUNING',
    0x0A28: 'BRDctrl_ADC_SETTARGET',
    0x0A29: 'BRDctrl_ADC_GETTARGET',
    0x0A2A: 'BRDctrl_ADC_SETCLKPHASE',
    0x0A2B: 'BRDctrl_ADC_GETCLKPHASE',
    0x0A2C: 'BRDctrl_ADC_SETDBLCLK',
    0x0A2D: 'BRDctrl_ADC_GETDBLCLK',
    0x0A2E: 'BRDctrl_ADC_SELFCLBR',
    0x0A30: 'BRDctrl_ADC_SETPRETRIGMODE',
    0x0A31: 'BRDctrl_ADC_GETPRETRIGMODE',
    0x0A32: 'BRDctrl_ADC_SETCLKTHR',
    0x0A33: 'BRDctrl_ADC_GETCLKTHR',
    0x0A34: 'BRDctrl_ADC_SETCODE',
    0x0A35: 'BRDctrl_ADC_GETCODE',
    0x0A37: 'BRDctrl_ADC_GETCFG',
    0x0A38: 'BRDctrl_ADC_WRITEINIFILE',
    0x0A39: 'BRDctrl_ADC_READINIFILE',
    0x0A3A: 'BRDctrl_ADC_SETTESTMODE',
    0x0A3B: 'BRDctrl_ADC_GETTESTMODE',
    0x0A3C: 'BRDctrl_ADC_SETSPECIFIC',
    0x0A3D: 'BRDctrl_ADC_GETSPECIFIC',
    0x0A3E: 'BRDctrl_ADC_SETCLKLOCATION',
    0x0A3F: 'BRDctrl_ADC_GETCLKLOCATION',
    0x0A40: 'BRDctrl_ADC_SETEXTCLKTHR',
    0x0A41: 'BRDctrl_ADC_GETEXTCLKTHR',
    0x0A42: 'BRDctrl_ADC_SETTITLEMODE',
    0x0A43: 'BRDctrl_ADC_GETTITLEMODE',
    0x0A44: 'BRDctrl_ADC_SETTITLEDATA',
    0x0A45: 'BRDctrl_ADC_GETTITLEDATA',
    0x0A46: 'BRDctrl_ADC_SETCLKINV',
    0x0A47: 'BRDctrl_ADC_GETCLKINV',
    0x0A48: 'BRDctrl_ADC_SETINPFILTER',
    0x0A49: 'BRDctrl_ADC_GETINPFILTER',
    0x0A4A: 'BRDctrl_ADC_SETMU',
    0x0A4B: 'BRDctrl_ADC_GETMU',
    0x0A4C: 'BRDctrl_ADC_SETSTARTSLAVE',
    0x0A4D: 'BRDctrl_ADC_GETSTARTSLAVE',
    0x0A4E: 'BRDctrl_ADC_SETCLOCKSLAVE',
    0x0A4F: 'BRDctrl_ADC_GETCLOCKSLAVE',
    0x0A50: 'BRDctrl_ADC_GETCHANORDER',
    0x0A52: 'BRDctrl_ADC_SETTESTSEQ',
    0x0A53: 'BRDctrl_ADC_GETTESTSEQ',
    0x0A54: 'BRDctrl_ADC_WRDELAY',
    0x0A55: 'BRDctrl_ADC_RDDELAY',
    0x0A56: 'BRDctrl_ADC_RSTDELAY',
    0x0A7F: 'BRDctrl_ADC_GETDLGPAGES',
    0x0A80: 'BRDctrl_ADC_FIFORESET',
    0x0A81: 'BRDctrl_ADC_ENABLE',
    0x0A82: 'BRDctrl_ADC_FIFOSTATUS',
    0x0A83: 'BRDctrl_ADC_GETDATA',
    0x0A84: 'BRDctrl_ADC_ISBITSOVERFLOW',
    0x0A88: 'BRDctrl_ADC_GETPRETRIGEVENT',
    0x0A89: 'BRDctrl_ADC_GETPREVENTPREC',
    0x0A8A: 'BRDctrl_ADC_GETSRCSTREAM',
    0x0A8B: 'BRDctrl_ADC_GETBLKCNT',
    0x0A8C: 'BRDctrl_ADC_ISCOMPLEX',
    0x0A8D: 'BRDctrl_ADC_SETFC',
    0x0A8E: 'BRDctrl_ADC_GETFC',
    0x0A8F: 'BRDctrl_ADC_PREPARESTART',
    0x0A90: 'BRDctrl_ADC_CLRBITSOVERFLOW'
}

# ADC error codes
brdAdcErrCode = \
{
    'BRDerr_ADC_ILLEGAL_CHANNEL'    : brdBaseCode['BRDctrl_ADC'] | 0xC0000000,
    'BRDerr_ADC_ILLEGAL_GAIN'       : brdBaseCode['BRDctrl_ADC'] | 0xC0000001,
    'BRDerr_ADC_ILLEGAL_INPRANGE'   : brdBaseCode['BRDctrl_ADC'] | 0xC0000002,
    'BRDerr_ADC_UNDEFINED'          : None
}

BRDerr_ADC = \
    {
        0xC0000A00: 'BRDerr_ADC_ILLEGAL_CHANNEL',
        0xC0000A01: 'BRDerr_ADC_ILLEGAL_GAIN',
        0xC0000A02: 'BRDerr_ADC_ILLEGAL_INPRANGE',
    }

# #pragma pack(push, 1)    
# 
# // for set and get Start mode command
# typedef struct _BRD_AdcStartMode
# {
#     U32		src;			// Start source
# U32		inv;			// Start inversion
# REAL64	level;			// Start level (Comparator Threshold value)
# BRD_StartMode	stndStart;	// Standart Start mode
# } BRD_AdcStartMode, *PBRD_AdcStartMode;
# 
# ##define BRD_CHANCNT			2
# #define BRD_ADC_MAXINPUT	32
# 
# #const int BRD_CHANCNT		= 2; # Number of channels
# #const int BRD_ADC_MAXINPUT	= 32; # Maximum inputs numbers
# 
# # programming input multiplexer
# typedef struct _BRD_AdcInpMux
# {
#     U32			muxCnt;					# Multiplexer counter
# BRD_ValChan mux[BRD_ADC_MAXINPUT];	# For each input
# } BRD_AdcInpMux, *PBRD_AdcInpMux;
# 
# # programming input source
# typedef struct _BRD_AdcInpSrc
# {
#     U32		source;		# Source select
# REAL64	voltage;	# Voltage (ADM12x1M/3M/10M, ADM416x200)
# U32		chan;		# Channel number (ADM216x2M5)
# } BRD_AdcInpSrc, *PBRD_AdcInpSrc;
# 
# # programming delay
# /*typedef struct _BRD_AdcDelay
# {
#     U32		nodeID;		# node ID: ADC inputs, external start, SYNX's
# U32		value;		# delay value
# } BRD_AdcDelay, *PBRD_AdcDelay;
# */
# # delayed nodeID (look at ctrladmpro.h)
# #enum {
#        #	BRDnid_ADC0		= 0,	# node ID: ADC input 0
#                                                               #	BRDnid_ADC1		= 1,	# node ID: ADC input 1
#                                                                                                                      #	BRDnid_ADC2		= 2,	# node ID: ADC input 2
#                                                                                                                                                                             #	BRDnid_ADC3		= 3,	# node ID: ADC input 3
#                                                                                                                                                                                                                                    #	BRDnid_EXTSTART = 8,	# node ID: submodule exernal start
#                                                                                                                                                                                                                                                                                                #	BRDnid_SYNX0	= 12,	# node ID: SYNX input 0
#                                                                                                                                                                                                                                                                                                                                                      #	BRDnid_SYNX1	= 13,	# node ID: SYNX input 1
#                                                                                                                                                                                                                                                                                                                                                                                                            #	BRDnid_SYNX2	= 14,	# node ID: SYNX input 2
#                                                                                                                                                                                                                                                                                                                                                                                                                                                                  #	BRDnid_SYNX3	= 15,	# node ID: SYNX input 3
#                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        #};
# 
# # getting configuration
# typedef struct _BRD_AdcCfg
# {
#     U32		Bits;			# разрядность
# U32		Encoding;		# тип кодировки (0 - прямой, 1 - двоично-дополнительный, 2 - код Грея)
# U32		MinRate;		# минимальная частота дискретизации
# U32		MaxRate;		# максимальная частота дискретизации
# U32		InpRange;		# входной диапазон
# U32		FifoSize;		# размер FIFO АЦП (в байтах)
# U32		NumChans;		# число каналов
# U32		ChanType;		# тип канала (1 - RF (gain in dB))
# } BRD_AdcCfg, *PBRD_AdcCfg;
# 
# # programming specific parameters
# typedef struct _BRD_AdcSpec
# {
#     U32		command;	# command number
# PVOID	arg;		# pointer of argument
# } BRD_AdcSpec, *PBRD_AdcSpec;
# 
# 
# # Numbers of Specific Command
# typedef enum _ADC_NUM_CMD
# {
#     ADCcmd_ADJUST		= 0,
#                            ADCcmd_INHALF		= 1,
# } ADC_NUM_CMD;
# 
# #pragma pack(pop)    
# 
# #endif # _CTRL_ADC_H
# 
# #
# # End of file
# #