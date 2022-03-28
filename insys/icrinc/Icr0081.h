//****************** File Icr0081.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодулей ADM28x1G, ADM214x200M
//
//*********************************************************

#ifndef _ICR0081_H
 #define _ICR0081_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x0081

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg0081 {
	USHORT	wTag;		// тэг структуры (ADM_CFG_TAG)
	USHORT	wSize;		// размер всех следующих полей структуры
	UCHAR	bAdmIfNum;	// номер интерфейса ADM
	ULONG	dPllRefGen;	// опорный генератор для ФАПЧ in Hz (default 40 MHz)
	ULONG	dPllFreq;	// частота, выдаваемая ФАПЧ in Hz (default 2 GHz)
	UCHAR	bAdcCnt;	// количество АЦП
	UCHAR	bIsRF;		// входной тракт: 1 - радиочастотный, 0 - нет (ADM214x200M ver 1.1)
} ICR_Cfg0081, *PICR_Cfg0081, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR0081_H

// ****************** End of file Icr0081.h **********************
