//****************** File Icr008C.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодулей 
//
//	Copyright (c) 2002-2005, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  08-09-05 - builded
//
//*********************************************************

#ifndef _ICR008C_H
 #define _ICR008C_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x008C

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg008C {
	USHORT	wTag;		// тэг структуры (ADM_CFG_TAG)
	USHORT	wSize;		// размер всех следующих полей структуры
	UCHAR	bAdmIfNum;	// номер интерфейса ADM
	ULONG	dGen;		// значение опорного генератора
	UCHAR	bIsOxco;	// наличие OXCO
	UCHAR	bIsDblFreq;	// наличие удвоителя частоты опорного генератора
	ULONG	dLPFCutoff;	// частота среза фильтра низких частот (Гц)
	UCHAR	bIsExtClk;	// наличие входа внешнего тактирования
	UCHAR	bIsStrobe;	// наличие стробирования выходного сигнала
	USHORT	wMaxAmpl;	// максимальная амплитуда выходного сигнала
} ICR_Cfg008C, *PICR_Cfg008C, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR008С_H

//****************** End of file Icr008С.h **********************
