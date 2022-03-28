/*
 ****************** File Adc414x65mRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADC414X65M
 *
 * (C) InSys by Dorokhin Andrey Aug, 2006
 *
 ************************************************************
*/

#ifndef _ADC414X65MREGS_H_
 #define _ADC414X65MREGS_H_

#pragma pack(push,1)

// ADC Frequency Divider register (+20)
typedef union _ADC_FDVR {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Div	: 3,	// 0x0 - 1, 0x1 - 2, 0x2 - 4, 0x3 - 8, 0x4 - 16, 0x5 - 32, 0x6 - 64, 0x7 - not use
		 Res	: 13;	// not use
  } ByBits;
} ADC_FDVR, *PADC_FDVR;

#pragma pack(pop)

#endif //_ADC414X65MREGS_H_

//
// End of file
//