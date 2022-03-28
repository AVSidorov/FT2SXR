/****************** File Adc818X800Srv.cpp ************
 *
 * BRD Driver for ADС service on ADM818X800
 *
 * (C) InSys by Dorokhin A. Jun 2007
 *
 *******************************************************/

#include "module.h"
#include "adc818x800srv.h"

#define	CURRFILE "ADC818X800SRV"

const double BRD_ADC_MAXGAINTUN	= 1.; // not useed

//***************************************************************************************
CAdc818X800Srv::CAdc818X800Srv(int idx, int srv_num, CModule* pModule, PADC818X800SRV_CFG pCfg) :
	CAdcSrv(idx, _BRDC("ADC818X800"), srv_num, pModule, pCfg, sizeof(ADC818X800SRV_CFG))
{
}

//***************************************************************************************
int CAdc818X800Srv::CtrlRelease(
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
void CAdc818X800Srv::GetAdcTetrNum(CModule* pModule)
{
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC818X800_TETR_ID);
}

//***************************************************************************************
void CAdc818X800Srv::FreeInfoForDialog(PVOID pInfo)
{
	PADC818X800SRV_INFO pSrvInfo = (PADC818X800SRV_INFO)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
// get parameters from device & set it into dialog
PVOID CAdc818X800Srv::GetInfoForDialog(CModule* pDev)
{
	PADC818X800SRV_CFG pSrvCfg = (PADC818X800SRV_CFG)m_pConfig;
	PADC818X800SRV_INFO pInfo = new ADC818X800SRV_INFO;
	pInfo->Size = sizeof(ADC818X800SRV_INFO);
	pInfo->pAdcInfo = (PADCSRV_INFO)CAdcSrv::GetInfoForDialog(pDev);
	ULONG code;
	GetCode(pDev, code);
	pInfo->pAdcInfo->Encoding = (UCHAR)code;
	//GetSpeedMode(pDev, pSrvInfo->SpeedMode);
	GetClkSource(pDev, pInfo->pAdcInfo->ClockSrc);
	GetClkValue(pDev, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	GetRate(pDev, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockValue);
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		GetInpRange(pDev, pInfo->Range[i], i);
		GetBias(pDev, pInfo->Bias[i], i);
	}

//	GetInpSrc(pDev, &(pSrvInfo->InpSrc));
//	GetTestMode(pDev, pSrvInfo->TestSrc);
//	pSrvInfo->TestSrc >>= 16;
//	GetHpfOff(pDev, pSrvInfo->HpfOff);

	pInfo->pAdcInfo->Range[0] = pSrvCfg->AdcCfg.InpRange;
	return pInfo;
}

//***************************************************************************************
int CAdc818X800Srv::SetPropertyFromDialog(void	*pDev, void	*args)
{
	CModule* pModule = (CModule*)pDev;
	PADC818X800SRV_INFO pInfo = (PADC818X800SRV_INFO)args;
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

	PADC818X800SRV_CFG pSrvCfg = (PADC818X800SRV_CFG)m_pConfig;
	pSrvCfg->AdcCfg.BaseExtClk = ROUND(pInfo->pAdcInfo->BaseExtClk);
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		SetInpRange(pModule, pInfo->Range[i], i);
		SetBias(pModule, pInfo->Bias[i], i);
	}


	return BRDerr_OK;
}

//***************************************************************************************
// read parameters from ini-file & set it
int CAdc818X800Srv::SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	TCHAR Buffer[128];
	TCHAR ParamName[128];
//	TCHAR* endptr;
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);

	double ClockValue;
	ULONG ClockSrc;
	GetClkSource(pDev, ClockSrc);
	GetClkValue(pDev, ClockSrc, ClockValue);
    IPC_getPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("800000.0"), Buffer, sizeof(Buffer), iniFilePath);
    double rate = BRDC_atof(Buffer);
	SetRate(pDev, rate, ClockSrc, ClockValue);

	double InpRange[MAX_ADC_CHAN], Bias[MAX_ADC_CHAN];
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
        BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
        IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("2.5"), Buffer, sizeof(Buffer), iniFilePath);
		InpRange[i] = atof(Buffer);
		SetInpRange(pDev, InpRange[i], i);
        BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
        IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
        Bias[i] = BRDC_atof(Buffer);
		SetBias(pDev, Bias[i], i);
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc818X800Srv::SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	TCHAR Buffer[128];
	TCHAR ParamName[128];
	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);
	PADC818X800SRV_CFG pSrvCfg = (PADC818X800SRV_CFG)m_pConfig;
    BRDC_sprintf(Buffer, _BRDC("%u"), pSrvCfg->AdcCfg.Encoding);
    IPC_writePrivateProfileString(SectionName, _BRDC("CodeType"), Buffer, iniFilePath);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
    BRDC_sprintf(Buffer, _BRDC("%.2f"), (double)pSrvCfg->AdcCfg.BaseExtClk);
    IPC_writePrivateProfileString(SectionName, _BRDC("BaseExternalClockValue"), Buffer, iniFilePath);
	double rate;
	GetRate(pDev, rate, clkValue);
    BRDC_sprintf(Buffer, _BRDC("%.2f"), rate);
    IPC_writePrivateProfileString(SectionName, _BRDC("SamplingRate"), Buffer, iniFilePath);

	double InpRange[MAX_ADC_CHAN], Bias[MAX_ADC_CHAN];
	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		GetInpRange(pDev, InpRange[i], i);
        BRDC_sprintf(Buffer, _BRDC("%.2f"), InpRange[i]);
        BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
        IPC_writePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		GetBias(pDev, Bias[i], i);
        BRDC_sprintf(Buffer, _BRDC("%.4f"), Bias[i]);
        BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
        IPC_writePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
	}

	// the function flushes the cache
    IPC_writePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}


