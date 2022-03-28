//****************** File Icr008A.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодулей 
//
//	Copyright (c) 2002-2004, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  30-08-04 - builded
//
//*********************************************************

#ifndef _ICR008A_H
 #define _ICR008A_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x008A

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg008A {
	USHORT	wTag;			// тэг структуры (ADM_CFG_TAG)
	USHORT	wSize;			// размер всех следующих полей структуры
	UCHAR	bAdmIfNum;		// номер интерфейса ADM
	ULONG	dGen;			// значение опорного генератора
	UCHAR	bDacCnt;		// количество ЦАП
	ULONG	dLPFCutoff;		// частота среза фильтра низких частот (Гц)
	UCHAR	bOutResist;		// выходное сопротивление (0 - 50 Ом, 1 - 75 Ом)
	UCHAR	bQuadModType;	// тип квадратурного модулятора ( 0 - нет, 1 – AD8345, 2 – AD8346, 3 – AD8349 )
	UCHAR	bIsExtClk;		// наличие входа внешнего тактирования
} ICR_Cfg008A, *PICR_Cfg008A, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR008A_H

//****************** End of file Icr008A.h **********************
