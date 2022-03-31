//****************** File icrFmc124p.h *******************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  базовых модулей FMC124P
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on FMC124P base modules
//
//	Copyright (c) 2015, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  28-07-15 - builded
//
//*******************************************************************

#ifndef __ICRFMC124P_H_
 #define __ICRFMC124P_H_

#pragma pack(push, 1)    

const USHORT FMC124P_CFG_TAG = 0x551E; // тэг для структуры конфигурационных параметров базового модуля FMC124P

// FMC124P module configuration
// Конфигурационные параметры базового модуля FMC124P
typedef struct _ICR_CfgFmc124p {
	U16	wTag;				// тэг структуры (FMC124P_CFG_TAG)
	U16	wSize;				// размер всех следующих полей структуры
	U08	bAdmIfCnt;			// количество интерфейсов ADM
	U32	dSysGen;			// System generator in Hz (default 250 MHz)
	U08	bDdsType;			// тип синтезатора DDS (0-non, 1-v2)
	U08	bSwitchType;		// тип коммутатора (0-non, 1-type5(FMC124P))
	U08	bAdrSwitch;			// адресный код коммутатора: 0x48 по умолчанию
	U32	dRefGenDDS;			// Reference generator for DDS (default 19.2 MHz)
	U08	bTermGenDDS;		// temperature-compensated generator for DDS (0 - non temperature-compensated)
	U32	dRefGen100M;		// Reference generator 6 in Hz (default 100 MHz)
	U08 bIsSodimDDR3;		// Установлена ли память DDR3 SODIM
	U08 bIsInternalDDR3;	// Установлена ли встроенная память DDR3 256x32 Мслов
} ICR_CfgFmc124p, *PICR_CfgFmc124p, ICR_Cfg551E, *PICR_Cfg551E;

#pragma pack(pop)    

#endif // __ICRFMC124P_H_

// ****************** End of file icrFmc124p.h **********************
