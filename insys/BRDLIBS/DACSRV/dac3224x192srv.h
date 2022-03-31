/*
 ****************** File Dac3224x192Srv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : DAC3224X192 section
 *
 * (C) InSys by Dorokhin Andrey Oct 2005
 *
 ************************************************************
*/

#ifndef _DAC3224X192SRV_H
 #define _DAC3224X192SRV_H

#include "ctrldac3224x192.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "dacsrv.h"
#include "mainregs.h"
#include "dac3224x192regs.h"

#include "dac3224x192srvinfo.h"

const int DAC3224X192_TETR_ID = 0x24; // tetrad id

#pragma pack(push,1)

typedef struct _DAC3224X192SRV_CFG {
	DACSRV_CFG DacCfg;
	U32		ChipCnt;		// число установленных микросхем ЦАП
} DAC3224X192SRV_CFG, *PDAC3224X192SRV_CFG;

#pragma pack(pop)

class CDac3224x192Srv: public CDacSrv
{

protected:

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetDacTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
//	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	void WriteDacReg(CModule* pModule, U32 chipMask, U32 addr, U32 data);
	void ChipsInit(CModule* pModule);

	virtual int SetProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	virtual int SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetChanMask(CModule* pModule, ULONG mask);
	virtual int GetChanMask(CModule* pModule, ULONG& mask);
	virtual int SetCode(CModule* pModule, ULONG type);
	virtual int GetCode(CModule* pModule, ULONG& type);
	virtual int SetSpecific(CModule* pModule, PBRD_DacSpec pSpec);

public:

	CDac3224x192Srv(int idx, int srv_num, CModule* pModule, PDAC3224X192SRV_CFG pCfg); // constructor

};

#endif // _DAC3224X192SRV_H

//
// End of file
//