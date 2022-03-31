//****************** File Adc28x800mSrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADC28x800M service
//
//	Copyright (c) 2002-2004, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  14-04-04 - builded
//
//*******************************************************************************

#ifndef _ADC28X800MSRVINFO_H
 #define _ADC28X800MSRVINFO_H

//#define MAX_CHAN 2

#pragma pack(1)

// Struct info about device
typedef struct _ADC28X800MSRV_INFO {
	USHORT		Size;					// sizeof(ADC28X800MSRV_INFO)
	PADCSRV_INFO pAdcInfo;
	ULONG		MainRefGen;				// frequency of main generator (значение основного опорного генератора (Гц))
	double		DacMainGen;				// DAC frequency trim for main generator (значение ЦАП подстройки частоты для основного генератора (Гц))
	ULONG		MaxVcoGenFreq;			// maximum frequency of auxiliary generator (максимальное значение частоты дополнительного перестраиваемого генератора (Гц))
	ULONG		MinVcoGenFreq;			// minimum frequency of auxiliary generator (минимальное значение частоты дополнительного перестраиваемого генератора (Гц))
	double		VcoClk;					// frequency of VCO (значение частоты ГУН (Гц))
	double		ExtClk;					// any external frequency of clock (любая из внешних частот тактирования (Гц))
	double		GainTuning[MAX_CHAN];	// подстройка усиления каждого канала 
	ULONG		StartSrc;				// источник старта
	UCHAR		InvStart;				// инверсия сигнала старта
	double		StartLevel;				// уровень сигнала старта
} ADC28X800MSRV_INFO, *PADC28X800MSRV_INFO;

#pragma pack()

#endif // _ADC28X800MSRVINFO_H

// ****************** End of file Adc28x800mSrvInfo.h ***************
