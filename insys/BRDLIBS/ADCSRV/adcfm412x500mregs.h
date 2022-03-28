/*
 ****************** File Fm412x500mRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for FM412x500M
 *
 * (C) InSys by Ekkore Feb 2012
 *
 ************************************************************
*/

//
#ifndef _FM412x500MREGS_H_
#define _FM412x500MREGS_H_

//
// Номера специфических косвенных регистров тетрады
//
enum 
{
	ADCnr_GAIN1		= 24, // 0x18
	ADCnr_SPD_DEVICE	= 0x203,
	ADCnr_SPD_CTRL		= 0x204,
	ADCnr_SPD_ADDR		= 0x205,
	ADCnr_SPD_DATA		= 0x206,
};

#pragma pack(push,1)

//
// Внутренние структуры битовых полей специфических косвенных регистров
//

//
// ADC Control register (+23)
//
typedef union  
{
	U32	AsWhole;		// Register as a Whole Word
	struct 				// Register as Bit Pattern
	{	U32	Res			: 4;	// not use
		U32	StartSrc	: 3,	// источник старта: 2 - внешний, 3 - программный
			Res1     	: 1,	// not use
			RefSel		: 2,	// на вход REFIN AD9818 подан сигнал от: 0-Si570/571, 1-внеш. генератора
			stResist	: 1,	// вх. сопротивление внешенго старта: 0 - 2.5кОм, 1 - 50 Ом
			Res3		: 5;	// not use
	} ByBits;
} ADC_CTRL, *PADC_CTRL;


#pragma pack(pop)

#endif //_FM412x500MREGS_H_

//
// End of file
//