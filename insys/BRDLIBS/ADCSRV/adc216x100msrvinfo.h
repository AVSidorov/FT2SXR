//****************** File Adc216x100mSrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADC216x100M service
//
//	Copyright (c) 2002-2007, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  11-01-07 - builded
//
//*******************************************************************************

#ifndef _ADC216X100MSRVINFO_H
 #define _ADC216X100MSRVINFO_H

#pragma pack(1)

const double ADC216X100M_BIASKOEF = 1.5; // 1.5 V

// Struct info about device
typedef struct _ADC216X100MSRV_INFO {
	USHORT		Size;					// sizeof(ADC216X100MSRV_INFO)
	PADCSRV_INFO pAdcInfo;
	ULONG		RefGen[3];				// frequency of generators (значения опорных генераторов (Гц))
	double		ExtClk;					// any external frequency of clock (любая из внешних частот тактирования (Гц))
	double		ThrClk;					// порог тактового сигнала
	UCHAR		Dither;					// 1 - включение розового шума
} ADC216X100MSRV_INFO, *PADC216X100MSRV_INFO;

#pragma pack()

#endif // _ADC216X100MSRVINFO_H

// ****************** End of file Adc216x100mSrvInfo.h ***************
