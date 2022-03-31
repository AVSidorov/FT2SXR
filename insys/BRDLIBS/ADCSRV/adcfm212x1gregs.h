/*
 ****************** File Fm212x1gRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for FM212x1G
 *
 * (C) InSys by Ekkore Nov 2011
 *
 ************************************************************
*/

//
#ifndef _FM212x1GREGS_H_
#define _FM212x1GREGS_H_

//
// Номера специфических косвенных регистров тетрады
//
enum 
{
	ADCnr_SPD_DEVICE	= 0x203,
	ADCnr_SPD_CTRL		= 0x204,
	ADCnr_SPD_ADDR		= 0x205,
	ADCnr_SPD_DATA		= 0x206,
	ADCnr_SPD_DATAH		= 0x207,
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
	{	U32	Res			: 4,	// not use
			StartSrc	: 3,	// источник старта: 0-кан.0, 1-кан.1, 2-внешний, 3-программный
			StartInv	: 1,	// полярность внеш. старта: 0-подъем, 1-спад
			StartSl		: 1,	// межмод. синхр-ция старта: 1-вкл. 0-как указано в StartSrc
			Res1     	: 1,	// not use
			stResist	: 1,	// вх. сопротивление внешенго старта: 0 - 2.5кОм, 1 - 50 Ом
			AccEn     	: 1,	// режим накопления блоков данных: 0-выкл., 1-включен
			RefSel		: 2,	// на вход REFIN ADF4350 подан сигнал от: 1-Si570/571, 2-внеш. генератора
			Res3		: 2;	// not use
	} ByBits;
} ADC_CTRL, *PADC_CTRL;


#pragma pack(pop)

#endif //_FM212x1GREGS_H_

//
// End of file
//