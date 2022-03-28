//****************** File IcrIcrSync_cP6.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для модуля Sync-cP6
//
//*********************************************************

#ifndef _ICRSYNCCP6_H
#define _ICRSYNCCP6_H

#include "utypes.h"
#include "icr.h"

#pragma pack(push, 1)    

const USHORT SYNCCP6_CFG_TAG = 0x53A2; // тэг для структуры конфигурационных параметров модуля Sync-cP6

// Конфигурационная структура модуля Sync-cP6
typedef struct _ICR_Cfg53A2
{
	U16		wTag;				// tag of structure (0x53A2)
	U16		wSize;				// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U32		nIntGenFreq;		// Частота встроенного генератора (Гц)
	U08		isInpTactSign;		// Наличие входа тактового сигнала
} ICR_Cfg53A2, *PICR_Cfg53A2, ICR_CfgSync_cP6, *PICR_CfgSync_cP6;

#pragma pack(pop) 

#endif // _ICRSYNCCP6_H

// ****************** End of file IcrIcrSync_cP6.h **********************
