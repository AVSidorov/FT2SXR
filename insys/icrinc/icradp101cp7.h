//****************** File icrAdp101cP7.h *******************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  базового модуля ADP101cP7 
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on ADP101cP7 base module
//
//	Copyright (c) 2004, Instrumental Systems,Corp.
//	Written by Dorokhin Brothers
//
//  History:
//  29-09-04 - builded
//
//*******************************************************************
#ifndef __ICRADP101cP7_H
 #define __ICRADP101cP7_H

#pragma pack(push, 1)    

const USHORT Adp101cP7_CFG_TAG = 0x5366; // тэг для структуры конфигурационных параметров базового модуля ADP101cP7

// ADP101cP7 module configuration
// Конфигурационная структура базового модуля ADP101cP7
typedef struct _ICR_Cfg5366 {
	U16	wTag;			// tag of structure (0x5366)
	U16	wSize;			// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08	bAdmIfCnt;		// Число интерфейсов ADM = 2
	U16	wMaxCpuClock;	// CPU clock (MHz) - 250 or 300
	U32	dBusClock;		// Bus clock (edited - default 100000000 Hz)
	U32	dSizeOfSDRAM;	// Размер SDRAM в байтах 0, 64Mb, 128Mb
	U08	bHostPldCnt;	// Host PLD counter
	U16	wSDRCON;		// init value for SDRAM  (edited)
	U16	bTypeOfLinks;	// External Links type (0 - TTL, 1 - LVDS)
	U16	wCpuMask;		// Маска наличия процессоров (битовое поле начиная с P1 -
						// P7 P6 P5 P4 P3 P2 P1, 0 - нет, 1 - есть)
	U16	wSdramMask;		// Маска наличия памяти у процессоров (битовое поле начиная с P1 - 
						// P7 P6 P5 P4 P3 P2 P1, 0 - нет, 1 - есть)
	U08 bX5;			// тип соединения на разъеме X5 (0-LINK, 1 - SYNX)
	U08 bX6;			// тип соединения на разъеме X6 (0-LINK, 1 - PIOX)

} ICR_Cfg5366, *PICR_Cfg5366, ICR_CfgAdp101cP7, *PICR_CfgAdp101cP7;

#pragma pack(pop)

#endif	// __ICRADP101cP7_H

// ****************** End of file icrAdp101cP7.h **********************