//***************************************************************************************
int CAdc818X800Srv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
//	ULONG AdcDivideMode;
	PADC818X800SRV_CFG pSrvCfg = (PADC818X800SRV_CFG)m_pConfig;
	if(Rate < pSrvCfg->AdcCfg.MinRate)
		Rate = pSrvCfg->AdcCfg.MinRate;
	if(Rate > pSrvCfg->AdcCfg.MaxRate)
		Rate = pSrvCfg->AdcCfg.MaxRate;

//	GetSpeedMode(pModule, AdcDivideMode);

//	ULONG ClkDivider = SINGLE_DIVIDER >> AdcDivideMode;

	double ClkVal = ClkValue;// / ClkDivider;
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

	ULONG warp = 0;
	if( Rate < 666000) {
		ULONG	mode = 0;
		ULONG Status = SetWarp(pModule, &mode);
	} else {
		ULONG	mode = 1;
		ULONG Status = SetWarp(pModule, &mode);
	}

	ULONG Status = CAdcSrv::SetRate(pModule, Rate, ClkSrc, ClkVal);
	return Status;
}

//***************************************************************************************
int CAdc818X800Srv::GetRate(CModule* pModule, double& Rate, double ClkValue)
{
	int Status = BRDerr_OK;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
//	ULONG AdcDivideMode = pCtrl->ByBits.Lrck;
//	ULONG ClkDivider = SINGLE_DIVIDER >> AdcDivideMode;
	CAdcSrv::GetRate(pModule, Rate, ClkValue);// / ClkDivider);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc818X800Srv::SetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	if( Chan > 7 )
		return BRDerr_BAD_PARAMETER;

	int Status = BRDerr_OK;
	double inp_range;
	Status = GetInpRange(pModule, inp_range, Chan); 
	if(Status != BRDerr_OK)
		return Status;
	double bias_range = inp_range / 2; // 50 %
	int max_dac_value = 255;
	int min_dac_value = 0;
	double half_dac_value = 128.;

	USHORT dac_data;
	if(fabs(Bias) > bias_range)
		dac_data = Bias > 0.0 ? max_dac_value : min_dac_value;
	else
		dac_data = (USHORT)floor((Bias / bias_range + 1.) * half_dac_value + 0.5);
	if(dac_data > max_dac_value)
		dac_data = max_dac_value;
	Bias = bias_range * (dac_data / half_dac_value - 1.);
	//USHORT incr_data = dac_data & 0x3;
	//dac_data >>= 2;

	PADC818X800SRV_CFG pSrvCfg = (PADC818X800SRV_CFG)m_pConfig;
	pSrvCfg->Bias[Chan] = Bias;
//	int offset = (Chan < 2) ? (Chan + BRDtdn_ADC414X65M_BIAS0) : (Chan - 2 + BRDtdn_ADC414X65M_BIAS2);

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADC818X800nr_BIASDACCTRL;
	PADC_BIASDACCTRL pThDacCtrl = (PADC_BIASDACCTRL)&param.val;
//	pThDacCtrl->ByBits.Mode = 0;
	pThDacCtrl->ByBits.Num = Chan+1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADC818X800nr_BIASDACDATA;
	param.val = dac_data;
	//PADC_BIASDACDATA pThDacData = (PADC_BIASDACDATA)&param.val;
	//pThDacData->ByBits.Data = dac_data;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
/*
	param.reg = ADC818X800nr_BIASDACCTRL;
	pThDacCtrl = (PADC_BIASDACCTRL)&param.val;
//	pThDacCtrl->ByBits.Mode = 1;
	pThDacCtrl->ByBits.Num = Chan;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADC818X800nr_BIASDACDATA;
	param.val = incr_data;
	//PADC_BIASDACDATA pThDacData = (PADC_BIASDACDATA)&param.val;
	//pThDacData->ByBits.Data = dac_data;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
*/
	return Status;
}

