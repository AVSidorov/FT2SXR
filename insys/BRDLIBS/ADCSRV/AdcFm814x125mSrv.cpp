/*
 ***************** File Fm814x125mSrv.cpp ************
 *
 * BRD Driver for ADС service on FM814x125M
 *
 * (C) InSys by Ekkore Aug 2011
 *
 ******************************************************
*/

#include "module.h"
#include "realdelay.h"
#include "AdcFm814x125mSrv.h"

#define	CURRFILE "FM814X125MSRV"

const double BRD_ADC_MAXGAINTUN	= 1.; // not used

//***************************************************************************************
CFm814x125mSrv::CFm814x125mSrv(int idx, int srv_num, CModule* pModule, FM814X125MSRV_Cfg *pCfg) :
	CAdcSrv(idx, _BRDC("FM814X125M"), srv_num, pModule, pCfg, sizeof(FM814X125MSRV_Cfg))
{
}

//***************************************************************************************
int CFm814x125mSrv::CtrlRelease( void *pDev, void *pServData, ULONG cmd, void *args )
{
	CModule* pModule = (CModule*)pDev;

	SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);

	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб (вместо BRDerr_OK)
}

//***************************************************************************************
void CFm814x125mSrv::GetAdcTetrNum( CModule *pModule )
{
	//
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	//

	//
	// Поиск номера терады. Если тетрада не найдена, то -1.
	//
	m_AdcTetrNum = GetTetrNum(pModule, m_index, FM814X125M_TETR_ID);
}

