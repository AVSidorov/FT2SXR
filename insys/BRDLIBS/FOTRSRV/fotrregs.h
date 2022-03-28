/******************* File FotrRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for FOTR
 *
 * (C) InSys by Dorokhin Andrey Jun, 2006
 *
 *************************************************************/

#ifndef _FOTRREGS_H_
#define _FOTRREGS_H_

#pragma pack(push,1)

// FOTR Status Register
typedef union _FOTR_STATUS {
	ULONG AsWhole; // FOTR Status Register as a Whole Word
	struct { // Status Register as Bit Pattern
		ULONG	CmdRdy			: 1, // 1 - Ready to do command
				FifoOutRdy		: 1, // 1 - Ready FIFO Output
				FifoOutEmpty	: 1, // 0 - Empty FIFO Output
				FifoOutReserved	: 1, // Reserved
				FifoInRdy		: 1, // 1 - Ready FIFO Input
				Reserved		: 4, // Reserved
				SfpPresent		: 1, // 1 - SFP Present
				SfpActive		: 1, // 1 - SFP Active
				TklActive		: 1, // 1 - TKL Active
				SfpFault		: 1, // 1 - SFP fault
				RmRst			: 1, // 1 - remote device send reset signal
				RmConnect		: 1, // 1 - remote device send data
				RmClkDone		: 1; // 1 - remote device clock is capture
	} ByBits;
} FOTR_STATUS, *PFOTR_STATUS;

// FOTR Mode1 Register (MODE1 +9)
typedef union _FOTR_MODE1 {
	ULONG AsWhole; // FOTR Mode1 Register as a Whole Word
	struct { // FOTR Mode1 Register as Bit Pattern
		ULONG	FifoOutRst	: 1, // FIFO output reset
				FifoInRst	: 1, // FIFO input reset
				ExtAddr		: 1, // 1 - external address mode
				Res0		: 1, // Reserved
				TestMode	: 2, // 01 - test command FIFO mode (only FOTR3G), 10 - test data FIFO mode (only FOTR3G)
//				TstCmd		: 1, // 1 - test command FIFO mode (only FOTR3G)
//				TstData		: 1, // 1 - test data FIFO mode (only FOTR3G)
				Res1		: 10; // Reserved
	} ByBits;
} FOTR_MODE1, *PFOTR_MODE1;

// FOTR Mode2 register (+10)
typedef union _FOTR_MODE2 {
  ULONG AsWhole; // FOTR Mode2 Register as a Whole Word
  struct { // FOTR Mode2 Register as Bit Pattern
   ULONG SfpEn		: 1,	// 1 - SFP Enable
		 LoopMode	: 1,	// 1 - Output connect to input (loop)
		 TestMode	: 1,	// 1 - test generator enable
		 Reserved	: 1,	// Reserved
		 TransOutRst: 1,	// 0 - Transport Output reset
		 TransInRst	: 1,	// 0 - Transport Input reset
		 LinkOutRst	: 1,	// 0 - Link Output reset
		 LinkInRst	: 1,	// 0 - Link Input reset
		 PhysOutRst	: 1,	// 0 - Physic Output reset
		 PhysInRst	: 1,	// 0 - Physic Input reset
		 PORstReq	: 1,	// 1 - Request of Physic Output reset signal
		 Reserved5	: 5;	// Reserved
  } ByBits;
} FOTR_MODE2, *PFOTR_MODE2;

// FOTR SFP EEPROM Control Register (SFPCTRL 0x204)
typedef union _FOTR_SFPROMCTRL {
	ULONG AsWhole; // FOTR SFP Control Register as a Whole Word
	struct { // FOTR SFP Control Register as Bit Pattern
		ULONG	RdCmd	: 1, // 1 - read EEPROM command
				WrCmd	: 1, // 1 - write EEPROM command
				Res0	: 6, // Reserved
				Addr	: 3, // address EEPROM area: 0 - bytes 0-255 at addres 0xA0, 1 - bytes 0-255 at addres 0xA2
				Res1	: 4, // Reserved
				Ready	: 1; // 1 - ready EEPROM for read/write
	} ByBits;
} FOTR_SFPROMCTRL, *PFOTR_SFPROMCTRL;

// Numbers of Command Registers
typedef enum _FOTR_NUM_CMD_REGS {
	FOTRnr_FOADRL		= 16, // 0x10
	FOTRnr_FOADRH		= 17, // 0x11
	FOTRnr_FOCNT		= 18, // 0x12
	FOTRnr_EXTADRL		= 19, // 0x13
	FOTRnr_EXTADRH		= 20, // 0x14
	FOTRnr_FDOADRL		= 21, // 0x15
	FOTRnr_FDOADRH		= 22, // 0x16
} FOTR_NUM_CMD_REGS;

// Numbers of Direct Registers
typedef enum _FOTR_NUM_DIRECT_REGS {
	FOTRnr_FLAGCLR		= 0x200,
	FOTRnr_SFPCTRL		= 0x204,	// 
	FOTRnr_SFPADDR		= 0x205,	// 
	FOTRnr_SFPDATA		= 0x206,	// 
	FOTRnr_RMFLAGL		= 0x210,	// 
	FOTRnr_RMFLAGH		= 0x211,	// 
	FOTRnr_LCFLAGL		= 0x212,	// 
	FOTRnr_LCFLAGH		= 0x213,	// 
	FOTRnr_FICNT		= 0x214,	// 
	FOTRnr_FICNTRD		= 0x215,	// 
	FOTRnr_LICNTERR		= 0x216,	// 
} FOTR_NUM_DIRECT_REGS;

#pragma pack(pop)

#endif //_FOTRREGS_H_
