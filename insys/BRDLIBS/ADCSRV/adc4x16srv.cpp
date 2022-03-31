/*
 ***************** File Adc4x16Srv.cpp ************
 *
 * BRD Driver for ADС service on ADMDDC4x16
 *
 * (C) InSys by Dorokhin A. Jan 2006
 *
 ******************************************************
*/

#include "module.h"
#include "adc4x16srv.h"

#define	CURRFILE _BRDC("ADC4x16SRV")

const double BRD_ADC_MAXGAINTUN	= 0.; // max gain tuning = 0

//***************************************************************************************
CAdc4x16Srv::CAdc4x16Srv(int idx, int srv_num, CModule* pModule, PADC4X16SRV_CFG pCfg) :
	CAdcSrv(idx, _BRDC("ADC4X16"), srv_num, pModule, pCfg, sizeof(ADC4X16SRV_CFG))
{
	m_attribute |= (BRDserv_ATTR_UNVISIBLE | BRDserv_ATTR_SUBSERVICE_ONLY);
}

//***************************************************************************************
int CAdc4x16Srv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	CAdcSrv::SetChanMask(pModule, 0);
//	return BRDerr_OK;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CAdc4x16Srv::GetAdcTetrNum(CModule* pModule)
{
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC4x16_TETR_ID);

	m_DdcTetrNum = GetTetrNum(pModule, m_index, DDC4x16_TETR_ID);
	if(m_DdcTetrNum == -1)
		m_DdcTetrNum = GetTetrNum(pModule, m_index, DDC4x16S_TETR_ID);

}

//***************************************************************************************
void CAdc4x16Srv::FreeInfoForDialog(PVOID pInfo)
{
	PADC4X16SRV_INFO pSrvInfo = (PADC4X16SRV_INFO)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc4x16Srv::GetInfoForDialog(CModule* pDev)
{
	PADC4X16SRV_CFG pSrvCfg = (PADC4X16SRV_CFG)m_pConfig;
	PADC4X16SRV_INFO pSrvInfo = new ADC4X16SRV_INFO;
	pSrvInfo->Size = sizeof(ADC4X16SRV_INFO);
	pSrvInfo->pAdcInfo = (PADCSRV_INFO)CAdcSrv::GetInfoForDialog(pDev);
	pSrvInfo->SubRefGen = pSrvCfg->SubRefGen;
	pSrvInfo->SubExtClk = pSrvCfg->SubExtClk;
	GetClkSource(pDev, pSrvInfo->pAdcInfo->ClockSrc);
	GetClkValue(pDev, pSrvInfo->pAdcInfo->ClockSrc, pSrvInfo->pAdcInfo->ClockValue);
	GetRate(pDev, pSrvInfo->pAdcInfo->SamplingRate, pSrvInfo->pAdcInfo->ClockValue);
	if(pSrvInfo->pAdcInfo->ClockSrc == BRDclks_ADC_EXTCLK)
		pSrvInfo->SubExtClk = pSrvInfo->pAdcInfo->ClockValue;

	return pSrvInfo;
}

//***************************************************************************************
int CAdc4x16Srv::SetPropertyFromDialog(void	*pDev, void	*args)
{
	CModule* pModule = (CModule*)pDev;
	PADC4X16SRV_INFO pInfo = (PADC4X16SRV_INFO)args;

	CAdcSrv::SetChanMask(pModule, pInfo->pAdcInfo->ChanMask);
	ULONG master = pInfo->pAdcInfo->SyncMode;
	CtrlMaster(pDev, NULL, BRDctrl_ADC_SETMASTER, &master);
	ULONG format = pInfo->pAdcInfo->Format;
	CtrlFormat(pDev, NULL, BRDctrl_ADC_SETFORMAT, &format);
	ULONG code = pInfo->pAdcInfo->Encoding;
	CtrlCode(pDev, NULL, BRDctrl_ADC_SETCODE, &code);

	PADC4X16SRV_CFG pAdcSrvCfg = (PADC4X16SRV_CFG)m_pConfig;
	pAdcSrvCfg->SubExtClk = ROUND(pInfo->SubExtClk);
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc4x16Srv::SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	BRDCHAR Buffer[128];
	BRDCHAR* endptr;
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);

	IPC_getPrivateProfileString(SectionName, _BRDC("ExternalClockValue"), _BRDC("80000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	double clkValue = BRDC_atof(Buffer);
	SetClkValue(pDev, BRDclks_ADC_EXTCLK, clkValue);

	IPC_getPrivateProfileString(SectionName, _BRDC("ClockSource"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	ULONG clkSrc = BRDC_strtol(Buffer, &endptr, 0);
	if(clkSrc)
		clkSrc = BRDclks_ADC_EXTCLK;
//	ULONG clkSrc = atoi(Buffer);
	SetClkSource(pDev, clkSrc);
	if(clkSrc != BRDclks_ADC_EXTCLK)
		SetClkValue(pDev, clkSrc, clkValue);
	GetClkValue(pDev, clkSrc, clkValue);

	IPC_getPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("100000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	double rate = BRDC_atof(Buffer);
	SetRate(pDev, rate, clkSrc, clkValue);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc4x16Srv::SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	//BRDCHAR Buffer[128];
	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);
//	PADC28X800MSRV_CFG pSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
//	sprintf(Buffer, "%u", pSrvCfg->AdcCfg.Encoding);
//	WritePrivateProfileString(SectionName, "CodeType", Buffer, iniFilePath);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	//sprintf_s(Buffer, "%u", clkSrc);
	//WritePrivateProfileString(SectionName, "ClockSource", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ClockSource"), clkSrc, 10, NULL);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
	if(clkSrc == BRDclks_ADC_EXTCLK)
	{
		//sprintf_s(Buffer, "%.2f", clkValue);
		//WritePrivateProfileString(SectionName, "ExternalClockValue", Buffer, iniFilePath);
		WriteInifileParam(iniFilePath, SectionName, _BRDC("ExternalClockValue"), clkValue, 2, NULL);
	}
	double rate;
	GetRate(pDev, rate, clkValue);
	//sprintf_s(Buffer, "%.2f", rate);
	//WritePrivateProfileString(SectionName, "SamplingRate", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("SamplingRate"), clkValue, 2, NULL);

	// the function flushes the cache
	IPC_writePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc4x16Srv::SetClkSource(CModule* pModule, ULONG ClkSrc)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DdcTetrNum;
	param.reg = ADM2IFnr_FSRC;
	param.val = ClkSrc;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	Sleep(1);
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(!pMode0->ByBits.Master)
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.reg = ADM2IFnr_FMODE;//ADM2IFnr_FSRC;
			param.val = BRDclks_SMCLK;//ClkSrc;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		else
			return BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc4x16Srv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
{
	ULONG source;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_FSRC;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		source = param.val;
	}
	else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.tetr = m_DdcTetrNum;
			param.reg = ADM2IFnr_FSRC;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
			source = param.val;
		}
		else
		{ // Slave
			source = BRDclks_EXTSYNX;
		}
	}
	ClkSrc = source;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc4x16Srv::SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC4X16SRV_CFG pAdcSrvCfg = (PADC4X16SRV_CFG)m_pConfig;
	switch(ClkSrc)
	{
	case BRDclks_ADC_REFGEN:		// Submodule Reference Generator
		ClkValue = pAdcSrvCfg->SubRefGen;
		break;
	case BRDclks_EXTSYNX:			// External clock from SYNX
	case BRDclks_ADC_EXTCLK:		// External clock
		pAdcSrvCfg->SubExtClk = ROUND(ClkValue);
		break;
	default:
//		ClkValue = 0.0;
		break;
	}
	return BRDerr_OK;
}
	
//***************************************************************************************
int CAdc4x16Srv::GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC4X16SRV_CFG pAdcSrvCfg = (PADC4X16SRV_CFG)m_pConfig;
	double Clk;
	switch(ClkSrc)
	{
	case BRDclks_ADC_REFGEN:		// Submodule Reference Generator
		Clk = pAdcSrvCfg->SubRefGen;
		break;
	case BRDclks_EXTSYNX:			// External clock from SYNX
	case BRDclks_ADC_EXTCLK:		// External clock
		Clk = pAdcSrvCfg->SubExtClk;
		break;
	default:
		Clk = 0.0;
		break;
	}
	ClkValue = Clk;
	return BRDerr_OK;
}
	
//***************************************************************************************
int CAdc4x16Srv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
	Rate = ClkValue;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc4x16Srv::GetRate(CModule* pModule, double& Rate, double ClkValue)
{
	Rate = ClkValue;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc4x16Srv::SetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	return Status;
}

//***************************************************************************************
int CAdc4x16Srv::GetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	Bias = 0.;
	return Status;
}

