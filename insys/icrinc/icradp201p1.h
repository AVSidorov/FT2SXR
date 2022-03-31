//****************** File icrAdp201p1.h *******************************
//
//  Определения констант и структур 
//  для работы с конфигурационным ППЗУ
//  базового модуля ADP201P1 
//
//  Constants & structures definitions
//  for identification & configuration EPROM
//  on ADP201P1 base module
//
//  Copyright (c) 2002-2005, Instrumental Systems,Corp.
//  Written by Dorokhin Brothers
//
//
//*******************************************************************
#ifndef __ICRADP201P1_H
 #define __ICRADP201P1_H

#pragma pack(push, 1)    

const USHORT Adp201P1_CFG_TAG = 0x5087; // тэг для структуры конфигурационных параметров базового модуля ADP201P1

// ADP201P1 module configuration
// Конфигурационная структура базового модуля ADP201P1
typedef struct _ICR_Cfg5087 {
    U16 wTag;       // tag of structure (0x5087)
    U16 wSize;      // size of all following fields of structure (without wTag + wSize) = sizeof(ICR_XXXX) - 4
    U08 bAdmIfCnt;  // ADM counter ADM = 1
    U16 wMaxCpuClock;   // CPU clock (MHz)  
    U32 dBusClock;  // Bus clock (edited - default 100000000 Hz)
    U32 dSizeOfSDRAM;// Размер SDRAM в байтах 0, 64Mb, 128Mb, 256Mb
    U08 bHostPldCnt;// Host PLD counter
} ICR_Cfg5087, *PICR_Cfg5087, ICR_CfgAdp201P1, *PICR_CfgAdp201P1;

#pragma pack(pop)    

#endif  // __ICRADP201P1_H

// ****************** End of file icrAdp201p1.h **********************
