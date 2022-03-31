/*
 ***************** File Fm212x1gSrv.cpp ************
 *
 * BRD Driver for ADС service on FM212x1G
 *
 * (C) InSys by Ekkore Nov 2011
 *
 ******************************************************
*/

#include "gipcy.h"

#include "module.h"
#include "realdelay.h"
#include "adcfm212x1gsrv.h"

#define	CURRFILE "FM212x1GSRV"

const double BRD_ADC_MAXGAINTUN	= 1.; // not used

//***************************************************************************************
CFm212x1gSrv::CFm212x1gSrv(int idx, int srv_num, CModule* pModule, FM212x1GSRV_Cfg *pCfg) :
	CAdcSrv(idx, _BRDC("FM212X1G"), srv_num, pModule, pCfg, sizeof(FM212x1GSRV_Cfg))
{
}

//***************************************************************************************
int CFm212x1gSrv::CtrlRelease( void *pDev, void *pServData, ULONG cmd, void *args )
{
	CModule* pModule = (CModule*)pDev;

	SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);

	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб (вместо BRDerr_OK)
}

//***************************************************************************************
void CFm212x1gSrv::GetAdcTetrNum( CModule *pModule )
{
	//
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	//

	//
	// Поиск номера терады. Если тетрада не найдена, то -1.
	//
	m_AdcTetrNum = GetTetrNum(pModule, m_index, FM212x1G_TETR_ID);
	if(-1==m_AdcTetrNum)
		m_AdcTetrNum  = GetTetrNum( pModule, m_index, FM212x1G_KFO_TETR_ID );
}

//***************************************************************************************
void CFm212x1gSrv::FreeInfoForDialog( PVOID pInfo )
{
	FM212x1GSRV_Info *pSrvInfo = (FM212x1GSRV_Info*)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CFm212x1gSrv::GetInfoForDialog( CModule *pModule )
{
	FM212x1GSRV_Cfg  *pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	FM212x1GSRV_Info *pInfo = new FM212x1GSRV_Info;
	int					ii;

	pInfo->Size = sizeof(FM212x1GSRV_Info);

	UCHAR code = pSrvCfg->AdcCfg.Encoding;
	
	pInfo->pAdcInfo = (ADCSRV_INFO*)CAdcSrv::GetInfoForDialog(pModule);
	if(pInfo->pAdcInfo->SyncMode == 3)
		pInfo->pAdcInfo->SyncMode = 1;
	pInfo->pAdcInfo->Encoding = code;
	pInfo->ExtClk = pSrvCfg->nSubExtClk;
	GetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
	GetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	GetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockValue);
	if(pInfo->pAdcInfo->ClockSrc == BRDclks_ADC_EXTCLK)
		pInfo->ExtClk = pInfo->pAdcInfo->ClockValue;

	ULONG mode;
	
	GetClkInv(pModule, mode);
	pInfo->InvClk = (UCHAR)mode;
	for( ii = 0; ii < MAX_CHAN; ii++ )
	{
		GetInpRange(pModule, pInfo->pAdcInfo->Range[ii], ii );
		GetBias(pModule, pInfo->pAdcInfo->Bias[ii], ii );
		GetInpFilter(pModule, (ULONG&)pInfo->aisInpFilter[ii], ii);
		GetDcCoupling( pModule, (ULONG&)pInfo->pAdcInfo->DcCoupling[ii], ii );
//		GetClkPhase(pDev, pSrvInfo->PhaseTuning[i], i);
		
		GetGainTuning( pModule, pInfo->aGainTun[ii], ii);
	}

	GetStartSlave(pModule, (ULONG&)pInfo->nStartSlave );
	GetClockSlave(pModule, (ULONG&)pInfo->nClockSlave );

	BRD_AdcStartMode start_mode;
	
	GetStartMode(pModule, &start_mode);
	pInfo->StartSrc = start_mode.src;
	pInfo->InvStart = start_mode.inv;
	pInfo->StartLevel = start_mode.level;
	pInfo->pAdcInfo->StartSrc = start_mode.stndStart.startSrc;
	pInfo->pAdcInfo->StartInv = start_mode.stndStart.startInv;
	pInfo->pAdcInfo->StartStopMode = start_mode.stndStart.trigOn;
	pInfo->pAdcInfo->StopSrc	= start_mode.stndStart.trigStopSrc;
	pInfo->pAdcInfo->StopInv	= start_mode.stndStart.stopInv;
	pInfo->pAdcInfo->ReStart = start_mode.stndStart.reStartMode;

	pInfo->Version      = pSrvCfg->nVersion;             // версия субмодуля
	pInfo->nAdcType		= pSrvCfg->nAdcType;	
	pInfo->nDacType		= pSrvCfg->nDacType;	
	pInfo->nDacRange	= pSrvCfg->nDacRange;	
	pInfo->nGenType		= pSrvCfg->nGenType;	
	pInfo->nGenAdr		= pSrvCfg->nGenAdr;	
	pInfo->nGenRef	    = pSrvCfg->nGenRef;    
	pInfo->nGenRefMax	= pSrvCfg->nGenRefMax; 

	for( ii=0; ii<2; ii++ )
		pInfo->adGainDb[ii] = pSrvCfg->adGainDb[ii];

	GetInpSrc(pModule, &(pInfo->nInputSource) );


	//BRDCHAR		msg[256];
	//BRDC_sprintf( msg, _BRDC("GetInfoForDialog: clkSrc = %d"), pInfo->pAdcInfo->ClockSrc);
	//MessageBox(NULL, msg, _BRDC("Debug"), MB_OK);

	return pInfo;
}

