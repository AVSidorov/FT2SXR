/*
 ***************** File Adc212x1gSrv.cpp ************
 *
 * BRD Driver for ADС service on ADM212x1G
 *
 * (C) InSys by Ekkore Okt 2010
 *
 ******************************************************
*/

#include "gipcy.h"

#include "module.h"
#include "realdelay.h"
#include "adc212x1gsrv.h"

#define	CURRFILE "ADC212X1GSRV"

const double BRD_ADC_MAXGAINTUN	= 1.; // not used

//***************************************************************************************
CAdc212x1gSrv::CAdc212x1gSrv(int idx, int srv_num, CModule* pModule, PADC212X1GSRV_Cfg pCfg) :
	CAdcSrv(idx, _BRDC("ADC212X1G"), srv_num, pModule, pCfg, sizeof(ADC212X1GSRV_Cfg))
{
}

//***************************************************************************************
int CAdc212x1gSrv::CtrlRelease( void *pDev, void *pServData, ULONG cmd, void *args )
{
	CModule* pModule = (CModule*)pDev;

	CAdcSrv::SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);

	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб (вместо BRDerr_OK)
}

//***************************************************************************************
void CAdc212x1gSrv::GetAdcTetrNum( CModule *pModule )
{
	//
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	//

	//
	// Поиск номера терады. Если тетрада не найдена, то -1.
	//
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC212X1G_TETR_ID);
}

//***************************************************************************************
void CAdc212x1gSrv::FreeInfoForDialog( PVOID pInfo )
{
	ADC212X1GSRV_Info *pSrvInfo = (ADC212X1GSRV_Info*)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc212x1gSrv::GetInfoForDialog( CModule *pModule )
{
	ADC212X1GSRV_Cfg  *pSrvCfg  = (ADC212X1GSRV_Cfg*)m_pConfig;
	ADC212X1GSRV_Info *pInfo = new ADC212X1GSRV_Info;
	int					ii;

	pInfo->Size = sizeof(ADC212X1GSRV_Info);

	UCHAR code = pSrvCfg->AdcCfg.Encoding;
	
	pInfo->pAdcInfo = (ADCSRV_INFO*)CAdcSrv::GetInfoForDialog(pModule);
	if(pInfo->pAdcInfo->SyncMode == 3)
		pInfo->pAdcInfo->SyncMode = 1;
	pInfo->pAdcInfo->Encoding = code;
	pInfo->ExtClk = pSrvCfg->SubExtClk;
	pInfo->nSlclkinClk = ROUND(pSrvCfg->dSlclkinClk);
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
//		GetDcCoupling( pModule, (ULONG&)pInfo->pAdcInfo->DcCoupling[ii], ii );
//		GetClkPhase(pDev, pSrvInfo->PhaseTuning[i], i);
		
		GetGainTuning( pModule, pInfo->aGainTun[ii], ii);
	}

//	GetStartClkSl(pModule, &(pSrvInfo->nStartClkSl) );

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

	pInfo->nRefGen0         = pSrvCfg->nRefGen0;             // опорный генератор 0 (Гц)
	pInfo->nRefGen1         = pSrvCfg->nRefGen1;             // опорный генератор 1 (Гц)
	pInfo->nRefGen1Min      = 0;                             // Si570/Si571 (Гц)
	pInfo->nRefGen1Max      = pSrvCfg->nRefGen0Max;          // Si570/Si571 (Гц)
	pInfo->nRefGen1Type     = pSrvCfg->nRefGen0Type;         // тип: непрогр. / Si570/Si571
	pInfo->nRefGen2         = 0;                             // опорный генератор 2 (Гц)
	pInfo->nRefGen2Type     = pSrvCfg->nRefGen2Type;         // тип опорного генератора 2
	pInfo->nLpfPassBand     = pSrvCfg->nLpfPassBand;         // частота среза ФНЧ (Гц)
	pInfo->Version          = pSrvCfg->nVersion;             // версия субмодуля
	pInfo->adIcrRange[0] = pSrvCfg->adIcrRange[0];
	pInfo->adIcrRange[1] = pSrvCfg->adIcrRange[1];
	pInfo->adIcrRange[2] = pSrvCfg->adIcrRange[2];
	pInfo->adIcrRange[3] = pSrvCfg->adIcrRange[3];

	GetInpSrc(pModule, &(pInfo->nInputSource) );

	//
	// Режимы работы кристалла АЦП
	//
	pInfo->nDoubleFreq = pSrvCfg->nDoubleFreq;

	//char		msg[256];
	//sprintf(msg, _BRDC("GetInfoForDialog: clkSrc = %d"), pInfo->pAdcInfo->ClockSrc);
	//MessageBox(NULL, msg, "Debug", MB_OK);

	return pInfo;
}

