/*
 ****************** File Adc214x400mRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADM1612x1M
 *
 * (C) InSys by Ekkore Nov 2008
 *
 ************************************************************
*/

#ifndef _ADC1612x1MREGS_H_
 #define _ADC1612x1MREGS_H_

#pragma pack(push,1)

//
// Chan1 register (+16 = ADM2IFnr_CHAN1)
//
typedef union _ADC_CHAN1 
{
	U32 AsWhole;	// Register as a Whole Word
	struct 
	{			// Register as Bit Pattern
		U32 ChanSel	: 16;	// Channel select
	} ByBits;
} ADC_CHAN1, *PADC_CHAN1;

//
// FORMAT register (+18 = ADM2IFnr_FORMAT)
//
typedef union _ADC_FORMAT 
{
	U32 AsWhole;	// Register as a Whole Word
	struct 
	{			// Register as Bit Pattern
		U32 Pack	: 2;	// 00 - 16-bit, 01 - 8-bit, 10 - 32-bit float, 11 - 32 bit fixed
	} ByBits;
} ADC_FORMAT, *PADC_FORMAT;

//
// Gain register (+21 = ADM2IFnr_GAIN)
//
typedef union _ADC_GAIN 
{
	U32 AsWhole;	// Register as a Whole Word
	struct 
	{			// Register as Bit Pattern
		U32 Chan0_1	: 2,	// gain of chan 0 and 1 (0 - 1, 01 - 2, 10 - 4, 11 - 8)
			Chan2_3	: 2,	// gain of chan 2 and 3	
			Chan4_5	: 2,	// gain of chan 4 and 5
			Chan6_7	: 2,	// gain of chan 6 and 7
			Chan8_9	: 2,	// gain of chan 8 and 9	
			Chan10_11	: 2,	// gain of chan 10 and 11	
			Chan12_13	: 2,	// gain of chan 12 and 13	
			Chan14_15	: 2;	// gain of chan 14 and 15	
	} ByBits;
} ADC_GAIN, *PADC_GAIN;


#pragma pack(pop)

#endif //_ADC1612x1MREGS_H_

//
// End of file
//