/*
 ****************** File Qm9857Regs.h *******************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for QM9857
 *
 * (C) InSys by Sklyarov A. Sep. 2006
 *
 *************************************************************
*/

#ifndef _QM9857REGS_H_
 #define _QM9857REGS_H_

#pragma pack(push,1)

// QM Control1 register (+0)
typedef union _QM_MODE0 {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Reset		: 1,	// reset
		 FifoRst	: 1,	// reset fifo
		 IrqEnb		: 1,	// enable IRQ
		 DrqEnb		: 1,	// enable DMA req
		 Master		: 1,	// master/slave
		 Start		: 1,	// start
		 Res		: 10;	// not use
  } ByBits;
} QM_MODE0, *PQM_MODE0;

// QM Mode3 Register (+11)
typedef union _QM_MODE3 {
	ULONG AsWhole; // Mode3 Register as a Whole Word
	struct { // Mode1 Register as Bit Pattern
	ULONG	Res0		: 2, // Reserved
    		ProgMask	: 2,  // mask for programming QM chips
			Res1		: 12; // Reserved
	} ByBits;
} QM_MODE3, *PQM_MODE3;

// QM Control1 register (+23)
typedef union _QM_CONTROL1 {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG SelProf	: 2,	// select profile
		 Res0		: 2,	// not use
		 FreqUpd	: 2,	// update freq
		 Res1		: 2,	// not use
		 ChipReset	: 1,	// 1 - reset DAC chips
		 Res2		: 7;	// not use
  } ByBits;
} QM_CONTROL1, *PQM_CONTROL1;

// DAC Chip Data Register
typedef union _QM_CHIPDATA {
	ULONG AsWhole; //  Register as a Whole Word
	struct { // Mode1 Register as Bit Pattern
		ULONG	Data	: 8,  // data for writing DAC chips
				Addr	: 8;  // number of register
	} ByBits;
} QM_CHIPDATA, *PQM_CHIPDATA;


// Qm read/write regs
typedef enum _QM_READ_WRITE_REGS {
	QM9857nr_READDATA		= 0x201,
	QM9857nr_WRITEDATA		= 0x202,
} QM9857_READ_WRITE_REGS;

// Qm  regs
typedef enum _QM9857_REGS {
	QM9857nr_MODE0		= 0,
	QM9857nr_CONTROL1	= 0x17,
} QM9857_REGS;

#pragma pack(pop)

#endif //__QM9857REGS_H_

//
// End of file
//