//****************** File Dsp201SrvInfo.h **********************************
//
//  Определения констант, структур и функций
//	для API диалога свойств службы DSP201
//
//  Constants, structures & functions definitions
//	for API Property DSP201 service
//
//	Copyright (c) 2010, Instrumental Systems,Corp.
//	Written by Dorokhin S.
//
//
//*******************************************************************************

#ifndef _DSP201SRVINFO_H
 #define _DSP201SRVINFO_H


#pragma pack(1)

// Struct info about device
typedef struct _DSP201SRV_INFO {
	USHORT		Size;					// sizeof(DSP201SRV_INFO)
} DSP201SRV_INFO, *PDSP201SRV_INFO;

#pragma pack()

#endif // _DSP201SRVINFO_H

// ****************** End of file DSP201SrvInfo.h ***************

