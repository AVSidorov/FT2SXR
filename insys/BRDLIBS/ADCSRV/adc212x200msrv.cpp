/*
 ***************** File Adc212x200mSrv.cpp ************
 *
 * BRD Driver for ADС service on ADM212x200M
 *
 * (C) InSys by Dorokhin A. Apr 2004
 *
 ******************************************************
*/

#include "module.h"
#include "gipcy.h"
#include "adc212x200msrv.h"

#define	CURRFILE _BRDC("ADC212X200MSRV")

//***************************************************************************************
CAdc212x200mSrv::CAdc212x200mSrv(int idx, int srv_num, CModule* pModule, PADC212X200MSRV_CFG pCfg) :
	CAdcSrv(idx, _BRDC("ADC212X200M"), srv_num, pModule, pCfg, sizeof(ADC212X200MSRV_CFG))
{
}

//***************************************************************************************
int CAdc212x200mSrv::CtrlRelease(
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
void CAdc212x200mSrv::GetAdcTetrNum(CModule* pModule)
{
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC212X200M_TETR_ID);
}

//***************************************************************************************
void CAdc212x200mSrv::FreeInfoForDialog(PVOID pInfo)
{
	PADC212X200MSRV_INFO pSrvInfo = (PADC212X200MSRV_INFO)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc212x200mSrv::GetInfoForDialog(CModule* pDev)
{
	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	PADC212X200MSRV_INFO pSrvInfo = new ADC212X200MSRV_INFO;
	pSrvInfo->Size = sizeof(ADC212X200MSRV_INFO);
	UCHAR code = pSrvCfg->AdcCfg.Encoding;
	pSrvInfo->pAdcInfo = (PADCSRV_INFO)CAdcSrv::GetInfoForDialog(pDev);
	if(pSrvInfo->pAdcInfo->SyncMode == 3)
		pSrvInfo->pAdcInfo->SyncMode = 1;
	pSrvInfo->pAdcInfo->Encoding = code;
	pSrvInfo->RefGen = pSrvCfg->SubRefGen;
	pSrvInfo->ExtClk = pSrvCfg->SubExtClk;
	GetClkSource(pDev, pSrvInfo->pAdcInfo->ClockSrc);
	GetClkValue(pDev, pSrvInfo->pAdcInfo->ClockSrc, pSrvInfo->pAdcInfo->ClockValue);
	GetRate(pDev, pSrvInfo->pAdcInfo->SamplingRate, pSrvInfo->pAdcInfo->ClockValue);
	if(pSrvInfo->pAdcInfo->ClockSrc == BRDclks_ADC_EXTCLK)
		pSrvInfo->ExtClk = pSrvInfo->pAdcInfo->ClockValue;
	ULONG dbl_mode;
	GetDblClk(pDev, dbl_mode);
	pSrvInfo->DblClockMode = (UCHAR)dbl_mode;
	ULONG inp0_ch1;
	GetInpSrc(pDev, &inp0_ch1);
	pSrvInfo->Inp0Ch1 = (UCHAR)inp0_ch1;
	for(int i = 0; i < MAX_CHAN; i++)
	{
		GetInpRange(pDev, pSrvInfo->pAdcInfo->Range[i], i);
		GetBias(pDev, pSrvInfo->pAdcInfo->Bias[i], i);
//		GetInpResist(pDev, pSrvInfo->pAdcInfo->Resist[i], i);
//		GetDcCoupling(pDev, pSrvInfo->pAdcInfo->DcCoupling[i], i);
		GetClkPhase(pDev, pSrvInfo->PhaseTuning[i], i);
	}
	GetGainTuning(pDev, pSrvInfo->GainTuning0, 0);

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
	pSrvInfo->pAdcInfo->ReStart = start_mode.stndStart.reStartMode;

	return pSrvInfo;
}

//***************************************************************************************
int CAdc212x200mSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
//	TCHAR msg[256];
	CModule* pModule = (CModule*)pDev;
	PADC212X200MSRV_INFO pInfo = (PADC212X200MSRV_INFO)args;
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

	PADC212X200MSRV_CFG pAdcSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	pAdcSrvCfg->SubExtClk = ROUND(pInfo->ExtClk);
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
//	sprintf(msg, _T("ClockValue = %f, SamplingRate = %f"), pInfo->pAdcInfo->ClockValue, pInfo->pAdcInfo->SamplingRate);
//	MessageBox(NULL, msg, "Debug", MB_OK);
	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
//	sprintf(msg, _T("ClockValue = %f, SamplingRate = %f"), pInfo->pAdcInfo->ClockValue, pInfo->pAdcInfo->SamplingRate);
//	MessageBox(NULL, msg, "Debug", MB_OK);
	ULONG dbl_mode = (ULONG)pInfo->DblClockMode;
	SetDblClk(pModule, dbl_mode);
	pInfo->DblClockMode = (UCHAR)dbl_mode;
	ULONG inp0_ch1 = pInfo->Inp0Ch1;
	SetInpSrc(pModule, &inp0_ch1);
	for(int i = 0; i < MAX_CHAN; i++)
	{
		SetInpRange(pModule, pInfo->pAdcInfo->Range[i], i);
		SetBias(pModule, pInfo->pAdcInfo->Bias[i], i);
		SetInpResist(pModule, pInfo->pAdcInfo->Resist[i], i);
		SetDcCoupling(pModule, pInfo->pAdcInfo->DcCoupling[i], i);
		SetClkPhase(pModule, pInfo->PhaseTuning[i], i);
	}
	SetGainTuning(pModule, pInfo->GainTuning0, 0);
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

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	BRDCHAR Buffer[128];
	BRDCHAR ParamName[128];
	BRDCHAR* endptr;
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);

	IPC_getPrivateProfileString(SectionName, _BRDC("ClockSource"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
//	ULONG clkSrc = atoi(Buffer);
	ULONG clkSrc = BRDC_strtol(Buffer, &endptr, 0);
	SetClkSource(pDev, clkSrc);
	IPC_getPrivateProfileString(SectionName, _BRDC("ExternalClockValue"), _BRDC("120000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	double clkValue = BRDC_atof(Buffer);
	PADC212X200MSRV_CFG pAdcSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	pAdcSrvCfg->SubExtClk = ROUND(clkValue);
	SetClkValue(pDev, clkSrc, clkValue);
	IPC_getPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("100000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	double rate = BRDC_atof(Buffer);
	SetRate(pDev, rate, clkSrc, clkValue);

	IPC_getPrivateProfileString(SectionName, _BRDC("DoubleClockingMode"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	ULONG dbl_mode = BRDC_atoi(Buffer);
	SetDblClk(pDev, dbl_mode);
//	SetSrcClk(pDev, dbl_mode);

	IPC_getPrivateProfileString(SectionName, _BRDC("ChannelOneSource"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	ULONG inp0_ch1 = BRDC_atoi(Buffer);;
	SetInpSrc(pDev, &inp0_ch1);

	double InpRange[MAX_CHAN], Bias[MAX_CHAN], PhaseTuning[MAX_CHAN];
	for(int i = 0; i < MAX_CHAN; i++)
	{
		BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
		IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.5"), Buffer, sizeof(Buffer), iniFilePath);
		InpRange[i] = BRDC_atof(Buffer);
		SetInpRange(pDev, InpRange[i], i);
		BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
		IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		Bias[i] = BRDC_atof(Buffer);
		SetBias(pDev, Bias[i], i);
		BRDC_sprintf(ParamName, _BRDC("ClockPhaseTuning%d"), i);
		GetPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		PhaseTuning[i] = BRDC_atof(Buffer);
		SetClkPhase(pDev, PhaseTuning[i], i);
	}
	GetPrivateProfileString(SectionName, _BRDC("GainTuning0"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	double GainTuning = BRDC_atof(Buffer);
	SetGainTuning(pDev, GainTuning, 0);

	BRD_AdcStartMode start;
	IPC_getPrivateProfileString(SectionName, _BRDC("StartSource"), _BRDC("3"), Buffer, sizeof(Buffer), iniFilePath);
	start.src = BRDC_atoi(Buffer);
	IPC_getPrivateProfileString(SectionName, _BRDC("StartInverting"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	start.inv = BRDC_atoi(Buffer);//0;
	IPC_getPrivateProfileString(SectionName, _BRDC("StartLevel"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	start.level = BRDC_atof(Buffer);//0.0;
	if(start.src != BRDsts_ADC_PRG)
		start.stndStart.startSrc = BRDsts_SUBMOD;
	else
		start.stndStart.startSrc = BRDsts_PRG;
	start.stndStart.startInv = 0;
	start.stndStart.trigOn = 1;
	start.stndStart.trigStopSrc = BRDsts_PRG;
	start.stndStart.stopInv = 0;
	IPC_getPrivateProfileString(SectionName, _BRDC("ReStart"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	start.stndStart.reStartMode = BRDC_atoi(Buffer);
	SetStartMode(pDev, &start);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	BRDCHAR Buffer[128];
	BRDCHAR ParamName[128];
	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);
	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	BRDC_sprintf(Buffer, _BRDC("%u"), pSrvCfg->AdcCfg.Encoding);
	IPC_writePrivateProfileString(SectionName, _BRDC("CodeType"), Buffer, iniFilePath);

	ULONG master;
	GetMaster(pDev, master);
	if(master == 3) master = 1;
	BRDC_sprintf(Buffer, _BRDC("%u"), master);
	IPC_writePrivateProfileString(SectionName, _BRDC("MasterMode"), Buffer, iniFilePath);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	BRDC_sprintf(Buffer, _BRDC("%u"), clkSrc);
	IPC_writePrivateProfileString(SectionName, _BRDC("ClockSource"), Buffer, iniFilePath);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
	if(clkSrc == BRDclks_ADC_EXTCLK)
	{
		BRDC_sprintf(Buffer, _BRDC("%.2f"), clkValue);
		IPC_writePrivateProfileString(SectionName, _BRDC("ExternalClockValue"), Buffer, iniFilePath);
	}
	double rate;
	GetRate(pDev, rate, clkValue);
	BRDC_sprintf(Buffer, _BRDC("%.2f"), rate);
	IPC_writePrivateProfileString(SectionName, _BRDC("SamplingRate"), Buffer, iniFilePath);

	ULONG dbl_mode;
	GetDblClk(pDev, dbl_mode);
//	GetSrcClk(pDev, dbl_mode);
	BRDC_sprintf(Buffer, _BRDC("%u"), dbl_mode);
	IPC_writePrivateProfileString(SectionName, _BRDC("DoubleClockingMode"), Buffer, iniFilePath);

	ULONG inp0_ch1;
	GetInpSrc(pDev, &inp0_ch1);
	BRDC_sprintf(Buffer, _BRDC("%u"), inp0_ch1);
	IPC_writePrivateProfileString(SectionName, _BRDC("ChannelOneSource"), Buffer, iniFilePath);

	double InpRange[MAX_CHAN], Bias[MAX_CHAN], PhaseTuning[MAX_CHAN];
	for(int i = 0; i < MAX_CHAN; i++)
	{
		GetInpRange(pDev, InpRange[i], i);
		BRDC_sprintf(Buffer, _BRDC("%.2f"), InpRange[i]);
		BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
		IPC_writePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		GetBias(pDev, Bias[i], i);
		BRDC_sprintf(Buffer, _BRDC("%.4f"), Bias[i]);
		BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
		IPC_writePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		GetClkPhase(pDev, PhaseTuning[i], i);
		BRDC_sprintf(Buffer, _BRDC("%.4f"), PhaseTuning[i]);
		BRDC_sprintf(ParamName, _BRDC("ClockPhaseTuning%d"), i);
		IPC_writePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
	}
	double GainTuning;
	GetGainTuning(pDev, GainTuning, 0);
	BRDC_sprintf(Buffer, _BRDC("%.4f"), GainTuning);
	IPC_writePrivateProfileString(SectionName, _BRDC("GainTuning0"), Buffer, iniFilePath);

	BRD_AdcStartMode start;
	GetStartMode(pDev, &start);
	BRDC_sprintf(Buffer, _BRDC("%u"), start.src);
	IPC_writePrivateProfileString(SectionName, _BRDC("StartSource"), Buffer, iniFilePath);
	BRDC_sprintf(Buffer, _BRDC("%u"), start.inv);
	IPC_writePrivateProfileString(SectionName, _BRDC("StartInverting"), Buffer, iniFilePath);
	BRDC_sprintf(Buffer, _BRDC("%.2f"), start.level);
	IPC_writePrivateProfileString(SectionName, _BRDC("StartLevel"), Buffer, iniFilePath);
	BRDC_sprintf(Buffer, _BRDC("%u"), start.stndStart.startSrc);
	IPC_writePrivateProfileString(SectionName, _BRDC("StartBaseSource"), Buffer, iniFilePath);
	BRDC_sprintf(Buffer, _BRDC("%u"), start.stndStart.startInv);
	IPC_writePrivateProfileString(SectionName, _BRDC("StartBaseInverting"), Buffer, iniFilePath);
	BRDC_sprintf(Buffer, _BRDC("%u"), start.stndStart.trigOn);
	IPC_writePrivateProfileString(SectionName, _BRDC("StartMode"), Buffer, iniFilePath);
	BRDC_sprintf(Buffer, _BRDC("%u"), start.stndStart.trigStopSrc);
	IPC_writePrivateProfileString(SectionName, _BRDC("StopSource"), Buffer, iniFilePath);
	BRDC_sprintf(Buffer, _BRDC("%u"), start.stndStart.stopInv);
	IPC_writePrivateProfileString(SectionName, _BRDC("StopInverting"), Buffer, iniFilePath);
	BRDC_sprintf(Buffer, _BRDC("%u"), start.stndStart.reStartMode);
	IPC_writePrivateProfileString(SectionName, _BRDC("ReStart"), Buffer, iniFilePath);

	// the function flushes the cache
	IPC_writePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::SetClkSource(CModule* pModule, ULONG ClkSrc)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
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
		else
			return BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
{
	ULONG source;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
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
int CAdc212x200mSrv::SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC212X200MSRV_CFG pAdcSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:		// disabled clock
		ClkValue = 0.0;
		break;
	case BRDclks_ADC_SUBGEN:		// Submodule generator
		ClkValue = pAdcSrvCfg->SubRefGen;
		break;
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
int CAdc212x200mSrv::GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC212X200MSRV_CFG pAdcSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	double Clk;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:		// disabled clock
		Clk = 0.0;
		break;
	case BRDclks_ADC_SUBGEN:		// Submodule generator
		Clk = pAdcSrvCfg->SubRefGen;
		break;
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
	
static double DeltaFreq;
int M212x200M_CLK_DIVIDER[BRD_CLKDIVCNT] = {1, 2, 4, 8};

//****************************************************************************************
int SetClkDivOpt(double RateHz, double curClk, double minRate)
{
	if(!curClk)
		return 1;
	int MaxDivider = (int)floor(curClk / minRate);
	int i = 0;
	for(i = BRD_CLKDIVCNT-1; i >= 0; i--)
		if(MaxDivider >= M212x200M_CLK_DIVIDER[i])
			break;
	int MaxInd = (i < 0) ? 0 : i;
	MaxDivider = M212x200M_CLK_DIVIDER[MaxInd];
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
				return M212x200M_CLK_DIVIDER[MaxInd];
			}
		}
		else 
		{
			for(int i = 0; i < MaxInd; i++) 
			{
				double RateLeft = curClk / M212x200M_CLK_DIVIDER[i];
				double RateRight = curClk / M212x200M_CLK_DIVIDER[i+1];
				if(RateHz <= RateLeft && RateHz >= RateRight)
				{
					if(RateLeft - RateHz <= RateHz - RateRight)
					{
						if (DeltaFreq > RateLeft - RateHz)
						{
							DeltaFreq = RateLeft - RateHz;
							return M212x200M_CLK_DIVIDER[i];
						}
					}
					else
					{
						if (DeltaFreq > RateHz - RateRight)
						{
							DeltaFreq = RateHz - RateRight;
							return M212x200M_CLK_DIVIDER[i+1];
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
int CAdc212x200mSrv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
	ULONG AdcRateDivider;
	if(ClkSrc == BRDclks_ADC_SUBGEN)
	{
		DeltaFreq = 1.e10;
		PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
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
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::GetRate(CModule* pModule, double& Rate, double ClkValue)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_FDVR;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	ULONG AdcRateDivider = (ULONG)pow(2., (int)param.val);
	Rate = ClkValue / AdcRateDivider;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::GetGainMask(CModule* pModule, int chan)
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
int CAdc212x200mSrv::SetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	double inp_range;
	Status = GetInpRange(pModule, inp_range, Chan); 
	if(Status != BRDerr_OK)
		return Status;
//	double value = pValChan->value;
	int max_dac_value = 192;
	int min_dac_value = 64;
	double half_dac_value = 128.;
	int gain_chan = GetGainMask(pModule, Chan);
	if(gain_chan & 1)
		inp_range *= 2;
	else
	{
		max_dac_value = 255;
		min_dac_value = 0;
	}
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	//param.tetr = m_AdcTetrNum;
	param.tetr = m_MainTetrNum;
	//param.reg = ADM2IFnr_STATUS;
	//PADM2IF_STATUS pStatus;
	//do {
	//	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//	pStatus = (PADM2IF_STATUS)&param.val;
	//} while(!pStatus->ByBits.CmdRdy);

	USHORT dac_data;
	if(fabs(Bias) > inp_range)
		dac_data = Bias > 0.0 ? max_dac_value : min_dac_value;
	else
		dac_data = (USHORT)floor((Bias / inp_range + 1.) * half_dac_value + 0.5);
	if(dac_data > max_dac_value)
		dac_data = max_dac_value;
	Bias = inp_range * (dac_data / half_dac_value - 1.);

	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	pSrvCfg->Bias[Chan] = Bias;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC212X200M_BIAS0 - 1] = (UCHAR)dac_data;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = Chan + BRDtdn_ADC212X200M_BIAS0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

//***************************************************************************************
int CAdc212x200mSrv::GetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
//	double inp_range = GetInpRange(pModule, Chan); 
	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	//UCHAR dac_data = (UCHAR)pSrvCfg->ThrDac[pValChan->chan + BRDtdn_ADC212x200M_BIAS0];
	//pValChan->value = inp_range * (dac_data / half_dac_value - 1.);
	Bias = pSrvCfg->Bias[Chan];
	return Status;
}

//***************************************************************************************
int CAdc212x200mSrv::SetClkPhase(CModule* pModule, double& Phase, ULONG Chan)
{
	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.;

	//if(WaitCmdReady(pModule, m_index, m_MainTetrNum, 1000))
	//	return BRDerr_WAIT_TIMEOUT;

	USHORT dac_data;
	if(fabs(Phase) > max_thr)
		dac_data = Phase > 0.0 ? 255 : 0;
	else
		dac_data = (USHORT)floor((Phase / max_thr + 1.) * 128. + 0.5);
	if(dac_data > 255)
		dac_data = 255;
	Phase = max_thr * (dac_data / 128. - 1.);

	pSrvCfg->ClkPhase[Chan] = Phase;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC212X200M_CLKPHASE0 - 1] = (UCHAR)dac_data;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = Chan + BRDtdn_ADC212X200M_CLKPHASE0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::GetClkPhase(CModule* pModule, double& Phase, ULONG Chan)
{
	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
//	pValChan->value = pSrvCfg->ClkPhase[pValChan->chan];
	Phase = pSrvCfg->ClkPhase[Chan];
	return BRDerr_OK;
}

const double BRD_ADC_MAXGAINTUN	= 2.; // max gain tuning = 2%
//***************************************************************************************
int CAdc212x200mSrv::SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	double max_thr = BRD_ADC_MAXGAINTUN; // %
//	double max_thr = pSrvCfg->RefPVS / 1000.;

	//if(WaitCmdReady(pModule, m_index, m_MainTetrNum, 1000))
	//	return BRDerr_WAIT_TIMEOUT;

	USHORT dac_data;
	if(fabs(GainTuning) > max_thr)
		dac_data = GainTuning > 0.0 ? 0 : 255;
	else
//		dac_data = (USHORT)floor((GainTuning / max_thr + 1.) * 128. + 0.5);
		dac_data = (USHORT)floor((1. - GainTuning / max_thr) * 128. + 0.5);
	if(dac_data > 255)
		dac_data = 255;
//	GainTuning = max_thr * (dac_data / 128. - 1.);
	GainTuning = max_thr * (1. - dac_data / 128.);

	pSrvCfg->GainTun = GainTuning;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC212X200M_GAINTUN0 - 1] = (UCHAR)dac_data;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC212X200M_GAINTUN0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
//	pValChan->value = pSrvCfg->GainTun;
//	pValChan->chan = 0;
//	pValChan->value = pSrvCfg->ClkPhase[pValChan->chan];
	GainTuning = pSrvCfg->GainTun;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::SetStartMode(CModule* pModule, PVOID pStartStopMode)
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

	ULONG mode;
	CAdcSrv::GetMaster(pModule, mode);
	if(mode)
	{
		// для вывода сигнала старта на разъем SYNX
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_MainTetrNum;
		param.reg = ADM2IFnr_MODE0;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
		pMode0->ByBits.Start = 1;
		pMode0->ByBits.AdmClk = 1;
		pMode0->ByBits.Master = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.reg = ADM2IFnr_STMODE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADM2IF_STMODE pStMode = (PADM2IF_STMODE)&param.val;
		pStMode->ByBits.SrcStart = BRDsts_TRDADC;
		pStMode->ByBits.TrigStart = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	return Status;
}

//***************************************************************************************
int CAdc212x200mSrv::GetStartMode(CModule* pModule, PVOID pStartStopMode)
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
int CAdc212x200mSrv::SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr)
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
	int gain_chan = 0;
	if(BRDsts_ADC_CHAN0 == source)
	{
		gain_chan = GetGainMask(pModule, 0);
		GetInpRange(pModule, inp_range, 0); 
	}
	if(BRDsts_ADC_CHAN1 == source)
	{
		gain_chan = GetGainMask(pModule, 1);
		GetInpRange(pModule, inp_range, 1); 
	}
	double value;
	int max_code = 255;
	int min_code = 0;
	switch(source)
	{
	case BRDsts_ADC_CHAN0:
	case BRDsts_ADC_CHAN1:
	{
//		double limit = (gain_chan & 1) ? ADC212x200M_STARTTHRCHAN : ADC212x200M_STARTTHREXT; 
		double koef = ADC212X200M_STARTCHANKOEF;
		max_code = ADC212X200M_STARTCHANMAXCODE;
		min_code = ADC212X200M_STARTCHANMINCODE;
		if(gain_chan & 1)
		{
//			max_code >>= 1;
//			min_code >>= 1;
			max_code = ((ADC212X200M_STARTCHANMAXCODE - 128) >> 1) + 128;
			min_code = 128 - ((128 - ADC212X200M_STARTCHANMINCODE) >> 1);
			koef /= 2;
		}
//		value = *pCmpThr *  limit / inp_range;
		value = *pCmpThr * (0.5 * koef) / inp_range;
//		value = *pCmpThr * koef / inp_range;
	}
		break;
	case BRDsts_ADC_EXT:
		value = *pCmpThr / 2;
		max_code = ADC212X200M_STARTEXTMAXCODE;
		min_code = ADC212X200M_STARTEXTMINCODE;
		break;
	case BRDsts_ADC_PRG:
		value = *pCmpThr;
		break;
	}

	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.;
	USHORT dac_data;
	if(fabs(value) > max_thr)
		dac_data = value > 0.0 ? max_code : min_code;
	else
		dac_data = (USHORT)floor((value / max_thr + 1.) * 128. + 0.5);
	if(dac_data > max_code)
		dac_data = max_code;
	value = max_thr * (dac_data / 128. - 1.);

	switch(source)
	{
	case BRDsts_ADC_CHAN0:
	case BRDsts_ADC_CHAN1:
	{
//		double limit = (gain_chan & 1) ? ADC212x200M_STARTTHRCHAN : ADC212x200M_STARTTHREXT; 
		double koef = ADC212X200M_STARTCHANKOEF;
		max_code = ADC212X200M_STARTCHANMAXCODE;
		min_code = ADC212X200M_STARTCHANMINCODE;
		if(gain_chan & 1)
		{
			max_code >>= 1;
			min_code >>= 1;
			koef /= 2;
		}
//		*pCmpThr = value * inp_range / limit;
		*pCmpThr = value * inp_range / (koef * 0.5);
	}
		break;
	case BRDsts_ADC_EXT:
		*pCmpThr = value * 2;
		max_code = ADC212X200M_STARTEXTMAXCODE;
		min_code = ADC212X200M_STARTEXTMINCODE;
		break;
	case BRDsts_ADC_PRG:
		*pCmpThr = value;
		break;
	}

//	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	pSrvCfg->StCmpThr = *pCmpThr;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC212X200M_STARTCMP - 1] = (UCHAR)dac_data;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC212X200M_STARTCMP;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//int CAdc212x200mSrv::SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr)
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
//	int gain_chan = 0;
//	if(BRDsts_ADC_CHAN0 == source)
//		gain_chan = GetGainMask(pModule, 0);
//	if(BRDsts_ADC_CHAN1 == source)
//		gain_chan = GetGainMask(pModule, 1);
//	int max_code = ADC212x200M_STARTCHANMAXCODE;
//	int min_code = ADC212x200M_STARTCHANMINCODE;
////	double max_thr = ADC212x200M_STARTTHRCHAN;
//	if(BRDsts_ADC_EXT == source || !(gain_chan & 1))
//	{
//		max_code = ADC212x200M_STARTEXTMAXCODE;
//		min_code = ADC212x200M_STARTEXTMINCODE;
////		max_thr = ADC212x200M_STARTTHREXT;
//	}
//	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
//	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.;
//	USHORT dac_data;
//	if(fabs(*pCmpThr) > max_thr)
//		dac_data = *pCmpThr > 0.0 ? max_code : min_code;
//	else
//		dac_data = (USHORT)floor((*pCmpThr / max_thr + 1.) * 128. + 0.5);
//	if(dac_data > max_code)
//		dac_data = max_code;
//	*pCmpThr = max_thr * (dac_data / 128. - 1.);
//
////	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
//	pSrvCfg->StCmpThr = *pCmpThr;
//	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC212x200M_STARTCMP - 1] = (UCHAR)dac_data;
//	param.reg = MAINnr_THDAC;
//	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
//	pThDac->ByBits.Data = dac_data;
//	pThDac->ByBits.Num = BRDtdn_ADC212x200M_STARTCMP;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	return BRDerr_OK;
//}

//int CAdc212x200mSrv::SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr)
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
//	double max_thr = ADC212x200M_STARTTHR;
//	USHORT dac_data;
//	if(fabs(*pCmpThr) > max_thr)
//		dac_data = *pCmpThr > 0.0 ? 255 : 0;
//	else
//		dac_data = (USHORT)floor((*pCmpThr / max_thr + 1.) * 128. + 0.5);
//	if(dac_data > 255)
//		dac_data = 255;
//	*pCmpThr = max_thr * (dac_data / 128. - 1.);
//
//	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
//	pSrvCfg->StCmpThr = *pCmpThr;
//	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC212x200M_STARTCMP - 1] = (UCHAR)dac_data;
//	param.reg = MAINnr_THDAC;
//	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
//	pThDac->ByBits.Data = dac_data;
//	pThDac->ByBits.Num = BRDtdn_ADC212x200M_STARTCMP;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	return BRDerr_OK;
//}
//***************************************************************************************
int CAdc212x200mSrv::GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	*pSource = pCtrl->ByBits.StartSrc;
	*pInv = pCtrl->ByBits.StartInv;
	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	*pCmpThr = pSrvCfg->StCmpThr;
	return BRDerr_OK;
}

//***************************************************************************************
// заменяет функцию установки источника тактовой частоты на функцию удвоения тактовой частоты
int CAdc212x200mSrv::SetDblClk(CModule* pModule, ULONG& mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.DblClk = mode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::GetDblClk(CModule* pModule, ULONG& mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	mode = pCtrl->ByBits.DblClk;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::SetClkLocation(CModule* pModule, ULONG& mode)
{
	mode = 1;
	CAdcSrv::SetClkLocation(pModule, mode);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::GetClkLocation(CModule* pModule, ULONG& mode)
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

//***************************************************************************************
int CAdc212x200mSrv::SetInpSrc(CModule* pModule, PVOID pCotrol)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.Chan1Src = *(PULONG)pCotrol;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::GetInpSrc(CModule* pModule, PVOID pCotrol)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	*(PULONG)pCotrol = pCtrl->ByBits.Chan1Src;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::SetGain(CModule* pModule, double& Gain, ULONG Chan)
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
int CAdc212x200mSrv::GetGain(CModule* pModule, double& Gain, ULONG Chan)
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
int CAdc212x200mSrv::SetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
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
int CAdc212x200mSrv::GetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
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
int CAdc212x200mSrv::SetCode(CModule* pModule, ULONG type)
{
	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;
	//param.tetr = m_AdcTetrNum;
	//param.reg = ADM2IFnr_FORMAT;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//PADM2IF_FORMAT pFormat = (PADM2IF_FORMAT)&param.val;
	//pFormat->ByBits.Code = type;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::GetCode(CModule* pModule, ULONG& type)
{
	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;
	//param.tetr = m_AdcTetrNum;
	//param.reg = ADM2IFnr_FORMAT;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//PADM2IF_FORMAT pFormat = (PADM2IF_FORMAT)&param.val;
	//type = pFormat->ByBits.Code;

	PADC212X200MSRV_CFG pSrvCfg = (PADC212X200MSRV_CFG)m_pConfig;
	type = pSrvCfg->AdcCfg.Encoding;

//	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x200mSrv::SetMaster(CModule* pModule, ULONG mode)
{
	CAdcSrv::SetMaster(pModule, mode);
	if(mode)
	{
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
	}
	return BRDerr_OK;
}

// ***************** End of file Adc212x200mSrv.cpp ***********************
