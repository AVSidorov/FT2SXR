/*
 ****************** File Adc214x400mRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADM214x400M(WB) and ADM212x500M(WB)
 *
 * (C) InSys by Ekkore Aug 2008
 *
 ************************************************************
*/

#ifndef _ADC214X400MREGS_H_
 #define _ADC214X400MREGS_H_

#pragma pack(push,1)

// ADC Frequency Divider register (+20)
typedef union _ADC_FDVR {
	U32	AsWhole;		// Register as a Whole Word
	struct {			// Register as Bit Pattern
		U32	Div	: 4,	// 0x0 - 1, 0x1 - 2, 0x2 - 4, 0x3 - 8, 0x4 - 16, 0x5 - 32, 0x6 - 64, 0x7 - 128, 0x8 - 256
			Res	: 12;	// not use
	} ByBits;	
} ADC_FDVR, *PADC_FDVR;

// ADC Control register (+23)
typedef union _ADC_CTRL {
	U32	AsWhole;		// Register as a Whole Word
	struct {			// Register as Bit Pattern
		U32	Res0		: 2,	// not use
			Clbr		: 1,	// 1 - calibration by zero
			Res			: 1;	// not use
		U32	StartSrc	: 3,	// select source of start: 0 - channel 0, 1 - channel 1, 2 - external input X5, 3 - program
			StartInv	: 1,	// 1 - inverting start
			StartClkSl	: 1,	// 1 - start & clock Slave mode
			Res1		: 7;	// not use
	} ByBits;
} ADC_CTRL, *PADC_CTRL;

// Gain register (+21)
typedef union _ADC_GAINWBLF {
	U32	AsWhole;		// Register as a Whole Word
	struct {			// Register as Bit Pattern
		U32 Chan0	: 8,	// gain of channel 0 (0 - largest range voltage)
			Chan1	: 8;	// gain of channel 1 (0 - largest range voltage)
	} ByBits;
} ADC_GAINWBLF, *PAC_GAINWBLF;

#pragma pack(pop)

#endif //_ADC214X400MREGS_H_

//
// End of file
//