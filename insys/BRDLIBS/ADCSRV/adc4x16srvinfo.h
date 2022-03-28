//****************** File Adc4x16SrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADC4x16 service
//
//	Copyright (c) 2002-2006, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  19-01-06 - builded
//
//*******************************************************************************

#ifndef _ADC4X16SRVINFO_H
 #define _ADC4X16SRVINFO_H

//#define MAX_CHAN 4

#pragma pack(1)

// Struct info about device
typedef struct _ADC4X16SRV_INFO {
	USHORT		Size;					// sizeof(ADC4x16SRV_INFO)
	PADCSRV_INFO pAdcInfo;
	ULONG		SubRefGen;				// frequency of submodule reference generator (значение субмодульного опорного генератора (Гц))
	double		SubExtClk;				// frequency of external clock (субмодульная внешняя частота тактирования (Гц))
} ADC4X16SRV_INFO, *PADC4X16SRV_INFO;

#pragma pack()

#endif // _ADC4X16SRVINFO_H

// ****************** End of file Adc4x16SrvInfo.h ***************
