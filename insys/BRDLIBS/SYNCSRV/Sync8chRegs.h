/*
 ****************** File sync8chRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for Sync8ch
 *
 * (C) InSys by Dorokhin A. Apr, 2009
 *
 ************************************************************
*/

#ifndef _SYNC8CHREGS_H_
 #define _SYNC8CHREGS_H_

#pragma pack(push,1)

// Sync8ch Control register
typedef union _SYNC8CH_CTRL1 {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG ClkEnE		: 1,	// CLKE enable
		 ClkEnF		: 1,	// CLKF enable
		 ClkEnG		: 1,	// CLKG enable
		 ClkStart	: 1,	// start of clock
		 Reserved0	: 4,	// Reserved
		 StartSl	: 1,	// 1 - start Slave mode
		 ClkSl		: 1,	// 1 - clock Slave mode
		 ClkInv		: 1,	// 1 - inverting clock
		 ClkSel		: 1,	// clock select
		 Reserved1	: 4;	// Reserved
  } ByBits;
} SYNC8CH_CTRL1, *PSYNC8CH_CTRL1;

// Sync8ch Strobe Control register
typedef union _SYNC8CH_STBCTRL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG StbAByPass	: 1,	// Strobe A channel by pass PLD
		 StbBByPass	: 1,	// Strobe B channel by pass PLD
		 StbCByPass	: 1,	// Strobe C channel by pass PLD
		 StbDByPass	: 1,	// Strobe D channel by pass PLD
		 Reserved0	: 4,	// Reserved
		 ClkDelay	: 3,	// time shift between strobes and CLKO
		 ClkDlySign	: 1,	// sign of time shift
		 StbClkInv	: 1,	// 0 - strobes resyncs rising CLKO, 1 - strobes resyncs falling CLKO
		 Reserved1	: 3;	// Reserved
  } ByBits;
} SYNC8CH_STBCTRL, *PSYNC8CH_STBCTRL;

// Sync8ch Strobe Enable register
typedef union _SYNC8CH_STBEN {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG StbAEn		: 1,	// Strobe A channel enable
		 StbBEn		: 1,	// Strobe B channel enable
		 StbCEn		: 1,	// Strobe C channel enable
		 StbDEn		: 1,	// Strobe D channel enable
		 StbCxxxEn	: 8,	// Strobe Cx channel enable
		 Reserved	: 4;	// Reserved
  } ByBits;
} SYNC8CH_STBEN, *PSYNC8CH_STBEN;

// Sync8ch Strobe Polarity register
typedef union _SYNC8CH_STBPOL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG StbAPol	: 1,	// Strobe A channel polarity
		 StbBPol	: 1,	// Strobe B channel polarity
		 StbCPol	: 1,	// Strobe C channel polarity
		 StbDPol	: 1,	// Strobe D channel polarity
		 StbCxxxPol	: 8,	// Strobe Cx channel polarity
		 Reserved	: 4;	// Reserved
  } ByBits;
} SYNC8CH_STBPOL, *PSYNC8CH_STBPOL;

// Sync8ch regs
typedef enum _SYNC8CH_NUM_CMD_REGS {
	SYNC8CHnr_FSRC		= 19, // 0x13
	SYNC8CHnr_FDVR0		= 20, // 0x14
	SYNC8CHnr_FDVR1		= 21, // 0x15
	SYNC8CHnr_CTRL1		= 23, // 0x17
	SYNC8CHnr_LOWPLL	= 24, // 0x18
	SYNC8CHnr_HIPLL		= 25, // 0x19
	SYNC8CHnr_FDVRE		= 26, // 0x1A
	SYNC8CHnr_FDVRF		= 27, // 0x1B
	SYNC8CHnr_FDVRG		= 28, // 0x1C
	SYNC8CHnr_STBCTRL	= 29, // 0x1D
	SYNC8CHnr_STBEN		= 30, // 0x1E
	SYNC8CHnr_STBPOL	= 31, // 0x1F
} SYNC8CH_NUM_CMD_REGS;

// Sync8ch read/write regs
typedef enum _SYNC8CH_NUM_DIRECT_REGS {
	SYNC8CHnr_DELSTB_AL		= 0x200,
	SYNC8CHnr_DELSTB_AH		= 0x201,
	SYNC8CHnr_WIDTHSTB_AL	= 0x202,
	SYNC8CHnr_WIDTHSTB_AH	= 0x203,
	SYNC8CHnr_CYCLSTB_AL	= 0x204,
	SYNC8CHnr_CYCLSTB_AH	= 0x205,
	SYNC8CHnr_DELSTB_BL		= 0x206,
	SYNC8CHnr_DELSTB_BH		= 0x207,
	SYNC8CHnr_WIDTHSTB_BL	= 0x208,
	SYNC8CHnr_WIDTHSTB_BH	= 0x209,
	SYNC8CHnr_CYCLSTB_BL	= 0x20A,
	SYNC8CHnr_CYCLSTB_BH	= 0x20B,
	SYNC8CHnr_DELSTB_CL		= 0x20C,
	SYNC8CHnr_DELSTB_CH		= 0x20D,
	SYNC8CHnr_WIDTHSTB_CL	= 0x20E,
	SYNC8CHnr_WIDTHSTB_CH	= 0x20F,
	SYNC8CHnr_CYCLSTB_CL	= 0x210,
	SYNC8CHnr_CYCLSTB_CH	= 0x211,
	SYNC8CHnr_DELSTB_DL		= 0x212,
	SYNC8CHnr_DELSTB_DH		= 0x213,
	SYNC8CHnr_WIDTHSTB_DL	= 0x214,
	SYNC8CHnr_WIDTHSTB_DH	= 0x215,
	SYNC8CHnr_CYCLSTB_DL	= 0x216,
	SYNC8CHnr_CYCLSTB_DH	= 0x217,
	SYNC8Pnr_SPDDEVICE		= 0x220,
	SYNC8Pnr_SPDCTRL		= 0x221,
	SYNC8Pnr_SPDADDR		= 0x222,
	SYNC8Pnr_SPDDATAL		= 0x223,
} SYNC8CH_NUM_DIRECT_REGS;

// ADF4106 PLL Reference Counter Latch
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

// ADF4106 PLL N Counter Latch
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

// ADF4106 PLL Function Latch
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

const double PLL_F_CMP_MAX = 100.e6; // 100. MHz

#pragma pack(pop)

#endif //_SYNC8CHREGS_H_

//
// End of file
//