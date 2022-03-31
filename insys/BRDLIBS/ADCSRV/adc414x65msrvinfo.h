//****************** File Adc414x65mSrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADC414x65M service
//
//	Copyright (c) 2002-2003, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  21-08-06 - builded
//
//*******************************************************************************

#ifndef _ADC414X65MSRVINFO_H
 #define _ADC414X65MSRVINFO_H

#define MAX_ADC_CHAN 4

#pragma pack(1)

// Struct info about device
typedef struct _ADC414X65MSRV_INFO {
	USHORT		Size;					// sizeof(ADC212X200MSRV_INFO)
	PADCSRV_INFO pAdcInfo;
	ULONG		ClkLocation;			// местоположение источника тактовой частоты (1 - на субмодуле)
	double		Range[MAX_ADC_CHAN];	// входной диапазон для каждого канала
	double		Bias[MAX_ADC_CHAN];		// смещение нуля для каждого канала
	ULONG		RefGen[3];				// frequency of generators (значения опорных генераторов (Гц))
	double		ExtClk;					// any external frequency of clock (любая из внешних частот тактирования (Гц))
	double		ThrClk;					// порог тактового сигнала
	double		ThrExtClk;				// порог внешнего тактового сигнала
} ADC414X65MSRV_INFO, *PADC414X65MSRV_INFO;

#pragma pack()

#endif // _ADC414X65MSRVINFO_H

// ****************** End of file Adc414x65mSrvInfo.h ***************