//***************************************************************************************
void CFm814x125mSrv::FreeInfoForDialog( PVOID pInfo )
{
	FM814X125MSRV_Info *pSrvInfo = (FM814X125MSRV_Info*)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CFm814x125mSrv::GetInfoForDialog( CModule *pModule )
{
	FM814X125MSRV_Cfg  *pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;
	FM814X125MSRV_Info *pInfo = new FM814X125MSRV_Info;
	int					ii;

	pInfo->Size = sizeof(FM814X125MSRV_Info);

	UCHAR code = pSrvCfg->AdcCfg.Encoding;
	
	pInfo->pAdcInfo = (ADCSRV_INFO*)CAdcSrv::GetInfoForDialog(pModule);
	if(pInfo->pAdcInfo->SyncMode == 3)
		pInfo->pAdcInfo->SyncMode = 1;
	pInfo->pAdcInfo->Encoding = code;
	pInfo->ExtClk = pSrvCfg->SubExtClk;
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

	for( ii=0; ii<8; ii++ )
		pInfo->adRange[ii] = pSrvCfg->adRange[ii];

	GetInpSrc(pModule, &(pInfo->nInputSource) );


	//BRDCHAR		msg[256];
	//BRDC_sprintf( msg, _BRDC("GetInfoForDialog: clkSrc = %d"), pInfo->pAdcInfo->ClockSrc);
	//MessageBox(NULL, msg, _BRDC("Debug"), MB_OK);

	return pInfo;
}

//***************************************************************************************
int CFm814x125mSrv::SetPropertyFromDialog( void *pDev, void *args )
{
	CModule				*pModule = (CModule*)pDev;
	FM814X125MSRV_Info	*pInfo = (FM814X125MSRV_Info*)args;
	FM814X125MSRV_Cfg	*pSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;
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
int CFm814x125mSrv::SetProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	FM814X125MSRV_Cfg	*pSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;
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
	pSrvCfg->SubExtClk = ROUND(clkValue);
	SetClkValue(pModule, 129, clkValue);

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
	// Извлечение порога для внешней тактовой частоты
	//
	double			thr;
	
	IPC_getPrivateProfileString(SectionName, _BRDC("ExternalClockThreshold"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	thr = BRDC_atof(Buffer);
	SetExtClkThr(pModule, thr );

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
		BRDC_sprintf( ParamName, _BRDC("InputRange%d"), ii);
		ret = IPC_getPrivateProfileString( SectionName, ParamName, _BRDC("0.5"), Buffer, sizeof(Buffer), iniFilePath);
		dVal = BRDC_atof(Buffer);
		SetInpRange(pModule, dVal, ii);

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

	IPC_getPrivateProfileString(SectionName, _BRDC("StartLevel"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.level = BRDC_atof(Buffer);

	//GetPrivateProfileString(SectionName, _BRDC("StartInverting"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	//rStMode.inv = BRDC_atoi(Buffer);

	CAdcSrv::GetStartMode(pModule, &rStMode.stndStart);
	//rStMode.stndStart.startSrc = 7; 
	//rStMode.stndStart.trigOn = 1; 
	SetStartMode(pModule, &rStMode);

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
int CFm814x125mSrv::SaveProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	BRDCHAR				ParamName[128];
	FM814X125MSRV_Cfg	*pSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;

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
		GetInpRange(pModule, dVal, i);
		BRDC_sprintf( ParamName, _BRDC("InputRange%d"), i);
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
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartLevel"), start.level, 2, NULL);
	//WriteInifileParam(iniFilePath, SectionName, _BRDC("StartInverting"), (ULONG)start.inv, 10, NULL);
	//WriteInifileParam(iniFilePath, SectionName, _BRDC("StartBaseSource"), (ULONG)start.stndStart.startSrc, 10, NULL);
	//WriteInifileParam(iniFilePath, SectionName, _BRDC("StartBaseInverting"), (ULONG)start.stndStart.startInv, 10, NULL);
	//WriteInifileParam(iniFilePath, SectionName, _BRDC("StartMode"), (ULONG)start.stndStart.trigOn, 10, NULL);
	//WriteInifileParam(iniFilePath, SectionName, _BRDC("StopSource"), (ULONG)start.stndStart.trigStopSrc, 10, NULL);
	//WriteInifileParam(iniFilePath, SectionName, _BRDC("StopInverting"), (ULONG)start.stndStart.stopInv, 10, NULL);
	//WriteInifileParam(iniFilePath, SectionName, _BRDC("ReStart"), (ULONG)start.stndStart.reStartMode, 10, NULL);

	// the function flushes the cache
	IPC_writePrivateProfileString(NULL, NULL, NULL, iniFilePath);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::SetClkSource( CModule *pModule, ULONG nClkSrc )
{
	FM814X125MSRV_Cfg	*pSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;
	ADM2IF_FSRC			fsrc;

	//
	// Учесть доработку на модуле версии 1.0
	//
	if( 0x10==pSrvCfg->nVersion )
		if( nClkSrc >= 0x1 )
			nClkSrc = 0x1;
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
int CFm814x125mSrv::GetClkSource( CModule *pModule, ULONG &refClkSrc )
{
	FM814X125MSRV_Cfg	*pSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;
	ADM2IF_FSRC			fsrc;

	//
	// Определить режим работы тетрады: Single или Slave
	//
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &fsrc.AsWhole );
	refClkSrc = fsrc.ByBits.Gen;

	//
	// Учесть доработку на модуле версии 1.0
	//
	if( 0x10==pSrvCfg->nVersion )
		if( refClkSrc >= 0x1 )
			refClkSrc = 0x81;


	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::SetClkValue( CModule *pModule, ULONG nClkSrc, double &refClkValue )
{
	FM814X125MSRV_Cfg	*pSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;

	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			refClkValue = 0.0;
			break;
		case BRDclks_ADC_SUBGEN:
			double		dClkValue;

			Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
			if( dClkValue != refClkValue )
			{
				dClkValue = refClkValue;
				err = Si571SetRate( pModule, &dClkValue ); // Запрограммировать Si570/Si571
				refClkValue = dClkValue;
			}
			break;
		case BRDclks_ADC_EXTCLK:		// External clock
		case BRDclks_ADC_CLK0_C2M:		// Base Unit Clock CLK0_C2M
		case BRDclks_ADC_CLK1_C2M:		// Base Unit Clock CLK1_C2M
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
int CFm814x125mSrv::GetClkValue( CModule *pModule, ULONG nClkSrc, double &refClkValue )
{
	FM814X125MSRV_Cfg	*pSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;

	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			refClkValue = 0.0;
			break;
		case BRDclks_ADC_SUBGEN:
			double		dClkValue ;

			Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
			refClkValue = dClkValue;
			break;
		case BRDclks_ADC_EXTCLK:		// External clock
		case BRDclks_ADC_CLK0_C2M:		// Base Unit Clock CLK0_C2M
		case BRDclks_ADC_CLK1_C2M:		// Base Unit Clock CLK1_C2M
			refClkValue = pSrvCfg->SubExtClk;
			break;
		default:
			refClkValue = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}

	return err;
}
	
//***************************************************************************************
int CFm814x125mSrv::SetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;
	CAdcSrv::SetClkLocation(pModule, refMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::GetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::SetExtClkThr(CModule* pModule, double& ClkThr)
{
	FM814X125MSRV_Cfg  *pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;

	double  dTmp;
	U32		code;

	if( ClkThr > 2.5 ) ClkThr = 2.5;
	if( ClkThr < -2.5 ) ClkThr = -2.5;
	dTmp  = (ClkThr + 2.5) / 5.0;
	dTmp *= 4096.0;
	code  = (U32)dTmp;
	SpdWrite( pModule, 1, 8, code );

	pSrvCfg->ExtClkThr = ClkThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::GetExtClkThr(CModule* pModule, double& ClkThr)
{
	FM814X125MSRV_Cfg  *pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;

	ClkThr = pSrvCfg->ExtClkThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::SetRate( CModule *pModule, double &refRate, ULONG nClkSrc, double ClkValue )
{
	FM814X125MSRV_Cfg	*pAdcSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;

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
		case BRDclks_ADC_CLK0_C2M:		// Base Unit Clock CLK0_C2M
		case BRDclks_ADC_CLK1_C2M:		// Base Unit Clock CLK1_C2M
			refRate = pAdcSrvCfg->SubExtClk;
			break;
		default:
			refRate = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}
	return err;
}

//***************************************************************************************
int CFm814x125mSrv::GetRate( CModule *pModule, double &refRate, double ClkValue )
{
	FM814X125MSRV_Cfg	*pAdcSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;
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
		case BRDclks_ADC_CLK0_C2M:		// Base Unit Clock CLK0_C2M
		case BRDclks_ADC_CLK1_C2M:		// Base Unit Clock CLK1_C2M
			refRate = pAdcSrvCfg->SubExtClk;
			break;
		default:
			refRate = 0.0;
			break;
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::SetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	FM814X125MSRV_Cfg  *pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;
	int					status = BRDerr_OK;
	double				inputRange;
	double				percent;
	double				thdacVoltage;
	double				dTmp;
	U32					code;


	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	if(0)
	{
		//
		// Временно, для отладки, просто выставить на ИПН указанный уровень
		//
		if( refBias > 2.5 ) refBias = 2.5;
		if( refBias < -2.5 ) refBias = -2.5;
		dTmp  = (refBias + 2.5) / 5.0;
		dTmp *= 4096.0;
		code  = (U32)dTmp;
		SpdWrite( pModule, 1, Chan, code );

		pSrvCfg->aBias[Chan] = refBias;

		return BRDerr_OK;
	}
	//
	// Результаты эксперимента показали следующее соответствие между
	// нпаряжением на выходе ИПН и смещением нуля в канале:
	//    ИПН=-1.07 В   -   СН +100% от шкалы
	//    ИПН=-0.216 В  -   СН +0%
	//    ИПН=+0.64 В   -   СН -1000% от шкалы
	//

	//
	// Получить режим работы входного тракта выбранного канала
	//
	status = GetInpRange(pModule, inputRange, Chan); 
	if( 0>status )
		return status;

	//
	// Определить требуемое напряжение ИПН
	//
	if( refBias >  inputRange ) refBias =  inputRange;
	if( refBias < -inputRange ) refBias = -inputRange;
	percent = refBias * 100.0 / inputRange;
	thdacVoltage = -0.216 - (0.855*percent/100.0);

	//
	// Запрограммировать ИПН на требуемое напряжение
	//
	dTmp  = (thdacVoltage + 2.5) / 5.0;
	dTmp *= 4096.0;
	code  = (U32)dTmp;
	SpdWrite( pModule, 1, Chan, code );

	pSrvCfg->aBias[Chan] = refBias;

	return status;
}

//***************************************************************************************
int CFm814x125mSrv::GetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	FM814X125MSRV_Cfg  *pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;

	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	refBias = pSrvCfg->aBias[Chan];

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::SetDcCoupling(CModule* pModule, ULONG InpType, ULONG Chan)
{
	U32			val, mask;

	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;
	mask = 1<<(1 + 2*Chan);

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_INP, &val );
	if( InpType )
		val |= mask;
	else
		val &= ~mask;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_INP, val );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::GetDcCoupling(CModule* pModule, ULONG& InpType, ULONG Chan)
{
	U32			val, mask;

	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;
	mask = 1<<(1 + 2*Chan);

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_INP, &val );
	InpType = (val&mask) ? 1 :0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan)
{
	U32			val, mask;

	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;
	mask = 1<<(2*Chan);

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_INP, &val );
	if( InpRes )
		val |= mask;
	else
		val &= ~mask;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_INP, val );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan)
{
	U32			val, mask;

	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;
	mask = 1<<(2*Chan);

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_INP, &val );
	InpRes = (val&mask) ? 1 :0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::SetStartMode( CModule *pModule, PVOID pStartStopMode )
{
	int Status = BRDerr_OK;
	BRD_AdcStartMode *pAdcStartMode = (BRD_AdcStartMode*)pStartStopMode;
	double thr = pAdcStartMode->level;

	Status = SetAdcStartMode(pModule, pAdcStartMode->src, pAdcStartMode->inv, &thr);
	if(Status != BRDerr_OK)
		return Status;
	pAdcStartMode->level = thr;

	BRD_StartMode *pStartMode = &pAdcStartMode->stndStart;
	//pStartMode->startInv = pAdcStartMode->inv;
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
int CFm814x125mSrv::GetStartMode( CModule *pModule, PVOID pStartStopMode )
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
int CFm814x125mSrv::SetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	FM814X125MSRV_Cfg	*pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;
	int					status;
	double				inpRange;

	inpRange = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / refGain;
	status = SetInpRange( pModule, inpRange, Chan );
	if( status >= 0 )
		refGain = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / inpRange;

	return status;
}

//***************************************************************************************
int CFm814x125mSrv::GetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	FM814X125MSRV_Cfg	*pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;
	int					status;
	double				inpRange;

	status = GetInpRange( pModule, inpRange, Chan );
	if( status >= 0 )
		refGain = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / inpRange;

	return status;
}

//***************************************************************************************
int CFm814x125mSrv::SetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	FM814X125MSRV_Cfg  *pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;
	ULONG				rangeIdx, rangeCode;
	int					ii;

	if( Chan >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	//
	// Вычислить код для заданной шкалы преобразования
	// (шкала Ш0 не используется)
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

	for( ii=1; ii<BRD_GAINCNT; ii++ )
	{
		//
		// Искать минимальное отклонение заданной шкалы преобразования
		//
		deltaRangeTmp = pSrvCfg->adRange[ii] - refInpRange;
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
		if( maxRange <= pSrvCfg->adRange[ii] )
		{
			maxRange = pSrvCfg->adRange[ii];
			maxRangeNo = ii;
		}
	}

	rangeIdx = deltaRangeNo;
	if( deltaRangeNo < 0 )
	{
		if( maxRangeNo < 0 )
			return BRDerr_NO_DATA;
		rangeIdx = maxRangeNo;
	}
	refInpRange = pSrvCfg->adRange[rangeIdx];
	rangeCode = rangeIdx;

	//
	// Записать значение кода в регистр GAIN
	//
	U32		gain;

	if( Chan < 4 )
	{
		IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, &gain );
		gain &= ~( 0xF << (Chan*4) );
		gain |= rangeCode << (Chan*4);
		IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, gain );
	}
	else
	{
		IndRegRead( pModule, m_AdcTetrNum, ADCnr_GAIN1, &gain );
		gain &= ~( 0xF << ((Chan-4)*4) );
		gain |= rangeCode << ((Chan-4)*4);
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_GAIN1, gain );
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::GetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	FM814X125MSRV_Cfg	*pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;
	U32					gain;
	ULONG				rangeIdx, rangeCode;

	if( Chan >= MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	if( Chan < 4 )
	{
		IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_GAIN, &gain );
		rangeCode = gain >> (4*Chan);
	}
	else
	{
		IndRegRead( pModule, m_AdcTetrNum, ADCnr_GAIN1, &gain );
		rangeCode = gain >> (4*(Chan-4));
	}
	rangeCode &= 0xF;
	rangeIdx = (rangeCode >= 5 ) ? 5 : rangeCode;
	refInpRange = pSrvCfg->adRange[rangeIdx];

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::SetCode( CModule *pModule, ULONG type )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::GetCode( CModule *pModule, ULONG &refType )
{
	refType = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::SetMaster( CModule *pModule, ULONG mode )
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
int CFm814x125mSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	int			err;

	err = CAdcSrv::SetChanMask( pModule, mask);
	SpdWrite( pModule, 0, 0, 0x80 );
	SpdWrite( pModule, 0, 0, 0x0 );
	SpdWrite( pModule, 0, 1, 0x60 );
	SpdWrite( pModule, 0, 2, 0x0 );
	SpdWrite( pModule, 0, 3, 0x0 );
	SpdWrite( pModule, 0, 4, 0x0 );

	return err;
}

//***************************************************************************************
int CFm814x125mSrv::ExtraInit(CModule* pModule)
{
	//
	// Начальная инициализация программируемых микросхем: АЦП, ИПН(ЦАП), генератора
	//
	FM814X125MSRV_Cfg	*pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;
	int					ii;

	RealDelay( 300, 1 );

	//
	// Инициализация АЦП
	//
	SetChanMask( pModule, 0x1 );
	SpdWrite( pModule, 0, 0, 0x80 );
	SpdWrite( pModule, 0, 0, 0x0 );
	SpdWrite( pModule, 0, 1, 0x60 );
	SpdWrite( pModule, 0, 2, 0x0 );
	SpdWrite( pModule, 0, 3, 0x0 );
	SpdWrite( pModule, 0, 4, 0x0 );

	//
	// Инициализация ИПН(ЦАП)
	//
	int			regAdr;

	for( regAdr=0; regAdr<16; regAdr++ )
		SpdWrite( pModule, 1, regAdr, 0x800 );
	for( ii=0; ii<MAX_ADC_CHAN; ii++ )
		pSrvCfg->aBias[ii] = 0.0;
	pSrvCfg->StCmpThr = 0.0;
	pSrvCfg->ExtClkThr = 0.0;

	//
	// Определение частоты кварца для генератора Si570/Si571
	//
	pSrvCfg->dGenFxtal = 0.0;
	if( (pSrvCfg->nGenType == 1) && (pSrvCfg->nGenRef != 0) )
	{
		U32			regData[20];
		U32			rfreqLo, rfreqHi, hsdivCode, n1Code;
		double		freqTmp;
		double		dRFreq, dHsdiv, dN1;
		ULONG		clkSrc;

		//
		// Подать питание на Si570/Si571
		//
		GetClkSource( pModule, clkSrc );
		SetClkSource( pModule, 1 );
		
		//
		// Считать регистры Si570/Si571
		//
		for( regAdr=7; regAdr<=12; regAdr++ )
			SpdRead( pModule, 3, regAdr, &regData[regAdr] );
		SetClkSource( pModule, clkSrc );

		//
		// Рассчитать частоту кварца
		//
		rfreqLo  = 0xFF & regData[12];
		rfreqLo |= (0xFF & regData[11]) << 8;
		rfreqLo |= (0xFF & regData[10]) << 16;
		rfreqLo |= (0xF & regData[9]) << 24;

		rfreqHi  = (0xF0 & regData[9]) >> 4;
		rfreqHi |= (0x3F & regData[8]) << 4;

		hsdivCode    = (0xE0 & regData[7]) >> 5;

		n1Code   = (0xC0 & regData[8]) >> 6;
		n1Code  |= (0x1F & regData[7]) << 2;

		dRFreq   = (double)rfreqLo;
		dRFreq  /= 1024.0 * 1024.0 * 256.0;
		dRFreq  += (double)rfreqHi;
		dHsdiv   = (double)( hsdivCode + 4 );
		dN1      = (n1Code==1) ? 1.0 : (double)( 0xFE & (n1Code+1));

		freqTmp  = (double)(pSrvCfg->nGenRef);
		freqTmp /= dRFreq;
		freqTmp *= dHsdiv * dN1;

		pSrvCfg->dGenFxtal = freqTmp;
	}

	//
	// запрещаем выдачу на базовый модуль сигналов CLK0_M2C, CLK1_M2C
	//
	ADC_CTRL regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	regCtrl.ByBits.clk0_m2c_en = 1;
	regCtrl.ByBits.clk1_m2c_en = 1;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm814x125mSrv::SpdRead(  CModule* pModule, U32 spdType, U32 regAdr, U32 *pRegVal )
{
	//
	// Типы SPD (spdType): 0 -АЦП, 1 - ИПН(ЦАП), 3 - генератор 
	//
	U32			spdCtrl = 0x1 | ((spdType==3) ? 0x490 : 0 );
	U32			val;
	int			ii;

	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DEVICE, spdType );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_ADDR, regAdr );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_CTRL, spdCtrl );
	for( ii=0;; ii++ )
	{
		IndRegRead( pModule, m_AdcTetrNum, ADCnr_SPD_CTRL, &val );
		if( ii>1000 )			// организуем паузу
			if( val & 0x8000 )
				break;		
	}
	IndRegRead( pModule, m_AdcTetrNum, ADCnr_SPD_DATA, pRegVal );

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm814x125mSrv::SpdWrite( CModule* pModule, U32 spdType, U32 regAdr, U32 regVal )
{
	U32			spdCtrl = 0x2 | ((spdType==3) ? 0x490 : 0 );
	U32			val;
	int			ii;

	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_CTRL, (U32)0 );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DEVICE, spdType );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_ADDR, regAdr );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DATA, regVal );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_CTRL, spdCtrl );
	for( ii=0;; ii++ )
	{
		IndRegRead( pModule, m_AdcTetrNum, ADCnr_SPD_CTRL, &val );
		if( ii>1000 )			// организуем паузу
			if( val & 0x8000 )
				break;		
	}

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm814x125mSrv::Si571SetRate( CModule* pModule, double *pRate )
{
	FM814X125MSRV_Cfg	*pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;
	U32			regData[20];
	U32			rfreqLo, rfreqHi, hsdivCode, n1Code;
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
	if( *pRate < 10000000.0 )
	{
		*pRate = 10000000.0;
		status = BRDerr_WARN;
	}
	if( (*pRate > 945000000.0) && (*pRate<970000000.0) )
	{
		*pRate = 945000000.0;
		status = BRDerr_WARN;
	}
	if( (*pRate > 1134000000.0) && (*pRate<1212500000.0) )
	{
		*pRate = 1134000000.0;
		status = BRDerr_WARN;
	}

	//
	// Вычислить коэффициенты
	//
	if( 0 > Si571CalcDiv( *pRate, pSrvCfg->dGenFxtal, &rfreqLo, &rfreqHi, &hsdivCode, &n1Code ) )
		return BRDerr_ERROR;

	regData[7]  = regData[13] = (hsdivCode << 5) | (n1Code >> 2);
	regData[8]  = regData[14] = (n1Code << 6) | (rfreqHi >> 4);
	regData[9]  = regData[15] = (rfreqHi << 4) | (rfreqLo >> 24);
	regData[10] = regData[16] = (rfreqLo >> 16) & 0xFF;
	regData[11] = regData[17] = (rfreqLo >> 8) & 0xFF;
	regData[12] = regData[18] = rfreqLo & 0xFF;	

	//
	// Запрограммировать генератор
	//
	SpdWrite( pModule, 3, 137, 0x10 );		// Freeze DCO
	for( regAdr=7; regAdr<=18; regAdr++ )
		SpdWrite( pModule, 3, regAdr, regData[regAdr] );		
	SpdWrite( pModule, 3, 137, 0x0 );		// Unfreeze DCO
	SpdWrite( pModule, 3, 135, 0x40 );		// Assert the NewFreq bit

	return status;	
}

