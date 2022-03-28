/****************** File Adc414x65mSrv.cpp ************
 *
 * BRD Driver for ADС service on ADM414x65M
 *
 * (C) InSys by Dorokhin A. Aug 2006
 *
 *******************************************************/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "adc414x65msrv.h"

#define	CURRFILE "ADC414X65MSRV"

const double BRD_ADC_MAXGAINTUN	= 2.5; // max gain tuning = 2.5

//***************************************************************************************
CAdc414x65mSrv::CAdc414x65mSrv(int idx, int srv_num, CModule* pModule, PADC414X65MSRV_CFG pCfg) :
	CAdcSrv(idx, _BRDC("ADC414X65M"), srv_num, pModule, pCfg, sizeof(ADC414X65MSRV_CFG))
{
}

//***************************************************************************************
int CAdc414x65mSrv::CtrlRelease(
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
void CAdc414x65mSrv::GetAdcTetrNum(CModule* pModule)
{
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC414X65M_TETR_ID);
}

//***************************************************************************************
void CAdc414x65mSrv::FreeInfoForDialog(PVOID pInfo)
{
	PADC414X65MSRV_INFO pSrvInfo = (PADC414X65MSRV_INFO)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
// get parameters from device & set it into dialog
PVOID CAdc414x65mSrv::GetInfoForDialog(CModule* pDev)
{
	PADC414X65MSRV_CFG pSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
	PADC414X65MSRV_INFO pSrvInfo = new ADC414X65MSRV_INFO;
	pSrvInfo->Size = sizeof(ADC414X65MSRV_INFO);
	UCHAR code = pSrvCfg->AdcCfg.Encoding;
	pSrvInfo->pAdcInfo = (PADCSRV_INFO)CAdcSrv::GetInfoForDialog(pDev);
	pSrvInfo->pAdcInfo->Encoding = code;
	pSrvInfo->RefGen[0] = pSrvCfg->SubRefGen[0];
	pSrvInfo->RefGen[1] = pSrvCfg->SubRefGen[1];
	pSrvInfo->RefGen[2] = pSrvCfg->SubRefGen[2];
	pSrvInfo->ExtClk = pSrvCfg->SubExtClk;
	GetClkLocation(pDev, pSrvInfo->ClkLocation);
	GetClkSource(pDev, pSrvInfo->pAdcInfo->ClockSrc);
	GetClkValue(pDev, pSrvInfo->pAdcInfo->ClockSrc, pSrvInfo->pAdcInfo->ClockValue);
	GetRate(pDev, pSrvInfo->pAdcInfo->SamplingRate, pSrvInfo->pAdcInfo->ClockValue);
	if(pSrvInfo->pAdcInfo->ClockSrc == BRDclks_ADC_EXTCLK)
		pSrvInfo->ExtClk = pSrvInfo->pAdcInfo->ClockValue;
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		GetInpRange(pDev, pSrvInfo->Range[i], i);
		GetBias(pDev, pSrvInfo->Bias[i], i);
	}
	GetClkThr(pDev, pSrvInfo->ThrClk);
	GetExtClkThr(pDev, pSrvInfo->ThrExtClk);

	return pSrvInfo;
}

//***************************************************************************************
int CAdc414x65mSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
//	TCHAR msg[256];
	CModule* pModule = (CModule*)pDev;
	PADC414X65MSRV_INFO pInfo = (PADC414X65MSRV_INFO)args;
	CAdcSrv::SetChanMask(pModule, pInfo->pAdcInfo->ChanMask);
	ULONG master = pInfo->pAdcInfo->SyncMode;
	CtrlMaster(pDev, NULL, BRDctrl_ADC_SETMASTER, &master);
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

	SetClkLocation(pModule, pInfo->ClkLocation);
	PADC414X65MSRV_CFG pAdcSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
	pAdcSrvCfg->SubExtClk = ROUND(pInfo->ExtClk);
	pAdcSrvCfg->AdcCfg.BaseExtClk = ROUND(pInfo->pAdcInfo->BaseExtClk);
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
//	sprintf(msg, _T("ClockValue = %f, SamplingRate = %f"), pInfo->pAdcInfo->ClockValue, pInfo->pAdcInfo->SamplingRate);
//	MessageBox(NULL, msg, "Debug", MB_OK);
	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
