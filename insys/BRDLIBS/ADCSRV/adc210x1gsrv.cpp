/*
 ***************** File Adc210x1gSrv.cpp ************
 *
 * BRD Driver for ADС service on ADM210x1G
 *
 * (C) InSys by Ekkore Okt 2009
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "Adc210x1gSrv.h"

#define	CURRFILE "ADC210X1GSRV"

const double BRD_ADC_MAXGAINTUN	= 1.; // not used

//***************************************************************************************
CAdc210x1gSrv::CAdc210x1gSrv(int idx, int srv_num, CModule* pModule, PADC210X1GSRV_Cfg pCfg) :
	CAdcSrv(idx, _BRDC("ADC210X1G"), srv_num, pModule, pCfg, sizeof(ADC210X1GSRV_Cfg))
{
}

//***************************************************************************************
int CAdc210x1gSrv::CtrlRelease( void *pDev, void *pServData, ULONG cmd, void *args )
{
	CModule* pModule = (CModule*)pDev;

	CAdcSrv::SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);

	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб (вместо BRDerr_OK)
}

//***************************************************************************************
void CAdc210x1gSrv::GetAdcTetrNum( CModule *pModule )
{
	//
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	//

	//
	// Поиск номера терады. Если тетрада не найдена, то -1.
	//
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC210X1G_TETR_ID);
}

//***************************************************************************************
void CAdc210x1gSrv::FreeInfoForDialog( PVOID pInfo )
{
	ADC210X1GSRV_Info *pSrvInfo = (ADC210X1GSRV_Info*)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc210x1gSrv::GetInfoForDialog( CModule *pModule )
{
	ADC210X1GSRV_Cfg  *pSrvCfg  = (ADC210X1GSRV_Cfg*)m_pConfig;
	ADC210X1GSRV_Info *pInfo = new ADC210X1GSRV_Info;
	int					ii;

	pInfo->Size = sizeof(ADC210X1GSRV_Info);

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
		GetDcCoupling( pModule, (ULONG&)pInfo->pAdcInfo->DcCoupling[ii], ii );
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
	pInfo->nRefGen1Min      = pSrvCfg->nRefGen1Min;          // опорный генератор 1 (Гц)
	pInfo->nRefGen1Max      = pSrvCfg->nRefGen1Max;          // опорный генератор 1 (Гц)
	pInfo->nRefGen1Type     = pSrvCfg->nRefGen1Type;         // тип опорного генератора 1
	pInfo->nRefGen2         = pSrvCfg->nRefGen2;             // опорный генератор 2 (Гц)
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
	pInfo->dDoubleFreqRange1 = pSrvCfg->dDoubleFreqRange1;
	pInfo->dDoubleFreqBias1  = pSrvCfg->dDoubleFreqBias1;

	//char		msg[256];
	//sprintf(msg, _T("GetInfoForDialog: clkSrc = %d"), pInfo->pAdcInfo->ClockSrc);
	//MessageBox(NULL, msg, "Debug", MB_OK);

	return pInfo;
}

//***************************************************************************************
int CAdc210x1gSrv::SetPropertyFromDialog( void *pDev, void *args )
{
	CModule				*pModule = (CModule*)pDev;
	ADC210X1GSRV_Info	*pInfo = (ADC210X1GSRV_Info*)args;
	ADC210X1GSRV_Cfg	*pSrvCfg = (ADC210X1GSRV_Cfg*)m_pConfig;
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
	//sprintf(msg, _T("SetPropertyFromDialog: clkSrc = %d"), pInfo->pAdcInfo->ClockSrc);
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

	//
	// Режимы работы кристалла АЦП
	//
	ADC210X1GSRV_DBLCLK		rDblClk;

	pSrvCfg->nDoubleFreq = pInfo->nDoubleFreq;
	pSrvCfg->dDoubleFreqBias1 = pInfo->dDoubleFreqBias1;
	pSrvCfg->dDoubleFreqRange1 = pInfo->dDoubleFreqRange1;

	rDblClk.size = sizeof(ADC210X1GSRV_DBLCLK);
	rDblClk.isDblClk = (0==pSrvCfg->nDoubleFreq) ? 0 : 1;
	rDblClk.inpSrc   = (1==pSrvCfg->nDoubleFreq) ? 0 : 1;
	rDblClk.valRange = 0;
	rDblClk.inp0Range = 0.0;
	rDblClk.inp1Range = pInfo->dDoubleFreqRange1;
	rDblClk.inp0Bias  = 0.0;
	rDblClk.inp1Bias  = pInfo->dDoubleFreqBias1;
	SetDblClkEx( pModule, &rDblClk );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::SetProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	ADC210X1GSRV_Cfg	*pSrvCfg = (ADC210X1GSRV_Cfg*)m_pConfig;
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

	GetPrivateProfileString(SectionName, _BRDC("ClockSource"), _BRDC("129"), Buffer, sizeof(Buffer), iniFilePath);
	clkSrc = BRDC_strtol(Buffer, &endptr, 0);
	//char		msg[256];
	//sprintf(msg, _T("SetProperties: clkSrc = %d"), clkSrc);
	//MessageBox(NULL, msg, "Debug", MB_OK);
	SetClkSource(pModule, clkSrc);

	//
	// Извлечение требуемого режима синхронизации MASTER/SLAVE
	//
	U32		isSlave;

	GetPrivateProfileString(SectionName, _BRDC("StartSlave"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	isSlave = BRDC_atoi(Buffer);
	SetStartSlave(pModule, isSlave);

	GetPrivateProfileString(SectionName, _BRDC("ClockSlave"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	isSlave = BRDC_atoi(Buffer);
	SetClockSlave(pModule, isSlave);

	//
	// Извлечение частоты внешнего генератора
	//
	double				clkValue;

	GetPrivateProfileString(SectionName, _BRDC("ExternalClockValue"), _BRDC("120000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	clkValue = BRDC_atof(Buffer);
	pSrvCfg->SubExtClk = ROUND(clkValue);
	SetClkValue(pModule, clkSrc, clkValue);

	GetPrivateProfileString(SectionName, _BRDC("SlclkinClockValue"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	pSrvCfg->dSlclkinClk = BRDC_atof(Buffer);

	//
	// Извлечение требуемой частоты дискретизации
	//
	double			rate;
	
	GetPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("100000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	rate = BRDC_atof(Buffer);
	SetRate(pModule, rate, clkSrc, clkValue);

	//
	// Извлечение требуемой инверсии тактовой частоты
	//
	U32				clkInv;
	
	GetPrivateProfileString(SectionName, _BRDC("ClockInv"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	clkInv = BRDC_atoi(Buffer);
	SetClkInv(pModule, clkInv);

	//
	// Извлечение для каждого канала требуемых:
	// 1. шкалы преобразования
	// 2. смещения нуля
	// 3. Подстройки шкалы преобразования
	//
	double			aInpRange[MAX_CHAN];
	double			aBias[MAX_CHAN];
	double			aGainTuning[MAX_CHAN];
	DWORD			ret;

	for( ii = 0; ii < MAX_CHAN; ii++ )
	{
		BRDC_sprintf(ParamName, _BRDC("InputRange%d"), ii);
		ret = GetPrivateProfileString(SectionName, ParamName, _BRDC("0.5"), Buffer, sizeof(Buffer), iniFilePath);
		aInpRange[ii] = BRDC_atof(Buffer);
		SetInpRange(pModule, aInpRange[ii], ii);

		BRDC_sprintf(ParamName, _BRDC("Bias%d"), ii);
		ret = GetPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		aBias[ii] = BRDC_atof(Buffer);
		SetBias(pModule, aBias[ii], ii);
		
		BRDC_sprintf(ParamName, _BRDC("GainTuning%d"), ii);
		ret = GetPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		aGainTuning[ii] = BRDC_atof(Buffer);
		SetGainTuning(pModule, aGainTuning[ii], ii);
	}

	//
	// Извлечение требуемого режима стартовой синхронизации
	//
	BRD_AdcStartMode rStMode;

	GetPrivateProfileString(SectionName, _BRDC("StartSource"), _BRDC("3"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.src = BRDC_atoi(Buffer);

	GetPrivateProfileString(SectionName, _BRDC("StartInverting"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.inv = BRDC_atoi(Buffer);

	GetPrivateProfileString(SectionName, _BRDC("StartLevel"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.level = BRDC_atof(Buffer);

	CAdcSrv::GetStartMode(pModule, &rStMode.stndStart);
	SetStartMode(pModule, &rStMode);

	//
	// Режимы работы кристалла АЦП
	//
	ADC210X1GSRV_DBLCLK		rDblClk;

	GetPrivateProfileString(SectionName, _BRDC("DoubleFreq"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	pSrvCfg->nDoubleFreq = BRDC_atoi(Buffer);

	GetPrivateProfileString(SectionName, _BRDC("DoubleFreqBias1"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	pSrvCfg->dDoubleFreqBias1 = BRDC_atof(Buffer);

	GetPrivateProfileString(SectionName, _BRDC("DoubleFreqRange1"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	pSrvCfg->dDoubleFreqRange1 = BRDC_atof(Buffer);

	rDblClk.size = sizeof(ADC210X1GSRV_DBLCLK);
	rDblClk.isDblClk = (0==pSrvCfg->nDoubleFreq) ? 0 : 1;
	rDblClk.inpSrc   = (1==pSrvCfg->nDoubleFreq) ? 0 : 1;
	rDblClk.valRange = 0;
	rDblClk.inp0Range = 0.0;
	rDblClk.inp1Range = pSrvCfg->dDoubleFreqRange1;
	rDblClk.inp0Bias  = 0.0;
	rDblClk.inp1Bias  = pSrvCfg->dDoubleFreqBias1;
	SetDblClkEx( pModule, &rDblClk );

	//
	// Выполнить запись в регистры кристалла АЦП напрямую
	//
	U32			val;
	int			maxii = 20;
	S32			indRegNo;

	for( ii = 0; ii < 16; ii++ )
	{
		BRDC_sprintf(ParamName, _BRDC("AdcReg%d"), ii);
		ret = GetPrivateProfileString(SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
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

	ret = GetPrivateProfileString(SectionName, _BRDC("GenChipAdr"), _BRDC("0x55"), Buffer, sizeof(Buffer), iniFilePath);
	if( 0 != ret )
		genChipAdr = BRDC_strtol( Buffer, &endptr, 0 );

	for( ii = 0; ii < sizeof(genAdr)/sizeof(genAdr[0]); ii++ )
	{
		//
		// Считать значение для записи в регистр генератора из ини-файла
		//
		BRDC_sprintf( ParamName, _BRDC("GenReg%d"), genAdr[ii] );
		ret = GetPrivateProfileString( SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
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
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_I2C_DATAH, (U32)0 );
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
		ret = GetPrivateProfileString(SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
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
int CAdc210x1gSrv::SaveProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	BRDCHAR				ParamName[128];
	ADC210X1GSRV_Cfg	*pSrvCfg = (ADC210X1GSRV_Cfg*)m_pConfig;

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
	WriteInifileParam(iniFilePath, SectionName, _BRDC("DoubleFreq"), (ULONG)pSrvCfg->nDoubleFreq, 10, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("DoubleFreqBias1"), pSrvCfg->dDoubleFreqBias1, 4, NULL);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("DoubleFreqRange1"), pSrvCfg->dDoubleFreqRange1, 4, NULL);

	// the function flushes the cache
	WritePrivateProfileString(NULL, NULL, NULL, iniFilePath);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::SetClkSource( CModule *pModule, ULONG nClkSrc )
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
	Sleep(100);
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, fsrc.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::GetClkSource( CModule *pModule, ULONG &refClkSrc )
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
int CAdc210x1gSrv::SetClkInv( CModule *pModule, ULONG nInv )
{
	ADM2IF_FSRC		fsrc;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &fsrc.AsWhole );
	fsrc.ByBits.Inv = nInv;
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, fsrc.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::GetClkInv( CModule *pModule, ULONG &refInv )
{
	ADM2IF_FSRC		fsrc;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &fsrc.AsWhole );
	refInv = fsrc.ByBits.Inv;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::SetClkValue( CModule *pModule, ULONG ClkSrc, double &refClkValue )
{
	PADC210X1GSRV_Cfg pAdcSrvCfg = (PADC210X1GSRV_Cfg)m_pConfig;
	switch(ClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			U32		nStartClkSl;

			GetStartClkSl(pModule, &nStartClkSl);
			if( nStartClkSl == 0 )
				refClkValue = 0.0;
			break;
		case BRDclks_ADC_SUBGEN0:
			refClkValue = pAdcSrvCfg->nRefGen0;
			//ClkValue = pAdcSrvCfg->PllRefGen;
			//OnPll(pModule, ClkValue, pAdcSrvCfg->PllRefGen);
			break;
		case BRDclks_ADC_SUBGEN1:			// SubModule Clock (internal reference)
			refClkValue = pAdcSrvCfg->nRefGen1;
			break;
		case BRDclks_ADC_EXTCLK:		// External clock
			pAdcSrvCfg->SubExtClk = ROUND(refClkValue);
			break;
		default:
	//		refClkValue = 0.0;
			break;
	}
	return BRDerr_OK;
}
	
//***************************************************************************************
int CAdc210x1gSrv::GetClkValue( CModule *pModule, ULONG ClkSrc, double &refClkValue )
{
	PADC210X1GSRV_Cfg pAdcSrvCfg = (PADC210X1GSRV_Cfg)m_pConfig;
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
			ClkVal = pAdcSrvCfg->nRefGen0;
			break;
		case BRDclks_ADC_SUBGEN1:
			ClkVal = pAdcSrvCfg->nRefGen1;
			break;
		case BRDclks_ADC_EXTCLK:		// External clock
			ClkVal = pAdcSrvCfg->SubExtClk;
			break;
		default:
			ClkVal = 0.0;
			break;
	}
	refClkValue = ClkVal;
	return BRDerr_OK;
}
	
//***************************************************************************************
int CAdc210x1gSrv::SetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;
	CAdcSrv::SetClkLocation(pModule, refMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::GetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;

	return BRDerr_OK;
}

//****************************************************************************************
static double DeltaFreq;
int M210X1G_CLK_DIVIDER[BRD_CLKDIVCNT] = {1, 2, 4, 8, 16};

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
		if(MaxDivider >= M210X1G_CLK_DIVIDER[i])
			break;
	
	int MaxInd = (i < 0) ? 0 : i;

	MaxDivider = M210X1G_CLK_DIVIDER[MaxInd];
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
				return M210X1G_CLK_DIVIDER[MaxInd];
			}
		}
		else 
		{
			for(int i = 0; i < MaxInd; i++) 
			{
				double RateLeft = curClk / M210X1G_CLK_DIVIDER[i];
				double RateRight = curClk / M210X1G_CLK_DIVIDER[i+1];

				if(RateHz <= RateLeft && RateHz >= RateRight)
				{
					if(RateLeft - RateHz <= RateHz - RateRight)
					{
						if (DeltaFreq > RateLeft - RateHz)
						{
							DeltaFreq = RateLeft - RateHz;
							return M210X1G_CLK_DIVIDER[i];
						}
					}
					else
					{
						if (DeltaFreq > RateHz - RateRight)
						{
							DeltaFreq = RateHz - RateRight;
							return M210X1G_CLK_DIVIDER[i+1];
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
int CAdc210x1gSrv::SetRate( CModule *pModule, double &refRate, ULONG nClkSrc, double dClkValue )
{
	U32					nAdcRateDivider;
	ADC210X1GSRV_Cfg	*pSrvCfg = (ADC210X1GSRV_Cfg*)m_pConfig;

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
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FDVR, (U32)powof2(nAdcRateDivider) );
	refRate = dClkValue / nAdcRateDivider;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::GetRate( CModule *pModule, double &refRate, double ClkValue )
{
	U32			val;
	U32			nAdcRateDivider;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FDVR, &val );
	nAdcRateDivider = (U32)pow( 2.0, (int)val );
	refRate = ClkValue / nAdcRateDivider;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::SetInpSrc( CModule *pModule, PVOID pCtrl )
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
int CAdc210x1gSrv::GetInpSrc( CModule *pModule, PVOID pCtrl )
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
int CAdc210x1gSrv::SetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	ADC210X1GSRV_Cfg  *pSrvCfg  = (ADC210X1GSRV_Cfg*)m_pConfig;

	//
	// Получить режим работы входного тракта выбранного канала
	//
	int Status = BRDerr_OK;
	double inp_range;
	Status = GetInpRange(pModule, inp_range, Chan); 
	if(Status != BRDerr_OK)
		return Status;
	double prec_range = inp_range / 10; // 10% from full range

	//
	// Скорректировать запрашиваемое смещение нуля
	//
	refBias *= -1.0;

	//
	// Рассчитать коды для ИПНов
	//
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
	refBias *= -1.0;


	//
	// Сохранить результаты расчета в m_pConfig
	//
	pSrvCfg->aBias[Chan] = refBias;
	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC210X1G_BIAS0 - 1] = (UCHAR)approx_data;
//	pSrvCfg->AdcCfg.ThrDac[Chan + BRDtdn_ADC210X1G_PRECBIAS0 - 1] = (UCHAR)prec_data;

	MAIN_THDAC thDac;

	thDac.ByBits.Data = approx_data;
	thDac.ByBits.Num  = Chan + BRDtdn_ADC210X1G_BIAS0;
	IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, thDac.AsWhole );

//	thDac.ByBits.Data = prec_data;
//	thDac.ByBits.Num  = Chan + BRDtdn_ADC210X1G_PRECBIAS0;
//	IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, thDac.AsWhole );

	return Status;
}

//***************************************************************************************
int CAdc210x1gSrv::GetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	ADC210X1GSRV_Cfg  *pSrvCfg  = (ADC210X1GSRV_Cfg*)m_pConfig;

	refBias = pSrvCfg->aBias[Chan];

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::SetStartMode( CModule *pModule, PVOID pStartStopMode )
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
int CAdc210x1gSrv::GetStartMode( CModule *pModule, PVOID pStartStopMode )
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
int CAdc210x1gSrv::SetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	ADC210X1GSRV_Cfg	*pSrvCfg  = (ADC210X1GSRV_Cfg*)m_pConfig;
	int					status;
	double				inpRange;

	inpRange = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / refGain;
	status = SetInpRange( pModule, inpRange, Chan );
	if( status >= 0 )
		refGain = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / inpRange;

	return status;
}

//***************************************************************************************
int CAdc210x1gSrv::GetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	ADC210X1GSRV_Cfg	*pSrvCfg  = (ADC210X1GSRV_Cfg*)m_pConfig;
	int					status;
	double				inpRange;

	status = GetInpRange( pModule, inpRange, Chan );
	if( status >= 0 )
		refGain = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / inpRange;

	return status;
}

//***************************************************************************************
int CAdc210x1gSrv::SetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	ADC210X1GSRV_Cfg  *pSrvCfg  = (ADC210X1GSRV_Cfg*)m_pConfig;
	ULONG				mask;
	int					ii;

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
int CAdc210x1gSrv::GetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	ADC210X1GSRV_Cfg	*pSrvCfg  = (ADC210X1GSRV_Cfg*)m_pConfig;
	ADM2IF_GAIN			gain;
	double				value;

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
int CAdc210x1gSrv::SetCode( CModule *pModule, ULONG type )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::GetCode( CModule *pModule, ULONG &refType )
{
	refType = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::SetMaster( CModule *pModule, ULONG mode )
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
int CAdc210x1gSrv::SetAdcStartMode( CModule *pModule, ULONG source, ULONG inv, double *pCmpThr )
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
	PADC210X1GSRV_Cfg pSrvCfg = (PADC210X1GSRV_Cfg)m_pConfig;
	double max_thr = pSrvCfg->AdcCfg.RefPVS / 1000.;

	double value = *pCmpThr;
	int max_code = 255;//ADC210X1G_STARTEXTMAXCODE;
	int min_code = 0;//ADC210X1G_STARTEXTMINCODE;
	if(BRDsts_ADC_CHAN0 == source || BRDsts_ADC_CHAN1 == source)
	{
		//double koef = ADC210X1G_STARTCHANKOEF;
		//max_code = ADC210X1G_STARTCHANMAXCODE;
		//min_code = ADC210X1G_STARTCHANMINCODE;
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
		//double koef = ADC210X1G_STARTCHANKOEF;
		//*pCmpThr = value * inp_range / koef;
		*pCmpThr = (value * inp_range) / max_thr;
	}

	//
	// Сохранить рассчитанные значения в структуре
	//
	pSrvCfg->StCmpThr = *pCmpThr;
	pSrvCfg->AdcCfg.ThrDac[BRDtdn_ADC210X1G_STARTCMP - 1] = (UCHAR)dac_data;

	//
	// Записать код в ИПН7
	//
	MAIN_THDAC thDac;

	thDac.ByBits.Data = dac_data;
	thDac.ByBits.Num = BRDtdn_ADC210X1G_STARTCMP;
	IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, thDac.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::GetAdcStartMode( CModule *pModule, PULONG pSource, PULONG pInv, double *pCmpThr )
{
	ADC210X1GSRV_Cfg	*pSrvCfg = (ADC210X1GSRV_Cfg*)m_pConfig;
	ADC_CTRL regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	*pSource = regCtrl.ByBits.StartSrc;
	*pInv    = regCtrl.ByBits.StartInv;

	*pCmpThr = pSrvCfg->StCmpThr;

	return BRDerr_OK;
}

//***************************************************************************************
//int CAdc210x1gSrv::ExtraInit(CModule* pModule)
//{
//	PADC210X1GSRV_Cfg pAdcSrvCfg = (PADC210X1GSRV_Cfg)m_pConfig;
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
int CAdc210x1gSrv::SetSpecific( CModule *pModule, BRD_AdcSpec *pSpec )
{
	int status;

	status = CAdcSrv::SetSpecific(pModule, pSpec);

	switch(pSpec->command)
	{
		case ADC210X1Gcmd_SETMU:
			status = SetMu(pModule, pSpec->arg);
			break;
		case ADC210X1Gcmd_GETMU:
			status = GetMu(pModule, pSpec->arg);
			break;
		case ADC210X1Gcmd_SETSTARTCLKSL:
			status = SetStartClkSl(pModule, pSpec->arg);
			break;
		case ADC210X1Gcmd_GETSTARTCLKSL:
			status = GetStartClkSl(pModule, pSpec->arg);
			break;
		case ADC210X1Gcmd_SETCDBLCLKEX:
			status = SetDblClkEx(pModule, pSpec->arg);
			break;
		case ADC210X1Gcmd_GETCDBLCLKEX:
			status = GetDblClkEx(pModule, pSpec->arg);
			break;
	}
	return status;
}

//***************************************************************************************
int CAdc210x1gSrv::SetMu( CModule *pModule, void *args )
{
	PADC210X1GSRV_MU pMU = (PADC210X1GSRV_MU)args;

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
int CAdc210x1gSrv::GetMu( CModule *pModule, void *args )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::SetStartClkSl( CModule *pModule, void *args )
{
	U32			*pParam = (U32*)args;
	ADC_CTRL	ctrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &ctrl.AsWhole );
	ctrl.ByBits.StartSl = (*pParam) & 0x1;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, ctrl.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::GetStartClkSl( CModule *pModule, void *args )
{
	U32			*pParam = (U32*)args;
	ADC_CTRL	ctrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &ctrl.AsWhole );
	*pParam = ctrl.ByBits.StartSl;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::SetDblClkEx(CModule* pModule, void *args)
{
	ADC210X1GSRV_DBLCLK		*pDblClk = (ADC210X1GSRV_DBLCLK*)args;
	double		aInpRange[2];
	double		aInpBias[2];
	double		val, maxVal;
	U32			cod, adr;
	int			ii;

	if( pDblClk->size != sizeof(ADC210X1GSRV_DBLCLK) )
		return BRDerr_BAD_PARAMETER;

	//
	// Выключить режим удвоения частоты
	//

	adr = 0x40;
	//IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGADDR, adr );
	//IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGDATA, (U32)0x200 );
	//Sleep(10);
	//IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGADDR, adr );
	//IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGDATA, (U32)0x2000 );
	//Sleep(10);

	if( pDblClk->isDblClk == 0 )
	{
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGADDR, adr );
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGDATA, (U32)0x2000 );
		
		return BRDerr_OK;
	}

	//
	// Включить режим удвоения частоты
	//
	SetChanMask( pModule, 0x3 );

	cod = 0x2080 | ((pDblClk->inpSrc==0) ? 0x40 : 0x00);
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGADDR, adr );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGDATA, cod );


	//
	// Установить корректирующее значение 
	// шкалы преобраз. каналов 0 и 1 (%): -24.0 .. +24.0
	//
	maxVal = (pDblClk->valRange != 1) ? 100.0 : 24.05;
	aInpRange[0] = pDblClk->inp0Range;
	aInpRange[1] = pDblClk->inp1Range;

	for( ii=0; ii<2; ii++ )
	{
		if( aInpRange[ii] < -maxVal )
			aInpRange[ii] = -maxVal;
		if( aInpRange[ii] > maxVal )
			aInpRange[ii] = maxVal;

		aInpRange[ii] /= maxVal;
		aInpRange[ii] *= 16384.0;
		aInpRange[ii] += 16384.0;

		cod = (U32)aInpRange[ii];
		adr = (ii==0) ? 0xB : 0x3;
		adr <<= 1;
		adr |= 0x40;
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGADDR, adr );
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGDATA, cod );
	}

	//
	// Установить корректирующее значение 
	// шкалы преобраз. каналов 0 и 1 (%): -24.0 .. +24.0
	// смещения нуля каналов 0 и 1 (%): -5.6 .. +5.6
	//
	maxVal = (pDblClk->valRange != 1) ? 100.0 : 5.696;
	aInpBias[0] = pDblClk->inp0Bias;
	aInpBias[1] = pDblClk->inp1Bias;
	for( ii=0; ii<2; ii++ )
	{
		if( aInpBias[ii] < -maxVal )
			aInpBias[ii] = -maxVal;
		if( aInpBias[ii] > maxVal )
			aInpBias[ii] = maxVal;
		
		val = aInpBias[ii];
		if( val<0.0 )
			val = -val;
		val /= maxVal;
		val *= 4095;

		cod = (U32)val;
		if( aInpBias[ii] < 0.0 )
			cod |= 0x1000;
		//cod = 0xFFFF;
		adr = (ii==0) ? 0xA : 0x2;
		adr <<= 1;
		adr |= 0x40;
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGADDR, adr );
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGDATA, cod );

		//for( ii=0; ; ii++ )
		//{
		//	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGADDR, adr );
		//	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCREGDATA, cod );
		//	Sleep(10);
		//}
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::GetDblClkEx(CModule* pModule, void *args)
{
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc210x1gSrv::GetDblClk(CModule* pModule, ULONG& mode)
{
	ADC210X1GSRV_Cfg	*pSrvCfg = (ADC210X1GSRV_Cfg*)m_pConfig;
	mode = (0==pSrvCfg->nDoubleFreq) ? 0 : 1;

	return BRDerr_OK;
}

// ***************** End of file Adc210x1gSrv.cpp ***********************
