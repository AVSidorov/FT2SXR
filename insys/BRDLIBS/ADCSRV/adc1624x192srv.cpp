/****************** File Adc1624x192Srv.cpp ************
 *
 * BRD Driver for ADС service on ADM1624x192
 *
 * (C) InSys by Dorokhin A. Jun 2007
 *
 *******************************************************/

#include "module.h"
#include "adc1624x192srv.h"

#define	CURRFILE "ADC1624X192SRV"

const double BRD_ADC_MAXGAINTUN	= 1.; // not useed

//***************************************************************************************
CAdc1624x192Srv::CAdc1624x192Srv(int idx, int srv_num, CModule* pModule, PADC1624X192SRV_CFG pCfg) :
	CAdcSrv(idx, _BRDC("ADC1624X192"), srv_num, pModule, pCfg, sizeof(ADC1624X192SRV_CFG))
{
}

//***************************************************************************************
int CAdc1624x192Srv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	ResetAdc(pModule, 0); // подать сигнал сброса
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);
//	return BRDerr_OK;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CAdc1624x192Srv::GetAdcTetrNum(CModule* pModule)
{
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC1624X192_TETR_ID);

	PADC1624X192SRV_CFG pSrvCfg = (PADC1624X192SRV_CFG)m_pConfig;
	m_Range[0] = (double)pSrvCfg->AdcCfg.InpRange / 1000;
	m_Range[1] = (double)pSrvCfg->AdcCfg.InpRange / 2000;
	m_Range[2] = (double)pSrvCfg->AdcCfg.InpRange / 4000;
	m_Range[3] = (double)pSrvCfg->AdcCfg.InpRange / 8000;
}

//***************************************************************************************
void CAdc1624x192Srv::FreeInfoForDialog(PVOID pInfo)
{
	PADC1624X192SRV_INFO pSrvInfo = (PADC1624X192SRV_INFO)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
// get parameters from device & set it into dialog
PVOID CAdc1624x192Srv::GetInfoForDialog(CModule* pDev)
{
	PADC1624X192SRV_CFG pSrvCfg = (PADC1624X192SRV_CFG)m_pConfig;
	PADC1624X192SRV_INFO pSrvInfo = new ADC1624X192SRV_INFO;
	pSrvInfo->Size = sizeof(ADC1624X192SRV_INFO);
	pSrvInfo->pAdcInfo = (PADCSRV_INFO)CAdcSrv::GetInfoForDialog(pDev);
	ULONG code;
	GetCode(pDev, code);
	pSrvInfo->pAdcInfo->Encoding = (UCHAR)code;
	GetSpeedMode(pDev, pSrvInfo->SpeedMode);
	GetClkSource(pDev, pSrvInfo->pAdcInfo->ClockSrc);
	GetClkValue(pDev, pSrvInfo->pAdcInfo->ClockSrc, pSrvInfo->pAdcInfo->ClockValue);
	GetRate(pDev, pSrvInfo->pAdcInfo->SamplingRate, pSrvInfo->pAdcInfo->ClockValue);
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		GetInpRange(pDev, pSrvInfo->Range[i], i);
		GetBias(pDev, pSrvInfo->Bias[i], i);
	}

	GetInpSrc(pDev, &(pSrvInfo->InpSrc));
	GetTestMode(pDev, pSrvInfo->TestSrc);
	pSrvInfo->TestSrc >>= 16;
	GetHpfOff(pDev, pSrvInfo->HpfOff);

	pSrvInfo->pAdcInfo->Range[0] = pSrvCfg->AdcCfg.InpRange;
	return pSrvInfo;
}