//	sprintf(msg, _T("ClockValue = %f, SamplingRate = %f"), pInfo->pAdcInfo->ClockValue, pInfo->pAdcInfo->SamplingRate);
//	MessageBox(NULL, msg, "Debug", MB_OK);
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		SetInpRange(pModule, pInfo->Range[i], i);
		SetBias(pModule, pInfo->Bias[i], i);
	}
	SetClkThr(pModule, pInfo->ThrClk);
	SetExtClkThr(pModule, pInfo->ThrExtClk);

	return BRDerr_OK;
}

//***************************************************************************************
// read parameters from ini-file & set it
int CAdc414x65mSrv::SetProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	TCHAR ParamName[128];
	TCHAR* endptr;
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);

	GetPrivateProfileString(SectionName, "AdmClockSource", "0", Buffer, sizeof(Buffer), iniFilePath);
	ULONG admLoc = atoi(Buffer);;
	SetClkLocation(pDev, admLoc);

	GetPrivateProfileString(SectionName, "ClockSource", "1", Buffer, sizeof(Buffer), iniFilePath);
//	ULONG clkSrc = atoi(Buffer);;
	ULONG clkSrc = strtol(Buffer, &endptr, 0);
	SetClkSource(pDev, clkSrc);
	GetPrivateProfileString(SectionName, "ExternalClockValue", "30000000.0", Buffer, sizeof(Buffer), iniFilePath);
	double clkValue = atof(Buffer);
	PADC414X65MSRV_CFG pSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
	pSrvCfg->SubExtClk = ROUND(clkValue);
	if(!admLoc)
		clkValue = pSrvCfg->AdcCfg.BaseExtClk;
	SetClkValue(pDev, clkSrc, clkValue);
	GetPrivateProfileString(SectionName, "SamplingRate", "65000000.0", Buffer, sizeof(Buffer), iniFilePath);
	double rate = atof(Buffer);
	SetRate(pDev, rate, clkSrc, clkValue);

	double InpRange[MAX_ADC_CHAN], Bias[MAX_ADC_CHAN];
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		sprintf_s(ParamName, "InputRange%d", i);
		GetPrivateProfileString(SectionName, ParamName, "0.5", Buffer, sizeof(Buffer), iniFilePath);
		InpRange[i] = atof(Buffer);
		SetInpRange(pDev, InpRange[i], i);
		sprintf_s(ParamName, "Bias%d", i);
		GetPrivateProfileString(SectionName, ParamName, "0.0", Buffer, sizeof(Buffer), iniFilePath);
		Bias[i] = atof(Buffer);
		SetBias(pDev, Bias[i], i);
	}
	GetPrivateProfileString(SectionName, "ClockThreshold", "0.0", Buffer, sizeof(Buffer), iniFilePath);
	double ThrClock = atof(Buffer);
	SetClkThr(pDev, ThrClock);
	GetPrivateProfileString(SectionName, "ExternalClockThreshold", "0.0", Buffer, sizeof(Buffer), iniFilePath);
	double ThrExtClock = atof(Buffer);
	SetExtClkThr(pDev, ThrExtClock);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc414x65mSrv::SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	TCHAR ParamName[128];
	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);
	PADC414X65MSRV_CFG pSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
	sprintf_s(Buffer, "%u", pSrvCfg->AdcCfg.Encoding);
	WritePrivateProfileString(SectionName, "CodeType", Buffer, iniFilePath);

	ULONG admLoc;
	GetClkLocation(pDev, admLoc);
	sprintf_s(Buffer, "%u", admLoc);
	WritePrivateProfileString(SectionName, "AdmClockSource", Buffer, iniFilePath);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	sprintf_s(Buffer, "%u", clkSrc);
	WritePrivateProfileString(SectionName, "ClockSource", Buffer, iniFilePath);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
	sprintf_s(Buffer, "%.2f", (double)pSrvCfg->SubExtClk);
	WritePrivateProfileString(SectionName, "ExternalClockValue", Buffer, iniFilePath);
	sprintf_s(Buffer, "%.2f", (double)pSrvCfg->AdcCfg.BaseExtClk);
	WritePrivateProfileString(SectionName, "BaseExternalClockValue", Buffer, iniFilePath);
	//if(clkSrc == BRDclks_ADC_EXTCLK)
	//{
	//	sprintf_s(Buffer, "%.2f", clkValue);
	//	WritePrivateProfileString(SectionName, "ExternalClockValue", Buffer, iniFilePath);
	//}
	double rate;
	GetRate(pDev, rate, clkValue);
	sprintf_s(Buffer, "%.2f", rate);
	WritePrivateProfileString(SectionName, "SamplingRate", Buffer, iniFilePath);

	double InpRange[MAX_ADC_CHAN], Bias[MAX_ADC_CHAN];
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		GetInpRange(pDev, InpRange[i], i);
		sprintf_s(Buffer, "%.2f", InpRange[i]);
		sprintf_s(ParamName, "InputRange%d", i);
		WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		GetBias(pDev, Bias[i], i);
		sprintf_s(Buffer, "%.4f", Bias[i]);
		sprintf_s(ParamName, "Bias%d", i);
		WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
	}
	double ThrClock;
	GetClkThr(pDev, ThrClock);
	sprintf_s(Buffer, "%.4f", ThrClock);
	WritePrivateProfileString(SectionName, "ClockThreshold", Buffer, iniFilePath);
	double ThrExtClock;
	GetExtClkThr(pDev, ThrExtClock);
	sprintf_s(Buffer, "%.4f", ThrExtClock);
	WritePrivateProfileString(SectionName, "ExternalClockThreshold", Buffer, iniFilePath);

	// the function flushes the cache
	WritePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc414x65mSrv::SetClkSource(CModule* pModule, ULONG ClkSrc)
{
	int Status = BRDerr_OK;
	ULONG admLoc = 0;
	GetClkLocation(pModule, admLoc);
	if(!admLoc)
		Status = CAdcSrv::SetClkSource(pModule, ClkSrc);
	else
	{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADM2IFnr_FSRC;
		param.val = ClkSrc;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		// для стартовой схемы
		param.tetr = m_MainTetrNum;
		param.reg = ADM2IFnr_FMODE;
		param.val = BRDclks_SMCLK; // источник такта находится на субмодуле
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.reg = ADM2IFnr_FDIV;
		param.val = 1; // источник такта находится на субмодуле
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

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
		//		Status = BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
		//}
	}
	return Status;
}

