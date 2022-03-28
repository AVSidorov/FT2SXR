/*
 ****************** File Adc28x1gRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADC28X1G
 *
 * (C) InSys by Dorokhin Andrey Jan, 2007
 *
 ************************************************************
*/

#ifndef _ADC28X1GREGS_H_
 #define _ADC28X1GREGS_H_

#pragma pack(push,1)

// ADC Frequency Divider register (+20)
typedef union _ADC_FDVR {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Div	: 3,	// 0x0 - 1, 0x1 - 2, 0x2 - 4, 0x3 - 8
		 Res	: 13;	// not use
  } ByBits;
} ADC_FDVR, *PADC_FDVR;

// ADC Control register (+23)
typedef union _ADC_CTRL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Chan1Src	: 1,	// select source of channel 1
		 DblClk		: 1,	// 1 - double clock mode
		 Res		: 2;	// not use
   ULONG StartSrc	: 3,	// select source of start: 0 - channel 0, 1 - channel 1, 2 - external input X1, 3 - program
		 StartInv	: 1,	// 1 - inverting start
		 StartClkSl	: 1,	// 1 - start & clock Slave mode
		 Res0		: 1,	// not use
		 AdcRegEn	: 1,	// 1 - internal ADC registers enable
		 Res1		: 5;	// not use
  } ByBits;
} ADC_CTRL, *PADC_CTRL;

// LMX2350 PLL IF_R Register
typedef union _PLL_IF_R {
  DWORD AsWhole; // Register as a Whole Word
  struct { // Register as Words
    WORD LoWord;
    WORD HiWord;
  } ByWord;
  struct { // Register as Bit Pattern
    UINT Addr		: 2,	// address 00
		 IF_R_CNTR	: 15,	//
		 IF_CP_WORD	: 2,	//
		 FoLD		: 3,	//
		 FRAC_16	: 1,	//
		 OSC		: 1;	//
  } ByBits;
} PLL_IF_R, *PPLL_IF_R;

typedef enum _PLL_FOLD {
  PLL_foldLockRF = 2,
  PLL_foldCntRF_R = 6,
  PLL_foldCntRF_N = 7
} PLL_FOLD; 

// LMX2350 PLL RF_R Register
typedef union _PLL_RF_R {
  DWORD AsWhole; // Register as a Whole Word
  struct { // Register as Bit Pattern
    WORD LoWord;
    WORD HiWord;
  } ByWord;
  struct { // Register as Bit Pattern
    UINT Addr		: 2,	// address 10
		 RF_R_CNTR	: 15,	//
		 RF_CP_WORD	: 5,	//
		 V2_EN		: 1,	//
		 DLL_MODE	: 1;	//
  } ByBits;
} PLL_RF_R, *PPLL_RF_R;

typedef enum _PLL_RF_PD_POL {
  PLL_pdpolNegative,
  PLL_pdpolPositive
} PLL_RF_PD_POL;

// LMX2350 PLL RF_N Register
typedef union _PLL_RF_N {
  DWORD AsWhole; // Register as a Whole Word
  struct { // Register as Bit Pattern
    WORD LoWord;
    WORD HiWord;
  } ByWord;
  struct { // Register as Bit Pattern
    UINT Addr		 : 2,	// address 11
		 FRAC_CNTR	 : 4,	//
		 RF_NA_CNTR  : 5,	//
		 RF_NB_CNTR	 : 10,	//
		 RF_CTL_WORD : 3;	//
  } ByBits;
} PLL_RF_N, *PPLL_RF_N;

// 2G
//const float PLL_F_CMP_MIN = 0.125e6; // 125 êÃö 
//const float PLL_F_CMP_MAX = 5.e6; // 5. MHz

// 1G
//const float PLL_F_CMP_MIN = 0.5e6; // 0.5 MHz
const float PLL_F_CMP_MIN = 0.125e6; // 0.125 MHz
const float PLL_F_CMP_MAX = 20.e6; // 20. MHz

const int PLL_P_DIV_16 = 16;
const int PLL_P_DIV_32 = 32;

