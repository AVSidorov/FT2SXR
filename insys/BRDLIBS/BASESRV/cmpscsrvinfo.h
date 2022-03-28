//****************** File CmpScSrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы компараторов базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of Base Comparator service
//
//	Copyright (c) 2006, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  14-10-06 - builded
//
//*******************************************************************

#ifndef _CMPSCSRVINFO_H
 #define _CMPSCSRVINFO_H

#ifdef _WIN32
	#ifdef SRVINFO_API_EXPORTS
	 #define SRVINFO_API __declspec(dllexport)
	#else
	 #define SRVINFO_API __declspec(dllimport)
	#endif
#else
    #define SRVINFO_API
#endif

#define NAME_SIZE 64

#pragma pack(1)

// Struct info about device
typedef struct _CMPSCSRV_INFO {
	USHORT		Size;				// sizeof(CMPSCSRV_INFO)
	TCHAR		Name[NAME_SIZE];	// название службы и модуля
	ULONG		RefPVS;				// Basic Voltage (опорное напряжение источников программируемых напряжений (мВольт))
	UCHAR		Source;				// источники сигналов для компараторов
	double		Threshold[2];		// пороги срабатывания компараторов
	UCHAR		SourceExt[2];		// источники сигналов в расширенном режиме
	UCHAR		ComparBits;			// разрядность компараторов
} CMPSCSRV_INFO, *PCMPSCSRV_INFO;

#pragma pack()

// Functions Declaration

#ifdef	__cplusplus
extern "C" {
#endif

//typedef int STDCALL INFO_DlgProp_Type(void* pDev, void* pServ, ULONG dlgMode, PVOID pSrvInfo); 
typedef int STDCALL INFO_InitPages_Type(void* pDev, DEVS_API_PropDlg* pPropDlg, void* pServ, ULONG* pNumDlg, PVOID pSrvInfo); 
typedef int STDCALL INFO_DeletePages_Type(ULONG numDlg); 
#ifdef _WIN32
typedef int STDCALL INFO_AddPages_Type(ULONG numDlg, HWND hParentWnd, ULONG viewMode, ULONG numPage, PVOID pPages); 
#endif
typedef int STDCALL INFO_SetProperty_Type(ULONG numDlg); 
typedef int STDCALL INFO_UpdateData_Type(ULONG numDlg, ULONG mode); 

//SRVINFO_API int STDCALL INFO_DialogProperty(void* pDev, void* pServ, ULONG dlgMode, PVOID pSrvInfo);
SRVINFO_API int STDCALL INFO_InitPages(void* pDev, DEVS_API_PropDlg* pPropDlg, void* pServ, ULONG* pNumDlg, PVOID pSrvInfo);
SRVINFO_API int STDCALL INFO_DeletePages(ULONG numDlg);
#ifdef _WIN32
SRVINFO_API int STDCALL INFO_AddPages(ULONG numDlg, HWND hParentWnd, ULONG viewMode, ULONG numPage, PVOID pPages);
#endif
SRVINFO_API int STDCALL INFO_SetProperty(ULONG numDlg);
SRVINFO_API int STDCALL INFO_UpdateData(ULONG numDlg, ULONG mode);

#ifdef	__cplusplus
};
#endif

#endif // _CMPSCSRVINFO_H

// ****************** End of file CmpScSrvInfo.h ***************