//***************************************************************************************
int CAdc414x65mSrv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
{
//	int Status = BRDerr_OK;
	ULONG admLoc = 0;
	GetClkLocation(pModule, admLoc);
	if(!admLoc)
		CAdcSrv::GetClkSource(pModule, ClkSrc);
	else
	{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADM2IFnr_FSRC;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		ClkSrc = param.val;

		//ULONG source;
		//DEVS_CMD_Reg param;
		//param.idxMain = m_index;
		//param.tetr = m_AdcTetrNum;
		//param.reg = ADM2IFnr_MODE0;
		//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		//PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
		//if(pMode0->ByBits.Master)
		//{ // Single
		//	param.reg = ADM2IFnr_FSRC;
		//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		//	source = param.val;
		//}
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
		//ClkSrc = source;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc414x65mSrv::SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
//	int Status = BRDerr_OK;
	ULONG admLoc = 0;
	GetClkLocation(pModule, admLoc);
	if(!admLoc)
		CAdcSrv::SetClkValue(pModule, ClkSrc, ClkValue);
	else
	{
		PADC414X65MSRV_CFG pAdcSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
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
//			int CAdcSrv::SetClkValue(pModule, ClkSrc, ClkValue);
			break;
		}
	}
	return BRDerr_OK;
}
	
