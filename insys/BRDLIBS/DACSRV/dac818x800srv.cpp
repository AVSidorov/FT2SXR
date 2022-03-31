/*
 ***************** File Dac818X800Srv.cpp ************
 *
 * BRD Driver for DAС service on ADM818X800
 *
 * (C) InSys by Dorokhin A. Jun 2007
 *
 ******************************************************
*/

#include "module.h"
#include "dac818x800srv.h"

#define	CURRFILE "DAC818X800SRV"

//***************************************************************************************
CDac818X800Srv::CDac818X800Srv(int idx, int srv_num, CModule* pModule, PDAC818X800SRV_CFG pCfg) :
	CDacSrv(idx, _BRDC("DAC818X800"), srv_num, pModule, pCfg, sizeof(DAC818X800SRV_CFG))
{
}

//***************************************************************************************
int CDac818X800Srv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
//	CDacSrv::SetChanMask(pModule, 0);
//	return BRDerr_OK;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CDac818X800Srv::GetDacTetrNum(CModule* pModule)
{
	m_DacTetrNum = GetTetrNum(pModule, m_index, DAC818X800_TETR_ID);
}
/*
// ***************************************************************************************
void CDac818X800Srv::FreeInfoForDialog(PVOID pInfo)
{
//	PDAC818X800SRV_INFO pSrvInfo = (PDAC818X800SRV_INFO)pInfo;
	PDACSRV_INFO pSrvInfo = (PDACSRV_INFO)pInfo;
	CDacSrv::FreeInfoForDialog(pSrvInfo->pDacInfo);
	delete pSrvInfo;
}
*/
//***************************************************************************************
PVOID CDac818X800Srv::GetInfoForDialog(CModule* pDev)
{
	PDAC818X800SRV_CFG pSrvCfg = (PDAC818X800SRV_CFG)m_pConfig;
//	PDACSRV_INFO pSrvInfo = new DACSRV_INFO;
//	pSrvInfo->Size = sizeof(DAC818X800SRV_INFO);
	UCHAR code = pSrvCfg->DacCfg.Encoding;
	PDACSRV_INFO pSrvInfo = (PDACSRV_INFO)CDacSrv::GetInfoForDialog(pDev);
	pSrvInfo->Encoding = code;
	//GetChanMask(pDev, pSrvInfo->ChanMask);
	//GetOutRange(pDev, pSrvInfo->Range[0], 0);

	GetClkSource(pDev, pSrvInfo->ClockSrc);
	GetClkValue(pDev, pSrvInfo->ClockSrc, pSrvInfo->ClockValue);
	GetRate(pDev, pSrvInfo->SamplingRate, pSrvInfo->ClockValue);

	return pSrvInfo;
}

//***************************************************************************************
int CDac818X800Srv::SetPropertyFromDialog(void	*pDev, void	*args)
{
	CModule* pModule = (CModule*)pDev;
	PDACSRV_INFO pInfo = (PDACSRV_INFO)args;
	CDacSrv::SetPropertyFromDialog(pDev, args);
	//SetChanMask(pModule, pInfo->ChanMask);
	//SetOutRange(pModule, pInfo->Range[0], 0);

	SetClkSource(pModule, pInfo->ClockSrc);
	SetClkValue(pModule, pInfo->ClockSrc, pInfo->ClockValue);
	SetRate(pModule, pInfo->SamplingRate, pInfo->ClockSrc, pInfo->ClockValue);
	SelfClbr(pModule);

	return BRDerr_OK;
}

//***************************************************************************************
int CDac818X800Srv::SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
    BRDCHAR Buffer[128];
	CDacSrv::SetProperties(pDev, iniFilePath, SectionName);

	//GetPrivateProfileString(SectionName, "ChannelMask", "1", Buffer, sizeof(Buffer), iniFilePath);
	//ULONG chanMask = atoi(Buffer);
	//SetChanMask(pDev, chanMask);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
    IPC_getPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("192000.0"), Buffer, sizeof(Buffer), iniFilePath);
    double rate = BRDC_atof(Buffer);
	SetRate(pDev, rate, clkSrc, clkValue);
	SelfClbr(pDev);

	return BRDerr_OK;
}

//***************************************************************************************
int CDac818X800Srv::SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
    BRDCHAR Buffer[128];
	CDacSrv::SaveProperties(pDev, iniFilePath, SectionName);
	PDAC818X800SRV_CFG pSrvCfg = (PDAC818X800SRV_CFG)m_pConfig;
    BRDC_sprintf(Buffer, _BRDC("%u"), pSrvCfg->DacCfg.Encoding);
    IPC_writePrivateProfileString(SectionName, _BRDC("CodeType"), Buffer, iniFilePath);

	//ULONG chanMask;
	//GetChanMask(pDev, chanMask);
	//sprintf_s(Buffer, "%u", chanMask);
	//WritePrivateProfileString(SectionName, "ChannelMask", Buffer, iniFilePath);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
	double rate;
	GetRate(pDev, rate, clkValue);
    BRDC_sprintf(Buffer, _BRDC("%.2f"), rate);
    IPC_writePrivateProfileString(SectionName, _BRDC("SamplingRate"), Buffer, iniFilePath);

	// the function flushes the cache
    IPC_writePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

