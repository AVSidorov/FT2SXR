/*
 ***************** File Fm816x250mSrv.cpp ************
 *
 * BRD Driver for ADС service on FM816x250M
 *
 * (C) InSys by Ekkore Aug 2011
 *
 ******************************************************
*/

#include "module.h"
#include "realdelay.h"
#include "gipcy.h"
#include "adcfm816x250msrv.h"

#define	CURRFILE "FM816x250MSRV"

const double BRD_ADC_MAXGAINTUN	= 1.; // not used

//***************************************************************************************
CFm816x250mSrv::CFm816x250mSrv(int idx, int srv_num, CModule* pModule, FM816x250MSRV_Cfg *pCfg) :
	CAdcSrv(idx, _BRDC("FM816x250M"), srv_num, pModule, pCfg, sizeof(FM816x250MSRV_Cfg))
{
}

//***************************************************************************************
int CFm816x250mSrv::CtrlRelease( void *pDev, void *pServData, ULONG cmd, void *args )
{
	CModule* pModule = (CModule*)pDev;

	SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);

	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб (вместо BRDerr_OK)
}

//***************************************************************************************
void CFm816x250mSrv::GetAdcTetrNum( CModule *pModule )
{
	//
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	//

	//
	// Поиск номера терады. Если тетрада не найдена, то -1.
	//
	m_AdcTetrNum = GetTetrNum(pModule, m_index, FM816x250M_TETR_ID);
	if(m_AdcTetrNum == -1)
		m_AdcTetrNum = GetTetrNum(pModule, m_index, FM416x250M_TETR_ID);
}

//***************************************************************************************
void CFm816x250mSrv::GetAdcTetrNumEx( CModule *pModule, ULONG TetrInstNum)
{
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	// Поиск номера терады. Если тетрада не найдена, то -1.
	m_AdcTetrNum = GetTetrNumEx(pModule, m_index, FM816x250M_TETR_ID, TetrInstNum);
	if(m_AdcTetrNum == -1)
		m_AdcTetrNum = GetTetrNumEx(pModule, m_index, FM416x250M_TETR_ID, TetrInstNum);
}

//***************************************************************************************
void CFm816x250mSrv::FreeInfoForDialog( PVOID pInfo )
{
	FM816x250MSRV_Info *pSrvInfo = (FM816x250MSRV_Info*)pInfo;
	if( NULL != pSrvInfo )
	{
		CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
		delete pSrvInfo;
	}
}

//***************************************************************************************
PVOID CFm816x250mSrv::GetInfoForDialog( CModule *pModule )
{
	return NULL;
}

