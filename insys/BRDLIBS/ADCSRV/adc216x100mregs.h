/*
 ****************** File Adc216x100mRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADC216X100M
 *
 * (C) InSys by Dorokhin Andrey Jan, 2007
 *
 ************************************************************
*/

#ifndef _ADC216X100MREGS_H_
 #define _ADC216X100MREGS_H_

#pragma pack(push,1)

// ADC Frequency Divider register (+20)
typedef union _ADC_FDVR {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Div	: 3,	// 0x0 - 1, 0x1 - 2, 0x2 - 4, 0x3 - 8, 0x4 - 16
		 Res	: 13;	// not use
  } ByBits;
} ADC_FDVR, *PADC_FDVR;

// ADC Control register (+23)
typedef union _ADC_CTRL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Res		: 8,	// not use
		 ClkSl		: 1,	// 1 - clock Slave mode
		 Dither		: 1,	// 1 - dither on
		 Res1		: 6;	// not use
  } ByBits;
} ADC_CTRL, *PADC_CTRL;
#pragma pack(pop)

#endif //_ADC216X100MREGS_H_

//
// End of file
//