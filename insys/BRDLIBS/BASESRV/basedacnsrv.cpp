/*
 ***************** File BasedacnSrv.cpp ************
 *
 * BRD Driver for new BaseDAC service
 *
 * (C) InSys by Sklyarov A., Dorokhin A. Mar 2011
 *
 ******************************************************
*/

#include "module.h"
#include "basedacnsrv.h"

#define	CURRFILE "BASEDACNSRV"

//***************************************************************************************
CBaseDacnSrv::CBaseDacnSrv(int idx, int srv_num, CModule* pModule, PBASEDACNSRV_CFG pCfg) :
	CDacSrv(idx, _BRDC("BASEDACN"), srv_num, pModule, pCfg, sizeof(BASEDACNSRV_CFG))
{
}

//***************************************************************************************
int CBaseDacnSrv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	CDacSrv::SetChanMask(pModule, 0);  // ?
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CBaseDacnSrv::GetDacTetrNum(CModule* pModule)
{
	m_DacTetrNum = GetTetrNum(pModule, m_index, BASEDACN_TETR_ID);
}
/*
// ***************************************************************************************
PVOID CBaseDacnSrv::GetInfoForDialog(CModule* pDev)
{
	PBASEDACNSRV_CFG pSrvCfg = (PBASEDACNSRV_CFG)m_pConfig;
	PDACSRV_INFO pSrvInfo = new DACSRV_INFO;
	UCHAR code = pSrvCfg->DacCfg.Encoding;
	pSrvInfo->pDacInfo = (PDACSRV_INFO)CDacSrv::GetInfoForDialog(pDev);
	pSrvInfo->pDacInfo->Encoding = code;

	//GetClkSource(pDev, pSrvInfo->pDacInfo->ClockSrc);
	//GetClkValue(pDev, pSrvInfo->pDacInfo->ClockSrc, pSrvInfo->pDacInfo->ClockValue);
	//GetRate(pDev, pSrvInfo->pDacInfo->SamplingRate, pSrvInfo->pDacInfo->ClockValue);

	return pSrvInfo;
}

// ***************************************************************************************
int CBaseDacnSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
	CModule* pModule = (CModule*)pDev;
	PDAC216X400MSRV_INFO pInfo = (PDAC216X400MSRV_INFO)args;
	CDacSrv::SetChanMask(pModule, pInfo->pDacInfo->ChanMask);
	ULONG master = pInfo->pDacInfo->SyncMode;
	CtrlMaster(pDev, NULL, BRDctrl_DAC_SETMASTER, &master);
//	ULONG format = pInfo->pDacInfo->Format;
//	CtrlFormat(pDev, NULL, BRDctrl_DAC_SETFORMAT, &format);
//	ULONG code = pInfo->pDacInfo->Encoding;
//	CtrlCode(pDev, NULL, BRDctrl_DAC_SETCODE, &code);
	BRD_StartMode start_mode;
	start_mode.startSrc = pInfo->pDacInfo->StartSrc;
	start_mode.startInv = pInfo->pDacInfo->StartInv;		
	start_mode.trigOn = pInfo->pDacInfo->StartStopMode;
	start_mode.trigStopSrc = pInfo->pDacInfo->StopSrc;		
	start_mode.stopInv = pInfo->pDacInfo->StopInv;
	CDacSrv::SetStartMode(pModule, &start_mode);

	BRD_EnVal st_delay;
	st_delay.enable	= pInfo->pDacInfo->Cnt0Enable;
	st_delay.value	= pInfo->pDacInfo->StartDelayCnt;
	CtrlStDelay(pDev, NULL, BRDctrl_DAC_SETSTDELAY, &st_delay);
	BRD_EnVal out_data;
	out_data.enable	= pInfo->pDacInfo->Cnt1Enable;
	out_data.value	= pInfo->pDacInfo->OutCnt;
	CtrlOutData(pDev, NULL, BRDctrl_DAC_SETOUTDATA, &out_data);
	BRD_EnVal skip_data;
	skip_data.enable = pInfo->pDacInfo->Cnt2Enable;
	skip_data.value	 = pInfo->pDacInfo->SkipCnt;
	CtrlSkipData(pDev, NULL, BRDctrl_DAC_SETSKIPDATA, &skip_data);

	//CDacSrv::SetPropertyFromDialog(pDev, args);
	PDAC216X400MSRV_CFG pSrvCfg = (PDAC216X400MSRV_CFG)m_pConfig;

	SetClkSource(pModule, pInfo->pDacInfo->ClockSrc);
	SetClkValue(pModule, pInfo->pDacInfo->ClockSrc, pInfo->pDacInfo->ClockValue);
	SetRate(pModule, pInfo->pDacInfo->SamplingRate, pInfo->pDacInfo->ClockSrc, pInfo->pDacInfo->ClockValue);

	return BRDerr_OK;
}

// ***************************************************************************************
int CBaseDacnSrv::SetProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
//	TCHAR Buffer[128];
//	TCHAR* endptr;
	CDacSrv::SetProperties(pDev, iniFilePath, SectionName);

	PDAC216X400MSRV_CFG pSrvCfg = (PDAC216X400MSRV_CFG)m_pConfig;

	GetPrivateProfileString(SectionName, "ClockSource", "2", Buffer, sizeof(Buffer), iniFilePath);
	ULONG clkSrc = strtol(Buffer, &endptr, 0);
	SetClkSource(pDev, clkSrc);
	GetPrivateProfileString(SectionName, "ExternalClockValue", "120000000.0", Buffer, sizeof(Buffer), iniFilePath);
	double clkValue = atof(Buffer);
	pSrvCfg->SubExtClk = ROUND(clkValue);
	SetClkValue(pDev, clkSrc, clkValue);
	GetPrivateProfileString(SectionName, "SamplingRate", "100000000.0", Buffer, sizeof(Buffer), iniFilePath);
	double rate = atof(Buffer);
	SetRate(pDev, rate, clkSrc, clkValue);

	return BRDerr_OK;
}

// ***************************************************************************************
int CBaseDacnSrv::SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	CDacSrv::SaveProperties(pDev, iniFilePath, SectionName);
	PDAC216X400MSRV_CFG pSrvCfg = (PDAC216X400MSRV_CFG)m_pConfig;
	sprintf_s(Buffer, "%u", pSrvCfg->DacCfg.Encoding);
	WritePrivateProfileString(SectionName, "CodeType", Buffer, iniFilePath);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	sprintf_s(Buffer, "%u", clkSrc);
	WritePrivateProfileString(SectionName, "ClockSource", Buffer, iniFilePath);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
	if(clkSrc == BRDclks_DAC_EXTCLK || clkSrc == BRDclks_DAC_EXTREFPLL)
	{
		sprintf_s(Buffer, "%.2f", clkValue);
		WritePrivateProfileString(SectionName, "ExternalClockValue", Buffer, iniFilePath);
	}
	double rate;
	GetRate(pDev, rate, clkValue);
	sprintf_s(Buffer, "%.2f", rate);
	WritePrivateProfileString(SectionName, "SamplingRate", Buffer, iniFilePath);
	

	// the function flushes the cache
	WritePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}
*/
//***************************************************************************************
int CBaseDacnSrv::SetCode(CModule* pModule, ULONG type)
{
	return BRDerr_OK;
}

