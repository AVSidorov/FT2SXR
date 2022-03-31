/*
 ***************** File Adc28x800mSrv.cpp ************
 *
 * BRD Driver for ADС service on ADM28x800M
 *
 * (C) InSys by Dorokhin A. Dec 2004
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "adc28x800msrv.h"

#define	CURRFILE "ADC28x800MSRV"

const double BRD_ADC_MAXGAINTUN	= 10.; // max gain tuning = 10%
const double BRD_ADC_MAXCLKTUN	= 5.; // max gain tuning = 5%

//***************************************************************************************
CAdc28x800mSrv::CAdc28x800mSrv(int idx, int srv_num, CModule* pModule, PADC28X800MSRV_CFG pCfg) :
	CAdcSrv(idx, _T("ADC28X800M"), srv_num, pModule, pCfg, sizeof(ADC28X800MSRV_CFG))
{
}

//***************************************************************************************
int CAdc28x800mSrv::CtrlRelease(
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
void CAdc28x800mSrv::GetAdcTetrNum(CModule* pModule)
{
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC28X800M_TETR_ID);
}

//***************************************************************************************
void CAdc28x800mSrv::FreeInfoForDialog(PVOID pInfo)
{
	PADC28X800MSRV_INFO pSrvInfo = (PADC28X800MSRV_INFO)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc28x800mSrv::GetInfoForDialog(CModule* pDev)
{
	PADC28X800MSRV_CFG pSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
	PADC28X800MSRV_INFO pSrvInfo = new ADC28X800MSRV_INFO;
	pSrvInfo->Size = sizeof(ADC28X800MSRV_INFO);
	pSrvInfo->pAdcInfo = (PADCSRV_INFO)CAdcSrv::GetInfoForDialog(pDev);
	pSrvInfo->MainRefGen = pSrvCfg->SubMainGen;
	double ClkTuning;
	GetClkTuning(pDev, ClkTuning);
	pSrvInfo->DacMainGen = ClkTuning;
	pSrvInfo->MaxVcoGenFreq = pSrvCfg->MaxVcoGenFreq;
	pSrvInfo->MinVcoGenFreq = pSrvCfg->MinVcoGenFreq;
	pSrvInfo->VcoClk = pSrvCfg->SubVcoClk;
	pSrvInfo->ExtClk = pSrvCfg->SubExtClk;
	GetClkSource(pDev, pSrvInfo->pAdcInfo->ClockSrc);
	GetClkValue(pDev, pSrvInfo->pAdcInfo->ClockSrc, pSrvInfo->pAdcInfo->ClockValue);
	GetRate(pDev, pSrvInfo->pAdcInfo->SamplingRate, pSrvInfo->pAdcInfo->ClockValue);
	if(pSrvInfo->pAdcInfo->ClockSrc == BRDclks_ADC_SUBVCOGEN)
		pSrvInfo->VcoClk = pSrvInfo->pAdcInfo->ClockValue;
	if(pSrvInfo->pAdcInfo->ClockSrc == BRDclks_ADC_EXTCLK || pSrvInfo->pAdcInfo->ClockSrc == BRDclks_ADC_MASTERCLK)
		pSrvInfo->ExtClk = pSrvInfo->pAdcInfo->ClockValue;
	for(int i = 0; i < MAX_CHAN; i++)
	{
		GetInpRange(pDev, pSrvInfo->pAdcInfo->Range[i], i);
		GetBias(pDev, pSrvInfo->pAdcInfo->Bias[i], i);
//		GetInpResist(pDev, pSrvInfo->pAdcInfo->Resist[i], i);
//		GetDcCoupling(pDev, pSrvInfo->pAdcInfo->DcCoupling[i], i);
//		GetClkPhase(pDev, pSrvInfo->PhaseTuning[i], i);
		GetGainTuning(pDev, pSrvInfo->GainTuning[i], i);
	}

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

	return pSrvInfo;
}

//***************************************************************************************
int CAdc28x800mSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
//	TCHAR msg[256];
	CModule* pModule = (CModule*)pDev;
	PADC28X800MSRV_INFO pInfo = (PADC28X800MSRV_INFO)args;
//	ULONG ClockSrc = pInfo->pAdcInfo->ClockSrc;
//	double ClockValue = pInfo->pAdcInfo->ClockValue;
//	double SamplingRate = pInfo->pAdcInfo->SamplingRate;
//	CAdcSrv::SetPropertyFromDialog(pDev, pInfo->pAdcInfo);
//	pInfo->pAdcInfo->ClockSrc = ClockSrc;
//	pInfo->pAdcInfo->ClockValue = ClockValue;
//	pInfo->pAdcInfo->SamplingRate = SamplingRate;
	CAdcSrv::SetChanMask(pModule, pInfo->pAdcInfo->ChanMask);
	ULONG master = pInfo->pAdcInfo->SyncMode;
	CtrlMaster(pDev, NULL, BRDctrl_ADC_SETMASTER, &master);
	ULONG format = pInfo->pAdcInfo->Format;
	CtrlFormat(pDev, NULL, BRDctrl_ADC_SETFORMAT, &format);
	ULONG code = pInfo->pAdcInfo->Encoding;
	CtrlCode(pDev, NULL, BRDctrl_ADC_SETCODE, &code);

	BRD_EnVal st_delay;
	st_delay.enable	= pInfo->pAdcInfo->Cnt0Enable;
	st_delay.value	= pInfo->pAdcInfo->StartDelayCnt;
	SetCnt(pModule, 0, &st_delay);
	BRD_EnVal acq_data;
	acq_data.enable	= pInfo->pAdcInfo->Cnt1Enable;
	acq_data.value	= pInfo->pAdcInfo->DaqCnt;
	SetCnt(pModule, 1, &acq_data);
	BRD_EnVal skip_data;
	skip_data.enable = pInfo->pAdcInfo->Cnt2Enable;
	skip_data.value	 = pInfo->pAdcInfo->SkipCnt;
	SetCnt(pModule, 2, &skip_data);

	PADC28X800MSRV_CFG pAdcSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
	pAdcSrvCfg->SubVcoClk = ROUND(pInfo->VcoClk);
	pAdcSrvCfg->SubExtClk = ROUND(pInfo->ExtClk);
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetClkTuning(pModule, pInfo->DacMainGen);
	for(int i = 0; i < MAX_CHAN; i++)
	{
		SetInpRange(pModule, pInfo->pAdcInfo->Range[i], i);
		SetBias(pModule, pInfo->pAdcInfo->Bias[i], i);
//		SetInpResist(pModule, pInfo->pAdcInfo->Resist[i], i);
		SetDcCoupling(pModule, pInfo->pAdcInfo->DcCoupling[i], i);
//		SetClkPhase(pModule, pInfo->PhaseTuning[i], i);
		SetGainTuning(pModule, pInfo->GainTuning[i], i);
	}
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
	SetStartMode(pModule, &start_mode);
	pInfo->StartLevel = start_mode.level;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::SetProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	TCHAR ParamName[128];
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);

	GetPrivateProfileString(SectionName, "VCOClockValue", "800000000.0", Buffer, sizeof(Buffer), iniFilePath);
	double clkValue = atof(Buffer);
	SetClkValue(pDev, BRDclks_ADC_SUBVCOGEN, clkValue);
	GetPrivateProfileString(SectionName, "ExternalClockValue", "200000000.0", Buffer, sizeof(Buffer), iniFilePath);
	clkValue = atof(Buffer);
	SetClkValue(pDev, BRDclks_ADC_EXTCLK, clkValue);

	GetPrivateProfileString(SectionName, "ClockSource", "1", Buffer, sizeof(Buffer), iniFilePath);
	ULONG clkSrc = atoi(Buffer);;
	SetClkSource(pDev, clkSrc);
	if(clkSrc != BRDclks_ADC_SUBVCOGEN && clkSrc != BRDclks_ADC_EXTCLK)
		SetClkValue(pDev, clkSrc, clkValue);
	GetClkValue(pDev, clkSrc, clkValue);

	GetPrivateProfileString(SectionName, "SamplingRate", "400000000.0", Buffer, sizeof(Buffer), iniFilePath);
	double rate = atof(Buffer);
	SetRate(pDev, rate, clkSrc, clkValue);

	GetPrivateProfileString(SectionName, "MainClockTuning", "0.0", Buffer, sizeof(Buffer), iniFilePath);
	double ClkTuning = atof(Buffer);
	SetClkTuning(pDev, ClkTuning);

	double InpRange[MAX_CHAN], Bias[MAX_CHAN], GainTuning[MAX_CHAN];
	for(int i = 0; i < MAX_CHAN; i++)
	{
		sprintf_s(ParamName, "InputRange%d", i);
		GetPrivateProfileString(SectionName, ParamName, "0.5", Buffer, sizeof(Buffer), iniFilePath);
		InpRange[i] = atof(Buffer);
		SetInpRange(pDev, InpRange[i], i);
		sprintf_s(ParamName, "Bias%d", i);
		GetPrivateProfileString(SectionName, ParamName, "0.0", Buffer, sizeof(Buffer), iniFilePath);
		Bias[i] = atof(Buffer);
		SetBias(pDev, Bias[i], i);
		sprintf_s(ParamName, "GainTuning%d", i);
		GetPrivateProfileString(SectionName, ParamName, "0.0", Buffer, sizeof(Buffer), iniFilePath);
		GainTuning[i] = atof(Buffer);
		SetGainTuning(pDev, GainTuning[i], i);
	}

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
	SetStartMode(pDev, &start);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	TCHAR ParamName[128];
	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);
//	PADC28X800MSRV_CFG pSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
//	sprintf(Buffer, "%u", pSrvCfg->AdcCfg.Encoding);
//	WritePrivateProfileString(SectionName, "CodeType", Buffer, iniFilePath);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	sprintf_s(Buffer, "%u", clkSrc);
	WritePrivateProfileString(SectionName, "ClockSource", Buffer, iniFilePath);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
	if(clkSrc == BRDclks_ADC_SUBVCOGEN)
	{
		sprintf_s(Buffer, "%.2f", clkValue);
		WritePrivateProfileString(SectionName, "VCOClockValue", Buffer, iniFilePath);
	}
	if(clkSrc == BRDclks_ADC_EXTCLK || clkSrc == BRDclks_ADC_MASTERCLK)
	{
		sprintf_s(Buffer, "%.2f", clkValue);
		WritePrivateProfileString(SectionName, "ExternalClockValue", Buffer, iniFilePath);
	}
	double rate;
	GetRate(pDev, rate, clkValue);
	sprintf_s(Buffer, "%.2f", rate);
	WritePrivateProfileString(SectionName, "SamplingRate", Buffer, iniFilePath);

	double ClkTuning;
	GetClkTuning(pDev, ClkTuning);
	sprintf_s(Buffer, "%.2f", ClkTuning);
	WritePrivateProfileString(SectionName, "MainClockTuning", Buffer, iniFilePath);

	double InpRange[MAX_CHAN], Bias[MAX_CHAN], GainTuning[MAX_CHAN];
	for(int i = 0; i < MAX_CHAN; i++)
	{
		GetInpRange(pDev, InpRange[i], i);
		sprintf_s(Buffer, "%.2f", InpRange[i]);
		sprintf_s(ParamName, "InputRange%d", i);
		WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		GetBias(pDev, Bias[i], i);
		sprintf_s(Buffer, "%.4f", Bias[i]);
		sprintf_s(ParamName, "Bias%d", i);
		WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		GetGainTuning(pDev, GainTuning[i], i);
		sprintf_s(Buffer, "%.4f", GainTuning[i]);
		sprintf_s(ParamName, "GainTuning%d", i);
		WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
	}

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

	// the function flushes the cache
	WritePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::SetClkSource(CModule* pModule, ULONG ClkSrc)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	if(BRDclks_ADC_MASTERCLK == ClkSrc)
	{
		pCtrl->ByBits.StartClkSl = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		return BRDerr_OK;
	}
	else
	{
		pCtrl->ByBits.StartClkSl = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_FSRC;
		param.val = ClkSrc;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.reg = ADM2IFnr_FSRC;
			param.val = ClkSrc;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
//		else
//			return BRDerr_; // функция в режиме Slave не выполнима
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
{
	ULONG source;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	if(pCtrl->ByBits.StartClkSl)
	{
		ClkSrc = BRDclks_ADC_MASTERCLK;
		return BRDerr_OK;
	}
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
int CAdc28x800mSrv::SetVcoClkValue(CModule* pModule, double& ClkValue)
{
	PADC28X800MSRV_CFG pSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
	double half_auxfreq = double(pSrvCfg->MaxVcoGenFreq + pSrvCfg->MinVcoGenFreq) / 2;
	double max_auxfreq = double(pSrvCfg->MaxVcoGenFreq - pSrvCfg->MinVcoGenFreq) / 2;
	ClkValue -= half_auxfreq;

//	ClkValue -= double(pSrvCfg->MinVcoGenFreq);
//	double max_auxfreq = double(pSrvCfg->MaxVcoGenFreq - pSrvCfg->MinVcoGenFreq);
	USHORT dac_data;
	if(fabs(ClkValue) >= max_auxfreq)
		dac_data = ClkValue > 0.0 ? 65535 : 0;
	else
		dac_data = (USHORT)floor((ClkValue / max_auxfreq + 1.) * 32768. + 0.5);
	if(dac_data > 65535)
		dac_data = 65535;
	ClkValue = max_auxfreq * (dac_data / 32768. - 1.);
//	ClkValue += double(pSrvCfg->MinVcoGenFreq);
	ClkValue += half_auxfreq;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_VCODAC;
	param.val = dac_data;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC28X800MSRV_CFG pAdcSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:			// disabled clock
		ClkValue = 0.0;
		break;
	case BRDclks_ADC_SUBMAINGEN:		// Submodule Main Generator
		ClkValue = pAdcSrvCfg->SubMainGen;
		break;
	case BRDclks_ADC_SUBVCOGEN:			// Submodule Auxiliary Generator
		SetVcoClkValue(pModule, ClkValue);
		pAdcSrvCfg->SubVcoClk = ROUND(ClkValue);
		ClkValue = pAdcSrvCfg->SubVcoClk;
		break;
	case BRDclks_ADC_EXTCLK:		// External clock
	case BRDclks_ADC_MASTERCLK:
		pAdcSrvCfg->SubExtClk = ROUND(ClkValue);
		break;
	default:
//		ClkValue = 0.0;
		break;
	}
	return BRDerr_OK;
}
	
//***************************************************************************************
int CAdc28x800mSrv::GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC28X800MSRV_CFG pAdcSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
	double Clk;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:			// disabled clock
		Clk = 0.0;
		break;
	case BRDclks_ADC_SUBMAINGEN:		// Submodule Main Generator
		Clk = pAdcSrvCfg->SubMainGen;
		break;
	case BRDclks_ADC_SUBVCOGEN:			// Submodule Auxiliary Generator
		Clk = pAdcSrvCfg->SubVcoClk;
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
int M28X800M_CLK_DIVIDER[BRD_CLKDIVCNT] = {1, 2, 4, 8, 16, 32};

//****************************************************************************************
int SetClkDivOpt(double RateHz, double curClk, double minRate)
{
	if(!curClk)
		return 1;
	int MaxDivider = (int)floor(curClk / minRate);
	int i = 0;
	for(i = BRD_CLKDIVCNT-1; i >= 0; i--)
		if(MaxDivider >= M28X800M_CLK_DIVIDER[i])
			break;
	int MaxInd = (i < 0) ? 0 : i;
	MaxDivider = M28X800M_CLK_DIVIDER[MaxInd];
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
				return M28X800M_CLK_DIVIDER[MaxInd];
			}
		}
		else 
		{
			for(int i = 0; i < MaxInd; i++) 
			{
				double RateLeft = curClk / M28X800M_CLK_DIVIDER[i];
				double RateRight = curClk / M28X800M_CLK_DIVIDER[i+1];
				if(RateHz <= RateLeft && RateHz >= RateRight)
				{
					if(RateLeft - RateHz <= RateHz - RateRight)
					{
						if (DeltaFreq > RateLeft - RateHz)
						{
							DeltaFreq = RateLeft - RateHz;
							return M28X800M_CLK_DIVIDER[i];
						}
					}
					else
					{
						if (DeltaFreq > RateHz - RateRight)
						{
							DeltaFreq = RateHz - RateRight;
							return M28X800M_CLK_DIVIDER[i+1];
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
// NOTE: не учитывается режим SLAVE
//***************************************************************************************
int CAdc28x800mSrv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
	ULONG AdcRateDivider;
	ULONG AuxDivMask;
	if(ClkSrc == BRDclks_ADC_SUBMAINGEN || ClkSrc == BRDclks_ADC_SUBVCOGEN)
	{
		DeltaFreq = 1.e10;
		PADC28X800MSRV_CFG pSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
		AdcRateDivider = SetClkDivOpt(Rate, ClkValue, pSrvCfg->AdcCfg.MinRate);
	}
	else
		AdcRateDivider = 1;

	if(AdcRateDivider > 16)
	{
		AdcRateDivider = 16;
		AuxDivMask = 1; // second cascad divider on
	}
	else
	{
		AuxDivMask = 0; // second cascad divider off
	}
	if(BRDclks_ADC_MASTERCLK == ClkSrc)
	{
		if(Rate > (ClkValue / 2 + ClkValue / 4))
			AuxDivMask = 0; // second cascad divider off
		else
			AuxDivMask = 1; // second cascad divider on
	}
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_FDVR;
	PADC_FDVR pFreqDvr = (PADC_FDVR)&param.val;
	int pow = powof2(AdcRateDivider);
	pFreqDvr->ByBits.Div = powof2(AdcRateDivider);
	pFreqDvr->ByBits.AuxDiv = AuxDivMask;
//	param.val = powof2(AdcRateDivider);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Rate = ClkValue / AdcRateDivider;	// first cascad divider
	Rate = AuxDivMask ? (Rate / 2) : Rate;	// second cascad divider
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::GetRate(CModule* pModule, double& Rate, double ClkValue)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_FDVR;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_FDVR pFreqDvr = (PADC_FDVR)&param.val;
	ULONG AdcRateDivider = (ULONG)pow(2., (int)pFreqDvr->ByBits.Div);
//	ULONG AdcRateDivider = (ULONG)pow(2, param.val);
	Rate = ClkValue / AdcRateDivider;	// first cascad divider
	Rate = pFreqDvr->ByBits.AuxDiv ? (Rate / 2) : Rate;	// second cascad divider
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::GetGainMask(CModule* pModule, int chan)
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
/*
//***************************************************************************************
int CAdc28x800mSrv::SetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	double inp_range;
	Status = GetInpRange(pModule, inp_range, Chan); 
	if(Status != BRDerr_OK)
		return Status;
	int max_dac_value = 255;
	int min_dac_value = 0;
	double half_dac_value = 128.;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	//param.reg = ADM2IFnr_STATUS;
	//PADM2IF_STATUS pStatus;
	//do {
	//	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//	pStatus = (PADM2IF_STATUS)&param.val;
	//} while(!pStatus->ByBits.CmdRdy);
	if(WaitCmdReady(pModule, m_index, m_MainTetrNum, 1000))
		return BRDerr_WAIT_TIMEOUT;

	USHORT dac_data;
	if(fabs(Bias) > inp_range)
		dac_data = Bias > 0.0 ? max_dac_value : min_dac_value;
	else
		dac_data = (USHORT)floor((Bias / inp_range + 1.) * half_dac_value + 0.5);
	if(dac_data > max_dac_value)
		dac_data = max_dac_value;
	Bias = inp_range * (dac_data / half_dac_value - 1.);

	PADC28x800MSRV_CFG pSrvCfg = (PADC28x800MSRV_CFG)m_pConfig;
	pSrvCfg->Bias[Chan] = Bias;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC28x800M_BIAS0 - 1] = (UCHAR)dac_data;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = Chan + BRDtdn_ADC28x800M_BIAS0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

//***************************************************************************************
int CAdc28x800mSrv::GetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
//	double inp_range = GetInpRange(pModule, Chan); 
	PADC28x800MSRV_CFG pSrvCfg = (PADC28x800MSRV_CFG)m_pConfig;
	//UCHAR dac_data = (UCHAR)pSrvCfg->ThrDac[pValChan->chan + BRDtdn_ADC212x200M_BIAS0];
	//pValChan->value = inp_range * (dac_data / half_dac_value - 1.);
	Bias = pSrvCfg->Bias[Chan];
	return Status;
}

//***************************************************************************************
int CAdc28x800mSrv::SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC28x800MSRV_CFG pSrvCfg = (PADC28x800MSRV_CFG)m_pConfig;
	double max_thr = 10.; // ±10%
//	double max_thr = pSrvCfg->RefPVS / 1000.;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
//	param.reg = ADM2IFnr_STATUS;
//	PADM2IF_STATUS pStatus;
//	do {
//		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
//		pStatus = (PADM2IF_STATUS)&param.val;
//	} while(!pStatus->ByBits.CmdRdy);
//
	if(WaitCmdReady(pModule, m_index, m_MainTetrNum, 1000))
		return BRDerr_WAIT_TIMEOUT;

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

	pSrvCfg->GainTun[Chan] = GainTuning;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC212x200M_GAINTUN0 - 1] = (UCHAR)dac_data;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC212x200M_GAINTUN0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC28x800MSRV_CFG pSrvCfg = (PADC28x800MSRV_CFG)m_pConfig;
//	pValChan->value = pSrvCfg->GainTun;
//	pValChan->chan = 0;
//	pValChan->value = pSrvCfg->ClkPhase[pValChan->chan];
	GainTuning = pSrvCfg->GainTun[Chan];
	return BRDerr_OK;
}
*/
//***************************************************************************************
int CAdc28x800mSrv::SetStartMode(CModule* pModule, PVOID pStartStopMode)
{
	int Status = BRDerr_OK;
	PBRD_AdcStartMode pAdcStartMode = (PBRD_AdcStartMode)pStartStopMode;
	double thr = pAdcStartMode->level;
	Status = SetAdcStartMode(pModule, pAdcStartMode->src, pAdcStartMode->inv, &thr);
	if(Status != BRDerr_OK)
		return Status;
	pAdcStartMode->level = thr;
	PBRD_StartMode pStartMode = &pAdcStartMode->stndStart;
//	PBRD_StartMode pStartMode = (PBRD_StartMode)args;
	//if(pAdcStartMode->startSrc != BRDsts_ADC_PRG)
	//	pStartMode->startSrc = BRDsts_SUBMOD;
	//else
	//	if(pStartMode->startSrc == BRDsts_SUBMOD)
	//		pStartMode->startSrc = BRDsts_PRG;
	Status = CAdcSrv::SetStartMode(pModule, pStartMode);
	return Status;
}

