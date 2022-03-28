/*
 ****************** File Adc1624x192Regs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADC16214X192
 *
 * (C) InSys by Dorokhin Andrey Jun, 2007
 *
 ************************************************************
*/

#ifndef _ADC16214X192REGS_H_
 #define _ADC16214X192REGS_H_

#pragma pack(push,1)

// Gain register (+21)
typedef union _ADC_GAIN {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Chan01		: 2,	// gain of channel 0 & 1 (0 - largest range voltage)
		 Chan23		: 2,	// gain of channel 2 & 3 (0 - largest range voltage)
		 Chan45		: 2,	// gain of channel 4 & 5 (0 - largest range voltage)
		 Chan67		: 2,	// gain of channel 6 & 7 (0 - largest range voltage)
		 Chan89		: 2,	// gain of channel 8 & 9 (0 - largest range voltage)
		 Chan1011	: 2,	// gain of channel 10 & 11 (0 - largest range voltage)
		 Chan1213	: 2,	// gain of channel 12 & 13 (0 - largest range voltage)
		 Chan1415	: 2;	// gain of channel 14 & 15 (0 - largest range voltage)
  } ByBits;
} ADC_GAIN, *PADC_GAIN;

// ADC Control register (+23)
typedef union _ADC_CTRL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Lrck		: 2,	// LRCK mode: 0 - SINGLE (CLK/512), 1 - DOUBLE (CLK/256), 2 - QUAD (CLK/128)
		 Test		: 2,	// test select: 0 - GND - GND, 1 - DAC0+ - DAC0-, 2 - DAC0+ - GND, 3 - 2.5V - GND
		 Sync		: 1,	// 0->1 - synchro pulse
		 Res		: 1,	// not use
		 InpSrc		: 2,	// input source select: 0 - GND, 1 - input, 2 - test, 3 - GND
		 Res1		: 2,	// not use
		 HPF		: 1,	// high-pass filter: 1 - off
		 Res2		: 1,	// not use
		 AdcRst		: 1,	// 0 - ADC reset
		 Res3		: 3;	// not use
  } ByBits;
} ADC_CTRL, *PADC_CTRL;

// ADC Bias DAC control register (+24)
typedef union _ADC_BIASDACCTRL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Num	: 4,	// number of channel (0-15)
	     Mode	: 2,	// 0 - write value, 1 - increment value, 2 - decrement value
		 Res	: 10;	// not use
  } ByBits;
} ADC_BIASDACCTRL, *PADC_BIASDACCTRL;

// ADC Bias DAC data register (+25)
typedef union _ADC_BIASDACDATA {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Data	: 12,	// number of channel (0-15)
		 Res	: 4;	// not use
  } ByBits;
} ADC_BIASDACDATA, *PADC_BIASDACDATA;

// Numbers of Command Registers
typedef enum _ADC1624X192_NUM_CMD_REGS {
	ADC1624X192nr_BIASDACCTRL	= 24, // 0x18
	ADC1624X192nr_BIASDACDATA	= 25, // 0x19
} ADC1624X192_NUM_CMD_REGS;

#pragma pack(pop)

#endif //_ADC16214X192REGS_H_

//
// End of file
//