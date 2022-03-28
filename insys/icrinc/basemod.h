//****************** File basemod.h *********************************
// ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
//  Определения констант, структур и функций
//	для API плагина пользовательского интерфейса
//	базовых модулей
//
//  Constants, structures & functions definitions
//	for API Plug-in of user interface of Base Modules.
//
//	Copyright (c) 2002-2003, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  20-01-03 - builded
//
//*******************************************************************

#ifndef	__BASEMOD_H_
#define	__BASEMOD_H_

#ifdef BASEMOD_API_EXPORTS
 #define BASEMOD_API __declspec(dllexport)
#else
 #define BASEMOD_API __declspec(dllimport)
#endif

#include "icrbmlst.h"

#define MAX_BMNAME 40

#pragma pack(1)

// Struct info about base module
typedef struct _BASEMOD_INFO {
	USHORT		wSize;				// sizeof(BASEMOD_INFO) (into Plug-In)
	PUCHAR		pCfgMem;			// Configuration memory pointer (from Plug-In)
	ULONG		nCfgMemSize;		// Configuration memory size (from Plug-In)
	ULONG		nRealCfgSize;		// Realy configuration memory size (from Plug-In)
	ICR_BMType	dType;				// Base Module Type (from Plug-In)
    TCHAR		sName[MAX_BMNAME];	// Base Module Name (from Plug-In)
	UCHAR		bAdmIfCnt;			// количество интерфейсов ADM (into Plug-In)
} BASEMOD_INFO, *PBASEMOD_INFO;

#pragma pack()

// Functions Declaration

#ifdef	__cplusplus
extern "C" {
#endif

typedef void __stdcall BASEMOD_GetInfo_Type(int* devNum, PBASEMOD_INFO pDevInfo); 
typedef void __stdcall BASEMOD_Close_Type(PBASEMOD_INFO pDevInfo); 
typedef int __stdcall BASEMOD_GetProp_Type(PBASEMOD_INFO pDevInfo); 
typedef int __stdcall BASEMOD_SetProp_Type(PBASEMOD_INFO pDevInfo); 
typedef int __stdcall BASEMOD_DlgProp_Type(PBASEMOD_INFO pDevInfo); 

BASEMOD_API int	asdasd(int aaa);
BASEMOD_API void __stdcall BASEMOD_GetInfo(int* devNum, PBASEMOD_INFO pDevInfo);
BASEMOD_API void __stdcall BASEMOD_Close(PBASEMOD_INFO pDevInfo);
BASEMOD_API int __stdcall BASEMOD_GetProperty(PBASEMOD_INFO pDevInfo);
BASEMOD_API int __stdcall BASEMOD_SetProperty(PBASEMOD_INFO pDevInfo);
BASEMOD_API int __stdcall BASEMOD_DialogProperty(PBASEMOD_INFO pDevInfo);

#ifdef	__cplusplus
};
#endif

#endif	// __BASEMOD_H_ 

// ****************** End of file basemod.h ***************
