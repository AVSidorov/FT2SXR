//****************** File Fm412x500mSrvInfo.h **********************************
//
//  Constants, structures & functions definitions
//	for API Property Dialog of FM412x500M service
//
// (C) InSys by Ekkore Feb 2012
//
//*******************************************************************************

#ifndef _FM412x500MSRVINFO_H
 #define _FM412x500MSRVINFO_H

#define MAX_ADC_CHAN		4

#pragma pack(1)

// Struct info about device
typedef struct _FM412X500MSRV_Info {
	USHORT		Size;					// sizeof(FM412X500MSRV_Info)
	ADCSRV_INFO *pAdcInfo;
	double		ExtClk;					// any external frequency of clock (любая из внешних частот тактирования (Гц))
	REAL64		adGainDb[MAX_CHAN];		// коэффициенты передачи для WB (LF и HF) в каждом канале (дБ)
	REAL64		aGainTun[MAX_CHAN];		// подстройки коэффициентов передачи в каждом канале
	ULONG		StartSrc;				// источник старта
	UCHAR		InvStart;				// инверсия сигнала старта
	double		StartLevel;				// уровень сигнала старта
	ULONG		PllRefGen;				// frequency of PLL reference generator (значение внутреннего опорного генератора для ФАПЧ (Гц))
	ULONG		PllFreq;				// frequency of PLL (частота, выдаваемая ФАПЧ (Гц))
	ULONG		ExtPllRef;				// frequency of external PLL reference (значение внешней опорной частоты для ФАПЧ (Гц))
	UCHAR		InvClk;					// инверсия сигнала тактирования
	UCHAR		Version;				// версия субмодуля

	U32	nAdcType;	// тип кристалла АЦП: 0-всегда
	U32	nDacType;	// тип кристалла ЦАП: 0-всегда
	U32	nDacRange;	// шкала преобразования ЦАП: (мВ)
	U32	nGenType;	// тип кристалла внутр. генератора: 0-не програм-ый, 1-Si571
	U32	nGenAdr;	// адресный код внутр. генератора: 0x49 по умолчанию
	U32	nGenRef;    // заводская установка частоты внутр. генератора (Гц)
	U32	nGenRefMax; // максимальная частота внутр. генератора (Гц)
//	U32	anRange[8];	// шкалы преобразования АЦП (мВ)
	//U32	nRefGen0;             // опорный генератор 0 (Гц)
	//U32	nRefGen1;             // опорный генератор 1 (Гц)
	//U32	nRefGen1Min;          // опорный генератор 1 (Гц)
	//U32	nRefGen1Max;          // опорный генератор 1 (Гц)
	//U32	nRefGen1Type;         // тип опорного генератора 1
	//U32	nRefGen2;             // опорный генератор 2 (Гц)
	//U32	nRefGen2Type;         // тип опорного генератора 2
	//U32	nLpfPassBand;         // частота среза ФНЧ (Гц)

	double		adRange[8];		// шкалы преобразования (В)
	U32			nInputSource;	// Иточник входного сигнала: 1 - на оба входа подается Земля
	U32			nStartSlave;	// 1 - старт от разъема SLSTRIN
	U32			nClockSlave;	// 1 - такт от разъема SLCLKIN
	U32			nSlclkinClk;	// частота сигнала на разъеме SLCLKIN (Гц)
	U32			aisInpFilter[2];// 1 - включение ФНЧ для соотв. АЦП
	//U32			nDoubleFreq;	// режим удвоения частоты: 0-нет, 1-вход IN0, 2-вход IN1
	//double		dDoubleFreqBias1; //Коррекция смещения нуля для АЦП1 (%): -100..+100
	//double		dDoubleFreqRange1;//Коррекция шкалы преобразования для АЦП1 (%): -100..+100
} FM412X500MSRV_Info, *PFM412X500MSRV_Info;
//
#pragma pack()

#endif // _FM412x500MSRVINFO_H

// ****************** End of file Adc210x1gSrvInfo.h ***************