//***************************************************************************************
int CAdc212x1gSrv::SetPropertyFromDialog( void *pDev, void *args )
{
	CModule				*pModule = (CModule*)pDev;
	ADC212X1GSRV_Info	*pInfo = (ADC212X1GSRV_Info*)args;
	ADC212X1GSRV_Cfg	*pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
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

	pSrvCfg->SubExtClk = ROUND(pInfo->ExtClk);
	pSrvCfg->dSlclkinClk = pInfo->nSlclkinClk;

	//char		msg[256];
	//sprintf(msg, _BRDC("SetPropertyFromDialog: clkSrc = %d"), pInfo->pAdcInfo->ClockSrc);
	//MessageBox(NULL, msg, "Debug", MB_OK);
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
		SetDcCoupling(pModule, pInfo->pAdcInfo->DcCoupling[ii], ii);
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

	//
	// Режимы работы кристалла АЦП
	//
	pSrvCfg->nDoubleFreq = pInfo->nDoubleFreq;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	ADC212X1GSRV_Cfg	*pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
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
	//char		msg[256];
	//sprintf(msg, _BRDC("SetProperties: clkSrc = %d"), clkSrc);
	//MessageBox(NULL, msg, "Debug", MB_OK);
	SetClkSource(pModule, clkSrc);

	//
	// Извлечение требуемого режима синхронизации MASTER/SLAVE
	//
	U32		isSlave;

    IPC_getPrivateProfileString(SectionName, _BRDC("StartSlave"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	isSlave = BRDC_atoi(Buffer);
	SetStartSlave(pModule, isSlave);

    IPC_getPrivateProfileString(SectionName, _BRDC("ClockSlave"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	isSlave = BRDC_atoi(Buffer);
	SetClockSlave(pModule, isSlave);

	//
	// Извлечение частоты внешнего генератора
	//
	double				clkValue;

    IPC_getPrivateProfileString(SectionName, _BRDC("ExternalClockValue"), _BRDC("120000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	clkValue = BRDC_atof(Buffer);
	pSrvCfg->SubExtClk = ROUND(clkValue);
	SetClkValue(pModule, BRDclks_ADC_EXTCLK, clkValue);

    IPC_getPrivateProfileString(SectionName, _BRDC("SlclkinClockValue"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	pSrvCfg->dSlclkinClk = BRDC_atof(Buffer);

	//
	// Извлечение требуемой частоты дискретизации
	//
	double			rate;
	
    IPC_getPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("100000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	rate = BRDC_atof(Buffer);
	SetRate(pModule, rate, clkSrc, clkValue);

	//
	// Извлечение требуемой инверсии тактовой частоты
	//
	U32				clkInv;
	
    IPC_getPrivateProfileString(SectionName, _BRDC("ClockInv"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	clkInv = BRDC_atoi(Buffer);
	SetClkInv(pModule, clkInv);

	//
	// Выключить, а затем включить микросхему АЦП
	//
	ULONG			chanMask;
		
    IPC_getPrivateProfileString(SectionName, _BRDC("ChannelMask"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
	chanMask = BRDC_strtol(Buffer, &endptr, 0);
	SetChanMask(pModule, 0);
	RealDelay( 1000, 0 );
	SetChanMask(pModule, chanMask);

	//
	// Извлечение для каждого канала требуемых:
	// 1. шкалы преобразования
	// 2. смещения нуля
	// 3. Подстройки шкалы преобразования
	//
	double			dVal;
	S32				nVal;
	DWORD			ret;

	for( ii = 0; ii < MAX_CHAN; ii++ )
	{
		//
		// ADM212x1GWB
		//
		if( pSrvCfg->nSubunitId == 0x271 )
		{
			BRDC_sprintf(ParamName, _BRDC("GainDb%d"), ii);
			ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
			if( Buffer[0]=='\0' )
			{
				BRDC_sprintf(ParamName, _BRDC("Gain%d"), ii);
				ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
			}
			if( Buffer[0] )
			{
				dVal = BRDC_atof(Buffer);
				SetGain(pModule, dVal, ii);
			}
		}

		//
		// ADM212x1G
		//
		else
		{
			BRDC_sprintf(ParamName, _BRDC("InputRange%d"), ii);
			ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
			if( Buffer[0] )
			{
				dVal = BRDC_atof(Buffer);
				SetInpRange(pModule, dVal, ii);
			}


			BRDC_sprintf(ParamName, _BRDC("DcCoupling%d"), ii);
			ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
			if( Buffer[0] )
			{
				nVal = BRDC_atoi(Buffer);
				SetDcCoupling( pModule, nVal, ii );
			}

			BRDC_sprintf(ParamName, _BRDC("Lpf%d"), ii);
			ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
			if( Buffer[0] )
			{
				nVal = BRDC_atoi(Buffer);
				SetInpFilter( pModule, nVal, ii );			
			}
		}

		BRDC_sprintf(ParamName, _BRDC("Bias%d"), ii);
		ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		if( Buffer[0] )
		{
			dVal = BRDC_atof(Buffer);
			SetBias(pModule, dVal, ii);
		}
	}

	//
	// Set PLL Mode
	//
	BRD_PllMode pllMode;

	IPC_getPrivateProfileString(SectionName, _BRDC("PllMuxOut"), _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
	if( Buffer[0] != '\0' )
	{
		pllMode.MuxOut = BRDC_atoi(Buffer);
		ADF4106_SetPllMode( pModule, &pllMode);
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
	// Режимы работы кристалла АЦП
	//
    IPC_getPrivateProfileString(SectionName, _BRDC("DoublrFreq"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	pSrvCfg->nDoubleFreq = BRDC_atoi(Buffer);

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

	//
	// Выполнить запись в регистры кристалла АЦП напрямую
	//
	U32			val;
	int			maxii = 20;
	S32			indRegNo;

	for( ii = 0; ii < 16; ii++ )
	{
		BRDC_sprintf(ParamName, _BRDC("AdcReg%d"), ii);
        ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
		if( 0 == ret )
			continue;

		val = BRDC_strtol( Buffer, &endptr, 0 );
		if( val == 0 )
			continue;

		indRegNo = ii;
		IndRegWrite( pModule, m_AdcTetrNum, indRegNo, val & 0xFFFF );
	}

	//
	// Выполнить запись в регистры генератора через I2C
	//
	U32			genAdr[] = { 7, 8, 9, 10, 11, 12, 135, 137 };
	U32			status, tmp;
	U32			genChipAdr = 0x55;		// Код адреса кристалла генератора

    ret = IPC_getPrivateProfileString(SectionName, _BRDC("GenChipAdr"), _BRDC("0x55"), Buffer, sizeof(Buffer), iniFilePath);
	if( 0 != ret )
		genChipAdr = BRDC_strtol( Buffer, &endptr, 0 );

	for( ii = 0; ii < sizeof(genAdr)/sizeof(genAdr[0]); ii++ )
	{
		//
		// Считать значение для записи в регистр генератора из ини-файла
		//
		BRDC_sprintf( ParamName, _BRDC("GenReg%d"), genAdr[ii] );
        ret = IPC_getPrivateProfileString( SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
		if( 0 == ret )
			continue;

		val = BRDC_strtol( Buffer, &endptr, 0 );
		if( val == 0 )
			continue;
		
		//
		// Выполнить запись а регистр генератора
		//
		do     DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		while( 0 == (status&0x1) );

		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_ADDR,  genAdr[ii] );
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_DATAL, val );
//		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_DATAH, (U32)0 );
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_CTRL,  (U32)0x1 | (genChipAdr<<8) );

		do     DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		while( 0 == (status&0x1) );

		//
		// Выполнить чтение из регистра генератора
		//
		do     DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		while( 0 == (status&0x1) );

		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_ADDR,  genAdr[ii] );
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_CTRL,  (U32)0x2 | (genChipAdr<<8) ); 

		do     DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		while( 0 == (status&0x1) );

		IndRegRead( pModule, m_AdcTetrNum, ADCnr_I2C_DATAL, &tmp );

		if( tmp != val )
		{
			BRDCHAR		lin[256];

			BRDC_sprintf( lin, _BRDC("В регистр Генератора #%d записали 0x%X, а прочитали 0x%X"), genAdr[ii], val, tmp );
		}
	}

	//
	// Выполнить запись в любые косвенные регистры тетрады напрямую
	//
	maxii = 20;

	for( ii = 0; ii < maxii; ii++ )
	{
		BRDC_sprintf(ParamName, _BRDC("IndReg%d"), ii);
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
int CAdc212x1gSrv::SaveProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	BRDCHAR				ParamName[128];
	ADC212X1GSRV_Cfg	*pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;

	CAdcSrv::SaveProperties(pModule, iniFilePath, SectionName);

	WriteInifileParam(iniFilePath, SectionName, _BRDC("CodeType"), (ULONG)pSrvCfg->AdcCfg.Encoding, 10, NULL);

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
	
	WriteInifileParam(iniFilePath, SectionName, _BRDC("SlclkinClockValue"), pSrvCfg->dSlclkinClk, 2, NULL);

	double rate;
	GetRate(pModule, rate, clkValue);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("SamplingRate"), rate, 2, NULL);

	ULONG clkInv;
	GetClkInv(pModule, clkInv);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ClockInv"), clkInv, 10, NULL);

	double	InpRange[MAX_CHAN];
	double	Bias[MAX_CHAN];
	double	GainTuning[MAX_CHAN];

	for(int i = 0; i < MAX_CHAN; i++)
	{
		GetInpRange(pModule, InpRange[i], i);
		BRDC_sprintf(ParamName, _BRDC("InputRange%d"), i);
		WriteInifileParam(iniFilePath, SectionName, ParamName, InpRange[i], 2, NULL);

		GetBias(pModule, Bias[i], i);
		BRDC_sprintf(ParamName, _BRDC("Bias%d"), i);
		WriteInifileParam(iniFilePath, SectionName, ParamName, Bias[i], 4, NULL);

		GetGainTuning(pModule, GainTuning[i], i);
		BRDC_sprintf(ParamName, _BRDC("GainTuning%d"), i);
		WriteInifileParam(iniFilePath, SectionName, ParamName, GainTuning[i], 4, NULL);
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

	U32		isSlave;

	GetStartSlave(pModule, (ULONG&)isSlave);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartSlave"), (ULONG)isSlave, 10, NULL);

	GetClockSlave(pModule, (ULONG&)isSlave);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("ClockSlave"), (ULONG)isSlave, 10, NULL);

	//
	// Режимы работы кристалла АЦП
	//
	WriteInifileParam(iniFilePath, SectionName, _BRDC("DoublrFreq"), (ULONG)pSrvCfg->nDoubleFreq, 10, NULL);

	// the function flushes the cache
    IPC_writePrivateProfileString(NULL, NULL, NULL, iniFilePath);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::GetCfg(PBRD_AdcCfg pCfg)
{
	ADC212X1GSRV_Cfg	*pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;

	CAdcSrv::GetCfg(pCfg);
	if( pSrvCfg->nSubunitId == 0x270 )	// Тип убмодуля - ADM212x1G
		pCfg->ChanType = 0;				// Коэф. передачи измеряется в разах
	else								// Тип субмодуля - ADM212x1GWB
		pCfg->ChanType = 1;				// Коэф. передачи измеряется в дБ

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetClkSource( CModule *pModule, ULONG nClkSrc )
{
	ADM2IF_FSRC		fsrc;

	//
	// Тактовая частота всегда выбирается только на субмодуле
	//
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &fsrc.AsWhole );
	fsrc.ByBits.Gen = nClkSrc;
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, fsrc.AsWhole );

	//
	// Повторная запись, после того, как отработается выключение питания
	//
	//Sleep(100);
	RealDelay(100,1);
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, fsrc.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::GetClkSource( CModule *pModule, ULONG &refClkSrc )
{
	ADM2IF_FSRC		fsrc;

	//
	// Определить режим работы тетрады: Single или Slave
	//
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &fsrc.AsWhole );
	refClkSrc = fsrc.ByBits.Gen;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetClkInv( CModule *pModule, ULONG nInv )
{
	ADM2IF_FSRC		fsrc;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &fsrc.AsWhole );
	fsrc.ByBits.Inv = nInv;
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, fsrc.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::GetClkInv( CModule *pModule, ULONG &refInv )
{
	ADM2IF_FSRC		fsrc;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &fsrc.AsWhole );
	refInv = fsrc.ByBits.Inv;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetClkValue( CModule *pModule, ULONG ClkSrc, double &refClkValue )
{
	ADC212X1GSRV_Cfg	*pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;

	switch(ClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			U32		nStartClkSl;

			GetStartClkSl(pModule, &nStartClkSl);
			if( nStartClkSl == 0 )
				refClkValue = 0.0;
			break;
		case BRDclks_ADC_SUBGEN0:
			if( pSrvCfg->nRefGen0Type == 0 )
			{
				//
				// Генератор на субмодуле непрограммируемый
				//
				refClkValue = pSrvCfg->nRefGen0;
			}
			else
			{
				double		dClkValue;

				Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
				if( dClkValue != refClkValue )
				{
					dClkValue = refClkValue;
					err = Si571SetRate( pModule, &dClkValue ); // Запрограммировать Si570/Si571
					Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
				}
				refClkValue = dClkValue;
			}
			break;
		case BRDclks_ADC_SUBGEN1:
			refClkValue = pSrvCfg->nRefGen1;
			break;
		case BRDclks_ADC_SUBSYNT:
		case BRDclks_ADC_EXTCLK:		// External clock
			pSrvCfg->SubExtClk = ROUND(refClkValue);
			break;
		default:
			refClkValue = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}
	return err;
}
	
//***************************************************************************************
int CAdc212x1gSrv::GetClkValue( CModule *pModule, ULONG ClkSrc, double &refClkValue )
{
	ADC212X1GSRV_Cfg *pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;

	switch(ClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			U32		nStartClkSl;

			GetStartClkSl(pModule, &nStartClkSl);
			if( nStartClkSl )
				refClkValue = pSrvCfg->SubExtClk;
			break;
		case BRDclks_ADC_SUBGEN0:
			if( pSrvCfg->nRefGen0Type == 0 )
			{
				//
				// Генератор на субмодуле непрограммируемый
				//
				refClkValue = pSrvCfg->nRefGen0;
			}
			else
			{
				double		dClkValue ;

				Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
				refClkValue = dClkValue;
			}
			break;
		case BRDclks_ADC_SUBGEN1:
			refClkValue = pSrvCfg->nRefGen1;
			break;
		case BRDclks_ADC_SUBSYNT:
		case BRDclks_ADC_EXTCLK:		// External clock
			refClkValue = pSrvCfg->SubExtClk;
			break;
		default:
			refClkValue = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}

	return BRDerr_OK;
}
	
//***************************************************************************************
int CAdc212x1gSrv::SetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;
	CAdcSrv::SetClkLocation(pModule, refMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::GetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;

	return BRDerr_OK;
}

//****************************************************************************************
static double g_dDeltaFreq;
int g_anM212X1G_CLK_DIVIDER[BRD_CLKDIVCNT] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 12, 14, 15, 16};

//****************************************************************************************
int SetClkDivOpt( double RateHz, double curClk, double minRate )
{
	int MaxDivider;
	int i = 0;

	if( 0.0 == curClk )
		return 1;
	if( minRate < 1000000.0 )
		minRate = 1000000.0;
	MaxDivider = (int)floor(curClk / minRate);

	for(i = BRD_CLKDIVCNT-1; i >= 0; i--)
		if(MaxDivider >= g_anM212X1G_CLK_DIVIDER[i])
			break;
	
	int MaxInd = (i < 0) ? 0 : i;

	MaxDivider = g_anM212X1G_CLK_DIVIDER[MaxInd];
	if(RateHz >= curClk)
	{
		if(g_dDeltaFreq > RateHz - curClk)
		{
			g_dDeltaFreq = RateHz - curClk;
			return 1;
		}
	}
	else
	{
		double minRateHz = curClk / MaxDivider;

		if(RateHz < minRateHz)
		{
			if(g_dDeltaFreq > minRateHz - RateHz)
			{
				g_dDeltaFreq = minRateHz - RateHz;
				return g_anM212X1G_CLK_DIVIDER[MaxInd];
			}
		}
		else 
		{
			for(int i = 0; i < MaxInd; i++) 
			{
				double RateLeft = curClk / g_anM212X1G_CLK_DIVIDER[i];
				double RateRight = curClk / g_anM212X1G_CLK_DIVIDER[i+1];

				if(RateHz <= RateLeft && RateHz >= RateRight)
				{
					if(RateLeft - RateHz <= RateHz - RateRight)
					{
						if (g_dDeltaFreq > RateLeft - RateHz)
						{
							g_dDeltaFreq = RateLeft - RateHz;
							return g_anM212X1G_CLK_DIVIDER[i];
						}
					}
					else
					{
						if (g_dDeltaFreq > RateHz - RateRight)
						{
							g_dDeltaFreq = RateHz - RateRight;
							return g_anM212X1G_CLK_DIVIDER[i+1];
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
int CAdc212x1gSrv::SetRate( CModule *pModule, double &refRate, ULONG nClkSrc, double dClkValue )
{
	ADC212X1GSRV_Cfg	*pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
	U32					nAdcRateDivider;
	int					err = BRDerr_OK;

	if( refRate > pSrvCfg->AdcCfg.MaxRate )
		refRate = pSrvCfg->AdcCfg.MaxRate;
	if( refRate < pSrvCfg->AdcCfg.MinRate )
		refRate = pSrvCfg->AdcCfg.MinRate;

	if( (nClkSrc == BRDclks_ADC_SUBGEN0) ||
		(nClkSrc == BRDclks_ADC_SUBSYNT) )
	{
		if( pSrvCfg->nRefGen0Type == 0 )
		{
			//
			// Генератор на субмодуле непрограммируемый
			//
			dClkValue = pSrvCfg->nRefGen0;
		}
		else
		{
			double		dClkValueAux;

			Si571GetRate( pModule, &dClkValueAux ); // Считать Si570/Si571
			if( dClkValueAux != refRate )
			{
				dClkValueAux = refRate;
				err = Si571SetRate( pModule, &dClkValueAux ); // Запрограммировать Si570/Si571
				Si571GetRate( pModule, &dClkValueAux ); // Считать Si570/Si571
				if( nClkSrc == BRDclks_ADC_SUBGEN0 )
					refRate = dClkValueAux;

			}

			//
			// Если выбран ФАПЧ, то запрограммировать его
			//
			if( nClkSrc == BRDclks_ADC_SUBSYNT )
				ADF4106_OnPll( pModule, refRate, pSrvCfg->SubExtClk );

			//
			// Запись значение 1 делителя частоты в регистр FDVR
			//
			nAdcRateDivider = 1;
			IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FDVR, nAdcRateDivider );

			return BRDerr_OK;
		}
	}
	if( nClkSrc == BRDclks_ADC_SUBGEN1 )
		dClkValue = pSrvCfg->nRefGen1;
	if( nClkSrc == BRDclks_ADC_EXTCLK )
		dClkValue = pSrvCfg->SubExtClk;

	//
	// Вычислить значение делителя частоты для записи в регистр FDVR
	//
	g_dDeltaFreq = 1.e10;
	if( refRate > pSrvCfg->AdcCfg.MaxRate)
		refRate = pSrvCfg->AdcCfg.MaxRate;
	nAdcRateDivider = SetClkDivOpt( refRate, dClkValue, pSrvCfg->AdcCfg.MinRate);

	//
	// Запись значение делителя частоты в регистр FDVR
	//
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FDVR, nAdcRateDivider );
	refRate = dClkValue / nAdcRateDivider;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::GetRate( CModule *pModule, double &refRate, double ClkValue )
{
	ADC212X1GSRV_Cfg	*pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
	ULONG				nClkSrc;
	double				dClkValue ;
	U32					nAdcRateDivider;

	GetClkSource( pModule, nClkSrc );

	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			dClkValue = 0.0;
			break;
		case BRDclks_ADC_SUBGEN0:
		case BRDclks_ADC_SUBSYNT:
			if( pSrvCfg->nRefGen0Type == 0 )
			{
				//
				// Генератор на субмодуле непрограммируемый
				//
				dClkValue = pSrvCfg->nRefGen0;
			}
			else
			{
				Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
			}
			break;
		case BRDclks_ADC_SUBGEN1:
			dClkValue = pSrvCfg->nRefGen1;
			break;
		case BRDclks_ADC_EXTCLK:
			dClkValue = pSrvCfg->SubExtClk;
			break;
		default:
			dClkValue = 0.0;
			break;
	}

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FDVR, &nAdcRateDivider );
	refRate = dClkValue / nAdcRateDivider;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetInpSrc( CModule *pModule, PVOID pCtrl )
{
	U32		inpSrc	= *(U32*)pCtrl;
return BRDerr_OK;

	if( inpSrc != 0 )
		inpSrc = 1;

	//
	// Читать/писать регистр CONTROL1 (какая-то херня!!!)
	//
	ADC_CTRL	regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	regCtrl.ByBits.Clbr = inpSrc;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::GetInpSrc( CModule *pModule, PVOID pCtrl )
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
int CAdc212x1gSrv::SetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	ADC212X1GSRV_Cfg	*pSrvCfg  = (ADC212X1GSRV_Cfg*)m_pConfig;
	int					Status = BRDerr_OK;
	U32					code;

	//
	// Скорректировать запрашиваемое смещение нуля для ADM212x1GWB (-10%..+10%)
	//
	if( pSrvCfg->nSubunitId == 0x271 )
		refBias *= 10.0;

	//
	// Вычислить код для ИПН
	//
	code = (U32)(S32)floor( ((refBias+100.0) / 200.0) * 255.0 );
	if( code > 255 )
		code = 255;
	
	//
	// Смещение нуля для ADM212x1G (-100%..+100%)
	//
	refBias = ((double)code / 255) * 200.0 - 100.0;

	//
	// Скорректировать запрашиваемое смещение нуля для ADM212x1GWB (-10%..+10%)
	//
	if( pSrvCfg->nSubunitId == 0x271 )
		refBias /= 10.0;


	//
	// Сохранить результаты расчета в m_pConfig
	//
	pSrvCfg->aBias[Chan] = refBias;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC212X1G_BIAS0 - 1] = (U08)code;

	MAIN_THDAC thDac;

	thDac.ByBits.Data = code;
	thDac.ByBits.Num  = Chan + BRDtdn_ADC212X1G_BIAS0;
	IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, thDac.AsWhole );

	return Status;
}

//***************************************************************************************
int CAdc212x1gSrv::GetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	ADC212X1GSRV_Cfg  *pSrvCfg  = (ADC212X1GSRV_Cfg*)m_pConfig;

	refBias = pSrvCfg->aBias[Chan];

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan)
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
int CAdc212x1gSrv::GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan)
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
int CAdc212x1gSrv::SetStartMode( CModule *pModule, PVOID pStartStopMode )
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
int CAdc212x1gSrv::GetStartMode( CModule *pModule, PVOID pStartStopMode )
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
int CAdc212x1gSrv::SetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	ADC212X1GSRV_Cfg	*pSrvCfg  = (ADC212X1GSRV_Cfg*)m_pConfig;
	int					err;
	double				inpRange;

	//
	// ADM212x1G
	//
	if( pSrvCfg->nSubunitId == 0x270 )
	{
		inpRange = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / refGain;
		err = SetInpRange( pModule, inpRange, Chan );
		if( err >= 0 )
			refGain = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / inpRange;

		return err;
	}

	//
	// ADM212x1GWB
	//
	U32			regVal;
	U32			attCode;
	U32			aAdcR0Code[6] = { 0x00, 0x3E, 0x68, 0xA6, 0xD5, 0xFF };
	U32			aAdcR1Code[6] = { 0x10, 0x80, 0x30, 0xA0, 0x80, 0xF0 };
	U32			idxAdcCode;
	U32			chanNo = (Chan==0) ? 0 : 1;

	if( refGain > 2.5 )
		refGain = 2.5;
	if( refGain < -15.5 )
		refGain = -15.5;

	if( refGain <= 0.0 )
	{
		//
		// Работает аттенюатор
		//
		attCode = (U32)floor( refGain * -2.0 );
		refGain = ((double)attCode) / -2.0;

		idxAdcCode = 0;
	}
	else
	{
		//
		// Работает усилитель внутри кристалла АЦП
		//
		attCode = 0;

		idxAdcCode = (U32)floor( refGain * 2.0 );
		refGain = ((double)idxAdcCode) / 2.0;
	}

	//
	// Программирование аттенюатора
	//
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, &regVal );
	regVal &= (Chan==0) ? 0xFFE0 : 0xE0FF;
	if( Chan == 1 )
		attCode <<= 8;
	regVal |= attCode;
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, regVal );

	//
	// Программирование кристалла АЦП
	//
	AdcRegWrite( pModule, chanNo, 0, aAdcR0Code[idxAdcCode] );
	AdcRegWrite( pModule, chanNo, 1, aAdcR1Code[idxAdcCode] );

	pSrvCfg->aGainDb[chanNo] = refGain;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::GetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	ADC212X1GSRV_Cfg	*pSrvCfg  = (ADC212X1GSRV_Cfg*)m_pConfig;
	int					err;
	double				inpRange;

	//
	// ADM212x1G
	//
	if( pSrvCfg->nSubunitId == 0x270 )
	{
		err = GetInpRange( pModule, inpRange, Chan );
		if( err >= 0 )
			refGain = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / inpRange;
		return err;
	}

	//
	// ADM212x1GWB
	//
	U32			chanNo = (Chan==0) ? 0 : 1;

	refGain = pSrvCfg->aGainDb[chanNo];

	return BRDerr_OK;

	//U32			regVal;
	//U32			gainCode;

	//IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, &regVal );
	//gainCode = regVal;
	//if( Chan == 1 )
	//	gainCode >>= 8;
	//gainCode &= 0x1F;
	//refGain = ((double)gainCode) / -2.0;
	//
	//return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	ADC212X1GSRV_Cfg  *pSrvCfg  = (ADC212X1GSRV_Cfg*)m_pConfig;
	ULONG				mask;
	int					ii;
	int					err;

	//
	// ADM212x1GWB
	//
	if( pSrvCfg->nSubunitId == 0x271 )
	{
		double			gainDb;

		gainDb = 20.0 * log( refInpRange * 1000.0 / (double)pSrvCfg->AdcCfg.InpRange);
		err = SetGain( pModule, gainDb, Chan );
		if( err >= 0 )
			refInpRange = (pSrvCfg->AdcCfg.InpRange / pow( 10.0, gainDb / 20.0 )) / 1000.0;
		return err;
	}

	//
	// ADM212x1G
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
	ADM2IF_GAIN		gain;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, &gain.AsWhole );
	switch(Chan)
	{
		case 0:
			gain.ByBits.Chan0 = mask;
			break;
		case 1:
			gain.ByBits.Chan1 = mask;
			break;
		default:
			return BRDerr_BAD_PARAMETER;
	}
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, gain.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::GetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	ADC212X1GSRV_Cfg	*pSrvCfg  = (ADC212X1GSRV_Cfg*)m_pConfig;
	ADM2IF_GAIN			gain;
	double				value;
	int					err;

	//
	// ADM212x1GWB
	//
	if( pSrvCfg->nSubunitId == 0x271 )
	{
		double			gainDb;

		err = GetGain( pModule, gainDb, Chan );
		if( err >= 0 )
			refInpRange = (pSrvCfg->AdcCfg.InpRange / pow( 10.0, gainDb / 20.0 )) / 1000.0;
		return err;
	}

	//
	// ADM212x1G
	//
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, &gain.AsWhole );
	switch(Chan)
	{
		case 0:
			value = pSrvCfg->adIcrRange[gain.ByBits.Chan0];
			break;
		case 1:
			value = pSrvCfg->adIcrRange[gain.ByBits.Chan1];
			break;
		default:
			return BRDerr_BAD_PARAMETER;
	}
	refInpRange = value;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetCode( CModule *pModule, ULONG type )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::GetCode( CModule *pModule, ULONG &refType )
{
	refType = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetMaster( CModule *pModule, ULONG mode )
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
int CAdc212x1gSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	int			err;
	int			chanNo;
	//ULONG		prevMask;

	//err = GetChanMask( pModule, prevMask );
	//if( err < 0 )
	//	return err;
	err = CAdcSrv::SetChanMask( pModule, mask );
	if( err < 0 )
		return err;

	RealDelay( 200, 0 );
	for( chanNo=0; chanNo<BRD_CHANCNT; chanNo++ )
		if( mask & (1<<chanNo) )
		{
			AdcRegWrite( pModule, chanNo, 0, 0x00 );
			AdcRegWrite( pModule, chanNo, 1, 0x10 );
			AdcRegWrite( pModule, chanNo, 2, 0x03 );
			AdcRegWrite( pModule, chanNo, 5, 0xB4 );
			AdcRegWrite( pModule, chanNo, 6, 0x04 );
			AdcRegWrite( pModule, chanNo, 7, 0x00 );
		}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::ExtraInit(CModule* pModule)
{
	ADC212X1GSRV_Cfg	*pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
	int					regAdr;

	pSrvCfg->aGainDb[0] = pSrvCfg->aGainDb[1] = 0.0;
	ADF4106_InitPLL( pModule );

	//
	// Определение частоты кварца для генератора Si570/Si571
	//
	pSrvCfg->dRefGen1Fxtal = 0.0;
	if( pSrvCfg->nRefGen0Type != 0 )
	{
		U32			regData[20];
		ULONG		clkSrc;

		//
		// Подать питание на Si570/Si571
		//
		GetClkSource( pModule, clkSrc );
		SetClkSource( pModule, 0 );
		SetClkSource( pModule, 1 );
		I2cWrite( pModule, -1, 135, 0x80 );		// Reset
		RealDelay(100,1);
		
		//
		// Считать регистры Si570/Si571
		//
		for( regAdr=7; regAdr<=12; regAdr++ )
		{
			I2cRead( pModule, -1, regAdr, &regData[regAdr] );
			//printf( "Read Si571 reg %d, val=0x%X\n", regAdr, regData[regAdr] );
		}
		SetClkSource( pModule, clkSrc );

		//
		// Рассчитать частоту кварца
		//
		SI571_CalcFxtal( &(pSrvCfg->dRefGen1Fxtal), (double)(pSrvCfg->nRefGen0), regData );
		printf( ">> XTAL = %f kHz\n", pSrvCfg->dRefGen1Fxtal/1000.0 );
		printf( ">> GREF = %f kHz\n", ((double)(pSrvCfg->nRefGen0))/1000.0 );
	}

	return BRDerr_OK;
}

//***************************************************************************************
int	CAdc212x1gSrv::AdcRegWrite( CModule* pModule, U32 adcType, U32 regAdr, U32 regVal )
{
	U32		status, addr;

	addr = (regAdr & 0x1F) | ((adcType&0x1)<<12);
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGADDR,  addr );

	do     DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
	while( 0 == (status&0x1) );

	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGDATA,  regVal );

	do     DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
	while( 0 == (status&0x1) );

	return BRDerr_OK;
}

//***************************************************************************************
int	CAdc212x1gSrv::I2cRead(  CModule* pModule, U32 i2cType, U32 regAdr, U32 *pRegVal )
{
	ADC212X1GSRV_Cfg *pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
	U32			status, tmp;
	U32			genChipAdr = pSrvCfg->nRefGen0Adr;

	do     DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
	while( 0 == (status&0x1) );

	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_ADDR,  regAdr );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_CTRL,  (U32)0x2 | (genChipAdr<<8) ); 

	do     DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
	while( 0 == (status&0x1) );

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_I2C_DATAL, &tmp );
	*pRegVal = tmp;

	return BRDerr_OK;
}

//***************************************************************************************
int	CAdc212x1gSrv::I2cWrite( CModule* pModule, U32 i2cType, U32 regAdr, U32 regVal )
{
	ADC212X1GSRV_Cfg *pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
	U32			status;
	U32			genChipAdr = pSrvCfg->nRefGen0Adr;

	do     DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
	while( 0 == (status&0x1) );

	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_ADDR,  regAdr );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_DATAL, regVal );
	// IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_DATAH, (U32)0 );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_CTRL,  (U32)0x1 | (genChipAdr<<8) );

	do     DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
	while( 0 == (status&0x1) );

	return BRDerr_OK;
}

//***************************************************************************************
int	CAdc212x1gSrv::Si571SetRate( CModule* pModule, double *pRate )
{
	ADC212X1GSRV_Cfg *pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
	U32			regData[20];
	int			regAdr;
	int			status = BRDerr_OK;

	//
	// Скорректировать частоту, если необходимо
	//
	if( *pRate > (double)pSrvCfg->nRefGen0Max )
	{
		*pRate = (double)pSrvCfg->nRefGen0Max;
		status = BRDerr_WARN;
	}

	SI571_SetRate( pRate, pSrvCfg->dRefGen1Fxtal, regData );

	//
	// Запрограммировать генератор
	//
	I2cWrite( pModule, -1, 137, 0x10 );		// Freeze DCO
	for( regAdr=7; regAdr<=18; regAdr++ )
		I2cWrite( pModule, -1, regAdr, regData[regAdr] );		
	I2cWrite( pModule, -1, 137, 0x0 );		// Unfreeze DCO
	I2cWrite( pModule, -1, 135, 0x40 );		// Assert the NewFreq bit

	//printf( "Write Si571: R7-R12: %02x, %02x, %02x, %02x, %02x, %02x,",
	//	regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );
	//printf( "x=%.1f, f=%.1f\n", pSrvCfg->dRefGen1Fxtal, *pRate );

	//
	// Считать регистры Si570/Si571
	//
	U32			regData2[20];
	for( regAdr=7; regAdr<=12; regAdr++ )
		I2cRead( pModule, -1, regAdr, &regData2[regAdr] );

	//printf( " Read Si571: R7-R12: %02x, %02x, %02x, %02x, %02x, %02x\n",
	//	regData2[7], regData2[8], regData2[9], regData2[10], regData2[11], regData2[12] );

	return BRDerr_OK;
}

//***************************************************************************************
int	CAdc212x1gSrv::Si571GetRate( CModule* pModule, double *pRate )
{
	ADC212X1GSRV_Cfg *pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
	U32			regData[20];
	ULONG		clkSrc;
	int			regAdr;

	*pRate = 0.0;

	//
	// Проверить источник частоты
	//
	GetClkSource( pModule, clkSrc );
	if( (clkSrc != BRDclks_ADC_SUBGEN0) && (clkSrc != BRDclks_ADC_SUBSYNT) )
        return BRDerr_ERROR;

	//
	// Считать регистры Si570/Si571
	//
	for( regAdr=7; regAdr<=12; regAdr++ )
		I2cRead( pModule, -1, regAdr, &regData[regAdr] );

	SI571_GetRate( pRate, pSrvCfg->dRefGen1Fxtal, regData );

	//printf( "Read2 Si571: R7-R12: %02x, %02x, %02x, %02x, %02x, %02x. ",
	//	regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );
	//printf( "x=%.1f, f=%.1f\n", pSrvCfg->dRefGen1Fxtal, *pRate );


	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetAdcStartMode( CModule *pModule, ULONG source, ULONG inv, double *pCmpThr )
{
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
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus;
	do 
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		pStatus = (PADM2IF_STATUS)&param.val;
	} while(!pStatus->ByBits.CmdRdy);

	//
	// Определить шкалу преобразования для выбранного источника сигнала старата
	//
	double inp_range = 2.5;
	if(BRDsts_ADC_CHAN0 == source)
		GetInpRange(pModule, inp_range, 0); 
	if(BRDsts_ADC_CHAN1 == source)
		GetInpRange(pModule, inp_range, 1); 

	//
	// Рассчитать код для ИПН7
	//
	PADC212X1GSRV_Cfg pSrvCfg = (PADC212X1GSRV_Cfg)m_pConfig;
	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.;

	double value = *pCmpThr;
	int max_code = 255;//ADC212X1G_STARTEXTMAXCODE;
	int min_code = 0;//ADC212X1G_STARTEXTMINCODE;
	if(BRDsts_ADC_CHAN0 == source || BRDsts_ADC_CHAN1 == source)
	{
		//double koef = ADC212X1G_STARTCHANKOEF;
		//max_code = ADC212X1G_STARTCHANMAXCODE;
		//min_code = ADC212X1G_STARTCHANMINCODE;
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
		//double koef = ADC212X1G_STARTCHANKOEF;
		//*pCmpThr = value * inp_range / koef;
		*pCmpThr = (value * inp_range) / max_thr;
	}

	//
	// Сохранить рассчитанные значения в структуре
	//
	pSrvCfg->StCmpThr = *pCmpThr;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC212X1G_STARTCMP - 1] = (UCHAR)dac_data;

	//
	// Записать код в ИПН7
	//
	MAIN_THDAC thDac;

	thDac.ByBits.Data = dac_data;
	thDac.ByBits.Num = BRDtdn_ADC212X1G_STARTCMP;
	IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, thDac.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::GetAdcStartMode( CModule *pModule, PULONG pSource, PULONG pInv, double *pCmpThr )
{
	ADC212X1GSRV_Cfg	*pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
	ADC_CTRL regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	*pSource = regCtrl.ByBits.StartSrc;
	*pInv    = regCtrl.ByBits.StartInv;

	*pCmpThr = pSrvCfg->StCmpThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetSpecific( CModule *pModule, BRD_AdcSpec *pSpec )
{
	int status;

	status = CAdcSrv::SetSpecific(pModule, pSpec);

	switch(pSpec->command)
	{
		case ADC212X1Gcmd_SETMU:
			status = SetMu(pModule, pSpec->arg);
			break;
		case ADC212X1Gcmd_GETMU:
			status = GetMu(pModule, pSpec->arg);
			break;
		case ADC212X1Gcmd_SETSTARTCLKSL:
			status = SetStartClkSl(pModule, pSpec->arg);
			break;
		case ADC212X1Gcmd_GETSTARTCLKSL:
			status = GetStartClkSl(pModule, pSpec->arg);
			break;
		case ADC212X1Gcmd_SETDISCNTR:
			status = SetStartDisCntr(pModule, pSpec->arg);
			break;
	}
	return status;
}

//***************************************************************************************
int CAdc212x1gSrv::SetMu( CModule *pModule, void *args )
{
	ADC212X1GSRV_MU *pMU = (ADC212X1GSRV_MU*)args;

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
		SetInpFilter(pModule, pMU->inpFilter[i], i);
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

//***************************************************************************************
int CAdc212x1gSrv::GetMu( CModule *pModule, void *args )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetStartClkSl( CModule *pModule, void *args )
{
	U32			*pParam = (U32*)args;
	ADC_CTRL	ctrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &ctrl.AsWhole );
	ctrl.ByBits.StartSl = (*pParam) & 0x1;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, ctrl.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::GetStartClkSl( CModule *pModule, void *args )
{
	U32			*pParam = (U32*)args;
	ADC_CTRL	ctrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &ctrl.AsWhole );
	*pParam = ctrl.ByBits.StartSl;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc212x1gSrv::SetStartDisCntr( CModule *pModule, void *args )
{
	U32		cntr = *(U32*)args;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_STARTDISCNTR, cntr );

	return BRDerr_OK;
}

//****************************************************************************************
int CAdc212x1gSrv::ADF4106_WritePllData(CModule* pModule, U16 hiWord, U16 loWord)
{
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_PLL_HIGH, hiWord );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_PLL_LOW, loWord );

	return BRDerr_OK;
}

//****************************************************************************************
int CAdc212x1gSrv::ADF4106_InitPLL( CModule* pModule )
{
	ADC212X1GSRV_Cfg *pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;

	pSrvCfg->Pll_R_CNT.AsWhole = 0;
	pSrvCfg->Pll_N_CNT.AsWhole = 0;
	pSrvCfg->Pll_Func.AsWhole = 0;

	pSrvCfg->Pll_R_CNT.ByBits.Addr = 0;
	pSrvCfg->Pll_N_CNT.ByBits.Addr = 1;
	pSrvCfg->Pll_Func.ByBits.Addr = 2;

	pSrvCfg->Pll_R_CNT.ByBits.AB_WIDTH = 1; //1.3 ns
	pSrvCfg->Pll_R_CNT.ByBits.TEST_MODE = 0;
	pSrvCfg->Pll_R_CNT.ByBits.LDP = 0;
	ADF4106_WritePllData(pModule, pSrvCfg->Pll_R_CNT.ByWord.HiWord, pSrvCfg->Pll_R_CNT.ByWord.LoWord);

	pSrvCfg->Pll_N_CNT.ByBits.CP_GAIN = 0;
	ADF4106_WritePllData(pModule, pSrvCfg->Pll_N_CNT.ByWord.HiWord, pSrvCfg->Pll_N_CNT.ByWord.LoWord);

	pSrvCfg->Pll_Func.ByBits.CNT_RST = 0;
	pSrvCfg->Pll_Func.ByBits.PWDN_1 = 0;
	pSrvCfg->Pll_Func.ByBits.PWDN_2 = 0;
	pSrvCfg->Pll_Func.ByBits.MUX_OUT = 2;
	pSrvCfg->Pll_Func.ByBits.PD_POL = 1;
	pSrvCfg->Pll_Func.ByBits.CP_3STATE = 0;
	pSrvCfg->Pll_Func.ByBits.FL_ENBL = 0;
	pSrvCfg->Pll_Func.ByBits.FL_MODE = 0;
	pSrvCfg->Pll_Func.ByBits.TIMER_CNT = 0;
	pSrvCfg->Pll_Func.ByBits.CUR_SET_1 = 3;
	pSrvCfg->Pll_Func.ByBits.CUR_SET_2 = 7;

	ADF4106_WritePllData(pModule, pSrvCfg->Pll_Func.ByWord.HiWord, pSrvCfg->Pll_Func.ByWord.LoWord);

	return BRDerr_OK;
}

int P_DIM[4] = {8, 16, 32, 64};

//****************************************************************************************
int CAdc212x1gSrv::ADF4106_OnPll(CModule* pModule, double& ClkValue, double PllRefValue)
{
	ADC212X1GSRV_Cfg *pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;

	double	dFout = ClkValue; // Частота вых. сигнала, Fvco (Гц)
	double	dFin = PllRefValue; // Частота вх. сигнала, Fosc (Гц)
	double	dFoutreal, dFoutrealAux; // Частота вых. сигнала, полученная врезультате вычислений (Гц)
	double	dFrelative, dFrelativeAux;		// Отностельная погрешность чатоты
	int		nR, nP, nA, nB;
	int		nRcnt, nAcnt, nBcnt;
	int		nPidx;

	double	dFcmax = 101000000.0;	// Максимально допустимая частота сравнения 100 МГц
	double	dFinmax = 301000000.0;	// Максимально допустимая частота вх. сигнала 300 МГц
	double	dFrelativemax = 0.0;	// Допустимая отностельная погрешность чатоты
	int		nRmin = 1, nRmax = 16383;
	int		aP[] = { 8, 16, 32, 64 };
	int		nAmin = 0, nAmax = 63, nAmaxTmp;
	int		nBmin = 3, nBmax = 8191;

	int		ii;

printf( "ADF4106_OnPll() p1\n" );
	if( dFin > dFinmax )
		return -1;

printf( "ADF4106_OnPll() p2\n" );
	//
	// Определяем делитель P
	//
	for( ii=0; ii<sizeof(aP)/sizeof(aP[0]); ii++ )
		if( (dFout/aP[ii]) <= dFcmax )
		{
			nP = aP[ii];
			nPidx = ii;
			break;
		}
	if( ii >= sizeof(aP)/sizeof(aP[0]) )
		return -2;

printf( "ADF4106_OnPll() p3\n" );
	//
	// Определяем Rmin
	//
	nRmin = (int)floor( dFin / dFcmax );
	if( nRmin < 1 )
		nRmin = 1;

	//
	// Определяем Bmin
	//
	nBmin = (int)floor( (dFin * (double)nP) / (dFout * (double)nRmin) );
	if( nBmin < 3 )
		nBmin = 3;

	dFrelative = 1.0;

	//
	// Поиск в цикле R, A, B
	//
	for( nRcnt=nRmin; nRcnt<=nRmax; nRcnt++ )
	{
		for( nBcnt=nBmin; nBcnt<=nBmax; nBcnt++ )
		{
			nAmaxTmp = min( nAmax, nBcnt-1 );
			for( nAcnt=nAmin; nAcnt<=nAmaxTmp; nAcnt++ )
			{
				dFoutrealAux  = dFin / (double)nRcnt;
				dFoutrealAux *= (double)(nP * nBcnt + nAcnt);
				dFrelativeAux = (dFoutrealAux - dFout) / dFout;
				dFrelativeAux = fabs( dFrelativeAux );
				if( dFrelativeAux < dFrelative )
				{
					dFrelative = dFrelativeAux;
					dFoutreal  = dFoutrealAux;
					nR = nRcnt;
					nA = nAcnt;
					nB = nBcnt;
				}
				if( dFrelative <= dFrelativemax )
					break;
			}
			if( dFrelative <= dFrelativemax )
				break;
		}
		if( dFrelative <= dFrelativemax )
			break;
	}

printf( ">>> Fosc=%.3f MHz, Fvco req=%.3f MHz, Fvco=%.3f MHz, dF=%.3f\n",
	   dFin/1000000.0, dFout/1000000.0, dFoutreal/1000000.0, dFrelative );
printf( ">>> A=%d, B=%d, R=%d, P=%d\n", nA, nB, nR, nP );

	pSrvCfg->Pll_R_CNT.ByBits.R_CNTR = nR;
	ADF4106_WritePllData(pModule, pSrvCfg->Pll_R_CNT.ByWord.HiWord, pSrvCfg->Pll_R_CNT.ByWord.LoWord);
	pSrvCfg->Pll_N_CNT.ByBits.A_CNTR = nA;
	pSrvCfg->Pll_N_CNT.ByBits.B_CNTR = nB;
	ADF4106_WritePllData(pModule, pSrvCfg->Pll_N_CNT.ByWord.HiWord, pSrvCfg->Pll_N_CNT.ByWord.LoWord);
	pSrvCfg->Pll_Func.ByBits.PRESCALER = nPidx;
	ADF4106_WritePllData(pModule, pSrvCfg->Pll_Func.ByWord.HiWord, pSrvCfg->Pll_Func.ByWord.LoWord);

	IPC_delay(50);
	return BRDerr_OK;
}

//int CAdc212x1gSrv::ADF4106_OnPll(CModule* pModule, double& ClkValue, double PllRefValue)
//{
//	ADC212X1GSRV_Cfg *pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
//
//	double f_vco = ClkValue;
//	double f_osc = PllRefValue;
//
////printf( "f_vco=%f, f_osc=%f\n", f_vco, f_osc );
//
//	double f_min = 325000000.; // 325 MHz
//	int A_max = 63;
//	int B_max = 8191;
//	int P_min = int(floor(f_vco / f_min));
//	int P = P_DIM[3];
//	for(int i = 0; i < 4; i++)
//		if(P_min <= P_DIM[i]) {
//			P = P_DIM[i];
//			break;
//		}
//
//		int R_max = int(floor(f_osc / (f_vco / (P * B_max + A_max)) ));
//		if(R_max > 16383) R_max = 16383;
//
//		int R_min = int(ceil(f_osc / PLL_F_CMP_MAX));
//		if(R_min < 1) R_min = 1;
//
//		int R, B_tmp, B, A = 0;
//		//int delta_min = 100; // 1G
//		double delta_min = 10000000; // 2G
//
//		for(int A = 0; A <= A_max; A++)
//		{
//			for(int R_tmp = R_min; R_tmp <= R_max; R_tmp++)
//			{
//				B_tmp = int(((f_vco * R_tmp / f_osc) - A) / P);
//				double f_new = (P * B_tmp + A) * (f_osc / R_tmp);
//				//int delta = int((fabs(pSrvCfg->PllFreq - f_new) / pSrvCfg->PllFreq) * 100); // 1G
//				//		double delta = (fabs(pSrvCfg->PllFreq - f_new) / pSrvCfg->PllFreq) * 10000000; // 2G
//				double delta = (fabs(pSrvCfg->PllFreq - f_new) / pSrvCfg->PllFreq); // 200M
//				if(delta < delta_min)
//				{
//					ClkValue = f_new;
//					delta_min = delta;
//					B = B_tmp;
//					R = R_tmp;
//					if(delta_min == 0)
//						break;
//				}
//			}
//			if(delta_min == 0) 
//				break;
//		}
////printf( "A=%d, B=%d, R=%d, P=%d\n", A, B, R, P );
//		pSrvCfg->Pll_R_CNT.ByBits.R_CNTR = R;
//		ADF4106_WritePllData(pModule, pSrvCfg->Pll_R_CNT.ByWord.HiWord, pSrvCfg->Pll_R_CNT.ByWord.LoWord);
//		pSrvCfg->Pll_N_CNT.ByBits.A_CNTR = A;
//		pSrvCfg->Pll_N_CNT.ByBits.B_CNTR = B;
//		ADF4106_WritePllData(pModule, pSrvCfg->Pll_N_CNT.ByWord.HiWord, pSrvCfg->Pll_N_CNT.ByWord.LoWord);
//		pSrvCfg->Pll_Func.ByBits.PRESCALER = P;
//		ADF4106_WritePllData(pModule, pSrvCfg->Pll_Func.ByWord.HiWord, pSrvCfg->Pll_Func.ByWord.LoWord);
//
//		Sleep(50);
//		return BRDerr_OK;
//}

//****************************************************************************************
int CAdc212x1gSrv::ADF4106_SetPllMode(CModule* pModule, PBRD_PllMode pPllMode)
{
	ADC212X1GSRV_Cfg *pSrvCfg = (ADC212X1GSRV_Cfg*)m_pConfig;
	pSrvCfg->Pll_Func.ByBits.MUX_OUT = pPllMode->MuxOut;
	ADF4106_WritePllData(pModule, pSrvCfg->Pll_Func.ByWord.HiWord, pSrvCfg->Pll_Func.ByWord.LoWord);
	return BRDerr_OK;
}


// ***************** End of file Adc212x1gSrv.cpp ***********************
