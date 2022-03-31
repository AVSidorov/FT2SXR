/*
 ****************** File Dio16OutRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for DIO16OUT
 *
 * (C) InSys by Dorokhin Andrey Feb, 2006
 *
 ************************************************************
*/

#ifndef _DIO16OUTREGS_H_
#define _DIO16OUTREGS_H_

#pragma pack(push,1)

// DIO16OUT Mode2 register (+10)
typedef union _DIO16OUT_MODE2 {
  ULONG AsWhole; // DIO16OUT Mode2 Register as a Whole Word
  struct { // DIO16OUT Mode2 Register as Bit Pattern
   ULONG PhaseVal	: 8,	// Shift Phase Value
		 PhaseSign	: 1,	// Shift Phase Sign
		 ClkInv		: 1,	// Clock Inverting
		 Reserved	: 5,	// Reserved
		 PhaseEn	: 1;	// Shift Phase Enable
  } ByBits;
} DIO16OUT_MODE2, *PDIO16OUT_MODE2;

// Numbers of Command Registers
typedef enum _DIO16OUT_NUM_CMD_REGS {
	DIO16OUTnr_MODE2		= 10, // 0x0A
} DIO16OUT_NUM_CMD_REGS;

#pragma pack(pop)

#endif //_DIO16OUTREGS_H_
