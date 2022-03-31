//****************** File icrAdp101cp1.h *******************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  базового модуля ADP101E1 
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on ADP101E1 base module
//
//	Copyright (c) 2002-2005, Instrumental Systems,Corp.
//	Written by Dorokhin Brothers
//
//  History:
//
//*******************************************************************
#ifndef __ICRADP101E1_H
 #define __ICRADP101E1_H

#pragma pack(push, 1)    

const USHORT Adp101E1_CFG_TAG = 0x5465; // тэг для структуры конфигурационных параметров базового модуля ADP101E1

// ADP101E1 module configuration
// Конфигурационная структура базового модуля ADP101E1
typedef struct _ICR_Cfg5465 {
	U16	wTag;		// tag of structure (0x5465)
	U16	wSize;		// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08	bAdmIfCnt;	// ADM counter ADM = 1
	U16	wMaxCpuClock;	// CPU clock (MHz) - 250 or 300
	U32	dBusClock;	// Bus clock (edited - default 100000000 Hz)
	U32	dSizeOfSDRAM;// Размер SDRAM в байтах 0, 64Mb, 128Mb, 256Mb
	U08	bHostPldCnt;// Host PLD counter
} ICR_Cfg5465, *PICR_Cfg5465, ICR_CfgAdp101E1, *PICR_CfgAdp101E1;

#pragma pack(pop)    

#endif	// __ICRADP101E1_H

// ****************** End of file icrAdp101cp1.h **********************