//***************************************************************************************
int CFm816x250mSrv::SetPropertyFromDialog( void *pDev, void *args )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::SetProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	FM816x250MSRV_Cfg	*pSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;
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
	// Извлечение для каждого канала требуемых:
	// 1. шкалы преобразования
	// 2. смещения нуля
	// 3. открытый/закрытый вход
	// 4. входное сопротивление
	//
	double			dVal;
	DWORD			ret;

	for( ii = 0; ii < MAX_ADC_CHAN; ii++ )
	{
		BRDC_sprintf( ParamName, _BRDC("InputRange%d"), ii);
        ret = IPC_getPrivateProfileString( SectionName, ParamName, _BRDC("0.5"), Buffer, sizeof(Buffer), iniFilePath);
        if( '\0' != Buffer[0] )
        {
            dVal = BRDC_atof(Buffer);
            SetInpRange(pModule, dVal, ii);
        }

		BRDC_sprintf( ParamName, _BRDC("Gain%d"), ii);
        ret = IPC_getPrivateProfileString( SectionName, ParamName, _BRDC("1.0"), Buffer, sizeof(Buffer), iniFilePath);
        if( '\0' != Buffer[0] )
        {
            dVal = BRDC_atof(Buffer);
            SetGain(pModule, dVal, ii);
        }

		BRDC_sprintf( ParamName, _BRDC("Bias%d"), ii);
        ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		dVal = BRDC_atof(Buffer);
		SetBias(pModule, dVal, ii);
	}

	//
	// Программирование токов буфферов всех кристаллов АЦП
	//
	S32	 bufCur[2];	//  Ток входного буфера 
	
    IPC_getPrivateProfileString(SectionName, _BRDC("BufCur1_Adc"), _BRDC("1111"), Buffer, sizeof(Buffer), iniFilePath);
	bufCur[0] = BRDC_atoi(Buffer);		
	
    IPC_getPrivateProfileString(SectionName, _BRDC("BufCur2_Adc"), _BRDC("1111"), Buffer, sizeof(Buffer), iniFilePath);
	bufCur[1] = BRDC_atoi(Buffer);

	for( ii = 0; ii < pSrvCfg->AdcCfg.MaxChan; ii++ )
		SetBufCur( pModule, bufCur, ii );

	//
	// Извлечение требуемого режима стартовой синхронизации
	//
	BRD_AdcStartMode rStMode;

    IPC_getPrivateProfileString(SectionName, _BRDC("StartSource"), _BRDC("3"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.src = BRDC_atoi(Buffer);

    IPC_getPrivateProfileString(SectionName, _BRDC("StartLevel"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.level = BRDC_atof(Buffer);

    //IPC_getPrivateProfileString(SectionName, _BRDC("StartInverting"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	//rStMode.inv = BRDC_atoi(Buffer);

	CAdcSrv::GetStartMode(pModule, &rStMode.stndStart);
	//rStMode.stndStart.startSrc = 7; 
	//rStMode.stndStart.trigOn = 1; 
	SetStartMode(pModule, &rStMode);

	//
	// Требуется Новая команда!!!
	//
	ULONG		inpResist;

    IPC_getPrivateProfileString(SectionName, _BRDC("StartResistance"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	inpResist = BRDC_atoi(Buffer);
	SetInpResist( pModule, inpResist, 1024 );

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
int CFm816x250mSrv::SaveProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	BRDCHAR				ParamName[128];
	FM816x250MSRV_Cfg	*pSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;

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

	for(int i = 0; i < MAX_ADC_CHAN; i++)
	{
		GetGain(pModule, dVal, i);
		BRDC_sprintf( ParamName, _BRDC("Gain%d"), i);
		WriteInifileParam(iniFilePath, SectionName, ParamName, dVal, 2, NULL);

		GetBias(pModule, dVal, i);
		BRDC_sprintf( ParamName, _BRDC("Bias%d"), i);
		WriteInifileParam(iniFilePath, SectionName, ParamName, dVal, 4, NULL);
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
int CFm816x250mSrv::SetClkSource( CModule *pModule, ULONG nClkSrc )
{
	FM816x250MSRV_Cfg	*pSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;
	ADM2IF_FSRC			fsrc;

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
int CFm816x250mSrv::GetClkSource( CModule *pModule, ULONG &refClkSrc )
{
	FM816x250MSRV_Cfg	*pSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;
	ADM2IF_FSRC			fsrc;

	//
	// Определить режим работы тетрады: Single или Slave
	//
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &fsrc.AsWhole );
	refClkSrc = fsrc.ByBits.Gen;



	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::SetClkValue( CModule *pModule, ULONG nClkSrc, double &refClkValue )
{
	FM816x250MSRV_Cfg	*pSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;

	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			refClkValue = 0.0;
			break;
		case BRDclks_ADC_SUBGEN:
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
int CFm816x250mSrv::GetClkValue( CModule *pModule, ULONG nClkSrc, double &refClkValue )
{
	FM816x250MSRV_Cfg	*pSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;

	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			refClkValue = 0.0;
			break;
		case BRDclks_ADC_SUBGEN:
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
int CFm816x250mSrv::SetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;
	CAdcSrv::SetClkLocation(pModule, refMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::GetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::SetRate( CModule *pModule, double &refRate, ULONG nClkSrc, double ClkValue )
{
	FM816x250MSRV_Cfg	*pAdcSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;

	SetClkSource( pModule, nClkSrc );
	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			refRate = 0.0;
			break;

		case BRDclks_ADC_SUBGEN:
			if( pAdcSrvCfg->nGenType == 0 )
			{
				//
				// Генератор на субмодуле непрограммируемый
				//
				refRate = pAdcSrvCfg->nGenRef;
			}
			else
			{
				//
				// Записать тактову частоту в генератор Si570/Si571
				//
				double		dClkValueRd;

				Si571GetRate( pModule, &dClkValueRd ); // Считать Si570/Si571
				//printf( ">> Rate = %f kHz\n", refRate);
				if( dClkValueRd != refRate )
					err = Si571SetRate( pModule, &refRate ); // Запрограммировать Si570/Si571
				//printf( ">> Rate = %f kHz\n", refRate);
				Si571GetRate( pModule, &refRate ); // Считать Si570/Si571
				//printf( ">> Rate = %f kHz\n", refRate);
			}
			break;
/*
		case BRDclks_ADC_SUBGEN:
			double		dClkValue;

			Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
			if( dClkValue != refRate )
			{
				dClkValue = refRate;
				err = Si571SetRate( pModule, &dClkValue ); // Запрограммировать Si570/Si571
				refRate = dClkValue;
			}
			break;*/
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
int CFm816x250mSrv::GetRate( CModule *pModule, double &refRate, double ClkValue )
{
	FM816x250MSRV_Cfg	*pAdcSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;
	ULONG				nClkSrc;

	GetClkSource( pModule, nClkSrc );

	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
			refRate = 0.0;
			break;
		case BRDclks_ADC_SUBGEN:
			if( pAdcSrvCfg->nGenType == 0 )
			{
				//
				// Генератор на субмодуле непрограммируемый
				//
				refRate = pAdcSrvCfg->nGenRef;
			}
			else
			{
				//
				// Считать тактовую частоту в генератор Si570/Si571
				//
				double		dClkValue ;

				Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
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
			break;
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::SetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	FM816x250MSRV_Cfg  *pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;
	int					status = BRDerr_OK;
	S32					biascode;

	//
	// refBias указывает коррекцию смещ. нуля в ЕМРах (-128..+127)
	// При завершении функции записываем в refBias смещение в Вольтах
	//

	if( Chan > pSrvCfg->AdcCfg.MaxChan )
		return BRDerr_BAD_PARAMETER;

	biascode = (S32)floor( refBias );
	if( biascode > 127 )
		biascode = 127;
	if( biascode < -128 )
		biascode = -128;

	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCMASK, (U32)(1<<Chan) );
	SpdWrite( pModule, SPDdev_ADC, 0x10, (U32)biascode );
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );

	refBias  = (double)biascode / 32768.0;
	refBias *= pSrvCfg->AdcCfg.InpRange / pSrvCfg->aGain[Chan];
	pSrvCfg->aBias[Chan] = refBias;

	return status;
}

//***************************************************************************************
int CFm816x250mSrv::GetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	FM816x250MSRV_Cfg  *pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;

	if( Chan > pSrvCfg->AdcCfg.MaxChan )
		return BRDerr_BAD_PARAMETER;

	refBias = pSrvCfg->aBias[Chan];

	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan)
{
	U32			val;

	//
	// Установить вх.сопротивление для сигнала внешнего старта: 0 - 2.5 кОм, 1 - 50 Ом
	//
	if( Chan != 1024 )
		return BRDerr_BAD_PARAMETER;

/*	IndRegRead( pModule, m_AdcTetrNum, ADCnr_EXTSTART, &val );
	val &= 0xFFFE;
	if( InpRes )
		val |= 0x1;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_EXTSTART, val );
*/
	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &val );
	val &= 0xFFFE;
	if( InpRes )
		val |= 0x1;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, val );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan)
{
	U32			val;

	//
	// Считать вх.сопротивление для сигнала внешнего старта: 0 - 2.5 кОм, 1 - 50 Ом
	//
	if( Chan != 1024 )
		return BRDerr_BAD_PARAMETER;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &val );
	InpRes = val & 0x1;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::SetStartMode( CModule *pModule, PVOID pStartStopMode )
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
		//mode0.ByBits.Start = 1;
		mode0.ByBits.AdmClk = 1;
		mode0.ByBits.Master = 1;
		IndRegWrite( pModule, m_MainTetrNum, ADM2IFnr_MODE0, mode0.AsWhole );

		IndRegRead( pModule, m_MainTetrNum, ADM2IFnr_STMODE, &stMode.AsWhole );
		stMode.ByBits.SrcStart = BRDsts_TRDADC;
		stMode.ByBits.TrigStart = 0;
		IndRegWrite( pModule, m_MainTetrNum, ADM2IFnr_STMODE, stMode.AsWhole );
	}
	return Status;
}

//***************************************************************************************
int CFm816x250mSrv::GetStartMode( CModule *pModule, PVOID pStartStopMode )
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
int CFm816x250mSrv::SetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	//
	// Коэффициент 1.0  соответствует шкале 1.25 Вольта
	// Коэффициент 1.25 соответствует шкале 1.00 Вольт
	//
	FM816x250MSRV_Cfg	*pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;
	U32					code;

	if( Chan > pSrvCfg->AdcCfg.MaxChan )
		return BRDerr_BAD_PARAMETER;

	if( refGain>= 1.25 )
	{	refGain = 1.25;	code = 0x00; }
	else
	{	refGain = 1.00; code = 0x0A; }

	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCMASK, (U32)(1<<Chan) );
	SpdWrite( pModule, SPDdev_ADC, 0x18, code );
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );

	pSrvCfg->aGain[Chan] = refGain;


/*
	int					status;
	double				inpRange;

	inpRange = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / refGain;
	status = SetInpRange( pModule, inpRange, Chan );
	if( status >= 0 )
		refGain = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / inpRange;
*/
	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::GetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	FM816x250MSRV_Cfg	*pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;

	if( Chan > pSrvCfg->AdcCfg.MaxChan )
		return BRDerr_BAD_PARAMETER;

	refGain = pSrvCfg->aGain[Chan];

	//int					status;
	//double				inpRange;

	//status = GetInpRange( pModule, inpRange, Chan );
	//if( status >= 0 )
	//	refGain = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / inpRange;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::SetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	FM816x250MSRV_Cfg  *pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;
	int					status;
	double				gain;

	if( Chan > pSrvCfg->AdcCfg.MaxChan )
		return BRDerr_BAD_PARAMETER;

	gain = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / refInpRange;
	status = SetGain( pModule, gain, Chan );
	refInpRange = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / gain;

	//U32					aCode[]   = { 0x00, 0x06, 0x07, 0x08, 0x09, 0x0A };
	//double				aRanges[] = { 1.00, 1.05, 1.10, 1.15, 1.20, 1.25 };
	//int					idx;


	//if( Chan > MAX_ADC_CHAN )
	//	return BRDerr_BAD_PARAMETER;

	//for( idx=0; idx<5; idx++ )
	//	if( refInpRange <= aRanges[idx] )
	//		break;

	//IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCMASK, (U32)(1<<Chan) );
	//SpdWrite( pModule, SPDdev_ADC, 0x18, aCode[idx] );
	//SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );

	//pSrvCfg->aInpRange[Chan] = refInpRange;

	return status;
}

//***************************************************************************************
int CFm816x250mSrv::GetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	FM816x250MSRV_Cfg	*pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;
	int					status;
	double				gain;

	if( Chan > pSrvCfg->AdcCfg.MaxChan )
		return BRDerr_BAD_PARAMETER;

	status = GetGain( pModule, gain, Chan );
	refInpRange = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / gain;

	//if( Chan >= MAX_ADC_CHAN )
	//	return BRDerr_BAD_PARAMETER;

	//refInpRange = pSrvCfg->aInpRange[Chan];

	return status;
}

//***************************************************************************************
int CFm816x250mSrv::SetCode( CModule *pModule, ULONG type )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::GetCode( CModule *pModule, ULONG &refType )
{
	refType = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::SetMaster( CModule *pModule, ULONG mode )
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
int CFm816x250mSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	FM816x250MSRV_Cfg	*pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;
	int			err;

//	mask = 0xF;
//	if( pSrvCfg->nSubunitId == 0x1050 )
//		mask = 0xFF;

	err = CAdcSrv::SetChanMask( pModule, mask);

	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCMASK, mask );
	SpdWrite( pModule, SPDdev_ADC, 0x14, 0x09 );
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );

	return err;
}

