//****************** File icrAds3224k195cPci.h *******************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  базового модуля Ads3224k195cPci 
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on Ads3224k195cPci base module
//
//	Copyright (c) 2004, Instrumental Systems,Corp.
//	Written by Dorokhin Brothers
//
//  History:
//  30-08-04 - builded
//
//*******************************************************************
#ifndef __ICRAds3224k195cPci_H
 #define __ICRAds3224k195cPci_H

#pragma pack(push, 1)    

const USHORT Ads3224k195cPci_CFG_TAG = 0x5370; // тэг для структуры конфигурационных параметров базового модуля Ads3224k195cPci

// Ads3224k195cPci module configuration
// Конфигурационная структура базового модуля Ads3224k195cPci
typedef struct _ICR_Cfg5370 {
	U16	wTag;			// tag of structure (0x5370)
	U16	wSize;			// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08	bAdmIfCnt;		// Число интерфейсов ADM = 1
	U16	wMaxCpuClock;		// CPU clock (MHz) - 250 or 300
	U32	dBusClock;		// Bus clock (edited - default 100000000 Hz)
	U32	dSizeOfSDRAM;		// Размер SDRAM в байтах 0, 64Mb, 128Mb
	U08	bHostPldCnt;		// Host PLD counter
	U08	bDioPldCnt;		// Dio PLD counter
	U16	wSDRCON;		// init value for SDRAM  (edited)
	U08	bTypeOfLinks;		// External Links type (0 - TTL, 1 - LVDS)
} ICR_Cfg5370, *PICR_Cfg5370, ICR_CfgAds3224k195cPci, *PICR_CfgAds3224k195cPci;

#pragma pack(pop)

#endif	// __ICRAds3224k195cPci_H

// ****************** End of file icrAds3224k195cPci.h **********************
