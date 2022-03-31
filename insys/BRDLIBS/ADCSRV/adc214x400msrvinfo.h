//****************** File Adc214x400mSrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADM214x400M(WB) and ADM212x500M(WB) service
//
// (C) InSys by Ekkore Aug 2008
//
//*******************************************************************************

#ifndef _ADC214X400MSRVINFO_H
 #define _ADC214X400MSRVINFO_H

#pragma pack(1)

// Struct info about device
typedef struct _ADC214X400MSRV_Info {
	USHORT		Size;					// sizeof(ADC214X400MSRV_INFO)
	ADCSRV_INFO *pAdcInfo;
	double		ExtClk;					// any external frequency of clock (любая из внешних частот тактирования (Гц))
	REAL64		aGainDb[MAX_CHAN];		// коэффициенты передачи для WB (LF и HF) в каждом канале (дБ)
	REAL64		aGainTun[MAX_CHAN];		// подстройки коэффициентов передачи в каждом канале
	ULONG		StartSrc;				// источник старта
	UCHAR		InvStart;				// инверсия сигнала старта
	double		StartLevel;				// уровень сигнала старта
	ULONG		PllRefGen;				// frequency of PLL reference generator (значение внутреннего опорного генератора для ФАПЧ (Гц))
	ULONG		PllFreq;				// frequency of PLL (частота, выдаваемая ФАПЧ (Гц))
	ULONG		ExtPllRef;				// frequency of external PLL reference (значение внешней опорной частоты для ФАПЧ (Гц))
	UCHAR		InvClk;					// инверсия сигнала тактирования
	UCHAR		Version;				// версия субмодуля
	U32			nSubType;				// тип субмодуля: 214x400M: 0x000-Standard, 0x001 -WB HF, 0x002-WB LF 
										//            или 212x500M: 0x100-Standard, 0x101 -WB HF, 0x102-WB LF
	U32			aRefGen[2];				// опорные генераторы 0 и 1 (Гц)
	U32			nStartClkSl;
	double		adIcrRange[4];			// шкалы преобразования для Standard (мВ) (default 10000, 2000, 500, 100)
	U32			nInputSource;			// Иточник входного сигнала: 1 - на оба входа подается Земля
} ADC214X400MSRV_Info, *PADC214X400MSRV_Info;

#pragma pack()

#endif // _ADC214X400MSRVINFO_H

// ****************** End of file Adc214x400mSrvInfo.h ***************
