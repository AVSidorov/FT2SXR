//****************** File Icr00A1.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодулей FM216x370MDA
//
//*********************************************************

#ifndef _ICR00A1_H
 #define _ICR00A1_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x00A1

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg00A1 {
	U16	wTag;		// тэг структуры (ADM_CFG_TAG)
	U16	wSize;		// размер всех следующих полей структуры
	U08	bAdmIfNum;	// номер интерфейса ADM
	U08	bAdcCnt;	// количество АЦП: 0-2
	U08	bDacCnt;	// количество ЦАП: 0-4
	U08	bDdcChip;	// тип кристалла DDC: 0-нет, 1-GC5016
	U16	reserve;
	U08	bGenType;	// тип кристалла внутр. генератора: 0-не програм-ый, 1-Si571
	U08	bGenAdr;	// адресный код внутр. генератора: 0x49 по умолчанию
	U32	nGenRef;    // заводская установка частоты внутр. генератора (Гц)
	U32	nGenRefMax; // максимальная частота внутр. генератора (Гц)
	U32	nAdcLoBand;	// нижняя частота полосы АЦП (Гц)
	U32	nAdcHiBand;	// верхняя частота полосы АЦП (Гц)
	U32	nDacLoBand;	// нижняя частота полосы ЦАП (Гц)
	U32	nDacHiBand;	// верхняя частота полосы ЦАП (Гц)
} ICR_Cfg00A1, *PICR_Cfg00A1, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR00A1_H

// ****************** End of file Icr00A1.h **********************
