/*
 ***************** File Fm214x250mSrv.cpp ************
 *
 * BRD Driver for ADС service on FM214x250M
 *
 * (C) InSys by Ekkore Nov 2011
 *
 ******************************************************
*/
#ifndef __linux__
#include <windows.h>
#include <winioctl.h>
#endif
#include "module.h"
#include "realdelay.h"
#include "AdcFm214x250mSrv.h"

#define	CURRFILE "FM214X250MSRV"

const double BRD_ADC_MAXGAINTUN	= 1.; // not used

//***************************************************************************************
CFm214x250mSrv::CFm214x250mSrv(int idx, int srv_num, CModule* pModule, FM214X250MSRV_Cfg *pCfg) :
	CAdcSrv(idx, _BRDC("FM214X250M"), srv_num, pModule, pCfg, sizeof(FM214X250MSRV_Cfg))
{
}

//***************************************************************************************
int CFm214x250mSrv::CtrlRelease( void *pDev, void *pServData, ULONG cmd, void *args )
{
	CModule* pModule = (CModule*)pDev;

	SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);

	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб (вместо BRDerr_OK)
}

//***************************************************************************************
void CFm214x250mSrv::GetAdcTetrNum( CModule *pModule )
{
	//
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	//

	//
	// Поиск номера терады. Если тетрада не найдена, то -1.
	//
	m_AdcTetrNum = GetTetrNum(pModule, m_index, FM214X250M_TETR_ID);
}

