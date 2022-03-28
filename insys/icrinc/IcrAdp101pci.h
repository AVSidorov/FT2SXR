//****************** File icrAdp101pci.h *******************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  базового модуля ADP101PCI 
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on ADP101PCI base module
//
//	Copyright (c) 2002-2003, Instrumental Systems,Corp.
//	Written by Dorokhin Brothers
//
//  History:
//  28-08-03 - builded
//  10-09-03 - modified
//
//*******************************************************************
#ifndef __ICRADP101PCI_H
 #define __ICRADP101PCI_H

#pragma pack(push, 1)    

const USHORT ADP101PCI_CFG_TAG = 0x5065; // тэг для структуры конфигурационных параметров базового модуля ADP101PCI

// ADP101PCI module configuration
// Конфигурационная структура базового модуля ADP101PCI
typedef struct _ICR_Cfg5065 {
	U16	wTag;		// tag of structure (0x5065)
	U16	wSize;		// size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
	U08	bAdmIfCnt;	// ADM counter ADM = 1
	U16	wMaxCpuClock;	// CPU clock (MHz) - 250 or 300
	U32	dBusClock;	// Bus clock (edited - default 100000000 Hz)
	U32	dSizeOfSDRAM;// Размер SDRAM в байтах 0, 64Mb, 128Mb, 256Mb
	U08	bHostPldCnt;// Host PLD counter
} ICR_Cfg5065, *PICR_Cfg5065, ICR_CfgAdp101Pci, *PICR_CfgAdp101Pci;

#pragma pack(pop)    

#endif	// __ICRADP101PCI_H

// ****************** End of file icrAdp101pci.h **********************
