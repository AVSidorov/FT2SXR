//****************** File icrFmc111p.h *******************************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  базовых модулей FMC111P
//
//  Constants & structures definitions
//	for identification & configuration EPROM
//  on FMC111P base modules
//
//	Copyright (c) 2016, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  14-07-16 - builded
//
//*******************************************************************

#ifndef __ICRFMC111P_H_
 #define __ICRFMC111P_H_

#pragma pack(push, 1)    

const USHORT FMC111P_CFG_TAG = 0x5521; // тэг для структуры конфигурационных параметров базового модуля FMC111P

// FMC111P module configuration
// Конфигурационные параметры базового модуля FMC111P
typedef struct _ICR_CfgFmc111p {
	U16	wTag;				// тэг структуры (FMC111P_CFG_TAG)
	U16	wSize;				// размер всех следующих полей структуры
	U08	bAdmIfCnt;			// количество интерфейсов ADM
	U32	dSysGen;			// System generator in Hz (default 250 MHz)
	U08	bDdsType;			// тип синтезатора DDS (0-non, 1-v2)
	U08	bSwitchType;		// тип коммутатора (0-non, 1-type5(like FMC124P))
	U08	bAdrSwitch;			// адресный код коммутатора: 0x48 по умолчанию
	U32	dRefGenDDS;			// Reference generator for DDS (default 19.2 MHz)
	U08	bTermGenDDS;		// temperature-compensated generator for DDS (0 - non temperature-compensated)
	U32	dRefGenMGT;			// MGT Reference generator 5 in Hz (default 156.25 MHz)
	U32	dRefGen100M;		// Reference generator 6 in Hz (default 100 MHz)
	U08	bIsPowerCtrl;		// Установлен ли контроллер питания
	U08	bAdrPower;			// адресный код контроллера питания: 0x48 по умолчанию
	U08 bIsInternalDDR3;	// Установлена ли встроенная память DDR3 256x64 Мслов
} ICR_CfgFmc111p, *PICR_CfgFmc111p, ICR_Cfg5521, *PICR_Cfg5521;

#pragma pack(pop)    

#endif // __ICRFMC111P_H_

// ****************** End of file icrFmc111p.h **********************
