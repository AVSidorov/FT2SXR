//****************** File Adc214x200mSrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADC214x200M service
//
//	Copyright (c) 2002-2007, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  11-01-07 - builded
//
//*******************************************************************************

#ifndef _ADC214X200MSRVINFO_H
 #define _ADC214X200MSRVINFO_H

#pragma pack(1)

// Struct info about device
typedef struct _ADC214X200MSRV_INFO {
	USHORT		Size;					// sizeof(ADC214X200MSRV_INFO)
	PADCSRV_INFO pAdcInfo;
	double		ExtClk;					// any external frequency of clock (любая из внешних частот тактирования (Гц))
	double		GainTuning[MAX_CHAN];	// подстройка усиления каждого канала
	ULONG		StartSrc;				// источник старта
	UCHAR		InvStart;				// инверсия сигнала старта
	double		StartLevel;				// уровень сигнала старта
	ULONG		PllRefGen;				// frequency of PLL reference generator (значение внутреннего опорного генератора для ФАПЧ (Гц))
	ULONG		PllFreq;				// frequency of PLL (частота, выдаваемая ФАПЧ (Гц))
	ULONG		ExtPllRef;				// frequency of external PLL reference (значение внешней опорной частоты для ФАПЧ (Гц))
	UCHAR		InvClk;					// инверсия сигнала тактирования
	UCHAR		Version;				// версия субмодуля
	UCHAR		IsRF;					// 1 - радиочастотный канал
	double		dBGain[MAX_CHAN];		// усиления каждого канала (дБ) - если IsRF = 1
} ADC214X200MSRV_INFO, *PADC214X200MSRV_INFO;

#pragma pack()

#endif // _ADC214X200MSRVINFO_H

// ****************** End of file Adc214x200mSrvInfo.h ***************
