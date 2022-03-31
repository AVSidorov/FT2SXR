//****************** File Icr0084.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодулей 
//
//*********************************************************

#ifndef _ICR0084_H
 #define _ICR0084_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x0084

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg0084 {
	USHORT	wTag;		// тэг структуры (ADM_CFG_TAG)
	USHORT	wSize;		// размер всех следующих полей структуры
	UCHAR	bAdmIfNum;	// номер интерфейса ADM
	UCHAR	bAdcCnt;	// количество АЦП
} ICR_Cfg0084, *PICR_Cfg0084, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR0084_H

// ****************** End of file Icr0084.h **********************
