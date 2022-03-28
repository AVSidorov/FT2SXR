/*
 ****************** File Fm214x250mRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for FM214x250M
 *
 * (C) InSys by Ekkore Nov 2011
 *
 ************************************************************
*/

//
#ifndef _FM214x250MREGS_H_
#define _FM214x250MREGS_H_

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

//typedef union  
//{
//	U32	AsWhole;		// Register as a Whole Word
//	struct 				// Register as Bit Pattern
//	{	U32	clk0_m2c_en : 1,	// 
//			clk1_m2c_en	: 1,	// 
//			Res			: 2;	// not use
//		U32	StartSrc	: 3,	// источник старта: 0 - канал 0, 2 - внешний, 3 - программный
//			Res1     	: 1,	// not use
//			bslip_set	: 1,	// установка сигнала BITSLIP
//			dcl_dis		: 1,	// запрет атоматич. калибровки
//			Res2		: 6;	// not use
//	} ByBits;
//} ADC_CTRL, *PADC_CTRL;
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

#endif //_FM214x250MREGS_H_

//
// End of file
//