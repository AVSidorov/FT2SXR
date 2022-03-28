/*
 ***************** File Fm412x500mSrv.cpp ************
 *
 * BRD Driver for ADС service on FM412x500M
 *
 * (C) InSys by Ekkore Feb 2012
 *
 ******************************************************
*/

#include "module.h"
#include "realdelay.h"
#include "adcfm412x500msrv.h"

#define	CURRFILE "FM412X500MSRV"

const double BRD_ADC_MAXGAINTUN	= 1.; // not used

//***************************************************************************************
CFm412x500mSrv::CFm412x500mSrv(int idx, int srv_num, CModule* pModule, FM412X500MSRV_Cfg *pCfg) :
	CAdcSrv(idx, _BRDC("FM412X500M"), srv_num, pModule, pCfg, sizeof(FM412X500MSRV_Cfg))
{
}

//***************************************************************************************
int CFm412x500mSrv::CtrlRelease( void *pDev, void *pServData, ULONG cmd, void *args )
{
	CModule* pModule = (CModule*)pDev;

	SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);

	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб (вместо BRDerr_OK)
}

//***************************************************************************************
void CFm412x500mSrv::GetAdcTetrNum( CModule *pModule )
{
	//
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	//

	//
	// Поиск номера терады. Если тетрада не найдена, то -1.
	//
	m_AdcTetrNum = GetTetrNum(pModule, m_index, FM412X500M_TETR_ID);
}

//***************************************************************************************
void CFm412x500mSrv::GetAdcTetrNumEx( CModule *pModule, ULONG TetrInstNum)
{
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	// Поиск номера терады. Если тетрада не найдена, то -1.
	m_AdcTetrNum = GetTetrNumEx(pModule, m_index, FM412X500M_TETR_ID, TetrInstNum);
}

//***************************************************************************************
void CFm412x500mSrv::FreeInfoForDialog( PVOID pInfo )
{
}

//***************************************************************************************
PVOID CFm412x500mSrv::GetInfoForDialog( CModule *pModule )
{
	return NULL;
}

