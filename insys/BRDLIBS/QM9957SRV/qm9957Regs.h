/*
 ****************** File qm9957Regs.h *******************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for QM9957
 *
 * (C) InSys by Sklyarov A. Nov. 2010
 *
 *************************************************************
*/

#ifndef _QM9957REGS_H_
 #define _QM9957REGS_H_

#pragma pack(push,1)

// QMC Mode0 register (+0)
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
	ULONG	chipMask	: 2, // mask for programming QM chips
			Res			: 14; // Reserved
	} ByBits;
} QM_MODE3, *PQM_MODE3;

// QM Control1 register (+23)
typedef union _QM_CONTROL1 {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG SelProf		: 3,	// select profile
		 MasterReset	: 2,	// 1 - reset DAC chips
		 IO_Reset		: 2,	// 
		 IO_Updata		: 1,	// 1- update I/O AD9957
		 SyncMode    	: 2,	
		 ipdclk	    	: 1,	
		 Master_Slave  	: 1,	// Signal M/S
		 SyncEnable    	: 1,	// Signal SYNC_EN
		 SyncSelect    	: 1,	// Signal SYNC_SEL
		 FD_ST			: 1,	// Signal FD_ST
		 FD_CLK			: 1;	// Signal FD_CLK
  } ByBits;
} QM_CONTROL1, *PQM_CONTROL1;

// QM read/write regs
typedef enum _QM_READ_WRITE_REGS {
	QM9957nr_ADDRREG		= 0x201,
	QM9957nr_DATAREG0		= 0x202,
	QM9957nr_DATAREG1		= 0x203,
	QM9957nr_DATAREG2		= 0x204,
	QM9957nr_DATAREG3		= 0x205,
} QM9957_READ_WRITE_REGS;

// QM  regs
typedef enum _QM9957_REGS {
	QM9957nr_MODE0		= 0,
	QM9957nr_CONTROL1	= 0x17,
} QM9957_REGS;

#pragma pack(pop)

#endif //__QM9957REGS_H_

//
// End of file
//