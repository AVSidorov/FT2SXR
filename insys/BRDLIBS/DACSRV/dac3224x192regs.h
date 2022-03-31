/*
 ****************** File Dac3224x192Regs.h *******************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for DAC3224X192
 *
 * (C) InSys by Dorokhin Andrey Oct, 2005
 *
 *************************************************************
*/

#ifndef _DAC3224X192REGS_H_
 #define _DAC3224X192REGS_H_

#pragma pack(push,1)

// DAC Mode2 Register (MODE2 +10)
typedef union _DAC_MODE2 {
	ULONG AsWhole; // Mode1 Register as a Whole Word
	struct { // Mode1 Register as Bit Pattern
		ULONG	ProgMask	: 4,  // mask for programming DAC chips
				Res			: 12; // Reserved
	} ByBits;
} DAC_MODE2, *PDAC_MODE2;

// DAC Control register (+23)
typedef union _DAC_CTRL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG ClkDivMode	: 2,	// clock divide mode
		 Res		: 2,	// not use
		 Sync		: 1,	// 0 -> 1 - syncro impulse generate
		 ChipsReset	: 1,	// 1 - reset DAC chips
		 Res0		: 10;	// not use
  } ByBits;
} DAC_CTRL, *PDAC_CTRL;

// DAC Chip Data Register
typedef union _DAC_CHIPDATA {
	ULONG AsWhole; // Mode1 Register as a Whole Word
	struct { // Mode1 Register as Bit Pattern
		ULONG	Data	: 8,  // data for writing DAC chips
				Res		: 8; // not use
	} ByBits;
} DAC_CHIPDATA, *PDAC_CHIPDATA;

// DAC Chip Address and control Register
typedef union _DAC_CHIPCTRL {
	ULONG AsWhole; // Mode1 Register as a Whole Word
	struct { // Mode1 Register as Bit Pattern
		ULONG	Addr	: 5,  // address for writing DAC chips
				Ctrl	: 11; // always "00110010000"
	} ByBits;
} DAC_CHIPCTRL, *PDAC_CHIPCTRL;

#define DAC_MAGIC_CTRL 0x190 // волшебная константа для записи в поле Ctrl

// Numbers of Direct Registers
typedef enum _DAC192_NUM_DIRECT_REGS {
	DAC192nr_CHIPDATA		= 0x201,
	DAC192nr_CHIPADDRCTRL	= 0x202,
} DAC192_NUM_DIRECT_REGS;

#pragma pack(pop)

#endif //_DAC3224X192REGS_H_

//
// End of file
//