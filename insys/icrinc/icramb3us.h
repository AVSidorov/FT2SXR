//****************** File icrAmb3us.h *******************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  базового модуля AMB3US
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on AMB3US base module
//
//	Copyright (c) 2007, Instrumental Systems,Corp.
//	Written by Dorokhin Brothers
//
//  History:
//
//*******************************************************************
#ifndef __ICRAMB3US_H
 #define __ICRAMB3US_H

#pragma pack(push, 1)    

const USHORT AMB3US_CFG_TAG = 0x5466; // тэг для структуры конфигурационных параметров базового модуля AMB3US

// AMB3US module configuration
// Конфигурационная структура базового модуля AMB3US
typedef struct _ICR_Cfg5466 {
	U16	wTag;		// tag of structure (0x5466)
	U16	wSize;		// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08	bAdmIfCnt;	// ADM counter ADM = 1
	U16	wReserved;	// not use
	U32	dSysClock;	// Bus clock (edited - default 100000000 Hz)
} ICR_Cfg5466, *PICR_Cfg5466, ICR_CfgAmb3us, *PICR_CfgAmb3us;

#pragma pack(pop)    

#endif	// __ICRAMB3US_H

// ****************** End of file icrAmb3us.h **********************
