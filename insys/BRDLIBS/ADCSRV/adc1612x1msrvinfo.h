//****************** File Adc1612x1mSrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADM1612x1M service
//
// (C) InSys by Ekkore Aug 2008
//
//*******************************************************************************

#ifndef _ADC1612x1MSRVINFO_H
 #define _ADC1612x1MSRVINFO_H

#pragma pack(1)

// Struct info about device
typedef struct _ADC1612x1MSRV_Info {
	USHORT		Size;					// sizeof(ADC1612x1MSRV_INFO)
	ADCSRV_INFO *pAdcInfo;
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
	U32			nSubType;				// тип субмодуля: 1612x1M: 0x000-Standard, 0x001 -WB LF, 0x002-WB HF 
										//            или 212x500M: 0x100-Standard, 0x101 -WB LF, 0x102-WB HF
	U32			aRefGen[2];				// опорные генераторы 0 и 1 (Гц)
} ADC1612x1MSRV_Info, *PADC1612x1MSRV_Info;

#pragma pack()

#endif // _ADC1612x1MSRVINFO_H

// ****************** End of file Adc1612x1mSrvInfo.h ***************
