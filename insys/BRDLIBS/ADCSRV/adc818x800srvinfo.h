//****************** File Adc818X800SrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADC818X800 service
//
//	Copyright (c) 2002-2007, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  08-06-07 - builded
//
//*******************************************************************************

#ifndef _ADC818X800SRVINFO_H
 #define _ADC818X800SRVINFO_H

#define MAX_ADC_CHAN 8

//#define SINGLE_DIVIDER 512

//#define MAX_SINGLE_RATE 51000.
//#define MAX_DOUBLE_RATE 102000.

#pragma pack(1)

// Struct info about device
typedef struct _ADC818X800SRV_INFO {
	USHORT		Size;					// sizeof(ADC818X800SRV_INFO)
	PADCSRV_INFO pAdcInfo;
	double		Range[MAX_ADC_CHAN];	// входной диапазон для каждого канала
	double		Bias[MAX_ADC_CHAN];		// смещение нуля для каждого канала
} ADC818X800SRV_INFO, *PADC818X800SRV_INFO;

#pragma pack()

#endif // _ADC818X800SRVINFO_H

// ****************** End of file Adc818X800SrvInfo.h ***************
