/*
 ****************** File ddc416x130mrfRegs.h *******************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for DDC416x130MRF
 *
 * (C) InSys by Sklyarov A. Dec. 2011
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
//-------------------------------------------------------------------------
// Numbers of Command Registers
typedef enum _DDCMRF_NUM_CMD_REGS {
	DDCMRF_MODE0		= 0x0,  // Регистр управления
	DDCMRF_STMODE		= 0x5,  // Регистр синхронизации
	DDCMRF_CNT0			= 0x6,  // Начальная задержка приёма в кадре
	DDCMRF_CNT1			= 0x7,  // Число принимаемых отсчетов в строке
	DDCMRF_CNT2			= 0x8,  // Число пропускаемых отсчетов в строке
	DDCMRF_MODE2		= 0xA,  // !!!!
	DDCMRF_CHAN1		= 0x10, // Выбор каналов 
	DDCMRF_FSRC			= 0x13, // Выбор источника тактовой частоты
	DDCMRF_SYNCDELAY	= 0x14,	// Задержка сигнала синхронизации DDC 
	DDCMRF_GAIN			= 0x15, // Выбор коэффициента усиления
	DDCMRF_HEAD1		= 0x16,	// Регистр идентификатора в заголовке 
	DDCMRF_CONTROL1		= 0x17, // Регистр управления субмодулем
	DDCMRF_CNT3			= 0x18, // Число строк в кадре
	DDCMRF_NCO			= 0x19, // Регистр для записи кода частоты NCO
	DDCMRF_MASK			= 0x1A, // Маска DDC при программировании NCO и фильтра
	DDCMRF_COE			= 0x1B, // Регистр для записи коэффициентов фильтра DDC
	DDCMRF_DECIM		= 0x1E, // Коэффициент децимации DDC
	DDCMRF_DELAY		= 0x1F,  // !!!!
} DDCMRF_READ_WRITE_REGS;

#define DDCMRF_ADMCONST	 0x201
/*
// Numbers of Direct Registers
typedef enum _GC_NUM_DIRECT_REGS {
	GC5016nr_ADMCONST	= 0x201,	// 
	GC5016nr_READDATA	= 0x202,
	GC5016nr_ADCOVER	= 0x203,
	GC5016nr_TLDATAL	= 0x20c,
	GC5016nr_TLDATAH	= 0x20d,
	GC5016nr_RDATAFLT	= 0x204,
	GC5016nr_IDATAFLT	= 0x205,
	GC5016nr_CONTFLT	= 0x206,
	GC5016nr_RWINDFFT	= 0x207,
	GC5016nr_IWINDFFT	= 0x208,
	GC5016nr_CONTFFT	= 0x209,
	GC5016nr_SKIPSAMPLES = 0x20e,
	GC5016nr_GETSAMPLES	= 0x20f,
	GC5016nr_HEADERENABLE= 0x210,
	GC5016nr_HEADERFRAME= 0x211,
	GC5016nr_DELAYSTART= 0x212,
	GC5016nr_NUMCHANS= 0x213,
} GC5016_REGS;
*/
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