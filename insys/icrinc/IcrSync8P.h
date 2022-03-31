//****************** File IcrSync8P.h *******************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  модулей Sync8P
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on Sync8P modules
//
//	Copyright (c) 2014, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  31-10-14 - builded
//
//*******************************************************************

#ifndef __ICRSYNC8P_H_
 #define __ICRSYNC8P_H_

#pragma pack(push, 1)

const USHORT SYNC8P_CFG_TAG = 0x551B; // тэг для структуры конфигурационных параметров модуля SYNC8P

// SYNC8P modules configuration
// Конфигурационные параметры модулей SYNC8P
typedef struct _ICR_CfgSync8P {
	U16		wTag;			// тэг структуры (SYNC8P_CFG_TAG)
	U16		wSize;			// размер всех следующих полей структуры
	U08		bAdmIfCnt;		// количество интерфейсов ADM
	U32		dSysGen;		// System generator in Hz (default 66 MHz)
	U32		dGen1;			// частота генератора 1, если он непрограммируемый, или заводская частота Si571 (default 50 MHz)
	U32		dGen2;			// generator 2 in Hz (default 0 MHz)
	U32		dOcxoGen;		// OCXO generator in Hz (default 0 MHz)
	U32		dPllFreq;		// частота, выдаваемая ФАПЧ in Hz (default 0 GHz)
	U08		bClkChanCnt;	// количество выходных каналов тактирования (default 4)
	U08		bStrbChanCnt;	// количество выходных каналов cтробирования (default 4)
	U08		bIsClkE;		// наличие дополнительного канала CLKE
	U08		bIsClkF;		// наличие дополнительного канала CLKF
	U08		bIsStrbA;		// наличие дополнительного канала STROA
	U08		bIsStrbB;		// наличие дополнительного канала STROB
	U08		bIsStrbD;		// наличие дополнительного канала STROD
	U08		bIsSync;		// наличие дополнительных разъемов межмодульной синхронизации
	U08		bIsLF;			// диапазон частот выходных сигналов тактирования: 1 - LF (0.1-400 МГц), 0 - HF(5-2500 МГц)
	U08		bGen1Type;		// тип внутреннего генератора 1 (0-непрограммируемый, 1-Si571)
	//U32		nRefGen1;		// частота генератора 1, если он непрограммируемый, или заводская частота (default 50 MHz)
	U32		nRefMaxGen1;	// максимальная частота генератора 1 (Гц)
	U08		bAdrGen1;		// адресный код  генератора 1: 0x49 по умолчанию
	U08		bGenPllType;	// тип внутреннего генератора ГУН (0-непрограммируемый, 1-Si571)
	U32		nRefGenPll;		// частота генератора ГУН, если он непрограммируемый, или заводская частота (default 50 MHz)
	U32		nRefMinGenPll;	// минимальная частота генератора ГУН (Гц)
	U32		nRefMaxGenPll;	// максимальная частота генератора ГУН (Гц)
	U08		bAdrGenPll;		// адресный код ГУН генератора: 0x49 по умолчанию
} ICR_CfgSync8P, *PICR_CfgSync8P, ICR_Cfg551B, *PICR_Cfg551B;

#pragma pack(pop)    

#endif // __ICRSYNC8P_H_

// ****************** End of file IcrSync8P.h **********************
