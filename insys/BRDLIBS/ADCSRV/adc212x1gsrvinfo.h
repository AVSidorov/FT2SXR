//****************** File Adc212x1gSrvInfo.h **********************************
//
//  Constants, structures & functions definitions
//	for API Property Dialog of ADM212x1G service
//
// (C) InSys by Ekkore Okt 2010
//
//*******************************************************************************

#ifndef _ADC212x1GSRVINFO_H
 #define _ADC212x1GSRVINFO_H

#pragma pack(1)

// Struct info about device
typedef struct _ADC212X1GSRV_Info {
	USHORT		Size;					// sizeof(ADC212X1GSRV_INFO)
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
	U32	nRefGen0;             // опорный генератор 0 (Гц)
	U32	nRefGen1;             // опорный генератор 1 (Гц)
	U32	nRefGen1Min;          // опорный генератор 1 (Гц)
	U32	nRefGen1Max;          // опорный генератор 1 (Гц)
	U32	nRefGen1Type;         // тип опорного генератора 1
	U32	nRefGen2;             // опорный генератор 2 (Гц)
	U32	nRefGen2Type;         // тип опорного генератора 2
	U32	nLpfPassBand;         // частота среза ФНЧ (Гц)
//	U32			nStartClkSl;
	double		adIcrRange[4];	// шкалы преобразования для Standard (мВ) (default 10000, 2000, 500, 100)
	U32			nInputSource;	// Иточник входного сигнала: 1 - на оба входа подается Земля
	U32			nStartSlave;	// 1 - старт от разъема SLSTRIN
	U32			nClockSlave;	// 1 - такт от разъема SLCLKIN
	U32			nSlclkinClk;	// частота сигнала на разъеме SLCLKIN (Гц)
	U32			aisInpFilter[2];// 1 - включение ФНЧ для соотв. АЦП
	U32			nDoubleFreq;	// режим удвоения частоты: 0-нет, 1-канал 0, 2-канал 1
} ADC212X1GSRV_Info, *PADC212X1GSRV_Info;
//
#pragma pack()

#endif // _ADC212x1GSRVINFO_H

// ****************** End of file Adc212x1gSrvInfo.h ***************
