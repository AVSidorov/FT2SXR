//****************** File Icr00A8.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодулей FM816x600M
//
//*********************************************************

#ifndef _ICR00A8_H
 #define _ICR00A8_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x00A8

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg00A8 {
	U16	wTag;		// тэг структуры (ADM_CFG_TAG)
	U16	wSize;		// размер всех следующих полей структуры
	U08	bAdmIfNum;	// номер интерфейса ADM
	U08	bAdcCnt;	// количество АЦП: 4, 8
	U08	bAdcType;	// тип кристалла АЦП: 1-всегда
	U08	bThdac16Type;	// тип 16-канального кристалла ЦАП-ИПН: 1-всегда
	U16	wThdac16Range;	// шкала преобразования 16-канального ЦАП-ИПН: (мВ)
	U08	bGenType;	// тип кристалла внутр. генератора: 0-не програм-ый, 1-Si571
	U08	bGenAdr;	// адресный код внутр. генератора: 0x49 по умолчанию
	U32	nGenRef;    // заводская установка частоты внутр. генератора (Гц)
	U32	nGenRefMax; // максимальная частота внутр. генератора (Гц)
	U32	nAdcLoBand;	// нижняя частота полосы АЦП (Гц)
	U32	nAdcHiBand;	// верхняя частота полосы АЦП (Гц)
	U32	nAdcBitrateMin;	// минимальный битрейт кристалла АЦП (мегабит/сек)
	U32	nAdcBitrateMax;	// максимальный битрейт кристалла АЦП (мегабит/сек)
	U16	wThdac1Range;	// шкала преобразования 1-канального ЦАП-ИПН: (мВ)
	U08	bInpType;		// тип входа: 0-закрытый, 1-открытый
	U08	bReserve;		// Резерв
	U16	awUcm[8];		// Коды для UСМ, заносимые в LTC2668: 0..65535
	U16	awBiasMax[8];	// значение смещения нуля для кода 0 в LTC2668 (мВ)
	S16	awBiasMin[8];	// значение смещения нуля для кода 0xFFFF в LTC2668 (мВ)
	U16	awDac0[8];		// Коды в LTC2668 для  смещения нуля, равного 0 мВольт: 0..65535
} ICR_Cfg00A8, *PICR_Cfg00A8, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR00A8_H

// ****************** End of file Icr00A8.h **********************
