//****************** File Dac3224x192SrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of DAC3224x192 service
//
//	Copyright (c) 2002-2005, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  12-10-05 - builded
//
//*******************************************************************************

#ifndef _DAC3224x192SRVINFO_H
 #define _DAC3224x192SRVINFO_H

//#define MAX_CHAN 32

#define SINGLE_DIVIDER 512

#define MAX_SINGLE_RATE 50000.
#define MAX_DOUBLE_RATE 100000.

#pragma pack(1)
/*
// Struct info about device
typedef struct _DAC3224x192SRV_INFO {
	USHORT		Size;					// sizeof(DAC3224x192SRV_INFO)
	PDACSRV_INFO pDacInfo;
	//UCHAR		DblClockMode;			// режим удвоения тактовой частоты
	//ULONG		RefGen;					// frequency of generators (значения опорных генераторов (Гц))
	//double		ExtClk;					// any external frequency of clock (любая из внешних частот тактирования (Гц))
	//double		PhaseTuning[MAX_CHAN];	// подстройка фазы тактового сигнала канала 0
	//double		GainTuning0;			// подстройка усиления канала 0
	//UCHAR		Inp0Ch1;				// 1 - к каналу 1 подключен вход 0
	//ULONG		StartSrc;				// источник старта
	//UCHAR		InvStart;				// инверсия сигнала старта
	//double		StartLevel;				// уровень сигнала старта
} DAC3224x192SRV_INFO, *PDAC3224x192SRV_INFO;
*/
#pragma pack()

#endif // _ADC212X200MSRVINFO_H

// ****************** End of file Dac3224x192SrvInfo.h ***************
