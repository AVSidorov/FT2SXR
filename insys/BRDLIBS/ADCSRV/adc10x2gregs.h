/*
 ****************** File Adc10x2gRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADC10x2G
 *
 * (C) InSys by Dorokhin Andrey Jun, 2005
 *
 *	15.03.2006 - add changies by ver. 2.0
 *	14.10.2008 - add changies by ver. 2.2
 *
 ************************************************************
*/

#ifndef _ADC10x2GREGS_H_
 #define _ADC10x2GREGS_H_

#pragma pack(push,1)

// ADC Control register (+23)
typedef union _ADC_CTRL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
//   ULONG Pge		: 1,	// 1 - ADC testing sequence
//		 Bist		: 1,	// 1 - MUX testing sequence
   ULONG Test		: 3,	// test mode: 1 - ADC testing sequence, 2 - MUX testing sequence, 4 - calibration by zero (ver 2.0 only)
		 Res		: 1;	// not use
   ULONG StartSrc	: 3,	// select source of start: 0 - channel, 1 - external input START IN, 2 - external input EXT START IN, 3 - program
		 StartInv	: 1,	// 1 - inverting start
		 StartSl	: 1,	// 1 - start Slave mode (ver 2.2+ only)
		 ClkSl		: 1,	// 1 - clock Slave mode (ver 2.2+ only)
		 TimerSync	: 1,	// 1 - timer sync with start signal (ver 2.2+ only)
		 AccEn		: 1,	// 1 - accumulator enable (ver spec only)
		 Res1		: 4;	// not use
  } ByBits;
} ADC_CTRL, *PADC_CTRL;

// LMX2350 PLL IF_R Register (v. 2.0)
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

// LMX2350 PLL RF_R Register (v. 2.0)
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

// LMX2350 PLL RF_N Register (v. 2.0)
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

const float PLL_F_CMP_MIN = 0.125e6; // 125 êÃö 
const float PLL_F_CMP_MAX = 5.e6; // 5. MHz

const int PLL_P_DIV_16 = 16;
const int PLL_P_DIV_32 = 32;

// ADF4106 PLL Reference Counter Latch (v. 2.1)
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

// ADF4106 PLL N Counter Latch (v. 2.1)
typedef union _PLL_NCNT {
  DWORD AsWhole; // Register as a Whole Word
  struct { // Register as Bit Pattern
    WORD LoWord;
    WORD HiWord;
  } ByWord;
  struct { // Register as Bit Pattern
    UINT Addr		: 2,	// address 01
		 A_CNTR		: 6,	// 
		 B_CNTR		: 13,	//
		 CP_GAIN	: 1,	//
		 Res1		: 2;	//
  } ByBits;
} PLL_NCNT, *PPLL_NCNT;

// ADF4106 PLL Function Latch (v. 2.1)
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
		 PRESCALER	: 2;	//
  } ByBits;
} PLL_FUNC, *PPLL_FUNC;

// v. 2.1
//const float PLL_F_CMP_MIN = 0.025e6; // 125 êÃö 
const double PLL_V21_F_CMP_MAX = 200.e6; // 200. MHz

// Numbers of Direct Registers
typedef enum _ADC10X2G_NUM_DIRECT_REGS {
	ADC10X2Gnr_STABILITY	= 0x207,
} ADC10X2G_NUM_DIRECT_REGS;

#pragma pack(pop)

#endif //_ADC10x2GREGS_H_

//
// End of file
//