//***************************************************************************************
int CAdc28x800mSrv::GetStartMode(CModule* pModule, PVOID pStartStopMode)
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
//	PBRD_StartMode pStartMode = (PBRD_StartMode)args;
	Status = CAdcSrv::GetStartMode(pModule, pStartMode);
	return Status;
}

//***************************************************************************************
//int CAdc28x800mSrv::SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr)
//{
//	DEVS_CMD_Reg param;
//	param.idxMain = m_index;
//	param.tetr = m_AdcTetrNum;
//	param.reg = ADCnr_CTRL1;
//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
//	pCtrl->ByBits.StartSrc = source;
//	pCtrl->ByBits.StartInv = inv;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//
//	param.tetr = m_MainTetrNum;
//	param.reg = ADM2IFnr_STATUS;
//	PADM2IF_STATUS pStatus;
//	do {
//		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
//		pStatus = (PADM2IF_STATUS)&param.val;
//	} while(!pStatus->ByBits.CmdRdy);
//
//	int max_code = ADC28X800M_STARTCHANMAXCODE;
//	int min_code = ADC28X800M_STARTCHANMINCODE;
//	double max_thr = ADC28X800M_STARTTHRCHAN;
//	if(BRDsts_ADC_EXT == source)
//	{
//		max_code = ADC28X800M_STARTEXTMAXCODE;
//		min_code = ADC28X800M_STARTEXTMINCODE;
//		max_thr = ADC28X800M_STARTTHREXT;
//	}
//	USHORT dac_data;
//	if(fabs(*pCmpThr) > max_thr)
//		dac_data = *pCmpThr > 0.0 ? max_code : min_code;
//	else
//		dac_data = (USHORT)floor((*pCmpThr / max_thr + 1.) * 128. + 0.5);
//	if(dac_data > max_code)
//		dac_data = max_code;
//	*pCmpThr = max_thr * (dac_data / 128. - 1.);
//
//	PADC28X800MSRV_CFG pSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
//	pSrvCfg->StCmpThr = *pCmpThr;
//	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC28x800M_STARTCMP - 1] = (UCHAR)dac_data;
//	param.reg = MAINnr_THDAC;
//	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
//	pThDac->ByBits.Data = dac_data;
//	pThDac->ByBits.Num = BRDtdn_ADC28x800M_STARTCMP;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	return BRDerr_OK;
//}

