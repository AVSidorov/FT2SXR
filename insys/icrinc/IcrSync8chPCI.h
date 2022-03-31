//****************** File IcrSync8chPCI.h *******************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  модулей Sync8chPCI
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on Sync8chPCI modules
//
//	Copyright (c) 2009, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  02-04-09 - builded
//
//*******************************************************************

#ifndef __ICRSYNC8CHPCI_H_
 #define __ICRSYNC8CHPCI_H_

#pragma pack(push, 1)    

const USHORT SYNC8CHPCI_CFG_TAG = 0x50A2; // тэг для структуры конфигурационных параметров модуля SYNC8CHPCI

// SYNC8CHPCI modules configuration
// Конфигурационные параметры модулей SYNC8CHPCI
typedef struct _ICR_CfgSync8chPCI {
	USHORT	wTag;			// тэг структуры (SYNC8CHPCI_CFG_TAG)
	USHORT	wSize;			// размер всех следующих полей структуры
	UCHAR	bAdmIfCnt;		// количество интерфейсов ADM
	ULONG	dSysGen;		// System generator in Hz (default 66 MHz)
	ULONG	dGen1;			// generator 1 in Hz (default 200 MHz)
	ULONG	dGen2;			// generator 2 in Hz (default 0 MHz)
	ULONG	dOcxoGen;		// OCXO generator in Hz (default 0 MHz)
	ULONG	dPllFreq;		// частота, выдаваемая ФАПЧ in Hz (default 0 GHz)
	UCHAR	bClkChanCnt;	// количество выходных каналов тактирования (default 2)
	UCHAR	bStrbChanCnt;	// количество выходных каналов cтробирования (default 2)
	UCHAR	bIsClkE;		// наличие дополнительного канала CLKE
	UCHAR	bIsClkF;		// наличие дополнительного канала CLKF
	UCHAR	bIsStrbA;		// наличие дополнительного канала STROA
	UCHAR	bIsStrbB;		// наличие дополнительного канала STROB
	UCHAR	bIsStrbD;		// наличие дополнительного канала STROD
	UCHAR	bIsSync;		// наличие дополнительных разъемов межмодульной синхронизации
	UCHAR	bIsLF;			// диапазон частот выходных сигналов тактирования: 1 - LF (0.1-400 МГц), 0 - HF(5-2500 МГц)
} ICR_CfgSync8chPCI, *PICR_CfgSync8chPCI, ICR_Cfg50A2, *PICR_Cfg50A2;

#pragma pack(pop)    

#endif // __ICRSYNC8CHPCI_H_

// ****************** End of file IcrSync8chPCI.h **********************
