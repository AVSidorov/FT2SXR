//****************** File Icr6713.h *********************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  базового модуля 6713
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on 6713 base module
//
//	Copyright (c) 2008, Instrumental Systems,Corp.
//	Written by Tsikin Eugene
//
//  History:
//  18-01-08 - builded
//
//*******************************************************************
#ifndef __ICR6713_H
 #define __ICR6713_H

#pragma pack(push, 1)

const USHORT ADP6713_CFG_TAG = 0x6713; // тэг для структуры конфигурационных параметров базового модуля 6713

// 6713 module configuration
// Конфигурационная структура базового модуля 6713
typedef struct _ICR_Cfg642A {
	U16	wTag;			// tag of structure (0x6713)
	U16	wSize;			// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08	bAdmIfCnt;		// ADM counter ADM = 1
	U32	dSizeOfSDRAM;	// Размер SDRAM (Mb) - 0, 64, 128
	U32	dSizeOfFlash;	// Размер Flash (Mb) - 0, 4
	U32	dSizeOfPLDSRAM;	// Размер SRAM на ПЛИС HOST (Kb) - 0, 512
	U08	bHostPldCnt;	// Host PLD counter
} ICR_Cfg6713, *PICR_Cfg6713, ICR_Cfg6713, *PICR_Cfg6713;

#pragma pack(pop)    

#endif	// __ICR6713_H

// ****************** End of file Icr6713.h *************************
