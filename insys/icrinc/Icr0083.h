//****************** File Icr0083.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодулей 
//
//*********************************************************

#ifndef _ICR0083_H
 #define _ICR0083_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x0083

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg0083 {
	USHORT	wTag;		// тэг структуры (ADM_CFG_TAG)
	USHORT	wSize;		// размер всех следующих полей структуры
	UCHAR	bAdmIfNum;	// номер интерфейса ADM
	ULONG	dGen[3];	// значения опорных генераторов 1-3
	UCHAR	bAdcCnt;	// количество АЦП
} ICR_Cfg0083, *PICR_Cfg0083, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR0083_H

// ****************** End of file Icr0083.h **********************