//***************************************************************************************
int CAdc28x800mSrv::SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr)
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

	double inp_range = 2.5;
	if(BRDsts_ADC_CHAN0 == source)
		GetInpRange(pModule, inp_range, 0); 
	if(BRDsts_ADC_CHAN1 == source)
		GetInpRange(pModule, inp_range, 1); 
	double value = *pCmpThr;
	int max_code = ADC28X800M_STARTEXTMAXCODE;
	int min_code = ADC28X800M_STARTEXTMINCODE;
	if(BRDsts_ADC_CHAN0 == source || BRDsts_ADC_CHAN1 == source)
	{
		double koef = ADC28X800M_STARTCHANKOEF;
		max_code = ADC28X800M_STARTCHANMAXCODE;
		min_code = ADC28X800M_STARTCHANMINCODE;
		value = *pCmpThr * koef / inp_range;
	}

	PADC28X800MSRV_CFG pSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.;
	USHORT dac_data;
	if(fabs(value) > max_thr)
		dac_data = value > 0.0 ? max_code : min_code;
	else
		dac_data = (USHORT)floor((value / max_thr + 1.) * 128. + 0.5);
	if(dac_data > max_code)
		dac_data = max_code;
	value = max_thr * (dac_data / 128. - 1.);

	*pCmpThr = value;
	if(BRDsts_ADC_CHAN0 == source || BRDsts_ADC_CHAN1 == source)
	{
		double koef = ADC28X800M_STARTCHANKOEF;
		*pCmpThr = value * inp_range / koef;
	}

	pSrvCfg->StCmpThr = *pCmpThr;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC28x800M_STARTCMP - 1] = (UCHAR)dac_data;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC28x800M_STARTCMP;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	*pSource = pCtrl->ByBits.StartSrc;
	*pInv = pCtrl->ByBits.StartInv;
	PADC28X800MSRV_CFG pSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
	*pCmpThr = pSrvCfg->StCmpThr;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::SetGain(CModule* pModule, double& Gain, ULONG Chan)
{
	int Status = BRDerr_OK;
	ULONG mask;
	if(Gain >= BRD_Gain[BRD_GAINCNT - 1])
	{
		if(Gain > BRD_Gain[BRD_GAINCNT - 1])
			Status = BRDerr_ADC_ILLEGAL_GAIN | BRDerr_INFO;
		mask = BRD_GAINCNT - 1;
	}
	else
	{
		if(Gain <= BRD_Gain[0])
		{
			if(Gain < BRD_Gain[0])
				Status = BRDerr_ADC_ILLEGAL_GAIN | BRDerr_INFO;
			mask = 0;
		}
		else 
		{
			for(int i = 0; i < BRD_GAINCNT - 1; i++)
			{
				if(Gain >= BRD_Gain[i] && Gain <= BRD_Gain[i+1])
				{
					if(Gain - BRD_Gain[i] <= BRD_Gain[i+1] - Gain)
						mask = i;

					else
						mask = i+1;
					break;
				}
			}
		}
	}
	Gain = BRD_Gain[mask];
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
int CAdc28x800mSrv::GetGain(CModule* pModule, double& Gain, ULONG Chan)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_GAIN pGain = (PADM2IF_GAIN)&param.val;
	double value;
	switch(Chan)
	{
	case 0:
		value = BRD_Gain[pGain->ByBits.Chan0];
		break;
	case 1:
		value = BRD_Gain[pGain->ByBits.Chan1];
		break;
	case 2:
		value = BRD_Gain[pGain->ByBits.Chan2];
		break;
	case 3:
		value = BRD_Gain[pGain->ByBits.Chan3];
		break;
	}
	Gain = value;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::SetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
{
	int Status = BRDerr_OK;
	ULONG mask;
	if(InpRange <= BRD_Range[BRD_GAINCNT - 1])
	{
		if(InpRange < BRD_Range[BRD_GAINCNT - 1])
			Status = BRDerr_ADC_ILLEGAL_INPRANGE | BRDerr_INFO;
		mask = BRD_GAINCNT - 1;
	}
	else
	{
		if(InpRange >= BRD_Range[0])
		{
			if(InpRange > BRD_Range[0])
				Status = BRDerr_ADC_ILLEGAL_INPRANGE | BRDerr_INFO;
			mask = 0;
		}
		else 
		{
			for(int i = 0; i < BRD_GAINCNT - 1; i++)
			{
				if(InpRange <= BRD_Range[i] && InpRange >= BRD_Range[i+1])
				{
					if(BRD_Range[i] - InpRange <= InpRange - BRD_Range[i+1])
						mask = i;

					else
						mask = i+1;
					break;
				}
			}
		}
	}
	InpRange = BRD_Range[mask];
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
int CAdc28x800mSrv::GetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_GAIN pGain = (PADM2IF_GAIN)&param.val;
	double value;
	switch(Chan)
	{
	case 0:
		value = BRD_Range[pGain->ByBits.Chan0];
		break;
	case 1:
		value = BRD_Range[pGain->ByBits.Chan1];
		break;
	case 2:
		value = BRD_Range[pGain->ByBits.Chan2];
		break;
	case 3:
		value = BRD_Range[pGain->ByBits.Chan3];
		break;
	}
	InpRange = value;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::SetClkThr(CModule* pModule, double& ClkTuning)
{
	double max_thr = BRD_ADC_MAXCLKTUN; // %

	USHORT dac_data;
	if(fabs(ClkTuning) >= max_thr)
		dac_data = ClkTuning > 0.0 ? 65535 : 0;
	else
		dac_data = (USHORT)floor((ClkTuning / max_thr + 1.) * 32768. + 0.5);
//		dac_data = (USHORT)floor((1. - ClkTuning / max_thr) * 128. + 0.5);
	if(dac_data > 65535)
		dac_data = 65535;
	ClkTuning = max_thr * (dac_data / 32768. - 1.);
//	ClkTuning = max_thr * (1. - dac_data / 128.);

	PADC28X800MSRV_CFG pSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
	pSrvCfg->DacMainGen = dac_data;

	ULONG ClkSrc;
	GetClkSource(pModule, ClkSrc);
	if(ClkSrc == BRDclks_ADC_SUBMAINGEN)
	{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADM2IFnr_VCODAC;
		param.val = dac_data;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::GetClkThr(CModule* pModule, double& ClkTuning)
{
	PADC28X800MSRV_CFG pSrvCfg = (PADC28X800MSRV_CFG)m_pConfig;
	double max_thr = BRD_ADC_MAXCLKTUN; // %
	USHORT dac_data = pSrvCfg->DacMainGen;
	ClkTuning = max_thr * (dac_data / 32768. - 1.);
//	int Status = BRDerr_CMD_UNSUPPORTED;
	int Status = BRDerr_OK;
	return Status;
}

//***************************************************************************************
int CAdc28x800mSrv::SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	return Status;
}

//***************************************************************************************
int CAdc28x800mSrv::GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	InpRes = 0;
	return Status;
}

//***************************************************************************************
int CAdc28x800mSrv::SetClkLocation(CModule* pModule, ULONG& mode)
{
	mode = 1;
	CAdcSrv::SetClkLocation(pModule, mode);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x800mSrv::GetClkLocation(CModule* pModule, ULONG& mode)
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

// ***************** End of file Adc28x800mSrv.cpp ***********************