//***************************************************************************************
int	CFm814x125mSrv::Si571GetRate( CModule* pModule, double *pRate )
{
	FM814X125MSRV_Cfg	*pSrvCfg  = (FM814X125MSRV_Cfg*)m_pConfig;
	U32			regData[20];
	U32			rfreqLo, rfreqHi, hsdivCode, n1Code;
	double		freqTmp;
	double		dRFreq, dHsdiv, dN1;
	ULONG		clkSrc;
	int			regAdr;

	*pRate = 0.0;

	//
	// Проверить источник частоты
	//
	GetClkSource( pModule, clkSrc );
	if( clkSrc != BRDclks_ADC_SUBGEN )
		BRDerr_ERROR;

	//
	// Считать регистры Si570/Si571
	//
	for( regAdr=7; regAdr<=12; regAdr++ )
		SpdRead( pModule, 3, regAdr, &regData[regAdr] );

	//
	// Восстановить коэффициенты 
	//
	rfreqLo  = 0xFF & regData[12];
	rfreqLo |= (0xFF & regData[11]) << 8;
	rfreqLo |= (0xFF & regData[10]) << 16;
	rfreqLo |= (0xF & regData[9]) << 24;

	rfreqHi  = (0xF0 & regData[9]) >> 4;
	rfreqHi |= (0x3F & regData[8]) << 4;

	hsdivCode    = (0xE0 & regData[7]) >> 5;

	n1Code   = (0xC0 & regData[8]) >> 6;
	n1Code  |= (0x1F & regData[7]) << 2;

	dRFreq   = (double)rfreqLo;
	dRFreq  /= 1024.0 * 1024.0 * 256.0;
	dRFreq  += (double)rfreqHi;
	dHsdiv   = (double)( hsdivCode + 4 );
	dN1      = (n1Code==1) ? 1.0 : (double)( 0xFE & (n1Code+1));

	//
	// Рассчитать частоту 
	//
	freqTmp  = (double)(pSrvCfg->dGenFxtal);
	freqTmp /= dHsdiv * dN1;
	freqTmp *= dRFreq;
	*pRate   = freqTmp;

	return BRDerr_OK;	
}