//***************************************************************************************
int CDac818X800Srv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
//	ULONG DacDivideMode;
	PDAC818X800SRV_CFG pSrvCfg = (PDAC818X800SRV_CFG)m_pConfig;
	if(Rate < pSrvCfg->DacCfg.MinRate)
		Rate = pSrvCfg->DacCfg.MinRate;
	if(Rate > pSrvCfg->DacCfg.MaxRate)
		Rate = pSrvCfg->DacCfg.MaxRate;



	double ClkVal = ClkValue;
	ULONG DacRateDivider = ROUND(ClkVal / Rate);
	DacRateDivider = DacRateDivider ? DacRateDivider : 1;
	while(1)
	{
		Rate = ClkVal / DacRateDivider;
		if(Rate < pSrvCfg->DacCfg.MinRate)
			DacRateDivider--;
		else
			if(Rate > pSrvCfg->DacCfg.MaxRate)
				DacRateDivider++;
			else
				break;
	}

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = DACnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	PDAC_CTRL pCtrl = (PDAC_CTRL)&param.val;
//	pCtrl->ByBits.Lrck = DacDivideMode;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	ULONG status = CDacSrv::SetRate(pModule, Rate, ClkSrc, ClkVal);
	return status;
}

//***************************************************************************************
int CDac818X800Srv::GetRate(CModule* pModule, double& Rate, double ClkValue)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = DACnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDAC_CTRL pCtrl = (PDAC_CTRL)&param.val;
//	ULONG DacDivideMode = pCtrl->ByBits.Lrck;
//	ULONG ClkDivider = SINGLE_DIVIDER;// >> DacDivideMode;
	CDacSrv::GetRate(pModule, Rate, ClkValue);// / ClkDivider);
	return BRDerr_OK;
}

//***************************************************************************************
int CDac818X800Srv::SetCode(CModule* pModule, ULONG type)
{
	return BRDerr_OK;
}

//***************************************************************************************
int CDac818X800Srv::GetCode(CModule* pModule, ULONG& type)
{
	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}
/*
// ***************************************************************************************
int CDac818X800Srv::SyncPulse(CModule* pModule, double msPeriod)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = DACnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDAC_CTRL pCtrl = (PDAC_CTRL)&param.val;
	pCtrl->ByBits.Sync = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pCtrl->ByBits.Sync = 1;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//pCtrl->ByBits.Sync = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	ULONG time = ULONG(msPeriod * 62 + 0.5);
	Sleep(time);
	return BRDerr_OK;
}


// ***************************************************************************************
int CDac818X800Srv::SelfClbr(CModule* pModule)
{
	double ClockValue;
	ULONG ClockSrc;
	GetClkSource(pModule, ClockSrc);
	GetClkValue(pModule, ClockSrc, ClockValue);
	double rate;
	GetRate(pModule, rate, ClockValue);
	double msPeriod = 1000 / rate;
	SyncPulse(pModule, msPeriod);
	return BRDerr_OK;
}

// ***************************************************************************************
int CDac818X800Srv::SetMute(CModule* pModule, ULONG mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = DACnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDAC_CTRL pCtrl = (PDAC_CTRL)&param.val;
	pCtrl->ByBits.Mute = mode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

// ***************************************************************************************
int CDac818X800Srv::GetMute(CModule* pModule, ULONG& mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = DACnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDAC_CTRL pCtrl = (PDAC_CTRL)&param.val;
	mode = pCtrl->ByBits.Mute;
	return BRDerr_OK;
}
*/
// ***************************************************************************************
int CDac818X800Srv::SetSpecific(CModule* pModule, PBRD_DacSpec pSpec)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	return Status;
/*
	int Status = BRDerr_OK;
	CDacSrv::SetSpecific(pModule, pSpec);
	switch(pSpec->command)
	{
	case DAC818X800cmd_SETMUTE:
		{
			ULONG mode = *(PULONG)pSpec->arg;
			Status = SetMute(pModule, mode);
		}
		break;
	case DAC818X800cmd_GETMUTE:
		{
			ULONG mode;
			Status = GetMute(pModule, mode);
			*(PULONG)pSpec->arg = mode;
		}
		break;
	}
	return Status;
*/
}

// ***************** End of file Dac818X800Srv.cpp ***********************
