/*
 ****************** File dr16regs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for DR16
 *
 * (C) InSys by Sklyarov A. Dec, 2009
 *
 ************************************************************
*/

#ifndef _DR16REGS_H_
 #define _DR16REGS_H_

#pragma pack(push,1)

// DR16 Control1 register
typedef union _DR16_CLK_CONTROL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG ClkSource	: 3,	// Source Of Clock: 0-5
		 DivVco		: 2,	// Divider VCO: 0-3
		 DivClkAdc	: 3,	// Divider Clock ADC: 0-4
		 Res		: 8;	// Reserved
  } ByBits;
} DR16_CLK_CONTROL, *PDR16_CLK_CONTROL;

typedef union _DR16_START_SYNC {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Sel_ST0	: 1,	// 
		 Sel_ST1	: 1,	// Divider VCO: 0-3
		 Sel_ST2	: 1,	// Divider Clock ADC: 0-4
		 Res		: 11,	// Reserved
		 ResetJtag	: 1,	//
		 RestartDiv	: 1;	//
  } ByBits;
} DR16_START_SYNC, *PDR16_START_SYNC;

// DR16 Control1 register
typedef union _DR16_TP_CONTROL_L {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
	  ULONG PeriodTpL	: 16;	// Period Tp
  } ByBits;
} DR16_TP_CONTROL_L, *PDR16_TP_CONTROL_L;
// DR16 Control1 register
typedef union _DR16_TP_CONTROL_H {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG PeriodTpH	: 14,	// Period Tp
		 InvExtTp	: 1,	// Enable Tp
		 EnableTp	: 1;	// Enable Tp
  } ByBits;
} DR16_TP_CONTROL_H, *PDR16_TP_CONTROL_H;

// Dr16  regs
typedef enum _DR16_NUM_CMD_REGS {
	DDSnr_START_SYNC		= 18, 
	DDSnr_CLK_CONTROL		= 20, 
	DDSnr_TP_CONTROL_L		= 0x201, 
	DDSnr_TP_CONTROL_H		= 0x202, 
} DR16_NUM_CMD_REGS;


#pragma pack(pop)

#endif //_DR16REGS_H_

//
// End of file
//