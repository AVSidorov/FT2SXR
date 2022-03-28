/*
 ***************** File BaseDacSrv.cpp ***********************
 *
 * BRD Driver for BaseDAC service
 *
 * (C) InSys by Dorokhin A. Jul 2006
 *
 ******************************************************
*/

#include "module.h"
#include "basedacsrv.h"

#define	CURRFILE "BASEDACSRV"

//***************************************************************************************
CBaseDacSrv::CBaseDacSrv(int idx, int srv_num, CModule* pModule, PBASEDACSRV_CFG pCfg) :
	CDacSrv(idx, _BRDC("BASEDAC"), srv_num, pModule, pCfg, sizeof(BASEDACSRV_CFG))
{
}

//***************************************************************************************
int CBaseDacSrv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
//	CModule* pModule = (CModule*)pDev;
	CDacSrv::CtrlRelease(pDev, pServData, cmd, args);
//	CDacSrv::SetChanMask(pModule, 0);
//	return BRDerr_OK;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
ULONG CBaseDacSrv::GetParamForStream(CModule* pModule)
{
	PBASEDACSRV_CFG pSrvCfg = (PBASEDACSRV_CFG)m_pConfig;
	ULONG param = CDacSrv::GetParamForStream(pModule);
	ULONG mask;
	GetChanMask(pModule, mask);
	if(mask < 3 && pSrvCfg->DacCfg.FifoType)
		param |= 0x0100; // для 1-канального режима (и внешнего FIFO) устанавливаем ширину локальной шины 16 разрядов
	return param;
}

//***************************************************************************************
void CBaseDacSrv::GetDacTetrNum(CModule* pModule)
{
	m_DacTetrNum = GetTetrNum(pModule, m_index, BASEDAC_TETR_ID);
}

//***************************************************************************************
PVOID CBaseDacSrv::GetInfoForDialog(CModule* pDev)
{
	PBASEDACSRV_CFG pSrvCfg = (PBASEDACSRV_CFG)m_pConfig;
	UCHAR code = pSrvCfg->DacCfg.Encoding;
	PDACSRV_INFO pSrvInfo = (PDACSRV_INFO)CDacSrv::GetInfoForDialog(pDev);
	pSrvInfo->Encoding = code;
	GetChanMask(pDev, pSrvInfo->ChanMask);
	pSrvInfo->Range[0] = pSrvCfg->DacCfg.OutRange;
//	GetOutRange(pDev, pSrvInfo->Range[0], 0);

	return pSrvInfo;
}

//***************************************************************************************
int CBaseDacSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
	CModule* pModule = (CModule*)pDev;
	PDACSRV_INFO pInfo = (PDACSRV_INFO)args;
	CDacSrv::SetPropertyFromDialog(pDev, args);
	SetChanMask(pModule, pInfo->ChanMask);
//	SetOutRange(pModule, pInfo->Range[0], 0);

	SetClkSource(pModule, pInfo->ClockSrc);
	SetClkValue(pModule, pInfo->ClockSrc, pInfo->ClockValue);
	SetRate(pModule, pInfo->SamplingRate, pInfo->ClockSrc, pInfo->ClockValue);

	return BRDerr_OK;
}

//***************************************************************************************
int CBaseDacSrv::SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	BRDCHAR Buffer[128];
	BRDCHAR* endptr;
