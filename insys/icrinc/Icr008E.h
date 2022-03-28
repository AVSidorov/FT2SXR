//****************** File Icr008E.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодулей ADM1624x192
//
//*********************************************************

#ifndef _ICR008E_H
 #define _ICR008E_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x008E

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg008E {
	U16		wTag;		// тэг структуры (ADM_CFG_TAG)
	U16		wSize;		// размер всех следующих полей структуры
	U08		bAdmIfNum;	// номер интерфейса ADM
	U08		bAdcCnt;	// количество АЦП: 0,2,4,...,14,16
	U08		bDacCnt;	// количество ЦАП: 0,2,4
} ICR_Cfg008E, *PICR_Cfg008E, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR008E_H

// ****************** End of file Icr008E.h **********************