//***************************************************************************************
int CAdc1624x192Srv::SetPropertyFromDialog(void	*pDev, void	*args)
{
	CModule* pModule = (CModule*)pDev;
	PADC1624X192SRV_INFO pInfo = (PADC1624X192SRV_INFO)args;
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

	PADC1624X192SRV_CFG pSrvCfg = (PADC1624X192SRV_CFG)m_pConfig;
	pSrvCfg->AdcCfg.BaseExtClk = ROUND(pInfo->pAdcInfo->BaseExtClk);
	SetSpeedMode(pModule, pInfo->SpeedMode);
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		SetInpRange(pModule, pInfo->Range[i], i);
		SetBias(pModule, pInfo->Bias[i], i);
	}

	SetInpSrc(pModule, &(pInfo->InpSrc));
	pInfo->TestSrc <<= 16;
	SetTestMode(pModule, pInfo->TestSrc);
	SetHpfOff(pModule, pInfo->HpfOff);
	SelfClbr(pModule);

	return BRDerr_OK;
}

//***************************************************************************************
// read parameters from ini-file & set it
int CAdc1624x192Srv::SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	BRDCHAR Buffer[128];
	BRDCHAR ParamName[128];
	BRDCHAR* endptr;
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);
	IPC_getPrivateProfileString(SectionName, _BRDC("SpeedMode"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	ULONG SpeedMode = BRDC_strtol(Buffer, &endptr, 0);
	SetSpeedMode(pDev, SpeedMode);

	double ClockValue;
	ULONG ClockSrc;
	GetClkSource(pDev, ClockSrc);
	GetClkValue(pDev, ClockSrc, ClockValue);
	IPC_getPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("192000.0"), Buffer, sizeof(Buffer), iniFilePath);
	double rate = BRDC_atof(Buffer);
	SetRate(pDev, rate, ClockSrc, ClockValue);

	double InpRange, Bias[MAX_ADC_CHAN];
	for(int i = 0; i < MAX_ADC_CHAN; i+=2)
	{
		BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
		IPC_getPrivateProfileString(SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
		if( Buffer[0] == '\0' )
		{
			BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i+1);
			IPC_getPrivateProfileString(SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
		}
		if( Buffer[0] == '\0' )
			InpRange = 0.5;
		else
			InpRange = BRDC_atof(Buffer);
		SetInpRange(pDev, InpRange, i);
	}
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
		IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		Bias[i] = BRDC_atof(Buffer);
		SetBias(pDev, Bias[i], i);
	}

	IPC_getPrivateProfileString(SectionName, _BRDC("InputSource"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
	ULONG InpSrc = BRDC_strtol(Buffer, &endptr, 0);
	SetInpSrc(pDev, &InpSrc);
	IPC_getPrivateProfileString(SectionName, _BRDC("TestSource"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	ULONG TestSrc = BRDC_strtol(Buffer, &endptr, 0);
	SetTestMode(pDev, TestSrc);
	IPC_getPrivateProfileString(SectionName, _BRDC("HpfOff"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	ULONG HpfOff = BRDC_strtol(Buffer, &endptr, 0);
	SetHpfOff(pDev, HpfOff);
	SelfClbr(pDev);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	//BRDCHAR Buffer[128];
	BRDCHAR ParamName[128];
	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);
	PADC1624X192SRV_CFG pSrvCfg = (PADC1624X192SRV_CFG)m_pConfig;
	//BRDC_sprintf(Buffer, _BRDC("%u"), pSrvCfg->AdcCfg.Encoding);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("CodeType"), (ULONG)pSrvCfg->AdcCfg.Encoding, 10, NULL);

	ULONG SpeedMode;
	GetSpeedMode(pDev, SpeedMode);
	//BRDC_sprintf(Buffer, _BRDC("%u"), SpeedMode);
	//WritePrivateProfileString(SectionName, _BRDC("SpeedMode"), Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("SpeedMode"), SpeedMode, 10, NULL);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
	//BRDC_sprintf(Buffer, _BRDC("%.2f"), (double)pSrvCfg->AdcCfg.BaseExtClk);
	//WritePrivateProfileString(SectionName, _BRDC("BaseExternalClockValue"), Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("BaseExternalClockValue"), (double)pSrvCfg->AdcCfg.BaseExtClk, 2, NULL);
	double rate;
	GetRate(pDev, rate, clkValue);
	//BRDC_sprintf(Buffer, _BRDC("%.2f"), rate);
	//WritePrivateProfileString(SectionName, _BRDC("SamplingRate"), Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("SamplingRate"), rate, 2, NULL);

	double InpRange[MAX_ADC_CHAN], Bias[MAX_ADC_CHAN];
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		GetInpRange(pDev, InpRange[i], i);
		//BRDC_sprintf(Buffer, _BRDC("%.2f"), InpRange[i]);
		//BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
		//WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		WriteInifileParam(iniFilePath, SectionName, ParamName, InpRange[i], 2, NULL);
		GetBias(pDev, Bias[i], i);
		//BRDC_sprintf(Buffer, _BRDC("%.4f"), Bias[i]);
		//BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
		//WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		WriteInifileParam(iniFilePath, SectionName, ParamName, Bias[i], 4, NULL);
	}
	ULONG InpSrc;
	GetInpSrc(pDev, &InpSrc);
	//BRDC_sprintf(Buffer, _BRDC("%u"), InpSrc);
	//WritePrivateProfileString(SectionName, _BRDC("InputSource"), Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("InputSource"), InpSrc, 10, NULL);
	ULONG TestSrc;
	GetTestMode(pDev, TestSrc);
	//BRDC_sprintf(Buffer, _BRDC("%u"), TestSrc);
	//WritePrivateProfileString(SectionName, _BRDC("TestSource"), Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("TestSource"), TestSrc, 10, NULL);
	ULONG HpfOff;
	GetHpfOff(pDev, HpfOff);
	//BRDC_sprintf(Buffer, _BRDC("%u"), HpfOff);
	//WritePrivateProfileString(SectionName, _BRDC("HpfOff"), Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("HpfOff"), HpfOff, 10, NULL);

	// the function flushes the cache
    IPC_writePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

//***************************************************************************************
//int CAdc414x65mSrv::SetClkSource(CModule* pModule, ULONG ClkSrc)
//{
//	int Status = BRDerr_OK;
//	Status = CAdcSrv::SetClkSource(pModule, ClkSrc);
//	SyncPulse(pModule, double ClkValue);
//	return Status;
//}
//
////***************************************************************************************
//int CAdc414x65mSrv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
//{
//	CAdcSrv::GetClkSource(pModule, ClkSrc);
//	return BRDerr_OK;
//}

//***************************************************************************************
//int CAdc1624x192Srv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
//{
//	ULONG AdcDivideMode;
//	PADC1624X192SRV_CFG pSrvCfg = (PADC1624X192SRV_CFG)m_pConfig;
//	if(Rate < pSrvCfg->AdcCfg.MinRate)
//		Rate = pSrvCfg->AdcCfg.MinRate;
//	if(Rate > pSrvCfg->AdcCfg.MaxRate)
//		Rate = pSrvCfg->AdcCfg.MaxRate;
//
//	if(Rate < MAX_SINGLE_RATE)
//		AdcDivideMode = 0; // SINGLE
//	else
//		if(Rate < MAX_DOUBLE_RATE)
//			AdcDivideMode = 1; // DOUBLE
//		else
//			AdcDivideMode = 2; // QUAD
//
//	ULONG ClkDivider = SINGLE_DIVIDER >> AdcDivideMode;
//
//	double ClkVal = ClkValue / ClkDivider;
//	ULONG AdcRateDivider = ROUND(ClkVal / Rate);
//	AdcRateDivider = AdcRateDivider ? AdcRateDivider : 1;
//	while(1)
//	{
//		Rate = ClkVal / AdcRateDivider;
//		if(Rate < pSrvCfg->AdcCfg.MinRate)
//			AdcRateDivider--;
//		else
//			if(Rate > pSrvCfg->AdcCfg.MaxRate)
//				AdcRateDivider++;
//			else
//				break;
//	}
//
//	DEVS_CMD_Reg param;
//	param.idxMain = m_index;
//	param.tetr = m_AdcTetrNum;
//	param.reg = ADCnr_CTRL1;
//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
//	pCtrl->ByBits.Lrck = AdcDivideMode;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//
//	ULONG Status = CAdcSrv::SetRate(pModule, Rate, ClkSrc, ClkVal);
//	return Status;
//}

//int CAdc1624x192Srv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
//{
//	ULONG AdcDivideMode;
//	PADC1624X192SRV_CFG pSrvCfg = (PADC1624X192SRV_CFG)m_pConfig;
//	if(Rate < pSrvCfg->AdcCfg.MinRate)
//		Rate = pSrvCfg->AdcCfg.MinRate;
//	if(Rate > pSrvCfg->AdcCfg.MaxRate)
//		Rate = pSrvCfg->AdcCfg.MaxRate;
//
//	double MinRate[3], MaxRate[3], Rate_[3];
//	MinRate[0] = pSrvCfg->AdcCfg.MinRate;
//	MinRate[1] = MAX_SINGLE_RATE;
//	MinRate[2] = MAX_DOUBLE_RATE;
//	MaxRate[0] = MAX_SINGLE_RATE;
//	MaxRate[1] = MAX_DOUBLE_RATE;
//	MaxRate[2] = pSrvCfg->AdcCfg.MaxRate;
//
//	for(int i = 0; i < 3; i++)
//	{
//		ULONG ClkDivider = SINGLE_DIVIDER >> i;
//		double ClkVal = ClkValue / ClkDivider;
//		ULONG AdcRateDivider = ROUND(ClkVal / Rate);
//		AdcRateDivider = AdcRateDivider ? AdcRateDivider : 1;
//		while(1)
//		{
//			Rate_[i] = ClkVal / AdcRateDivider;
//			if(Rate_[i] < MinRate[i])
//				AdcRateDivider--;
//			else
//				if(Rate_[i] > MaxRate[i])
//					AdcRateDivider++;
//				else
//					break;
//		}
//	}
//
//	if(fabs(Rate - Rate_[0]) < fabs(Rate - Rate_[1]))
//	{
//		if(fabs(Rate - Rate_[0]) < fabs(Rate - Rate_[2]))
//		{
//			AdcDivideMode = 0; // SINGLE
//			Rate = Rate_[0];
//		}
//	}
//	else
//	{
//		if(fabs(Rate - Rate_[1]) < fabs(Rate - Rate_[2]))
//		{
//			AdcDivideMode = 1; // DOUBLE
//			Rate = Rate_[1];
//		}
//		else
//		{
//			AdcDivideMode = 2; // QUAD
//			Rate = Rate_[2];
//		}
//	}
//
//	ULONG ClkDivider = SINGLE_DIVIDER >> AdcDivideMode;
//	double ClkVal = ClkValue / ClkDivider;
//	//ULONG AdcRateDivider = ROUND(ClkVal / Rate);
//	//AdcRateDivider = AdcRateDivider ? AdcRateDivider : 1;
//
//	DEVS_CMD_Reg param;
//	param.idxMain = m_index;
//	param.tetr = m_AdcTetrNum;
//	param.reg = ADCnr_CTRL1;
//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
//	pCtrl->ByBits.Lrck = AdcDivideMode;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//
//	ULONG Status = CAdcSrv::SetRate(pModule, Rate, ClkSrc, ClkVal);
//	return Status;
//}

//***************************************************************************************
int CAdc1624x192Srv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
	ULONG AdcDivideMode;
	PADC1624X192SRV_CFG pSrvCfg = (PADC1624X192SRV_CFG)m_pConfig;
	if(Rate < pSrvCfg->AdcCfg.MinRate)
		Rate = pSrvCfg->AdcCfg.MinRate;
	if(Rate > pSrvCfg->AdcCfg.MaxRate)
		Rate = pSrvCfg->AdcCfg.MaxRate;

	GetSpeedMode(pModule, AdcDivideMode);

	ULONG ClkDivider = SINGLE_DIVIDER >> AdcDivideMode;

	double ClkVal = ClkValue / ClkDivider;
	ULONG AdcRateDivider = ROUND(ClkVal / Rate);
	AdcRateDivider = AdcRateDivider ? AdcRateDivider : 1;
	while(1)
	{
		Rate = ClkVal / AdcRateDivider;
		if(Rate < pSrvCfg->AdcCfg.MinRate)
			AdcRateDivider--;
		else
			if(Rate > pSrvCfg->AdcCfg.MaxRate)
				AdcRateDivider++;
			else
				break;
	}
	ULONG Status = CAdcSrv::SetRate(pModule, Rate, ClkSrc, ClkVal);

	return Status;
}

//***************************************************************************************
int CAdc1624x192Srv::GetRate(CModule* pModule, double& Rate, double ClkValue)
{
	int Status = BRDerr_OK;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	ULONG AdcDivideMode = pCtrl->ByBits.Lrck;
	ULONG ClkDivider = SINGLE_DIVIDER >> AdcDivideMode;
	CAdcSrv::GetRate(pModule, Rate, ClkValue / ClkDivider);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::SetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	double inp_range;
	Status = GetInpRange(pModule, inp_range, Chan); 
	if(Status != BRDerr_OK)
		return Status;
	double bias_range = inp_range / 10; // 10 %
	int max_dac_value = 16383;
	int min_dac_value = 0;
	double half_dac_value = 8192.;

	USHORT dac_data;
	if(fabs(Bias) > bias_range)
		dac_data = Bias > 0.0 ? max_dac_value : min_dac_value;
	else
		dac_data = (USHORT)floor((Bias / bias_range + 1.) * half_dac_value + 0.5);
	if(dac_data > max_dac_value)
		dac_data = max_dac_value;
	Bias = bias_range * (dac_data / half_dac_value - 1.);
	USHORT incr_data = dac_data & 0x3;
	dac_data >>= 2;

	PADC1624X192SRV_CFG pSrvCfg = (PADC1624X192SRV_CFG)m_pConfig;
	pSrvCfg->Bias[Chan] = Bias;
//	int offset = (Chan < 2) ? (Chan + BRDtdn_ADC414X65M_BIAS0) : (Chan - 2 + BRDtdn_ADC414X65M_BIAS2);

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADC1624X192nr_BIASDACCTRL;
	PADC_BIASDACCTRL pThDacCtrl = (PADC_BIASDACCTRL)&param.val;
	pThDacCtrl->ByBits.Mode = 0;
	pThDacCtrl->ByBits.Num = Chan;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADC1624X192nr_BIASDACDATA;
	param.val = dac_data;
	//PADC_BIASDACDATA pThDacData = (PADC_BIASDACDATA)&param.val;
	//pThDacData->ByBits.Data = dac_data;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADC1624X192nr_BIASDACCTRL;
	pThDacCtrl = (PADC_BIASDACCTRL)&param.val;
	pThDacCtrl->ByBits.Mode = 1;
	pThDacCtrl->ByBits.Num = Chan;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADC1624X192nr_BIASDACDATA;
	param.val = incr_data;
	//PADC_BIASDACDATA pThDacData = (PADC_BIASDACDATA)&param.val;
	//pThDacData->ByBits.Data = dac_data;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

//***************************************************************************************
int CAdc1624x192Srv::GetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	PADC1624X192SRV_CFG pSrvCfg = (PADC1624X192SRV_CFG)m_pConfig;
	Bias = pSrvCfg->Bias[Chan];
	return Status;
}

//***************************************************************************************
int CAdc1624x192Srv::SetGain(CModule* pModule, double& Gain, ULONG Chan)
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
	PADC_GAIN pGain = (PADC_GAIN)&param.val;
	switch(Chan)
	{
	case 0:
	case 1:
		pGain->ByBits.Chan01 = mask;
		break;
	case 2:
	case 3:
		pGain->ByBits.Chan23 = mask;
		break;
	case 4:
	case 5:
		pGain->ByBits.Chan45 = mask;
		break;
	case 6:
	case 7:
		pGain->ByBits.Chan67 = mask;
		break;
	case 8:
	case 9:
		pGain->ByBits.Chan89 = mask;
		break;
	case 10:
	case 11:
		pGain->ByBits.Chan1011 = mask;
		break;
	case 12:
	case 13:
		pGain->ByBits.Chan1213 = mask;
		break;
	case 14:
	case 15:
		pGain->ByBits.Chan1415 = mask;
		break;
	}
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return Status;
}

//***************************************************************************************
int CAdc1624x192Srv::GetGain(CModule* pModule, double& Gain, ULONG Chan)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_GAIN pGain = (PADC_GAIN)&param.val;
	double value;
	switch(Chan)
	{
	case 0:
	case 1:
		value = BRD_Gain[pGain->ByBits.Chan01];
		break;
	case 2:
	case 3:
		value = BRD_Gain[pGain->ByBits.Chan23];
		break;
	case 4:
	case 5:
		value = BRD_Gain[pGain->ByBits.Chan45];
		break;
	case 6:
	case 7:
		value = BRD_Gain[pGain->ByBits.Chan67];
		break;
	case 8:
	case 9:
		value = BRD_Gain[pGain->ByBits.Chan89];
		break;
	case 10:
	case 11:
		value = BRD_Gain[pGain->ByBits.Chan1011];
		break;
	case 12:
	case 13:
		value = BRD_Gain[pGain->ByBits.Chan1213];
		break;
	case 14:
	case 15:
		value = BRD_Gain[pGain->ByBits.Chan1415];
		break;
	}
	Gain = value;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::SetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
{
	int Status = BRDerr_OK;
	ULONG mask;
	if(InpRange <= m_Range[BRD_GAINCNT - 1])
	{
		if(InpRange < m_Range[BRD_GAINCNT - 1])
			Status = BRDerr_ADC_ILLEGAL_INPRANGE | BRDerr_INFO;
		mask = BRD_GAINCNT - 1;
	}
	else
	{
		if(InpRange >= m_Range[0])
		{
			if(InpRange > m_Range[0])
				Status = BRDerr_ADC_ILLEGAL_INPRANGE | BRDerr_INFO;
			mask = 0;
		}
		else 
		{
			for(int i = 0; i < BRD_GAINCNT - 1; i++)
			{
				if(InpRange <= m_Range[i] && InpRange >= m_Range[i+1])
				{
					if(m_Range[i] - InpRange <= InpRange - m_Range[i+1])
						mask = i;

					else
						mask = i+1;
					break;
				}
			}
		}
	}
	InpRange = m_Range[mask];
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_GAIN pGain = (PADC_GAIN)&param.val;
	switch(Chan)
	{
	case 0:
	case 1:
		pGain->ByBits.Chan01 = mask;
		break;
	case 2:
	case 3:
		pGain->ByBits.Chan23 = mask;
		break;
	case 4:
	case 5:
		pGain->ByBits.Chan45 = mask;
		break;
	case 6:
	case 7:
		pGain->ByBits.Chan67 = mask;
		break;
	case 8:
	case 9:
		pGain->ByBits.Chan89 = mask;
		break;
	case 10:
	case 11:
		pGain->ByBits.Chan1011 = mask;
		break;
	case 12:
	case 13:
		pGain->ByBits.Chan1213 = mask;
		break;
	case 14:
	case 15:
		pGain->ByBits.Chan1415 = mask;
		break;
	}
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return Status;
}

//***************************************************************************************
int CAdc1624x192Srv::GetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_GAIN pGain = (PADC_GAIN)&param.val;
	double value;
	switch(Chan)
	{
	case 0:
	case 1:
		value = m_Range[pGain->ByBits.Chan01];
		break;
	case 2:
	case 3:
		value = m_Range[pGain->ByBits.Chan23];
		break;
	case 4:
	case 5:
		value = m_Range[pGain->ByBits.Chan45];
		break;
	case 6:
	case 7:
		value = m_Range[pGain->ByBits.Chan67];
		break;
	case 8:
	case 9:
		value = m_Range[pGain->ByBits.Chan89];
		break;
	case 10:
	case 11:
		value = m_Range[pGain->ByBits.Chan1011];
		break;
	case 12:
	case 13:
		value = m_Range[pGain->ByBits.Chan1213];
		break;
	case 14:
	case 15:
		value = m_Range[pGain->ByBits.Chan1415];
		break;
	}
	InpRange = value;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::SetCode(CModule* pModule, ULONG type)
{
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::GetCode(CModule* pModule, ULONG& type)
{
	PADC1624X192SRV_CFG pSrvCfg = (PADC1624X192SRV_CFG)m_pConfig;
	type = pSrvCfg->AdcCfg.Encoding;

//	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::ExtraInit(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.AdmClk = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	ResetAdc(pModule, 1); // снять сигнал сброса

	//PADC1624X192SRV_CFG pSrvCfg = (PADC1624X192SRV_CFG)m_pConfig;
	//m_Range[0] = (double)pSrvCfg->AdcCfg.InpRange / 1000;
	//m_Range[1] = (double)pSrvCfg->AdcCfg.InpRange / 2000;
	//m_Range[2] = (double)pSrvCfg->AdcCfg.InpRange / 4000;
	//m_Range[3] = (double)pSrvCfg->AdcCfg.InpRange / 8000;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::ResetAdc(CModule* pModule, ULONG mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.AdcRst = mode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::SyncPulse(CModule* pModule, double msPeriod)
{
	ULONG mode;
	GetHpfOff(pModule, mode);
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.Sync = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pCtrl->ByBits.Sync = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	pCtrl->ByBits.Sync = 0;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	ULONG time = 0;
	if(mode)
	{ // HPF - OFF
		time = ULONG(msPeriod * 2500 + 0.5);
	}
	else
	{ // HPF - ON
		time = ULONG(msPeriod * 100012 + 0.5);
	}
	Sleep(time);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::AdcEnable(CModule* pModule, ULONG enable)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master && enable)
	{ // Master
		param.reg = ADM2IFnr_MODE1;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		param.val = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.val = 0x10;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		Sleep(50);
		Sleep(50);
	}
	CAdcSrv::AdcEnable(pModule, enable);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::SelfClbr(CModule* pModule)
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

//***************************************************************************************
int CAdc1624x192Srv::SetHpfOff(CModule* pModule, ULONG mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.HPF = mode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::GetHpfOff(CModule* pModule, ULONG& mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	mode = pCtrl->ByBits.HPF;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::SetSpeedMode(CModule* pModule, ULONG mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.Lrck = mode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::GetSpeedMode(CModule* pModule, ULONG& mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	mode = pCtrl->ByBits.Lrck;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::SetTestMode(CModule* pModule, ULONG mode)
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
int CAdc1624x192Srv::GetTestMode(CModule* pModule, ULONG& mode)
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
int CAdc1624x192Srv::SetInpSrc(CModule* pModule, PVOID pCotrol)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.InpSrc = *(PULONG)pCotrol;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::GetInpSrc(CModule* pModule, PVOID pCotrol)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	*(PULONG)pCotrol = pCtrl->ByBits.InpSrc;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1624x192Srv::SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec)
{
//	int Status = BRDerr_CMD_UNSUPPORTED;
	int Status = BRDerr_OK;
	CAdcSrv::SetSpecific(pModule, pSpec);
	switch(pSpec->command)
	{
	case ADC16214X192cmd_SETHPFOFF:
		{
			ULONG mode = *(PULONG)pSpec->arg;
			Status = SetHpfOff(pModule, mode);
		}
		break;
	case ADC16214X192cmd_GETHPFOFF:
		{
			ULONG mode;
			Status = GetHpfOff(pModule, mode);
			*(PULONG)pSpec->arg = mode;
		}
		break;
	case ADC16214X192cmd_SETSPEEDMODE:
		{
			ULONG mode = *(PULONG)pSpec->arg;
			Status = SetSpeedMode(pModule, mode);
		}
		break;
	case ADC16214X192cmd_GETSPEEDMODE:
		{
			ULONG mode;
			Status = GetSpeedMode(pModule, mode);
			*(PULONG)pSpec->arg = mode;
		}
		break;
	}
	return Status;
}

// ***************** End of file Adc1624x192Srv.cpp ***********************