//***************************************************************************************
int CFm212x1gSrv::SetPropertyFromDialog( void *pDev, void *args )
{
	CModule				*pModule = (CModule*)pDev;
	FM212x1GSRV_Info	*pInfo = (FM212x1GSRV_Info*)args;
	FM212x1GSRV_Cfg	*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;
	int					ii;

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

	pSrvCfg->nSubExtClk = ROUND(pInfo->ExtClk);

	//BRDCHAR		msg[256];
	//BRDC_sprintf( msg, _BRDC("SetPropertyFromDialog: clkSrc = %d"), pInfo->pAdcInfo->ClockSrc);
	//MessageBox(NULL, msg, _BRDC("Debug"), MB_OK);
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);

	SetStartSlave(pModule, pInfo->nStartSlave );
	SetClockSlave(pModule, pInfo->nClockSlave );

	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);

	SetClkInv(pModule, pInfo->InvClk);

	for( ii = 0; ii < MAX_CHAN; ii++ )
	{
		SetInpRange(pModule, pInfo->pAdcInfo->Range[ii], ii);
		SetBias(pModule, pInfo->pAdcInfo->Bias[ii], ii);
		SetInpFilter(pModule, pInfo->aisInpFilter[ii], ii);
//		SetDcCoupling(pModule, pInfo->pAdcInfo->DcCoupling[ii], ii);
//		SetClkPhase(pModule, pInfo->PhaseTuning[i], i);
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
int CFm212x1gSrv::SetProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
    //FM212x1GSRV_Cfg	*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;
	BRDCHAR	Buffer[128];
	BRDCHAR	ParamName[128];
	BRDCHAR*	endptr;
	int		ii;

	//
	// Извлечение параметров работы субмодуля из INI-файла 
	// и инициализация субмодуля в соответствии с этими параметрами
	//


	//
	// Извлечение стандартных параметров
	//
	CAdcSrv::SetProperties(pModule, iniFilePath, SectionName);

	//
	// Извлечение источника тактовой частоты
	//
	U32			clkSrc;

    IPC_getPrivateProfileString(SectionName, _BRDC("ClockSource"), _BRDC("129"), Buffer, sizeof(Buffer), iniFilePath);
	clkSrc = BRDC_strtol(Buffer, &endptr, 0);
	SetClkSource(pModule, clkSrc);

	//
	// Извлечение частоты внешнего генератора
	//
	double				clkValue;

    IPC_getPrivateProfileString(SectionName, _BRDC("ExternalClockValue"), _BRDC("120000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	clkValue = BRDC_atof(Buffer);
	SetClkValue(pModule, BRDclks_ADC_EXTCLK, clkValue);

	//
	// Установление частоты генератора Si571 для работы синтезатора в режиме PLL
	//
    IPC_getPrivateProfileString(SectionName, _BRDC("SubClockValue"), _BRDC("1000.0"), Buffer, sizeof(Buffer), iniFilePath);
	clkValue = BRDC_atof(Buffer);
	SetClkValue(pModule, BRDclks_ADC_SUBGEN_PLL, clkValue);

	//
	// Извлечение требуемой частоты дискретизации
	//
	double			rate;
	
    IPC_getPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("100000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	rate = BRDC_atof(Buffer);
	SetRate(pModule, rate, clkSrc, clkValue);

	//
	// Извлечение маски каналов
	//
	ULONG chanMask;

    IPC_getPrivateProfileString(SectionName, _BRDC("ChannelMask"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
	chanMask = BRDC_strtol(Buffer, &endptr, 0);
	SetChanMask(pModule, chanMask);

	//
	// Извлечение для каждого канала требуемых:
	// 1. шкалы преобразования
	// 2. смещения нуля
	// 3. открытый/закрытый вход
	// 4. входное сопротивление
	//
	double			dVal;
	ULONG			nVal;
	DWORD			ret;

	for( ii = 0; ii < MAX_ADC_CHAN; ii++ )
	{
		BRDC_sprintf( ParamName, _BRDC("GainDB%d"), ii);
        ret = IPC_getPrivateProfileString( SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		dVal = BRDC_atof(Buffer);
		SetGain(pModule, dVal, ii);

		BRDC_sprintf( ParamName, _BRDC("Bias%d"), ii);
        ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		dVal = BRDC_atof(Buffer);
		SetBias(pModule, dVal, ii);
		
		BRDC_sprintf( ParamName, _BRDC("DcCoupling%d"), ii);
        ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
		nVal = BRDC_atoi(Buffer);
		SetDcCoupling(pModule, nVal, ii);

		BRDC_sprintf( ParamName, _BRDC("InputResistance%d"), ii);
        ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
		nVal = BRDC_atoi(Buffer);
		SetInpResist(pModule, nVal, ii);
	}

	//
	// Извлечение требуемого режима стартовой синхронизации
	//
	BRD_AdcStartMode rStMode;

    IPC_getPrivateProfileString(SectionName, _BRDC("StartSource"), _BRDC("3"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.src = BRDC_atoi(Buffer);

    IPC_getPrivateProfileString(SectionName, _BRDC("StartInverting"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.inv = BRDC_atoi(Buffer);

    IPC_getPrivateProfileString(SectionName, _BRDC("StartLevel"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.level = BRDC_atof(Buffer);

	CAdcSrv::GetStartMode(pModule, &rStMode.stndStart);
	SetStartMode(pModule, &rStMode);

	//
	// Входное сопротивление сигнала внешнего старта
	//
	ULONG		inpResist;

    IPC_getPrivateProfileString(SectionName, _BRDC("StartResistance"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	inpResist = BRDC_atoi(Buffer);
	SetInpResist( pModule, inpResist, 1024 );

	//
	// Режим удвоения частоты
	//
	FM212x1GSRV_DBLCLK		rDblClk;
	U32						nDoubleFreq;
	U32						isDoubleCalibrate;
	double					dDoubleFreqBias0, dDoubleFreqRange0;
	double					dDoubleFreqBias1, dDoubleFreqRange1;

	IPC_getPrivateProfileString(SectionName, _BRDC("DoubleFreq"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	nDoubleFreq = BRDC_atoi(Buffer);

	IPC_getPrivateProfileString(SectionName, _BRDC("DoubleCalibrate"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	isDoubleCalibrate = BRDC_atoi(Buffer);

    IPC_getPrivateProfileString(SectionName, _BRDC("DoubleFreqBias0"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	dDoubleFreqBias0 = BRDC_atof(Buffer);

    IPC_getPrivateProfileString(SectionName, _BRDC("DoubleFreqBias1"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	dDoubleFreqBias1 = BRDC_atof(Buffer);

    IPC_getPrivateProfileString(SectionName, _BRDC("DoubleFreqRange0"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	dDoubleFreqRange0 = BRDC_atof(Buffer);

    IPC_getPrivateProfileString(SectionName, _BRDC("DoubleFreqRange1"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	dDoubleFreqRange1 = BRDC_atof(Buffer);

	rDblClk.size = sizeof(FM212x1GSRV_DBLCLK);
	rDblClk.isDblClk = (0==nDoubleFreq) ? 0 : 1;
	rDblClk.isDblClk|= (isDoubleCalibrate) ? 0x10 : 0x00;
	rDblClk.inpSrc   = (1==nDoubleFreq) ? 0 : 1;
	rDblClk.valRange = 0;
	rDblClk.inp0Range = dDoubleFreqRange0;
	rDblClk.inp1Range = dDoubleFreqRange1;
	rDblClk.inp0Bias  = dDoubleFreqBias0;
	rDblClk.inp1Bias  = dDoubleFreqBias1;
	SetDblClkEx( pModule, &rDblClk );

	//
	// Режим временных меток
	//
	U32		time_stamp; 
	IPC_getPrivateProfileString(SectionName,_BRDC("TimeStampOn"),_BRDC("0"),Buffer,sizeof(Buffer),iniFilePath);
	time_stamp = BRDC_strtol(Buffer,&endptr,0);
	TimeStamp(pModule, &time_stamp);

	//
	// Запись в регистр задержек шины данных субмодуля
	//
    IPC_getPrivateProfileString(SectionName, _BRDC("IODelay"), _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
	if( '\0' != Buffer[0] )
	{
		int		ioDelay;
		U32		val;

		ioDelay = BRDC_atoi(Buffer);
		val = (ioDelay<0) ? 0x0 : 0x1;
		if(ioDelay<0)
			ioDelay = -ioDelay;

		IndRegWrite( pModule, m_AdcTetrNum, 0x207, (U32)0x2 );	// сброс задержки в исх. состояние
		for( ii=0; ii<ioDelay; ii++ )
			IndRegWrite( pModule, m_AdcTetrNum, 0x207, val );
	}
	{
		BRDCHAR		keyName[20];
		int			keyIdx;

		for( keyIdx=0; keyIdx<=0xF; keyIdx++ )
		{
			BRDC_sprintf( keyName, _BRDC("IODelay%d"), keyIdx );
            IPC_getPrivateProfileString(SectionName, keyName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
			if( '\0' != Buffer[0] )
			{
				int		ioDelay;
				U32		val;

				ioDelay = BRDC_atoi(Buffer);
				//printf( ">>> '%s' = '%s'\n", keyName, Buffer );
				val = (ioDelay<0) ? 0x0 : 0x1;
				if(ioDelay<0)
					ioDelay = -ioDelay;

				IndRegWrite( pModule, m_AdcTetrNum, 0x2F0+keyIdx, (U32)0x2 );	// сброс задержки в исх. состояние
				for( ii=0; ii<ioDelay; ii++ )
					IndRegWrite( pModule, m_AdcTetrNum, 0x2F0+keyIdx, val );
			}
		}
	}

	//
	// Выполнить запись в любые косвенные регистры тетрады напрямую
	//
	U32			val;
	int			maxii = 20;
	S32			indRegNo;

	maxii = 20;

	for( ii = 0; ii < maxii; ii++ )
	{
		BRDC_sprintf( ParamName, _BRDC("IndReg%d"), ii);
        ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
		if( 0 == ret )
			continue;
		maxii = ii + 20;	// разрыв между индексами - не более 20

		val = BRDC_strtol( Buffer, &endptr, 0 );
		if( val == 0 )
			continue;

		indRegNo = (val>>16) & 0xFFF;
		IndRegWrite( pModule, m_AdcTetrNum, indRegNo, val & 0xFFFF );
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SaveProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	BRDCHAR				ParamName[128];
    //FM212x1GSRV_Cfg	*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;

	CAdcSrv::SaveProperties(pModule, iniFilePath, SectionName);

	ULONG		master;
	GetMaster(pModule, master);
	if( master == 3) 
		master = 1;
	WriteInifileParam(iniFilePath, SectionName, _BRDC("MasterMode"), master, 10, NULL);

	ULONG		clkSrc;
	GetClkSource(pModule, clkSrc);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ClockSource"), clkSrc, 10, NULL);

	double clkValue;
	GetClkValue(pModule, clkSrc, clkValue);
	if(clkSrc == BRDclks_ADC_EXTCLK)
		WriteInifileParam(iniFilePath, SectionName, _BRDC("ExternalClockValue"), clkValue, 2, NULL);
	
	double rate;
	GetRate(pModule, rate, clkValue);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("SamplingRate"), rate, 2, NULL);

	double	dVal;
	ULONG	nVal;

	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		GetGain(pModule, dVal, i);
		BRDC_sprintf( ParamName, _BRDC("GainDB%d"), i);
		WriteInifileParam(iniFilePath, SectionName, ParamName, dVal, 2, NULL);

		GetBias(pModule, dVal, i);
		BRDC_sprintf( ParamName, _BRDC("Bias%d"), i);
		WriteInifileParam(iniFilePath, SectionName, ParamName, dVal, 4, NULL);

		GetDcCoupling(pModule, nVal, i);
		BRDC_sprintf( ParamName, _BRDC("DcCoupling%d"), i);
		WriteInifileParam(iniFilePath, SectionName, ParamName, nVal, 10, NULL);

		GetInpResist(pModule, nVal, i);
		BRDC_sprintf( ParamName, _BRDC("InputResistance%d"), i);
		WriteInifileParam(iniFilePath, SectionName, ParamName, nVal, 10, NULL);
	}

	BRD_AdcStartMode start;
	GetStartMode(pModule, &start);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartSource"), (ULONG)start.src, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartInverting"), (ULONG)start.inv, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartLevel"), start.level, 2, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartBaseSource"), (ULONG)start.stndStart.startSrc, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartBaseInverting"), (ULONG)start.stndStart.startInv, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartMode"), (ULONG)start.stndStart.trigOn, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StopSource"), (ULONG)start.stndStart.trigStopSrc, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StopInverting"), (ULONG)start.stndStart.stopInv, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ReStart"), (ULONG)start.stndStart.reStartMode, 10, NULL);

	// the function flushes the cache
    IPC_writePrivateProfileString(NULL, NULL, NULL, iniFilePath);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::GetCfg(PBRD_AdcCfg pCfg)
{
    //FM212x1GSRV_Cfg	*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;

	CAdcSrv::GetCfg(pCfg);
	pCfg->ChanType = 1;				// Коэф. передачи измеряется в дБ

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetClkSource( CModule *pModule, ULONG nClkSrc )
{
	FM212x1GSRV_Cfg	*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;
	ADM2IF_FSRC			regFsrc;
	ADC_CTRL			regCtrl;

	//
	// Считать текущее значение регистров FSRC, CONTROL1
	//
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &regFsrc.AsWhole );
	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );

	//
	// Анализировать источние тпктовой частоты
	//
	switch( nClkSrc )
	{
		case BRDclks_ADC_DISABLED:
			regFsrc.ByBits.Gen    = 0x0;
			regCtrl.ByBits.RefSel = 0x0;
			break;
		case BRDclks_ADC_SUBGEN:
			regFsrc.ByBits.Gen    = 0x1;
			regCtrl.ByBits.RefSel = 0x0;
			break;
		case BRDclks_ADC_SUBGEN_PLL:
			regFsrc.ByBits.Gen    = 0x3;
			regCtrl.ByBits.RefSel = 0x1;
			break;
		case BRDclks_ADC_EXTCLK:
			regFsrc.ByBits.Gen    = 0x81;
			regCtrl.ByBits.RefSel = 0x0;
			break;
		case BRDclks_ADC_EXTCLK_PLL:
			regFsrc.ByBits.Gen    = 0x3;
			regCtrl.ByBits.RefSel = 0x2;
			break;
		default:
			return BRDerr_BAD_PARAMETER;
	}

	pSrvCfg->nClkSrc = nClkSrc;

	//
	// Записать новые значения в регистры FSRC, CONTROL1
	//
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, regFsrc.AsWhole );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

	//
	// Повторная запись, после того, как отработается выключение питания
	//
	//Sleep(100);
	RealDelay(100,1);
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, regFsrc.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::GetClkSource( CModule *pModule, ULONG &refClkSrc )
{
	FM212x1GSRV_Cfg	*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;

	refClkSrc = pSrvCfg->nClkSrc;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetClkValue( CModule *pModule, ULONG ClkSrc, double &refClkValue )
{
	FM212x1GSRV_Cfg	*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;

	switch(ClkSrc)
	{
		case BRDclks_ADC_DISABLED:
			refClkValue = 0.0;
			break;
		case BRDclks_ADC_SUBGEN:
		case BRDclks_ADC_SUBGEN_PLL:
			if( pSrvCfg->nGenType == 0 )
			{
				//
				// Генератор на субмодуле непрограммируемый
				//
				refClkValue = pSrvCfg->nGenRef;
			}
			else
			{
				double		dClkValue;

				Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
				if( dClkValue != refClkValue )
				{
					dClkValue = refClkValue;
					err = Si571SetRate( pModule, &dClkValue ); // Запрограммировать Si570/Si571
					refClkValue = dClkValue;
				}
			}
			break;
		case BRDclks_ADC_EXTCLK:
		case BRDclks_ADC_EXTCLK_PLL:
			pSrvCfg->nSubExtClk = ROUND(refClkValue);
			break;
		default:
			refClkValue = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}

	return err;
}
	
//***************************************************************************************
int CFm212x1gSrv::GetClkValue( CModule *pModule, ULONG ClkSrc, double &refClkValue )
{
	FM212x1GSRV_Cfg	*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;

	switch(ClkSrc)
	{
		case BRDclks_ADC_DISABLED:
			refClkValue = 0.0;
			break;
		case BRDclks_ADC_SUBGEN:
		case BRDclks_ADC_SUBGEN_PLL:
			if( pSrvCfg->nGenType == 0 )
			{
				//
				// Генератор на субмодуле непрограммируемый
				//
				refClkValue = pSrvCfg->nGenRef;
			}
			else
			{
				double		dClkValue ;

				Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
				refClkValue = dClkValue;
			}
			break;
		case BRDclks_ADC_EXTCLK:
		case BRDclks_ADC_EXTCLK_PLL:
			refClkValue = pSrvCfg->nSubExtClk;
			break;
		default:
			refClkValue = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}

	return err;
}
	
//***************************************************************************************
int CFm212x1gSrv::SetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;
	CAdcSrv::SetClkLocation(pModule, refMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::GetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetRate( CModule *pModule, double &refRate, ULONG nClkSrc, double ClkValue )
{
	FM212x1GSRV_Cfg	*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;
	double				dClkValue;

	if( refRate > pSrvCfg->AdcCfg.MaxRate )
		refRate = pSrvCfg->AdcCfg.MaxRate;
	if( refRate < pSrvCfg->AdcCfg.MinRate )
		refRate = pSrvCfg->AdcCfg.MinRate;

	//SetClkSource( pModule, nClkSrc );
	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:
			refRate = 0.0;
			break;
		case BRDclks_ADC_SUBGEN:
			if( pSrvCfg->nGenType == 0 )
			{
				//
				// Генератор на субмодуле непрограммируемый
				//
				refRate = pSrvCfg->nGenRef;
			}
			else
			{
				//
				// Записать тактову частоту в генератор Si570/Si571
				//
				double		dClkValueRd;

				Si571GetRate( pModule, &dClkValueRd ); // Считать Si570/Si571
				if( dClkValueRd != refRate )
					err = Si571SetRate( pModule, &refRate ); // Запрограммировать Si570/Si571
				Si571GetRate( pModule, &refRate ); // Считать Si570/Si571
			}
			break;
		case BRDclks_ADC_SUBGEN_PLL:
			if( pSrvCfg->nGenType == 0 )
			{
				//
				// Генератор на субмодуле непрограммируемый
				//
				dClkValue = pSrvCfg->nGenRef;
			}
			else
			{
				Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
			}
            IPC_delay(1000);
			err = Adf4350SetPllMode( pModule, &dClkValue, &refRate );
			break;
		case BRDclks_ADC_EXTCLK:
			refRate = pSrvCfg->nSubExtClk;
			break;
		case BRDclks_ADC_EXTCLK_PLL:
			dClkValue = pSrvCfg->nSubExtClk;
			err = Adf4350SetPllMode( pModule, &dClkValue, &refRate );
			break;
		default:
			refRate = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}
	if( err>=0 )
		pSrvCfg->dSamplingRate = refRate;

	return err;
}

//***************************************************************************************
int CFm212x1gSrv::GetRate( CModule *pModule, double &refRate, double ClkValue )
{
	FM212x1GSRV_Cfg		*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;
	ULONG				nClkSrc;
	double				dClkValue ;

	GetClkSource( pModule, nClkSrc );

	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			refRate = 0.0;
			break;
		case BRDclks_ADC_SUBGEN:
			Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
			refRate = dClkValue;
			break;
		case BRDclks_ADC_EXTCLK:
			refRate = pSrvCfg->nSubExtClk;
			break;
		default:
			refRate = pSrvCfg->dSamplingRate;
			break;
	}

	if( pSrvCfg->nDoubleFreq )
		refRate *= 2.0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	FM212x1GSRV_Cfg  *pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	double  dTmp;
    S32		code;


	if( Chan >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	if( refBias > 100.0 )  refBias = 100.0;
	if( refBias < -100.0 ) refBias = -100.0;
	dTmp  = refBias / 100.0;
	dTmp *= 32767.0;
    code  = (S32)floor(dTmp);
    SpdWrite( pModule, SPDdev_THDAC, Chan, (U32)code );

	pSrvCfg->aBias[Chan] = refBias;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::GetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	FM212x1GSRV_Cfg  *pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;

	if( Chan >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	refBias = pSrvCfg->aBias[Chan];

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetDcCoupling(CModule* pModule, ULONG InpType, ULONG Chan)
{
	U32			val, mask;

	if( Chan >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;
	mask = (Chan==0) ? 0x2 : 0x20;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_INP, &val );
	if( InpType )
		val |= mask;
	else
		val &= ~mask;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_INP, val );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::GetDcCoupling(CModule* pModule, ULONG& InpType, ULONG Chan)
{
	U32			val, mask;

	if( Chan >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;
	mask = (Chan==0) ? 0x2 : 0x20;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_INP, &val );
	InpType = (val&mask) ? 1 :0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan)
{
	if( Chan == 1024 )
	{
		//
		// Установить вх.сопротивление для сигнала внешнего старта: 0 - 2.5 кОм, 1 - 50 Ом
		//
		ADC_CTRL			regCtrl;

		IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
		if( InpRes )
			regCtrl.ByBits.stResist = 1;
		else
			regCtrl.ByBits.stResist = 0;
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

		return BRDerr_OK;
	}

	return BRDerr_BAD_PARAMETER;
}

//***************************************************************************************
int CFm212x1gSrv::GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan)
{
	if( Chan == 1024 )
	{
		//
		// Считать вх.сопротивление для сигнала внешнего старта: 0 - 2.5 кОм, 1 - 50 Ом
		//
		ADC_CTRL			regCtrl;

		IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
		InpRes = regCtrl.ByBits.stResist;

		return BRDerr_OK;
	}

	return BRDerr_BAD_PARAMETER;
}

//***************************************************************************************
int CFm212x1gSrv::SetStartMode( CModule *pModule, PVOID pStartStopMode )
{
	int Status = BRDerr_OK;
	PBRD_AdcStartMode pAdcStartMode = (PBRD_AdcStartMode)pStartStopMode;
	double	thr = pAdcStartMode->level;
	ULONG	inv = pAdcStartMode->inv;

	Status = SetAdcStartMode(pModule, pAdcStartMode->src, inv, &thr);
	if(Status != BRDerr_OK)
		return Status;
	pAdcStartMode->level = thr;

	PBRD_StartMode pStartMode = &pAdcStartMode->stndStart;
	Status = CAdcSrv::SetStartMode(pModule, pStartMode);

	ULONG mode;
	CAdcSrv::GetMaster(pModule, mode);
	if(mode)
	{
		// для вывода сигнала старта на разъем SYNX
		ADM2IF_MODE0 mode0;
		ADM2IF_STMODE stMode;

		IndRegRead( pModule, m_MainTetrNum, ADM2IFnr_MODE0, &mode0.AsWhole );
		mode0.ByBits.Start = 1;
		mode0.ByBits.AdmClk = 1;
		mode0.ByBits.Master = 1;
		IndRegWrite( pModule, m_MainTetrNum, ADM2IFnr_MODE0, mode0.AsWhole );

		IndRegRead( pModule, m_MainTetrNum, ADM2IFnr_STMODE, &stMode.AsWhole );
		stMode.ByBits.SrcStart = BRDsts_TRDADC;
		stMode.ByBits.TrigStart = 0;
		IndRegWrite( pModule, m_MainTetrNum, ADM2IFnr_MODE0, stMode.AsWhole );
	}
	return Status;
}

//***************************************************************************************
int CFm212x1gSrv::GetStartMode( CModule *pModule, PVOID pStartStopMode )
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
int CFm212x1gSrv::SetGain( CModule *pModule, double &refGain, ULONG chanNo )
{
	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	S32					codeAtt = 0;

	if( chanNo >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	codeAtt = (S32)(( -refGain + 0.25) * 2);
	if( codeAtt < 0 )
		codeAtt = 0;
	if( codeAtt > 63 )
		codeAtt = 63;
	refGain = -((double)codeAtt) / 2.0;

	if( chanNo == 0 )
		SpdWrite( pModule, SPDdev_ATT0, 0, codeAtt );
	else
		SpdWrite( pModule, SPDdev_ATT1, 0, codeAtt );
	pSrvCfg->adGainDb[chanNo] = refGain;
	
	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::GetGain( CModule *pModule, double &refGain, ULONG chanNo )
{
	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;

	if( chanNo >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;
	
	refGain = pSrvCfg->adGainDb[chanNo];

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetInpRange( CModule *pModule, double &refInpRange, ULONG chanNo )
{
	FM212x1GSRV_Cfg  *pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	int					status;
	double				gainDB, tmp;

	tmp  = (double)(pSrvCfg->AdcCfg.InpRange) / 1000.0;
	tmp /= refInpRange;
	gainDB  = log10( tmp );
	gainDB *= 20.0;

	status = SetGain( pModule, gainDB, chanNo );

	tmp  = pow(10.0, gainDB/20.0);
	refInpRange = (double)(pSrvCfg->AdcCfg.InpRange) / tmp / 1000.0;
	//refInpRange = (double)(pSrvCfg->AdcCfg.InpRange) / pow(10.0, gainDB/20.0) / 1000.0;

	return status;
}

//***************************************************************************************
int CFm212x1gSrv::GetInpRange( CModule *pModule, double &refInpRange, ULONG chanNo )
{
	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	double				gainDB;

	if( chanNo >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	gainDB = pSrvCfg->adGainDb[chanNo];
	refInpRange = (double)(pSrvCfg->AdcCfg.InpRange) /  pow(10.0, gainDB/20.0) / 1000.0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetCode( CModule *pModule, ULONG type )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::GetCode( CModule *pModule, ULONG &refType )
{
	refType = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::IsBitsOverflow(CModule* pModule, ULONG& OverBits)
{
	U32			status;

	DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
	OverBits = 0x3 & (status>>14);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SelfClbr( CModule *pModule )
{
	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	double		clkValue = 50000000.0;
	double		rate = 1800000000.0;

	RealDelay( 10000, 1 );
	SetClkSource( pModule, BRDclks_ADC_SUBGEN );
	SetClkValue( pModule, BRDclks_ADC_SUBGEN, clkValue );
	SetRate( pModule, rate, BRDclks_ADC_SUBGEN, clkValue );
	RealDelay( 10000, 1 );

	SpdWrite( pModule, SPDdev_ADC, 0x00, 0xA000 ); 
	RealDelay( 2, 1 );

	SpdWrite( pModule, SPDdev_ADC, 0x00, 0x2000 ); 
	RealDelay( 2, 1 );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetMaster( CModule *pModule, ULONG mode )
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

		IndRegRead( pModule, m_MainTetrNum, ADM2IFnr_MODE0, &mode0.AsWhole );
		mode0.ByBits.Start = 1;
		mode0.ByBits.AdmClk = 1;
		mode0.ByBits.Master = 1;
		IndRegWrite( pModule, m_MainTetrNum, ADM2IFnr_MODE0, mode0.AsWhole );
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	int				err;
//	int				val;

	if( pSrvCfg->nDoubleFreq )
		mask = 0x3;
	err = CAdcSrv::SetChanMask( pModule, mask);

	//
	// Включить/выключить питание АЦП
	//
	//val = 0x0000;
	//if( 0 == (mask & 0x1) )
	//	val |= 0x0400;
	//if( 0 == (mask & 0x2) )
	//	val |= 0x0800;

	//SpdWrite( pModule, SPDdev_ADC, 0x00, val );
	//RealDelay( 100, 1 );

	return err;
}

//***************************************************************************************
int CFm212x1gSrv::GetChanMask(CModule* pModule, ULONG& mask)
{
	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	int				err = BRDerr_OK;

	switch( pSrvCfg->nDoubleFreq )
	{
		case 1: mask = 0x1; break;
		case 2: mask = 0x2; break;
		case 0:	
		default:
			err = CAdcSrv::GetChanMask( pModule, mask); break;
	}

    return err;
}

//***************************************************************************************
int CFm212x1gSrv::TimeStamp(CModule* pModule, void *args)
{
	//FM212x1GSRV_Cfg		*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;
	U32	on = *(U32*)args;
	U32	val = 0;
	
	IndRegRead(pModule, m_AdcTetrNum, ADCnr_CTRL1, &val);
	if (on)
		val |= 0x8000;
	else
		val &= 0x7FFF;
	IndRegWrite(pModule, m_AdcTetrNum, ADCnr_CTRL1, val);

	return BRDerr_OK;
}

//***************************************************************************************
//int CFm212x1gSrv::PrepareStart(CModule *pModule, void *arg)
//{
//	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
//	U32						val;
//
//	if( pSrvCfg->nTsOn )
//	{
//		IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &val );
//		val |= 0x8000;
//		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, val );
//	}
//	return BRDerr_OK;
//}

//***************************************************************************************
int CFm212x1gSrv::ExtraInit(CModule* pModule)
{
	//
	// Начальная инициализация программируемых микросхем: АЦП, ИПН(ЦАП), генератора
	//
	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	int					ii;

	//pSrvCfg->nTsOn = 0;

	RealDelay(300,1);

	//
	// Инициализация АЦП
	// Низкое питание обоих каналов АЦП (рег. 0x00)
	//
	SetChanMask( pModule, 0x3 );
//	SpdWrite( pModule, SPDdev_ADC, 0x00, 0x0C00 ); 
	SpdWrite( pModule, SPDdev_ADC, 0x00, 0x2000 ); 
	SpdWrite( pModule, SPDdev_ADC, 0x01, 0x2A0E );
	SpdWrite( pModule, SPDdev_ADC, 0x02, 0x0000 );
	SpdWrite( pModule, SPDdev_ADC, 0x03, 0x4000 );
	SpdWrite( pModule, SPDdev_ADC, 0x04, 0xFE4B );
	SpdWrite( pModule, SPDdev_ADC, 0x05, 0x0000 );
	SpdWrite( pModule, SPDdev_ADC, 0x06, 0x1C20 );
	SpdWrite( pModule, SPDdev_ADC, 0x07, 0x4140 );
	SpdWrite( pModule, SPDdev_ADC, 0x08, 0x0000 );
	SpdWrite( pModule, SPDdev_ADC, 0x09, 0x0000 );
	SpdWrite( pModule, SPDdev_ADC, 0x0A, 0x0000 );
	SpdWrite( pModule, SPDdev_ADC, 0x0B, 0x4000 );
	SpdWrite( pModule, SPDdev_ADC, 0x0C, 0x0004 );
	SpdWrite( pModule, SPDdev_ADC, 0x0D, 0x0000 );
	SpdWrite( pModule, SPDdev_ADC, 0x0E, 0x0003 );
	SpdWrite( pModule, SPDdev_ADC, 0x0F, 0x0000 );

	//
	// Инициализация ИПН(ЦАП)
	//
	int			regAdr;

	SpdWrite( pModule, SPDdev_THDAC, 0x0, 0x0000 );
	SpdWrite( pModule, SPDdev_THDAC, 0x1, 0x0000 );
	SpdWrite( pModule, SPDdev_THDAC, 0x2, 0x0000 );
	SpdWrite( pModule, SPDdev_THDAC, 0x3, 0x0000 );
	pSrvCfg->aBias[0] = 0.0;
	pSrvCfg->aBias[1] = 0.0;
	pSrvCfg->dStCmpThr = 0.0;
	//pSrvCfg->dExtClkThr = 0.0;

	//
	// Инициализация Аттенюаторов
	//
	SpdWrite( pModule, SPDdev_ATT0, 0, 0x0 );
	SpdWrite( pModule, SPDdev_ATT1, 0, 0x0 );
	for( ii=0; ii<MAX_ADC_CHAN; ii++ )
		pSrvCfg->adGainDb[ii] = 0.0;

	//
	// Определение частоты кварца для генератора Si570/Si571
	//

	pSrvCfg->dGenFxtal = 0.0;
	if( (pSrvCfg->nGenType == 1) && (pSrvCfg->nGenRef != 0) )
	{
		U32			regData[20];
		//U32			rfreqLo, rfreqHi, hsdivCode, n1Code;
		//double		freqTmp;
		//double		dRFreq, dHsdiv, dN1;
		ULONG		clkSrc;

		//
		// Подать питание на Si570/Si571
		//
		GetClkSource( pModule, clkSrc );
		SetClkSource( pModule, 0 );
		SetClkSource( pModule, 1 );
		SpdWrite( pModule, SPDdev_GEN, 135, 0x80 );		// Reset
		RealDelay(100,1);
		
		//
		// Считать регистры Si570/Si571
		//
		//U32		vall;
		//for( ii=0; ii<256; ii++ )
		//{
		//	pSrvCfg->nGenAdr = ii;
		//	SpdRead( pModule, SPDdev_GEN, 9, &vall );
		//	if( (vall&0xFF) != 0xFF )
		//		printf( "adr=0x%X, vall=0x%X\n", ii, vall );
		//}
		//pSrvCfg->nGenAdr = 0x55;

		for( regAdr=7; regAdr<=12; regAdr++ )
			SpdRead( pModule, SPDdev_GEN, regAdr, &regData[regAdr] );
		SetClkSource( pModule, clkSrc );

		//
		// Рассчитать частоту кварца
		//
		SI571_CalcFxtal( &(pSrvCfg->dGenFxtal), (double)(pSrvCfg->nGenRef), regData );
		//printf( ">> XTAL = %f kHz\n", pSrvCfg->dGenFxtal/1000.0 );
		//printf( ">> GREF = %f kHz\n", ((double)(pSrvCfg->nGenRef))/1000.0 );
	}

	//
	// Режим удвоения частоты
	//
	pSrvCfg->nDoubleFreq = 0;
	pSrvCfg->dDoubleFreqBias1 = 0.0;
	pSrvCfg->dDoubleFreqRange1 = 0.0;

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm212x1gSrv::SpdRead(  CModule* pModule, U32 spdType, U32 regAdr, U32 *pRegVal )
{
	//
	// Типы SPD (spdType): 
	//   0x00 - АЦП0, 0x01 - ИПН(ЦАП), 0x02 - генератор 
	//   0x03 - синтезатор, 0x04 - атт0, 0x14 - атт1
	//   0x05 - датчик температуры
	//
	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	U32				spdCtrl;
	U32				status;

	spdCtrl  = 0x1;
	switch(spdType)
	{
		case SPDdev_GEN:	spdCtrl |= (pSrvCfg->nGenAdr & 0xFF) << 4;	break;
		case SPDdev_TEMP:	spdCtrl |= (pSrvCfg->nTempAdr & 0xFF) << 4;	break;
		default:			spdCtrl |= spdType & 0xF0;
	}

	for(;;)
	{
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DEVICE, spdType & 0xF );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_ADDR, regAdr );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_CTRL, spdCtrl );
	for(;;)
	{
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}
	IndRegRead( pModule, m_AdcTetrNum, ADCnr_SPD_DATA, pRegVal );
	if( spdType == SPDdev_SYNT )
	{
		U32			val;
		IndRegRead( pModule, m_AdcTetrNum, ADCnr_SPD_DATAH, &val );
		*pRegVal &= 0xFFFF;
		*pRegVal |= val<<16;
	}

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm212x1gSrv::SpdWrite( CModule* pModule, U32 spdType, U32 regAdr, U32 regVal )
{
	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	U32				spdCtrl;
	U32				status;

	spdCtrl  = 0x2;
	switch(spdType)
	{
		case SPDdev_GEN:	spdCtrl |= (pSrvCfg->nGenAdr & 0xFF) << 4;	break;
		case SPDdev_TEMP:	spdCtrl |= (pSrvCfg->nTempAdr & 0xFF) << 4;	break;
		default:			spdCtrl |= spdType & 0xF0;
	}

	for(;;)
	{
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DEVICE, spdType & 0xF );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_ADDR, regAdr );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DATA, regVal );
	if( spdType == SPDdev_SYNT )
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DATAH, regVal>>16 );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_CTRL, spdCtrl );
	for(;;)
	{
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm212x1gSrv::Si571SetRate( CModule* pModule, double *pRate )
{
	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	U32			regData[20];
//	U32			rfreqLo, rfreqHi, hsdivCode, n1Code;
	int			regAdr;
	int			status = BRDerr_OK;

	//
	// Скорректировать частоту, если необходимо
	//
	if( *pRate > (double)pSrvCfg->nGenRefMax )
	{
		*pRate = (double)pSrvCfg->nGenRefMax;
		status = BRDerr_WARN;
	}

	SI571_SetRate( pRate, pSrvCfg->dGenFxtal, regData );

	//
	// Запрограммировать генератор
	//
	SpdWrite( pModule, SPDdev_GEN, 137, 0x10 );		// Freeze DCO
	for( regAdr=7; regAdr<=18; regAdr++ )
		SpdWrite( pModule, SPDdev_GEN, regAdr, regData[regAdr] );		
	SpdWrite( pModule, SPDdev_GEN, 137, 0x0 );		// Unfreeze DCO
	SpdWrite( pModule, SPDdev_GEN, 135, 0x40 );		// Assert the NewFreq bit
	RealDelay( 300, 1 );

	return status;	
}

//***************************************************************************************
int	CFm212x1gSrv::Si571GetRate( CModule* pModule, double *pRate )
{
	FM212x1GSRV_Cfg	*pSrvCfg  = (FM212x1GSRV_Cfg*)m_pConfig;
	U32			regData[20];
	//U32			rfreqLo, rfreqHi, hsdivCode, n1Code;
	//double		freqTmp;
	//double		dRFreq, dHsdiv, dN1;
    //ULONG		clkSrc;
	int			regAdr;

	*pRate = 0.0;

	//
	// Проверить источник частоты
	//
    //GetClkSource( pModule, clkSrc );
    //if( clkSrc != BRDclks_ADC_SUBGEN )
    //	BRDerr_ERROR;

	//
	// Считать регистры Si570/Si571
	//
	for( regAdr=7; regAdr<=12; regAdr++ )
		SpdRead( pModule, SPDdev_GEN, regAdr, &regData[regAdr] );

	SI571_GetRate( pRate, pSrvCfg->dGenFxtal, regData );
	//printf( "Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n", regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] )

	return BRDerr_OK;	
}


//***************************************************************************************
int	CFm212x1gSrv::Adf4350SetPllMode( CModule* pModule, double *pFref, double *pRate )
{
//	FM212x1GSRV_Cfg *pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;
	ADF4350_TReg	regs[10];
	S32				realSize;
	int				ii;
	S32				err;

	err = ADF4350_PllMode( pFref, pRate, regs, 10, &realSize );
	if( 0 > err)
		return BRDerr_BAD_PARAMETER;

	//
	// Запрограммировать синтезатор 
	//
	for( ii=0; ii<realSize; ii++ )
		SpdWrite( pModule, SPDdev_SYNT, regs[ii].adr, regs[ii].val );

	RealDelay(200,1);

	return BRDerr_OK;	
}

//***************************************************************************************
int CFm212x1gSrv::SetAdcStartMode( CModule *pModule, ULONG source, ULONG inv, double *pCmpThr )
{
	FM212x1GSRV_Cfg *pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;

	//
	// Установить источник сигнала старта и инверсию сигнала старта
	//
	ADC_CTRL regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	regCtrl.ByBits.StartSrc = source;
	regCtrl.ByBits.StartInv = inv;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

	//
	// Дождаться готовности тетрвды
	//
	U32		regStatus;

	do 
		DirRegRead( pModule, m_MainTetrNum, ADM2IFnr_STATUS, &regStatus );
	while( 0 == (regStatus&0x1) );

	//
	// Установить опорное напряжение для стартовой синхронизации
	// Если источник вн. старта - внешний, то Ucmp = -2.5..+2.5 Вольт
	// Если источник вн. старта - каналы 0 или 1, то Ucmp = -2.5..+2.5 Вольт
	//
	S32		code;

    if( *pCmpThr > +100.0 ) { *pCmpThr = +100.0; }
    if( *pCmpThr < -100.0 ) { *pCmpThr = -100.0; }
    if( source == 0x2 ) {
        code = (S32)( 19660.0 * ((*pCmpThr) / 100.0) );	// внешний
    } else {
        code = (S32)( 16383.0 * ((*pCmpThr) / 100.0) );	// каналы 0 или 1
    }

    SpdWrite( pModule, SPDdev_THDAC, 2, (U32)code );
    SpdWrite( pModule, SPDdev_THDAC, 3, (U32)code );

	pSrvCfg->dStCmpThr = *pCmpThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::GetAdcStartMode( CModule *pModule, PULONG pSource, PULONG pInv, double *pCmpThr )
{
	FM212x1GSRV_Cfg	*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;
	ADC_CTRL regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	*pSource = regCtrl.ByBits.StartSrc;
	*pInv    = regCtrl.ByBits.StartInv;

	*pCmpThr = pSrvCfg->dStCmpThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetSpecific( CModule *pModule, BRD_AdcSpec *pSpec )
{
	int status;

	status = CAdcSrv::SetSpecific(pModule, pSpec);

	switch(pSpec->command)
	{
		case FM212x1Gcmd_SETMU:
			status = SetMu(pModule, pSpec->arg);
			break;
		case FM212x1Gcmd_GETMU:
			status = GetMu(pModule, pSpec->arg);
			break;
		case FM212x1Gcmd_GETEVENTCNT:
			U32		val, counterLo, counterHi;

			IndRegRead( pModule, m_AdcTetrNum, ADCnr_EVENTCNTL, &counterLo );
			IndRegRead( pModule, m_AdcTetrNum, ADCnr_EVENTCNTH, &val );
			IndRegRead( pModule, m_AdcTetrNum, ADCnr_PREVPREC,  &counterHi );
			counterLo &= 0xFFFF;
			counterLo |= (val & 0xffff) << 16;
			counterHi &= 0xFFFF;
			counterHi >>= 8;
			*(__int64*)pSpec->arg = (__int64)counterLo | ((__int64)counterHi << 32);

			break;
		case FM212x1Gcmd_RESETEVENTCNT:
			IndRegWrite( pModule, m_AdcTetrNum, ADCnr_EVENTCNTL, (ULONG)0 );
			break;
		case FM212x1Gcmd_SETCDBLCLKEX:
			status = SetDblClkEx(pModule, pSpec->arg);
			break;
		case FM212x1Gcmd_GETCDBLCLKEX:
			status = GetDblClkEx(pModule, pSpec->arg);
			break;
		case FM212X1Gcmd_TIMESTAMP:
			status = TimeStamp(pModule, pSpec->arg);
			break;
		case FM212x1Gcmd_SPDREAD:
		{
			FM212x1GSRV_SPDREAD		*pSpd = (FM212x1GSRV_SPDREAD*)pSpec->arg;
			status = SpdRead( pModule, pSpd->spdType, pSpd->regAdr, &(pSpd->regVal) );
			break;
		}
		case FM212x1Gcmd_SPDWRITE:
		{
			FM212x1GSRV_SPDWRITE	*pSpd = (FM212x1GSRV_SPDWRITE*)pSpec->arg;
			status = SpdWrite( pModule, pSpd->spdType, pSpd->regAdr, pSpd->regVal );
			break;
		}
	}
	return status;
}

//***************************************************************************************
int CFm212x1gSrv::SetMu( CModule *pModule, void *args )
{
	FM212x1GSRV_MU *pMU = (FM212x1GSRV_MU*)args;
	int				ii;

	//
	// Проверить тип субмодуля в параметрах
	//

	SetMaster( pModule, pMU->master );
	CtrlFormat( pModule, NULL, BRDctrl_ADC_SETFORMAT, &pMU->format );
	SetClkSource( pModule, pMU->clockSrc );
	SetClkValue( pModule, pMU->clockSrc, pMU->clockValue );
	SetRate( pModule, pMU->samplingRate, pMU->clockSrc, pMU->clockValue );
	SetChanMask( pModule, pMU->chanMask );
	for( ii = 0; ii < MAX_ADC_CHAN; ii++ )
	{
		SetGain(pModule, pMU->gaindb[ii], ii);
		SetBias(pModule, pMU->bias[ii], ii);
		SetDcCoupling(pModule, pMU->dcCoupling[ii], ii);
		SetInpResist(pModule, pMU->inpResist[ii], ii);
	}
	SetStartMode( pModule, &(pMU->adcStMode) );
	SetInpResist( pModule, pMU->stResist, 1024 );
	SetDblClkEx( pModule, &(pMU->dblClk) );

	//
	// Установить режим претриггера
	//
	ULONG sample_size = pMU->format ? pMU->format : 2;
	int chans = 0;
	for( ii = 0; ii < 16; ii++)
		chans += (pMU->chanMask >> ii) & 0x1;
	BRD_PretrigMode pretrigger;
	pretrigger.enable = 0;
	pretrigger.assur = 0;
	pretrigger.external = 0;
	if( pMU->pretrigMode )
	{
		pretrigger.enable = 1;
		pretrigger.assur = (pMU->pretrigMode == 2) ? 1 : 0;
		pretrigger.external = (pMU->pretrigMode == 3) ? 1 : 0;
	}
	pretrigger.size = ULONG(( pMU->pretrigSamples * chans * sample_size) / sizeof(ULONG));
	SetPretrigMode( pModule, &pretrigger);


	BRD_EnVal	rCnt;

	rCnt.enable = pMU->cnt0Enable;
	rCnt.value  = pMU->cnt0Value;
	SetCnt( pModule, 0, &rCnt);

	rCnt.enable = pMU->cnt1Enable;
	rCnt.value  = pMU->cnt1Value;
	SetCnt( pModule, 1, &rCnt);

	rCnt.enable = pMU->cnt2Enable;
	rCnt.value  = pMU->cnt2Value;
	SetCnt( pModule, 2, &rCnt);


	BRD_EnVal	rTitle;

	rTitle.enable = pMU->titleEnable;
	rTitle.value  = pMU->titleSize;
	SetTitleMode( pModule, &rTitle );
	SetTitleData( pModule, (ULONG*)&(pMU->titleData) );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::GetMu( CModule *pModule, void *args )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::SetDblClkEx(CModule* pModule, void *args)
{
	FM212x1GSRV_Cfg		*pSrvCfg = (FM212x1GSRV_Cfg*)m_pConfig;
	FM212x1GSRV_DBLCLK	*pDblClk = (FM212x1GSRV_DBLCLK*)args;
	U32					ctrlReg;
	U32					chanMask;
	U32					adcReg0, adcRegE;
	S32					code;

	if( pSrvCfg->isBw2500M )
		printf( "> > > Mode /2500M ON\n" );
	else
		printf( "> > > Mode /2500M OFF\n" );

	if( 0x1 & (pDblClk->isDblClk) == 0 )
	{	
		if( pSrvCfg->nDoubleFreq != 0 )
		{
			chanMask = (pSrvCfg->nDoubleFreq==1) ? 0x1 : 0x2;
			SetChanMask( pModule, chanMask );
			SpdWrite( pModule, SPDdev_ADC, 0x00, 0x2000 ); 
			if( pSrvCfg->isBw2500M )
				SpdWrite( pModule, SPDdev_ADC, 0x0E, 0x0003 );	// 29.06.2016 
		}
		pSrvCfg->nDoubleFreq = 0;
	}
	else
	{
		if( pSrvCfg->isBw2500M )
			adcReg0 = (pDblClk->inpSrc==0) ? 0x20A0 : 0x2000;	// 29.06.2016 
		else
			adcReg0 = (pDblClk->inpSrc==0) ? 0x20C0 : 0x2080;
		SetChanMask( pModule, 0x3 );
		SpdWrite( pModule, SPDdev_ADC, 0x00, adcReg0 ); 
		if( pSrvCfg->isBw2500M )
		{
			adcRegE = (pDblClk->inpSrc==0) ? 0x0003 : 0x0043;	// 29.06.2016 
			SpdWrite( pModule, SPDdev_ADC, 0x0E, adcRegE ); 	// 29.06.2016 
		}

		pSrvCfg->nDoubleFreq = (pDblClk->inpSrc==0) ? 1 : 2;
	}

	//
	// Сообщить прошивке, какой выбран режим
	//
	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &ctrlReg );
	ctrlReg &= ~0x4000;
	if( 0x1 == pDblClk->isDblClk )
		ctrlReg |= 0x4000;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, ctrlReg );

	//
	// Компенсация ШП
	//
	code  = (S32)floor(0x3FFF * pDblClk->inp0Range / 24.05 );
	code += 0x4000;
	if( code > 0x7FFF )
		code = 0x7FFF;
	if( code < 0 )
		code = 0;
	SpdWrite( pModule, SPDdev_ADC, 0x0B, code ); 
//printf( "\n....==== range0 = %7.3f, regB = 0x%X\n", pDblClk->inp0Range, code );
	code  = (S32)floor(0x3FFF * pDblClk->inp1Range / 24.05 );
	code += 0x4000;
	if( code > 0x7FFF )
		code = 0x7FFF;
	if( code < 0 )
		code = 0;
	SpdWrite( pModule, SPDdev_ADC, 0x03, code ); 
//printf( "....==== range1 = %7.3f, reg3 = 0x%X\n", pDblClk->inp1Range, code );

	//
	// Компенсация смещения нуля
	//
	code = (S32)floor( 0xFFF * fabs( pDblClk->inp0Bias ) / 5.696 );
	if( code > 0xFFF )
		code = 0xFFF;
	if( 0 > pDblClk->inp0Bias )
		code |= 0x1000;
	SpdWrite( pModule, SPDdev_ADC, 0x0A, code ); 
//printf( "....==== bias0 = %7.3f, regA = 0x%X\n", pDblClk->inp0Bias, code );

	code = (S32)floor( 0xFFF * fabs( pDblClk->inp1Bias ) / 5.696 );
	if( code > 0xFFF )
		code = 0xFFF;
	if( 0 > pDblClk->inp1Bias )
		code |= 0x1000;
	SpdWrite( pModule, SPDdev_ADC, 0x02, code ); 
//printf( "....==== bias1 = %7.3f, reg2 = 0x%X\n", pDblClk->inp1Bias, code );



//printf( "\n....oooo DoubleFreq. Mode=%d, Src=%d\n", pDblClk->isDblClk, pDblClk->inpSrc );
//printf( "....oooo DoubleFreq. Result=%d\n\n", pSrvCfg->nDoubleFreq  );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm212x1gSrv::GetDblClkEx(CModule* pModule, void *args)
{
//	FM212x1GSRV_DBLCLK *pDblClk = (FM212x1GSRV_DBLCLK*)args;

	return BRDerr_OK;
}


// ***************** End of file Fm212x1gSrv.cpp ***********************