//***************************************************************************************
int	CFm814x125mSrv::Si571CalcDiv( double dRate, double dFXtal, 
								  U32 *pRfreqLo, U32 *pRfreqHi, U32 *pHsdivCode, U32 *pN1Code )
{
	double		dRFreq, dHsdiv, dN1;
	int			ii;

	//
	// Вычислить коэффициенты HS_DIV, N1
	//
	dHsdiv = dN1 = 0.0;

	if( dRate>=1212500000.0 )
	{
		dHsdiv = 4;
		dN1 = 1;
	}
	else if( dRate>=970000000.0 )
	{
		dHsdiv = 5;
		dN1 = 1;
	}
	else
	{
		double		dDcoMin = 4850000000.0;
		double		dDcoMax = 5670000000.0;
		double		adHsdiv[] = { 4.0, 5.0, 6.0, 7.0, 9.0, 11.0 };
		double		dFreqDco, dFreqTmp;
		S32			n1;

		dFreqDco = dDcoMax;
		for( n1=1; n1<=128; n1++ )
		{
			if( (n1>1) && (n1&0x1) )
				continue;				// только четные n1
			for( ii=0; ii<sizeof(adHsdiv)/sizeof(adHsdiv[0]); ii++ )
			{
				dFreqTmp = dRate * adHsdiv[ii] * (double)n1;
				if( (dFreqTmp>=dDcoMin) && (dFreqTmp<=dFreqDco) )
				{
					dFreqDco = dFreqTmp;
					dHsdiv = adHsdiv[ii];
					dN1 = (double)n1;
				}
			}
		}

	}
	if( (dHsdiv==0.0) || (dN1==0.0) )
		BRDerr_ERROR;

	//
	// Вычислить коэффициент RFREQ
	//
	dRFreq  = dRate * dHsdiv * dN1;
	dRFreq /= dFXtal;

	//
	// Преобразовать коэффициенты в коды
	//
	*pRfreqHi = (U32)dRFreq;
	*pRfreqLo = (U32)( (dRFreq - (double)(*pRfreqHi)) * 1024.0 * 1024.0 * 256.0 );
	*pHsdivCode = (U32)(dHsdiv-4.0);
	*pN1Code = (U32)(dN1-1.0);

	return BRDerr_OK;	
}

