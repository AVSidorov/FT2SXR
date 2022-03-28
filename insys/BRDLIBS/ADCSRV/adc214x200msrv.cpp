/*
 ***************** File Adc214x200mSrv.cpp ************
 *
 * BRD Driver for ADС service on ADM214x200M
 *
 * (C) InSys by Dorokhin A. Jan 2007
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "adc214x200msrv.h"

#define	CURRFILE "ADC214X200MSRV"

//***************************************************************************************
CAdc214x200mSrv::CAdc214x200mSrv(int idx, int srv_num, CModule* pModule, PADC214X200MSRV_CFG pCfg) :
	CAdcSrv(idx, _BRDC("ADC214X200M"), srv_num, pModule, pCfg, sizeof(ADC214X200MSRV_CFG))
{
}

//***************************************************************************************
int CAdc214x200mSrv::CtrlRelease(
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
void CAdc214x200mSrv::GetAdcTetrNum(CModule* pModule)
{
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC214X200M_TETR_ID);
}

//***************************************************************************************
void CAdc214x200mSrv::FreeInfoForDialog(PVOID pInfo)
{
	PADC214X200MSRV_INFO pSrvInfo = (PADC214X200MSRV_INFO)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc214x200mSrv::GetInfoForDialog(CModule* pDev)
{
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	PADC214X200MSRV_INFO pSrvInfo = new ADC214X200MSRV_INFO;
	pSrvInfo->Size = sizeof(ADC214X200MSRV_INFO);
	UCHAR code = pSrvCfg->AdcCfg.Encoding;
	pSrvInfo->pAdcInfo = (PADCSRV_INFO)CAdcSrv::GetInfoForDialog(pDev);
	if(pSrvInfo->pAdcInfo->SyncMode == 3)
		pSrvInfo->pAdcInfo->SyncMode = 1;
	pSrvInfo->pAdcInfo->Encoding = code;
	pSrvInfo->ExtClk = pSrvCfg->SubExtClk;
	GetClkSource(pDev, pSrvInfo->pAdcInfo->ClockSrc);
	GetClkValue(pDev, pSrvInfo->pAdcInfo->ClockSrc, pSrvInfo->pAdcInfo->ClockValue);
	GetRate(pDev, pSrvInfo->pAdcInfo->SamplingRate, pSrvInfo->pAdcInfo->ClockValue);
	if(pSrvInfo->pAdcInfo->ClockSrc == BRDclks_ADC_EXTCLK)
		pSrvInfo->ExtClk = pSrvInfo->pAdcInfo->ClockValue;
	ULONG mode;
	GetClkInv(pDev, mode);
	pSrvInfo->InvClk = (UCHAR)mode;
	for(int i = 0; i < MAX_CHAN; i++)
	{
		pSrvInfo->pAdcInfo->Range[i] = pSrvCfg->AdcCfg.InpRange / 1000.;
		if(pSrvCfg->IsRF)
			GetGain(pDev, pSrvInfo->dBGain[i], i);
		else
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
	pSrvInfo->pAdcInfo->ReStart = start_mode.stndStart.reStartMode;

	pSrvInfo->PllRefGen = pSrvCfg->PllRefGen;
	pSrvInfo->PllFreq = pSrvCfg->PllFreq;
//	pSrvInfo->ExtPllRef = pSrvCfg->ExtPllRef;
	
	pSrvInfo->Version = pSrvCfg->Version;
	pSrvInfo->IsRF = pSrvCfg->IsRF;

	return pSrvInfo;
}

//***************************************************************************************
int CAdc214x200mSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
//	TCHAR msg[256];
	CModule* pModule = (CModule*)pDev;
	PADC214X200MSRV_INFO pInfo = (PADC214X200MSRV_INFO)args;
//	ULONG ClockSrc = pInfo->pAdcInfo->ClockSrc;
//	double ClockValue = pInfo->pAdcInfo->ClockValue;
//	double SamplingRate = pInfo->pAdcInfo->SamplingRate;
//	CAdcSrv::SetPropertyFromDialog(pDev, pInfo->pAdcInfo);
//	pInfo->pAdcInfo->ClockSrc = ClockSrc;
//	pInfo->pAdcInfo->ClockValue = ClockValue;
//	pInfo->pAdcInfo->SamplingRate = SamplingRate;
	
	//CAdcSrv::SetChanMask(pModule, pInfo->pAdcInfo->ChanMask);
	SetChanMask(pModule, pInfo->pAdcInfo->ChanMask);
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

	PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	pAdcSrvCfg->SubExtClk = ROUND(pInfo->ExtClk);
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
//	sprintf(msg, _T("ClockValue = %f, SamplingRate = %f"), pInfo->pAdcInfo->ClockValue, pInfo->pAdcInfo->SamplingRate);
//	MessageBox(NULL, msg, "Debug", MB_OK);
	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
//	sprintf(msg, _T("ClockValue = %f, SamplingRate = %f"), pInfo->pAdcInfo->ClockValue, pInfo->pAdcInfo->SamplingRate);
//	MessageBox(NULL, msg, "Debug", MB_OK);

	SetClkInv(pModule, pInfo->InvClk);

	for(int i = 0; i < MAX_CHAN; i++)
	{
		if(pAdcSrvCfg->IsRF)
			SetGain(pModule, pInfo->dBGain[i], i);
		else
			SetInpRange(pModule, pInfo->pAdcInfo->Range[i], i);
		SetBias(pModule, pInfo->pAdcInfo->Bias[i], i);
		SetInpResist(pModule, pInfo->pAdcInfo->Resist[i], i);
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
	start_mode.stndStart.reStartMode = pInfo->pAdcInfo->ReStart;
	SetStartMode(pModule, &start_mode);
	pInfo->StartLevel = start_mode.level;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	BRDCHAR Buffer[128];
	BRDCHAR ParamName[128];
	BRDCHAR* endptr;
/*
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus;
	do {
		pDev->RegCtrl(DEVScmd_REGREADDIR, &param);
		pStatus = (PADM2IF_STATUS)&param.val;
	} while(!pStatus->ByBits.CmdRdy);
	char msg[256];
	sprintf(msg, "val = %X", param.val);
	MessageBox(NULL, msg, "Debug1", MB_OK);
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pDev->RegCtrl(DEVScmd_REGREADDIR, &param);
	sprintf(msg, "val = %X", param.val);
	MessageBox(NULL, msg, "Debug2", MB_OK);
*/
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);

	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;

	GetPrivateProfileString(SectionName, _BRDC("PllMuxOut"), _BRDC("2"), Buffer, sizeof(Buffer), iniFilePath);
	pSrvCfg->PllMuxOut = BRDC_atoi(Buffer);
	SetMuxOut(pDev, pSrvCfg->PllMuxOut);

	//GetPrivateProfileString(SectionName, "ExternalPllReference", "50000000.0", Buffer, sizeof(Buffer), iniFilePath);
	//BRD_Adc10x2gPll pllMode;
	//pllMode.ExtRef = atof(Buffer);
	//SetPllMode(pDev, &pllMode);

	GetPrivateProfileString(SectionName, _BRDC("ClockSource"), _BRDC("129"), Buffer, sizeof(Buffer), iniFilePath);
