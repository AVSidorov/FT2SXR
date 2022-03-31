/*
 ****************** File duc9957Regs.h *******************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for DUC9957
 *
 * (C) InSys by Sklyarov A. Dec. 2009
 *
 *************************************************************
*/

#ifndef _DUC9957REGS_H_
 #define _DUC9957REGS_H_

#pragma pack(push,1)

// DUC Control1 register (+0)
typedef union _DUC_MODE0 {
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
} DUC_MODE0, *PDUC_MODE0;

// DUC Mode3 Register (+11)
typedef union _DUC_MODE3 {
	ULONG AsWhole; // Mode3 Register as a Whole Word
	struct { // Mode1 Register as Bit Pattern
	ULONG	Res0		: 2, // Reserved
    		ProgMask	: 2,  // mask for programming QM chips
			Res1		: 12; // Reserved
	} ByBits;
} DUC_MODE3, *PDUC_MODE3;

// DUC Control1 register (+23)
typedef union _DUC_CONTROL1 {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG SelProf		: 3,	// select profile
		 MasterReset	: 2,	// 1 - reset DAC chips
		 IO_Reset		: 2,	// 
		 IO_Updata		: 2,	// update 
		 SyncMode    	: 3,	
		 Res2			: 4;	// not use
  } ByBits;
} DUC_CONTROL1, *PDUC_CONTROL1;
/*
// DUC Chip Data Register
typedef union _DUC_CHIPDATA {
	ULONG AsWhole; //  Register as a Whole Word
	struct { // Mode1 Register as Bit Pattern
		ULONG	Data	: 8,  // data for writing DAC chips
				Addr	: 8;  // number of register
	} ByBits;
} DUC_CHIPDATA, *PDUC_CHIPDATA;
*/

// DUC read/write regs
typedef enum _DUC_READ_WRITE_REGS {
	DUC9957nr_ADDRREG		= 0x201,
	DUC9957nr_DATAREG0		= 0x202,
	DUC9957nr_DATAREG1		= 0x203,
	DUC9957nr_DATAREG2		= 0x204,
	DUC9957nr_DATAREG3		= 0x205,
} DUC9957_READ_WRITE_REGS;

// DUC  regs
typedef enum _DUC9957_REGS {
	DUC9957nr_MODE0		= 0,
	DUC9957nr_CONTROL1	= 0x17,
} DUC9957_REGS;

#pragma pack(pop)

#endif //__DUC9957REGS_H_

//
// End of file
//