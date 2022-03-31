//****************** File icrAdp101v7.h *******************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  базового модуля ADP101v7 
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on ADP101v7 base module
//
//	Copyright (c) 2004, Instrumental Systems,Corp.
//	Written by Dorokhin Brothers
//
//  History:
//  29-09-04 - builded
//
//*******************************************************************
#ifndef __ICRADP101v7_H
 #define __ICRADP101v7_H

#pragma pack(push, 1)    

const USHORT Adp101v7_CFG_TAG = 0x7101; // тэг для структуры конфигурационных параметров базового модуля ADP101v7

// ADP101v7 module configuration
// Конфигурационная структура базового модуля ADP101v7
typedef struct _ICR_Cfg7101 {
	U16	wTag;			// tag of structure (0x7101)
	U16	wSize;			// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08	bAdmIfCnt;		// Число интерфейсов ADM = 2
	U16	wMaxCpuClock;		// CPU clock (MHz) - 250 or 300
	U32	dBusClock;		// Bus clock (edited - default 100000000 Hz)
	U32	dSizeOfSDRAM;		// Размер SDRAM в байтах 0, 64Mb, 128Mb
	U08	bHostPldCnt;		// Host PLD counter
	U16	wSDRCON;		// init value for SDRAM  (edited)
	U08	bTypeOfLinks;		// External Links type (0 - TTL, 1 - LVDS)
	U16	wCpuMask;		// Маска наличия процессоров (битовое поле начиная с P1 -
					// P7 P6 P5 P4 P3 P2 P1, 0 - нет, 1 - есть)
	U16	wSdramMask;		// Маска наличия памяти у процессоров (битовое поле начиная с P1 - 
					// P7 P6 P5 P4 P3 P2 P1, 0 - нет, 1 - есть)
} ICR_Cfg7101, *PICR_Cfg7101, ICR_CfgAdp101v7, *PICR_CfgAdp101v7;

#pragma pack(pop)

#endif	// __ICRADP101v7_H

// ****************** End of file icrAdp101v7.h **********************
