/*
 ****************** File 8gcRegs.h *******************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for 8GC
 *
 * (C) InSys by Sklyarov A. Apr. 2012
 *
 *************************************************************
*/

#ifndef _8GCREGS_H_
 #define _8GCREGS_H_

#pragma pack(push,1)



// DDCSYNC Register
typedef union _GC_DDCSYNC {
	ULONG AsWhole; //  Register as a Whole Word
	struct { // GC_DDCSYNC Register as Bit Pattern
		ULONG	SyncMode: 2,   
				ProgSync: 1,
				Slave:	  1,
				ResDiv:	  1;
	} ByBits;
} GC_DDCSYNC, *PGC_DDCSYNC;
// DDCSYNC Register
typedef union _GC_AGCGAIN {
	ULONG AsWhole; //  Register as a Whole Word
	struct { // GC_ADCGAIN Register as Bit Pattern
		ULONG	Gain0: 1,   
				Gain1: 1,
				Gain2: 1,
				Gain3: 1;
	} ByBits;
} GC_ADCGAIN, *PGC_ADCGAIN;

#define	Reg_ADC_MODES	 0x8
#define	Reg_ADC_OFFSET	 0x10
#define	Reg_ADCGAIN		 0x15 
#define	Reg_ADC_VREF	 0x18
#define	Reg_ADC_BUFCURRENT1  0x36
#define	Reg_ADC_DEV_UPDATE   0xFF
#define	Reg_ADC_BUFCURRENT2  0x107

#define  CONST_SUBTRD_ID	  0x280
#define  CONST_SUBTRD_MOD	  0x281
#define  CONST_SUBTRD_VER	  0x282
#define  CONST_SUBADM_ID	  0x283
#define  CONST_SUBPLD_ID	  0x284
#define  CONST_SUBPLD_MODE	  0x285
#define  CONST_SUBPLD_VER	  0x286
#define  CONST_SUBPLD_BUILD   0x287


// Numbers of Command Registers
typedef enum _GC_NUM_CMD_REGS {
	GCnr_MODE0			= 0,
	GCnr_MODE2			= 0xA,
	GCnr_TESTSEQUENCE	= 0xC,

	GCnr_ADC_DELAY		= 0x18,//22,

	GCnr_DDCSYNC		= 0x19,//22,
	GCnr_ADMMODE		= 0x1A, //28,
	GCnr_DDCMODE		= 0x1B, //30,
	GCnr_MASK_DEVICE	= 0x1D, 
	GCnr_ADR_HIGH		= 0x1E, 
	GCnr_DELAY_CTRL		= 0x1F, 
} GC_READ_WRITE_REGS;

// Numbers of Direct Registers
typedef enum _GC_NUM_DIRECT_REGS {
	GCnr_ADMCONST	= 0x201,	// 
	GCnr_ADCOVER	= 0x203,
	GCnr_DDC_ADR_SHIFT	 = 0x300,
	GCnr_DDC_ADR_SHIFT_R = 0x380,
} GC_REGS;

// ADM const register (+201)
typedef union _DDC_CONST {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Build		: 4,	// build number of project
	     VerLow		: 4,	// version number (low part)
	     VerHigh	: 4,	// version number (high part)
		 Mod		: 4;	// modification
  } ByBits;
} DDC_CONST, *PDDC_CONST;



#pragma pack(pop)

#endif //__8GCREGS_H_

//
// End of file
//