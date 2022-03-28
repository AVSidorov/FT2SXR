//****************** File Adc10x2gSrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADC10x2G service
//
//	Copyright (c) 2002-2005, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  16-06-05 - builded
//  15-03-06 - added by ver. 2.0
//
//*******************************************************************************

#ifndef _ADC10X2GSRVINFO_H
 #define _ADC10X2GSRVINFO_H

//#define MAX_CHAN 1

#pragma pack(1)

// Struct info about device
typedef struct _ADC10X2GSRV_INFO {
	USHORT		Size;					// sizeof(ADC10X2GSRV_INFO)
	PADCSRV_INFO pAdcInfo;
	UCHAR		TestMode;				// режимы тестирования
	double		ExtClk;					// any external frequency of clock (любая из внешних частот тактирования (Гц))
	double		PhaseTuning;			// подстройка фазы тактового сигнала
	double		GainTuning;				// подстройка усиления каждого канала 
	ULONG		StartSrc;				// источник старта
	UCHAR		InvStart;				// инверсия сигнала старта
	double		StartLevel;				// уровень сигнала старта
	UCHAR		Version;				// версия модуля
	ULONG		PllRefGen;				// frequency of PLL reference generator (значение внутреннего опорного генератора для ФАПЧ (Гц))
	ULONG		PllFreq;				// frequency of PLL (частота, выдаваемая ФАПЧ (Гц))
	ULONG		ExtPllRef;				// frequency of external PLL reference (значение внешней опорной частоты для ФАПЧ (Гц))
	UCHAR		StartSl;				// 1 - старт от разъема START IN
	UCHAR		ClkSl;					// 1 - старт от разъема CLK IN
	UCHAR		TimerSync;				// 1 - режим синхронизации таймера от сигнала старта
} ADC10X2GSRV_INFO, *PADC10X2GSRV_INFO;

#pragma pack()

#endif // _ADC10X2GSRVINFO_H

// ****************** End of file Adc10x2gSrvInfo.h ***************
