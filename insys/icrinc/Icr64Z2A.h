//****************** File Icr64Z2A.h ********************************
//
//  Определения констант и структур
//	для работы с конфигурационным ППЗУ
//  базового модуля 64Z2A
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on 64Z2A base module
//
//	Copyright (c) 2008, Instrumental Systems,Corp.
//	Written by Tsikin Eugene
//
//  History:
//  11-01-08 - builded
//
//*******************************************************************
#ifndef __ICR64Z2A_H
 #define __ICR64Z2A_H

#pragma pack(push, 1)    

const USHORT ADP64Z2A_CFG_TAG = 0x642A; // тэг для структуры конфигурационных параметров базового модуля 64Z2A

// 64Z2A module configuration
// Конфигурационная структура базового модуля 64Z2A
typedef struct _ICR_Cfg642A {
	U16	wTag;			// tag of structure (0x642A)
	U16	wSize;			// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08	bAdmIfCnt;		// ADM counter ADM = 1
	U16	wMaxCpuClock;	// Макс. тактовая частота процессора (MHz) - 720, 1000
	U32	dSizeOfSDRAMA;	// Размер SDRAM на EMIFA (Mb) - 0, 128, 256, 512
	U32	dSizeOfSDRAMB;	// Размер SDRAM на EMIFB (Mb) - 0, 32
	U32	dSizeOfFlash;	// Размер Flash (Mb) - 0, 4
	U32	dSizeOfSBSRAM;	// Размер SBSRAM (Mb) - 0, 4, 8, 16
	U08	bHostPldCnt;	// Host PLD counter
} ICR_Cfg642A, *PICR_Cfg642A, ICR_Cfg64Z2A, *PICR_Cfg64Z2A;

#pragma pack(pop)    

#endif	// __ICR64Z2A_H

// ****************** End of file Icr64Z2A.h ************************
