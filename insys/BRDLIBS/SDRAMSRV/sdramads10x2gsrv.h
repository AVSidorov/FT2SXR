/*
 ****************** File SdramAds10x2gSrv.h *************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : SdramAds10x2g section
 *
 * (C) InSys by Dorokhin Andrey Nov, 2005
 *
 ************************************************************
*/

#ifndef _SDRAMADS10X2GSRV_H
 #define _SDRAMADS10X2GSRV_H

#include "ctrlsdram.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "sdramregs.h"
#include "sdramsrv.h"

#include "sdramambpcdsrvinfo.h"

const int SDRAMADS10X2G_TETR_ID = 0x25; // tetrad id

#pragma pack(push,1)

typedef struct _SDRAMADS10X2GSRV_CFG {
	SDRAMSRV_CFG SdramCfg;	//
	UCHAR	PrimWidth;		// Primary DDR SDRAM Width
	UCHAR	CasLatency;		// задержка сигнала выборки по столбцам
	UCHAR	Attributes;		// bit 1 = 1 - registered memory
} SDRAMADS10X2GSRV_CFG, *PSDRAMADS10X2GSRV_CFG;

#pragma pack(pop)

class CSdramAds10x2gSrv : public CSdramSrv
{

protected:

	virtual void GetSdramTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	UCHAR ReadSpdByte(CModule* pModule, ULONG OffsetSPD, ULONG CtrlSPD);
	ULONG GetCfgFromSpd(CModule* pModule, PSDRAMAMBPCDSRV_CFG pCfgSPD);
	virtual ULONG CheckCfg(CModule* pModule);
	virtual void MemInit(CModule* pModule, ULONG init);

public:

	CSdramAmbpcdSrv(int idx, int srv_num, CModule* pModule, PSDRAMAMBPCDSRV_CFG pCfg); // constructor
	
};

#endif // _SDRAMADS10X2GSRV_H

//
// End of file
//