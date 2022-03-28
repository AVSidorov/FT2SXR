/*
 ***************** File Adc216x100mSrv.cpp ************
 *
 * BRD Driver for ADС service on ADM216x100M
 *
 * (C) InSys by Dorokhin A. Jan 2007
 *
 ******************************************************
*/

#include "module.h"
#include "adc216x100msrv.h"

#define	CURRFILE _BRDС("ADC216X100MSRV")

const double BRD_ADC_MAXGAINTUN	= 2.; // max gain tuning = 2%
//***************************************************************************************
CAdc216x100mSrv::CAdc216x100mSrv(int idx, int srv_num, CModule* pModule, PADC216X100MSRV_CFG pCfg) :
	CAdcSrv(idx, _BRDC("ADC216X100M"), srv_num, pModule, pCfg, sizeof(ADC216X100MSRV_CFG))
{
}

//***************************************************************************************
int CAdc216x100mSrv::CtrlRelease(
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
void CAdc216x100mSrv::GetAdcTetrNum(CModule* pModule)
{
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC216X100M_TETR_ID);
}

//***************************************************************************************
void CAdc216x100mSrv::FreeInfoForDialog(PVOID pInfo)
{
	PADC216X100MSRV_INFO pSrvInfo = (PADC216X100MSRV_INFO)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc216x100mSrv::GetInfoForDialog(CModule* pDev)
{
	PADC216X100MSRV_CFG pSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;
	PADC216X100MSRV_INFO pSrvInfo = new ADC216X100MSRV_INFO;
	pSrvInfo->Size = sizeof(ADC216X100MSRV_INFO);
	UCHAR code = pSrvCfg->AdcCfg.Encoding;
	pSrvInfo->pAdcInfo = (PADCSRV_INFO)CAdcSrv::GetInfoForDialog(pDev);
	if(pSrvInfo->pAdcInfo->SyncMode == 3)
		pSrvInfo->pAdcInfo->SyncMode = 1;
	pSrvInfo->pAdcInfo->Encoding = code;
	pSrvInfo->RefGen[0] = pSrvCfg->SubRefGen[0];
	pSrvInfo->RefGen[1] = pSrvCfg->SubRefGen[1];
	pSrvInfo->RefGen[2] = pSrvCfg->SubRefGen[2];
	pSrvInfo->ExtClk = pSrvCfg->SubExtClk;
	GetClkSource(pDev, pSrvInfo->pAdcInfo->ClockSrc);
	GetClkValue(pDev, pSrvInfo->pAdcInfo->ClockSrc, pSrvInfo->pAdcInfo->ClockValue);
	GetRate(pDev, pSrvInfo->pAdcInfo->SamplingRate, pSrvInfo->pAdcInfo->ClockValue);
	if(pSrvInfo->pAdcInfo->ClockSrc == BRDclks_ADC_EXTCLK)
		pSrvInfo->ExtClk = pSrvInfo->pAdcInfo->ClockValue;
	for(int i = 0; i < MAX_CHAN; i++)
	{
		GetInpRange(pDev, pSrvInfo->pAdcInfo->Range[i], i);
		GetBias(pDev, pSrvInfo->pAdcInfo->Bias[i], i);
//		GetInpResist(pDev, pSrvInfo->pAdcInfo->Resist[i], i);
//		GetDcCoupling(pDev, pSrvInfo->pAdcInfo->DcCoupling[i], i);
//		GetClkPhase(pDev, pSrvInfo->PhaseTuning[i], i);
//		GetGainTuning(pDev, pSrvInfo->GainTuning[i], i);
	}
	GetClkThr(pDev, pSrvInfo->ThrClk);
	ULONG Dither;
	GetDither(pDev, Dither);
	pSrvInfo->Dither = (UCHAR)Dither;
	return pSrvInfo;
}

//***************************************************************************************
int CAdc216x100mSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
//	BRDCHAR msg[256];
	CModule* pModule = (CModule*)pDev;
	PADC216X100MSRV_INFO pInfo = (PADC216X100MSRV_INFO)args;
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

	BRD_StartMode start_mode;
	start_mode.startSrc = pInfo->pAdcInfo->StartSrc;
	start_mode.startInv = pInfo->pAdcInfo->StartInv;		
	start_mode.trigOn = pInfo->pAdcInfo->StartStopMode;
	start_mode.trigStopSrc = pInfo->pAdcInfo->StopSrc;		
	start_mode.stopInv = pInfo->pAdcInfo->StopInv;
	start_mode.reStartMode = pInfo->pAdcInfo->ReStart;
	CAdcSrv::SetStartMode(pModule, &start_mode);

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

	PADC216X100MSRV_CFG pAdcSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;
	pAdcSrvCfg->SubExtClk = ROUND(pInfo->ExtClk);
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
//	sprintf(msg, _T("ClockValue = %f, SamplingRate = %f"), pInfo->pAdcInfo->ClockValue, pInfo->pAdcInfo->SamplingRate);
//	MessageBox(NULL, msg, "Debug", MB_OK);
	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
//	sprintf(msg, _T("ClockValue = %f, SamplingRate = %f"), pInfo->pAdcInfo->ClockValue, pInfo->pAdcInfo->SamplingRate);
//	MessageBox(NULL, msg, "Debug", MB_OK);
	for(int i = 0; i < MAX_CHAN; i++)
	{
		SetInpRange(pModule, pInfo->pAdcInfo->Range[i], i);
		SetBias(pModule, pInfo->pAdcInfo->Bias[i], i);
		SetInpResist(pModule, pInfo->pAdcInfo->Resist[i], i);
		SetDcCoupling(pModule, pInfo->pAdcInfo->DcCoupling[i], i);
//		SetClkPhase(pModule, pInfo->PhaseTuning[i], i);
	}
	SetClkThr(pModule, pInfo->ThrClk);
	SetDither(pModule, pInfo->Dither);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
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
	PADC216X100MSRV_CFG pAdcSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;
	pAdcSrvCfg->SubExtClk = ROUND(clkValue);
	SetClkValue(pDev, clkSrc, clkValue);
    IPC_getPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("100000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	double rate = BRDC_atof(Buffer);
	SetRate(pDev, rate, clkSrc, clkValue);

	double InpRange[MAX_CHAN], Bias[MAX_CHAN];//, GainTuning[MAX_CHAN];
	for(int i = 0; i < MAX_CHAN; i++)
	{
		BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
        IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.2"), Buffer, sizeof(Buffer), iniFilePath);
		InpRange[i] = BRDC_atof(Buffer);
		SetInpRange(pDev, InpRange[i], i);
		BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
        IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		Bias[i] = BRDC_atof(Buffer);
		SetBias(pDev, Bias[i], i);
		//sprintf_s(ParamName, "GainTuning%d", i);
        //IPC_getPrivateProfileString(SectionName, ParamName, "0.0", Buffer, sizeof(Buffer), iniFilePath);
		//GainTuning[i] = atof(Buffer);
		//SetGainTuning(pDev, GainTuning[i], i);
	}
    IPC_getPrivateProfileString(SectionName, _BRDC("ClockThreshold"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	double ThrClock = BRDC_atof(Buffer);
	SetClkThr(pDev, ThrClock);

    IPC_getPrivateProfileString(SectionName, _BRDC("Dither"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	ULONG Dither = BRDC_atoi(Buffer);
	SetDither(pDev, Dither);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
//	TCHAR Buffer[128];
	BRDCHAR ParamName[128];
	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);
	PADC216X100MSRV_CFG pSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;

	WriteInifileParam(iniFilePath, SectionName, _BRDC("CodeType"), (ULONG)pSrvCfg->AdcCfg.Encoding, 10, NULL);
	//sprintf_s(Buffer, "%u", pSrvCfg->AdcCfg.Encoding);
	//WritePrivateProfileString(SectionName, "CodeType", Buffer, iniFilePath);

	ULONG master;
	GetMaster(pDev, master);
	if(master == 3) master = 1;
	//sprintf_s(Buffer, "%u", master);
	//WritePrivateProfileString(SectionName, "MasterMode", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("MasterMode"), master, 10, NULL);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	//sprintf_s(Buffer, "%u", clkSrc);
	//WritePrivateProfileString(SectionName, "ClockSource", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ClockSource"), clkSrc, 16, NULL);
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
	WriteInifileParam(iniFilePath, SectionName, _BRDC("SamplingRate"), rate, 2, NULL);

	double InpRange[MAX_CHAN], Bias[MAX_CHAN];//, GainTuning[MAX_CHAN];
	for(int i = 0; i < MAX_CHAN; i++)
	{
		GetInpRange(pDev, InpRange[i], i);
		BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
		//sprintf_s(Buffer, "%.2f", InpRange[i]);
		//WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		WriteInifileParam(iniFilePath, SectionName, ParamName, InpRange[i], 2, NULL);
		GetBias(pDev, Bias[i], i);
		BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
		//sprintf_s(Buffer, "%.4f", Bias[i]);
		//WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		WriteInifileParam(iniFilePath, SectionName, ParamName, Bias[i], 4, NULL);
		//GetGainTuning(pDev, GainTuning[i], i);
		//sprintf_s(Buffer, "%.4f", GainTuning[i]);
		//sprintf_s(ParamName, "GainTuning%d", i);
		//WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
	}
	double ThrClock;
	GetClkThr(pDev, ThrClock);
	//sprintf_s(Buffer, "%.4f", ThrClock);
	//WritePrivateProfileString(SectionName, "ClockThreshold", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ClockThreshold"), ThrClock, 4, NULL);

	ULONG Dither;
	GetDither(pDev, Dither);
	//sprintf_s(Buffer, "%d", Dither);
	//WritePrivateProfileString(SectionName, "Dither", Buffer, iniFilePath);
    WriteInifileParam(iniFilePath, SectionName, _BRDC("Dither"), Dither, 10, NULL);

	// the function flushes the cache
    IPC_writePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::SetClkSource(CModule* pModule, ULONG ClkSrc)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_FSRC;
	param.val = ClkSrc;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(!pMode0->ByBits.Master)
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.reg = ADM2IFnr_FMODE;
			param.val = 7;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			param.reg = ADM2IFnr_FDIV;
			param.val = 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		else
			return BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
	}
	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;
	//param.tetr = m_AdcTetrNum;
	//param.reg = ADM2IFnr_MODE0;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	//if(pMode0->ByBits.Master)
	//{ // Single
	//	param.reg = ADM2IFnr_FSRC;
	//	param.val = ClkSrc;
	//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//}
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
	//		return BRDerr_NOT_ACTION; // ������� � ������ Slave �� ���������
	//}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
{
	ULONG source;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_FSRC;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	source = param.val;
/*
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
*/
	ClkSrc = source;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC216X100MSRV_CFG pAdcSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:		// disabled clock
		ClkValue = 0.0;
		break;
		case BRDclks_ADC_SUBGEN1:		// Submodule generator 1
			ClkValue = pAdcSrvCfg->SubRefGen[0];
			break;
		case BRDclks_ADC_SUBGEN2:		// Submodule generator 2
			ClkValue = pAdcSrvCfg->SubRefGen[1];
			break;
		case BRDclks_ADC_SUBGEN3:		// Submodule generator 3
			ClkValue = pAdcSrvCfg->SubRefGen[2];
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
int CAdc216x100mSrv::GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC216X100MSRV_CFG pAdcSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;
	double ClkVal = 0;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:		// disabled clock
		ClkVal = 0.0;
		break;
	case BRDclks_ADC_SUBGEN1:		// Submodule generator
		ClkVal = pAdcSrvCfg->SubRefGen[0];
		break;
	case BRDclks_ADC_SUBGEN2:		// Submodule generator
		ClkVal = pAdcSrvCfg->SubRefGen[1];
		break;
	case BRDclks_ADC_SUBGEN3:		// Submodule generator
		ClkVal = pAdcSrvCfg->SubRefGen[2];
		break;
	case BRDclks_ADC_EXTCLK:		// External clock
		ClkVal = pAdcSrvCfg->SubExtClk;
		break;
	default:
		ClkVal = 0.0;
		break;
	}
	ClkValue = ClkVal;
	return BRDerr_OK;
}
	
static double DeltaFreq;
int M216X100M_CLK_DIVIDER[BRD_CLKDIVCNT] = {1, 2, 4, 8, 16};

//****************************************************************************************
int SetClkDivOpt(double RateHz, double curClk, double minRate)
{
	if(!curClk)
		return 1;
	int MaxDivider = (int)floor(curClk / minRate);
	int i = 0;
	for(i = BRD_CLKDIVCNT-1; i >= 0; i--)
		if(MaxDivider >= M216X100M_CLK_DIVIDER[i])
			break;
	int MaxInd = (i < 0) ? 0 : i;
	MaxDivider = M216X100M_CLK_DIVIDER[MaxInd];
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
				return M216X100M_CLK_DIVIDER[MaxInd];
			}
		}
		else 
		{
			for(int i = 0; i < MaxInd; i++) 
			{
				double RateLeft = curClk / M216X100M_CLK_DIVIDER[i];
				double RateRight = curClk / M216X100M_CLK_DIVIDER[i+1];
				if(RateHz <= RateLeft && RateHz >= RateRight)
				{
					if(RateLeft - RateHz <= RateHz - RateRight)
					{
						if (DeltaFreq > RateLeft - RateHz)
						{
							DeltaFreq = RateLeft - RateHz;
							return M216X100M_CLK_DIVIDER[i];
						}
					}
					else
					{
						if (DeltaFreq > RateHz - RateRight)
						{
							DeltaFreq = RateHz - RateRight;
							return M216X100M_CLK_DIVIDER[i+1];
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
int CAdc216x100mSrv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
	ULONG AdcRateDivider;
	if(ClkSrc != BRDclks_ADC_MASTERCLK)
	{
		DeltaFreq = 1.e10;
		PADC216X100MSRV_CFG pSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;
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
int CAdc216x100mSrv::GetRate(CModule* pModule, double& Rate, double ClkValue)
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
int CAdc216x100mSrv::SetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	double inp_range;
	Status = GetInpRange(pModule, inp_range, Chan); 
	if(Status != BRDerr_OK)
		return Status;
	double full_range = inp_range * ADC216X100M_BIASKOEF;
//	double prec_range = inp_range / 10; // 10% from full range
	double prec_range = full_range / 10; // 10% from full range

	int max_dac_value = 255;
	int min_dac_value = 0;
	//int max_dac_value = 0;
	//int min_dac_value = 255;
	double half_dac_value = 128.;

	USHORT approx_data;
//	if(fabs(Bias) > inp_range)
	if(fabs(Bias) > full_range)
		approx_data = Bias > 0.0 ? min_dac_value : max_dac_value;
	else
		approx_data = (USHORT)floor((Bias / full_range + 1.) * half_dac_value + 0.5);
		//approx_data = (USHORT)floor((1. - Bias / full_range) * half_dac_value + 0.5);
	if(approx_data > max_dac_value)
		approx_data = max_dac_value;
	double approx_bias = full_range * (approx_data / half_dac_value - 1.);
	//double approx_bias = full_range * (1. - approx_data / half_dac_value);
	double delta_bias = Bias - approx_bias;
	approx_data = 256 - approx_data;

	USHORT prec_data;
	if(fabs(delta_bias) > prec_range)
		prec_data = delta_bias > 0.0 ? min_dac_value : max_dac_value;
	else
		prec_data = (USHORT)floor((delta_bias / prec_range + 1.) * half_dac_value + 0.5);
		//prec_data = (USHORT)floor((1. - delta_bias / prec_range) * half_dac_value + 0.5);
	if(prec_data > max_dac_value)
		prec_data = max_dac_value;
	double prec_bias = prec_range * (prec_data / half_dac_value - 1.);
	//double prec_bias = prec_range * (1. - prec_data / half_dac_value);
	prec_data = 256 - prec_data;

	Bias = approx_bias + prec_bias;

	PADC216X100MSRV_CFG pSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;
	pSrvCfg->Bias[Chan] = Bias;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC216X100M_BIAS0 - 1] = (UCHAR)approx_data;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC216X100M_PRECBIAS0 - 1] = (UCHAR)prec_data;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_THDAC;
	param.val = 0;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = approx_data;
	pThDac->ByBits.Num = Chan + BRDtdn_ADC216X100M_BIAS0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	pThDac->ByBits.Data = prec_data;
	pThDac->ByBits.Num = Chan + BRDtdn_ADC216X100M_PRECBIAS0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

//***************************************************************************************
int CAdc216x100mSrv::GetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	PADC216X100MSRV_CFG pSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;
	Bias = pSrvCfg->Bias[Chan];
	return Status;
}

//***************************************************************************************
int CAdc216x100mSrv::SetClkLocation(CModule* pModule, ULONG& mode)
{
	mode = 1;
	CAdcSrv::SetClkLocation(pModule, mode);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::GetClkLocation(CModule* pModule, ULONG& mode)
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
int CAdc216x100mSrv::SetClkThr(CModule* pModule, double& ThrVal)
{
	PADC216X100MSRV_CFG pSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;
	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.;

	//if(WaitCmdReady(pModule, m_index, m_MainTetrNum, 1000))
	//	return BRDerr_WAIT_TIMEOUT;

	USHORT dac_data;
	if(fabs(ThrVal) > max_thr)
		dac_data = ThrVal > 0.0 ? 255 : 0;
	else
		dac_data = (USHORT)floor((ThrVal / max_thr + 1.) * 128. + 0.5);
	if(dac_data > 255)
		dac_data = 255;
	ThrVal = max_thr * (dac_data / 128. - 1.);

	pSrvCfg->ThrClk = ThrVal;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC216X100M_THRCLK - 1] = (UCHAR)dac_data;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_THDAC;
	param.val = 0;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC216X100M_THRCLK;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::GetClkThr(CModule* pModule, double& ThrVal)
{
	PADC216X100MSRV_CFG pSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;
//	pValChan->value = pSrvCfg->ClkPhase[pValChan->chan];
	ThrVal = pSrvCfg->ThrClk;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::SetGain(CModule* pModule, double& Gain, ULONG Chan)
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
int CAdc216x100mSrv::GetGain(CModule* pModule, double& Gain, ULONG Chan)
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
int CAdc216x100mSrv::SetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
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
int CAdc216x100mSrv::GetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
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
int CAdc216x100mSrv::SetCode(CModule* pModule, ULONG type)
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
int CAdc216x100mSrv::GetCode(CModule* pModule, ULONG& type)
{
	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;
	//param.tetr = m_AdcTetrNum;
	//param.reg = ADM2IFnr_FORMAT;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//PADM2IF_FORMAT pFormat = (PADM2IF_FORMAT)&param.val;
	//type = pFormat->ByBits.Code;

	PADC216X100MSRV_CFG pSrvCfg = (PADC216X100MSRV_CFG)m_pConfig;
	type = pSrvCfg->AdcCfg.Encoding;

//	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::SetMaster(CModule* pModule, ULONG mode)
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

//***************************************************************************************
int CAdc216x100mSrv::SetDither(CModule* pModule, ULONG dither)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.Dither = dither;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::GetDither(CModule* pModule, ULONG& dither)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	dither = pCtrl->ByBits.Dither;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::SetMu(CModule* pModule, void	*args)
{
	PADC216X100MSRV_MU pMU = (PADC216X100MSRV_MU)args;

	CAdcSrv::SetChanMask(pModule, pMU->chanMask);
	SetMaster(pModule, pMU->syncMode);

	CtrlFormat(pModule, NULL, BRDctrl_ADC_SETFORMAT, &pMU->format);

	//PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	SetClkSource(pModule, pMU->clockSrc);
	SetClkValue(pModule, pMU->clockSrc, pMU->clockValue);
	SetRate(pModule, pMU->samplingRate, pMU->clockSrc, pMU->clockValue);

	for(int i = 0; i < MAX_CHAN; i++)
	{
		SetInpRange(pModule, pMU->range[i], i);
		SetBias(pModule, pMU->bias[i], i);
		SetInpResist(pModule, pMU->resist[i], i);
		SetDcCoupling(pModule, pMU->dcCoupling[i], i);
	}
	SetClkThr(pModule, pMU->thrClk);
	SetDither(pModule, pMU->dither);

	BRD_StartMode start_mode;
	start_mode.startSrc		= pMU->baseStartSrc;
	start_mode.startInv		= pMU->baseStartInv;
	start_mode.trigOn		= pMU->startStopMode;
	start_mode.trigStopSrc	= pMU->baseStopSrc;
	start_mode.stopInv		= pMU->baseStopInv;
	start_mode.reStartMode	= pMU->reStart;
	SetStartMode(pModule, &start_mode);

	BRD_EnVal st_delay;
	st_delay.enable	= pMU->cnt0Enable;
	st_delay.value	= pMU->startDelayCnt;
	SetCnt(pModule, 0, &st_delay);
	pMU->startDelayCnt = st_delay.value;
	BRD_EnVal acq_data;
	acq_data.enable	= pMU->cnt1Enable;
	acq_data.value	= pMU->daqCnt;
	SetCnt(pModule, 1, &st_delay);
	pMU->daqCnt = acq_data.value;
	BRD_EnVal skip_data;
	skip_data.enable = pMU->cnt2Enable;
	skip_data.value	 = pMU->skipCnt;
	SetCnt(pModule, 2, &st_delay);
	pMU->skipCnt = skip_data.value;

	ULONG sample_size = pMU->format ? pMU->format : 2;
	int chans = 0;
	for(ULONG i = 0; i < 16; i++)
		chans += (pMU->chanMask >> i) & 0x1;

	BRD_PretrigMode pretrigger;
	pretrigger.enable = pMU->pretrigEnable;
	pretrigger.assur = pMU->pretrigAssur;
	pretrigger.external = pMU->pretrigExernal;
	pretrigger.size = ULONG((pMU->pretrigSamples * chans * sample_size) / sizeof(ULONG));
	SetPretrigMode(pModule, &pretrigger);
	pMU->pretrigSamples = (pretrigger.size * sizeof(ULONG)) / (chans * sample_size);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc216x100mSrv::SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec)
{
//	int Status = BRDerr_CMD_UNSUPPORTED;
	int Status = BRDerr_OK;
	CAdcSrv::SetSpecific(pModule, pSpec);
	DEVS_CMD_Reg param;
	param.idxMain = m_index;

	switch(pSpec->command)
	{
	case ADC216X100Mcmd_SETDITHER:
		{
			ULONG dither = *(PULONG)pSpec->arg;
			SetDither(pModule, dither);
		}
		break;
	case ADC216X100Mcmd_GETDITHER:
		{
			ULONG dither;
			GetDither(pModule, dither);
			*(PULONG)pSpec->arg = dither;
		}
		break;
	case ADC216X100Mcmd_SETMU:
		{
			PADC216X100MSRV_MU pMU = (PADC216X100MSRV_MU)pSpec->arg;
			SetMu(pModule, pMU);
		}
		break;
	}
	return Status;
}

// ***************** End of file Adc216x100mSrv.cpp ***********************
