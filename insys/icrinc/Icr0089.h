//****************** File Icr0089.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодуля ADM212x200M 
//
//*********************************************************

#ifndef _ICR0089_H
 #define _ICR0089_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x0089

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg0089 {
	USHORT	wTag;		// тэг структуры (ADM_CFG_TAG)
	USHORT	wSize;		// размер всех следующих полей структуры
	UCHAR	bAdmIfNum;	// номер интерфейса ADM
	ULONG	dGen;		// значения опорного генератора
	UCHAR	bAdcCnt;	// количество АЦП
} ICR_Cfg0089, *PICR_Cfg0089, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR0083_H

// ****************** End of file Icr0089.h **********************
