//****************** File BaseDacSrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы ЦАП базового модуля
//
//  Constants, structures & functions definitions
//	for API Property Dialog of Base DAC service
//
//	Copyright (c) 2002-2003, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  14-04-04 - builded
//
//*******************************************************************

#ifndef _BASEDACSRVINFO_H
 #define _BASEDACSRVINFO_H

#ifdef BDSINFO_API_EXPORTS
 #define BDSINFO_API __declspec(dllexport)
#else
 #define BDSINFO_API __declspec(dllimport)
#endif

#define BASE_MAXREFS 2
#define MAX_CHAN 2

#pragma pack(1)
/*
// Struct info about device
typedef struct _BASEDACSRV_INFO {
	USHORT		Size;					// sizeof(BASEDACSRV_INFO)
	PDACSRV_INFO pDacInfo;
	UCHAR		OnFilter;		// тип включенного фильтра 
	double		RangeAF;		// выходной диапазон при включенном активном фильтре для каждого канала
	USHORT		CutoffAF;		// cufoff frequence of active filter (частота среза активного фильтра (сотни Гц))
} BASEDACSRV_INFO, *PBASEDACSRV_INFO;
*/
#pragma pack()

#endif // _BASEDACSRVINFO_H

// ****************** End of file BaseDacSrvInfo.h ***************
