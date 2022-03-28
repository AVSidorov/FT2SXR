/*
 ****************** File mrfddcRegs.h *******************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for MRFDDC
 *
 * (C) InSys by Sklyarov A. Apr. 2012
 *
 *************************************************************
*/

#ifndef _DDC416x130MRFREGS_H_
 #define _DDC416x130MRFREGS_H_

#pragma pack(push,1)

//-----------------------------------------------------------------------
typedef union _STMODEM {
	ULONG AsWhole; // Start Mode Register as a Whole Word
	struct { // Start Mode Register as Bit Pattern
		ULONG	SrcStart	: 1, // 0-program start,1-external start
				Res1		: 3, // Reserved
				EnbFrame	: 1, // 1 - enable frame mode
				Res2		: 1, // Reserved
				InvStart	: 1, // 1 - inverse external start
				Res3		: 9; // Reserved
	} ByBits;
} REG_STMODEM, *PREG_STMODEM;
//----------------------------------------------------------------
typedef union _CONTROL1 {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Dith		: 1,	// bit DITH ADC 
	     Res1		: 3,	// Reserve
		 Source		: 2,	// Source of Data: 0-ADC, 1-DDC
		 Res2		: 2,	// Reserve
		 EnbHeader	: 1,	// 1-Enable Of Header
		 EnbImit	: 1,	// 1-Enable OfImitator
		 StartTrig	: 1,	// 1-Triger Start
		 ExtStartDdc: 1,	// 0-Program Start DDC, 1- External Start DDC
		 StartDdc	: 1,	// 1-Start DDC
		 DdcSync	: 2,	// 1-Sync Mode Ddc
		 EnbTestCount: 1;	// 1- Enable Test Count
  } ByBits;
} REG_CONTROL1, *PREG_CONTROL1;

#define  CONST_SUBTRD_ID	  0x280
#define  CONST_SUBTRD_MOD	  0x281
#define  CONST_SUBTRD_VER	  0x282
#define  CONST_SUBADM_ID	  0x283
#define  CONST_SUBPLD_ID	  0x284
#define  CONST_SUBPLD_MODE	  0x285
#define  CONST_SUBPLD_VER	  0x286
#define  CONST_SUBPLD_BUILD   0x287

//-------------------------------------------------------------------------
// Numbers of Command Registers
typedef enum _MRFDDC_NUM_CMD_REGS {
	MRFDDC_GAIN		= 0x15, // Выбор коэффициента усиления
	MRFDDC_INP			= 0x16,	// Регистр управления входным мультиплексором 
	MRFDDC_CONTROL1	= 0x17, // Регистр управления субмодулем
	MRFDDC_NCO			= 0x19, // Регистр для записи кода частоты NCO
	MRFDDC_WRNCO		= 0x1A, // Запись кода частота в DDC
	MRFDDC_COE			= 0x1B, // Регистр для записи коэффициентов фильтра DDC
	MRFDDC_DECIM		= 0x1E, // Регистр коэффициента децимации DDC
	MRFDDC_DELAY_CTRL	= 0x1F,  // !!!!
} MRFDDC_READ_WRITE_REGS;

#define DDCMRF_ADMCONST	 0x201
typedef union _DDC_CONST {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Build		: 4,	// build number of project
	     VerLow		: 4,	// version number (low part)
	     VerHigh	: 4,	// version number (high part)
		 Mod		: 4;	// modification
  } ByBits;
} DDC_CONST, *PDDC_CONST;

typedef union _CONTFLT {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG EnableFlt	: 1,	// 0 - disable flt, 1 - enable flt 
	     RstFlt		: 1,	// Reset of write flt: 1 - reset
		 Res1		: 3,	// Reserve
		 SizeFlt	: 10,	// Size of Flt
		 Res2		: 1;	// Reserve
  } ByBits;
} CONTFLT, *PCONTFLT;


#pragma pack(pop)

#endif //__GC5016REGS_H_

//
// End of file
//