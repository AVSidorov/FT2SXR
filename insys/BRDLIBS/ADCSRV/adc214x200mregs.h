/*
 ****************** File Adc214x200mRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADC214X200M
 *
 * (C) InSys by Dorokhin Andrey Jan, 2007
 *
 ************************************************************
*/

#ifndef _ADC214X200MREGS_H_
 #define _ADC214X200MREGS_H_

#pragma pack(push,1)

// ADC Frequency Divider register (+20)
typedef union _ADC_FDVR {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Div	: 4,	// 0x0 - 1, 0x1 - 2, 0x2 - 4, 0x3 - 8, 0x4 - 16, 0x5 - 32, 0x6 - 64, 0x7 - 128, 0x8 - 256
		 Res	: 12;	// not use
  } ByBits;
} ADC_FDVR, *PADC_FDVR;

// Gain register for RF channels (+21)
typedef union _ADC_GAIN {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Chan0	: 5,	// gain of channel 0 (0 - largest range voltage)
		 Res0	: 3,	// 
		 Chan1	: 5,	// gain of channel 1 (0 - largest range voltage)
		 Res1	: 3;	// 
  } ByBits;
} ADC_GAIN, *PADC_GAIN;

// ADC Control register (+23)
typedef union _ADC_CTRL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Res0		: 2,	// not use
		 Clbr		: 1,	// 1 - calibration by zero
		 Res		: 1;	// not use
   ULONG StartSrc	: 3,	// select source of start: 0 - channel 0, 1 - channel 1, 2 - external input X5, 3 - program
		 StartInv	: 1,	// 1 - inverting start
		 StartClkSl	: 1,	// 1 - start & clock Slave mode
		 Res1		: 7;	// not use
  } ByBits;
} ADC_CTRL, *PADC_CTRL;

// ADF4002 PLL Reference Counter Latch
typedef union _PLL_RCNT {
  DWORD AsWhole; // Register as a Whole Word
  struct { // Register as Words
    WORD LoWord;
    WORD HiWord;
  } ByWord;
  struct { // Register as Bit Pattern
    UINT Addr		: 2,	// address 00
		 R_CNTR		: 14,	//
		 AB_WIDTH	: 2,	//
		 TEST_MODE	: 2,	//
		 LDP		: 1,	//
		 Res		: 3;	// must be 0
  } ByBits;
} PLL_RCNT, *PPLL_RCNT;

// ADF4002 PLL N Counter Latch
typedef union _PLL_NCNT {
  DWORD AsWhole; // Register as a Whole Word
  struct { // Register as Bit Pattern
    WORD LoWord;
    WORD HiWord;
  } ByWord;
  struct { // Register as Bit Pattern
    UINT Addr		: 2,	// address 01
		 Res		: 6,	// reserved
		 N_CNTR		: 13,	//
		 CP_GAIN	: 1,	//
		 Res1		: 2;	//
  } ByBits;
} PLL_NCNT, *PPLL_NCNT;

// ADF4002 PLL Function Latch
typedef union _PLL_FUNC {
  DWORD AsWhole; // Register as a Whole Word
  struct { // Register as Bit Pattern
    WORD LoWord;
    WORD HiWord;
  } ByWord;
  struct { // Register as Bit Pattern
    UINT Addr		: 2,	// address 10
		 CNT_RST	: 1,	//
		 PWDN_1		: 1,	//
		 MUX_OUT	: 3,	//
		 PD_POL		: 1,	//
		 CP_3STATE	: 1,	//
		 FL_ENBL	: 1,	//
		 FL_MODE	: 1,	//
		 TIMER_CNT	: 4,	//
		 CUR_SET_1	: 3,	//
		 CUR_SET_2	: 3,	//
		 PWDN_2		: 1,	//
		 Res		: 2;	//
  } ByBits;
} PLL_FUNC, *PPLL_FUNC;

// 200M
const float PLL_F_CMP_MIN = 0.025e6; // 25 êÃö 
//const float PLL_F_CMP_MAX = 50.e6; // 50. MHz
const float PLL_F_CMP_MAX = 200.e6; // 200 MHz

// ADC Internal Control Register (0x63)
typedef union _ADC_STBY_DF {
  UCHAR AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   UCHAR Res		: 3,	// (D0-D2)
		 Encode		: 1,	// (D3) 0 - 2's complement, 1 - binary
		 Res1		: 3,	// (D4-D6)
		 Standby	: 1;	// (D7)
  } ByBits;
} ADC_STBY_DF, *PADC_STBY_DF;

// ADC Internal Control Register (0x6D)
typedef union _ADC_REF_PWR {
  UCHAR AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   UCHAR Res		: 4,	// (D0-D3)
		 Ref		: 1,	// (D4) 0 - internal reference, 1 - external reference
		 PwrScal	: 3;	// (D5-D7) Power Scalling
  } ByBits;
} ADC_REF_PWR, *PADC_REF_PWR;

// ADC Internal Control Register (0x7E)
typedef union _ADC_CUR_TERM {
  UCHAR AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   UCHAR Current	: 2,	// (D0-D1) 0 - 3.5 mA, 1 - 2.5 mA, 2 - 4.5 mA, 3 - 1.75 mA
		 ClkTerm	: 3,	// (D2-D4) Clock Termination
		 DataTerm	: 3;	// (D5-D7) Data Termination
  } ByBits;
} ADC_CUR_TERM, *PADC_CUR_TERM;

// Numbers of Command Registers
typedef enum _ADC214X200M_NUM_CMD_REGS {
	ADC214X200Mnr_REGADDR	= 26, // 0x1A
	ADC214X200Mnr_REGDATA	= 27, // 0x1B
} ADC214X200M_NUM_CMD_REGS;

#pragma pack(pop)

#endif //_ADC214X200MREGS_H_

//
// End of file
//