//***************************************************************************************
int CFm814x125mSrv::SetAdcStartMode( CModule *pModule, ULONG source, ULONG inv, double *pCmpThr )
{
	FM814X125MSRV_Cfg *pSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;

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
	// Установить опорное напряжение для стартовой синхронизации
	//
	double  dTmp;
	U32		code;

	if( *pCmpThr > 2.5 ) *pCmpThr = 2.5;
	if( *pCmpThr < -2.5 ) *pCmpThr = -2.5;
	dTmp  = (*pCmpThr + 2.5) / 5.0;
	dTmp *= 4096.0;
	code  = (U32)dTmp;
	if( 0==source )
		SpdWrite( pModule, 1, 10, code );
	if( 2==source )
		SpdWrite( pModule, 1, 9, code );

	pSrvCfg->StCmpThr = *pCmpThr;

return BRDerr_OK;

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
int CFm814x125mSrv::GetAdcStartMode( CModule *pModule, PULONG pSource, PULONG pInv, double *pCmpThr )
{
	FM814X125MSRV_Cfg	*pSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;
	ADC_CTRL regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	*pSource = regCtrl.ByBits.StartSrc;
	*pInv    = 0;	//regCtrl.ByBits.StartInv;

	*pCmpThr = pSrvCfg->StCmpThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::SetSpecific( CModule *pModule, BRD_AdcSpec *pSpec )
{
	int status;

	status = CAdcSrv::SetSpecific(pModule, pSpec);

	switch(pSpec->command)
	{
		case FM814X125Mcmd_SETMU:
			status = SetMu(pModule, pSpec->arg);
			break;
		case FM814X125Mcmd_GETMU:
			status = GetMu(pModule, pSpec->arg);
			break;
	}
	return status;
}

//***************************************************************************************
int CFm814x125mSrv::SetMu( CModule *pModule, void *args )
{
	FM814X125MSRV_Cfg	*pSrvCfg = (FM814X125MSRV_Cfg*)m_pConfig;
	FM814X125MSRV_MU	*pMU = (FM814X125MSRV_MU*)args;

	CAdcSrv::SetChanMask(pModule, pMU->chanMask);
	SetMaster(pModule, pMU->syncMode);

	CtrlFormat( pModule, NULL, BRDctrl_ADC_SETFORMAT, &pMU->format );

	SetClkSource(pModule, pMU->clockSrc);
	pSrvCfg->SubExtClk = ROUND(pMU->clockValue);
	SetClkValue(pModule, pMU->clockSrc, pMU->clockValue);
	SetRate(pModule, pMU->samplingRate, pMU->clockSrc, pMU->clockValue);
	SetChanMask(pModule, pMU->chanMask);
	SetExtClkThr(pModule, pMU->clockLevel );

	for(int ii = 0; ii < BRD_CHANCNT; ii++)
	{
		SetInpRange(pModule, pMU->range[ii], ii);
		SetBias(pModule, pMU->bias[ii], ii);
		SetInpResist(pModule, pMU->inpResist[ii], ii);
		SetDcCoupling(pModule, pMU->dcCoupling[ii], ii);
	}

	BRD_AdcStartMode rStMode;

	rStMode.src = pMU->startSrc;
	rStMode.level = pMU->startLevel;
	CAdcSrv::GetStartMode(pModule, &rStMode.stndStart);
	SetStartMode(pModule, &rStMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x125mSrv::GetMu( CModule *pModule, void *args )
{
	return BRDerr_OK;
}


// ***************** End of file Fm814x125mSrv.cpp ***********************
