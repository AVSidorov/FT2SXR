//****************** File Adc212x200mSrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADC212x200M service
//
//	Copyright (c) 2002-2003, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  14-04-04 - builded
//
//*******************************************************************************

#ifndef _ADC212X200MSRVINFO_H
 #define _ADC212X200MSRVINFO_H

//#define MAX_ADC_CHAN 2

#pragma pack(1)

// Struct info about device
typedef struct _ADC212X200MSRV_INFO {
	USHORT		Size;					// sizeof(ADC212X200MSRV_INFO)
	PADCSRV_INFO pAdcInfo;
	UCHAR		DblClockMode;			// режим удвоения тактовой частоты
	ULONG		RefGen;					// frequency of generators (значения опорных генераторов (Гц))
	double		ExtClk;					// any external frequency of clock (любая из внешних частот тактирования (Гц))
	double		PhaseTuning[MAX_CHAN];	// подстройка фазы тактового сигнала канала 0
	double		GainTuning0;			// подстройка усиления канала 0
	UCHAR		Inp0Ch1;				// 1 - к каналу 1 подключен вход 0
	ULONG		StartSrc;				// источник старта
	UCHAR		InvStart;				// инверсия сигнала старта
	double		StartLevel;				// уровень сигнала старта
} ADC212X200MSRV_INFO, *PADC212X200MSRV_INFO;

#pragma pack()

#endif // _ADC212X200MSRVINFO_H

// ****************** End of file Adc212x200mSrvInfo.h ***************