//***************************************************************************************
int CFm816x250mSrv::ExtraInit(CModule* pModule)
{
	//
	// Начальная инициализация программируемых микросхем: АЦП, ИПН(ЦАП), генератора
	//
	FM816x250MSRV_Cfg	*pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;
	int					ii;

	// 
	// Инициализация АЦП
	//
	SetChanMask( pModule, 0x1 );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCMASK, (U32)0xFF );
	SpdWrite( pModule, SPDdev_ADC, 0x10, 0 );
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );
	SpdWrite( pModule, SPDdev_ADC, 0x14, 0x09 );
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );
	SpdWrite( pModule, SPDdev_ADC, 0x18, 0x0A );
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );
	for( ii=0; ii<MAX_ADC_CHAN; ii++ )
	{
		pSrvCfg->aBias[ii] = 0.0;
		pSrvCfg->aGain[ii] = 1.00;
	}

	//
	// Инициализация ИПН(ЦАП)
	//
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_EXTSTART, (U32)0 );

	//
	// Определение частоты кварца для генератора Si570/Si571
	//
	pSrvCfg->dGenFxtal = 0.0;
	if( (pSrvCfg->nGenType == 1) && (pSrvCfg->nGenRef != 0) )
	{
		U32			regAdr, regData[20];
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
		//printf( "\n>> XTAL = %f kHz\n", pSrvCfg->dGenFxtal/1000.0 );
		//printf( "\n>> GREF = %f kHz\n", ((double)(pSrvCfg->nGenRef))/1000.0 );
		//rfreqLo  = 0xFF & regData[12];
		//rfreqLo |= (0xFF & regData[11]) << 8;
		//rfreqLo |= (0xFF & regData[10]) << 16;
		//rfreqLo |= (0xF & regData[9]) << 24;

		//rfreqHi  = (0xF0 & regData[9]) >> 4;
		//rfreqHi |= (0x3F & regData[8]) << 4;

		//hsdivCode    = (0xE0 & regData[7]) >> 5;

		//n1Code   = (0xC0 & regData[8]) >> 6;
		//n1Code  |= (0x1F & regData[7]) << 2;

		//dRFreq   = (double)rfreqLo;
		//dRFreq  /= 1024.0 * 1024.0 * 256.0;
		//dRFreq  += (double)rfreqHi;
		//dHsdiv   = (double)( hsdivCode + 4 );
		//dN1      = (n1Code==1) ? 1.0 : (double)( 0xFE & (n1Code+1));

		//freqTmp  = (double)(pSrvCfg->nGenRef);
		//freqTmp /= dRFreq;
		//freqTmp *= dHsdiv * dN1;

		//pSrvCfg->dGenFxtal = freqTmp;
	}

	//
	// запрещаем выдачу на базовый модуль сигналов CLK0_M2C, CLK1_M2C
	//
	//ADC_CTRL regCtrl;

	//IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	//regCtrl.ByBits.clk0_m2c_en = 1;
	//regCtrl.ByBits.clk1_m2c_en = 1;
	//IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm816x250mSrv::SpdRead(  CModule* pModule, U32 spdType, U32 regAdr, U32 *pRegVal )
{
	//
	// Типы SPD (spdType): 0 -АЦП, 1 - ИПН(ЦАП), 3 - генератор 
	//
	FM816x250MSRV_Cfg	*pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;
	U32			spdCtrl;
	U32			status;
	//int			ii;

	spdCtrl  = 0x1;
	if( spdType==SPDdev_GEN )
		spdCtrl |= (pSrvCfg->nGenAdr & 0xFF) << 4;

	for(;;)
	{
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DEVICE, spdType );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_ADDR, regAdr );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_CTRL, spdCtrl );
	//RealDelay(5,1);
	//for( ii=0;; ii++ )
	for(;;)
	{
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		//if( ii>1000 )			// организуем паузу
		//	continue;
		if( status & 0x01 )
			break;		
	}
	IndRegRead( pModule, m_AdcTetrNum, ADCnr_SPD_DATA, pRegVal );

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm816x250mSrv::SpdWrite( CModule* pModule, U32 spdType, U32 regAdr, U32 regVal )
{
	FM816x250MSRV_Cfg	*pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;
	U32			spdCtrl;
	U32			status;
	//int			ii;

	spdCtrl  = 0x2;
	if( spdType==SPDdev_GEN )
		spdCtrl |= (pSrvCfg->nGenAdr & 0xFF) << 4;

	for(;;)
	{
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_CTRL, (U32)0 );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DEVICE, spdType );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_ADDR, regAdr );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DATA, regVal );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_CTRL, spdCtrl );
	//RealDelay(5,1);
	//for( ii=0;; ii++ )
	for(;;)
	{
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		//if( ii>1000 )			// организуем паузу
		//	continue;
		if( status & 0x01 )
			break;		
	}

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm816x250mSrv::Si571SetRate( CModule* pModule, double *pRate )
{
	FM816x250MSRV_Cfg	*pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;
	U32			regData[20];
	int			regAdr;
	int			status = BRDerr_OK;

	//
	// Скорректировать частоту, если необходимо
	//
	if( *pRate > (double)pSrvCfg->nGenRefMax )
	{
		*pRate = (double)pSrvCfg->nGenRefMax;
		//printf( ">> Rate(MAX) = %f kHz\n", *pRate);
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
	for( regAdr=7; regAdr<=18; regAdr++ )
	{
		U32 val = 0;
		SpdRead( pModule, SPDdev_GEN, regAdr, &val );
		if((val & 0xFF) != (regData[regAdr] & 0xFF))
		{
			printf( ">> ERROR!!! addr=%d rd=%X wr=%X\n", regAdr, val, regData[regAdr]);
		}
	}
	//printf( ">> Rate = %f kHz\n", *pRate);
	return status;	
}

//***************************************************************************************
int	CFm816x250mSrv::Si571GetRate( CModule* pModule, double *pRate )
{
	FM816x250MSRV_Cfg	*pSrvCfg  = (FM816x250MSRV_Cfg*)m_pConfig;
	U32			regData[20];
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
		SpdRead( pModule, SPDdev_GEN, regAdr, &regData[regAdr] );

	SI571_GetRate( pRate, pSrvCfg->dGenFxtal, regData );

	return BRDerr_OK;	
}

//***************************************************************************************
int CFm816x250mSrv::SetAdcStartMode( CModule *pModule, ULONG source, ULONG inv, double *pCmpThr )
{
	FM816x250MSRV_Cfg *pSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;

	//
	// Источник сигнала старта и инверсия сигнала старта
	// ИГНОРИРУЮТСЯ
	//


	//
	// Установить опорное напряжение для стартовой синхронизации
	//
	double  dTmp, dDacRange = pSrvCfg->nDacRange*1000.0;
	U32		code, val;
/*
	if( *pCmpThr > dDacRange ) *pCmpThr = dDacRange;
	if( *pCmpThr < -dDacRange) *pCmpThr = -dDacRange;
	dTmp  = (*pCmpThr + dDacRange/2.0) / dDacRange;
	dTmp *= 4096.0;
	dTmp  = floor(dTmp);
	*pCmpThr = (dTmp / 4096.0) * dDacRange - dDacRange/2.0;
*/
	if( *pCmpThr > 5.0) *pCmpThr = 5.0;
	if( *pCmpThr < 0.0) *pCmpThr = 0.0;
	dTmp  = *pCmpThr / 5.0;
	dTmp *= 4096.0;
	dTmp  = floor(dTmp);
	*pCmpThr = dTmp / 4096.0 * 5.0;

	code  = (U32)dTmp;
	code<<= 2;
	code &= 0x3FFC;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_EXTSTART, &val );
	val  &= 0xC003;
	code |= val;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_EXTSTART, code );

	pSrvCfg->StCmpThr = *pCmpThr;

	//printf("dTmp = %d, StCmpThr = %.2f Volt, code = 0x%0X\n", dTmp, pSrvCfg->StCmpThr, code);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::GetAdcStartMode( CModule *pModule, PULONG pSource, PULONG pInv, double *pCmpThr )
{
	FM816x250MSRV_Cfg	*pSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;

	*pSource = 0;
	*pInv    = 0;
	*pCmpThr = pSrvCfg->StCmpThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::SetSpecific( CModule *pModule, BRD_AdcSpec *pSpec )
{
	int status;

	status = CAdcSrv::SetSpecific(pModule, pSpec);

	switch(pSpec->command)
	{
		case FM816x250Mcmd_SETMU:
			status = SetMu(pModule, pSpec->arg);
			break;
		case FM816x250Mcmd_GETMU:
			status = GetMu(pModule, pSpec->arg);
			break;
		case FM816x250Mcmd_SUBSYNC:
			{
				ULONG start = *(ULONG*)pSpec->arg;
				ADM2IF_MODE0 mode0;
				IndRegRead( pModule, m_MainTetrNum, ADM2IFnr_MODE0, &mode0.AsWhole );
				mode0.ByBits.Start = start;
				IndRegWrite( pModule, m_MainTetrNum, ADM2IFnr_MODE0, mode0.AsWhole );
				IndRegRead( pModule, m_MainTetrNum, ADM2IFnr_MODE0, &mode0.AsWhole );
				//BRDC_printf(_BRDC("\n\nMODE0 = %X\n\n"),mode0.AsWhole);
				break;
			}
	}
	return status;
}

//***************************************************************************************
int CFm816x250mSrv::SetMu( CModule *pModule, void *args )
{
	FM816x250MSRV_Cfg	*pSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;
	FM816x250MSRV_MU	*pMU = (FM816x250MSRV_MU*)args;

	CAdcSrv::SetChanMask(pModule, pMU->chanMask);
	SetMaster(pModule, pMU->syncMode);

	CtrlFormat( pModule, NULL, BRDctrl_ADC_SETFORMAT, &pMU->format );

	SetClkSource(pModule, pMU->clockSrc);
	pSrvCfg->SubExtClk = ROUND(pMU->clockValue);
	SetClkValue(pModule, pMU->clockSrc, pMU->clockValue);
	SetRate(pModule, pMU->samplingRate, pMU->clockSrc, pMU->clockValue);
	SetChanMask(pModule, pMU->chanMask);

	for(int ii = 0; ii < BRD_CHANCNT; ii++)
	{
		SetInpRange(pModule, pMU->range[ii], ii);
		SetBias(pModule, pMU->bias[ii], ii);
	}

	BRD_AdcStartMode rStMode;

	rStMode.src = pMU->startSrc;
	rStMode.level = pMU->startLevel;
	CAdcSrv::GetStartMode(pModule, &rStMode.stndStart);
	SetStartMode(pModule, &rStMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::GetMu( CModule *pModule, void *args )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm816x250mSrv::SetBufCur(CModule* pModule, S32 bufCur[2], ULONG chanNo )
{	
	FM816x250MSRV_Cfg	*pSrvCfg = (FM816x250MSRV_Cfg*)m_pConfig;
	U32					code = 0;

	if( chanNo > pSrvCfg->AdcCfg.MaxChan )
		return BRDerr_BAD_PARAMETER;

	
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCMASK, (U32)(1<<chanNo) );

	if( (bufCur[0]>=-100) && (bufCur[0]<=530) )
	{
		code   = bufCur[0] / 10;
		code <<= 2;
		code  |= 2;
		code  &= 0xFF;
		SpdWrite( pModule, SPDdev_ADC, 0x36 , code );
		printf( "\nADC Buffer Current 1 Code = 0x%X (ch%d reg 0x36)\n", code, chanNo );
	}

	if( (bufCur[1]>=-100) && (bufCur[1]<=530) )
	{
		code   = bufCur[1] / 10;
		code <<= 2;
		code  &= 0xFF;
		SpdWrite( pModule, SPDdev_ADC, 0x107 , code );
		printf( "ADC Buffer Current 2 Code = 0x%X (ch%d reg 0x107)\n\n", code, chanNo );
	}

	SpdWrite( pModule, SPDdev_ADC, 0xFF , 1 );
	SpdWrite( pModule, SPDdev_ADC, 0xFF , 0 );

	//pSrvCfg->BufCur1[chanNo] = bufCur[0];
	//pSrvCfg->BufCur2[chanNo] = bufCur[1];

	return BRDerr_OK;
}

// ***************** End of file Fm816x250mSrv.cpp ***********************
