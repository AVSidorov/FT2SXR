//****************** File IcrRfdr4.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для модуля RFDR4
//
//*********************************************************

#ifndef _ICRRFDR4_H
#define _ICRRFDR4_H

#include "utypes.h"
#include "icr.h"

#pragma pack(push, 1)    

const USHORT RFDR4_CFG_TAG = 0x53B4; // тэг для структуры конфигурационных параметров модуля RFDR4

// Конфигурационная структура модуля RFDR4
typedef struct _ICR_Cfg53B4
{
	U16		wTag;				// tag of structure (0x53B4)
	U16		wSize;			// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08		bAdmIfCnt;		// количество интерфейсов ADM
	U08		bAdcCnt;			// Количество АЦП
	U08		bAttenuationMax;	// Максимальное значение аттенюатора (дБ)
	U08		bAttenuationStep;	// Шаг перестройки аттенюатора (дБ)
	U08		bDdcCnt;			// Количество каналов DDC на один канал АЦП
	U32		nIntGenFreq;		// Частота встроенного генератора (Гц)
	U32		nDdcDecimation;		// Коэффициент децимации DDC
	U32		nDdcBand;			// Ширина полосы DDC (Гц)
	U16		wDdcGain;			// Коэффициент передачи DDC (мили разы)
	U16		wFifoSizeK			// Размер ФИФО АЦП на один канал (тысяч отсчетов)
} ICR_Cfg53B4, *PICR_Cfg53B4, ICR_CfgRfdr4, *PICR_CfgRfdr4;

#pragma pack(pop) 

#endif // _ICRRFDR4_H

// ****************** End of file IcrRfdr4.h **********************
