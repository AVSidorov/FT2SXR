/*
 ***************** File Adc10x2gSrv.cpp ************
 *
 * BRD Driver for ADС service on ADS10x2G
 *
 * (C) InSys by Dorokhin A. Jun 2005
 *
 *	14.03.2006 - add changies by ver. 2.0
 *  24.05.2006 - Add for SYNX signal (SetStartMode)
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "adc10x2gsrv.h"

#define	CURRFILE "ADC10X2GSRV"

const double BRD_ADC_MAXGAINTUN	= 10.; // max gain tuning = 10%
const double BRD_ADC_PRECBIAS	= 0.1; // precision bias = 10%

//***************************************************************************************
CAdc10x2gSrv::CAdc10x2gSrv(int idx, int srv_num, CModule* pModule, PADC10X2GSRV_CFG pCfg) :
	CAdcSrv(idx, _T("ADC10X2G"), srv_num, pModule, pCfg, sizeof(ADC10X2GSRV_CFG))
{
	m_TetrModif = 1;
}

//***************************************************************************************
int CAdc10x2gSrv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	CAdcSrv::SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);
//	return BRDerr_OK;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CAdc10x2gSrv::GetAdcTetrNum(CModule* pModule)
{
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC10X2G_TETR_ID);
	if(m_AdcTetrNum != -1)
	{
	}
}

//***************************************************************************************
void CAdc10x2gSrv::FreeInfoForDialog(PVOID pInfo)
{
	PADC10X2GSRV_INFO pSrvInfo = (PADC10X2GSRV_INFO)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc10x2gSrv::GetInfoForDialog(CModule* pDev)
{
	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	PADC10X2GSRV_INFO pSrvInfo = new ADC10X2GSRV_INFO;
	pSrvInfo->Size = sizeof(ADC10X2GSRV_INFO);
	pSrvInfo->pAdcInfo = (PADCSRV_INFO)CAdcSrv::GetInfoForDialog(pDev);
	if(pSrvInfo->pAdcInfo->SyncMode == 3)
		pSrvInfo->pAdcInfo->SyncMode = 1;
	pSrvInfo->ExtClk = pSrvCfg->SubExtClk;
	GetClkSource(pDev, pSrvInfo->pAdcInfo->ClockSrc);
	GetClkValue(pDev, pSrvInfo->pAdcInfo->ClockSrc, pSrvInfo->pAdcInfo->ClockValue);
	GetRate(pDev, pSrvInfo->pAdcInfo->SamplingRate, pSrvInfo->pAdcInfo->ClockValue);
	if(pSrvInfo->pAdcInfo->ClockSrc == BRDclks_ADC_EXTCLK || 
		pSrvInfo->pAdcInfo->ClockSrc == BRDclks_ADC_MASTERCLK)
		pSrvInfo->ExtClk = pSrvInfo->pAdcInfo->ClockValue;
	ULONG test_mode;
	GetTestMode(pDev, test_mode);
	pSrvInfo->TestMode = (UCHAR)(test_mode >> 16);
	GetInpRange(pDev, pSrvInfo->pAdcInfo->Range[0], 0);
	GetBias(pDev, pSrvInfo->pAdcInfo->Bias[0], 0);
//	GetInpResist(pDev, pSrvInfo->pAdcInfo->Resist[i], 0);
//	GetDcCoupling(pDev, pSrvInfo->pAdcInfo->DcCoupling[i], 0);
	GetClkPhase(pDev, pSrvInfo->PhaseTuning, 0);
	GetGainTuning(pDev, pSrvInfo->GainTuning, 0);

	BRD_AdcStartMode start_mode;
	GetStartMode(pDev, &start_mode);
	pSrvInfo->StartSrc = start_mode.src;
	pSrvInfo->InvStart = start_mode.inv;
	pSrvInfo->StartLevel = start_mode.level;
	pSrvInfo->pAdcInfo->StartSrc = start_mode.stndStart.startSrc;
	pSrvInfo->pAdcInfo->StartInv = start_mode.stndStart.startInv;
	pSrvInfo->pAdcInfo->StartStopMode = start_mode.stndStart.trigOn;
	pSrvInfo->pAdcInfo->StopSrc	= start_mode.stndStart.trigStopSrc;
	pSrvInfo->pAdcInfo->StopInv	= start_mode.stndStart.stopInv;
	pSrvInfo->pAdcInfo->ReStart	= start_mode.stndStart.reStartMode;

	pSrvInfo->Version = 0x10;
	pSrvInfo->PllRefGen = 0;
	pSrvInfo->PllFreq = 0;
//	pSrvInfo->FracMode = 1;
//	pSrvInfo->FracMode = pSrvCfg->FracMode;
	if(m_TetrModif == 2)
	{
		pSrvInfo->Version = 0x20;
		pSrvInfo->PllRefGen = pSrvCfg->PllRefGen;
		pSrvInfo->PllFreq = pSrvCfg->PllFreq;
		pSrvInfo->ExtPllRef = pSrvCfg->ExtPllRef;
	}

	ULONG StartSl;
	ULONG ClkSl;
	ULONG TimerSync;
	GetSpecParam(pDev, StartSl, ClkSl, TimerSync);
	pSrvInfo->StartSl = (UCHAR)StartSl;
	pSrvInfo->ClkSl = (UCHAR)ClkSl;
	pSrvInfo->TimerSync = (UCHAR)TimerSync;


	return pSrvInfo;
}

//***************************************************************************************
int CAdc10x2gSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
	CModule* pModule = (CModule*)pDev;
	PADC10X2GSRV_INFO pInfo = (PADC10X2GSRV_INFO)args;
//	ULONG ClockSrc = pInfo->pAdcInfo->ClockSrc;
//	double ClockValue = pInfo->pAdcInfo->ClockValue;
//	double SamplingRate = pInfo->pAdcInfo->SamplingRate;
//	CAdcSrv::SetPropertyFromDialog(pDev, pInfo->pAdcInfo);
//	pInfo->pAdcInfo->ClockSrc = ClockSrc;
//	pInfo->pAdcInfo->ClockValue = ClockValue;
//	pInfo->pAdcInfo->SamplingRate = SamplingRate;
	CAdcSrv::SetChanMask(pModule, pInfo->pAdcInfo->ChanMask);
	ULONG master = pInfo->pAdcInfo->SyncMode;
	SetMaster(pModule, master);
//	CtrlMaster(pDev, NULL, BRDctrl_ADC_SETMASTER, &master);
	ULONG format = pInfo->pAdcInfo->Format;
	CtrlFormat(pDev, NULL, BRDctrl_ADC_SETFORMAT, &format);
	ULONG code = pInfo->pAdcInfo->Encoding;
	CtrlCode(pDev, NULL, BRDctrl_ADC_SETCODE, &code);

	BRD_EnVal st_delay;
	st_delay.enable	= pInfo->pAdcInfo->Cnt0Enable;
	st_delay.value	= pInfo->pAdcInfo->StartDelayCnt;
	SetCnt(pModule, 0, &st_delay);
	pInfo->pAdcInfo->StartDelayCnt = st_delay.value;
	BRD_EnVal acq_data;
	acq_data.enable	= pInfo->pAdcInfo->Cnt1Enable;
	acq_data.value	= pInfo->pAdcInfo->DaqCnt;
	SetCnt(pModule, 1, &acq_data);
	pInfo->pAdcInfo->DaqCnt = acq_data.value;
	BRD_EnVal skip_data;
	skip_data.enable = pInfo->pAdcInfo->Cnt2Enable;
	skip_data.value	 = pInfo->pAdcInfo->SkipCnt;
	SetCnt(pModule, 2, &skip_data);
	pInfo->pAdcInfo->SkipCnt = skip_data.value;

	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	pAdcSrvCfg->SubExtClk = ROUND(pInfo->ExtClk);
	if(m_TetrModif == 2)
		pAdcSrvCfg->ExtPllRef = pInfo->ExtPllRef;
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	ULONG test_mode = (ULONG)pInfo->TestMode << 16;
	SetTestMode(pModule, test_mode);

	SetInpRange(pModule, pInfo->pAdcInfo->Range[0], 0);
	SetBias(pModule, pInfo->pAdcInfo->Bias[0], 0);
	SetInpResist(pModule, pInfo->pAdcInfo->Resist[0], 0);
	SetDcCoupling(pModule, pInfo->pAdcInfo->DcCoupling[0], 0);
	SetClkPhase(pModule, pInfo->PhaseTuning, 0);
	SetGainTuning(pModule, pInfo->GainTuning, 0);

	BRD_AdcStartMode start_mode;
//	GetStartMode(pModule, &start_mode);
	start_mode.src					= pInfo->StartSrc;
	start_mode.inv					= pInfo->InvStart;
	start_mode.level				= pInfo->StartLevel;
	start_mode.stndStart.startSrc	= pInfo->pAdcInfo->StartSrc;
	start_mode.stndStart.startInv	= pInfo->pAdcInfo->StartInv;
	start_mode.stndStart.trigOn		= pInfo->pAdcInfo->StartStopMode;
	start_mode.stndStart.trigStopSrc= pInfo->pAdcInfo->StopSrc;
	start_mode.stndStart.stopInv	= pInfo->pAdcInfo->StopInv;
	start_mode.stndStart.reStartMode = pInfo->pAdcInfo->ReStart;
	SetStartMode(pModule, &start_mode);
	pInfo->StartLevel = start_mode.level;

	SetSpecParam(pModule, pInfo->StartSl, pInfo->ClkSl, pInfo->TimerSync);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);
//	ULONG master;
//	GetMaster(pDev, master);
//	SetMaster(pDev, master);

	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	GetPrivateProfileString(SectionName, "PllFracMode", "0", Buffer, sizeof(Buffer), iniFilePath);
	pAdcSrvCfg->FracMode = atoi(Buffer);

	GetPrivateProfileString(SectionName, "PllOutCurrent", "8", Buffer, sizeof(Buffer), iniFilePath);
	pAdcSrvCfg->OutCurrent = atoi(Buffer);

	GetPrivateProfileString(SectionName, "PllFold", "2", Buffer, sizeof(Buffer), iniFilePath);
	int fold = atoi(Buffer);
	SetFold(pDev, fold);

	GetPrivateProfileString(SectionName, "PllMuxOut", "2", Buffer, sizeof(Buffer), iniFilePath);
	pAdcSrvCfg->PllMuxOut = atoi(Buffer);
	SetMuxOut(pDev, pAdcSrvCfg->PllMuxOut);

	GetPrivateProfileString(SectionName, "ExternalPllReference", "50000000.0", Buffer, sizeof(Buffer), iniFilePath);
	BRD_Adc10x2gPll pllMode;
	pllMode.ExtRef = atof(Buffer);
	SetPllMode(pDev, &pllMode);

	GetPrivateProfileString(SectionName, "ClockSource", "129", Buffer, sizeof(Buffer), iniFilePath);
	ULONG clkSrc = atoi(Buffer);
	SetClkSource(pDev, clkSrc);
	GetPrivateProfileString(SectionName, "ExternalClockValue", "2000000000.0", Buffer, sizeof(Buffer), iniFilePath);
	double clkValue = atof(Buffer);
	pAdcSrvCfg->SubExtClk = ROUND(clkValue);
	SetClkValue(pDev, clkSrc, clkValue);
	GetPrivateProfileString(SectionName, "SamplingRate", "1000000000.0", Buffer, sizeof(Buffer), iniFilePath);
	double rate = atof(Buffer);
	SetRate(pDev, rate, clkSrc, clkValue);

	GetPrivateProfileString(SectionName, "TestMode", "0", Buffer, sizeof(Buffer), iniFilePath);
	ULONG test_mode = atoi(Buffer);
	SetTestMode(pDev, test_mode);

	GetPrivateProfileString(SectionName, "InputRange", "0.05", Buffer, sizeof(Buffer), iniFilePath);
	double InpRange = atof(Buffer);
	SetInpRange(pDev, InpRange, 0);
	GetPrivateProfileString(SectionName, "Bias", "0.0", Buffer, sizeof(Buffer), iniFilePath);
	double Bias = atof(Buffer);
	SetBias(pDev, Bias, 0);
	GetPrivateProfileString(SectionName, "GainTuning", "0.0", Buffer, sizeof(Buffer), iniFilePath);
	double GainTuning = atof(Buffer);
	SetGainTuning(pDev, GainTuning, 0);
	GetPrivateProfileString(SectionName, "ClockPhaseTuning", "0.0", Buffer, sizeof(Buffer), iniFilePath);
	double PhaseTuning = atof(Buffer);
	SetClkPhase(pDev, PhaseTuning, 0);

	BRD_AdcStartMode start;
	GetPrivateProfileString(SectionName, "StartSource", "3", Buffer, sizeof(Buffer), iniFilePath);
	start.src = atoi(Buffer);
	GetPrivateProfileString(SectionName, "StartInverting", "0", Buffer, sizeof(Buffer), iniFilePath);
	start.inv = atoi(Buffer);//0;
	GetPrivateProfileString(SectionName, "StartLevel", "0.0", Buffer, sizeof(Buffer), iniFilePath);
	start.level = atof(Buffer);//0.0;
	if(start.src != BRDsts_ADC_PRG)
		start.stndStart.startSrc = BRDsts_SUBMOD;
	else
		start.stndStart.startSrc = BRDsts_PRG;
	start.stndStart.startInv = 0;
	start.stndStart.trigOn = 1;
	start.stndStart.trigStopSrc = BRDsts_PRG;
	start.stndStart.stopInv = 0;
	GetPrivateProfileString(SectionName, "ReStart", "0", Buffer, sizeof(Buffer), iniFilePath);
	start.stndStart.reStartMode = atoi(Buffer);
	SetStartMode(pDev, &start);

	GetPrivateProfileString(SectionName, "BlockTitle", "0", Buffer, sizeof(Buffer), iniFilePath);
	ULONG TitleOn = atoi(Buffer);

	BRD_AdcSpec spec;
	spec.command = ADC10X2Gcmd_SETTITLEMODE;
	spec.arg = &TitleOn;
	SetSpecific(pDev, &spec);

	GetPrivateProfileString(SectionName, "StartSl", "0", Buffer, sizeof(Buffer), iniFilePath);
	ULONG StartSl = atoi(Buffer);
	GetPrivateProfileString(SectionName, "ClkSl", "0", Buffer, sizeof(Buffer), iniFilePath);
	ULONG ClkSl = atoi(Buffer);
	GetPrivateProfileString(SectionName, "TimerSync", "0", Buffer, sizeof(Buffer), iniFilePath);
	ULONG TimerSync = atoi(Buffer);
	SetSpecParam(pDev, StartSl, ClkSl, TimerSync);

	GetPrivateProfileString(SectionName, "AccEn", "0", Buffer, sizeof(Buffer), iniFilePath);
	ULONG AccEn = atoi(Buffer);
	GetPrivateProfileString(SectionName, "AccCnt", "0", Buffer, sizeof(Buffer), iniFilePath);
	ULONG AccCnt = atoi(Buffer);
	SetAccumulator(pDev, AccEn, AccCnt);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);

	ULONG master;
	GetMaster(pDev, master);
	if(master == 3) master = 1;
	sprintf_s(Buffer, "%u", master);
	WritePrivateProfileString(SectionName, "MasterMode", Buffer, iniFilePath);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	sprintf_s(Buffer, "%u", clkSrc);
	WritePrivateProfileString(SectionName, "ClockSource", Buffer, iniFilePath);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
	if(clkSrc == BRDclks_ADC_EXTCLK || clkSrc == BRDclks_ADC_MASTERCLK || clkSrc == BRDclks_ADC_DISABLED)
	{
		sprintf_s(Buffer, "%.2f", clkValue);
		WritePrivateProfileString(SectionName, "ExternalClockValue", Buffer, iniFilePath);
	}
	double rate;
	GetRate(pDev, rate, clkValue);
	sprintf_s(Buffer, "%.2f", rate);
	WritePrivateProfileString(SectionName, "SamplingRate", Buffer, iniFilePath);

	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	sprintf_s(Buffer, "%u", pAdcSrvCfg->ExtPllRef);
	WritePrivateProfileString(SectionName, "ExternalPllReference", Buffer, iniFilePath);
//	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
//	sprintf(Buffer, "%u", pAdcSrvCfg->FracMode);
//	WritePrivateProfileString(SectionName, "PllFracMode", Buffer, iniFilePath);

	ULONG test_mode;
	GetTestMode(pDev, test_mode);
	sprintf_s(Buffer, "%u", test_mode);
	WritePrivateProfileString(SectionName, "TestMode", Buffer, iniFilePath);

	double InpRange, Bias, GainTuning, PhaseTuning;
	GetInpRange(pDev, InpRange, 0);
	sprintf_s(Buffer, "%.2f", InpRange);
	WritePrivateProfileString(SectionName, "InputRange", Buffer, iniFilePath);
	GetBias(pDev, Bias, 0);
	sprintf_s(Buffer, "%.4f", Bias);
	WritePrivateProfileString(SectionName, "Bias", Buffer, iniFilePath);
	GetGainTuning(pDev, GainTuning, 0);
	sprintf_s(Buffer, "%.4f", GainTuning);
	WritePrivateProfileString(SectionName, "GainTuning", Buffer, iniFilePath);
	GetClkPhase(pDev, PhaseTuning, 0);
	sprintf_s(Buffer, "%.4f", PhaseTuning);
	WritePrivateProfileString(SectionName, "ClockPhaseTuning", Buffer, iniFilePath);

	BRD_AdcStartMode start;
	GetStartMode(pDev, &start);
	sprintf_s(Buffer, "%u", start.src);
	WritePrivateProfileString(SectionName, "StartSource", Buffer, iniFilePath);
	sprintf_s(Buffer, "%u", start.inv);
	WritePrivateProfileString(SectionName, "StartInverting", Buffer, iniFilePath);
	sprintf_s(Buffer, "%.2f", start.level);
	WritePrivateProfileString(SectionName, "StartLevel", Buffer, iniFilePath);
	sprintf_s(Buffer, "%u", start.stndStart.startSrc);
	WritePrivateProfileString(SectionName, "StartBaseSource", Buffer, iniFilePath);
	sprintf_s(Buffer, "%u", start.stndStart.startInv);
	WritePrivateProfileString(SectionName, "StartBaseInverting", Buffer, iniFilePath);
	sprintf_s(Buffer, "%u", start.stndStart.trigOn);
	WritePrivateProfileString(SectionName, "StartMode", Buffer, iniFilePath);
	sprintf_s(Buffer, "%u", start.stndStart.trigStopSrc);
	WritePrivateProfileString(SectionName, "StopSource", Buffer, iniFilePath);
	sprintf_s(Buffer, "%u", start.stndStart.stopInv);
	WritePrivateProfileString(SectionName, "StopInverting", Buffer, iniFilePath);
	sprintf_s(Buffer, "%u", start.stndStart.reStartMode);
	WritePrivateProfileString(SectionName, "ReStart", Buffer, iniFilePath);

	ULONG TitleOn;
	BRD_AdcSpec spec;
	spec.command = ADC10X2Gcmd_GETTITLEMODE;
	spec.arg = &TitleOn;
	SetSpecific(pDev, &spec);
	sprintf_s(Buffer, "%u", TitleOn);
	WritePrivateProfileString(SectionName, "BlockTitle", Buffer, iniFilePath);

	ULONG StartSl;
	ULONG ClkSl;
	ULONG TimerSync;
	GetSpecParam(pDev, StartSl, ClkSl, TimerSync);
	sprintf_s(Buffer, "%d", StartSl);
	WritePrivateProfileString(SectionName, "StartSl", Buffer, iniFilePath);
	sprintf_s(Buffer, "%d", ClkSl);
	WritePrivateProfileString(SectionName, "ClkSl", Buffer, iniFilePath);
	sprintf_s(Buffer, "%d", TimerSync);
	WritePrivateProfileString(SectionName, "TimerSync", Buffer, iniFilePath);

	// the function flushes the cache
	WritePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

////***************************************************************************************
//int CAdc10x2gSrv::SetClkMode(CModule* pModule, PVOID pMode)
//{
//	int Status = BRDerr_CMD_UNSUPPORTED;
//	PBRD_PllClkMode pAdcClk = (PBRD_PllClkMode)pMode;
//	Status = SetClkSource(pModule, pAdcClk->src);
//	Status = SetClkValue(pModule, pAdcClk->src, pAdcClk->value, pAdcClk->pllRef);
//	return Status;
//}
//
////***************************************************************************************
//int CAdc10x2gSrv::GetClkMode(CModule* pModule, PVOID pMode)
//{
//	int Status = BRDerr_CMD_UNSUPPORTED;
//	PBRD_PllClkMode pAdcClk = (PBRD_PllClkMode)pMode;
//	ULONG src = pAdcClk->src;
//	Status = GetClkSource(pModule, src);
//	pAdcClk->src = src;
//	Status = GetClkValue(pModule, pAdcClk->src, pAdcClk->value);
//	return Status;
//}
//
////***************************************************************************************
//int CAdc10x2gSrv::SetSyncMode(CModule* pModule, PVOID pMode)
//{
//	int Status = BRDerr_CMD_UNSUPPORTED;
//	PBRD_PllSyncMode pSyncMode = (PBRD_PllSyncMode)pMode;
//	Status = SetClkSource(pModule, pSyncMode->clkSrc);
//	Status = SetClkValue(pModule, pSyncMode->clkSrc, pSyncMode->clkValue, pSyncMode->pllRef);
//	Status = SetRate(pModule, pSyncMode->rate, pSyncMode->clkSrc, pSyncMode->clkValue);
//	return Status;
//}
//
////***************************************************************************************
//int CAdc10x2gSrv::GetSyncMode(CModule* pModule, PVOID pMode)
//{
//	int Status = BRDerr_CMD_UNSUPPORTED;
//	PBRD_PllSyncMode pSyncMode = (PBRD_PllSyncMode)pMode;
//	ULONG src = pSyncMode->clkSrc;
//	Status = GetClkSource(pModule, src);
//	pSyncMode->clkSrc = src;
//	Status = GetClkValue(pModule, pSyncMode->clkSrc, pSyncMode->clkValue);
//	Status = GetRate(pModule, pSyncMode->rate, pSyncMode->clkValue);
//	return Status;
//}

//***************************************************************************************
int CAdc10x2gSrv::SetClkSource(CModule* pModule, ULONG ClkSrc)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
//	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_FSRC;
		param.val = ClkSrc;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	//else
	//{ // Master/Slave
	//	param.tetr = m_MainTetrNum;
	//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//	if(pMode0->ByBits.Master)
	//	{ // Master
	//		param.reg = ADM2IFnr_FSRC;
	//		param.val = ClkSrc;
	//		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//	}
	//	else
	//		return BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
	//}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
{
	ULONG source;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
//	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_FSRC;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		source = param.val;
	}
	//else
	//{ // Master/Slave
	//	param.tetr = m_MainTetrNum;
	//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//	if(pMode0->ByBits.Master)
	//	{ // Master
	//		param.reg = ADM2IFnr_FSRC;
	//		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//		source = param.val;
	//	}
	//	else
	//	{ // Slave
	//		source = BRDclks_EXTSYNX;
	//	}
	//}
	ClkSrc = source;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:			// disabled clock
		//ClkValue = 0.0;
		OffPLL(pModule);
		break;
	case BRDclks_ADC_INTREFPLL:			// SubModule Clock (internal reference) - ver. 2.0 only !!!
		ClkValue = pAdcSrvCfg->PllFreq;
		OnPll(pModule, ClkValue, pAdcSrvCfg->PllRefGen);
		break;
	case BRDclks_ADC_EXTREFPLL:			// SubModule Clock (external reference - EXT REF IN) - ver. 2.0 only !!!
		{
		ClkValue = pAdcSrvCfg->PllFreq;
		OnPll(pModule, ClkValue, pAdcSrvCfg->ExtPllRef);
		}
		break;
	case BRDclks_ADC_EXTCLK:			// External clock
	case BRDclks_ADC_MASTERCLK:
		pAdcSrvCfg->SubExtClk = ROUND(ClkValue);
		OffPLL(pModule);
		break;
	default:
//		ClkValue = 0.0;
		break;
	}
	return BRDerr_OK;
}
	
//***************************************************************************************
int CAdc10x2gSrv::GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	double Clk;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:			// disabled clock
		Clk = pAdcSrvCfg->SubExtClk;
		//Clk = 0.0;
		break;
	case BRDclks_ADC_INTREFPLL:			// SubModule Clock (internal reference) - ver. 2.0 only !!!
	case BRDclks_ADC_EXTREFPLL:			// SubModule Clock (external reference - EXT REF IN) - ver. 2.0 only !!!
		Clk = pAdcSrvCfg->PllFreq;
		break;
	case BRDclks_ADC_EXTCLK:			// External clock
	case BRDclks_ADC_MASTERCLK:
		Clk = pAdcSrvCfg->SubExtClk;
		break;
	default:
		Clk = 0.0;
		break;
	}
	ClkValue = Clk;
	return BRDerr_OK;
}
	
static double DeltaFreq;
int ADC10x2G_CLK_DIVIDER[BRD_CLKDIVCNT] = {1, 2, 4, 8, 16};

//****************************************************************************************
int SetClkDivOpt(double RateHz, double curClk, double minRate)
{
	if(!curClk)
		return 1;
	int MaxDivider = (int)floor(curClk / minRate);
	int i = 0;
	for(i = BRD_CLKDIVCNT-1; i >= 0; i--)
		if(MaxDivider >= ADC10x2G_CLK_DIVIDER[i])
			break;
	int MaxInd = (i < 0) ? 0 : i;
	MaxDivider = ADC10x2G_CLK_DIVIDER[MaxInd];
	if(RateHz >= curClk)
	{
		if(DeltaFreq > RateHz - curClk)
		{
			DeltaFreq = RateHz - curClk;
			return 1;
		}
	}
	else
	{
		double minRateHz = curClk / MaxDivider;
		if(RateHz < minRateHz)
		{
			if(DeltaFreq > minRateHz - RateHz)
			{
				DeltaFreq = minRateHz - RateHz;
				return ADC10x2G_CLK_DIVIDER[MaxInd];
			}
		}
		else 
		{
			for(int i = 0; i < MaxInd; i++) 
			{
				double RateLeft = curClk / ADC10x2G_CLK_DIVIDER[i];
				double RateRight = curClk / ADC10x2G_CLK_DIVIDER[i+1];
				if(RateHz <= RateLeft && RateHz >= RateRight)
				{
					if(RateLeft - RateHz <= RateHz - RateRight)
					{
						if (DeltaFreq > RateLeft - RateHz)
						{
							DeltaFreq = RateLeft - RateHz;
							return ADC10x2G_CLK_DIVIDER[i];
						}
					}
					else
					{
						if (DeltaFreq > RateHz - RateRight)
						{
							DeltaFreq = RateHz - RateRight;
							return ADC10x2G_CLK_DIVIDER[i+1];
						}
					}
					break;
				}
			}
		}
	}
	return 1;
}

//***************************************************************************************
int CAdc10x2gSrv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
	if(m_TetrModif == 2)
	{
		ULONG AdcRateDivider;
		if(ClkSrc != BRDclks_ADC_MASTERCLK)
		{
			DeltaFreq = 1.e10;
			PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
			AdcRateDivider = SetClkDivOpt(Rate, ClkValue, pSrvCfg->AdcCfg.MinRate);
		}
		else
			AdcRateDivider = 1;

		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADM2IFnr_FDVR;
		param.val = powof2(AdcRateDivider);
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		Rate = ClkValue / AdcRateDivider;
	}
	else
		Rate = ClkValue;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetRate(CModule* pModule, double& Rate, double ClkValue)
{
	if(m_TetrModif == 2)
	{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADM2IFnr_FDVR;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		ULONG AdcRateDivider = (ULONG)pow(2., (int)param.val);
		Rate = ClkValue / AdcRateDivider;
	}
	else
		Rate = ClkValue;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetGainMask(CModule* pModule, int chan)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_GAIN pGain = (PADM2IF_GAIN)&param.val;
	int gain_mask;
	switch(chan)
	{
	case 0:
		gain_mask = pGain->ByBits.Chan0;
		break;
	case 1:
		gain_mask = pGain->ByBits.Chan1;
		break;
	case 2:
		gain_mask = pGain->ByBits.Chan2;
		break;
	case 3:
		gain_mask = pGain->ByBits.Chan3;
		break;
	}
	return gain_mask;
}

//***************************************************************************************
int CAdc10x2gSrv::SetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	double inp_range;
	Status = GetInpRange(pModule, inp_range, Chan); 
	if(Status != BRDerr_OK)
		return Status;
	int max_dac_value = 255;
	int min_dac_value = 0;
	double half_dac_value = 128.;

//	if(WaitCmdReady(pModule, m_index, m_MainTetrNum, 1000))
//		return BRDerr_WAIT_TIMEOUT;

	USHORT dac_data;
	if(fabs(Bias) > inp_range)
		dac_data = Bias > 0.0 ? max_dac_value : min_dac_value;
	else
		dac_data = (USHORT)floor((Bias / inp_range + 1.) * half_dac_value + 0.5);
	if(dac_data > max_dac_value)
		dac_data = max_dac_value;
	double highBias = inp_range * (dac_data / half_dac_value - 1.);
	double Delta = Bias - highBias;
	USHORT dac_data_prec = (USHORT)floor((Delta / (inp_range * BRD_ADC_PRECBIAS) + 1.0) * half_dac_value + 0.5);
	if(dac_data_prec > max_dac_value)
		dac_data_prec = max_dac_value;
	double precBias = (inp_range * BRD_ADC_PRECBIAS) * ((double)dac_data_prec / half_dac_value - 1.);
	Bias = highBias + precBias;

	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	pSrvCfg->Bias = Bias;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC10X2G_BIAS - 1] = (UCHAR)dac_data;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC10X2G_PRECBIAS - 1] = (UCHAR)dac_data_prec;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC10X2G_BIAS;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pThDac->ByBits.Data = dac_data_prec;
	pThDac->ByBits.Num = BRDtdn_ADC10X2G_PRECBIAS;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

//***************************************************************************************
int CAdc10x2gSrv::GetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
//	double inp_range = GetInpRange(pModule, Chan); 
	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	//UCHAR dac_data = (UCHAR)pSrvCfg->ThrDac[pValChan->chan + BRDtdn_ADC212x200M_BIAS0];
	//pValChan->value = inp_range * (dac_data / half_dac_value - 1.);
	Bias = pSrvCfg->Bias;
	return Status;
}

//***************************************************************************************
//int CAdc10x2gSrv::SetClkPhase(CModule* pModule, double& Phase, ULONG Chan)
//{
//	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
//	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.; // mVolt -> Volt
//
//	//if(WaitCmdReady(pModule, m_index, m_MainTetrNum, 1000))
//	//	return BRDerr_WAIT_TIMEOUT;
//
//	USHORT dac_data;
//	if(fabs(Phase) > max_thr)
//		dac_data = Phase > 0.0 ? 255 : 0;
//	else
//		dac_data = (USHORT)floor((Phase / max_thr + 1.) * 128. + 0.5);
//	if(dac_data > 255)
//		dac_data = 255;
//	Phase = max_thr * (dac_data / 128. - 1.);
//
//	pSrvCfg->ClkPhase = Phase;
//	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC10X2G_CLKPHASE - 1] = (UCHAR)dac_data;
//	DEVS_CMD_Reg param;
//	param.idxMain = m_index;
//	param.tetr = m_MainTetrNum;
//	param.reg = MAINnr_THDAC;
//	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
//	pThDac->ByBits.Data = dac_data;
//	pThDac->ByBits.Num = BRDtdn_ADC10X2G_CLKPHASE;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	return BRDerr_OK;
//}

//***************************************************************************************
int CAdc10x2gSrv::SetClkPhase(CModule* pModule, double& Phase, ULONG Chan)
{
	double max_range = 250.; // пикосекунд
	double value = Phase; // в пикосекундах

	int max_code = 56;
	int min_code = 0;
	int shift_code = 183;

//	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
//	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.;
//	value = max_thr * value / inp_range;
	USHORT dac_data;
	if(fabs(value) > max_range)
		dac_data = value > 0.0 ? max_code : min_code;
	else
		dac_data = (USHORT)floor((value / max_range + 1.) * 28. + 0.5);
	if(dac_data > max_code)
		dac_data = max_code;

	value = max_range * (dac_data / 28. - 1.);
	Phase = value;

	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	pSrvCfg->ClkPhase = Phase;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC10X2G_CLKPHASE - 1] = (UCHAR)dac_data + shift_code;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data + shift_code;
	pThDac->ByBits.Num = BRDtdn_ADC10X2G_CLKPHASE;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
} 

//***************************************************************************************
int CAdc10x2gSrv::GetClkPhase(CModule* pModule, double& Phase, ULONG Chan)
{
	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
//	pValChan->value = pSrvCfg->ClkPhase[pValChan->chan];
	Phase = pSrvCfg->ClkPhase;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	double max_thr = BRD_ADC_MAXGAINTUN; // ±10%
//	double max_thr = pSrvCfg->RefPVS / 1000.;

//	if(WaitCmdReady(pModule, m_index, m_MainTetrNum, 1000))
//		return BRDerr_WAIT_TIMEOUT;

	USHORT dac_data;
	if(fabs(GainTuning) > max_thr)
		dac_data = GainTuning > 0.0 ? 0 : 255;
	else
		dac_data = (USHORT)floor((GainTuning / max_thr + 1.) * 128. + 0.5);
//		dac_data = (USHORT)floor((1. - GainTuning / max_thr) * 128. + 0.5);
	if(dac_data > 255)
		dac_data = 255;
	GainTuning = max_thr * (dac_data / 128. - 1.);
//	GainTuning = max_thr * (1. - dac_data / 128.);

	pSrvCfg->GainTun = GainTuning;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC10X2G_GAINTUN - 1] = (UCHAR)dac_data;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC10X2G_GAINTUN;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
//	pValChan->value = pSrvCfg->GainTun;
//	pValChan->chan = 0;
//	pValChan->value = pSrvCfg->ClkPhase[pValChan->chan];
	GainTuning = pSrvCfg->GainTun;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetStartMode(CModule* pModule, PVOID pStartStopMode)
{
	int Status = BRDerr_OK;
	PBRD_AdcStartMode pAdcStartMode = (PBRD_AdcStartMode)pStartStopMode;
	double thr = pAdcStartMode->level;
	Status = SetAdcStartMode(pModule, pAdcStartMode->src, pAdcStartMode->inv, &thr);
	if(Status != BRDerr_OK)
		return Status;
	pAdcStartMode->level = thr;
	PBRD_StartMode pStartMode = &pAdcStartMode->stndStart;

	//Status = CAdcSrv::SetStartMode(pModule, pStartMode);
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	param.reg = ADM2IFnr_STMODE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_STMODE pStMode = (PADM2IF_STMODE)&param.val;

	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	if(pSrvCfg->Version >= 0x22 && m_TetrVer >= 0x300)
	{
		pStMode->ByBits.SrcStart  = BRDsts_SUBMOD;
		pStMode->ByBits.TrigStart = 1;
	}
	else
	{
		pStMode->ByBits.SrcStart  = pStartMode->startSrc;
		pStMode->ByBits.TrigStart = pStartMode->trigOn;
	}
	pStMode->ByBits.SrcStop   = pStartMode->trigStopSrc;
	pStMode->ByBits.InvStart  = pStartMode->startInv;
	pStMode->ByBits.InvStop   = pStartMode->stopInv;
	pStMode->ByBits.Restart   = pStartMode->reStartMode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// для вывода сигнала старта на разъем SYNX
//	DEVS_CMD_Reg param;
//	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Start = 1;
	pMode0->ByBits.AdmClk = 1;
	pMode0->ByBits.Master = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_STMODE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//PADM2IF_STMODE pStMode = (PADM2IF_STMODE)&param.val;
	pStMode = (PADM2IF_STMODE)&param.val;
	pStMode->ByBits.SrcStart = BRDsts_SUBMOD;
	pStMode->ByBits.TrigStart = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

//***************************************************************************************
int CAdc10x2gSrv::GetStartMode(CModule* pModule, PVOID pStartStopMode)
{
	int Status = BRDerr_OK;
	PBRD_AdcStartMode pAdcStartMode = (PBRD_AdcStartMode)pStartStopMode;
	ULONG source, inv;
	double thr;
	Status = GetAdcStartMode(pModule, &source, &inv, &thr);
	if(Status != BRDerr_OK)
		return Status;
	pAdcStartMode->src = source;
	pAdcStartMode->inv = inv;
	pAdcStartMode->level = thr;
	PBRD_StartMode pStartMode = &pAdcStartMode->stndStart;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	param.reg = ADM2IFnr_STMODE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_STMODE pStMode = (PADM2IF_STMODE)&param.val;
	pStartMode->startSrc	= pStMode->ByBits.SrcStart;
	pStartMode->trigStopSrc	= pStMode->ByBits.SrcStop;
	pStartMode->startInv	= pStMode->ByBits.InvStart;
	pStartMode->stopInv		= pStMode->ByBits.InvStop;
	pStartMode->trigOn		= pStMode->ByBits.TrigStart;
	pStartMode->reStartMode	= pStMode->ByBits.Restart;
	//Status = CAdcSrv::GetStartMode(pModule, pStartMode);
	return Status;
}

//***************************************************************************************
int CAdc10x2gSrv::SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.StartSrc = source;
	pCtrl->ByBits.StartInv = inv;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.tetr = m_MainTetrNum;
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus;
	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		pStatus = (PADM2IF_STATUS)&param.val;
	} while(!pStatus->ByBits.CmdRdy);

	double inp_range = ADC10X2G_STARTTHREXT;
	double value = *pCmpThr;
	//double bias;
	//GetBias(pModule, bias, 0);
	if(BRDsts_ADC_CHAN == source)
	{
		GetInpRange(pModule, inp_range, 0); 
	//	value = *pCmpThr - bias;
	}

	int max_code = 255;
	int min_code = 0;

	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.;
	value = max_thr * value / inp_range;
	USHORT dac_data;
	if(fabs(value) > max_thr)
		dac_data = value > 0.0 ? max_code : min_code;
	else
		dac_data = (USHORT)floor((value / max_thr + 1.) * 128. + 0.5);
	if(dac_data > max_code)
		dac_data = max_code;

	value = max_thr * (dac_data / 128. - 1.);
	*pCmpThr = value * inp_range / max_thr;
//	if(BRDsts_ADC_CHAN == source)
//		*pCmpThr += bias;

	UCHAR bias_data = pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC10X2G_BIAS - 1];
	if(BRDsts_ADC_CHAN == source)
		dac_data -= (bias_data - 128);

	pSrvCfg->StCmpThr = *pCmpThr;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC10X2G_STARTCMP - 1] = (UCHAR)dac_data;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC10X2G_STARTCMP;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	*pSource = pCtrl->ByBits.StartSrc;
	*pInv = pCtrl->ByBits.StartInv;
	PADC10X2GSRV_CFG pSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	*pCmpThr = pSrvCfg->StCmpThr;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetPretrigMode(CModule* pModule, PBRD_PretrigMode pPreMode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_PRTMODE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_PRTMODE pPrtMode = (PADM2IF_PRTMODE)&param.val;
	pPrtMode->ByBits.Enable = pPreMode->enable;
	pPrtMode->ByBits.External = pPreMode->external;
	pPrtMode->ByBits.Assur = pPreMode->assur;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_FTYPE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	int widthFifo = param.val >> 3; // ширина FIFO (в том числе вспомогательного претриггерного) в байтах
	PADCSRV_CFG pAdcSrvCfg = (PADCSRV_CFG)m_pConfig;
	int deepFifo = (pAdcSrvCfg->FifoSize / widthFifo) >> 1; // глубина вспомогательного претриггерного FIFO

	int pre_size = deepFifo - pPreMode->size * sizeof(ULONG) / widthFifo; // pPreMode->size - в 32-битных словах
	param.reg = ADM2IFnr_CNTAF;
	param.val = (pre_size == deepFifo) ? deepFifo - 2 : pre_size ; // в словах ширины FIFO
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pre_size = deepFifo - param.val; // в словах ширины FIFO
	pPreMode->size = pre_size * widthFifo / sizeof(ULONG); // в 32-битных словах

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetPretrigMode(CModule* pModule, PBRD_PretrigMode pPreMode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_PRTMODE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_PRTMODE pPrtMode = (PADM2IF_PRTMODE)&param.val;
	pPreMode->enable = pPrtMode->ByBits.Enable;
	pPreMode->external = pPrtMode->ByBits.External;
	pPreMode->assur = pPrtMode->ByBits.Assur;

	param.reg = ADM2IFnr_FTYPE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	int widthFifo = param.val >> 3; // ширина FIFO (в том числе вспомогательного претриггерного) в байтах
	PADCSRV_CFG pAdcSrvCfg = (PADCSRV_CFG)m_pConfig;
	int deepFifo = (pAdcSrvCfg->FifoSize / widthFifo) >> 1; // глубина вспомогательного претриггерного FIFO

	param.reg = ADM2IFnr_CNTAF;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	ULONG pre_size = deepFifo - param.val; // в словах ширины FIFO
	pPreMode->size = pre_size * widthFifo / sizeof(ULONG); // в 32-битных словах

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetPretrigEvent(CModule* pModule, ULONG& EventStart)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_PRTEVENTLO;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	ULONG sync_event = param.val & 0xffff;
	param.reg = ADCnr_PRTEVENTHI;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	sync_event |= ((param.val  & 0xffff) << 16); // получаем в 128-разрядных словах
//	ULONG start_addr = GetStartAddr(pModule);
//	*(ULONG*)args = sync_event - start_addr;
	EventStart = sync_event << 2; // возвращаем в 32-разрядных словах
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetGain(CModule* pModule, double& Gain, ULONG Chan)
{
	int Status = BRDerr_OK;
	ULONG mask;
	const double *pGainVal = BRD_Gain;
	if(m_TetrModif == 2)
		pGainVal = BRD_Gain_v2;
	if(Gain >= pGainVal[BRD_GAINCNT - 1])
	{
		if(Gain > pGainVal[BRD_GAINCNT - 1])
			Status = BRDerr_ADC_ILLEGAL_GAIN | BRDerr_INFO;
		mask = BRD_GAINCNT - 1;
	}
	else
	{
		if(Gain <= pGainVal[0])
		{
			if(Gain < pGainVal[0])
				Status = BRDerr_ADC_ILLEGAL_GAIN | BRDerr_INFO;
			mask = 0;
		}
		else 
		{
			for(int i = 0; i < BRD_GAINCNT - 1; i++)
			{
				if(Gain >= pGainVal[i] && Gain <= pGainVal[i+1])
				{
					if(Gain - pGainVal[i] <= pGainVal[i+1] - Gain)
						mask = i;

					else
						mask = i+1;
					break;
				}
			}
		}
	}
	Gain = pGainVal[mask];
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_GAIN pGain = (PADM2IF_GAIN)&param.val;
	switch(Chan)
	{
	case 0:
		pGain->ByBits.Chan0 = mask;
		break;
	case 1:
		pGain->ByBits.Chan1 = mask;
		break;
	case 2:
		pGain->ByBits.Chan2 = mask;
		break;
	case 3:
		pGain->ByBits.Chan3 = mask;
		break;
	}
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return Status;
}

//***************************************************************************************
int CAdc10x2gSrv::GetGain(CModule* pModule, double& Gain, ULONG Chan)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_GAIN pGain = (PADM2IF_GAIN)&param.val;
	double value;
	const double *pGainVal = BRD_Gain;
	if(m_TetrModif == 2)
		pGainVal = BRD_Gain_v2;
	switch(Chan)
	{
	case 0:
		value = pGainVal[pGain->ByBits.Chan0];
		break;
	case 1:
		value = pGainVal[pGain->ByBits.Chan1];
		break;
	case 2:
		value = pGainVal[pGain->ByBits.Chan2];
		break;
	case 3:
		value = pGainVal[pGain->ByBits.Chan3];
		break;
	}
	Gain = value;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
{
	int Status = BRDerr_OK;
	ULONG mask;
	const double *pRangeVal = BRD_Range;
	if(m_TetrModif == 2)
		pRangeVal = BRD_Range_v2;
	if(InpRange <= pRangeVal[BRD_GAINCNT - 1])
	{
		if(InpRange < pRangeVal[BRD_GAINCNT - 1])
			Status = BRDerr_ADC_ILLEGAL_INPRANGE | BRDerr_INFO;
		mask = BRD_GAINCNT - 1;
	}
	else
	{
		if(InpRange >= pRangeVal[0])
		{
			if(InpRange > pRangeVal[0])
				Status = BRDerr_ADC_ILLEGAL_INPRANGE | BRDerr_INFO;
			mask = 0;
		}
		else 
		{
			for(int i = 0; i < BRD_GAINCNT - 1; i++)
			{
				if(InpRange <= pRangeVal[i] && InpRange >= pRangeVal[i+1])
				{
					if(pRangeVal[i] - InpRange <= InpRange - pRangeVal[i+1])
						mask = i;
					else
						mask = i+1;
					break;
				}
			}
		}
	}
	InpRange = pRangeVal[mask];
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_GAIN pGain = (PADM2IF_GAIN)&param.val;
	switch(Chan)
	{
	case 0:
		pGain->ByBits.Chan0 = mask;
		break;
	case 1:
		pGain->ByBits.Chan1 = mask;
		break;
	case 2:
		pGain->ByBits.Chan2 = mask;
		break;
	case 3:
		pGain->ByBits.Chan3 = mask;
		break;
	}
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return Status;
}

//***************************************************************************************
int CAdc10x2gSrv::GetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_GAIN pGain = (PADM2IF_GAIN)&param.val;
	double value;
	const double *pRangeVal = BRD_Range;
	if(m_TetrModif == 2)
		pRangeVal = BRD_Range_v2;
	switch(Chan)
	{
	case 0:
		value = pRangeVal[pGain->ByBits.Chan0];
		break;
	case 1:
		value = pRangeVal[pGain->ByBits.Chan1];
		break;
	case 2:
		value = pRangeVal[pGain->ByBits.Chan2];
		break;
	case 3:
		value = pRangeVal[pGain->ByBits.Chan3];
		break;
	}
	InpRange = value;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetTestMode(CModule* pModule, ULONG mode)
{
	CAdcSrv::SetTestMode(pModule, mode & 0xFFFF);
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.Test = mode >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetTestMode(CModule* pModule, ULONG& mode)
{
	ULONG test_data;
	CAdcSrv::GetTestMode(pModule, test_data);
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	mode = test_data | (pCtrl->ByBits.Test << 16);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::ExtraInit(CModule* pModule)
{
	CheckCfg(pModule);
	InitPLL(pModule);
	return BRDerr_OK;
}

//****************************************************************************************
int CAdc10x2gSrv::CheckCfg(CModule* pModule)
{
	DEVS_CMD_Reg RegParam;
	RegParam.idxMain = m_index;
	RegParam.tetr = m_AdcTetrNum;
	RegParam.reg = ADM2IFnr_IDMOD;
	pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
	m_TetrModif = RegParam.val;

	RegParam.tetr = m_AdcTetrNum;
	RegParam.reg = ADM2IFnr_VER;
	pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
	m_TetrVer = RegParam.val;

	return BRDerr_OK;
}

//****************************************************************************************
//int CAdc10x2gSrv::readPllData(CModule* pModule, USHORT& HiWord, USHORT& LoWord)
//{
//	DEVS_CMD_Reg param;
//	param.idxMain = m_index;
//	param.tetr = m_AdcTetrNum;
//	param.reg = ADCnr_HIPLL;
//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	HiWord = param.val;
//	param.reg = ADCnr_LOWPLL;
//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	LoWord = param.val;
//	return BRDerr_OK;
//}

//****************************************************************************************
int CAdc10x2gSrv::writePllData(CModule* pModule, USHORT HiWord, USHORT LoWord)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_HIPLL;
	param.val = HiWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADCnr_LOWPLL;
	param.val = LoWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//****************************************************************************************
int CAdc10x2gSrv::InitPLL(CModule* pModule)
{
	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	pAdcSrvCfg->Pll_IF_R.AsWhole = 0;
	pAdcSrvCfg->Pll_RF_R.AsWhole = 0;
	pAdcSrvCfg->Pll_RF_N.AsWhole = 0;

	pAdcSrvCfg->Pll_IF_R.ByBits.Addr = 0;
	pAdcSrvCfg->Pll_RF_R.ByBits.Addr = 2;
	pAdcSrvCfg->Pll_RF_N.ByBits.Addr = 3;

	pAdcSrvCfg->Pll_RF_N.ByBits.RF_CTL_WORD = 4; // reset
	if(pAdcSrvCfg->Version <= 0x20)
		writePllData(pModule, pAdcSrvCfg->Pll_RF_N.ByWord.HiWord, pAdcSrvCfg->Pll_RF_N.ByWord.LoWord);

	pAdcSrvCfg->Pll_RF_N.ByBits.RF_CTL_WORD = 1; // normal - 0 = Prescaler Modulus select = 16 (1 GHz), 1 = 32 (2 GHz)
	if(pAdcSrvCfg->Version <= 0x20)
		writePllData(pModule, pAdcSrvCfg->Pll_RF_N.ByWord.HiWord, pAdcSrvCfg->Pll_RF_N.ByWord.LoWord);

	pAdcSrvCfg->Pll_IF_R.ByBits.FRAC_16 = 1; // 0 - 1/16 resolution, 1 - 1/32 resolution
	pAdcSrvCfg->Pll_IF_R.ByBits.FoLD = 2; // RF Digital Lock Detect
	if(pAdcSrvCfg->Version <= 0x20)
		writePllData(pModule, pAdcSrvCfg->Pll_IF_R.ByWord.HiWord, pAdcSrvCfg->Pll_IF_R.ByWord.LoWord);

//	pAdcSrvCfg->Pll_RF_R.ByBits.DLL_MODE = 1; // fast
	pAdcSrvCfg->Pll_RF_R.ByBits.DLL_MODE = 0; // normal
	int RfPdPol = PLL_pdpolPositive;// RF VCO - positive
	int OutCurrent = pAdcSrvCfg->OutCurrent; // 8;
	pAdcSrvCfg->Pll_RF_R.ByBits.RF_CP_WORD = (OutCurrent << 1) || RfPdPol;
//	pAdcSrvCfg->Pll_RF_R.ByBits.RF_CP_WORD = 9; // RF VCO - positive, RF Charge Pump output current = 500 mkA
//	pAdcSrvCfg->Pll_RF_R.ByBits.RF_CP_WORD = 8; // RF VCO - negative, RF Charge Pump output current = 500 mkA
	if(pAdcSrvCfg->Version <= 0x20)
		writePllData(pModule, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord, pAdcSrvCfg->Pll_RF_R.ByWord.LoWord);

	pAdcSrvCfg->Pll_R_CNT.AsWhole = 0;
	pAdcSrvCfg->Pll_N_CNT.AsWhole = 0;
	pAdcSrvCfg->Pll_Func.AsWhole = 0;

	pAdcSrvCfg->Pll_R_CNT.ByBits.Addr = 0;
	pAdcSrvCfg->Pll_N_CNT.ByBits.Addr = 1;
	pAdcSrvCfg->Pll_Func.ByBits.Addr = 2;

	pAdcSrvCfg->Pll_R_CNT.ByBits.AB_WIDTH = 1; //1.3 ns
	pAdcSrvCfg->Pll_R_CNT.ByBits.TEST_MODE = 0;
	pAdcSrvCfg->Pll_R_CNT.ByBits.LDP = 0;
	if(pAdcSrvCfg->Version > 0x20)
		writePllData(pModule, pAdcSrvCfg->Pll_R_CNT.ByWord.HiWord, pAdcSrvCfg->Pll_R_CNT.ByWord.LoWord);

	pAdcSrvCfg->Pll_N_CNT.ByBits.CP_GAIN = 0;
	if(pAdcSrvCfg->Version > 0x20)
		writePllData(pModule, pAdcSrvCfg->Pll_N_CNT.ByWord.HiWord, pAdcSrvCfg->Pll_N_CNT.ByWord.LoWord);

	pAdcSrvCfg->Pll_Func.ByBits.CNT_RST = 0;
	pAdcSrvCfg->Pll_Func.ByBits.PWDN_1 = 0;
	pAdcSrvCfg->Pll_Func.ByBits.PWDN_2 = 0;
	pAdcSrvCfg->Pll_Func.ByBits.MUX_OUT = 2;
	pAdcSrvCfg->Pll_Func.ByBits.PD_POL = 1;
	pAdcSrvCfg->Pll_Func.ByBits.CP_3STATE = 0;
	pAdcSrvCfg->Pll_Func.ByBits.FL_ENBL = 0;
	pAdcSrvCfg->Pll_Func.ByBits.FL_MODE = 0;
	pAdcSrvCfg->Pll_Func.ByBits.TIMER_CNT = 0;
	pAdcSrvCfg->Pll_Func.ByBits.CUR_SET_1 = 3;
	pAdcSrvCfg->Pll_Func.ByBits.CUR_SET_2 = 7;

	if(pAdcSrvCfg->Version > 0x20)
		writePllData(pModule, pAdcSrvCfg->Pll_Func.ByWord.HiWord, pAdcSrvCfg->Pll_Func.ByWord.LoWord);

	return BRDerr_OK;
}

//****************************************************************************************
int CAdc10x2gSrv::OffPLL(CModule* pModule)
{
//	pAdcSrvCfg->Pll_RF_R pAdcSrvCfg->Pll_RF_R;
//	readPllData(pModule, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord, pAdcSrvCfg->Pll_RF_R.ByWord.LoWord);
	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	pAdcSrvCfg->Pll_RF_R.ByBits.V2_EN = 0;
	if(pAdcSrvCfg->Version <= 0x20)
		writePllData(pModule, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord, pAdcSrvCfg->Pll_RF_R.ByWord.LoWord);

	return BRDerr_OK;
}

//****************************************************************************************
int CAdc10x2gSrv::OnPll(CModule* pModule, double& ClkValue, double PllRefValue)
{
	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
//	pAdcSrvCfg->Pll_RF_R pAdcSrvCfg->Pll_RF_R;
//	pAdcSrvCfg->Pll_RF_R.ByBits.Addr = 2;
//	readPllData(pModule, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord, pAdcSrvCfg->Pll_RF_R.ByWord.LoWord);
//	pAdcSrvCfg->Pll_RF_N pAdcSrvCfg->Pll_RF_N;
//	pAdcSrvCfg->Pll_RF_N.ByBits.Addr = 3;
//	readPllData(pModule, pAdcSrvCfg->Pll_RF_N.ByWord.HiWord, pAdcSrvCfg->Pll_RF_N.ByWord.LoWord);

	if(pAdcSrvCfg->Version > 0x20)
	{
		OnPll_v21(pModule, ClkValue, PllRefValue);
		return BRDerr_OK;
	}

	double f_vco = ClkValue;
	double f_osc = PllRefValue;
	int A = 0;
	int F = 0;
	int P = (pAdcSrvCfg->Pll_RF_N.ByBits.RF_CTL_WORD) ? (PLL_P_DIV_16 << 1) : PLL_P_DIV_16;
	double f_cmp;
	f_cmp = PLL_F_CMP_MIN;
	int R_max = int(floor(f_osc / f_cmp)); // 7 ; 80
	f_cmp = PLL_F_CMP_MAX;
	int R_min = int(ceil(f_osc / f_cmp)); // 3(0); 4
//	if(R_min < 3) R_min = 3;
	if(R_min < 4) R_min = 4;
	int R, B_tmp, B = 2000;
//	int delta_min = 100;
	double delta_min = 10000000;
//	for(int R_tmp = R_min; R_tmp <= R_max; R_tmp++)
	for(int R_tmp = R_max; R_tmp >= R_min; R_tmp--)
	{
		B_tmp = int(f_vco / P * R_tmp / f_osc);
//		double f_new = P * B_tmp * (f_osc / R_tmp);
		double f_new = P * B_tmp * f_osc / R_tmp;
//		int delta = int((fabs(pAdcSrvCfg->PllFreq - f_new) / pAdcSrvCfg->PllFreq) * 100);
		double delta = (fabs(pAdcSrvCfg->PllFreq - f_new) / pAdcSrvCfg->PllFreq) * 10000000;
		if(delta < delta_min)
		{
			ClkValue = f_new;
			delta_min = delta;
			B = B_tmp;
			R = R_tmp;
		}
	}
	if(pAdcSrvCfg->FracMode)
	{
		B_tmp = B - 1;
		int F_tmp = int(f_vco * R / f_osc - P * B_tmp);
		if(F_tmp > 15) {
			B_tmp = B;
			F_tmp = int(f_vco * R / f_osc - P * B_tmp);
		}
		B = B_tmp;
		F = F_tmp;
		ClkValue = (P * B + F) * (f_osc / R);
	}
	pAdcSrvCfg->Pll_RF_R.ByBits.RF_R_CNTR = R;
	writePllData(pModule, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord, pAdcSrvCfg->Pll_RF_R.ByWord.LoWord);
//	updatePllData(pAdcSrvCfg->Pll_RF_R.ByWord.LoWord, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord);

	pAdcSrvCfg->Pll_RF_N.ByBits.FRAC_CNTR = F;
	pAdcSrvCfg->Pll_RF_N.ByBits.RF_NA_CNTR = A;
	pAdcSrvCfg->Pll_RF_N.ByBits.RF_NB_CNTR = B;
	writePllData(pModule, pAdcSrvCfg->Pll_RF_N.ByWord.HiWord, pAdcSrvCfg->Pll_RF_N.ByWord.LoWord);
//	updatePllData(pAdcSrvCfg->Pll_RF_N.ByWord.LoWord, pAdcSrvCfg->Pll_RF_N.ByWord.HiWord);

//	Sleep(500);
	return BRDerr_OK;
}

//****************************************************************************************
int CAdc10x2gSrv::SetFold(CModule* pModule, int fold)
{
	if(fold != PLL_foldLockRF && fold != PLL_foldCntRF_R && fold != PLL_foldCntRF_N)
		return BRDerr_BAD_PARAMETER;
	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	if(pAdcSrvCfg->Version <= 0x20)
	{
		pAdcSrvCfg->Pll_IF_R.ByBits.FoLD = fold;
		writePllData(pModule, pAdcSrvCfg->Pll_IF_R.ByWord.HiWord, pAdcSrvCfg->Pll_IF_R.ByWord.LoWord);
//		updatePllData(m_IF_R.ByWord.LoWord, m_IF_R.ByWord.HiWord);
	}
	return BRDerr_OK;
}

int P_DIM[4] = {8, 16, 32, 64};

//****************************************************************************************
int CAdc10x2gSrv::OnPll_v21(CModule* pModule, double& ClkValue, double PllRefValue)
{
	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;

	double f_vco = ClkValue;
	double f_osc = PllRefValue;

	double f_min = 325000000.; // 325 MHz
	int A_max = 63;
	int B_max = 8191;
	int P_min = int(floor(f_vco / f_min));
	int P = P_DIM[3];
	for(int i = 0; i < 4; i++)
		if(P_min <= P_DIM[i]) {
			P = P_DIM[i];
			break;
		}

	int R_max = int(floor(f_osc / (f_vco / (P * B_max + A_max)) ));
	if(R_max > 16383) R_max = 16383;

	int R_min = int(ceil(f_osc / PLL_V21_F_CMP_MAX));
	if(R_min < 1) R_min = 1;

	int R, B_tmp, B, A = 0;
	//int delta_min = 100; // 1G
	double delta_min = 10000000; // 2G

	for(int A = 0; A <= A_max; A++)
	{
		for(int R_tmp = R_min; R_tmp <= R_max; R_tmp++)
		{
			B_tmp = int(((f_vco * R_tmp / f_osc) - A) / P);
			double f_new = (P * B_tmp + A) * (f_osc / R_tmp);
			//int delta = int((fabs(pAdcSrvCfg->PllFreq - f_new) / pAdcSrvCfg->PllFreq) * 100); // 1G
	//		double delta = (fabs(pAdcSrvCfg->PllFreq - f_new) / pAdcSrvCfg->PllFreq) * 10000000; // 2G
			double delta = (fabs(pAdcSrvCfg->PllFreq - f_new) / pAdcSrvCfg->PllFreq); // 200M
			if(delta < delta_min)
			{
				ClkValue = f_new;
				delta_min = delta;
				B = B_tmp;
				R = R_tmp;
				if(delta_min == 0)
					break;
			}
		}
		if(delta_min == 0)
			break;
	}
	pAdcSrvCfg->Pll_R_CNT.ByBits.R_CNTR = R;
	writePllData(pModule, pAdcSrvCfg->Pll_R_CNT.ByWord.HiWord, pAdcSrvCfg->Pll_R_CNT.ByWord.LoWord);
	pAdcSrvCfg->Pll_N_CNT.ByBits.A_CNTR = A;
	pAdcSrvCfg->Pll_N_CNT.ByBits.B_CNTR = B;
	writePllData(pModule, pAdcSrvCfg->Pll_N_CNT.ByWord.HiWord, pAdcSrvCfg->Pll_N_CNT.ByWord.LoWord);
	pAdcSrvCfg->Pll_Func.ByBits.PRESCALER = P;
	writePllData(pModule, pAdcSrvCfg->Pll_Func.ByWord.HiWord, pAdcSrvCfg->Pll_Func.ByWord.LoWord);

	Sleep(50);
	return BRDerr_OK;
}

//****************************************************************************************
int CAdc10x2gSrv::SetMuxOut(CModule* pModule, U08 muxout)
{
	if(muxout < 0 || muxout > 7)
		return BRDerr_BAD_PARAMETER;
	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	if(pAdcSrvCfg->Version > 0x20)
	{
		pAdcSrvCfg->Pll_Func.ByBits.MUX_OUT = muxout;
		writePllData(pModule, pAdcSrvCfg->Pll_Func.ByWord.HiWord, pAdcSrvCfg->Pll_Func.ByWord.LoWord);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetPllMode(CModule* pModule, PBRD_Adc10x2gPll pMode)
{
	int Status = BRDerr_OK;
	PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	pAdcSrvCfg->ExtPllRef = ROUND(pMode->ExtRef);
	return Status;
}

//***************************************************************************************
int CAdc10x2gSrv::SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec)
{
	int Status = BRDerr_OK;
	switch(pSpec->command)
	{
	case ADC10X2Gcmd_SETPLLMODE:
		{
		PBRD_Adc10x2gPll pMode = (PBRD_Adc10x2gPll)pSpec->arg;
		SetPllMode(pModule, pMode);
		}
		break;
	case ADC10X2Gcmd_GETEVENTCNT:
		{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADCnr_EVENTCNTL;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		ULONG counterLo = param.val & 0xffff;
		param.reg = ADCnr_EVENTCNTH;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		counterLo |= (param.val & 0xffff) << 16;
		param.reg = ADCnr_PREVPREC;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		ULONG counterHi = (param.val & 0xffff) >> 8;
		__int64 counter = (__int64)counterLo | ((__int64)counterHi << 32);
//		*(PULONG)pSpec->arg = counter;
		*(__int64*)pSpec->arg = counter;
		}
		break;
	case ADC10X2Gcmd_RESETEVENTCNT:
		{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADCnr_EVENTCNTL;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		break;
	case ADC10X2Gcmd_SETTITLEMODE:
		{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADM2IFnr_TLMODE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADM2IF_TLMODE pMode = (PADM2IF_TLMODE)&param.val;
		pMode->ByBits.TitleOn = *(PULONG)pSpec->arg;
//		TCHAR buf[64];
//		sprintf_s(buf, _T("%s: TitleOn = %d"), pModule->GetName(), pMode->ByBits.TitleOn);
//		MessageBox(NULL, buf, "B10x2G", MB_OK);
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		break;
	case ADC10X2Gcmd_GETTITLEMODE:
		{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADM2IFnr_TLMODE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADM2IF_TLMODE pMode = (PADM2IF_TLMODE)&param.val;
		*(PULONG)pSpec->arg = pMode->ByBits.TitleOn;
		}
		break;
	case ADC10X2Gcmd_STABILITY:
		{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADC10X2Gnr_STABILITY;
		param.val = *(PULONG)pSpec->arg;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		break;
	case ADC10X2Gcmd_SETSTARTIN:
		{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADCnr_CTRL1;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
		pCtrl->ByBits.StartSl = *(PULONG)pSpec->arg;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		break;
	case ADC10X2Gcmd_SETCLKIN:
		{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADCnr_CTRL1;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
		pCtrl->ByBits.ClkSl = *(PULONG)pSpec->arg;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		break;
	case ADC10X2Gcmd_SETTIMERSYNC:
		{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADCnr_CTRL1;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
		pCtrl->ByBits.TimerSync = *(PULONG)pSpec->arg;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		break;
	case ADC10X2Gcmd_GETSTARTIN:
		{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADCnr_CTRL1;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
		*(PULONG)pSpec->arg = pCtrl->ByBits.StartSl;
		}
		break;
	case ADC10X2Gcmd_SETACCMODE:
		{
		PBRD_Adc10x2gAcc pMode = (PBRD_Adc10x2gAcc)pSpec->arg;
		SetAccumulator(pModule, pMode->AccOn, pMode->AccCnt);
		}
		break;
	}
	return Status;
}

//***************************************************************************************
int CAdc10x2gSrv::SetSpecParam(CModule* pModule, ULONG StartSl, ULONG ClkSl, ULONG TimerSync)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.StartSl = StartSl;
	pCtrl->ByBits.ClkSl = ClkSl;
	pCtrl->ByBits.TimerSync = TimerSync;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetSpecParam(CModule* pModule, ULONG& StartSl, ULONG& ClkSl, ULONG& TimerSync)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	StartSl = pCtrl->ByBits.StartSl;
	ClkSl = pCtrl->ByBits.ClkSl;
	TimerSync = pCtrl->ByBits.TimerSync;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetAccumulator(CModule* pModule, ULONG AccEn, ULONG AccCnt)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_MODE3;
	param.val = AccCnt;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.AccEn = AccEn;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
//int CAdc10x2gSrv::GetSpecific(CModule* pModule, PBRD_AdcSpec pSpec)
//{
//	int Status = BRDerr_OK;
//	switch()
//	{
//	case ADC10X2Gcmd_GETPLLMODE:
//		{
//		PBRD_Adc10x2gPll pMode = (PBRD_Adc10x2gPll)pSpec->arg;
//		PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
//		pMode->ExtRef = pAdcSrvCfg->ExtPllRef;
//		}
//		break;
//	}
//	return Status;
//}

//***************************************************************************************
int CAdc10x2gSrv::SetCnt(CModule* pModule, ULONG numreg, PBRD_EnVal pEnVal)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	switch(numreg)
	{
	case 0:
		pMode0->ByBits.Cnt0En = pEnVal->enable;
		break;
	case 1:
		pMode0->ByBits.Cnt1En = pEnVal->enable;
		break;
	case 2:
		pMode0->ByBits.Cnt2En = pEnVal->enable;
		break;
	}
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_FTYPE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	int widthFifo = param.val >> 3; // ширина FIFO (в байтах)
	param.val = pEnVal->value * sizeof(ULONG) / widthFifo; // pEnVal->value - в 32-битных словах
	param.reg = ADM2IFnr_CNT0 + numreg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pEnVal->value = param.val * widthFifo / sizeof(ULONG); // pEnVal->value - в 32-битных словах

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetCnt(CModule* pModule, ULONG numreg, PBRD_EnVal pEnVal)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	switch(numreg)
	{
	case 0:
		pEnVal->enable = pMode0->ByBits.Cnt0En;
		break;
	case 1:
		pEnVal->enable = pMode0->ByBits.Cnt1En;
		break;
	case 2:
		pEnVal->enable = pMode0->ByBits.Cnt2En;
		break;
	}

	param.reg = ADM2IFnr_FTYPE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	int widthFifo = param.val >> 3; // ширина FIFO (в байтах)

	param.reg = ADM2IFnr_CNT0 + numreg;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pEnVal->value = param.val * widthFifo / sizeof(ULONG); // pEnVal->value - в 32-битных словах
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetMaster(CModule* pModule, ULONG mode)
{
	m_MasterMode = mode;
	CAdcSrv::SetMaster(pModule, mode);

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.reg = ADM2IFnr_MODE0;
	param.tetr = m_MainTetrNum;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Start = 1;
	pMode0->ByBits.AdmClk = 1;
	pMode0->ByBits.Master = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetMaster(CModule* pModule, ULONG& mode)
{
	mode = m_MasterMode;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::SetClkLocation(CModule* pModule, ULONG& mode)
{
	mode = 1;
	CAdcSrv::SetClkLocation(pModule, mode);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc10x2gSrv::GetClkLocation(CModule* pModule, ULONG& mode)
{
	mode = 1;
	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;
	//param.tetr = m_AdcTetrNum;
	//param.reg = ADM2IFnr_MODE0;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	//mode = pMode0->ByBits.AdmClk;
	return BRDerr_OK;
}

// ***************** End of file Adc10x2gSrv.cpp ***********************
