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

#ifndef _GRADCREGS_H_
 #define _GRADCREGS_H_

#pragma pack(push,1)

// GRADC Mode1 register (+10)
typedef union _GRADC_MODE2 {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG CDIV		: 1,	// CDIV
		 SCR		: 1,	// SCR
		 SYNCA		: 1,	// SYNCA
		 ProgMux	: 1,	// ProgMux
		 ProgAdc	: 1,	// ProgMux
		 Reseta		: 1,	// ProgMux
		 Res		: 11;	// not use
  } ByBits;
} GRADC_MODE2, *PGRADC_MODE2;



// GRADC  regs
typedef enum _GRADC_REGS {
	INP_MUX		= 0x201,
	GAIN_0KP	= 0x202,
	GAIN_1KP	= 0x203,
	ADC_ADDR	= 0x204,
	ADC_DATA	= 0x205,
	DDC_MODE	= 0x206,
	MASK_OBZ	= 0x207,
	DDC_ADDR	= 0x208,
	NCO_LOW		= 0x209,
	NCO_HIGH	= 0x20A,
	FLT_LOW		= 0x20B,
	FLT_HIGH	= 0x20C,
	MASK_CHN	= 0x20D,
	GAIN_DDC	= 0x20E,
	OBZ_DECIM	= 0x20F,
	OBZ_GAIN	= 0x210,
} GRADC_REGS;

// GRADC DDC_MODE register 
typedef union _GRADC_DDC_MODE {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Output		: 2,	
		 DecimCic	: 9,	
		 DecimFir	: 2,	
		 Cic_bypass	: 1,	
		 Fir_bypass	: 1,	
		 res		: 1;	
  } ByBits;
} GRADC_DDC_MODE, *PGRADC_DDC_MODE;

// GRADC OBZ_DECIM register 
typedef union _GRADC_OBZ_DECIM {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG DecimCicObz	: 9,	
		 DecimFirObz	: 2,	
		 res			: 5;	
  } ByBits;
} GRADC_OBZ_DECIM, *PGRADC_OBZ_DECIM;

#pragma pack(pop)

#endif //__GRADCREGS_H_

//
// End of file
//