//***************************************************************************************
int CAdc414x65mSrv::GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
//	int Status = BRDerr_OK;
	ULONG admLoc = 0;
	GetClkLocation(pModule, admLoc);
	if(!admLoc)
		CAdcSrv::GetClkValue(pModule, ClkSrc, ClkValue);
	else
	{
		PADC414X65MSRV_CFG pAdcSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
		double Clk;
		switch(ClkSrc)
		{
		case BRDclks_ADC_DISABLED:		// disabled clock
			Clk = 0.0;
			break;
		case BRDclks_ADC_SUBGEN1:		// Submodule generator
			Clk = pAdcSrvCfg->SubRefGen[0];
			break;
		case BRDclks_ADC_SUBGEN2:		// Submodule generator
			Clk = pAdcSrvCfg->SubRefGen[1];
			break;
		case BRDclks_ADC_SUBGEN3:		// Submodule generator
			Clk = pAdcSrvCfg->SubRefGen[2];
			break;
		case BRDclks_ADC_EXTCLK:		// External clock
			Clk = pAdcSrvCfg->SubExtClk;
			break;
		default:
//			CAdcSrv::GetClkValue(pModule, ClkSrc, ClkValue);
			break;
		}
		ClkValue = Clk;
	}
	return BRDerr_OK;
}
	
static double DeltaFreq;
int M414x65M_CLK_DIVIDER[BRD_CLKDIVCNT] = {1, 2, 4, 8, 16, 32, 64};