// ADC Internal Control Register (000)
typedef union _ADC_CTRL0 {
  USHORT AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   USHORT Standby	: 2,	// (D0, D1)
		  Encode	: 1,	// (D2) 0 - binary, 1 - gray
		  Dmux		: 1,	// (D3)
		  InputMode	: 2,	// (D4, D5)
		  ClkMode	: 2,	// (D6, D7)
		  Decim		: 1,	// (D8)
		  Res		: 1,	// (D9)
		  Calibr	: 2,	// (D10, D11)
		  WaitClbr	: 2,	// (D12, D13)
		  FDataReady: 1,	// (D14)
		  Res1		: 1;	// (D15)
  } ByBits;
} ADC_CTRL0, *PADC_CTRL0;

// ADC Internal Control Register (001)
typedef union _ADC_CTRL1 {
  USHORT AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   USHORT Gain0		: 8,	// gain channel I
		  Gain1		: 8;	// gain channel Q
  } ByBits;
} ADC_CTRL1, *PADC_CTRL1;

// ADC Internal Control Register (002)
typedef union _ADC_CTRL2 {
  USHORT AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   USHORT Offset0	: 8,	// offset channel I
		  Offset1	: 8;	// offset channel Q
  } ByBits;
} ADC_CTRL2, *PADC_CTRL2;

// ADC Internal Control Register (003)
typedef union _ADC_CTRL3 {
  USHORT AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   USHORT GainComp	: 7,	// gain compensation (Q is matched to I)
		  Res		: 9;	// reserved
  } ByBits;
} ADC_CTRL3, *PADC_CTRL3;

// ADC Internal Control Register (004)
typedef union _ADC_CTRL4 {
  USHORT AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   USHORT ISA0		: 3,	// channel I
		  ISA1		: 3,	// channel Q
		  Res1		: 10;	// must = 1000010000 (binary)
  } ByBits;
} ADC_CTRL4, *PADC_CTRL4;
/*
// ADC Internal Control Register (005)
typedef union _ADC_CTRL5 {
  USHORT AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   USHORT Isa0		: 3,	// channel I
		  Isa1		: 3,	// channel Q
		  Res1		: 10;	// must = 1000010000 (binary)
  } ByBits;
} ADC_CTRL5, *PADC_CTRL5;

// ADC Internal Control Register (006)
typedef union _ADC_CTRL6 {
  USHORT AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   USHORT Isa0		: 3,	// channel I
		  Isa1		: 3,	// channel Q
		  Res1		: 10;	// must = 1000010000 (binary)
  } ByBits;
} ADC_CTRL6, *PADC_CTRL6;
*/
// ADC Internal Control Register (007)
typedef union _ADC_CTRL7 {
  USHORT AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   USHORT DRDA0		: 3,	// clock I
		  DRDA1		: 3,	// clock Q
		  FiSDA		: 5,	// channel Q
		  Res1		: 5;	// reserved
  } ByBits;
} ADC_CTRL7, *PADC_CTRL7;

/*
// Numbers of ADC Internal Control Registers
typedef enum _ADC28X1G_NUM_CMD_REGS {
	ADCCtrlnr_MAINREG		= 0, // 
	ADCCtrlnr_GAINADJUST	= 1, //
	ADCCtrlnr_BIASREG		= 2, //
	ADCCtrlnr_GAINCOMP		= 3, //
	ADCCtrlnr_ISA			= 4, //
	ADCCtrlnr_TESTABILITY	= 5, //
	ADCCtrlnr_BIT			= 6, //
	ADCCtrlnr_BIT			= 6, //
} ADC28X1G_NUM_CMD_REGS;
*/
// Numbers of Command Registers
typedef enum _ADC28X1G_NUM_CMD_REGS {
	ADC28X1Gnr_REGADDR	= 26, // 0x1A
	ADC28X1Gnr_REGDATA	= 27, // 0x1B
} ADC28X1G_NUM_CMD_REGS;

#pragma pack(pop)

#endif //_ADC28X1GREGS_H_

//
// End of file
//