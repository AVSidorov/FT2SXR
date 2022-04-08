//****************** File Icr009E.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодулей FM1620x1M
//
//*********************************************************

#ifndef _ICR009E_H
 #define _ICR009E_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x009E

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg009E {
	U16	wTag;		// тэг структуры (ADM_CFG_TAG)
	U16	wSize;		// размер всех следующих полей структуры
	U08	bAdmIfNum;	// номер интерфейса ADM
	U08	bAdcCnt;	// количество АЦП: 0-16
	U08	bDacCnt;	// количество ЦАП: 0-4
	U08	bGenType;	// тип кристалла внутр. генератора: 0-не програм-ый, 1-Si571
	U08	bGenAdr;	// адресный код внутр. генератора: 0x49 по умолчанию
	U32	nGenRef;    // заводская установка частоты внутр. генератора (Гц)
	U32	nGenRefMax; // максимальная частота внутр. генератора (Гц)
	U08	bBiasChip;	// тип кристалла (цап), управляющего смещением 0 для АЦП: 0-нет, 1-LTC2668
	U08	bThrChip;	// тип кристалла (цап), управляющего порогами компаратора для АЦП: 0-нет, 1-AD5686RBCPZ
	U08	bDivChip;	// тип кристалла делителя частоты: 0-нет, 1-LMK01801BISQ
	U08	bAdcChip;	// тип кристалла АЦП: 0-нет, 1-??
	U08	bDacChip;	// тип кристалла ЦАП: 0-нет, 1-??
} ICR_Cfg009E, *PICR_Cfg009E, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR009E_H

// ****************** End of file Icr009E.h **********************