//***************************************************************************************
int CAdc4x16Srv::SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	return Status;
}

//***************************************************************************************
int CAdc4x16Srv::GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	GainTuning = 0.;
	return Status;
}

//***************************************************************************************
int CAdc4x16Srv::SetDcCoupling(CModule* pModule, ULONG InpType, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	return Status;
}

//***************************************************************************************
int CAdc4x16Srv::GetDcCoupling(CModule* pModule, ULONG& InpType, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	InpType = 0;
	return Status;
}

//***************************************************************************************
int CAdc4x16Srv::SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	return Status;
}

//***************************************************************************************
int CAdc4x16Srv::GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	InpRes = 0;
	return Status;
}

//***************************************************************************************
int CAdc4x16Srv::ClrBitsOverflow(CModule* pModule, ULONG flags)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_FLAGCLR;
	param.val = ~flags << 9;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc4x16Srv::IsBitsOverflow(CModule* pModule, ULONG& OverBits)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;

	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	OverBits = (param.val & 0x1E00) >> 9;
	//OverBits = param.val;

	PADC4X16SRV_CFG pAdcSrvCfg = (PADC4X16SRV_CFG)m_pConfig;
	ULONG chan_mask = 0;
	for(int i = 0; i < pAdcSrvCfg->AdcCfg.MaxChan; i++)
		chan_mask += 1 << i;
	//GetChanMask(pModule, chan_mask);
	OverBits &= chan_mask;
	//param.reg = ADCnr_FLAGCLR;
	//param.val = 0;//0xFFFF;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc4x16Srv::FifoReset(CModule* pModule)
{
	CAdcSrv::FifoReset(pModule);
	ClrBitsOverflow(pModule, 0xf);
	return BRDerr_OK;
}

// ***************** End of file Adc4x16Srv.cpp ***********************
