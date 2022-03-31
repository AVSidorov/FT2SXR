//****************** File Icr6203.h *********************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  базового модуля 6203
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on 6203 base module
//
//	Copyright (c) 2008, Instrumental Systems,Corp.
//	Written by Tsikin Eugene
//
//  History:
//  28-01-08 - builded
//
//*******************************************************************
#ifndef __ICR6203_H
 #define __ICR6203_H

#pragma pack(push, 1)    

const USHORT ADP6203_CFG_TAG = 0x6203; // тэг для структуры конфигурационных параметров базового модуля 6203

// 6203 module configuration
// Конфигурационная структура базового модуля 6203
typedef struct _ICR_Cfg6202 {
	U16	wTag;			// tag of structure (0x6202)
	U16	wSize;			// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08	bAdmIfCnt;		// ADM counter ADM = 1
	U16	wMaxCpuClock;	// Макс. тактовая частота процессора (MHz), по умолч. 240
	U32	dSizeOfSDRAM;	// Размер SDRAM (Mb) - 0, 16, 32
	U32	dSizeOfDPRAM;	// Размер DPRAM (Kb) - 0, 128, 256
	U32	dSizeOfSSRAM;	// Размер SSRAM (Mb) - 0, 2, 4, 6, 8
	U08 bFifoCnt;		// Fifo counter
} ICR_Cfg6202, *PICR_Cfg6202, ICR_Cfg6203, *PICR_Cfg6203;;

#pragma pack(pop)    

#endif	// __ICR6203_H

// ****************** End of file Icr6203.h *************************
