/*
 ****************** File TestExRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for TEST_CTRL
 *
 * (C) InSys by Dorokhin Andrey Feb, 2008
 *
 ************************************************************
*/

#ifndef _TESTEXREGS_H_
#define _TESTEXREGS_H_

#pragma pack(push,1)

// Multiplexer control register (0x0F)
typedef union _TESTEX_MUX {
  ULONG AsWhole; // Register as a Whole Word
  struct { // Register as Bit Pattern
   ULONG OutMux		: 4, // output sources multiplexer
		 Res		: 4; // reserved
  } ByBits;
} TESTEX_MUX, *PTESTEX_MUX;

// Ñheck unit control register (0x1C)
typedef union _TESTEX_CHECK {
  ULONG AsWhole; // Register as a Whole Word
  struct { // Register as Bit Pattern
   ULONG Reset		: 1, // reset of check unit
		 Res		: 4, // reserved
		 Start		: 1, // 1 - enable of check unit
		 Res1		: 1, // reserved
		 FixMode	: 1, // 1 - fixed of block type
		 BlockType	: 4, // type of block
		 Res2		: 4; // reserved
  } ByBits;
} TESTEX_CHECK, *PTESTEX_CHECK;

// Generation unit control register (0x1E)
typedef union _TESTEX_GEN {
  ULONG AsWhole; // Register as a Whole Word
  struct { // Register as Bit Pattern
   ULONG Reset		: 1, // reset of generation unit
		 Res		: 4, // reserved
		 Start		: 1, // 1 - enable of generation unit
		 CntEnbl	: 1, // 1 - enable counters of generation unit
		 FixMode	: 1, // 1 - fixed of block type
		 BlockType	: 4, // type of block
		 Res2		: 3, // reserved
		 Ready		: 1; // 1 - force set FIFO ready
  } ByBits;
} TESTEX_GEN, *PTESTEX_GEN;

// Ñheck unit address of error memory register (0x218)
typedef union _TESTEX_CHECKERADR {
  ULONG AsWhole; // Register as a Whole Word
  struct { // Register as Bit Pattern
   ULONG AdrWord	: 4, // address of word 
		 NumError	: 4, // number of error
		 Res		: 8; // reserved
  } ByBits;
} TESTEX_CHECKERADR, *PTESTEX_CHECKERADR;

// Numbers of Command Registers
typedef enum _TESTEX_NUM_CMD_REGS {
	TESTEXnr_MUXCTRL		= 15, // 0x0F - multiplexer control
	TESTEXnr_GENENBLCNT		= 26, // 0x1A - enable counter of generation unit
	TESTEXnr_GENDISCNT		= 27, // 0x1B - disable counter of generation unit
	TESTEXnr_CHKCTRL		= 28, // 0x1Ñ - ñheck unit control
	TESTEXnr_CHKSIZE		= 29, // 0x1D - block size of ñheck unit
	TESTEXnr_GENCTRL		= 30, // 0x1E - generation unit control
	TESTEXnr_GENSIZE		= 31, // 0x1F - block size of generation unit
} TESTEX_NUM_CMD_REGS;

// Numbers of Direct Registers
typedef enum _TESTEX_NUM_DIRECT_REGS {
	TESTEXnr_CHKRDBLKL		= 0x210,	// read block counter of check unit (low part)
	TESTEXnr_CHKRDBLKH		= 0x211,	// read block counter of check unit (high part)
	TESTEXnr_CHKOKBLKL		= 0x212,	// OK read block counter of check unit (low part)
	TESTEXnr_CHKOKBLKH		= 0x213,	// OK block counter of check unit (high part)
	TESTEXnr_CHKERRBLKL		= 0x214,	// error read block counter of check unit (low part)
	TESTEXnr_CHKERRBLKH		= 0x215,	// error read block counter of check unit (high part)
	TESTEXnr_CHKTOTALERRL	= 0x216,	// total error counter of check unit (low part)
	TESTEXnr_CHKTOTALERRH	= 0x217,	// total error counter of check unit (high part)
	TESTEXnr_CHKERRADDR		= 0x218,	// error memory address of check unit (WR)
	TESTEXnr_CHKERRDATA		= 0x219,	// error memory data of check unit (RD)
	TESTEXnr_GENWRBLKL		= 0x21A,	// write block counter of generation unit (low part)
	TESTEXnr_GENWRBLKH		= 0x21B,	// write block counter of generation unit (high part)
} TESTEX_NUM_DIRECT_REGS;

#pragma pack(pop)

#endif //_TESTEXREGS_H_