//****************************************************************************************
int SetClkDivOpt(double RateHz, double curClk, double minRate)
{
	if(!curClk)
		return 1;
	int MaxDivider = (int)floor(curClk / minRate);
	int i = 0;
	for(i = BRD_CLKDIVCNT-1; i >= 0; i--)
		if(MaxDivider >= M414x65M_CLK_DIVIDER[i])
			break;
	int MaxInd = (i < 0) ? 0 : i;
	MaxDivider = M414x65M_CLK_DIVIDER[MaxInd];
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
				return M414x65M_CLK_DIVIDER[MaxInd];
			}
		}
		else 
		{
			for(int i = 0; i < MaxInd; i++) 
			{
				double RateLeft = curClk / M414x65M_CLK_DIVIDER[i];
				double RateRight = curClk / M414x65M_CLK_DIVIDER[i+1];
				if(RateHz <= RateLeft && RateHz >= RateRight)
				{
					if(RateLeft - RateHz <= RateHz - RateRight)
					{
						if (DeltaFreq > RateLeft - RateHz)
						{
							DeltaFreq = RateLeft - RateHz;
							return M414x65M_CLK_DIVIDER[i];
						}
					}
					else
					{
						if (DeltaFreq > RateHz - RateRight)
						{
							DeltaFreq = RateHz - RateRight;
							return M414x65M_CLK_DIVIDER[i+1];
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
int CAdc414x65mSrv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
//	int Status = BRDerr_OK;
	ULONG admLoc = 0;
	GetClkLocation(pModule, admLoc);
	if(!admLoc)
		CAdcSrv::SetRate(pModule, Rate, ClkSrc, ClkValue);
	else
	{
		ULONG AdcRateDivider;
		DeltaFreq = 1.e10;
		PADC414X65MSRV_CFG pSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
		AdcRateDivider = SetClkDivOpt(Rate, ClkValue, pSrvCfg->AdcCfg.MinRate);

		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADM2IFnr_FDVR;
		param.val = powof2(AdcRateDivider);
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		Rate = ClkValue / AdcRateDivider;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc414x65mSrv::GetRate(CModule* pModule, double& Rate, double ClkValue)
{
//	int Status = BRDerr_OK;
	ULONG admLoc = 0;
	GetClkLocation(pModule, admLoc);
	if(!admLoc)
		CAdcSrv::GetRate(pModule, Rate, ClkValue);
	else
	{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_AdcTetrNum;
		param.reg = ADM2IFnr_FDVR;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		ULONG AdcRateDivider = (ULONG)pow(2., (int)param.val);
		Rate = ClkValue / AdcRateDivider;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc414x65mSrv::SetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	double inp_range;
	Status = GetInpRange(pModule, inp_range, Chan); 
	if(Status != BRDerr_OK)
		return Status;
	double bias_range = inp_range / 4; // 25% from full range
	int max_dac_value = 255;
	int min_dac_value = 0;
	double half_dac_value = 128.;

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
	if(fabs(Bias) > bias_range)
		dac_data = Bias > 0.0 ? min_dac_value : max_dac_value;
	else
		//dac_data = (USHORT)floor((Bias / bias_range + 1.) * half_dac_value + 0.5);
		dac_data = (USHORT)floor((1. - Bias / bias_range) * half_dac_value + 0.5);
	if(dac_data > max_dac_value)
		dac_data = max_dac_value;
	//Bias = bias_range * (dac_data / half_dac_value - 1.);
	Bias = bias_range * (1. - dac_data / half_dac_value);

	PADC414X65MSRV_CFG pSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
	pSrvCfg->Bias[Chan] = Bias;
	int offset = (Chan < 2) ? (Chan + BRDtdn_ADC414X65M_BIAS0) : (Chan - 2 + BRDtdn_ADC414X65M_BIAS2);
	pSrvCfg->AdcCfg.ThrDac[offset - 1] = (UCHAR)dac_data;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = offset;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

//***************************************************************************************
int CAdc414x65mSrv::GetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
//	double inp_range = GetInpRange(pModule, Chan); 
	PADC414X65MSRV_CFG pSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
	//UCHAR dac_data = (UCHAR)pSrvCfg->ThrDac[pValChan->chan + BRDtdn_ADC212x200M_BIAS0];
	//pValChan->value = inp_range * (dac_data / half_dac_value - 1.);
	Bias = pSrvCfg->Bias[Chan];
	return Status;
}

//***************************************************************************************
int CAdc414x65mSrv::SetClkThr(CModule* pModule, double& ThrVal)
{
	PADC414X65MSRV_CFG pSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
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
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC414X65M_THRCLK - 1] = (UCHAR)dac_data;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC414X65M_THRCLK;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc414x65mSrv::GetClkThr(CModule* pModule, double& ThrVal)
{
	PADC414X65MSRV_CFG pSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
//	pValChan->value = pSrvCfg->ClkPhase[pValChan->chan];
	ThrVal = pSrvCfg->ThrClk;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc414x65mSrv::SetExtClkThr(CModule* pModule, double& ThrVal)
{
	PADC414X65MSRV_CFG pSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
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

	pSrvCfg->ThrExtClk = ThrVal;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC414X65M_THREXTCLK - 1] = (UCHAR)dac_data;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC414X65M_THREXTCLK;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc414x65mSrv::GetExtClkThr(CModule* pModule, double& ThrVal)
{
	PADC414X65MSRV_CFG pSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
//	pValChan->value = pSrvCfg->ClkPhase[pValChan->chan];
	ThrVal = pSrvCfg->ThrExtClk;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc414x65mSrv::SetGain(CModule* pModule, double& Gain, ULONG Chan)
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
int CAdc414x65mSrv::GetGain(CModule* pModule, double& Gain, ULONG Chan)
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
int CAdc414x65mSrv::SetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
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
int CAdc414x65mSrv::GetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
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
int CAdc414x65mSrv::SetCode(CModule* pModule, ULONG type)
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
int CAdc414x65mSrv::GetCode(CModule* pModule, ULONG& type)
{
	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;
	//param.tetr = m_AdcTetrNum;
	//param.reg = ADM2IFnr_FORMAT;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//PADM2IF_FORMAT pFormat = (PADM2IF_FORMAT)&param.val;
	//type = pFormat->ByBits.Code;

	PADC414X65MSRV_CFG pSrvCfg = (PADC414X65MSRV_CFG)m_pConfig;
	type = pSrvCfg->AdcCfg.Encoding;

//	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

// ***************** End of file Adc414x65mSrv.cpp ***********************
