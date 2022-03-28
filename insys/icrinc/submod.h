//****************** File submod.h *********************************
//
//  Определения констант, структур и функций
//	для API плагина пользовательского интерфейса
//	субмодулей
//
//  Constants, structures & functions definitions
//	for API Plug-in of user interface of SubModules.
//
//	Copyright (c) 2002-2003, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  20-01-03 - builded
//  20-08-03 - modified typedef struct SUBMOD_INFO for compatibility with Builder C++
//				old typedef struct  {
//				new typedef struct _SUBMOD_INFO {
//
//*******************************************************************

#ifndef	__SUBMOD_H_
#define	__SUBMOD_H_

#ifdef SUBMOD_API_EXPORTS
 #define SUBMOD_API __declspec(dllexport)
#else
 #define SUBMOD_API __declspec(dllimport)
#endif

#include "icradmlst.h"

#pragma pack(1)

// Struct info about submodule
typedef struct _SUBMOD_INFO {
	USHORT		wSize;			// sizeof(SUBMOD_INFO)
	PUCHAR		pCfgMem;		// Configuration memory
	ULONG		nCfgMemSize;	// Size of Configuration memory
	ULONG		nRealCfgSize;	// Realy size of Configuration memory
	ICR_ADMType	Type;			// Submodule Type
    TCHAR		sName[80];		// Submodule Name
} SUBMOD_INFO, *PSUBMOD_INFO;

#pragma pack()

//
//====== Functions Declaration
//

#ifdef	__cplusplus
extern "C" {
#endif

typedef void __stdcall SUBMOD_GetInfo_Type(int* devNum, PSUBMOD_INFO pDevInfo); 
typedef void __stdcall SUBMOD_Close_Type(PSUBMOD_INFO pDevInfo); 
typedef int __stdcall SUBMOD_SetProp_Type(PSUBMOD_INFO pDevInfo); 
typedef int __stdcall SUBMOD_GetProp_Type(PSUBMOD_INFO pDevInfo); 
typedef int __stdcall SUBMOD_DlgProp_Type(PSUBMOD_INFO pDevInfo); 

SUBMOD_API void __stdcall SUBMOD_GetInfo(int* devNum, PSUBMOD_INFO pDevInfo);
SUBMOD_API void __stdcall SUBMOD_Close(PSUBMOD_INFO pDevInfo);
SUBMOD_API int __stdcall SUBMOD_SetProperty(PSUBMOD_INFO pDevInfo);
SUBMOD_API int __stdcall SUBMOD_GetProperty(PSUBMOD_INFO pDevInfo);
SUBMOD_API int __stdcall SUBMOD_DialogProperty(PSUBMOD_INFO pDevInfo);

#ifdef	__cplusplus
};
#endif

#endif	// __SUBMOD_H_ 

// ****************** End of file submod.h ***************

