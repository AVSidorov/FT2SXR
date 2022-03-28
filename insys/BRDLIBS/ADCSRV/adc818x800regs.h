/*
 ****************** File Adc818X800Regs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADC818X800
 *
 * (C) InSys by Dorokhin Andrey Jun, 2007
 *
 ************************************************************
*/

#ifndef _ADC818X800REGS_H_
 #define _ADC818X800REGS_H_

#pragma pack(push,1)

// Gain register (+21)
typedef union _ADC_GAIN {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Chan0		: 2,	// gain of channel 0 (0 - largest range voltage)
		 Chan1		: 2,	// gain of channel 1 (0 - largest range voltage)
		 Chan2		: 2,	// gain of channel 2 (0 - largest range voltage)
		 Chan3		: 2,	// gain of channel 3 (0 - largest range voltage)
		 Chan4		: 2,	// gain of channel 4 (0 - largest range voltage)
		 Chan5		: 2,	// gain of channel 5 (0 - largest range voltage)
		 Chan6		: 2,	// gain of channel 6 (0 - largest range voltage)
		 Chan7		: 2;	// gain of channel 7 (0 - largest range voltage)
  } ByBits;
} ADC_GAIN, *PADC_GAIN;

// ADC Control register (+23)
typedef union _ADC_CTRL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG ADCWARP	: 1,	// WARP mode: 0 - freq 0..500KHz, 1 - >500KHz
		 Res		: 15;	// not use
  } ByBits;
} ADC_CTRL, *PADC_CTRL;

// ADC Bias DAC control register (+24)
typedef union _ADC_BIASDACCTRL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Num	: 4,	// number of channel (0-7)
		 Res	: 12;	// not use
  } ByBits;
} ADC_BIASDACCTRL, *PADC_BIASDACCTRL;

// ADC Bias DAC data register (+25)
typedef union _ADC_BIASDACDATA {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Data	: 8,	// number of channel (0-15)
		 Res	: 8;	// not use
  } ByBits;
} ADC_BIASDACDATA, *PADC_BIASDACDATA;

// Numbers of Command Registers
typedef enum _ADC818X800_NUM_CMD_REGS {
	ADC818X800nr_BIASDACCTRL	= 24, // 0x18
	ADC818X800nr_BIASDACDATA	= 25, // 0x19
} ADC818X800_NUM_CMD_REGS;

#pragma pack(pop)

#endif //_ADC818X800REGS_H_

//
// End of file
//