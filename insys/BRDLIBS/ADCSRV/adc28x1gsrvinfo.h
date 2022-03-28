//****************** File Adc28x1gSrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADC28x1G service
//
//	Copyright (c) 2002-2007, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  11-01-07 - builded
//
//*******************************************************************************

#ifndef _ADC28X1GSRVINFO_H
 #define _ADC28X1GSRVINFO_H

#pragma pack(1)

// Struct info about device
typedef struct _ADC28X1GSRV_INFO {
	USHORT		Size;					// sizeof(ADC28X1GSRV_INFO)
	PADCSRV_INFO pAdcInfo;
	UCHAR		DblClockMode;			// режим удвоения тактовой частоты
	double		ExtClk;					// any external frequency of clock (любая из внешних частот тактирования (Гц))
	UCHAR		Inp0Ch1;				// 1 - к каналу 1 подключен вход 0
	ULONG		StartSrc;				// источник старта
	UCHAR		InvStart;				// инверсия сигнала старта
	double		StartLevel;				// уровень сигнала старта
	ULONG		PllRefGen;				// frequency of PLL reference generator (значение внутреннего опорного генератора для ФАПЧ (Гц))
	ULONG		PllFreq;				// frequency of PLL (частота, выдаваемая ФАПЧ (Гц))
	ULONG		ExtPllRef;				// frequency of external PLL reference (значение внешней опорной частоты для ФАПЧ (Гц))
	double		GainTuning[MAX_CHAN];	// подстройка коэффициента передачи в каждом канале
//	UCHAR		AdcReg[8];				// internal ADC registers 0-7
	UCHAR		GainComp;				// Gain Compensation
	UCHAR		Fisda;					// Fine Sampling Delay Adjustment (FiSDA)
	UCHAR		UseClbr;				// Use last calibration value
} ADC28X1GSRV_INFO, *PADC28X1GSRV_INFO;

#pragma pack()

#endif // _ADC212X200MSRVINFO_H

// ****************** End of file Adc28x1gSrvInfo.h ***************
