/*
 ****************** File Adc214x10mRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADM214x10M(X)
 *
 * (C) InSys by Ekkore Aug 2009
 *
 ************************************************************
*/

#ifndef _ADC214x10MREGS_H_
 #define _ADC214x10MREGS_H_

#pragma pack(push,1)

//=========================================================
// Chan1 register (+16 = ADM2IFnr_CHAN1)
//=========================================================
typedef union _ADC_CHAN1 
{
	U32 AsWhole;	// Register as a Whole Word
	struct 
	{			// Register as Bit Pattern
		U32 ChanSel	: 2;	// Channel select
	} ByBits;
} ADC_CHAN1, *PADC_CHAN1;

//=========================================================
// FORMAT register (+18 = ADM2IFnr_FORMAT)
//=========================================================
typedef union _ADC_FORMAT 
{
	U32 AsWhole;	// Register as a Whole Word
	struct 
	{			// Register as Bit Pattern
		U32 Pack	: 2;	// 00 - 16-bit, 01 - 8-bit
	} ByBits;
} ADC_FORMAT, *PADC_FORMAT;

//=========================================================
// CONTROL1 register (+23 = ADCnr_CTRL1)
//=========================================================
typedef union _ADC_CTRL
{
	U32 AsWhole;	// Register as a Whole Word
	struct 
	{			// Register as Bit Pattern
		U32 Cnt	: 4;	// Size of useful part of MUX_DATA
	} ByBits;
} ADC_CTRL, *PADC_CTRL;

//=========================================================
// MUX_DATA register (+0x208 = ADC214x10Mnr_MUXDATA0)
//=========================================================
typedef union _ADC_MUXDATA
{
	U32 AsWhole;	// Register as a Whole Word
	struct 
	{			// Register as Bit Pattern
		U32 InAdc0	: 4,	// Input nomber for ADC0
			GainAdc0: 2,	// Gain of ADC0: 0-1, 1-2, 2-4, 3-8
			Res     : 2,
			InAdc1	: 4,	// Input nomber for ADC1
			GainAdc1: 2;	// Gain of ADC1: 0-1, 1-2, 2-4, 3-8
	} ByBits;
} ADC_MUXDATA, *PADC_MUXDATA;

//=========================================================
// Numbers of Command Registers
//=========================================================
typedef enum _ADC214x10M_NUM_CMD_REGS 
{
	ADC214x10Mnr_MUXDATA0	= 0x210, 
} ADCADC214x10M_NUM_CMD_REGS;

#pragma pack(pop)

#endif //_ADC214x10MREGS_H_

//
// End of file
//