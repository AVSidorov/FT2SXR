//****************** File icrAdp201v5.h *******************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  базового модуля ADP201v5 
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on ADP201v5 base module
//
//	Copyright (c) 2006, Instrumental Systems,Corp.
//	Written by Dorokhin Brothers
//
//  History:
//  15-05-06 - builded
//
//*******************************************************************
#ifndef __ICRADP201v5_H
 #define __ICRADP201v5_H

#pragma pack(push, 1)    

const USHORT Adp201v5_CFG_TAG = 0x7102; // тэг для структуры конфигурационных параметров базового модуля ADP201v5

// ADP201v5 module configuration
// Конфигурационная структура базового модуля ADP201v5
typedef struct _ICR_Cfg7102 {
	U16	wTag;			// tag of structure (0x7102)
	U16	wSize;			// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08	bAdmIfCnt;		// Число интерфейсов ADM = 2
	U16	wMaxCpuClock;	// CPU clock (MHz) - 500 or 600
	U32	dBusClock;		// Bus clock (edited - default 100000000 Hz)
	U32	dSizeOfSDRAM;	// Размер SDRAM в байтах 0, 64Mb, 128Mb, 256Mb
	U08	bHostPldCnt;	// Host PLD counter
	U16	wCpuMask;		// Маска наличия процессоров (битовое поле начиная с P1 -
						// P5 P4 P3 P2 P1, 0 - нет, 1 - есть)
	U16	wSdramMask;		// Маска наличия памяти у процессоров (битовое поле начиная с P1 - 
						// P5 P4 P3 P2 P1, 0 - нет, 1 - есть)
	U16	wPMask;			// Маска наличия внешнего разъема P2 ( 0 - нет, 1 - есть)
	U08	wSDRAM_RAS;		// SDRAM parameter RAS
	U08	wSDRAM_CAS;		// SDRAM parameter CAS
	U08	wSDRAM_BANK;	// SDRAM parameter BANK
	U08	wSDRAM_CL;		// SDRAM parameter CL

	U08	bAdmMask;		// Маска наличия интерфейсов ADM у процессоров
						// (битовое поле начиная с P1 - 
						// P5 P4 P3 P2 P1, 0 - нет, 1 - есть)
	U08	bIndustrial;	// Industrial version of module

} ICR_Cfg7102, *PICR_Cfg7102, ICR_CfgAdp201v5, *PICR_CfgAdp201v5;

#pragma pack(pop)

#endif	// __ICRADP201v5_H

// ****************** End of file icrAdp201v5.h **********************