#if defined(__IPC_WIN__) || defined(__IPC_LINUX__)
    IPC_getPrivateProfileString(SectionName, _BRDC("ChannelMask"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
#else
	GetPrivateProfileString(SectionName, _BRDC("ChannelMask"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
#endif
	//ULONG chanMask = BRDC_atoi(Buffer);
	ULONG chanMask = BRDC_strtol(Buffer, &endptr, 0);//strtol
	SetChanMask(pDev, chanMask);

	CDacSrv::SetProperties(pDev, iniFilePath, SectionName);

	//GetPrivateProfileString(SectionName, "ClockSource", "1", Buffer, sizeof(Buffer), iniFilePath);
	//ULONG clkSrc = atoi(Buffer);
	//SetClkSource(pDev, clkSrc);
	//GetPrivateProfileString(SectionName, "BaseExternalClockValue", "100000.0", Buffer, sizeof(Buffer), iniFilePath);
	//double clkValue = atof(Buffer);
	//SetClkValue(pDev, clkSrc, clkValue);
	//GetPrivateProfileString(SectionName, "SamplingRate", "100000.0", Buffer, sizeof(Buffer), iniFilePath);
	//double rate = atof(Buffer);
	//SetRate(pDev, rate, clkSrc, clkValue);

	return BRDerr_OK;
}

//***************************************************************************************
int CBaseDacSrv::SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	//TCHAR Buffer[128];
	CDacSrv::SaveProperties(pDev, iniFilePath, SectionName);
	PBASEDACSRV_CFG pSrvCfg = (PBASEDACSRV_CFG)m_pConfig;
	//sprintf_s(Buffer, "%u", pSrvCfg->DacCfg.Encoding);
    //IPC_writePrivateProfileString(SectionName, "CodeType", Buffer, iniFilePath);
	ULONG encode = pSrvCfg->DacCfg.Encoding;
	WriteInifileParam(iniFilePath, SectionName, _BRDC("CodeType"), encode, 10, NULL);

	ULONG chanMask;
	GetChanMask(pDev, chanMask);
	//sprintf_s(Buffer, "%u", chanMask);
    //IPC_writePrivateProfileString(SectionName, "ChannelMask", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ChannelMask"), chanMask, 16, NULL);

	// the function flushes the cache
#if defined(__IPC_WIN__) || defined(__IPC_LINUX__)
	IPC_writePrivateProfileString(NULL, NULL, NULL, iniFilePath);
#else
	WritePrivateProfileString(NULL, NULL, NULL, iniFilePath);
#endif
	return BRDerr_OK;
}

//***************************************************************************************
int CBaseDacSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PBASEDAC_MODE1 pMode1 = (PBASEDAC_MODE1)&param.val;
	pMode1->ByBits.ChanSel = mask;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CBaseDacSrv::GetChanMask(CModule* pModule, ULONG& mask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PBASEDAC_MODE1 pMode1 = (PBASEDAC_MODE1)&param.val;
	mask = pMode1->ByBits.ChanSel;

	return BRDerr_OK;
}

//***************************************************************************************
int CBaseDacSrv::SetCode(CModule* pModule, ULONG type)
{
	return BRDerr_OK;
}

//***************************************************************************************
int CBaseDacSrv::GetCode(CModule* pModule, ULONG& type)
{
	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CBaseDacSrv::GetCfg(PVOID pCfg)
{
	PDACSRV_CFG pSrvCfg = (PDACSRV_CFG)m_pConfig;
	PBRD_BaseDacCfg pDacCfg = (PBRD_BaseDacCfg)pCfg;
	pDacCfg->Bits = pSrvCfg->Bits;
	pDacCfg->Encoding = pSrvCfg->Encoding;
	pDacCfg->FifoSize = pSrvCfg->FifoSize;
	pDacCfg->OutRange = pSrvCfg->OutRange;
	pDacCfg->MinRate = pSrvCfg->MinRate;
	pDacCfg->MaxRate = pSrvCfg->MaxRate;
	pDacCfg->MaxChan = pSrvCfg->MaxChan;
	pDacCfg->FifoType = pSrvCfg->FifoType;
	return BRDerr_OK;
}

//***************************************************************************************
//int CBaseDacSrv::GetCfg(PBRD_DacCfg pCfg)
//{
//	PBASEDACSRV_CFG pSrvCfg = (PBASEDACSRV_CFG)m_pConfig;
//	CDacSrv::GetCfg(pCfg);
//	pCfg->OutRange = pSrvCfg->AFRange;
//	return BRDerr_OK;
//}

// ***************** End of file BaseDacSrv.cpp ***********************
