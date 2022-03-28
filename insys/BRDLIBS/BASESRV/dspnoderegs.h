/*
 ****************** File DspNodeRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for DSP node
 *
 * (C) InSys by Dorokhin Andrey Jun, 2005
 *
 ************************************************************
*/

#ifndef _DSPNODEREGS_H_
#define _DSPNODEREGS_H_

#pragma pack(push,1)

// mode register (9)
typedef union _IN_MODE1 {
  ULONG AsWhole;
  struct {
   ULONG Res	: 4,  // not use
		 Flow	: 1,  // 0 - into PLD, 1 - from ADM PLD to DSP PLD
		 Res1	: 11;  // not use
  } ByBits;
} IN_MODE1, *PIN_MODE1;

// Numbers of Command Registers
typedef enum _IN_NUM_CMD_REGS {
	INnr_MODE1		= 9, // 0x09
	INnr_SFLAG		= 12, // 0x0Ñ
} IN_NUM_CMD_REGS;

// Numbers of Command Registers
typedef enum _OUT_NUM_CMD_REGS {
	OUTnr_SFLAG		= 12, // 0x0Ñ
} OUT_NUM_CMD_REGS;

// High address & command register (26)
typedef union _SBSRAM_ADDRH {
  ULONG AsWhole;
  struct {
   ULONG Addr	: 5,  // Read Memory Mode : 0 - auto, 1 - random
		 Res	: 9,  // not use
		 Cmd	: 1,  // 0 - write, 1 - read
		 Bank	: 1;  // number of memory bank (0, 1)
  } ByBits;
} SBSRAM_ADDRH, *PSBSRAM_ADDRH;

// read/write memory commands
typedef enum _SBSRAM_CMD {
	SBSRAMcmd_WRITE	= 0,
	SBSRAMcmd_READ	= 1
} SBSRAM_CMD;

#define SBSRAM_MAXCHIPS 4

// Numbers of Command Registers
typedef enum _SBSRAM_NUM_CMD_REGS {
	SBSRAMnr_DATAL		= 23, // 0x17
	SBSRAMnr_DATAH		= 24, // 0x18
	SBSRAMnr_ADDRL		= 25, // 0x19
	SBSRAMnr_ADDRH		= 26, // 0x1A
} SBSRAM_NUM_CMD_REGS;

#pragma pack(pop)

#endif //_DSPNODEREGS_H_

//
// End of file
//