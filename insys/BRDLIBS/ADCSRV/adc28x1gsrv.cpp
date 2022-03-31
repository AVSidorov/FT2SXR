/*
 ***************** File Adc28x1gSrv.cpp ************
 *
 * BRD Driver for ADС service on ADM28x1G
 *
 * (C) InSys by Dorokhin A. Jan 2007
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "adc28x1gsrv.h"

#define	CURRFILE "ADC28X1GSRV"

const double BRD_ADC_MAXGAINTUN	= 1.5; // max gain tuning = 1.5 dB

//***************************************************************************************
CAdc28x1gSrv::CAdc28x1gSrv(int idx, int srv_num, CModule* pModule, PADC28X1GSRV_CFG pCfg) :
	CAdcSrv(idx, _T("ADC28X1G"), srv_num, pModule, pCfg, sizeof(ADC28X1GSRV_CFG))
{
}

//***************************************************************************************
int CAdc28x1gSrv::CtrlRelease(
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
void CAdc28x1gSrv::GetAdcTetrNum(CModule* pModule)
{
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC28X1G_TETR_ID);
}

//***************************************************************************************
void CAdc28x1gSrv::FreeInfoForDialog(PVOID pInfo)
{
	PADC28X1GSRV_INFO pSrvInfo = (PADC28X1GSRV_INFO)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc28x1gSrv::GetInfoForDialog(CModule* pDev)
{
	PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	PADC28X1GSRV_INFO pSrvInfo = new ADC28X1GSRV_INFO;
	pSrvInfo->Size = sizeof(ADC28X1GSRV_INFO);
//	UCHAR code = pSrvCfg->AdcCfg.Encoding;
	pSrvInfo->pAdcInfo = (PADCSRV_INFO)CAdcSrv::GetInfoForDialog(pDev);
	if(pSrvInfo->pAdcInfo->SyncMode == 3)
		pSrvInfo->pAdcInfo->SyncMode = 1;
//	pSrvInfo->pAdcInfo->Encoding = code;
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

	PADC_CTRL0 pReg0 = (PADC_CTRL0)&pSrvCfg->AdcReg[0];
	pSrvInfo->UseClbr = pReg0->ByBits.Calibr;
	PADC_CTRL3 pReg3 = (PADC_CTRL3)&pSrvCfg->AdcReg[3];
	pSrvInfo->GainComp = pReg3->ByBits.GainComp;
	PADC_CTRL7 pReg7 = (PADC_CTRL7)&pSrvCfg->AdcReg[7];
	pSrvInfo->Fisda = pReg7->ByBits.FiSDA;

	//pSrvInfo->UseClbr = (pSrvCfg->AdcReg[0] >> 10) & 0x1;
	//pSrvInfo->GainComp = (UCHAR)pSrvCfg->AdcReg[3];
	//pSrvInfo->Fisda = (pSrvCfg->AdcReg[7] >> 6) & 0x1f;

	return pSrvInfo;
}

//***************************************************************************************
int CAdc28x1gSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
//	TCHAR msg[256];
	CModule* pModule = (CModule*)pDev;
	PADC28X1GSRV_INFO pInfo = (PADC28X1GSRV_INFO)args;
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
	BRD_EnVal acq_data;
	acq_data.enable	= pInfo->pAdcInfo->Cnt1Enable;
	acq_data.value	= pInfo->pAdcInfo->DaqCnt;
	SetCnt(pModule, 1, &acq_data);
	BRD_EnVal skip_data;
	skip_data.enable = pInfo->pAdcInfo->Cnt2Enable;
	skip_data.value	 = pInfo->pAdcInfo->SkipCnt;
	SetCnt(pModule, 2, &skip_data);

	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
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
//		SetInpResist(pModule, pInfo->pAdcInfo->Resist[i], i);
		SetDcCoupling(pModule, pInfo->pAdcInfo->DcCoupling[i], i);
//		SetClkPhase(pModule, pInfo->PhaseTuning[i], i);
		SetGainTuning(pModule, pInfo->GainTuning[i], i);
	}
	BRD_AdcStartMode start_mode;
//	GetStartMode(pModule, &start_mode);
	start_mode.src					 = pInfo->StartSrc;
	start_mode.inv					 = pInfo->InvStart;
	start_mode.level				 = pInfo->StartLevel;
	start_mode.stndStart.startSrc	 = pInfo->pAdcInfo->StartSrc;
	start_mode.stndStart.startInv	 = pInfo->pAdcInfo->StartInv;
	start_mode.stndStart.trigOn		 = pInfo->pAdcInfo->StartStopMode;
	start_mode.stndStart.trigStopSrc = pInfo->pAdcInfo->StopSrc;
	start_mode.stndStart.stopInv	 = pInfo->pAdcInfo->StopInv;
	start_mode.stndStart.reStartMode = pInfo->pAdcInfo->ReStart;
	SetStartMode(pModule, &start_mode);
	pInfo->StartLevel = start_mode.level;

	ADC_CTRL0 reg0;
	reg0.AsWhole = pAdcSrvCfg->AdcReg[0];
	reg0.ByBits.Calibr = pInfo->UseClbr;
	reg0.ByBits.WaitClbr = 0;
	if(reg0.ByBits.Calibr == 3)
	{
		if(pInfo->pAdcInfo->SamplingRate > 500000000.)
			reg0.ByBits.WaitClbr = 3;
		else
			if(pInfo->pAdcInfo->SamplingRate > 250000000.)
				reg0.ByBits.WaitClbr = 2;
			else
				if(pInfo->pAdcInfo->SamplingRate > 125000000.)
					reg0.ByBits.WaitClbr = 1;
	}
	SetAdcReg(pModule, 0, reg0.AsWhole);
	if(reg0.ByBits.Calibr == 3)
	{
		Sleep(10);
		reg0.ByBits.Calibr = 1;
		SetAdcReg(pModule, 0, reg0.AsWhole);
		pInfo->UseClbr = reg0.ByBits.Calibr;
	}
	ADC_CTRL3 reg3;
	reg3.AsWhole = pAdcSrvCfg->AdcReg[3];
	reg3.ByBits.GainComp = pInfo->GainComp;
	SetAdcReg(pModule, 3, reg3.AsWhole);

	ADC_CTRL7 reg7;
	reg7.AsWhole = pAdcSrvCfg->AdcReg[7];
	reg7.ByBits.FiSDA = pInfo->Fisda;
	SetAdcReg(pModule, 7, reg7.AsWhole);

	//int regval = pAdcSrvCfg->AdcReg[0];
	//regval |= pInfo->UseClbr << 10;
	//SetAdcReg(pModule, 0, regval);
	//int regval = pAdcSrvCfg->AdcReg[3];
	//regval &= 0xFF80;
	//regval |= pInfo->GainComp;
	//SetAdcReg(pModule, 3, regval);
	//regval = pAdcSrvCfg->AdcReg[7];
	//regval |= pInfo->Fisda << 6;
	//SetAdcReg(pModule, 7, regval);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x1gSrv::SetProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	TCHAR ParamName[128];
	TCHAR* endptr;
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);

	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	GetPrivateProfileString(SectionName, "PllFracMode", "0", Buffer, sizeof(Buffer), iniFilePath);
	pAdcSrvCfg->FracMode = atoi(Buffer);

	GetPrivateProfileString(SectionName, "PllOutCurrent", "8", Buffer, sizeof(Buffer), iniFilePath);
	pAdcSrvCfg->OutCurrent = atoi(Buffer);

	GetPrivateProfileString(SectionName, "PllFold", "2", Buffer, sizeof(Buffer), iniFilePath);
	int fold = atoi(Buffer);
	SetFold(pDev, fold);

	//GetPrivateProfileString(SectionName, "ExternalPllReference", "50000000.0", Buffer, sizeof(Buffer), iniFilePath);
	//BRD_Adc10x2gPll pllMode;
	//pllMode.ExtRef = atof(Buffer);
	//SetPllMode(pDev, &pllMode);

	GetPrivateProfileString(SectionName, "ClockSource", "129", Buffer, sizeof(Buffer), iniFilePath);
//	ULONG clkSrc = atoi(Buffer);
	ULONG clkSrc = strtol(Buffer, &endptr, 0);
	SetClkSource(pDev, clkSrc);
	GetPrivateProfileString(SectionName, "ExternalClockValue", "120000000.0", Buffer, sizeof(Buffer), iniFilePath);
	double clkValue = atof(Buffer);
	pAdcSrvCfg->SubExtClk = ROUND(clkValue);
	SetClkValue(pDev, clkSrc, clkValue);
	GetPrivateProfileString(SectionName, "SamplingRate", "100000000.0", Buffer, sizeof(Buffer), iniFilePath);
	double rate = atof(Buffer);
	SetRate(pDev, rate, clkSrc, clkValue);

	GetPrivateProfileString(SectionName, "DoubleClockingMode", "0", Buffer, sizeof(Buffer), iniFilePath);
	ULONG dbl_mode = atoi(Buffer);
	SetDblClk(pDev, dbl_mode);
//	SetSrcClk(pDev, dbl_mode);

	GetPrivateProfileString(SectionName, "ChannelOneSource", "0", Buffer, sizeof(Buffer), iniFilePath);
	ULONG inp0_ch1 = atoi(Buffer);;
	SetInpSrc(pDev, &inp0_ch1);

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
	GetPrivateProfileString(SectionName, "ReStart", "0", Buffer, sizeof(Buffer), iniFilePath);
	start.stndStart.reStartMode = atoi(Buffer);
	SetStartMode(pDev, &start);

	GetPrivateProfileString(SectionName, "UseCalibration", "0", Buffer, sizeof(Buffer), iniFilePath);
	PADC_CTRL0 pReg0 = (PADC_CTRL0)&pAdcSrvCfg->AdcReg[0];
	pReg0->ByBits.Calibr = strtol(Buffer, &endptr, 0);
	SetAdcReg(pDev, 0, pReg0->AsWhole);

	GetPrivateProfileString(SectionName, "GainCompensation", "0", Buffer, sizeof(Buffer), iniFilePath);
	int regval = strtol(Buffer, &endptr, 0);
	SetAdcReg(pDev, 3, regval);

	GetPrivateProfileString(SectionName, "ISA0", "3", Buffer, sizeof(Buffer), iniFilePath);
	int isa0 = strtol(Buffer, &endptr, 0);
	GetPrivateProfileString(SectionName, "ISA1", "3", Buffer, sizeof(Buffer), iniFilePath);
	int isa1 = strtol(Buffer, &endptr, 0);
	regval = isa0 | (isa1 << 3) | 0x8400;
	SetAdcReg(pDev, 4, regval);

	GetPrivateProfileString(SectionName, "BIT", "0", Buffer, sizeof(Buffer), iniFilePath);
//	int regval = atoi(Buffer);
	regval = strtol(Buffer, &endptr, 0);
	SetAdcReg(pDev, 6, regval);

	GetPrivateProfileString(SectionName, "DRDA0", "4", Buffer, sizeof(Buffer), iniFilePath);
//	int drda0 = atoi(Buffer);
	int drda0 = strtol(Buffer, &endptr, 0);
	GetPrivateProfileString(SectionName, "DRDA1", "4", Buffer, sizeof(Buffer), iniFilePath);
//	int drda1 = atoi(Buffer);
	int drda1 = strtol(Buffer, &endptr, 0);
//	regval = drda0 | (drda1 << 3);

	GetPrivateProfileString(SectionName, "FiSDA", "0", Buffer, sizeof(Buffer), iniFilePath);
	int fisda = strtol(Buffer, &endptr, 0);
	regval = drda0 | (drda1 << 3) | (fisda << 6);

	SetAdcReg(pDev, 7, regval);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x1gSrv::SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	TCHAR ParamName[128];
	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);
//	PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
//	sprintf_s(Buffer, "%u", pSrvCfg->AdcCfg.Encoding);
//	WritePrivateProfileString(SectionName, "CodeType", Buffer, iniFilePath);

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
	if(clkSrc == BRDclks_ADC_EXTCLK)
	{
		sprintf_s(Buffer, "%.2f", clkValue);
		WritePrivateProfileString(SectionName, "ExternalClockValue", Buffer, iniFilePath);
	}
	double rate;
	GetRate(pDev, rate, clkValue);
	sprintf_s(Buffer, "%.2f", rate);
	WritePrivateProfileString(SectionName, "SamplingRate", Buffer, iniFilePath);

	//PADC10X2GSRV_CFG pAdcSrvCfg = (PADC10X2GSRV_CFG)m_pConfig;
	//sprintf_s(Buffer, "%u", pAdcSrvCfg->ExtPllRef);
	//WritePrivateProfileString(SectionName, "ExternalPllReference", Buffer, iniFilePath);

	ULONG dbl_mode;
	GetDblClk(pDev, dbl_mode);
//	GetSrcClk(pDev, dbl_mode);
	sprintf_s(Buffer, "%u", dbl_mode);
	WritePrivateProfileString(SectionName, "DoubleClockingMode", Buffer, iniFilePath);

	ULONG inp0_ch1;
	GetInpSrc(pDev, &inp0_ch1);
	sprintf_s(Buffer, "%u", inp0_ch1);
	WritePrivateProfileString(SectionName, "ChannelOneSource", Buffer, iniFilePath);

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
	sprintf_s(Buffer, "%u", start.stndStart.reStartMode);
	WritePrivateProfileString(SectionName, "ReStart", Buffer, iniFilePath);

	PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;

	PADC_CTRL0 pReg0 = (PADC_CTRL0)&pSrvCfg->AdcReg[0];
	sprintf_s(Buffer, "%u", pReg0->ByBits.Calibr);
	WritePrivateProfileString(SectionName, "UseCalibration", Buffer, iniFilePath);

	PADC_CTRL3 pReg3 = (PADC_CTRL3)&pSrvCfg->AdcReg[3];
	sprintf_s(Buffer, "%u", pReg3->ByBits.GainComp);
	WritePrivateProfileString(SectionName, "GainCompensation", Buffer, iniFilePath);

	PADC_CTRL7 pReg7 = (PADC_CTRL7)&pSrvCfg->AdcReg[7];
	sprintf_s(Buffer, "%u", pReg7->ByBits.FiSDA);
//	int fisda = (pAdcSrvCfg->AdcReg[7] >> 6) & 0x1f;
//	sprintf_s(Buffer, "%u", fisda);
	WritePrivateProfileString(SectionName, "FiSDA", Buffer, iniFilePath);

	// the function flushes the cache
	WritePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x1gSrv::SetClkSource(CModule* pModule, ULONG ClkSrc)
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
int CAdc28x1gSrv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
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
int CAdc28x1gSrv::SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:		// disabled clock
		ClkValue = 0.0;
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
int CAdc28x1gSrv::GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	double Clk;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:		// disabled clock
		Clk = 0.0;
		break;
	case BRDclks_ADC_REFPLL:			// SubModule Clock (internal reference)
//	case BRDclks_ADC_INTREFPLL:			// SubModule Clock (internal reference)
//	case BRDclks_ADC_EXTREFPLL:			// SubModule Clock (external reference - EXT REF IN)
		Clk = pAdcSrvCfg->PllFreq;
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
int M28X1G_CLK_DIVIDER[BRD_CLKDIVCNT] = {1, 2, 4, 8, 16, 32};

//****************************************************************************************
int SetClkDivOpt(double RateHz, double curClk, double minRate)
{
	if(!curClk)
		return 1;
	int MaxDivider = (int)floor(curClk / minRate);
	int i = 0;
	for(i = BRD_CLKDIVCNT-1; i >= 0; i--)
		if(MaxDivider >= M28X1G_CLK_DIVIDER[i])
			break;
	int MaxInd = (i < 0) ? 0 : i;
	MaxDivider = M28X1G_CLK_DIVIDER[MaxInd];
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
//				return MaxInd;
				return M28X1G_CLK_DIVIDER[MaxInd];
			}
		}
		else 
		{
			for(int i = 0; i < MaxInd; i++) 
			{
				double RateLeft = curClk / M28X1G_CLK_DIVIDER[i];
				double RateRight = curClk / M28X1G_CLK_DIVIDER[i+1];
				if(RateHz <= RateLeft && RateHz >= RateRight)
				{
					if(RateLeft - RateHz <= RateHz - RateRight)
					{
						if (DeltaFreq > RateLeft - RateHz)
						{
							DeltaFreq = RateLeft - RateHz;
							return M28X1G_CLK_DIVIDER[i];
						}
					}
					else
					{
						if (DeltaFreq > RateHz - RateRight)
						{
							DeltaFreq = RateHz - RateRight;
							return M28X1G_CLK_DIVIDER[i+1];
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
int CAdc28x1gSrv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
	ULONG AdcRateDivider;
//	if(ClkSrc != BRDclks_ADC_EXTCLK)
//	{
		DeltaFreq = 1.e10;
		PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
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
int CAdc28x1gSrv::GetRate(CModule* pModule, double& Rate, double ClkValue)
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
//int CAdc28x1gSrv::GetGainMask(CModule* pModule, int chan)
//{
//	DEVS_CMD_Reg param;
//	param.idxMain = m_index;
//	param.tetr = m_AdcTetrNum;
//	param.reg = ADM2IFnr_GAIN;
//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	PADM2IF_GAIN pGain = (PADM2IF_GAIN)&param.val;
//	int gain_mask;
//	switch(chan)
//	{
//	case 0:
//		gain_mask = pGain->ByBits.Chan0;
//		break;
//	case 1:
//		gain_mask = pGain->ByBits.Chan1;
//		break;
//	case 2:
//		gain_mask = pGain->ByBits.Chan2;
//		break;
//	case 3:
//		gain_mask = pGain->ByBits.Chan3;
//		break;
//	}
//	return gain_mask;
//}

//***************************************************************************************
int CAdc28x1gSrv::SetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	double inp_range;
	Status = GetInpRange(pModule, inp_range, Chan); 
	if(Status != BRDerr_OK)
		return Status;
	double bias_range = inp_range / 4; // 25% from full range

//	int max_dac_value = 255;
//	int min_dac_value = 127;
//	double half_dac_value = 128.;
	int max_pos_dac_value = 255;
	int max_neg_dac_value = 127;
	double max_bias_value = 127.;

	USHORT dac_data;
	if(fabs(Bias) > bias_range)
	{
		if(Bias > 0)
		{
			dac_data = max_pos_dac_value;
			Bias = bias_range;
		}
		else
		{
			dac_data = max_neg_dac_value;
			Bias = -bias_range;
		}
	}
	else
	{
		if(Bias)
		{
			dac_data = (USHORT)floor(fabs(Bias) / bias_range * max_bias_value + 0.5);
			double bias_val = (bias_range / max_bias_value) * dac_data;
			if(Bias > 0)
			{
				dac_data += 128;
				Bias = bias_val;
			}
			else
				Bias = -bias_val;
		}
		else
			dac_data = 0;
	}

	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	pAdcSrvCfg->Bias[Chan] = Bias;

	ADC_CTRL2 reg2;
	reg2.AsWhole = pAdcSrvCfg->AdcReg[2];
	//int regval = pAdcSrvCfg->AdcReg[2];

	if(Chan)
	{
		reg2.ByBits.Offset1 = dac_data;
		//dac_data <<= 8;
		//regval &= 0xFF;
	}
	else
	{
		reg2.ByBits.Offset0 = dac_data;
		//regval &= 0xFF00;
	}
	//regval |= dac_data;

	SetAdcReg(pModule, 2, reg2.AsWhole);
	//SetAdcReg(pModule, 2, regval);

	return Status;
}

//***************************************************************************************
int CAdc28x1gSrv::GetBias(CModule* pModule, double& Bias, ULONG Chan)
{
	int Status = BRDerr_OK;
	PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	Bias = pSrvCfg->Bias[Chan];
	return Status;
}

//***************************************************************************************
int CAdc28x1gSrv::SetStartMode(CModule* pModule, PVOID pStartStopMode)
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
int CAdc28x1gSrv::GetStartMode(CModule* pModule, PVOID pStartStopMode)
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
int CAdc28x1gSrv::SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr)
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

	double inp_range = ADC28X1G_STARTTHREXT;

	if(BRDsts_ADC_CHAN0 == source)
		GetInpRange(pModule, inp_range, 0); 
	if(BRDsts_ADC_CHAN1 == source)
		GetInpRange(pModule, inp_range, 1); 

	double value = *pCmpThr;

	int max_code = 255;
	int min_code = 0;

	PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
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

	pSrvCfg->StCmpThr = *pCmpThr;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC28X1G_STARTCMP - 1] = (UCHAR)dac_data;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_ADC28X1G_STARTCMP;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//int CAdc28x1gSrv::SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr)
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
//	PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
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
////	PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
//	pSrvCfg->StCmpThr = *pCmpThr;
//	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC212x200M_STARTCMP - 1] = (UCHAR)dac_data;
//	param.reg = MAINnr_THDAC;
//	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
//	pThDac->ByBits.Data = dac_data;
//	pThDac->ByBits.Num = BRDtdn_ADC212x200M_STARTCMP;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	return BRDerr_OK;
//}

//int CAdc28x1gSrv::SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr)
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
//	PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
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
int CAdc28x1gSrv::GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	*pSource = pCtrl->ByBits.StartSrc;
	*pInv = pCtrl->ByBits.StartInv;
	PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	*pCmpThr = pSrvCfg->StCmpThr;
	return BRDerr_OK;
}

//***************************************************************************************
// заменяет функцию установки источника тактовой частоты на функцию удвоения тактовой частоты
int CAdc28x1gSrv::SetDblClk(CModule* pModule, ULONG& mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.DblClk = mode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;

	ADC_CTRL0 reg0;
	reg0.AsWhole = pAdcSrvCfg->AdcReg[0];
	reg0.ByBits.ClkMode = mode ? 0 : 2;
	SetAdcReg(pModule, 0, reg0.AsWhole);

	//int regval = pAdcSrvCfg->AdcReg[0];
	//if(mode)
	//	regval &= 0xff3f; // D7 = D6 = 0
	//else
	//	regval |= 0x0080; // D7 = 1
	//SetAdcReg(pModule, 0, regval);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x1gSrv::GetDblClk(CModule* pModule, ULONG& mode)
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
int CAdc28x1gSrv::SetClkLocation(CModule* pModule, ULONG& mode)
{
	mode = 1;
	CAdcSrv::SetClkLocation(pModule, mode);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x1gSrv::GetClkLocation(CModule* pModule, ULONG& mode)
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
int CAdc28x1gSrv::SetInpSrc(CModule* pModule, PVOID pCotrol)
{
	ULONG mode = *(PULONG)pCotrol;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pCtrl = (PADC_CTRL)&param.val;
	pCtrl->ByBits.Chan1Src = mode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;

	ADC_CTRL0 reg0;
	reg0.AsWhole = pAdcSrvCfg->AdcReg[0];
	reg0.ByBits.InputMode = mode ? 2 : 3;
	SetAdcReg(pModule, 0, reg0.AsWhole);

	//int regval = pAdcSrvCfg->AdcReg[0];
	//if(mode)
	//	regval &= 0xffef; // D4 = 0
	//else
	//	regval |= 0x0010; // D4 = 1
	//SetAdcReg(pModule, 0, regval);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x1gSrv::GetInpSrc(CModule* pModule, PVOID pCotrol)
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
int CAdc28x1gSrv::SetGain(CModule* pModule, double& Gain, ULONG Chan)
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
int CAdc28x1gSrv::GetGain(CModule* pModule, double& Gain, ULONG Chan)
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
int CAdc28x1gSrv::SetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
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
int CAdc28x1gSrv::GetInpRange(CModule* pModule, double& InpRange, ULONG Chan)
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
int CAdc28x1gSrv::SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	double max_thr = BRD_ADC_MAXGAINTUN; // %
//	double max_thr = pSrvCfg->RefPVS / 1000.;

	//if(WaitCmdReady(pModule, m_index, m_MainTetrNum, 1000))
	//	return BRDerr_WAIT_TIMEOUT;

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

	ADC_CTRL1 reg1;
	reg1.AsWhole = pSrvCfg->AdcReg[1];
	//int regval = pSrvCfg->AdcReg[1];


	if(Chan)
	{
		reg1.ByBits.Gain1 = dac_data;
		//dac_data <<= 8;
		//regval &= 0xFF;
	}
	else
	{
		reg1.ByBits.Gain0 = dac_data;
		//regval &= 0xFF00;
	}
	//regval |= dac_data;

	SetAdcReg(pModule, 1, reg1.AsWhole);
	//SetAdcReg(pModule, 1, regval);

	//pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC212X200M_GAINTUN0 - 1] = (UCHAR)dac_data;
	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;
	//param.tetr = m_MainTetrNum;
	//param.reg = MAINnr_THDAC;
	//PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	//pThDac->ByBits.Data = dac_data;
	//pThDac->ByBits.Num = BRDtdn_ADC212X200M_GAINTUN0;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x1gSrv::GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC28X1GSRV_CFG pSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
//	pValChan->value = pSrvCfg->GainTun;
//	pValChan->chan = 0;
//	pValChan->value = pSrvCfg->ClkPhase[pValChan->chan];
	GainTuning = pSrvCfg->GainTun[Chan];
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x1gSrv::SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	return Status;
}

//***************************************************************************************
int CAdc28x1gSrv::GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	InpRes = 0;
	return Status;
}

//***************************************************************************************
int CAdc28x1gSrv::SetCode(CModule* pModule, ULONG type)
{
	CAdcSrv::SetCode(pModule, type);

	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;

	ADC_CTRL0 reg0;
	reg0.AsWhole = pAdcSrvCfg->AdcReg[0];

	//int regval = pAdcSrvCfg->AdcReg[0];
	switch(type)
	{
	case BRDcode_TWOSCOMPLEMENT:
	case BRDcode_STRAIGHTBINARY:
		reg0.ByBits.Encode = 1;
		//regval |= 0x0004; // D2 = 1
		break;
	case BRDcode_GRAY:
		reg0.ByBits.Encode = 0;
		//regval &= 0xfffb; // D2 = 0
		break;
	default:
		reg0.ByBits.Encode = 1;
		//regval |= 0x0004;
		break;
	}
	SetAdcReg(pModule, 0, reg0.AsWhole);
//	SetAdcReg(pModule, 0, regval);
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
//
////***************************************************************************************
//int CAdc28x1gSrv::GetCode(CModule* pModule, ULONG& type)
//{
//	//DEVS_CMD_Reg param;
//	//param.idxMain = m_index;
//	//param.tetr = m_AdcTetrNum;
//	//param.reg = ADM2IFnr_FORMAT;
//	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	//PADM2IF_FORMAT pFormat = (PADM2IF_FORMAT)&param.val;
//	//type = pFormat->ByBits.Code;
//	type = BRDcode_TWOSCOMPLEMENT;
//	return BRDerr_OK;
//}
//
//***************************************************************************************
int CAdc28x1gSrv::SetMaster(CModule* pModule, ULONG mode)
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
int CAdc28x1gSrv::ExtraInit(CModule* pModule)
{
	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	for(int i = 0; i < 8; i++)
		pAdcSrvCfg->AdcReg[i] = 0;
	pAdcSrvCfg->AdcReg[0] = 0x00b4;
	pAdcSrvCfg->AdcReg[4] = 0x8400;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADC_CTRL pAdcCtrl = (PADC_CTRL)&param.val;
	pAdcCtrl->ByBits.AdcRegEn = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	InitPLL(pModule);
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
int CAdc28x1gSrv::writePllData(CModule* pModule, USHORT HiWord, USHORT LoWord)
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
int CAdc28x1gSrv::InitPLL(CModule* pModule)
{
	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	pAdcSrvCfg->Pll_IF_R.AsWhole = 0;
	pAdcSrvCfg->Pll_RF_R.AsWhole = 0;
	pAdcSrvCfg->Pll_RF_N.AsWhole = 0;

	pAdcSrvCfg->Pll_IF_R.ByBits.Addr = 0;
	pAdcSrvCfg->Pll_RF_R.ByBits.Addr = 2;
	pAdcSrvCfg->Pll_RF_N.ByBits.Addr = 3;

	pAdcSrvCfg->Pll_RF_N.ByBits.RF_CTL_WORD = 4; // reset

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADCnr_HIPLL;
	param.val = pAdcSrvCfg->Pll_RF_N.ByWord.HiWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADCnr_LOWPLL;
	param.val = pAdcSrvCfg->Pll_RF_N.ByWord.LoWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	updatePllData(pAdcSrvCfg->Pll_RF_N.ByWord.LoWord, pAdcSrvCfg->Pll_RF_N.ByWord.HiWord);

//	pAdcSrvCfg->Pll_RF_N.ByBits.RF_CTL_WORD = 1; // normal - 0 = Prescaler Modulus select = 16 (1 GHz), 1 = 32 (2 GHz)
	pAdcSrvCfg->Pll_RF_N.ByBits.RF_CTL_WORD = 0; // normal - 0 = Prescaler Modulus select = 16 (1 GHz), 1 = 32 (2 GHz)

	param.reg = ADCnr_HIPLL;
	param.val = pAdcSrvCfg->Pll_RF_N.ByWord.HiWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADCnr_LOWPLL;
	param.val = pAdcSrvCfg->Pll_RF_N.ByWord.LoWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	updatePllData(pAdcSrvCfg->Pll_RF_N.ByWord.LoWord, pAdcSrvCfg->Pll_RF_N.ByWord.HiWord);

	pAdcSrvCfg->Pll_IF_R.ByBits.FRAC_16 = 1; // 0 - 1/16 resolution, 1 - 1/32 resolution
	pAdcSrvCfg->Pll_IF_R.ByBits.FoLD = 2; // RF Digital Lock Detect

	param.reg = ADCnr_HIPLL;
	param.val = pAdcSrvCfg->Pll_IF_R.ByWord.HiWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADCnr_LOWPLL;
	param.val = pAdcSrvCfg->Pll_IF_R.ByWord.LoWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	updatePllData(pAdcSrvCfg->Pll_IF_R.ByWord.LoWord, pAdcSrvCfg->Pll_IF_R.ByWord.HiWord);

//	pAdcSrvCfg->Pll_RF_R.ByBits.DLL_MODE = 1; // fast
	pAdcSrvCfg->Pll_RF_R.ByBits.DLL_MODE = 0; // normal
	int RfPdPol = PLL_pdpolPositive;// RF VCO - positive
	int OutCurrent = pAdcSrvCfg->OutCurrent; // 8;
	pAdcSrvCfg->Pll_RF_R.ByBits.RF_CP_WORD = (OutCurrent << 1) || RfPdPol;
//	pAdcSrvCfg->Pll_RF_R.ByBits.RF_CP_WORD = 9; // RF VCO - positive, RF Charge Pump output current = 500 mkA
//	pAdcSrvCfg->Pll_RF_R.ByBits.RF_CP_WORD = 8; // RF VCO - negative, RF Charge Pump output current = 500 mkA

	param.reg = ADCnr_HIPLL;
	param.val = pAdcSrvCfg->Pll_RF_R.ByWord.HiWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADCnr_LOWPLL;
	param.val = pAdcSrvCfg->Pll_RF_R.ByWord.LoWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	updatePllData(pAdcSrvCfg->Pll_RF_R.ByWord.LoWord, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord);

	return BRDerr_OK;
}

//****************************************************************************************
int CAdc28x1gSrv::OffPLL(CModule* pModule)
{
//	pAdcSrvCfg->Pll_RF_R pAdcSrvCfg->Pll_RF_R;
//	readPllData(pModule, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord, pAdcSrvCfg->Pll_RF_R.ByWord.LoWord);
	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	pAdcSrvCfg->Pll_RF_R.ByBits.V2_EN = 0;
	writePllData(pModule, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord, pAdcSrvCfg->Pll_RF_R.ByWord.LoWord);

	return BRDerr_OK;
}

//****************************************************************************************
int CAdc28x1gSrv::OnPll(CModule* pModule, double& ClkValue, double PllRefValue)
{
	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
//	pAdcSrvCfg->Pll_RF_R pAdcSrvCfg->Pll_RF_R;
//	pAdcSrvCfg->Pll_RF_R.ByBits.Addr = 2;
//	readPllData(pModule, pAdcSrvCfg->Pll_RF_R.ByWord.HiWord, pAdcSrvCfg->Pll_RF_R.ByWord.LoWord);
//	pAdcSrvCfg->Pll_RF_N pAdcSrvCfg->Pll_RF_N;
//	pAdcSrvCfg->Pll_RF_N.ByBits.Addr = 3;
//	readPllData(pModule, pAdcSrvCfg->Pll_RF_N.ByWord.HiWord, pAdcSrvCfg->Pll_RF_N.ByWord.LoWord);

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
//	int delta_min = 100; // 1G
//	double delta_min = 10000000; // 2G
	double delta_min = (f_osc - PLL_F_CMP_MIN) / f_osc;
	for(int R_tmp = R_min; R_tmp <= R_max; R_tmp++)
//	for(int R_tmp = R_max; R_tmp >= R_min; R_tmp--)
	{
		B_tmp = int(f_vco / P * R_tmp / f_osc);
//		double f_new = P * B_tmp * (f_osc / R_tmp);
		double f_new = P * B_tmp * f_osc / R_tmp;
//		int delta = int((fabs(pAdcSrvCfg->PllFreq - f_new) / pAdcSrvCfg->PllFreq) * 100); // 1G
		double delta = (fabs(pAdcSrvCfg->PllFreq - f_new) / pAdcSrvCfg->PllFreq); // 2G
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
int CAdc28x1gSrv::SetFold(CModule* pModule, int fold)
{
	if(fold != PLL_foldLockRF && fold != PLL_foldCntRF_R && fold != PLL_foldCntRF_N)
		return BRDerr_BAD_PARAMETER;
	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	pAdcSrvCfg->Pll_IF_R.ByBits.FoLD = fold;
	writePllData(pModule, pAdcSrvCfg->Pll_IF_R.ByWord.HiWord, pAdcSrvCfg->Pll_IF_R.ByWord.LoWord);
//	updatePllData(m_IF_R.ByWord.LoWord, m_IF_R.ByWord.HiWord);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc28x1gSrv::SetAdcReg(CModule* pModule, int regNum, int regVal)
{
//	int Status = BRDerr_CMD_UNSUPPORTED;
	int Status = BRDerr_OK;

	PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
	pAdcSrvCfg->AdcReg[regNum] = regVal;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADC28X1Gnr_REGADDR;
	param.val = regNum;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADC28X1Gnr_REGDATA;
	param.val = regVal;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return Status;
}

//***************************************************************************************
int CAdc28x1gSrv::SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec)
{
//	int Status = BRDerr_CMD_UNSUPPORTED;
	int Status = BRDerr_OK;
	CAdcSrv::SetSpecific(pModule, pSpec);
	switch(pSpec->command)
	{
	case ADC28X1Gcmd_SETINTREG:
		{
			PBRD_AdcIntReg pAdcReg = (PBRD_AdcIntReg)pSpec->arg;
			SetAdcReg(pModule, pAdcReg->addr, pAdcReg->data);
		}
		break;
	case ADC28X1Gcmd_GETINTREG:
		{
			PBRD_AdcIntReg pAdcReg = (PBRD_AdcIntReg)pSpec->arg;
			PADC28X1GSRV_CFG pAdcSrvCfg = (PADC28X1GSRV_CFG)m_pConfig;
			pAdcReg->data = pAdcSrvCfg->AdcReg[pAdcReg->addr];
		}
		break;
	}
	return Status;
}

// ***************** End of file Adc28x1gSrv.cpp ***********************
