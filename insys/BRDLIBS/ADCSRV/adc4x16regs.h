/*
 ****************** File Adc4x16Regs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADC4x16
 *
 * (C) InSys by Dorokhin Andrey Jan, 2006
 *
 ************************************************************
*/

#ifndef _ADC4x16REGS_H_
 #define _ADC4x16REGS_H_

#pragma pack(push,1)
// ADC Input register (+22)
/*typedef union _ADC_INP {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG InpR0		: 1,	// input resistance of channel 0 (0 - 1 MOm, 1 - 50 Om)
		 InpType0	: 1,	// input type of channel 0 (1 - DC coupling, open input)
		 Res		: 2;	// not use
   ULONG InpR1		: 1,	// input resistance of channel 1 (0 - 1 MOm, 1 - 50 Om)
		 InpType1	: 1,	// input type of channel 1 (1 - DC coupling, open input)
		 Res1		: 2;	// not use
   ULONG InpR2		: 1,	// input resistance of channel 2 (0 - 1 MOm, 1 - 50 Om)
		 InpType2	: 1,	// input type of channel 2 (1 - DC coupling, open input)
		 Res2		: 2;	// not use
   ULONG InpR3		: 1,	// input resistance of channel 3 (0 - 1 MOm, 1 - 50 Om)
		 InpType3	: 1,	// input type of channel 3 (1 - DC coupling, open input)
		 Res3		: 2;	// not use
  } ByBits;
} ADC_INP, *PADC_INP;
*/
// ADC Frequency Divider register (+20)
typedef union _ADC_FDVR {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
	ULONG Div		: 3,	// main divider: 0x0 - 1, 0x1 - 2, 0x2 - 4, 0x3 - 8, 0x4 - 16
		  AuxDiv	: 1,	// Auxiliary divider: 0x0 - 1, 0x1 - 2
		  Res		: 12;	// not use
  } ByBits;
} ADC_FDVR, *PADC_FDVR;

// ADC Control register (+23)
typedef union _ADC_CTRL {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG Res		: 4;	// not use
   ULONG StartSrc	: 3,	// select source of start: 0 - channel 0, 1 - channel 1, 2 - external input START IN, 3 - program
		 StartInv	: 1,	// 1 - inverting start
		 StartClkSl	: 1,	// 1 - start & clock Slave mode
		 Res1		: 7;	// not use
  } ByBits;
} ADC_CTRL, *PADC_CTRL;
/*
// ADC Status Register
typedef union _ADC_STATUS {
	ULONG AsWhole; // Board Status Register as a Whole Word
	struct { // Status Register as Bit Pattern
		ULONG	CmdRdy		: 1, // Ready to do command
				FifoRdy		: 1, // Ready FIFO 
				Empty		: 1, // Empty FIFO
				AlmostEmpty	: 1, // Almost Empty FIFO
				HalfFull	: 1, // Half Full FIFO
				AlmostFull	: 1, // Almost Full FIFO
				Full		: 1, // Full FIFO
				Overflow	: 1, // Overflow FIFO
				Underflow	: 1, // Underflow FIFO
				ErrorAccess	: 1, // register access error
				Start		: 1, // work enabled
				Reserved0	: 3, // Reserved
				OverAdc0	: 1, // Bits Overflow of ADC0
				OverAdc1	: 1; // Bits Overflow of ADC1
//				Reserved0	: 4, // Reserved
//				OverAdc		: 1; // Bits Overflow of one or more ADC
	} ByBits;
} ADC_STATUS, *PADC_STATUS;

// ADC FLAG_CLR register
typedef union _ADC_FLAGCLR {
  ULONG AsWhole; // DIO32_IN FLAG_CLR Register as a Whole Word
  struct { // DIO32_IN FLAG_CLR Register as Bit Pattern
   ULONG Reserved0	: 18, // Reserved
		 ClrOvrAdc0	: 1,  // 1 - clear flag overflow ADC0
		 ClrOvrAdc1	: 1;  // 1 - clear flag overflow ADC1
  } ByBits;
} ADC_FLAGCLR, *PADC_FLAGCLR;

// Numbers of Command Registers
typedef enum _ADC_NUM_CMD_REGS {
	ADCnr_INP		= 22,
	ADCnr_CTRL1		= 23,
} ADC_NUM_CMD_REGS;

// Numbers of Constant Registers
typedef enum _ADC_NUM_DIRECT_REGS {
	ADCnr_FLAGCLR		= 0x200,
	ADCnr_PREVPREC		= 0x209,	// start event by pretrigger mode: sample number into word
	ADCnr_PRTEVENTLO	= 0x20A,	// start event by pretrigger mode: word number into buffer (low part)
	ADCnr_PRTEVENTHI	= 0x20B,	// start event by pretrigger mode: word number into buffer (high part)
} ADC_NUM_DIRECT_REGS;
*/
#pragma pack(pop)

#endif //_ADC28X800MREGS_H_

//
// End of file
//