//	ULONG clkSrc = atoi(Buffer);
	ULONG clkSrc = BRDC_strtol(Buffer, &endptr, 0);
	SetClkSource(pDev, clkSrc);
	GetPrivateProfileString(SectionName, _BRDC("ExternalClockValue"), _BRDC("120000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	double clkValue = BRDC_atof(Buffer);
	PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	pAdcSrvCfg->SubExtClk = ROUND(clkValue);
	SetClkValue(pDev, clkSrc, clkValue);
	GetPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("100000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	double rate = BRDC_atof(Buffer);
	SetRate(pDev, rate, clkSrc, clkValue);
	GetPrivateProfileString(SectionName, _BRDC("ClockInv"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	ULONG clkInv = BRDC_atoi(Buffer);
	SetClkInv(pDev, clkInv);

	double InpRange[MAX_CHAN], Bias[MAX_CHAN], GainTuning[MAX_CHAN];
	for(int i = 0; i < MAX_CHAN; i++)
	{
		if(pSrvCfg->IsRF)
		{
			BRDC_sprintf(ParamName, _BRDC("Gain%d"), i);
			GetPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
			InpRange[i] = BRDC_atof(Buffer);
			SetGain(pDev, InpRange[i], i);
		}
		else
		{
			BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
			GetPrivateProfileString(SectionName, ParamName, _BRDC("0.5"), Buffer, sizeof(Buffer), iniFilePath);
			InpRange[i] = BRDC_atof(Buffer);
			SetInpRange(pDev, InpRange[i], i);
		}
		BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
		GetPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		Bias[i] = BRDC_atof(Buffer);
		SetBias(pDev, Bias[i], i);
		BRDC_sprintf(ParamName, _BRDC("GainTuning%d"), i);
		GetPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		GainTuning[i] = BRDC_atof(Buffer);
		SetGainTuning(pDev, GainTuning[i], i);
	}

	BRD_AdcStartMode start;
	GetPrivateProfileString(SectionName, _BRDC("StartSource"), _BRDC("3"), Buffer, sizeof(Buffer), iniFilePath);
	start.src = BRDC_atoi(Buffer);
	GetPrivateProfileString(SectionName, _BRDC("StartInverting"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	start.inv = BRDC_atoi(Buffer);//0;
	GetPrivateProfileString(SectionName, _BRDC("StartLevel"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	start.level = BRDC_atof(Buffer);//0.0;
	CAdcSrv::GetStartMode(pDev, &start.stndStart);
	//if(start.src != BRDsts_ADC_PRG)
	//	start.stndStart.startSrc = BRDsts_SUBMOD;
	//else
	//{
	//	if(start.stndStart.startSrc != BRDsts_BASEDDS)
	//		start.stndStart.startSrc = BRDsts_PRG;
	//}
	//start.stndStart.startInv = 0;
	//start.stndStart.trigOn = 1;
	//start.stndStart.trigStopSrc = BRDsts_PRG;
	//start.stndStart.stopInv = 0;
	//GetPrivateProfileString(SectionName, "ReStart", "0", Buffer, sizeof(Buffer), iniFilePath);
	//start.stndStart.reStartMode = atoi(Buffer);
	SetStartMode(pDev, &start);

	GetPrivateProfileString(SectionName, _BRDC("ClkOutPosn"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
	UCHAR regval = (UCHAR)BRDC_strtol(Buffer, &endptr, 0);
	SetAdcReg(pDev, 0x62, regval);

	GetPrivateProfileString(SectionName, _BRDC("Gain"), _BRDC("8"), Buffer, sizeof(Buffer), iniFilePath);
	regval = (UCHAR)BRDC_strtol(Buffer, &endptr, 0);
	SetAdcReg(pDev, 0x68, regval);

	GetPrivateProfileString(SectionName, _BRDC("ClkGain"), _BRDC("32"), Buffer, sizeof(Buffer), iniFilePath);
	regval = (UCHAR)BRDC_strtol(Buffer, &endptr, 0);
	SetAdcReg(pDev, 0x6B, regval);

	PADC_CUR_TERM pRegCurTerm = (PADC_CUR_TERM)&pSrvCfg->AdcReg[0x0E];
	GetPrivateProfileString(SectionName, _BRDC("LvdsCurrent"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	pRegCurTerm->ByBits.Current = (UCHAR)BRDC_strtol(Buffer, &endptr, 0);
	GetPrivateProfileString(SectionName, _BRDC("ClkTerm"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	pRegCurTerm->ByBits.ClkTerm = (UCHAR)BRDC_strtol(Buffer, &endptr, 0);
	GetPrivateProfileString(SectionName, _BRDC("DataTerm"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	pRegCurTerm->ByBits.DataTerm = (UCHAR)BRDC_strtol(Buffer, &endptr, 0);
	SetAdcReg(pDev, 0x7E, pRegCurTerm->AsWhole);

	GetPrivateProfileString(SectionName, _BRDC("CurrentDouble"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
	regval = (UCHAR)BRDC_strtol(Buffer, &endptr, 0);
	regval <<= 6;
	SetAdcReg(pDev, 0x7F, regval);

	PADC_REF_PWR pRegRefPwr = (PADC_REF_PWR)&pSrvCfg->AdcReg[0x0D];
	GetPrivateProfileString(SectionName, _BRDC("Ref"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	pRegRefPwr->ByBits.Ref = (UCHAR)BRDC_strtol(Buffer, &endptr, 0);
	GetPrivateProfileString(SectionName, _BRDC("PowerScalling"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
	pRegRefPwr->ByBits.PwrScal = (UCHAR)BRDC_strtol(Buffer, &endptr, 0);
	SetAdcReg(pDev, 0x6D, pRegRefPwr->AsWhole);

	GetPrivateProfileString(SectionName, _BRDC("TestPattern"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	regval = (UCHAR)BRDC_strtol(Buffer, &endptr, 0);
	regval <<= 5;
	SetAdcReg(pDev, 0x65, regval);
	GetPrivateProfileString(SectionName, _BRDC("CustomPattern"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	int val = BRDC_strtol(Buffer, &endptr, 0);
	SetAdcReg(pDev, 0x69, (UCHAR)val);
	SetAdcReg(pDev, 0x6A, (UCHAR)(val >> 8));

	// for ADM214x250M
	if(pSrvCfg->Version >= 0x50)
	{
		val = 0;
		SetAdcReg(pDev, 0x20, (UCHAR)val);
		SetAdcReg(pDev, 0x3F, (UCHAR)val);
		SetAdcReg(pDev, 0x68, (UCHAR)val);
		SetAdcReg(pDev, 0x62, (UCHAR)val);
		SetAdcReg(pDev, 0x63, (UCHAR)val);
		GetPrivateProfileString(SectionName, _BRDC("Gain"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
		regval = (UCHAR)BRDC_strtol(Buffer, &endptr, 0);
		pSrvCfg->AdcRegGain = regval;
		regval <<= 4;
		SetAdcReg(pDev, 0x55, regval);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	//TCHAR Buffer[128];
	BRDCHAR ParamName[128];
	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;

	//sprintf_s(Buffer, "%u", pSrvCfg->AdcCfg.Encoding);
	//WritePrivateProfileString(SectionName, "CodeType", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("CodeType"), (ULONG)pSrvCfg->AdcCfg.Encoding, 10, NULL);

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

	ULONG clkInv;
	GetClkInv(pDev, clkInv);
	//sprintf_s(Buffer, "%u", clkInv);
	//WritePrivateProfileString(SectionName, "ClockInv", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ClockInv"), clkInv, 10, NULL);

	double InpRange[MAX_CHAN], Bias[MAX_CHAN], GainTuning[MAX_CHAN];
	for(int i = 0; i < MAX_CHAN; i++)
	{
		if(pSrvCfg->IsRF)
		{
			GetGain(pDev, InpRange[i], i);
			BRDC_sprintf(ParamName, _BRDC("Gain%d"), i);
			WriteInifileParam(iniFilePath, SectionName, ParamName, InpRange[i], 2, NULL);
		}
		else
		{
			GetInpRange(pDev, InpRange[i], i);
			BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
			WriteInifileParam(iniFilePath, SectionName, ParamName, InpRange[i], 2, NULL);
		}
		GetBias(pDev, Bias[i], i);
		BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
		//sprintf_s(Buffer, "%.4f", Bias[i]);
		//WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		WriteInifileParam(iniFilePath, SectionName, ParamName, Bias[i], 4, NULL);
		GetGainTuning(pDev, GainTuning[i], i);
		BRDC_sprintf(ParamName, _BRDC("GainTuning%d"), i);
		//sprintf_s(Buffer, "%.4f", GainTuning[i]);
		//WritePrivateProfileString(SectionName, ParamName, Buffer, iniFilePath);
		WriteInifileParam(iniFilePath, SectionName, ParamName, GainTuning[i], 4, NULL);
	}

	BRD_AdcStartMode start;
	GetStartMode(pDev, &start);

	//sprintf_s(Buffer, "%u", start.src);
	//WritePrivateProfileString(SectionName, "StartSource", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartSource"), (ULONG)start.src, 10, NULL);
	//sprintf_s(Buffer, "%u", start.inv);
	//WritePrivateProfileString(SectionName, "StartInverting", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartInverting"), (ULONG)start.inv, 10, NULL);
	//sprintf_s(Buffer, "%.2f", start.level);
	//WritePrivateProfileString(SectionName, "StartLevel", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartLevel"), start.level, 2, NULL);

	//sprintf_s(Buffer, "%u", start.stndStart.startSrc);
	//WritePrivateProfileString(SectionName, "StartBaseSource", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartBaseSource"), (ULONG)start.stndStart.startSrc, 10, NULL);
	//sprintf_s(Buffer, "%u", start.stndStart.startInv);
	//WritePrivateProfileString(SectionName, "StartBaseInverting", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartBaseInverting"), (ULONG)start.stndStart.startInv, 10, NULL);
	//sprintf_s(Buffer, "%u", start.stndStart.trigOn);
	//WritePrivateProfileString(SectionName, "StartMode", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartMode"), (ULONG)start.stndStart.trigOn, 10, NULL);
	//sprintf_s(Buffer, "%u", start.stndStart.trigStopSrc);
	//WritePrivateProfileString(SectionName, "StopSource", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StopSource"), (ULONG)start.stndStart.trigStopSrc, 10, NULL);
	//sprintf_s(Buffer, "%u", start.stndStart.stopInv);
	//WritePrivateProfileString(SectionName, "StopInverting", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StopInverting"), (ULONG)start.stndStart.stopInv, 10, NULL);
	//sprintf_s(Buffer, "%u", start.stndStart.reStartMode);
	//WritePrivateProfileString(SectionName, "ReStart", Buffer, iniFilePath);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ReStart"), (ULONG)start.stndStart.reStartMode, 10, NULL);

	// for ADM214x250M
	if(pSrvCfg->Version >= 0x50)
	{
		WriteInifileParam(iniFilePath, SectionName, _BRDC("Gain"), (ULONG)pSrvCfg->AdcRegGain, 10, NULL);
	}
	// the function flushes the cache
	WritePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::SetClkSource(CModule* pModule, ULONG ClkSrc)
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
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADM2IF_FSRC pFsrc = (PADM2IF_FSRC)&param.val;
		pFsrc->ByBits.Gen = ClkSrc;
		//param.val = ClkSrc;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.reg = ADM2IFnr_FSRC;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
			PADM2IF_FSRC pFsrc = (PADM2IF_FSRC)&param.val;
			pFsrc->ByBits.Gen = ClkSrc;
			//param.val = ClkSrc;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		else
			return BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
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
		PADM2IF_FSRC pFsrc = (PADM2IF_FSRC)&param.val;
		source = pFsrc->ByBits.Gen;
		//source = param.val;
	}
	else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.reg = ADM2IFnr_FSRC;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
			PADM2IF_FSRC pFsrc = (PADM2IF_FSRC)&param.val;
			source = pFsrc->ByBits.Gen;
			//source = param.val;
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
int CAdc214x200mSrv::SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:		// disabled clock
		ClkValue = 0.0;
		break;
	case BRDclks_ADC_SUBGEN:
		{
		ClkValue = pAdcSrvCfg->PllRefGen;
		double clk_tmp = (double)pAdcSrvCfg->PllFreq;
		OnPll(pModule, clk_tmp, pAdcSrvCfg->PllRefGen);
		}
		break;
	case BRDclks_ADC_REFPLL:			// SubModule Clock (internal reference)
		ClkValue = pAdcSrvCfg->PllFreq;
		OnPll(pModule, ClkValue, pAdcSrvCfg->PllRefGen);
		pAdcSrvCfg->PllFreq = (U32)ClkValue;
		break;
	//case BRDclks_ADC_INTREFPLL:			// SubModule Clock (internal reference)
	//	ClkValue = pAdcSrvCfg->PllFreq;
	//	OnPll(pModule, ClkValue, pAdcSrvCfg->PllRefGen);
	//	break;
	//case BRDclks_ADC_EXTREFPLL:			// SubModule Clock (external reference - EXT REF IN)
	//	ClkValue = pAdcSrvCfg->PllFreq;
	//	OnPll(pModule, ClkValue, pAdcSrvCfg->ExtPllRef);
	//	break;
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
int CAdc214x200mSrv::GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	double ClkVal;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:		// disabled clock
		ClkVal = 0.0;
		break;
	case BRDclks_ADC_SUBGEN:
		ClkVal = pAdcSrvCfg->PllRefGen;
		break;
	case BRDclks_ADC_REFPLL:			// SubModule Clock (internal reference)
//	case BRDclks_ADC_INTREFPLL:			// SubModule Clock (internal reference)
//	case BRDclks_ADC_EXTREFPLL:			// SubModule Clock (external reference - EXT REF IN)
		ClkVal = pAdcSrvCfg->PllFreq;
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
int M214x200M_CLK_DIVIDER[BRD_CLKDIVCNT] = {1, 2, 4, 8, 16, 32, 64, 128, 256};

//****************************************************************************************
int SetClkDivOpt(double RateHz, double curClk, double minRate)
{
	if(!curClk)
		return 1;
	int MaxDivider = (int)floor(curClk / minRate);
	int i = 0;
	for(i = BRD_CLKDIVCNT-1; i >= 0; i--)
		if(MaxDivider >= M214x200M_CLK_DIVIDER[i])
			break;
	int MaxInd = (i < 0) ? 0 : i;
	MaxDivider = M214x200M_CLK_DIVIDER[MaxInd];
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
				return M214x200M_CLK_DIVIDER[MaxInd];
			}
		}
		else 
		{
			for(int i = 0; i < MaxInd; i++) 
			{
				double RateLeft = curClk / M214x200M_CLK_DIVIDER[i];
				double RateRight = curClk / M214x200M_CLK_DIVIDER[i+1];
				if(RateHz <= RateLeft && RateHz >= RateRight)
				{
					if(RateLeft - RateHz <= RateHz - RateRight)
					{
						if (DeltaFreq > RateLeft - RateHz)
						{
							DeltaFreq = RateLeft - RateHz;
							return M214x200M_CLK_DIVIDER[i];
						}
					}
					else
					{
						if (DeltaFreq > RateHz - RateRight)
						{
							DeltaFreq = RateHz - RateRight;
							return M214x200M_CLK_DIVIDER[i+1];
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
int CAdc214x200mSrv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
	ULONG AdcRateDivider;
//	if(ClkSrc != BRDclks_ADC_EXTCLK)
//	{
		DeltaFreq = 1.e10;
		PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
		if(Rate > pSrvCfg->AdcCfg.MaxRate)
			Rate = pSrvCfg->AdcCfg.MaxRate;
		//if(Rate < pSrvCfg->AdcCfg.MinRate)
		//	Rate = pSrvCfg->AdcCfg.MinRate;
		AdcRateDivider = SetClkDivOpt(Rate, ClkValue, pSrvCfg->AdcCfg.MinRate);
//	}
//	else
//		AdcRateDivider = 1;

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
int CAdc214x200mSrv::GetRate(CModule* pModule, double& Rate, double ClkValue)
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
int CAdc214x200mSrv::SetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	if(pSrvCfg->IsRF && pSrvCfg->Version >= 0x50)
	{
		int regval = 0x40;
		SetAdcRegVal(pModule, Chan, 0x53, regval);
		regval = (int)floor(Bias);
		if(regval < -32) regval = -32;
		if(regval > 31) regval = 31;
		pSrvCfg->AdcRegBias = regval & 0x3F;
		SetAdcRegVal(pModule, Chan, 0x63, regval);
		return Status;
	}


	double inp_range;
	Status = GetInpRange(pModule, inp_range, Chan); 
	if(Status != BRDerr_OK)
		return Status;
	double prec_range = inp_range / 10; // 10% from full range

	int max_dac_value = 255;
	int min_dac_value = 0;
	double half_dac_value = 128.;

	USHORT approx_data;
	if(fabs(Bias) > inp_range)
	{
		approx_data = Bias > 0.0 ? max_dac_value : min_dac_value;
		Bias = inp_range;
	}
	else
		approx_data = (USHORT)floor((Bias / inp_range + 1.) * half_dac_value + 0.5);
	if(approx_data > max_dac_value)
		approx_data = max_dac_value;
	double approx_bias = inp_range * (approx_data / half_dac_value - 1.);
	double delta_bias = Bias - approx_bias;

	USHORT prec_data;
	if(fabs(delta_bias) > prec_range)
		prec_data = delta_bias > 0.0 ? max_dac_value : min_dac_value;
	else
		prec_data = (USHORT)floor((delta_bias / prec_range + 1.) * half_dac_value + 0.5);
	if(prec_data > max_dac_value)
		prec_data = max_dac_value;
	double prec_bias = prec_range * (prec_data / half_dac_value - 1.);

	Bias = approx_bias + prec_bias;

	pSrvCfg->Bias[Chan] = Bias;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC214X200M_BIAS0 - 1] = (UCHAR)approx_data;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC214X200M_PRECBIAS0 - 1] = (UCHAR)prec_data;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = approx_data;
	pThDac->ByBits.Num = Chan + BRDtdn_ADC214X200M_BIAS0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	pThDac->ByBits.Data = prec_data;
	pThDac->ByBits.Num = Chan + BRDtdn_ADC214X200M_PRECBIAS0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

//***************************************************************************************
int CAdc214x200mSrv::GetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	if(pSrvCfg->IsRF && pSrvCfg->Version >= 0x50)
		Bias = 0.0;
	else
		Bias = pSrvCfg->Bias[Chan];
	return Status;
}

const double BRD_ADC_MAXGAINTUN	= 2.; // max gain tuning = 2%
const double BRD_ADC_MAXGAINTUNRF = 1.; // max gain tuning = 1 dB
//***************************************************************************************
int CAdc214x200mSrv::SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	double max_thr = BRD_ADC_MAXGAINTUN; // %
	if(pSrvCfg->IsRF)
		max_thr = BRD_ADC_MAXGAINTUNRF; // dB
//	double max_thr = pSrvCfg->RefPVS / 1000.;

	//if(WaitCmdReady(pModule, m_index, m_MainTetrNum, 1000))
	//	return BRDerr_WAIT_TIMEOUT;

	USHORT dac_data;
	if(fabs(GainTuning) > max_thr)
		dac_data = GainTuning > 0.0 ? 255 : 0;
//		dac_data = GainTuning > 0.0 ? 0 : 255;
	else
		dac_data = (USHORT)floor((GainTuning / max_thr + 1.) * 128. + 0.5);
//		dac_data = (USHORT)floor((1. - GainTuning / max_thr) * 128. + 0.5);
	if(dac_data > 255)
		dac_data = 255;
	GainTuning = max_thr * (dac_data / 128. - 1.);
//	GainTuning = max_thr * (1. - dac_data / 128.);

	pSrvCfg->GainTun[Chan] = GainTuning;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC214X200M_GAINTUN0 - 1] = (UCHAR)dac_data;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = Chan + BRDtdn_ADC214X200M_GAINTUN0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
//	pValChan->value = pSrvCfg->GainTun;
//	pValChan->chan = 0;
//	pValChan->value = pSrvCfg->ClkPhase[pValChan->chan];
	GainTuning = pSrvCfg->GainTun[Chan];
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::SetStartMode(CModule* pModule, PVOID pStartStopMode)
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

	pStartMode->startInv = pAdcStartMode->inv;
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
int CAdc214x200mSrv::GetStartMode(CModule* pModule, PVOID pStartStopMode)
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
	pAdcStartMode->inv = pStartMode->startInv;
	return Status;
}

//***************************************************************************************
int CAdc214x200mSrv::SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr)
{
//	MessageBox(NULL, "SetAdcStartMode", "Debug", MB_OK);

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.StartSrc = source;
//	pCtrl->ByBits.StartInv = inv; // через STMODE
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

	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.;

	double value = *pCmpThr;
	int max_code = 255;//ADC214X200M_STARTEXTMAXCODE;
	int min_code = 0;//ADC214X200M_STARTEXTMINCODE;
	if(BRDsts_ADC_CHAN0 == source || BRDsts_ADC_CHAN1 == source)
	{
		//double koef = ADC214X200M_STARTCHANKOEF;
		//max_code = ADC214X200M_STARTCHANMAXCODE;
		//min_code = ADC214X200M_STARTCHANMINCODE;
		//value = *pCmpThr * koef / inp_range;
		value = (*pCmpThr / inp_range) * max_thr;
	}

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
		//double koef = ADC214X200M_STARTCHANKOEF;
		//*pCmpThr = value * inp_range / koef;
		*pCmpThr = (value * inp_range) / max_thr;
	}

	pSrvCfg->StCmpThr = *pCmpThr;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC214X200M_STARTCMP - 1] = (UCHAR)dac_data;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC214X200M_STARTCMP;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	*pSource = pCtrl->ByBits.StartSrc;
	*pInv = pCtrl->ByBits.StartInv;
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	*pCmpThr = pSrvCfg->StCmpThr;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::SetClkLocation(CModule* pModule, ULONG& mode)
{
	mode = 1;
	CAdcSrv::SetClkLocation(pModule, mode);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::GetClkLocation(CModule* pModule, ULONG& mode)
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
int CAdc214x200mSrv::SetGain(CModule* pModule, double& Gain, ULONG Chan)
{
	int Status = BRDerr_OK;

	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	if(pSrvCfg->IsRF)
		return SetGainRF(pModule, Gain, Chan);

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
int CAdc214x200mSrv::GetGain(CModule* pModule, double& Gain, ULONG Chan)
{
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	if(pSrvCfg->IsRF)
		return GetGainRF(pModule, Gain, Chan);

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
int CAdc214x200mSrv::SetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
{
	int Status = BRDerr_OK;

	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	if(pSrvCfg->IsRF)
		return BRDerr_CMD_UNSUPPORTED;

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
int CAdc214x200mSrv::GetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
{
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	if(pSrvCfg->IsRF)
	{
		double gain;
		GetGainRF(pModule, gain, Chan);
		InpRange = (pSrvCfg->AdcCfg.InpRange / pow(10., gain/20)) / 1000.;
		return BRDerr_OK;
	}
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
int CAdc214x200mSrv::SetCode(CModule* pModule, ULONG type)
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
int CAdc214x200mSrv::GetCode(CModule* pModule, ULONG& type)
{
	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;
	//param.tetr = m_AdcTetrNum;
	//param.reg = ADM2IFnr_FORMAT;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//PADM2IF_FORMAT pFormat = (PADM2IF_FORMAT)&param.val;
	//type = pFormat->ByBits.Code;

	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	type = pSrvCfg->AdcCfg.Encoding;

//	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	CAdcSrv::SetChanMask(pModule, mask);
	Sleep(200);
	Sleep(200);
	Sleep(200);
	return BRDerr_OK;
}
//***************************************************************************************
int CAdc214x200mSrv::SetMaster(CModule* pModule, ULONG mode)
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
int CAdc214x200mSrv::ExtraInit(CModule* pModule)
{
	PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	for(int i = 0; i < 16; i++)
		pAdcSrvCfg->AdcReg[i] = 0;

	//UCHAR regval = 2;
	//SetAdcReg(pDev, 0x6C, regval);
	//regval = 8;
	//SetAdcReg(pDev, 0x6C, regval);

	InitPLL(pModule);
	return BRDerr_OK;
}

//****************************************************************************************
//int CAdc214x200mSrv::readPllData(CModule* pModule, USHORT& HiWord, USHORT& LoWord)
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
int CAdc214x200mSrv::writePllData(CModule* pModule, USHORT HiWord, USHORT LoWord)
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
int CAdc214x200mSrv::InitPLL(CModule* pModule)
{
	PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG )m_pConfig;
	pAdcSrvCfg->Pll_R_CNT.AsWhole = 0;
	pAdcSrvCfg->Pll_N_CNT.AsWhole = 0;
	pAdcSrvCfg->Pll_Func.AsWhole = 0;

	pAdcSrvCfg->Pll_R_CNT.ByBits.Addr = 0;
	pAdcSrvCfg->Pll_N_CNT.ByBits.Addr = 1;
	pAdcSrvCfg->Pll_Func.ByBits.Addr = 2;

	pAdcSrvCfg->Pll_R_CNT.ByBits.AB_WIDTH = 1; //1.3 ns
	pAdcSrvCfg->Pll_R_CNT.ByBits.TEST_MODE = 0;
	pAdcSrvCfg->Pll_R_CNT.ByBits.LDP = 0;
	//pAdcSrvCfg->Pll_R_CNT.ByBits.R_CNTR = 1;
	writePllData(pModule, pAdcSrvCfg->Pll_R_CNT.ByWord.HiWord, pAdcSrvCfg->Pll_R_CNT.ByWord.LoWord);

	pAdcSrvCfg->Pll_N_CNT.ByBits.CP_GAIN = 1;
	//pAdcSrvCfg->Pll_N_CNT.ByBits.N_CNTR = 5;
	writePllData(pModule, pAdcSrvCfg->Pll_N_CNT.ByWord.HiWord, pAdcSrvCfg->Pll_N_CNT.ByWord.LoWord);

	pAdcSrvCfg->Pll_Func.ByBits.CNT_RST = 0;
	pAdcSrvCfg->Pll_Func.ByBits.PWDN_1 = 0;
	pAdcSrvCfg->Pll_Func.ByBits.PWDN_2 = 0;
	pAdcSrvCfg->Pll_Func.ByBits.MUX_OUT = 2;
	pAdcSrvCfg->Pll_Func.ByBits.PD_POL = 1;
	pAdcSrvCfg->Pll_Func.ByBits.CP_3STATE = 0;
	pAdcSrvCfg->Pll_Func.ByBits.FL_ENBL = 0;
	pAdcSrvCfg->Pll_Func.ByBits.FL_MODE = 0;
	pAdcSrvCfg->Pll_Func.ByBits.TIMER_CNT = 2;
	pAdcSrvCfg->Pll_Func.ByBits.CUR_SET_1 = 3;
	pAdcSrvCfg->Pll_Func.ByBits.CUR_SET_2 = 7;

	writePllData(pModule, pAdcSrvCfg->Pll_Func.ByWord.HiWord, pAdcSrvCfg->Pll_Func.ByWord.LoWord);

	return BRDerr_OK;
}
/*
//****************************************************************************************
int CAdc214x200mSrv::OffPLL(CModule* pModule)
{
//	pAdcSrvCfg->Pll_RF_R pAdcSrvCfg->Pll_RF_R;
//	readPllData(pModule, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord, pAdcSrvCfg->Pll_RF_R.ByWord.LoWord);
	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	pAdcSrvCfg->Pll_RF_R.ByBits.V2_EN = 0;
	writePllData(pModule, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord, pAdcSrvCfg->Pll_RF_R.ByWord.LoWord);

	return BRDerr_OK;
}
*/
//****************************************************************************************
int CAdc214x200mSrv::OnPll(CModule* pModule, double& ClkValue, double PllRefValue)
{
	PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG )m_pConfig;

	double f_vco = ClkValue;
	double f_osc = PllRefValue;

	double f_cmp_min;
	int R_max = 16383;
	int N_max = 8191;
	double nn_tmp0 = f_vco / N_max;
	double nn_tmp1 = f_osc / R_max;
	if(nn_tmp0 > nn_tmp1)
		f_cmp_min = nn_tmp0;
	else
		f_cmp_min = nn_tmp1;
	if(f_cmp_min < PLL_F_CMP_MIN)
		f_cmp_min = PLL_F_CMP_MIN;
	R_max = int(floor(f_osc / f_cmp_min));
	int R_min = int(ceil(f_osc / PLL_F_CMP_MAX));
	if(R_min < 1) R_min = 1;
	int R, N_tmp, N = 2000;
	//int delta_min = 100; // 1G
//	double delta_min = 10000000; // 2G
	double delta_min = (f_osc - f_cmp_min) / f_osc;
	for(int R_tmp = R_min; R_tmp <= R_max; R_tmp++)
	{
		N_tmp = int(f_vco * R_tmp / f_osc);
		double f_new = N_tmp * (f_osc / R_tmp);
		//int delta = int((fabs(pAdcSrvCfg->PllFreq - f_new) / pAdcSrvCfg->PllFreq) * 100); // 1G
//		double delta = (fabs(pAdcSrvCfg->PllFreq - f_new) / pAdcSrvCfg->PllFreq) * 10000000; // 2G
		double delta = (fabs(pAdcSrvCfg->PllFreq - f_new) / pAdcSrvCfg->PllFreq); // 200M
		if(delta < delta_min)
		{
			ClkValue = f_new;
			delta_min = delta;
			N = N_tmp;
			R = R_tmp;
			if(delta_min == 0)
				break;
		}
	}

	pAdcSrvCfg->Pll_R_CNT.ByBits.R_CNTR = R;
	writePllData(pModule, pAdcSrvCfg->Pll_R_CNT.ByWord.HiWord, pAdcSrvCfg->Pll_R_CNT.ByWord.LoWord);
	pAdcSrvCfg->Pll_N_CNT.ByBits.N_CNTR = N;
	writePllData(pModule, pAdcSrvCfg->Pll_N_CNT.ByWord.HiWord, pAdcSrvCfg->Pll_N_CNT.ByWord.LoWord);

	writePllData(pModule, pAdcSrvCfg->Pll_Func.ByWord.HiWord, pAdcSrvCfg->Pll_Func.ByWord.LoWord);
	return BRDerr_OK;
}

//****************************************************************************************
int CAdc214x200mSrv::SetMuxOut(CModule* pModule, U08 muxout)
{
	if(muxout < 0 || muxout > 7)
		return BRDerr_BAD_PARAMETER;
	PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG )m_pConfig;
	pAdcSrvCfg->Pll_Func.ByBits.MUX_OUT = muxout;
	writePllData(pModule, pAdcSrvCfg->Pll_Func.ByWord.HiWord, pAdcSrvCfg->Pll_Func.ByWord.LoWord);

	return BRDerr_OK;
}

// запись в оба АЦП
//***************************************************************************************
int CAdc214x200mSrv::SetAdcReg(CModule* pModule, int regNum, UCHAR regVal)
{
//	int Status = BRDerr_CMD_UNSUPPORTED;
	int Status = BRDerr_OK;

	PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	int num = regNum & 0xf;
	pAdcSrvCfg->AdcReg[num] = regVal;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADC214X200Mnr_REGADDR;
	param.val = regNum | 0x8000;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADC214X200Mnr_REGDATA;
	param.val = regVal;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

// запись в конкретный АЦП
//***************************************************************************************
int CAdc214x200mSrv::SetAdcRegVal(CModule* pModule, int adcNum, int regNum, UCHAR val)
{
	int Status = BRDerr_OK;

	PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	int num = regNum & 0xf;
	pAdcSrvCfg->AdcReg[num] = val;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADC214X200Mnr_REGADDR;
	param.val = regNum | (adcNum << 12);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADC214X200Mnr_REGDATA;
	param.val = val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

//***************************************************************************************
int CAdc214x200mSrv::SetClkInv(CModule* pModule, ULONG Inv)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_FSRC;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_FSRC pFsrc = (PADM2IF_FSRC)&param.val;
	pFsrc->ByBits.Inv = Inv;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::GetClkInv(CModule* pModule, ULONG& Inv)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_FSRC;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_FSRC pFsrc = (PADM2IF_FSRC)&param.val;
	Inv = pFsrc->ByBits.Inv;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::SetMu(CModule* pModule, void	*args)
{
	PADC214X200MSRV_MU pMU = (PADC214X200MSRV_MU)args;

	CAdcSrv::SetChanMask(pModule, pMU->chanMask);
	SetMaster(pModule, pMU->syncMode);

	CtrlFormat(pModule, NULL, BRDctrl_ADC_SETFORMAT, &pMU->format);

	//PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	SetClkSource(pModule, pMU->clockSrc);
	SetClkValue(pModule, pMU->clockSrc, pMU->clockValue);
	SetRate(pModule, pMU->samplingRate, pMU->clockSrc, pMU->clockValue);

	SetClkInv(pModule, pMU->invClk);

	for(int i = 0; i < MAX_CHAN; i++)
	{
		SetInpRange(pModule, pMU->range[i], i);
		SetBias(pModule, pMU->bias[i], i);
		SetInpResist(pModule, pMU->resist[i], i);
		SetDcCoupling(pModule, pMU->dcCoupling[i], i);
		SetGainTuning(pModule, pMU->gainTuning[i], i);
	}

	BRD_AdcStartMode start_mode;
	start_mode.src					= pMU->startSrc;
	start_mode.inv					= pMU->invStart;
	start_mode.level				= pMU->startLevel;
	start_mode.stndStart.startSrc	= pMU->baseStartSrc;
	start_mode.stndStart.startInv	= pMU->baseStartInv;
	start_mode.stndStart.trigOn		= pMU->startStopMode;
	start_mode.stndStart.trigStopSrc= pMU->baseStopSrc;
	start_mode.stndStart.stopInv	= pMU->baseStopInv;
	start_mode.stndStart.reStartMode = pMU->reStart;
	SetStartMode(pModule, &start_mode);
	pMU->startLevel = start_mode.level;

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

	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	pSrvCfg->PllMuxOut = 2;
	SetMuxOut(pModule, pSrvCfg->PllMuxOut);

	UCHAR regval = 1;
	SetAdcReg(pModule, 0x62, regval); // ClkOutPosn
	regval = 8;
	SetAdcReg(pModule, 0x68, regval); // Gain
	regval = 32;
	SetAdcReg(pModule, 0x6B, regval); // ClkGain
	regval = 0;
	SetAdcReg(pModule, 0x7E, regval); // LvdsCurrent, ClkTerm, DataTerm
	regval = 1 << 6;
	SetAdcReg(pModule, 0x7F, regval); // CurrentDouble

	regval = 1 << 5;
	SetAdcReg(pModule, 0x6D, regval); // Ref, PowerScalling

	regval = 0;
	SetAdcReg(pModule, 0x65, regval); // TestPattern

	SetAdcReg(pModule, 0x69, regval); // CustomPattern - low byte
	SetAdcReg(pModule, 0x6A, regval); // CustomPattern - most byte

	// for ADM214x250M
	if(pSrvCfg->Version >= 0x50)
	{
		SetAdcReg(pModule, 0x20, regval);
		SetAdcReg(pModule, 0x3F, regval);
		SetAdcReg(pModule, 0x68, regval);
		SetAdcReg(pModule, 0x62, regval);
		SetAdcReg(pModule, 0x63, regval);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec)
{
//	int Status = BRDerr_CMD_UNSUPPORTED;
	int Status = BRDerr_OK;
	CAdcSrv::SetSpecific(pModule, pSpec);
	DEVS_CMD_Reg param;
	param.idxMain = m_index;

	switch(pSpec->command)
	{
	case ADC214X200Mcmd_SETMU:
		{
			SetMu(pModule, pSpec->arg);
		}
		break;
	case ADC214X200Mcmd_SETGAIN:
		{
			UCHAR regval = *(PUCHAR)pSpec->arg;
			PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
			pSrvCfg->AdcRegGain = regval;
			regval <<= 4;
			SetAdcReg(pModule, 0x55, regval);
		}
		break;
	case ADC214X200Mcmd_SETCHANGAIN:
		{
			PBRD_ValChan pValChan = (PBRD_ValChan)pSpec->arg;
			//UCHAR regval = *(PUCHAR)pSpec->arg;
			PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
			//pSrvCfg->AdcRegGain = regval;
			UCHAR regval = (UCHAR)pValChan->value;
			regval <<= 4;
			SetAdcRegVal(pModule, pValChan->chan, 0x55, regval);
		}
		break;
	//case ADC214X200Mcmd_SETBIAS:
	//	{
	//		UCHAR regval = *(PUCHAR)pSpec->arg;
	//		PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	//		pSrvCfg->AdcRegBias = regval;
	//		SetAdcReg(pModule, 0x63, regval);
	//	}
	//	break;
	}
	return Status;
}

//***************************************************************************************
int CAdc214x200mSrv::SetDcCoupling(CModule* pModule, ULONG InpType, ULONG Chan)
{
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;

	if(pSrvCfg->IsRF)
		CAdcSrv::SetDcCoupling(pModule, 0, Chan);
	else
		CAdcSrv::SetDcCoupling(pModule, InpType, Chan);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan)
{
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;

	if(pSrvCfg->IsRF)
		CAdcSrv::SetInpResist(pModule, 1, Chan);
	else
		CAdcSrv::SetInpResist(pModule, InpRes, Chan);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::SetGainRF(CModule* pModule, double& Gain, ULONG Chan)
{
	int Status = BRDerr_OK;
	ULONG mask;
	if(Gain > 20.)
	{
		Status = BRDerr_ADC_ILLEGAL_GAIN | BRDerr_INFO;
		mask = 0;
	}
	else
	{
		if(Gain < -4.)
		{
			Status = BRDerr_ADC_ILLEGAL_GAIN | BRDerr_INFO;
			mask = 24;
		}
		else 
			mask = ULONG(20 - Gain);
	}
	Gain = 20. - mask;
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
		break;
	}
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return Status;
}

// возвращает Gain в дБ (от -4 до 20)
//***************************************************************************************
int CAdc214x200mSrv::GetGainRF(CModule* pModule, double& Gain, ULONG Chan)
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
		if(pGain->ByBits.Chan0 > 24)
			pGain->ByBits.Chan0 = 24;
		value = 20. - pGain->ByBits.Chan0;
		break;
	case 1:
		if(pGain->ByBits.Chan1 > 24)
			pGain->ByBits.Chan1 = 24;
		value = 20. - pGain->ByBits.Chan1;
		break;
	}
	Gain = value;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::GetCfg(PBRD_AdcCfg pCfg)
{
	CAdcSrv::GetCfg(pCfg);
	PADC214X200MSRV_CFG pSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	pCfg->ChanType = pSrvCfg->IsRF;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::ClrBitsOverflow(CModule* pModule, ULONG flags)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = 0x202;//ADCnr_FLAGCLR;
	param.val = flags << 14;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x200mSrv::IsBitsOverflow(CModule* pModule, ULONG& OverBits)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	OverBits = param.val >> 14;
	return BRDerr_OK;
}

// ***************** End of file Adc214x200mSrv.cpp ***********************
