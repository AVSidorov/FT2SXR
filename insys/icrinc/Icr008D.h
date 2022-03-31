//****************** File Icr008D.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодуля ADM-FOTR
//
//*********************************************************

#ifndef _ICR008D_H
 #define _ICR008D_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x008D

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg008D {
	USHORT	wTag;		// тэг структуры (ADM_CFG_TAG)
	USHORT	wSize;		// размер всех следующих полей структуры
	UCHAR	bAdmIfNum;	// номер интерфейса ADM
	ULONG	dGen;		// значения опорного генератора
	UCHAR	bChanCnt;	// количество каналов
	UCHAR	bTranciever; // тип приемо-передатчика
} ICR_Cfg008D, *PICR_Cfg008D, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR008D_H

// ****************** End of file Icr008D.h **********************