//***************************************************************************************
int CFm412x500mSrv::SetPropertyFromDialog( void *pDev, void *args )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;
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

    IPC_getPrivateProfileString(SectionName, _BRDC("ExternalClockValue"), _BRDC("1000.0"), Buffer, sizeof(Buffer), iniFilePath);
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
	// Извлечение требуемого режима стартовой синхронизации
	//
	BRD_AdcStartMode rStMode;

    IPC_getPrivateProfileString(SectionName, _BRDC("StartSource"), _BRDC("3"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.src = BRDC_atoi(Buffer);

    //IPC_getPrivateProfileString(SectionName, _BRDC("StartInverting"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	//rStMode.inv = BRDC_atoi(Buffer);
	rStMode.inv = 0;

    IPC_getPrivateProfileString(SectionName, _BRDC("StartLevel"), _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
	rStMode.level = BRDC_atof(Buffer);

	CAdcSrv::GetStartMode(pModule, &rStMode.stndStart);
	SetStartMode(pModule, &rStMode);

	//
	// Требуется Новая команда!!!
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
	DWORD		ret;

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
int CFm412x500mSrv::SaveProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
//	BRDCHAR				ParamName[128];
//	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;

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

	//double	dVal;
	//ULONG	nVal;

	//for(int i = 0; i < MAX_ADC_CHAN; i++)
	//{
	//	GetGain(pModule, dVal, i);
	//	BRDC_sprintf( ParamName, _BRDC("GainDB%d"), i);
	//	WriteInifileParam(iniFilePath, SectionName, ParamName, dVal, 2, NULL);

	//	GetBias(pModule, dVal, i);
	//	BRDC_sprintf( ParamName, _BRDC("Bias%d"), i);
	//	WriteInifileParam(iniFilePath, SectionName, ParamName, dVal, 4, NULL);

	//	GetDcCoupling(pModule, nVal, i);
	//	BRDC_sprintf( ParamName, _BRDC("DcCoupling%d"), i);
	//	WriteInifileParam(iniFilePath, SectionName, ParamName, nVal, 10, NULL);

	//	GetInpResist(pModule, nVal, i);
	//	BRDC_sprintf( ParamName, _BRDC("InputResistance%d"), i);
	//	WriteInifileParam(iniFilePath, SectionName, ParamName, nVal, 10, NULL);
	//}

	BRD_AdcStartMode start;
	GetStartMode(pModule, &start);
	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartSource"), (ULONG)start.src, 10, NULL);
//	WriteInifileParam(iniFilePath, SectionName, _BRDC("StartInverting"), (ULONG)start.inv, 10, NULL);
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
int CFm412x500mSrv::GetCfg(PBRD_AdcCfg pCfg)
{
//	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;

	CAdcSrv::GetCfg(pCfg);
	pCfg->ChanType = 0;				// Коэф. передачи измеряется в разах (нет коэффициентов кроме 1)

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetClkSource( CModule *pModule, ULONG nClkSrc )
{
	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;
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
int CFm412x500mSrv::GetClkSource( CModule *pModule, ULONG &refClkSrc )
{
	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;

	refClkSrc = pSrvCfg->nClkSrc;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetClkValue( CModule *pModule, ULONG ClkSrc, double &refClkValue )
{
	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;
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
int CFm412x500mSrv::GetClkValue( CModule *pModule, ULONG ClkSrc, double &refClkValue )
{
	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;
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
int CFm412x500mSrv::SetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;
	CAdcSrv::SetClkLocation(pModule, refMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::GetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetRate( CModule *pModule, double &refRate, ULONG nClkSrc, double ClkValue )
{
	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;
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

	return err;
}

//***************************************************************************************
int CFm412x500mSrv::GetRate( CModule *pModule, double &refRate, double ClkValue )
{
	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;

	refRate = pSrvCfg->dSamplingRate;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	return BRDerr_CMD_UNSUPPORTED;
}

//***************************************************************************************
int CFm412x500mSrv::GetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	refBias = 0.0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan)
{
	ADC_CTRL			regCtrl;

	//
	// Установить вх.сопротивление для сигнала внешнего старта: 0 - 2.5 кОм, 1 - 50 Ом
	//
	if( Chan != 1024 )
		return BRDerr_BAD_PARAMETER;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	if( InpRes )
		regCtrl.ByBits.stResist = 1;
	else
		regCtrl.ByBits.stResist = 0;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan)
{
	ADC_CTRL			regCtrl;

	//
	// Считать вх.сопротивление для сигнала внешнего старта: 0 - 2.5 кОм, 1 - 50 Ом
	//
	if( Chan != 1024 )
		return BRDerr_BAD_PARAMETER;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	InpRes = regCtrl.ByBits.stResist;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetStartMode( CModule *pModule, PVOID pStartStopMode )
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
int CFm412x500mSrv::GetStartMode( CModule *pModule, PVOID pStartStopMode )
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
	Status = CAdcSrv::GetStartMode(pModule, pStartMode);
	pAdcStartMode->inv = pStartMode->startInv;

	return Status;
}

//***************************************************************************************
int CFm412x500mSrv::SetGain(CModule* pModule, double& refGain, ULONG Chan)
{
	refGain = 1.0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::GetGain(CModule* pModule, double& refGain, ULONG Chan)
{
	refGain = 1.0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetInpRange(CModule* pModule, double& refInpRange, ULONG Chan)
{
	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;

	refInpRange = pSrvCfg->AdcCfg.InpRange;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::GetInpRange(CModule* pModule, double& refInpRange, ULONG Chan)
{
	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;

	refInpRange = pSrvCfg->AdcCfg.InpRange;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetCode( CModule *pModule, ULONG type )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::GetCode( CModule *pModule, ULONG &refType )
{
	refType = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetMaster( CModule *pModule, ULONG mode )
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
int CFm412x500mSrv::ExtraInit(CModule* pModule)
{
	//
	// Начальная инициализация программируемых микросхем: АЦП, ИПН(ЦАП), генератора
	//
	FM412X500MSRV_Cfg	*pSrvCfg  = (FM412X500MSRV_Cfg*)m_pConfig;

	RealDelay( 300, 1 );

	pSrvCfg->nClkSrc = BRDclks_ADC_DISABLED;
	pSrvCfg->nSubExtClk = 0;
	pSrvCfg->nBaseUnitClk = 0;
	pSrvCfg->dSamplingRate = 0.0;

	//
	// Инициализация ИПН(ЦАП)
	//
	ULONG		clkLocation = 1;
	SetClkLocation( pModule, clkLocation );

	//
	// Инициализация ИПН(ЦАП)
	//
	SpdWrite( pModule, SPDdev_THDAC, 0, 0x8000 );
	pSrvCfg->dStCmpThr = 0.0;

	//
	// Определение частоты кварца для генератора Si570/Si571
	//
	pSrvCfg->dGenFxtal = 0.0;
	if( (pSrvCfg->nGenType == 1) && (pSrvCfg->nGenRef != 0) )
	{
		U32			regAdr, regData[20];
		ULONG		clkSrc;

		//
		// Подать питание на Si570/Si571
		//
		GetClkSource( pModule, clkSrc );
		SetClkSource( pModule, 0 );
		SetClkSource( pModule, 1 );
		SpdWrite( pModule, SPDdev_GEN, 135, 0x80 );		// Reset
        RealDelay(300,1);
		
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
		//printf( "Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n", regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );
		//printf( ">> XTAL = %f kHz\n", pSrvCfg->dGenFxtal/1000.0 );
		//printf( ">> GREF = %f kHz\n", ((double)(pSrvCfg->nGenRef))/1000.0 );
	}

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm412x500mSrv::SpdRead(  CModule* pModule, U32 spdType, U32 regAdr, U32 *pRegVal )
{
	//
	// Типы SPD (spdType): 
	//   0x01 - ИПН(ЦАП) AD5541, 0x02 - генератор Si571
	//   0x03 - синтезатор AD9518
	//
	FM412X500MSRV_Cfg	*pSrvCfg  = (FM412X500MSRV_Cfg*)m_pConfig;
	U32			spdCtrl;
	U32			status;

	spdCtrl  = 0x1;
	if( spdType==SPDdev_GEN )
		spdCtrl |= (pSrvCfg->nGenAdr & 0xFF) << 4;
		//spdCtrl |= 0x490;
		//spdCtrl |= 0x550;
	for(;;)
	{
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DEVICE, spdType & 0xFF );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_ADDR, regAdr );
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_CTRL, spdCtrl );
	for(;;)
	{
#ifdef __linux__
        IPC_delay(1);
#endif
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}
	IndRegRead( pModule, m_AdcTetrNum, ADCnr_SPD_DATA, pRegVal );

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm412x500mSrv::SpdWrite( CModule* pModule, U32 spdType, U32 regAdr, U32 regVal )
{
	FM412X500MSRV_Cfg	*pSrvCfg  = (FM412X500MSRV_Cfg*)m_pConfig;
	U32			spdCtrl;
	U32			status;

	spdCtrl  = 0x2;
	if( spdType==SPDdev_GEN )
		spdCtrl |= (pSrvCfg->nGenAdr & 0xFF) << 4;
		//spdCtrl |= 0x490;
		//spdCtrl |= 0x550;
	for(;;)
	{
		DirRegRead( pModule, m_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_SPD_DEVICE, spdType & 0xFF );
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
int	CFm412x500mSrv::Si571SetRate( CModule* pModule, double *pRate )
{
	FM412X500MSRV_Cfg	*pSrvCfg  = (FM412X500MSRV_Cfg*)m_pConfig;
	U32			regData[20];
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
	SpdWrite( pModule, SPDdev_GEN, 135, 0x50 );		// Assert the NewFreq bit

	return status;	
}

//***************************************************************************************
int	CFm412x500mSrv::Si571GetRate( CModule* pModule, double *pRate )
{
	FM412X500MSRV_Cfg	*pSrvCfg  = (FM412X500MSRV_Cfg*)m_pConfig;
	U32			regData[20];
    //ULONG		clkSrc;
	int			regAdr;

	*pRate = 0.0;

	//
	// Проверить источник частоты
	//
    //GetClkSource( pModule, clkSrc );
    //if( clkSrc != BRDclks_ADC_SUBGEN )
    //    return BRDerr_ERROR;

	//
	// Считать регистры Si570/Si571
	//
	for( regAdr=7; regAdr<=12; regAdr++ )
		SpdRead( pModule, SPDdev_GEN, regAdr, &regData[regAdr] );

	SI571_GetRate( pRate, pSrvCfg->dGenFxtal, regData );
	//printf( "Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n", regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );

	return BRDerr_OK;	
}

//***************************************************************************************
int	CFm412x500mSrv::Ad9518SetDividerMode( CModule* pModule, double *pClk, double *pRate )
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
int	CFm412x500mSrv::Ad9518SetPllMode( CModule* pModule, double *pFref, double *pRate )
{
	FM412X500MSRV_Cfg *pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;
	AD9518_TReg		regs[36];
	S32				realSize;
	int				ii;
	S32				err;

	err = AD9518_PllMode( pFref, pRate, regs, 36, &realSize, pSrvCfg->nSyntType );
	if( 0 > err)
		return Sub_Printf( NULL, BRDerr_BAD_PARAMETER, _BRDC( "Ad9518SetPllMode()" ),
						   _BRDC( "Can't set PLL. PLL type=%d, FRef=%f, SRate=%f" ), 
						  pSrvCfg->nSyntType, *pFref, *pRate );

	//
	// Запрограммировать синтезатор 
	//
	for( ii=0; ii<realSize; ii++ )
		SpdWrite( pModule, SPDdev_SYNT, regs[ii].adr, regs[ii].val );

	for( ii=0; ii<realSize; ii++ )
		if( regs[ii].adr>=0x10 && regs[ii].adr<=0x15)
			printf( "Ad9518SetPllMode() reg%X = 0x%X\n", regs[ii].adr, regs[ii].val );

	//
	// Калибровка ГУН
	//
	U32		val;

	IPC_delay(100);
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x01 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(100);
	{
		SpdRead( pModule, SPDdev_SYNT, 0x01F, &val );
		printf( "Ad9518SetPllMode() reg01F = 0x%X\n", val );
	}
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x00 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(100);
	{
		SpdRead( pModule, SPDdev_SYNT, 0x01F, &val );
		printf( "Ad9518SetPllMode() reg01F = 0x%X\n", val );
	}
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x01 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(400);
	{
		SpdRead( pModule, SPDdev_SYNT, 0x01F, &val );
		printf( "Ad9518SetPllMode() reg01F = 0x%X\n", val );
	}

	SpdWrite( pModule, SPDdev_SYNT, 0x0F0, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F1, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F2, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F3, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F4, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F5, 0x04);

	return BRDerr_OK;	
}

//***************************************************************************************
int	CFm412x500mSrv::Ad9518SetDirectPllMode( CModule* pModule, AD9518_TDividers *pDividers )
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
	U32		val;

    IPC_delay(100);
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x01 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(100);
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x00 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(100);
	{
		SpdRead( pModule, SPDdev_SYNT, 0x01F, &val );
		printf( "Ad9518SetDirectPllMode() reg01F = 0x%X\n", val );
	}
	SpdWrite( pModule, SPDdev_SYNT, 0x018, 0x01 );
	SpdWrite( pModule, SPDdev_SYNT, 0x232, 0x01 );
    IPC_delay(400);
	{
		SpdRead( pModule, SPDdev_SYNT, 0x01F, &val );
		printf( "Ad9518SetDirectPllMode() reg01F = 0x%X\n", val );
	}

	SpdWrite( pModule, SPDdev_SYNT, 0x0F0, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F1, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F2, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F3, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F4, 0x04);
	SpdWrite( pModule, SPDdev_SYNT, 0x0F5, 0x04);

	return BRDerr_OK;	
}

//***************************************************************************************
int CFm412x500mSrv::SetAdcStartMode( CModule *pModule, ULONG source, ULONG inv, double *pCmpThr )
{
	FM412X500MSRV_Cfg *pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;

	//
	// Установить источник сигнала старта и инверсию сигнала старта
	//
	ADC_CTRL regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	regCtrl.ByBits.StartSrc = source;
	//regCtrl.ByBits.StartInv = inv;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

	//
	// Дождаться готовности тетрвды MAIN
	//
	U32		regStatus;

	do 
		DirRegRead( pModule, m_MainTetrNum, ADM2IFnr_STATUS, &regStatus );
	while( 0 == (regStatus&0x1) );


	//
	// Установить опорное напряжение для стартовой синхронизации
	//
	S32		code;
	//double  dTmp;

	if( *pCmpThr > 2.5 ) *pCmpThr = 2.5;
	if( *pCmpThr < -2.5 ) *pCmpThr = -2.5;
	code = (S32)( 32767.0 * (*pCmpThr) / 2.5 );
	//dTmp  = (*pCmpThr + 2.5) / 5.0;
	//dTmp *= 32768.0;
	//code  = (U32)dTmp;

	//// если выход за разрядную сетку
	//if( code > 0xFFFF )
	//	code = 0xFFFF;

	SpdWrite( pModule, SPDdev_THDAC, 0, (U32)code );

	pSrvCfg->dStCmpThr = *pCmpThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::GetAdcStartMode( CModule *pModule, PULONG pSource, PULONG pInv, double *pCmpThr )
{
	FM412X500MSRV_Cfg	*pSrvCfg = (FM412X500MSRV_Cfg*)m_pConfig;
	ADC_CTRL regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	*pSource = regCtrl.ByBits.StartSrc;
	*pInv    = 0;	//regCtrl.ByBits.StartInv;

	*pCmpThr = pSrvCfg->dStCmpThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetSpecific( CModule *pModule, BRD_AdcSpec *pSpec )
{
	int status;

	status = CAdcSrv::SetSpecific(pModule, pSpec);

	switch(pSpec->command)
	{
		case FM412X500Mcmd_SETMU:
			status = SetMu(pModule, pSpec->arg);
			break;
		case FM412X500Mcmd_GETMU:
			status = GetMu(pModule, pSpec->arg);
			break;
		case FM412X500Mcmd_GETEVENTCNT:
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
		case FM412X500Mcmd_RESETEVENTCNT:
			IndRegWrite( pModule, m_AdcTetrNum, ADCnr_EVENTCNTL, (ULONG)0 );
			break;
		case FM412X500Mcmd_SUBSYNC:
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
int CFm412x500mSrv::SetMu( CModule *pModule, void *args )
{
	PFM412X500MSRV_MU pMU = (PFM412X500MSRV_MU)args;

	//
	// Проверить тип субмодуля в параметрах
	//
	if( pMU->subType != 0 )
		return BRDerr_BAD_PARAMETER;

	CAdcSrv::SetChanMask(pModule, pMU->chanMask);
	SetMaster(pModule, pMU->syncMode);

	CtrlFormat( pModule, NULL, BRDctrl_ADC_SETFORMAT, &pMU->format );

	//
	// Частота дискретизации
	//
	SetClkSource(pModule, pMU->clockSrc);
	SetClkValue(pModule, pMU->clockSrc, pMU->clockValue);
	SetRate(pModule, pMU->samplingRate, pMU->clockSrc, pMU->clockValue);

	//
	// Стартовая синхронизация
	//
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
	SetInpResist( pModule, pMU->startResist, 1024 );

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
int CFm412x500mSrv::GetMu( CModule *pModule, void *args )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::SetStartSlave(CModule* pModule, ULONG StartSlave)
{
	//
	// Установить/обнулить бит CONTROL1[START_SLAVE(15)]
	//
	U32			value;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &value );
	value &= ~(1<<15);
	if( StartSlave )
		value |= 1<<15;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, value );
	//printf( ">>> SetStartSlave(%d) code=0x%X\n", StartSlave, value );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm412x500mSrv::GetStartSlave(CModule* pModule, ULONG& refStartSlave)
{
	//
	// Считать бит CONTROL1[START_SLAVE(15)]
	//
	U32			value;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &value );
	refStartSlave = 0x1 & (value>>15);

	return BRDerr_OK;
}


// ***************** End of file Fm412x500mSrv.cpp ***********************
