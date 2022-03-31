/*
 ***************** File Adc214x400mSrv.cpp ************
 *
 * BRD Driver for ADС service on ADM214x400M(WB) and ADM212x500M(WB)
 *
 * (C) InSys by Ekkore Aug 2008
 *
 ******************************************************
*/

#include "module.h"
#include "adc214x400msrv.h"

#define	CURRFILE "ADC214X400MSRV"

//***************************************************************************************
CAdc214x400mSrv::CAdc214x400mSrv(int idx, int srv_num, CModule* pModule, PADC214X400MSRV_Cfg pCfg) :
	CAdcSrv(idx, _BRDC("ADC214X400M"), srv_num, pModule, pCfg, sizeof(ADC214X400MSRV_Cfg))
{
}

//***************************************************************************************
int CAdc214x400mSrv::CtrlRelease( void *pDev, void *pServData, ULONG cmd, void *args )
{
	CModule* pModule = (CModule*)pDev;

	CAdcSrv::SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);
//	return BRDerr_OK;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CAdc214x400mSrv::GetAdcTetrNum(CModule* pModule)
{
	//
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	//

	//
	// Поиск номера терады. Если тетрада не найдена, то -1.
	//
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC214X400M_TETR_ID);
}

//***************************************************************************************
void CAdc214x400mSrv::FreeInfoForDialog(PVOID pInfo)
{
	ADC214X400MSRV_Info *pSrvInfo = (ADC214X400MSRV_Info*)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc214x400mSrv::GetInfoForDialog(CModule* pDev)
{
	ADC214X400MSRV_Cfg  *pSrvCfg  = (ADC214X400MSRV_Cfg*)m_pConfig;
	ADC214X400MSRV_Info *pSrvInfo = new ADC214X400MSRV_Info;
	int					ii;

	pSrvInfo->Size = sizeof(ADC214X400MSRV_Info);

	UCHAR code = pSrvCfg->AdcCfg.Encoding;
	
	pSrvInfo->pAdcInfo = (ADCSRV_INFO*)CAdcSrv::GetInfoForDialog(pDev);
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
	for( ii = 0; ii < MAX_CHAN; ii++ )
	{
		if( (pSrvCfg->nSubType & 0xF)  == 0 )
		{
			//Standard Unit
			GetInpRange(pDev, pSrvInfo->pAdcInfo->Range[ii], ii );
		}
		else
		{
			//WB HF or WB LF
			GetGain( pDev, pSrvInfo->aGainDb[ii], ii );
			pSrvInfo->pAdcInfo->Range[ii] = pSrvCfg->AdcCfg.InpRange;
		}
		GetBias(pDev, pSrvInfo->pAdcInfo->Bias[ii], ii );
//		GetInpResist(pDev, pSrvInfo->pAdcInfo->Resist[i], i);
//		GetDcCoupling(pDev, pSrvInfo->pAdcInfo->DcCoupling[i], i);
//		GetClkPhase(pDev, pSrvInfo->PhaseTuning[i], i);
		
		GetGainTuning( pDev, pSrvInfo->aGainTun[ii], ii);
	}

	GetStartClkSl(pDev, &(pSrvInfo->nStartClkSl) );

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

	pSrvInfo->Version  = pSrvCfg->nVersion;
	pSrvInfo->nSubType = pSrvCfg->nSubType;
	pSrvInfo->aRefGen[0] = pSrvCfg->aRefGen[0];
	pSrvInfo->aRefGen[1] = pSrvCfg->aRefGen[1];
	pSrvInfo->adIcrRange[0] = pSrvCfg->adIcrRange[0];
	pSrvInfo->adIcrRange[1] = pSrvCfg->adIcrRange[1];
	pSrvInfo->adIcrRange[2] = pSrvCfg->adIcrRange[2];
	pSrvInfo->adIcrRange[3] = pSrvCfg->adIcrRange[3];

	GetInpSrc(pDev, &(pSrvInfo->nInputSource) );

	return pSrvInfo;
}

//***************************************************************************************
int CAdc214x400mSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
//	BRDCHAR msg[256];
	CModule* pModule = (CModule*)pDev;
	PADC214X400MSRV_Info pInfo = (PADC214X400MSRV_Info)args;
	int				ii;

	CAdcSrv::SetChanMask(pModule, pInfo->pAdcInfo->ChanMask);
	ULONG master = pInfo->pAdcInfo->SyncMode;
	SetMaster(pModule, master);

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

	PADC214X400MSRV_Cfg pAdcSrvCfg = (PADC214X400MSRV_Cfg)m_pConfig;
	pAdcSrvCfg->SubExtClk = ROUND(pInfo->ExtClk);
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
//	sprintf(msg, _BRDC("ClockValue = %f, SamplingRate = %f"), pInfo->pAdcInfo->ClockValue, pInfo->pAdcInfo->SamplingRate);
//	MessageBox(NULL, msg, "Debug", MB_OK);
	SetStartClkSl(pModule, &(pInfo->nStartClkSl) );
	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
//	sprintf(msg, _BRDC("ClockValue = %f, SamplingRate = %f"), pInfo->pAdcInfo->ClockValue, pInfo->pAdcInfo->SamplingRate);
//	MessageBox(NULL, msg, "Debug", MB_OK);

	SetClkInv(pModule, pInfo->InvClk);

	for( ii = 0; ii < MAX_CHAN; ii++ )
	{
		// ne WB 
		if( (pAdcSrvCfg->nSubType & 0xF)  == 0 )
			SetInpRange(pModule, pInfo->pAdcInfo->Range[ii], ii);
		SetInpResist(pModule, pInfo->pAdcInfo->Resist[ii], ii);
		SetDcCoupling(pModule, pInfo->pAdcInfo->DcCoupling[ii], ii);
		SetBias(pModule, pInfo->pAdcInfo->Bias[ii], ii);
//		SetClkPhase(pModule, pInfo->PhaseTuning[i], i);
		
		//WB HF or WB LF
		if( (pAdcSrvCfg->nSubType & 0xF)  != 0 )
			SetGain(pModule, pInfo->aGainDb[ii], ii );
		SetGainTuning(pModule, pInfo->aGainTun[ii], ii );
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

	SetInpSrc( pModule, &(pInfo->nInputSource) );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	BRDCHAR Buffer[128];
	BRDCHAR ParamName[128];
	BRDCHAR* endptr;

	//
	// Извлечение параметров работы субмодуля из INI-файла 
	// и инициализация субмодуля в соответствии с этими параметрами
	//


	//
	// Извлечение стандартных параметров
	//
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);

	//
	// Извлечение источника тактовой частоты
	//
	U32			clkSrc;

	IPC_getPrivateProfileString(SectionName, _BRDC("ClockSource"), _BRDC("129"), Buffer, sizeof(Buffer), iniFilePath);
	clkSrc = BRDC_strtol(Buffer, &endptr, 0); //strtol
	SetClkSource(pDev, clkSrc);

	//
	// Извлечение требуемого режима синхронизации MASTER/SLAVE
	//
	U32		nStartClkSl;

	IPC_getPrivateProfileString(SectionName, _BRDC("StartClkSl"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	nStartClkSl = BRDC_atoi(Buffer); //atoi
	SetStartClkSl(pDev, &nStartClkSl);

	//
	// Извлечение частоты внешнего генератора
	//
	ADC214X400MSRV_Cfg *pAdcSrvCfg = (ADC214X400MSRV_Cfg*)m_pConfig;
	double				clkValue;

	IPC_getPrivateProfileString(SectionName, _BRDC("ExternalClockValue"), _BRDC("120000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	clkValue = BRDC_atof(Buffer);//atof
	pAdcSrvCfg->SubExtClk = ROUND(clkValue);
	SetClkValue(pDev, clkSrc, clkValue);

	//
	// Извлечение требуемой частоты дискретизации
	//
	double			rate;
	
	IPC_getPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("100000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	rate = BRDC_atof(Buffer); //atof
	SetRate(pDev, rate, clkSrc, clkValue);

	//
	// Извлечение требуемой инверсии тактовой частоты
	//
	U32				clkInv;
	
	IPC_getPrivateProfileString(SectionName, _BRDC("ClockInv"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	clkInv = BRDC_atoi(Buffer); //atoi
	SetClkInv(pDev, clkInv);

	//
	// Извлечение для каждого канала требуемых:
	// 1. шкалы преобразования
	// 2. смещения нуля
	// 3. Подстройки шкалы преобразования
	//
	double			aInpRange[MAX_CHAN];
	double			aGain[MAX_CHAN];
	double			aBias[MAX_CHAN];
	double			aGainTuning[MAX_CHAN];
	DWORD			ret;

	for(int ii = 0; ii < MAX_CHAN; ii++)
	{
		if( (pAdcSrvCfg->nSubType & 0xF)  == 0 )
		{
			// Standard Subunit
			BRDC_sprintf(ParamName, _BRDC("InputRange%d"), ii);
			ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.5"), Buffer, sizeof(Buffer), iniFilePath);
			aInpRange[ii] = BRDC_atof(Buffer); //atof
			SetInpRange(pDev, aInpRange[ii], ii);
		}
		else
		{
			// WB HF, WB LF
			BRDC_sprintf(ParamName, _BRDC("GainDB%d"), ii); 
			ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.5"), Buffer, sizeof(Buffer), iniFilePath);
			if( ret != 0 )
			{
				aGain[ii] = BRDC_atof(Buffer); //atof
				SetGain(pDev, aGain[ii], ii);
			}
		}


		BRDC_sprintf(ParamName, _BRDC("Bias%d"), ii);
		ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		aBias[ii] = BRDC_atof(Buffer); //atof
		SetBias(pDev, aBias[ii], ii);
		
		BRDC_sprintf(ParamName, _BRDC("GainTuning%d"), ii);
		ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		aGainTuning[ii] = BRDC_atof(Buffer); //atof
		SetGainTuning(pDev, aGainTuning[ii], ii);
	}

	//
	// Извлечение требуемого режима стартовой синхронизации
	//
	BRD_AdcStartMode rStMode;

	IPC_getPrivateProfileString(SectionName, _BRDC("StartSource"), _BRDC("3"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.src = BRDC_atoi(Buffer); //atoi

	IPC_getPrivateProfileString(SectionName, _BRDC("StartInverting"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.inv = BRDC_atoi(Buffer); //atoi

	IPC_getPrivateProfileString(SectionName, _BRDC("StartLevel"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.level = BRDC_atof(Buffer); //atof

	CAdcSrv::GetStartMode(pDev, &rStMode.stndStart);
	SetStartMode(pDev, &rStMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	PADC214X400MSRV_Cfg pSrvCfg = (PADC214X400MSRV_Cfg)m_pConfig;
	BRDCHAR ParamName[128];

	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);

	WriteInifileParam(iniFilePath, SectionName, _BRDC("CodeType"), (ULONG)pSrvCfg->AdcCfg.Encoding, 10, NULL);

	ULONG master;
	GetMaster(pDev, master);
	if(master == 3) master = 1;
	WriteInifileParam(iniFilePath, SectionName, _BRDC("MasterMode"), master, 10, NULL);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ClockSource"), clkSrc, 16, NULL);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
	if(clkSrc == BRDclks_ADC_EXTCLK)
	{
		WriteInifileParam(iniFilePath, SectionName, _BRDC("ExternalClockValue"), clkValue, 2, NULL);
	}
	double rate;
	GetRate(pDev, rate, clkValue);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("SamplingRate"), rate, 2, NULL);

	ULONG clkInv;
	GetClkInv(pDev, clkInv);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ClockInv"), clkInv, 10, NULL);

	double InpRange[MAX_CHAN], Bias[MAX_CHAN], GainTuning[MAX_CHAN], GainDB[MAX_CHAN];
	for(int i = 0; i < MAX_CHAN; i++)
	{
		if( (pSrvCfg->nSubType & 0xF)  == 0 )
		{
			// Standard Subunit
			GetInpRange(pDev, InpRange[i], i);
			BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
			WriteInifileParam(iniFilePath, SectionName, ParamName, InpRange[i], 2, NULL);

			GetBias(pDev, Bias[i], i);
			BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
			WriteInifileParam(iniFilePath, SectionName, ParamName, Bias[i], 4, NULL);
		}
		else
		{
			// WB HF, WB LF
			GetGain(pDev, GainDB[i], i);
			GainDB[i] = floor( GainDB[i] + 0.5 );
			BRDC_sprintf(ParamName, _BRDC("GainDB%d"), i);
			WriteInifileParam(iniFilePath, SectionName, ParamName, GainDB[i], 4, NULL);
		}
		GetGainTuning(pDev, GainTuning[i], i);
		BRDC_sprintf(ParamName, _BRDC("GainTuning%d"), i);
		WriteInifileParam(iniFilePath, SectionName, ParamName, GainTuning[i], 4, NULL);

	}

	BRD_AdcStartMode start;
	GetStartMode(pDev, &start);

	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartSource"), (ULONG)start.src, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartInverting"), (ULONG)start.inv, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartLevel"), start.level, 2, NULL);

	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartBaseSource"), (ULONG)start.stndStart.startSrc, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartBaseInverting"), (ULONG)start.stndStart.startInv, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartMode"), (ULONG)start.stndStart.trigOn, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StopSource"), (ULONG)start.stndStart.trigStopSrc, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StopInverting"), (ULONG)start.stndStart.stopInv, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ReStart"), (ULONG)start.stndStart.reStartMode, 10, NULL);

	U32		nStartClkSl;

	GetStartClkSl(pDev, &nStartClkSl);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartClkSl"), (ULONG)nStartClkSl, 10, NULL);

	// the function flushes the cache
	IPC_writePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetCfg(PBRD_AdcCfg pCfg)
{
	ADC214X400MSRV_Cfg	*pSrvCfg = (ADC214X400MSRV_Cfg*)m_pConfig;

	CAdcSrv::GetCfg(pCfg);
	if( (pSrvCfg->nSubType&0xF) == 0 )	// Тип убмодуля - не WB
		pCfg->ChanType = 0;				// Коэф. передачи измеряется в разах
	else								// Тип субмодуля - WB
		pCfg->ChanType = 1;				// Коэф. передачи измеряется в дБ

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetClkSource(CModule* pModule, ULONG nClkSrc)
{
	ADM2IF_FSRC		regFsrc;

	//
	// Тактовая частота всегда выбирается только на субмодуле
	//
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &regFsrc.AsWhole );
	regFsrc.ByBits.Gen = nClkSrc;
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, regFsrc.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetClkSource(CModule* pModule, ULONG& refClkSrc)
{
	ADM2IF_FSRC		regFsrc;

	//
	// Определить режим работы тетрады: Single или Slave
	//
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &regFsrc.AsWhole );
	refClkSrc = regFsrc.ByBits.Gen;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC214X400MSRV_Cfg pAdcSrvCfg = (PADC214X400MSRV_Cfg)m_pConfig;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:		// disabled clock
		U32		nStartClkSl;

		GetStartClkSl(pModule, &nStartClkSl);
		if( nStartClkSl == 0 )
			ClkValue = 0.0;
		break;
	case BRDclks_ADC_SUBGEN0:
		ClkValue = pAdcSrvCfg->aRefGen[0];
		//ClkValue = pAdcSrvCfg->PllRefGen;
		//OnPll(pModule, ClkValue, pAdcSrvCfg->PllRefGen);
		break;
	case BRDclks_ADC_SUBGEN1:			// SubModule Clock (internal reference)
		ClkValue = pAdcSrvCfg->aRefGen[1];
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
int CAdc214x400mSrv::GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PADC214X400MSRV_Cfg pAdcSrvCfg = (PADC214X400MSRV_Cfg)m_pConfig;
	double ClkVal;
	switch(ClkSrc)
	{
	case BRDclks_ADC_DISABLED:		// disabled clock
		U32		nStartClkSl;

		ClkVal = 0.0;
		GetStartClkSl(pModule, &nStartClkSl);
		if( nStartClkSl )
			ClkVal = pAdcSrvCfg->SubExtClk;
		break;
	case BRDclks_ADC_SUBGEN0:
		ClkVal = pAdcSrvCfg->aRefGen[0];
		break;
	case BRDclks_ADC_SUBGEN1:
		ClkVal = pAdcSrvCfg->aRefGen[1];
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
int M214x400M_CLK_DIVIDER[BRD_CLKDIVCNT] = {1, 2, 4, 8, 16};

//****************************************************************************************
int SetClkDivOpt(double RateHz, double curClk, double minRate)
{
	int MaxDivider;
	int i = 0;

	if( 0.0 == curClk )
		return 1;
	if( minRate < 1000000.0 )
		minRate = 1000000.0;
	MaxDivider = (int)floor(curClk / minRate);

	for(i = BRD_CLKDIVCNT-1; i >= 0; i--)
		if(MaxDivider >= M214x400M_CLK_DIVIDER[i])
			break;
	
	int MaxInd = (i < 0) ? 0 : i;

	MaxDivider = M214x400M_CLK_DIVIDER[MaxInd];
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
				return M214x400M_CLK_DIVIDER[MaxInd];
			}
		}
		else 
		{
			for(int i = 0; i < MaxInd; i++) 
			{
				double RateLeft = curClk / M214x400M_CLK_DIVIDER[i];
				double RateRight = curClk / M214x400M_CLK_DIVIDER[i+1];

				if(RateHz <= RateLeft && RateHz >= RateRight)
				{
					if(RateLeft - RateHz <= RateHz - RateRight)
					{
						if (DeltaFreq > RateLeft - RateHz)
						{
							DeltaFreq = RateLeft - RateHz;
							return M214x400M_CLK_DIVIDER[i];
						}
					}
					else
					{
						if (DeltaFreq > RateHz - RateRight)
						{
							DeltaFreq = RateHz - RateRight;
							return M214x400M_CLK_DIVIDER[i+1];
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
int CAdc214x400mSrv::SetRate( CModule *pModule, double &refRate, ULONG nClkSrc, double dClkValue )
{
	U32					nAdcRateDivider;
	ADC214X400MSRV_Cfg	*pSrvCfg = (ADC214X400MSRV_Cfg*)m_pConfig;

	//
	// Вычислить значение делителя частоты для записи в регистр FDVR
	//
//	if(nClkSrc != BRDclks_ADC_EXTCLK)
//	{
		DeltaFreq = 1.e10;
		if( refRate > pSrvCfg->AdcCfg.MaxRate)
			refRate = pSrvCfg->AdcCfg.MaxRate;
		nAdcRateDivider = SetClkDivOpt( refRate, dClkValue, pSrvCfg->AdcCfg.MinRate);
//	}
//	else
//		AdcRateDivider = 1;

	//
	// Запись значение делителя частоты в регистр FDVR
	//
	U32				fdvr;

	fdvr = powof2(nAdcRateDivider);
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FDVR, fdvr );
	refRate = dClkValue / nAdcRateDivider;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetRate( CModule *pModule, double &refRate, double ClkValue)
{
	U32				fdvr;
	ULONG			nAdcRateDivider;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FDVR, &fdvr );
	nAdcRateDivider = (ULONG)pow(2., (int)fdvr);
	refRate = ClkValue / nAdcRateDivider;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetInpSrc(CModule* pModule, PVOID pCtrl)
{
	U32		inpSrc	= *(U32*)pCtrl;

	if( inpSrc != 0 )
		inpSrc = 1;

	//
	// Читать/писать регистр CONTROL1
	//
	ADC_CTRL	regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	regCtrl.ByBits.Clbr = inpSrc;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetInpSrc(CModule* pModule, PVOID pCtrl)
{
	U32		*pInpSrc	= (U32*)pCtrl;

	//
	// Читать регистр CONTROL1
	//
	ADC_CTRL	regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	*pInpSrc = regCtrl.ByBits.Clbr;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	ADC214X400MSRV_Cfg  *pSrvCfg  = (ADC214X400MSRV_Cfg*)m_pConfig;

	//
	// Проверить параметры
	//
	if( (pSrvCfg->nSubType & 0xF)  != 0 ) // WB(LF/HF) Subunit
	{
		return BRDerr_CMD_UNSUPPORTED;
	}
	if( Chan >= 2 ) 
	{
		return BRDerr_BAD_PARAMETER;
	}

	//if( pSrvCfg->nVersion >= 0x20 )	// Version 2.0 and more
	//	refBias *= -1.0;

	//
	// Получить режим работы входного тракта выбранного канала
	//
	int status = BRDerr_OK;
	double inp_range;
	status = GetInpRange(pModule, inp_range, Chan); 
	if( status < 0 )
		return status;

	//
	// Получить отклонение смещения нуля для выбранного канала
	//
	int			iiP, iiR, iiAdc, iiRange;
	ADC_INP		regInp;
	ADM2IF_GAIN	regGain;
	double		biasDeviation;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_INP, &regInp.AsWhole );
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, &regGain.AsWhole );

	iiP = (Chan==0) ? regInp.ByBits.InpType0 : regInp.ByBits.InpType1;
	iiR = (Chan==0) ? regInp.ByBits.InpR0 : regInp.ByBits.InpR1;
	iiAdc = Chan;
	iiRange = (Chan==0) ? regGain.ByBits.Chan0 : regGain.ByBits.Chan1;

	biasDeviation = pSrvCfg->awBiasDeviation[iiP][iiR][iiAdc][iiRange];
	biasDeviation /= 10000.0;
	biasDeviation *= inp_range;

	//
	// Скорректировать запрашиваемое смещение нуля
	//
	refBias -= biasDeviation;	// учесть отклонение
	refBias /= 2;				// учесть 200% от ШП

	if( pSrvCfg->nVersion >= 0x20 )	// Version 2.0 and more
		refBias *= -1.0;

	//
	// Рассчитать коды для ИПНов
	//

	double prec_range = inp_range / 10; // 10% from full range

	int max_dac_value = 255;
	int min_dac_value = 0;
	double half_dac_value = 128.;

	USHORT approx_data;
	if(fabs(refBias) > inp_range)
		approx_data = refBias > 0.0 ? max_dac_value : min_dac_value;
	else
		approx_data = (USHORT)floor((refBias / inp_range + 1.) * half_dac_value + 0.5);
	if(approx_data > max_dac_value)
		approx_data = max_dac_value;
	double approx_bias = inp_range * (approx_data / half_dac_value - 1.);
	double delta_bias = refBias - approx_bias;

	USHORT prec_data;
	if(fabs(delta_bias) > prec_range)
		prec_data = delta_bias > 0.0 ? max_dac_value : min_dac_value;
	else
		prec_data = (USHORT)floor((delta_bias / prec_range + 1.) * half_dac_value + 0.5);
	if(prec_data > max_dac_value)
		prec_data = max_dac_value;
	double prec_bias = prec_range * (prec_data / half_dac_value - 1.);

	//
	// Скорректировать реальное смещение нуля
	//
	refBias = approx_bias + prec_bias;
	if( pSrvCfg->nVersion >= 0x20 )	// Version 2.0 and more
		refBias *= -1.0;
	refBias *= 2.0;
	refBias += biasDeviation;

	//
	// Сохранить результаты расчета в m_pConfig
	//
	pSrvCfg->aBias[Chan] = refBias;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC214X400M_BIAS0 - 1] = (UCHAR)approx_data;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC214X400M_PRECBIAS0 - 1] = (UCHAR)prec_data;

	//
	// Записать полученные коды в ИПНы
	//
	MAIN_THDAC	thdac;

	thdac.ByBits.Data = approx_data;
	thdac.ByBits.Num = Chan + BRDtdn_ADC214X400M_BIAS0;
	IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, thdac.AsWhole );

	thdac.ByBits.Data = prec_data;
	thdac.ByBits.Num = Chan + BRDtdn_ADC214X400M_PRECBIAS0;
	IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, thdac.AsWhole );

	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;
	//param.tetr = m_MainTetrNum;
	//param.reg = MAINnr_THDAC;
	//PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	//pThDac->ByBits.Data = approx_data;
	//pThDac->ByBits.Num = Chan + BRDtdn_ADC214X400M_BIAS0;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//pThDac->ByBits.Data = prec_data;
	//pThDac->ByBits.Num = Chan + BRDtdn_ADC214X400M_PRECBIAS0;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);



	return status;
}

//***************************************************************************************
int CAdc214x400mSrv::GetBias(CModule* pModule, double& refBias, ULONG Chan)
{
	ADC214X400MSRV_Cfg  *pSrvCfg  = (ADC214X400MSRV_Cfg*)m_pConfig;

	if( (pSrvCfg->nSubType & 0xF)  != 0 ) // WB(LF/HF) Subunit
	{
		//refBias = 0.0;
		return BRDerr_CMD_UNSUPPORTED;
	}

	int Status = BRDerr_OK;
	refBias = pSrvCfg->aBias[Chan];
	return Status;
}

const double BRD_ADC_MAXGAINTUN	= 10.; // max gain tuning = 10%
//***************************************************************************************
int CAdc214x400mSrv::SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC214X400MSRV_Cfg pSrvCfg = (PADC214X400MSRV_Cfg)m_pConfig;
	double max_thr = BRD_ADC_MAXGAINTUN; // %
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

	pSrvCfg->aGainTun[Chan] = GainTuning;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC214X400M_GAINTUN0 - 1] = (UCHAR)dac_data;

	MAIN_THDAC		regThdac;

	regThdac.ByBits.Data = dac_data;
	regThdac.ByBits.Num = Chan + BRDtdn_ADC214X400M_GAINTUN0;
	IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, regThdac.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan)
{
	PADC214X400MSRV_Cfg pSrvCfg = (PADC214X400MSRV_Cfg)m_pConfig;
//	pValChan->value = pSrvCfg->GainTun;
//	pValChan->chan = 0;
//	pValChan->value = pSrvCfg->ClkPhase[pValChan->chan];
	GainTuning = pSrvCfg->aGainTun[Chan];
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetStartMode(CModule* pModule, PVOID pStartStopMode)
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
		ADM2IF_MODE0	mode0;
		ADM2IF_STMODE	stmode;

		IndRegRead( pModule, m_MainTetrNum, ADM2IFnr_MODE0, &mode0.AsWhole );
		mode0.ByBits.Start = 1;
		mode0.ByBits.AdmClk = 1;
		mode0.ByBits.Master = 1;
		IndRegWrite( pModule, m_MainTetrNum, ADM2IFnr_MODE0, mode0.AsWhole );

		IndRegRead( pModule, m_MainTetrNum, ADM2IFnr_STMODE, &stmode.AsWhole );
		stmode.ByBits.SrcStart = BRDsts_TRDADC;
		stmode.ByBits.TrigStart = 0;
		IndRegWrite( pModule, m_MainTetrNum, ADM2IFnr_STMODE, stmode.AsWhole );

		//DEVS_CMD_Reg param;
		//param.idxMain = m_index;
		//param.tetr = m_MainTetrNum;
		//param.reg = ADM2IFnr_MODE0;
		//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		//PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
		//pMode0->ByBits.Start = 1;
		//pMode0->ByBits.AdmClk = 1;
		//pMode0->ByBits.Master = 1;
		//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		//param.reg = ADM2IFnr_STMODE;
		//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		//PADM2IF_STMODE pStMode = (PADM2IF_STMODE)&param.val;
		//pStMode->ByBits.SrcStart = BRDsts_TRDADC;
		//pStMode->ByBits.TrigStart = 0;
		//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	return Status;
}
 
//***************************************************************************************
int CAdc214x400mSrv::GetStartMode(CModule* pModule, PVOID pStartStopMode)
{
	int Status = BRDerr_OK;
	BRD_AdcStartMode *pAdcStartMode = (BRD_AdcStartMode*)pStartStopMode;
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
int CAdc214x400mSrv::SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr)
{
	ADC214X400MSRV_Cfg		*pSrvCfg = (ADC214X400MSRV_Cfg*)m_pConfig;
	ADC_CTRL				regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	regCtrl.ByBits.StartSrc = source;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

	//
	// Ожидание готовности
	//
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
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

	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.;

	double value = *pCmpThr;
	int max_code = 255;//ADC214X400M_STARTEXTMAXCODE;
	int min_code = 0;//ADC214X400M_STARTEXTMINCODE;
	if(BRDsts_ADC_CHAN0 == source || BRDsts_ADC_CHAN1 == source)
		value = (*pCmpThr / inp_range) * max_thr;

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
		*pCmpThr = (value * inp_range) / max_thr;

	pSrvCfg->StCmpThr = *pCmpThr;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC214X400M_STARTCMP - 1] = (UCHAR)dac_data;

	MAIN_THDAC		regThdac;

	regThdac.ByBits.Data = dac_data;
	regThdac.ByBits.Num = BRDtdn_ADC214X400M_STARTCMP;
	IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, regThdac.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr)
{
	ADC214X400MSRV_Cfg *pSrvCfg = (ADC214X400MSRV_Cfg*)m_pConfig;
	ADC_CTRL				regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	*pSource = regCtrl.ByBits.StartSrc;
	*pInv    = regCtrl.ByBits.StartInv;
	*pCmpThr = pSrvCfg->StCmpThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetClkLocation(CModule* pModule, ULONG& mode)
{
	mode = 1;
	CAdcSrv::SetClkLocation(pModule, mode);
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetClkLocation(CModule* pModule, ULONG& mode)
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
int CAdc214x400mSrv::SetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	ADC214X400MSRV_Cfg  *pSrvCfg  = (ADC214X400MSRV_Cfg*)m_pConfig;
	int			status = BRDerr_OK;
	//ULONG		mask;


	//
	// Для субмодулей "не WB" вызываем функцию SetInpRange()
	//
	if( (pSrvCfg->nSubType & 0xF)  == 0 )
	{
		double		inpRange;

		inpRange = pSrvCfg->AdcCfg.InpRange / 1000.0 / refGain;
		status = SetInpRange( pModule, inpRange, Chan );
		refGain = pSrvCfg->AdcCfg.InpRange / 1000.0 / inpRange;

		return status;
	}


	//
	// WB LF Subunit
	//
	if( (pSrvCfg->nSubType & 0xF)  == 0x2 )
	{
		S32		code;

		//
		// Truncate GainDB
		//
		if( refGain < -4.0 )
			refGain = -4.0;
		if( refGain > 20.0 )
			refGain = 20.0;

		//
		// Вычислить код для заданного коэф. усиления
		//
		code = 20 - (S32)refGain;

		//
		// Записать код в регистр GAIN
		//
		U32		value;

		IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, &value );
		if( Chan==0 )
		{
			value &= ~(0x1F << 0);
			value |= code << 0;
		}
		if( Chan==1 )
		{
			value &= ~(0x1F << 8);
			value |= code << 8;
		}
		IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, value );


		//
		// Сохранить заданное значение
		//
		pSrvCfg->aGainDb[Chan] = refGain;
	}

	//
	// WB HF Subunit
	//
	if( (pSrvCfg->nSubType & 0xF)  == 0x1 )
	{
		U32		thdacCode = 0x80;
		int		ii;

		//
		// Calculate Thdac Code
		//

		//
		// Truncate GainDB
		//
		if( refGain < -37.0 )
			refGain = -37.0;
		if( refGain > 20.0 )
			refGain = 20.0;

		//
		// Use Approximation Table
		//
		double	aG[9] = { -37.0,  -30.0,  -25.0, -12.5, -5.0,  0.0,  5.0, 15.0, 20.0 };
		double	aU[9] = {  -2.5, -1.428, -1.028,   0.0, 0.53, 0.97, 1.37, 2.17,  2.5 };
		double	thdacVoltage = 0.0;

		for( ii=0; ii<=8; ii++ )
		{
			if( (refGain >= aG[ii]) && (refGain <= aG[ii+1]))
			{
				//
				// Calculate Thdac Code
				//
				thdacVoltage  = (aU[ii+1]-aU[ii]);
				thdacVoltage /= (aG[ii+1]-aG[ii]);
				thdacVoltage *= (refGain -aG[ii]);
				thdacVoltage +=  aU[ii];

				thdacCode = (U32)(-thdacVoltage / 2.5 * 127.0 + 127.0);
				
				//
				// Calculate Real GainDB
				//
				thdacVoltage  = (double)((S32)thdacCode-127);
				thdacVoltage *= -2.5 / 127.0;
				
				refGain  = (aG[ii+1]-aG[ii]);
				refGain /= (aU[ii+1]-aU[ii]);
				refGain *= (thdacVoltage -aU[ii]);
				refGain +=  aG[ii];

				break;
			}
		}

		//
		// Write Thdac Code to THDACs
		//
		MAIN_THDAC		regThdac;

		pSrvCfg->aGainDb[Chan] = refGain;
		pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC214X400M_GAIN0 - 1] = (U08)thdacCode;

		regThdac.ByBits.Data = thdacCode;
		regThdac.ByBits.Num = Chan + BRDtdn_ADC214X400M_GAIN0;
		IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, regThdac.AsWhole);

		//
		// Set precision IPN to 0 Volt
		//
		regThdac.ByBits.Data = 0x80;
		regThdac.ByBits.Num = Chan + BRDtdn_ADC214X400M_PRECGAIN0;
		IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, regThdac.AsWhole);

	}

	return status;
}

//***************************************************************************************
int CAdc214x400mSrv::GetGain(CModule* pModule, double& refGain, ULONG Chan)
{
	ADC214X400MSRV_Cfg  *pSrvCfg  = (ADC214X400MSRV_Cfg*)m_pConfig;
	int				status;

	//
	// Для субмодулей "не WB" вызываем функцию GetInpRange()
	//
	if( (pSrvCfg->nSubType & 0xF)  == 0 )
	{
		double		inpRange;

		status = GetInpRange( pModule, inpRange, Chan );
		refGain = pSrvCfg->AdcCfg.InpRange / 1000.0 / inpRange;
		
		return status;
	}

	//
	// WB HF Subunit
	//
	if( (pSrvCfg->nSubType & 0xF) == 0x1 )
	{
		GetGainWB( pModule, refGain, Chan);
	}

	//
	// WB LF Subunit
	//
	if( (pSrvCfg->nSubType & 0xF) == 0x2 )
	{
		GetGainWB( pModule, refGain, Chan);
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	ADC214X400MSRV_Cfg  *pSrvCfg  = (ADC214X400MSRV_Cfg*)m_pConfig;
	int					status = BRDerr_OK;
	ULONG				mask;
	int					ii;

	//
	// Для субмодулей WB(LF/HF) вызываем функцию SetGain()
	//
	if( (pSrvCfg->nSubType & 0xF)  != 0 ) // WB(LF/HF) Subunit
	{
		double		gainDB, tmp;

		tmp  = pSrvCfg->AdcCfg.InpRange / 1000.0;
		tmp /= refInpRange;
		gainDB  = log( tmp );
		gainDB *= 20.0;

		status = SetGain( pModule, gainDB, Chan );
		
		refInpRange = (pSrvCfg->AdcCfg.InpRange / pow(10., gainDB/20)) / 1000.;

		return status;
	}

	//
	// Для субмодулей "не WB" выполняем расчет здесь
	//

	//
	//
	// Вычислить код для заданной шкалы преобразования
	// Выбираем шкалу имеющую номинал больше заданной 
	// и максимально близкий к заданной
	//
	double	deltaRange;		// разница между выбранной шкалой и заданной шкалой
	double	deltaRangeTmp;	// разница между выбранной шкалой и заданной шкалой
	S32		deltaRangeNo;	// номер выбранной шкалы
	double	maxRange;		// максимальная шкала преобразования
	S32		maxRangeNo;		// номер максимальнаой шкалы преобразования

	deltaRange = 1000.0 * 1000.0 * 1000.0;
	deltaRangeNo = -1;
	maxRange = 0.0;
	maxRangeNo = -1;

	for( ii=0; ii<BRD_GAINCNT; ii++ )
	{
		//
		// Искать минимальное отклонение заданной шкалы преобразования
		//
		deltaRangeTmp = pSrvCfg->adIcrRange[ii] - refInpRange;
		if( deltaRangeTmp >=0 )
		{
			if( deltaRange >= deltaRangeTmp )
			{
				deltaRange = deltaRangeTmp;
				deltaRangeNo = ii;
			}
		}

		//
		// Искать максимальную шкалу преобразования
		//
		if( maxRange <= pSrvCfg->adIcrRange[ii] )
		{
			maxRange = pSrvCfg->adIcrRange[ii];
			maxRangeNo = ii;
		}
	}

	mask = deltaRangeNo;
	if( deltaRangeNo < 0 )
	{
		if( maxRangeNo < 0 )
			return BRDerr_NO_DATA;
		mask = maxRangeNo;
	}

	refInpRange = pSrvCfg->adIcrRange[mask];

	//
	// Записать значение кода в регистр GAIN
	//
	ADM2IF_GAIN		regGain;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, &regGain.AsWhole );
	switch(Chan)
	{
		case 0:
			regGain.ByBits.Chan0 = mask;
			break;
		case 1:
			regGain.ByBits.Chan1 = mask;
			break;
	}
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, regGain.AsWhole );

	return status;
}

//***************************************************************************************
int CAdc214x400mSrv::GetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	ADC214X400MSRV_Cfg  *pSrvCfg  = (ADC214X400MSRV_Cfg*)m_pConfig;
	int			status;

	//
	// Для субмодулей WB(LF/HF) вызываем функцию GetGainWB()
	//
	if( (pSrvCfg->nSubType & 0xF)  != 0 ) // WB(LF/HF) Subunit
	{
		double gain;

		status = GetGainWB( pModule, gain, Chan );
		refInpRange = (pSrvCfg->AdcCfg.InpRange / pow(10., gain/20)) / 1000.;

		return status;
	}

	//
	// Для субмодулей "не WB" выполняем расчет здесь
	//

	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;
	//param.tetr = m_AdcTetrNum;
	//param.reg = ADM2IFnr_GAIN;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//ADM2IF_GAIN		*pGain = (ADM2IF_GAIN*)&param.val;

	double			value = 0.0;
	ADM2IF_GAIN		regGain;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, &regGain.AsWhole );
	switch(Chan)
	{
		case 0:
			value = pSrvCfg->adIcrRange[regGain.ByBits.Chan0];
			break;
		case 1:
			value = pSrvCfg->adIcrRange[regGain.ByBits.Chan1];
			break;
	}
	refInpRange = value;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetCnt(CModule* pModule, ULONG numreg, PBRD_EnVal pEnVal)
{
	U32				val;
	ADM2IF_MODE0	regMode0;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_MODE0, &regMode0.AsWhole );

	switch(numreg)
	{
		case 0:
			regMode0.ByBits.Cnt0En = pEnVal->enable;
			break;
		case 1:
			regMode0.ByBits.Cnt1En = pEnVal->enable;
			break;
		case 2:
			regMode0.ByBits.Cnt2En = pEnVal->enable;
			break;
	}
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_MODE0, regMode0.AsWhole );

	//IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FTYPE, &val );
	//int widthFifo = val >> 3; // ширина FIFO (в байтах)
	int widthFifo = 8;

	val = pEnVal->value * sizeof(ULONG) / widthFifo; // pEnVal->value - в 32-битных словах
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_CNT0 + numreg, val );
	pEnVal->value = val * widthFifo / sizeof(ULONG); // pEnVal->value - в 32-битных словах

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetCnt(CModule* pModule, ULONG numreg, PBRD_EnVal pEnVal)
{
	U32				val;
	ADM2IF_MODE0	regMode0;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_MODE0, &regMode0.AsWhole );

	switch(numreg)
	{
		case 0:
			pEnVal->enable = regMode0.ByBits.Cnt0En;
			break;
		case 1:
			pEnVal->enable = regMode0.ByBits.Cnt1En;
			break;
		case 2:
			pEnVal->enable = regMode0.ByBits.Cnt2En;
			break;
	}

	//IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FTYPE, &val );
	//int widthFifo = val >> 3; // ширина FIFO (в байтах)
	int widthFifo = 8;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_CNT0 + numreg, &val );
	pEnVal->value = val * widthFifo / sizeof(ULONG); // pEnVal->value - в 32-битных словах

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetCode(CModule* pModule, ULONG type)
{
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetCode(CModule* pModule, ULONG& type)
{
	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetMaster(CModule* pModule, ULONG mode)
{
	//
	// Режимы синхронизации, задаваемые параметром mode:
	// бит D0 - режим нашей тетрады: 1 - Single, 0 - Slave
	// бит D1 - режим тетрады MAIN:  1 - Master, 0 - Slave
	//
	CAdcSrv::SetMaster(pModule, mode);

	//
	// Стартовать тетраду MAIN, если режим нашей тетрады - Slave
	//
	if(mode==2)
	{
		ADM2IF_MODE0	mode0;

		IndRegRead( pModule, m_MainTetrNum, ADM2IFnr_GAIN, &mode0.AsWhole );
		mode0.ByBits.Start = 1;
		mode0.ByBits.AdmClk = 1;
		mode0.ByBits.Master = 1;
		IndRegWrite( pModule, m_MainTetrNum, ADM2IFnr_GAIN, mode0.AsWhole );
	}
	return BRDerr_OK;
}

//***************************************************************************************
//int CAdc214x400mSrv::ExtraInit(CModule* pModule)
//{
//	PADC214X400MSRV_Cfg pAdcSrvCfg = (PADC214X400MSRV_Cfg)m_pConfig;
//	for(int i = 0; i < 16; i++)
//		pAdcSrvCfg->AdcReg[i] = 0;
//
//	//UCHAR regval = 2;
//	//SetAdcReg(pDev, 0x6C, regval);
//	//regval = 8;
//	//SetAdcReg(pDev, 0x6C, regval);
//
//	InitPLL(pModule);
//	return BRDerr_OK;
//}

//***************************************************************************************
int CAdc214x400mSrv::SetClkInv(CModule* pModule, ULONG nInv)
{
	ADM2IF_FSRC		regFsrc;
	
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &regFsrc.AsWhole );
	regFsrc.ByBits.Inv = nInv;
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, regFsrc.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetClkInv(CModule* pModule, ULONG& refInv)
{
	ADM2IF_FSRC		regFsrc;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &regFsrc.AsWhole );
	refInv = regFsrc.ByBits.Inv;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec)
{
//	int Status = BRDerr_CMD_UNSUPPORTED;
	int Status = BRDerr_OK;
	CAdcSrv::SetSpecific(pModule, pSpec);
	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;

	switch(pSpec->command)
	{
		case ADC214X400Mcmd_SETMU:
			SetMu(pModule, pSpec->arg);
			break;
		case ADC214X400Mcmd_GETMU:
			GetMu(pModule, pSpec->arg);
			break;
		case ADC214X400Mcmd_SETSTARTCLKSL:
			SetStartClkSl(pModule, pSpec->arg);
			break;
		case ADC214X400Mcmd_GETSTARTCLKSL:
			GetStartClkSl(pModule, pSpec->arg);
			break;

		//
		// Специальные команды для спецпрошивки ПЛИС ADM модификации 0x100
		// (узел прорерживания sumsung)
		//
		case ADC214X400Mcmd_SMSETMODE:
			SmSetMode(pModule, pSpec->arg);
			break;
		case ADC214X400Mcmd_SMLOADDATA:
			SmLoadData(pModule, pSpec->arg);
			break;
		case ADC214X400Mcmd_SMISAVAILABLE:
			SmIsAvailable(pModule, pSpec->arg);
			break;
	}
	return Status;
}

//***************************************************************************************
int CAdc214x400mSrv::SetMu(CModule* pModule, void	*args)
{
	PADC214X400MSRV_MU pMU = (PADC214X400MSRV_MU)args;

	//
	// Проверить тип субмодуля в параметрах
	//
	if( pMU->subType != 0 )
		return BRDerr_BAD_PARAMETER;

	//
	// CAdcSrv::SetProperties()
	//
	SetMaster(pModule, pMU->syncMode);
	CAdcSrv::SetChanMask(pModule, pMU->chanMask);

	//PADC214X200MSRV_CFG pAdcSrvCfg = (PADC214X200MSRV_CFG)m_pConfig;
	SetClkSource(pModule, pMU->clockSrc);
	SetClkValue(pModule, pMU->clockSrc, pMU->clockValue);
	SetRate(pModule, pMU->samplingRate, pMU->clockSrc, pMU->clockValue);

	CtrlFormat( pModule, NULL, BRDctrl_ADC_SETFORMAT, &pMU->format );

	ULONG		code = 0;		//!!!
	SetCode( pModule, code );	//!!!


	for(int i = 0; i < MAX_CHAN; i++)
	{
		SetInpRange(pModule, pMU->range[i], i);
		SetBias(pModule, pMU->bias[i], i);
		SetInpResist(pModule, pMU->resist[i], i);
		SetDcCoupling(pModule, pMU->dcCoupling[i], i);
//!!!	SetGainTuning(pModule, pMU->gainTuning[i], i);
	}


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

	//
	// CAdc214x400mSrv::SetProperties()
	//

	SetClkSource(pModule, pMU->clockSrc);

	U32		nStartClkSl = 0;
	SetStartClkSl( pModule, &nStartClkSl );

	SetClkValue(pModule, pMU->clockSrc, pMU->clockValue);
	SetRate(pModule, pMU->samplingRate, pMU->clockSrc, pMU->clockValue);
	SetClkInv(pModule, pMU->invClk);	//!!!

	for(int i = 0; i < MAX_CHAN; i++)
	{
		SetInpRange(pModule, pMU->range[i], i);
		SetBias(pModule, pMU->bias[i], i);
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

	CAdcSrv::GetStartMode(pModule, &start_mode.stndStart);
	SetStartMode(pModule, &start_mode);

	pMU->startLevel = start_mode.level;

	return BRDerr_OK;
}

/*
// ***************************************************************************************
int CAdc214x400mSrv::SetMu(CModule* pModule, void	*args)
{
	PADC214X400MSRV_MU pMU = (PADC214X400MSRV_MU)args;

	//
	// Проверить тип субмодуля в параметрах
	//
	if( pMU->subType != 0 )
		return BRDerr_BAD_PARAMETER;

	CAdcSrv::SetChanMask(pModule, pMU->chanMask);
	SetMaster(pModule, pMU->syncMode);

	CtrlFormat( pModule, NULL, BRDctrl_ADC_SETFORMAT, &pMU->format );

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

	return BRDerr_OK;
}
*/
//***************************************************************************************
int CAdc214x400mSrv::GetMu(CModule* pModule, void	*args)
{
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SetStartClkSl(CModule* pModule, void *args)
{
	U32			*pParam = (U32*)args;
	ADC_CTRL	regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	regCtrl.ByBits.StartClkSl = (*pParam) & 0x1;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetStartClkSl(CModule* pModule, void *args)
{
	U32			*pParam = (U32*)args;
	ADC_CTRL	regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	*pParam = regCtrl.ByBits.StartClkSl;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::GetGainWB(CModule* pModule, double& refGain, ULONG Chan)
{
	ADC214X400MSRV_Cfg  *pSrvCfg  = (ADC214X400MSRV_Cfg*)m_pConfig;

	if( (pSrvCfg->nSubType & 0xF) == 0 ) 
	{
		// Standard Subunit
		refGain = 1.0;
		return BRDerr_ERROR;
	}

	//
	// WB HF Subunit
	//
	if( (pSrvCfg->nSubType & 0xF) == 0x1 )
	{
		refGain = pSrvCfg->aGainDb[Chan];
		if( refGain < -37.0 )
			refGain = -37.0;
		if( refGain > 20.0 )
			refGain = 20.0;
	}

	//
	// WB LF Subunit
	//
	if( (pSrvCfg->nSubType & 0xF) == 0x2 )
	{
		refGain = pSrvCfg->aGainDb[Chan];
		if( refGain < -4.0 )
			refGain = -4.0;
		if( refGain > 20.0 )
			refGain = 20.0;
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SmSetMode(CModule* pModule, void *args)
{
	ADC214X400MSRV_SMSETMODE	*pParam = (ADC214X400MSRV_SMSETMODE*)args;
	U32				regVal;

	if( (pParam->sumCnt%2) == 1 )
		return BRDerr_BAD_PARAMETER;
	if( pParam->shiftCnt > 16 || pParam->shiftCnt < 0 )
		return BRDerr_BAD_PARAMETER;

	regVal  = pParam->isCalculate & 1;
	regVal |= (pParam->isTest & 1) << 4;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CLCTRL, regVal );

	regVal = pParam->sumCnt / 2;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CLSUM, regVal );

	regVal = (1<<pParam->shiftCnt) - 1;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CLSHIFT, regVal );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SmLoadData(CModule* pModule, void *args)
{
	ADC214X400MSRV_SMLOADDATA	*pParam = (ADC214X400MSRV_SMLOADDATA*)args;
	U32				regVal, data;
	int				ii;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_IDMOD, &regVal );
	regVal &= ~0x20;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CLCTRL, regVal );
	regVal |= 0x20;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CLCTRL, regVal );

	for( ii=0; ii<pParam->size; ii++ )
	{
		data = pParam->pBuf[ii];
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CLDATA, data );
	}

	regVal &= ~0x20;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CLCTRL, regVal );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x400mSrv::SmIsAvailable(CModule* pModule, void *args)
{
	U32			*pParam = (U32*)args;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_IDMOD, pParam );

	return BRDerr_OK;
}
/*
// ***************************************************************************************
S32		CAdc214x400mSrv::IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, U32 *pVal )
{
	DEVS_CMD_Reg	param;

	param.idxMain = m_index;
	param.tetr = tetrNo;
	param.reg = regNo;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	*pVal = param.val;

	return BRDerr_OK;
}

// ***************************************************************************************
S32		CAdc214x400mSrv::IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, ULONG *pVal )
{
	return IndRegRead( pModule, tetrNo, regNo, (U32*)pVal );
}

// ***************************************************************************************
S32		CAdc214x400mSrv::IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, U32 val )
{
	DEVS_CMD_Reg	param;

	param.idxMain = m_index;
	param.tetr = tetrNo;
	param.reg = regNo;
	param.val = val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

// ***************************************************************************************
S32		CAdc214x400mSrv::IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, ULONG val )
{
	return IndRegWrite( pModule, tetrNo, regNo, (U32)val );
}
*/
// ***************** End of file Adc214x400mSrv.cpp ***********************
