//****************** File IcrVK3.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для модуля DR-16
//
//*********************************************************

#ifndef _ICRDR16_H
#define _ICRDR16_H

#include "utypes.h"
#include "icr.h"

#pragma pack(push, 1)    

const USHORT DR16_CFG_TAG = 0x53B0; // тэг для структуры конфигурационных параметров модуля DR16

// DR-16 module configuration
// Конфигурационная структура модуля DR-16
typedef struct _ICR_Cfg53B0
{
	U16		wTag;				// tag of structure (0x5506)
	U16		wSize;				// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08		isDuc;				// наличие DUC (Digital Up Converter): 0, 1
	U08		isDucTransformator;	// наличие трансформаторного выхода DUCа: 0, 1
	U08		isGenerator;		// наличие : 0, 1
	U08		isSynthesizer;		// наличие : 0, 1
	U08		isExtTactInput;		// наличие : 0, 1
	U08		isExtStartInput;	// наличие : 0, 1
	U08		isExtStartOutput;	// наличие : 0, 1
	U08		isLinks;			// наличие : 0, 1
	U08		isPiox;				// наличие : 0, 1
	U08		isSynx;				// наличие : 0, 1
	U08		isLvdsPiox;			// наличие : 0, 1
	U08		isPci64;			// наличие : 0, 1
	U08		isIndustrial;		// наличие : 0, 1

	U08		bAdcCnt;			// количество АЦП: 1..16
	U08		bAdcBits;			// разрядность АЦП (число бит): 12, 14, 16
	U08		bAdcEncoding;		// тип кодировки: 0 - прямой код, 1 - двоично-дополнит.
	U32		nAdcMinRate;		// минимальная частота дискретизации (Гц)
	U32		nAdcMaxRate;		// максимальная частота дискретизации (Гц)
	U32		nAdcLowFreq;		// нижняя частота среза АЦП (Гц)
	U32		nAdcHighFreq;		// верхняя частота среза АЦП (Гц)
	U16		wAdcRange;			// входной диапазон АЦП (мВольт)
	U08		bAdcInpResist;		// входное сопротивление АЦП: 0 - 50 Ом, 1 - 75 Ом

	U08		bDdcCnt;			// количество микросхем  DDC: 1-4
	U32		nDucHighFreq;		// верхняя частота среза DUC (Гц)
	U16		wDucRange;			// шкала преобразования DUC (мВ)
	U32		nGenFreq;			// частота опорного генератора (Гц)
	U16		wGenPrec;			// точночть частоты опорного генератора (0.01 ppm)
	U32		nSyntFreq;			// частота синтезатора
} ICR_Cfg53B0, *PICR_Cfg53B0, ICR_CfgDR16, *PICR_CfgDR16;

#pragma pack(pop) 

#endif // _ICRDR16_H

// ****************** End of file IcrDR16.h **********************
