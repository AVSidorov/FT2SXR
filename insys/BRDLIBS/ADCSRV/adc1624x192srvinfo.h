//****************** File Adc1624x192SrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADC1624x192 service
//
//	Copyright (c) 2002-2007, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  08-06-07 - builded
//
//*******************************************************************************

#ifndef _ADC1624x192SRVINFO_H
 #define _ADC1624x192SRVINFO_H

#define MAX_ADC_CHAN 16

#define SINGLE_DIVIDER 512

#define MAX_SINGLE_RATE 51000.
#define MAX_DOUBLE_RATE 102000.

#pragma pack(1)

// Struct info about device
typedef struct _ADC1624X192SRV_INFO {
	USHORT		Size;					// sizeof(ADC1624X192SRV_INFO)
	PADCSRV_INFO pAdcInfo;
	double		Range[MAX_ADC_CHAN];	// входной диапазон для каждого канала
	double		Bias[MAX_ADC_CHAN];		// смещение нуля для каждого канала
	ULONG		InpSrc;
	ULONG		TestSrc;
	ULONG		HpfOff;					// High-pass filter (0 - on, 1 - off)
	ULONG		SpeedMode;				// 0 - SINGLE, 1 - DOUBLE, 2 - QUAD
} ADC1624X192SRV_INFO, *PADC1624X192SRV_INFO;

#pragma pack()

#endif // _ADC1624X192SRVINFO_H

// ****************** End of file Adc1624x192SrvInfo.h ***************
