//****************** File device.h **********************************
//
//  Определения констант, структур и функций
//	для API плагина драйверов устройств
//
//  Constants, structures & functions definitions
//	for API Plug-in of device drivers
//
//	Copyright (c) 2002-2003, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  20-01-03 - builded
//
//*******************************************************************

#ifndef	__DEVICE_H_
#define	__DEVICE_H_

#ifdef DEVICE_API_EXPORTS
 #define DEVICE_API __declspec(dllexport)
#else
 #define DEVICE_API __declspec(dllimport)
#endif

#define MAX_DEVNAME 40
#define MAX_ADMNUM 4

#pragma pack(1)

// Bus Types
enum
{
	BUS_TYPE_UNKNOWN,
	BUS_TYPE_PCI,
	BUS_TYPE_ISA,
	BUS_TYPE_RS232,
	BUS_TYPE_1394,
	BUS_TYPE_USB,
	BUS_TYPE_PCMCIA,
	BUS_TYPE_VME,
};

// Struct info about device
typedef struct _DEVICE_INFO {
	USHORT	wSize;				// sizeof(DEVICE_INFO)
	USHORT	wType;				// Device Type
	UCHAR	bVersion;			// Device Version
    TCHAR	sName[MAX_DEVNAME];	// Device Name
	ULONG	nPid;				// Device Physical ID
    UCHAR	bBusType;			// Bus Type (Unknown, PCI, ISA, RS232, 1394, USB, PCMCIA, VME, ... )
    USHORT	wBusNum;			// Bus Number
    USHORT	wDevNum;			// Device Number 
    USHORT	wSlotNum;			// Slot Number 
	int		nInstance;			// Device Instance
	PUCHAR	pBaseCfgMem;		// Configuration memory of base module
	ULONG	nBaseCfgMemSize;	// Size of Configuration memory of base module
	ULONG	nRealBaseCfgSize;	// Realy size of Configuration memory of base module
	PUCHAR	pAdmCfgMem[MAX_ADMNUM];		// Configuration memory of submodule
	ULONG	nAdmCfgMemSize[MAX_ADMNUM];	// Size of Configuration memory of submodule
	ULONG	nRealAdmCfgSize[MAX_ADMNUM];// Realy size of Configuration memory of submodule
} DEVICE_INFO, *PDEVICE_INFO;

#pragma pack()

// Functions Declaration

#ifdef	__cplusplus
extern "C" {
#endif

typedef void __stdcall DEVICE_GetInfo_Type(int* devNum, PDEVICE_INFO pDevInfo); 
typedef void __stdcall DEVICE_Close_Type(PDEVICE_INFO pDevInfo); 
typedef int __stdcall DEVICE_ReadICR_Type(PDEVICE_INFO pDevInfo, UCHAR bDevs); 
typedef int __stdcall DEVICE_WriteICR_Type(PDEVICE_INFO pDevInfo, UCHAR bDevs); 

DEVICE_API void __stdcall DEVICE_GetInfo(int* devNum, PDEVICE_INFO pDevInfo);
DEVICE_API void __stdcall DEVICE_Close(PDEVICE_INFO pDevInfo);
DEVICE_API int __stdcall DEVICE_ReadIdCfgRom(PDEVICE_INFO pDevInfo, UCHAR bDevs);
DEVICE_API int __stdcall DEVICE_WriteIdCfgRom(PDEVICE_INFO pDevInfo, UCHAR bDevs);

#ifdef	__cplusplus
};
#endif

#endif	// __DEVICE_H_ 

// ****************** End of file device.h ***************