//***************************************************************************************
void CFm214x250mSrv::FreeInfoForDialog( PVOID pInfo )
{
	FM214X250MSRV_Info *pSrvInfo = (FM214X250MSRV_Info*)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CFm214x250mSrv::GetInfoForDialog( CModule *pModule )
{
	FM214X250MSRV_Cfg  *pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;
	FM214X250MSRV_Info *pInfo = new FM214X250MSRV_Info;
	int					ii;

	pInfo->Size = sizeof(FM214X250MSRV_Info);

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
int CFm214x250mSrv::SetPropertyFromDialog( void *pDev, void *args )
{
	CModule				*pModule = (CModule*)pDev;
	FM214X250MSRV_Info	*pInfo = (FM214X250MSRV_Info*)args;
	FM214X250MSRV_Cfg	*pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;
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
int CFm214x250mSrv::SetProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	FM214X250MSRV_Cfg	*pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;
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
	// Извлечение частоты генератора базового модуля
	//
    IPC_getPrivateProfileString(SectionName, _BRDC("BaseClockValue"), _BRDC("1000.0"), Buffer, sizeof(Buffer), iniFilePath);
	clkValue = BRDC_atof(Buffer);
	SetClkValue(pModule, BRDclks_ADC_BASEGEN, clkValue);

	//
	// Установление частоты генератора Si571 для работы синтезатора в режиме PLL
	//
    IPC_getPrivateProfileString(SectionName, _BRDC("SubClockValue"), _BRDC("1000.0"), Buffer, sizeof(Buffer), iniFilePath);
	clkValue = BRDC_atof(Buffer);
	SetClkValue(pModule, BRDclks_ADC_SUBGEN_PLL, clkValue);

	//
	// Установка делителей синтезетора AD9518  для работы синтезатора в режиме PLL
	// (значения делителей задаются напрямую в ини-файле)
	//
	AD9518_TDividers	rDividers;
	int					isPllDividers = 0;

    IPC_getPrivateProfileString(SectionName, _BRDC("AD9518DividerA"), _BRDC("-1"), Buffer, sizeof(Buffer), iniFilePath);
	rDividers.divA = BRDC_atoi(Buffer);
    IPC_getPrivateProfileString(SectionName, _BRDC("AD9518DividerB"), _BRDC("-1"), Buffer, sizeof(Buffer), iniFilePath);
	rDividers.divB = BRDC_atoi(Buffer);
    IPC_getPrivateProfileString(SectionName, _BRDC("AD9518DividerP"), _BRDC("-1"), Buffer, sizeof(Buffer), iniFilePath);
	rDividers.divP = BRDC_atoi(Buffer);
    IPC_getPrivateProfileString(SectionName, _BRDC("AD9518DividerR"), _BRDC("-1"), Buffer, sizeof(Buffer), iniFilePath);
	rDividers.divR = BRDC_atoi(Buffer);
    IPC_getPrivateProfileString(SectionName, _BRDC("AD9518DividerDo"), _BRDC("-1"), Buffer, sizeof(Buffer), iniFilePath);
	rDividers.divDo = BRDC_atoi(Buffer);
    IPC_getPrivateProfileString(SectionName, _BRDC("AD9518DividerPll"), _BRDC("-1"), Buffer, sizeof(Buffer), iniFilePath);
	rDividers.divPll = BRDC_atoi(Buffer);
    IPC_getPrivateProfileString(SectionName, _BRDC("AD9518DividerRate"), _BRDC("-1"), Buffer, sizeof(Buffer), iniFilePath);
	rDividers.rate = BRDC_atof(Buffer);

	if( (0<=rDividers.divA) && (0<=rDividers.divP) && (0<=rDividers.divDo) &&
		(0<=rDividers.divB) && (0<=rDividers.divR) && (0<=rDividers.divPll) &&
		(0<=rDividers.rate) 
	  )
		isPllDividers = 1;
	if( (BRDclks_ADC_SUBGEN_PLL != clkSrc) && (BRDclks_ADC_EXTCLK_PLL != clkSrc) )
		isPllDividers = 0;
	if( isPllDividers )
		if( 0<=Ad9518SetDirectPllMode( pModule, &rDividers ) )
			pSrvCfg->dSamplingRate = rDividers.rate;

	//
	// Извлечение требуемой частоты дискретизации
	//
	double			rate;
	
    IPC_getPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("100000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	rate = BRDC_atof(Buffer);
	if( 0 == isPllDividers )
		SetRate(pModule, rate, clkSrc, clkValue);

	//
	// Извлечение маски каналов
	//
	ULONG chanMask;

    IPC_getPrivateProfileString(SectionName, _BRDC("ChannelMask"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
	chanMask = BRDC_strtol(Buffer, &endptr, 0);
	SetChanMask(pModule, chanMask);

	//
	// Извлечение порога для внешней тактовой частоты
	//
	//double			thr;
	//
    //IPC_getPrivateProfileString(SectionName, _BRDC("ExternalClockThreshold"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	//thr = BRDC_atof(Buffer);
	//SetExtClkThr(pModule, thr );

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

	rStMode.inv = 0;

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
int CFm214x250mSrv::SaveProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	BRDCHAR				ParamName[128];
//	FM214X250MSRV_Cfg	*pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;

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
int CFm214x250mSrv::GetCfg(PBRD_AdcCfg pCfg)
{
//	FM214X250MSRV_Cfg	*pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;

	CAdcSrv::GetCfg(pCfg);
	pCfg->ChanType = 1;				// Коэф. передачи измеряется в дБ

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetClkSource( CModule *pModule, ULONG nClkSrc )
{
	FM214X250MSRV_Cfg	*pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;
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
			regFsrc.ByBits.Gen    = 0x1;
			regCtrl.ByBits.RefSel = 0x1;
			break;
		case BRDclks_ADC_BASEGEN:
			regFsrc.ByBits.Gen    = 0x85;
			regCtrl.ByBits.RefSel = 0x0;
			break;
		case BRDclks_ADC_EXTCLK:
			regFsrc.ByBits.Gen    = 0x81;
			regCtrl.ByBits.RefSel = 0x0;
			break;
		case BRDclks_ADC_EXTCLK_PLL:
			regFsrc.ByBits.Gen    = 0x81;
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
    //IPC_delay(100);
	RealDelay(100,1);
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, regFsrc.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::GetClkSource( CModule *pModule, ULONG &refClkSrc )
{
	FM214X250MSRV_Cfg	*pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;

	refClkSrc = pSrvCfg->nClkSrc;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetClkValue( CModule *pModule, ULONG ClkSrc, double &refClkValue )
{
	FM214X250MSRV_Cfg	*pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;
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
		case BRDclks_ADC_BASEGEN:
			pSrvCfg->nBaseUnitClk = ROUND(refClkValue);
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
int CFm214x250mSrv::GetClkValue( CModule *pModule, ULONG ClkSrc, double &refClkValue )
{
	FM214X250MSRV_Cfg	*pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;
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
		case BRDclks_ADC_BASEGEN:
			refClkValue = pSrvCfg->nBaseUnitClk;
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
int CFm214x250mSrv::SetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;
	CAdcSrv::SetClkLocation(pModule, refMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::GetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetExtClkThr(CModule* pModule, double& ClkThr)
{
	//FM214X250MSRV_Cfg  *pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;

	//double  dTmp;
	//U32		code;

	//if( ClkThr > 2.5 ) ClkThr = 2.5;
	//if( ClkThr < -2.5 ) ClkThr = -2.5;
	//dTmp  = (ClkThr + 2.5) / 5.0;
	//dTmp *= 4096.0;
	//code  = (U32)dTmp;
	//SpdWrite( pModule, 1, 8, code );

	//pSrvCfg->dExtClkThr = ClkThr;

	//return BRDerr_OK;
	return BRDerr_CMD_UNSUPPORTED;
}

//***************************************************************************************
int CFm214x250mSrv::GetExtClkThr(CModule* pModule, double& ClkThr)
{
	FM214X250MSRV_Cfg  *pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;

	ClkThr = pSrvCfg->dExtClkThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetRate( CModule *pModule, double &refRate, ULONG nClkSrc, double ClkValue )
{
	FM214X250MSRV_Cfg	*pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;
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
				dClkValue = pSrvCfg->nGenRef;
				err = Ad9518SetDividerMode( pModule, &dClkValue, &refRate );
			}
			else
			{
				double		dClkValueRd;

				//
				// Рассчитать тактовую частоту, формируемую генератором Si570/Si571
				//
				dClkValue = refRate;
				if( dClkValue > pSrvCfg->nGenRefMax )
					dClkValue = pSrvCfg->nGenRefMax;
				if( dClkValue < 10000000.0 )
				{
					for( int divPll=1; divPll<=6; divPll++ )
					{
						for( int divDo=1; divDo<=32; divDo++ )
						{	if( (dClkValue*divPll*divDo) >= 10000000.0 )
							{
								dClkValue *= divPll*divDo;
								break;
							}
						}
						if( dClkValue >= 10000000.0 )
							break;
					}
				}
				if( dClkValue < 10000000.0 )
					dClkValue *= 6*32;

				//
				// Записать тактову частоту в генератор Si570/Si571
				//
				Si571GetRate( pModule, &dClkValueRd ); // Считать Si570/Si571
				if( dClkValueRd != dClkValue )
					err = Si571SetRate( pModule, &dClkValue ); // Запрограммировать Si570/Si571
				Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571

				//
				// Запрограммировать делители синтезатора AD9518
				//
				err = Ad9518SetDividerMode( pModule, &dClkValue, &refRate );
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
			err = Ad9518SetPllMode( pModule, &dClkValue, &refRate );
			break;
		case BRDclks_ADC_BASEGEN:
			dClkValue = pSrvCfg->nBaseUnitClk;
			err = Ad9518SetDividerMode( pModule, &dClkValue, &refRate );
			break;
		case BRDclks_ADC_EXTCLK:
			dClkValue = pSrvCfg->nSubExtClk;
			err = Ad9518SetDividerMode( pModule, &dClkValue, &refRate );
			break;
		case BRDclks_ADC_EXTCLK_PLL:
			dClkValue = pSrvCfg->nSubExtClk;
			err = Ad9518SetPllMode( pModule, &dClkValue, &refRate );
			break;
		default:
			refRate = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}
	if( err>=0 )
		pSrvCfg->dSamplingRate = refRate;

/*
	SetClkSource( pModule, nClkSrc );
	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			refRate = 0.0;
			break;
		case BRDclks_ADC_SUBGEN:
			double		dClkValue;

			Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
			if( dClkValue != refRate )
			{
				dClkValue = refRate;
				err = Si571SetRate( pModule, &dClkValue ); // Запрограммировать Si570/Si571
				refRate = dClkValue;
			}
			break;
		case BRDclks_ADC_EXTCLK:		// External clock
			refRate = pAdcSrvCfg->nSubExtClk;
			break;
		default:
			refRate = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}
*/
	return err;
}

//***************************************************************************************
int CFm214x250mSrv::GetRate( CModule *pModule, double &refRate, double ClkValue )
{
	FM214X250MSRV_Cfg	*pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;
/*
	ULONG				nClkSrc;

	GetClkSource( pModule, nClkSrc );

	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			refRate = 0.0;
			break;
		case BRDclks_ADC_SUBGEN:
			double		dClkValue ;

			Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
			refRate = dClkValue;
			break;
		case BRDclks_ADC_EXTCLK:		// External clock
			refRate = pAdcSrvCfg->nSubExtClk;
			break;
		default:
			refRate = 0.0;
			break;
	}
*/

	refRate = pSrvCfg->dSamplingRate;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	FM214X250MSRV_Cfg  *pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;
	double  dTmp;
	U32		code;


	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	if( refBias > 100.0 )  refBias = 100.0;
	if( refBias < -100.0 ) refBias = -100.0;
	dTmp  = refBias / 100.0;
	dTmp *= 32767.0;
	code  = (U32)floor(dTmp);
	SpdWrite( pModule, SPDdev_THDAC, 2+Chan, code );

	pSrvCfg->aBias[Chan] = refBias;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::GetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	FM214X250MSRV_Cfg  *pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;

	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	refBias = pSrvCfg->aBias[Chan];

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetDcCoupling(CModule* pModule, ULONG InpType, ULONG Chan)
{
	U32			val, mask;

	if( Chan > MAX_ADC_CHAN )
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
int CFm214x250mSrv::GetDcCoupling(CModule* pModule, ULONG& InpType, ULONG Chan)
{
	U32			val, mask;

	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;
	mask = (Chan==0) ? 0x2 : 0x20;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_INP, &val );
	InpType = (val&mask) ? 1 :0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan)
{
	U32			val, mask;

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

	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;
	mask = (Chan==0) ? 0x1 : 0x10;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_INP, &val );
	if( InpRes )
		val |= mask;
	else
		val &= ~mask;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_INP, val );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan)
{
	U32			val, mask;

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

	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;
	mask = (Chan==0) ? 0x1 : 0x10;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_INP, &val );
	InpRes = (val&mask) ? 1 :0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetStartMode( CModule *pModule, PVOID pStartStopMode )
{
	int Status = BRDerr_OK;
	PBRD_AdcStartMode pAdcStartMode = (PBRD_AdcStartMode)pStartStopMode;
	double thr = pAdcStartMode->level;

	Status = SetAdcStartMode(pModule, pAdcStartMode->src, 0, &thr);
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
int CFm214x250mSrv::GetStartMode( CModule *pModule, PVOID pStartStopMode )
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
int CFm214x250mSrv::SetGain( CModule *pModule, double &refGain, ULONG chanNo )
{
	FM214X250MSRV_Cfg	*pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;
	S32					codeAtt = 0;
	S32					codeAdc = 0x50;

	if( chanNo >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	if( refGain < 0 )
	{

		codeAtt = (S32)(( -refGain + 0.25) * 2);
		if( codeAtt < 0 )
			codeAtt = 0;
		if( codeAtt > 63 )
			codeAtt = 63;
		refGain = -((double)codeAtt) / 2.0;
	}
	if( refGain > 0 )
	{

		codeAdc = (S32)(( refGain + 0.25) * 2);
		if( codeAdc < 0 )
			codeAdc = 0;
		if( codeAdc > 7 )
			codeAdc = 7;
		refGain = ((double)codeAdc) / 2.0;
		codeAdc += 5;
		codeAdc <<= 4;
	}

	if( chanNo == 0 )
	{
		SpdWrite( pModule, SPDdev_ADC0, 0x25, codeAdc );
		SpdWrite( pModule, SPDdev_ATT0, 0, codeAtt );
	}
	else
	{
		SpdWrite( pModule, SPDdev_ADC1, 0x25, codeAdc );
		SpdWrite( pModule, SPDdev_ATT1, 0, codeAtt );
	}
	pSrvCfg->adGainDb[chanNo] = refGain;
	
	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::GetGain( CModule *pModule, double &refGain, ULONG chanNo )
{
	FM214X250MSRV_Cfg	*pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;

	if( chanNo >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;
	
	refGain = pSrvCfg->adGainDb[chanNo];

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetInpRange( CModule *pModule, double &refInpRange, ULONG chanNo )
{
	FM214X250MSRV_Cfg  *pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;
	int					status;
	double				gainDB, tmp;

	tmp  = (double)(pSrvCfg->AdcCfg.InpRange) / 1000.0;
	tmp /= refInpRange;
	gainDB  = log10( tmp );
	gainDB *= 20.0;

	status = SetGain( pModule, gainDB, chanNo );

	refInpRange = (double)(pSrvCfg->AdcCfg.InpRange) / pow(10.0, gainDB/20.0) / 1000.0;

	return status;
}

//***************************************************************************************
int CFm214x250mSrv::GetInpRange( CModule *pModule, double &refInpRange, ULONG chanNo )
{
	FM214X250MSRV_Cfg	*pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;
	double				gainDB;

	if( chanNo >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	gainDB = pSrvCfg->adGainDb[chanNo];
	refInpRange = (double)(pSrvCfg->AdcCfg.InpRange) /  pow(10.0, gainDB/20.0) / 1000.0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetCode( CModule *pModule, ULONG type )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::GetCode( CModule *pModule, ULONG &refType )
{
	refType = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetMaster( CModule *pModule, ULONG mode )
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
int CFm214x250mSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	int			err;

	err = CAdcSrv::SetChanMask( pModule, mask);

	//
	// Включить/выключить питание АЦП
	//
	if( mask & 0x2 )
	{
		SpdWrite( pModule, SPDdev_ADC0, 0x43, 0x00 );
		SpdWrite( pModule, SPDdev_ADC1, 0x43, 0x00 );
	}
	else
	{
		SpdWrite( pModule, SPDdev_ADC1, 0x43, 0x04 );
		if( mask & 0x1 )
			SpdWrite( pModule, SPDdev_ADC0, 0x43, 0x00 );
		else
			SpdWrite( pModule, SPDdev_ADC0, 0x43, 0x04 );
	}
	RealDelay( 10, 1 );

	return err;
}

//***************************************************************************************
int CFm214x250mSrv::ExtraInit(CModule* pModule)
{
	//
	// Начальная инициализация программируемых микросхем: АЦП, ИПН(ЦАП), генератора
	//
	FM214X250MSRV_Cfg	*pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;
	int					ii;

	RealDelay( 300, 1 );

	//
	// Инициализация АЦП
	//
	SetChanMask( pModule, 0x3 );
	SpdWrite( pModule, SPDdev_ADC0, 0x03, 0x03 );
	//SpdWrite( pModule, SPDdev_ADC0, 0x4A, 0x01 );
	SpdWrite( pModule, SPDdev_ADC1, 0x03, 0x03 );
	//SpdWrite( pModule, SPDdev_ADC1, 0x4A, 0x01 );

	//
	// Инициализация ИПН(ЦАП)
	//
	int			regAdr;

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

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm214x250mSrv::SpdRead(  CModule* pModule, U32 spdType, U32 regAdr, U32 *pRegVal )
{
	//
	// Типы SPD (spdType): 
	//   0x00 - АЦП0, 0x10 - АЦП1, 0x01 - ИПН(ЦАП), 0x02 - генератор 
	//   0x03 - синтезатор, 0x04 - атт0, 0x14 - атт1
	//
	FM214X250MSRV_Cfg	*pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;
	U32			spdCtrl;
	U32			status;

	spdCtrl  = 0x1;
	switch(spdType)
	{
		case SPDdev_GEN:	spdCtrl |= (pSrvCfg->nGenAdr & 0xFF) << 4;	break;
		default:			spdCtrl |= spdType & 0xF0;
	}
	//spdCtrl |= (spdType==SPDdev_GEN) ? 0x490 : spdType&0xF0; 

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

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm214x250mSrv::SpdWrite( CModule* pModule, U32 spdType, U32 regAdr, U32 regVal )
{
	FM214X250MSRV_Cfg	*pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;
	U32			spdCtrl;
	U32			status;

	spdCtrl  = 0x2;
	switch(spdType)
	{
		case SPDdev_GEN:	spdCtrl |= (pSrvCfg->nGenAdr & 0xFF) << 4;	break;
		default:			spdCtrl |= spdType & 0xF0;
	}
	//spdCtrl |= (spdType==SPDdev_GEN) ? 0x490 : spdType&0xF0; 
	for(;;)
	{
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DEVICE, spdType & 0xF );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_ADDR, regAdr );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DATA, regVal );
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
int	CFm214x250mSrv::Si571SetRate( CModule* pModule, double *pRate )
{
	FM214X250MSRV_Cfg	*pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;
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

	return status;	
}

//***************************************************************************************
int	CFm214x250mSrv::Si571GetRate( CModule* pModule, double *pRate )
{
	FM214X250MSRV_Cfg	*pSrvCfg  = (FM214X250MSRV_Cfg*)m_pConfig;
	U32			regData[20];
	//U32			rfreqLo, rfreqHi, hsdivCode, n1Code;
	//double		freqTmp;
	//double		dRFreq, dHsdiv, dN1;
	ULONG		clkSrc;
	int			regAdr;

	*pRate = 0.0;

	//
	// Проверить источник частоты
	//

// 	GetClkSource( pModule, clkSrc );
// 	if( clkSrc != BRDclks_ADC_SUBGEN )
//         return BRDerr_ERROR;

	//
	// Считать регистры Si570/Si571
	//
	for( regAdr=7; regAdr<=12; regAdr++ )
		SpdRead( pModule, SPDdev_GEN, regAdr, &regData[regAdr] );

	SI571_GetRate( pRate, pSrvCfg->dGenFxtal, regData );

	return BRDerr_OK;	
}

//***************************************************************************************
int	CFm214x250mSrv::Ad9518SetDividerMode( CModule* pModule, double *pClk, double *pRate )
{
	AD9518_TReg		regs[24];
	S32				realSize;
	int				ii;
	S32				err;

	err = AD9518_DividerMode( pClk, pRate, regs, 24, &realSize );
	if( 0 > err)
		return BRDerr_BAD_PARAMETER;

	//
	// Запрограммировать синтезатор 
	//
	for( ii=0; ii<realSize; ii++ )
		SpdWrite( pModule, SPDdev_SYNT, regs[ii].adr, regs[ii].val );


	return BRDerr_OK;	
}

//***************************************************************************************
int	CFm214x250mSrv::Ad9518SetPllMode( CModule* pModule, double *pFref, double *pRate )
{
	FM214X250MSRV_Cfg *pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;
	AD9518_TReg		regs[36];
	S32				realSize;
	int				ii;
	S32				err;

	err = AD9518_PllMode( pFref, pRate, regs, 36, &realSize, pSrvCfg->nSyntType );
	if( 0 > err)
		return BRDerr_BAD_PARAMETER;

	//
	// Запрограммировать синтезатор 
	//
	for( ii=0; ii<realSize; ii++ )
		SpdWrite( pModule, SPDdev_SYNT, regs[ii].adr, regs[ii].val );

	//
	// Калибровка ГУН
	//
    IPC_delay(100);
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x01 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(100);
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x00 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(100);
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x01 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(400);

	SpdWrite( pModule, SPDdev_SYNT, 0x0F0, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F1, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F2, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F3, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F4, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F5, 0x04);

	return BRDerr_OK;	
}

//***************************************************************************************
int	CFm214x250mSrv::Ad9518SetDirectPllMode( CModule* pModule, AD9518_TDividers *pDividers )
{
	AD9518_TReg		regs[36];
	S32				realSize;
	int				ii;
	S32				err;

	err = AD9518_DirectPllMode( pDividers, regs, 36, &realSize );
	if( 0 > err)
		return BRDerr_BAD_PARAMETER;

	//
	// Запрограммировать синтезатор 
	//
	for( ii=0; ii<realSize; ii++ )
		SpdWrite( pModule, SPDdev_SYNT, regs[ii].adr, regs[ii].val );


	//
	// Калибровка ГУН
	//
    IPC_delay(100);
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x01 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(100);
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x00 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(100);
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x01 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(400);

	SpdWrite( pModule, SPDdev_SYNT, 0x0F0, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F1, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F2, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F3, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F4, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F5, 0x04);

	return BRDerr_OK;	
}

//***************************************************************************************
int CFm214x250mSrv::SetAdcStartMode( CModule *pModule, ULONG source, ULONG inv, double *pCmpThr )
{
	FM214X250MSRV_Cfg *pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;

	//
	// Установить источник сигнала старта и инверсию сигнала старта
	//
	ADC_CTRL regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	regCtrl.ByBits.StartSrc = source;
	//regCtrl.ByBits.StartInv = inv;
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

	if( *pCmpThr > +100.0 ) *pCmpThr = +100.0;
	if( *pCmpThr < -100.0 ) *pCmpThr = -100.0;
	if( source == 0x2 )
		code = (S32)( 32767.0 * (*pCmpThr) / 100.0 );	// внешний 
	else
		code = (S32)( 16383.0 * (*pCmpThr) / 100.0 );	// каналы 0 или 1 

	SpdWrite( pModule, SPDdev_THDAC, 1, (U32)code );

	pSrvCfg->dStCmpThr = *pCmpThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::GetAdcStartMode( CModule *pModule, PULONG pSource, PULONG pInv, double *pCmpThr )
{
	FM214X250MSRV_Cfg	*pSrvCfg = (FM214X250MSRV_Cfg*)m_pConfig;
	ADC_CTRL regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	*pSource = regCtrl.ByBits.StartSrc;
	*pInv    = 0;	//regCtrl.ByBits.StartInv;

	*pCmpThr = pSrvCfg->dStCmpThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm214x250mSrv::SetSpecific( CModule *pModule, BRD_AdcSpec *pSpec )
{
	int status;

	status = CAdcSrv::SetSpecific(pModule, pSpec);

	switch(pSpec->command)
	{
		case FM214X250Mcmd_SETMU:
			status = SetMu(pModule, pSpec->arg);
			break;
		case FM214X250Mcmd_GETMU:
			status = GetMu(pModule, pSpec->arg);
			break;
	}
	return status;
}

//***************************************************************************************
int CFm214x250mSrv::SetMu( CModule *pModule, void *args )
{
	PFM214X250MSRV_MU pMU = (PFM214X250MSRV_MU)args;

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
		SetInpResist(pModule, pMU->inpFilter[i], i);
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
int CFm214x250mSrv::GetMu( CModule *pModule, void *args )
{
	return BRDerr_OK;
}


// ***************** End of file Fm214x250mSrv.cpp ***********************
