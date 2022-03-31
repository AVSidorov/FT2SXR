//****************** File Icr008F.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодулей ADMDAC216x400M
//
//	Copyright (c) 2008, Instrumental Systems,Corp.
//	Written by Tsikin Eugene
//
//  History:
//  29-01-08 - builded
//
//*********************************************************

#ifndef _ICR008F_H
 #define _ICR008F_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x008F

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg008F {
	USHORT	wTag;		// тэг структуры (ADM_CFG_TAG)
	USHORT	wSize;		// размер всех следующих полей структуры
	UCHAR	bAdmIfNum;	// номер интерфейса ADM
	ULONG	dGen;		// значение опорного генератора
	UCHAR	bFreqTune;	// подстройка частоты встроенного опорного генератора
	UCHAR	bDacCnt;	// количество ЦАП
	ULONG	dLPFCutoff;	// частота среза фильтра низких частот (Гц)
	UCHAR	bOutResist;	// выходное сопротивление (0 - 50 Ом, 1 - 75 Ом)
	ULONG	dOscFreq;	// частота ГУН (Гц) (по умолч. 400 МГц)
	UCHAR	bOutCasMod;	// модификация выходного каскада 0 - LF, 1 - HF
	UCHAR	bQuadModType;// тип квадратурного модулятора ( 0 - нет, 1 – AD8345, 2 – AD8346, 3 – AD8349 )
	UCHAR	bIsExtClk;	// наличие входа внешнего тактирования
} ICR_Cfg008F, *PICR_Cfg008F, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR008F_H

// ****************** End of file Icr008F.h **********************
