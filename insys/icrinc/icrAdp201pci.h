//****************** File icrAdp101pci.h *******************************
//
//  Определения констант и структур 
//  для работы с конфигурационным ППЗУ
//  базового модуля ADP201PCI 
//
//  Constants & structures definitions
//  for identification & configuration EPROM
//  on ADP201PCI base module
//
//  Copyright (c) 2002-2004, Instrumental Systems,Corp.
//  Written by Dorokhin Brothers
//
//
//*******************************************************************
#ifndef __ICRADP201PCI_H
 #define __ICRADP201PCI_H

#pragma pack(push, 1)    

const USHORT Adp201PCI_CFG_TAG = 0x5085; // тэг для структуры конфигурационных параметров базового модуля ADP201PCI

// ADP201PCI module configuration
// Конфигурационная структура базового модуля ADP201PCI
typedef struct _ICR_Cfg5085 {
    U16 wTag;       // tag of structure (0x5085)
    U16 wSize;      // size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
    U08 bAdmIfCnt;  // ADM counter ADM = 1
    U16 wMaxCpuClock;   // CPU clock (MHz)  
    U32 dBusClock;  // Bus clock (edited - default 100000000 Hz)
    U32 dSizeOfSDRAM;// Размер SDRAM в байтах 0, 64Mb, 128Mb, 256Mb
    U08 bHostPldCnt;// Host PLD counter
} ICR_Cfg5085, *PICR_Cfg5085, ICR_CfgAdp201Pci, *PICR_CfgAdp201Pci;

#pragma pack(pop)    

#endif  // __ICRADP201PCI_H

// ****************** End of file icrAdp201pci.h **********************
