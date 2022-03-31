/*
 ****************** File Dio16InRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for DIO16IN
 *
 * (C) InSys by Dorokhin Andrey Feb, 2006
 *
 ************************************************************
*/

#ifndef _DIO16INREGS_H_
#define _DIO16INREGS_H_

#pragma pack(push,1)

// DIO16IN Mode2 register (+10)
typedef union _DIO16IN_MODE2 {
  ULONG AsWhole; // DIO16IN Mode2 Register as a Whole Word
  struct { // DIO16IN Mode2 Register as Bit Pattern
   ULONG PhaseVal	: 8,	// Shift Phase Value
		 PhaseSign	: 1,	// Shift Phase Sign
		 ClkInv		: 1,	// Clock Inverting
		 Reserved	: 5,	// Reserved
		 PhaseEn	: 1;	// Shift Phase Enable
  } ByBits;
} DIO16IN_MODE2, *PDIO16IN_MODE2;

// DIO16IN Control register (+23)
typedef union _DIO16IN_CTRL {
  ULONG AsWhole; // DIO16IN Control Register as a Whole Word
  struct { // DIO16IN Control Register as Bit Pattern
   ULONG SingleMode	: 1,	// 1 - single transfer mode
		 ReadWrite	: 1,	// 1 - write data, 0 - read data
		 AddrData	: 1,	// 1 - address, 0 - data
		 Reserved0	: 1,	// Reserved
		 TestMode	: 1,	// 1 - test mode
		 Reserved1	: 6;	// Reserved
  } ByBits;
} DIO16IN_CTRL, *PDIO16IN_CTRL;

// Numbers of Command Registers
typedef enum _DIO16IN_NUM_CMD_REGS {
	DIO16INnr_MODE2		= 10, // 0x0A
	DIO16INnr_CTRL1		= 23, // 0x17
} DIO16IN_NUM_CMD_REGS;

// Numbers of Direct Registers
typedef enum _DIO16IN_NUM_DIRECT_REGS {
	DIO16INnr_READ		= 0x202,	// read data register
	DIO16INnr_WRITE		= 0x203,	// write data register
	DIO16INnr_RDCTRL	= 0x204,	// read start register
} DIO16IN_NUM_DIRECT_REGS;

#pragma pack(pop)

#endif //_DIO16INREGS_H_