//***************************************************************************************
int CBaseDacnSrv::GetCode(CModule* pModule, ULONG& type)
{
	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CBaseDacnSrv::SetDacReg(CModule* pModule, int regNum, int regVal)
{
//	int Status = BRDerr_CMD_UNSUPPORTED;
	int Status = BRDerr_OK;

//	PBASEDACNSRV_CFG pDacSrvCfg = (PBASEDACNSRV_CFG)m_pConfig;
//	pDacSrvCfg->DacReg[regNum] = regVal;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = DACnr_REGADDR;
	param.val = regNum;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = DACnr_REGDATA;
	param.val = regVal;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

//***************************************************************************************
int CBaseDacnSrv::SetSpecific(CModule* pModule, PBRD_DacSpec pSpec)
{
//	int Status = BRDerr_CMD_UNSUPPORTED;
	int Status = BRDerr_OK;
	CDacSrv::SetSpecific(pModule, pSpec);
	switch(pSpec->command)
	{
	case BASEDACNcmd_SETINTREG:
		{
			PBRD_DacIntReg pDacReg = (PBRD_DacIntReg)pSpec->arg;
			SetDacReg(pModule, pDacReg->addr, pDacReg->data);
		}
		break;
/*
case DAC216X400Mcmd_GETINTREG:
		{
			PBRD_DacIntReg pDacReg = (PBRD_DacIntReg)pSpec->arg;
			PDAC216X400MSRV_CFG pDacSrvCfg = (PDAC216X400MSRV_CFG)m_pConfig;
			//pDacReg->data = pDacSrvCfg->DacReg[pDacReg->addr];
		}
		break;
*/
	}
	return Status;
}
// ***************** End of file BasedacnSrv.cpp ***********************
