//****************** File IcrSyncCP6R.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для модуля Sync-CP6R
//
//*********************************************************

#ifndef _ICRSYMCCP6R_H
#define _ICRSYMCCP6R_H

#include "utypes.h"
#include "icr.h"

#pragma pack(push, 1)    

const USHORT RFDR4_CFG_TAG = 0x53B7; // тэг для структуры конфигурационных параметров модуля Sync-CP6R

// Конфигурационная структура модуля Sync-CP6R
typedef struct _ICR_Cfg53B7
{
	U16		wTag;				// tag of structure (0x53B7)
	U16		wSize;			// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08		bDacCnt;			// Количество ЦАП
	U08		bRes[3];			// Резерв
	U32		nTactGenFreq;		// Частота встроенного тактового генератора (Гц)
	U32		nRefGenFreq;		// Частота встроенного опорного генератора (Гц)
	U32		nDucInterpolation;	// Коэффициент интерполяции DUC
	U16		wFifoSizeK			// Размер ФИФО АЦП на один канал (тысяч отсчетов)
} ICR_Cfg53B7, *PICR_Cfg53B7, ICR_CfgSyncCP6R, *PICR_SyncCP6R;

#pragma pack(pop) 

#endif // _ICRSYMCCP6R_H

// ****************** End of file IcrSyncCP6R.h **********************