//***************************************************************************************
int CAdc818X800Srv::GetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	if( Chan > 7 )
		return BRDerr_BAD_PARAMETER;

	int Status = BRDerr_OK;
	PADC818X800SRV_CFG pSrvCfg = (PADC818X800SRV_CFG)m_pConfig;
	Bias = pSrvCfg->Bias[Chan];
	return Status;
}

//***************************************************************************************
int CAdc818X800Srv::SetGain(CModule* pModule, double& Gain, ULONG Chan)
{
	if( Chan > 7 )
		return BRDerr_BAD_PARAMETER;

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
	case 4:
		pGain->ByBits.Chan4 = mask;
		break;
	case 5:
		pGain->ByBits.Chan5 = mask;
		break;
	case 6:
		pGain->ByBits.Chan6 = mask;
		break;
	case 7:
		pGain->ByBits.Chan7 = mask;
		break;
	}
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return Status;
}

//***************************************************************************************
int CAdc818X800Srv::GetGain(CModule* pModule, double& Gain, ULONG Chan)
{
	if( Chan > 7 )
		return BRDerr_BAD_PARAMETER;

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
	case 4:
		value = BRD_Gain[pGain->ByBits.Chan4];
		break;
	case 5:
		value = BRD_Gain[pGain->ByBits.Chan5];
		break;
	case 6:
		value = BRD_Gain[pGain->ByBits.Chan6];
		break;
	case 7:
		value = BRD_Gain[pGain->ByBits.Chan7];
		break;
	}
	Gain = value;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc818X800Srv::SetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
{
	if( Chan > 7 )
		return BRDerr_BAD_PARAMETER;

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
	case 4:
		pGain->ByBits.Chan4 = mask;
		break;
	case 5:
		pGain->ByBits.Chan5 = mask;
		break;
	case 6:
		pGain->ByBits.Chan6 = mask;
		break;
	case 7:
		pGain->ByBits.Chan7 = mask;
		break;
	}
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return Status;
}

//***************************************************************************************
int CAdc818X800Srv::GetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
{
	if( Chan > 7 )
		return BRDerr_BAD_PARAMETER;

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
		value = m_Range[pGain->ByBits.Chan0];
		break;
	case 1:
		value = m_Range[pGain->ByBits.Chan1];
		break;
	case 2:
		value = m_Range[pGain->ByBits.Chan2];
		break;
	case 3:
		value = m_Range[pGain->ByBits.Chan3];
		break;
	case 4:
		value = m_Range[pGain->ByBits.Chan4];
		break;
	case 5:
		value = m_Range[pGain->ByBits.Chan5];
		break;
	case 6:
		value = m_Range[pGain->ByBits.Chan6];
		break;
	case 7:
		value = m_Range[pGain->ByBits.Chan7];
		break;
	}
	InpRange = value;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc818X800Srv::SetCode(CModule* pModule, ULONG type)
{
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc818X800Srv::GetCode(CModule* pModule, ULONG& type)
{
	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc818X800Srv::ExtraInit(CModule* pModule)
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

	PADC818X800SRV_CFG pSrvCfg = (PADC818X800SRV_CFG)m_pConfig;
	m_Range[0] = (double)pSrvCfg->AdcCfg.InpRange / 1000;
	m_Range[1] = (double)pSrvCfg->AdcCfg.InpRange / 2000;
	m_Range[2] = (double)pSrvCfg->AdcCfg.InpRange / 4000;
	m_Range[3] = (double)pSrvCfg->AdcCfg.InpRange / 8000;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc818X800Srv::ResetAdc(CModule* pModule, ULONG mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
//	pCtrl->ByBits.AdcRst = mode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}




//***************************************************************************************
int CAdc818X800Srv::SetWarp(CModule* pModule, PVOID pCotrol)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.ADCWARP = *(PULONG)pCotrol;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc818X800Srv::GetWarp(CModule* pModule, PVOID pCotrol)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	*(PULONG)pCotrol = pCtrl->ByBits.ADCWARP;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc818X800Srv::SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
/*
	int Status = BRDerr_OK;
	CAdcSrv::SetSpecific(pModule, pSpec);
	switch(pSpec->command)
	{
	case ADC818X800cmd_SETWARP:
		{
			ULONG mode = *(PULONG)pSpec->arg;
			Status = SetWarp(pModule, &mode);
		}
		break;
	case ADC818X800cmd_GETWARP:
		{
			ULONG mode;
			Status = GetWarp(pModule, &mode);
			*(PULONG)pSpec->arg = mode;
		}
		break;
	default: break;
	}
*/
	return Status;
}

// ***************** End of file Adc818X800Srv.cpp ***********************
