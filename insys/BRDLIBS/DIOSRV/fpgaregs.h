/*
 ****************** File FpgaRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADMFPGA
 *
 * (C) InSys by Dorokhin Andrey Apr, 2006
 *
 ************************************************************
*/

#ifndef _ADMFPGAREGS_H_
#define _ADMFPGAREGS_H_

#pragma pack(push,1)

// ADMFPGA Mode2 register (+10)
typedef union _ADMFPGA_MODE2 {
  ULONG AsWhole; // ADMFPGA Mode2 Register as a Whole Word
  struct { // ADMFPGA Mode2 Register as Bit Pattern
   ULONG VData		: 8,	// Pld data
		 VProg		: 1,	// 1 - reset PLD configuration
		 VProgEn	: 1,	// 1 - enable writing of VPROG
		 VClk		: 1,	// 1 - first signal VCLK
		 Reserved	: 1,	// Reserved
		 IdPld		: 1;	// not used
  } ByBits;
} ADMFPGA_MODE2, *PADMFPGA_MODE2;

// Numbers of Command Registers
typedef enum _ADMFPGA_NUM_CMD_REGS {
	ADMFPGAnr_MODE2		= 10, // 0x0A
} ADMFPGA_NUM_CMD_REGS;

// Numbers of Direct Registers
typedef enum _ADMFPGA_NUM_DIRECT_REGS {
	ADMFPGAnr_SIG		= 0x210,	// signature
	ADMFPGAnr_ID		= 0x211,	// ID of Pld
	ADMFPGAnr_MOD		= 0x212,	// modification of Pld
	ADMFPGAnr_VER		= 0x213,	// version of Pld
	ADMFPGAnr_TEST		= 0x214,	// test data
} ADMFPGA_NUM_DIRECT_REGS;

#pragma pack(pop)

#endif //_ADMFPGAREGS_H_
