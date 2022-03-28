/*
 ***************** File Fm814x250mSrv.cpp ************
 *
 * BRD Driver for ADС service on FM814x250M
 *
 * (C) InSys by Ekkore Aug 2011
 *
 ******************************************************
*/

//#include <conio.h>
#include "module.h"
#include "realdelay.h"
#include "gipcy.h"
#include "AdcFm814x250mSrv.h"

#define	CURRFILE "FM814X250MSRV"

const double BRD_ADC_MAXGAINTUN	= 1.; // used with CAdcSrv::SetGainTuning()

//***************************************************************************************
CFm814x250mSrv::CFm814x250mSrv(int idx, int srv_num, CModule* pModule, FM814X250MSRV_Cfg *pCfg) :
	CAdcSrv(idx, _BRDC("FM814X250M"), srv_num, pModule, pCfg, sizeof(FM814X250MSRV_Cfg))
{
}

//***************************************************************************************
int CFm814x250mSrv::CtrlRelease( void *pDev, void *pServData, ULONG cmd, void *args )
{
	CModule* pModule = (CModule*)pDev;

	SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);

	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб (вместо BRDerr_OK)
}

//***************************************************************************************
void CFm814x250mSrv::GetAdcTetrNum( CModule *pModule )
{
	//
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	//

	//
	// Поиск номера терады. Если тетрада не найдена, то -1.
	//
	m_AdcTetrNum = GetTetrNum(pModule, m_index, FM814X250M_TETR_ID);
}

//***************************************************************************************
void CFm814x250mSrv::GetAdcTetrNumEx( CModule *pModule, ULONG TetrInstNum)
{
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	// Поиск номера терады. Если тетрада не найдена, то -1.
	m_AdcTetrNum = GetTetrNumEx(pModule, m_index, FM814X250M_TETR_ID, TetrInstNum);
}

//***************************************************************************************
void CFm814x250mSrv::FreeInfoForDialog( PVOID pInfo )
{
}

//***************************************************************************************
PVOID CFm814x250mSrv::GetInfoForDialog( CModule *pModule )
{
	return NULL;
}

//***************************************************************************************
int CFm814x250mSrv::SetPropertyFromDialog( void *pDev, void *args )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::SetProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	FM814X250MSRV_Cfg	*pSrvCfg = (FM814X250MSRV_Cfg*)m_pConfig;
	BRDCHAR			Buffer[128];
	BRDCHAR			ParamName[128];
	BRDCHAR*		endptr;
	int				ii;

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
	pSrvCfg->nSubExtClk = ROUND(clkValue);
	SetClkValue(pModule, BRDclks_ADC_EXTCLK, clkValue);

	//
	// Извлечение частоты генератора базового модуля
	//
	IPC_getPrivateProfileString(SectionName, _BRDC("BaseClockValue"), _BRDC("1000.0"), Buffer, sizeof(Buffer), iniFilePath);
	clkValue = BRDC_atof(Buffer);
	SetClkValue(pModule, BRDclks_ADC_BASEGEN, clkValue);

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
	DWORD			ret;

	for( ii = 0; ii < MAX_ADC_CHAN; ii++ )
	{
		BRDC_sprintf( ParamName, _BRDC("Bias%d"), ii);
		ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC("0.0"), Buffer, sizeof(Buffer), iniFilePath);
		dVal = BRDC_atof(Buffer);
		SetBias(pModule, dVal, ii);
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
	SetStartMode(pModule, &rStMode);

	//
	// Отладка. Программирование ИПНов
	//
	for( ii = 0; ii < THDAC_CNT; ii++ )
	{
		BRDC_sprintf( ParamName, _BRDC("Thdac%d"), ii);
		ret = IPC_getPrivateProfileString(SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
		if( Buffer[0] )
		{
			dVal = BRDC_atof(Buffer);
			SetThdac( pModule, ii, dVal );
		}
	}

	//
	// Для НИЩИРЕТА
	//
	IPC_getPrivateProfileString(SectionName, _BRDC("RateDivider"), _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
	if( Buffer[0] )
	{
		U32		regVal, cod, divider;

		divider = BRDC_atoi(Buffer);
		switch( divider )
		{
			case 2:  cod = 0x1; break;
			case 4:  cod = 0x2; break;
			default: cod = 0x0; break;
		}
		IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regVal );
		regVal &= ~0x180;
		regVal |= cod<<7;
		IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regVal );
	}

	for( ii = 0; ii < 0x3A; ii++ )
	{
		BRDC_sprintf( ParamName, _BRDC("Reg%X"), ii);
		IPC_getPrivateProfileString(SectionName, ParamName, _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
		if( Buffer[0] )
		{
			U32		regVal;
			regVal  = BRDC_strtol(Buffer, &endptr, 0);
			regVal &= 0x1F;
			SpdWrite( pModule, SPDdev_ADC0, ii, regVal );
			SpdWrite( pModule, SPDdev_ADC0, 0xFF, 1 );
			SpdWrite( pModule, SPDdev_ADC1, ii, regVal );
			SpdWrite( pModule, SPDdev_ADC1, 0xFF, 1 );
			SpdWrite( pModule, SPDdev_ADC2, ii, regVal );
			SpdWrite( pModule, SPDdev_ADC2, 0xFF, 1 );
			SpdWrite( pModule, SPDdev_ADC3, ii, regVal );
			SpdWrite( pModule, SPDdev_ADC3, 0xFF, 1 );

			printf( ">>>> Write %d(0x%X) to ADC Reg%X\n", regVal, regVal, ii );
		}
	}
	//GetPrivateProfileString(SectionName, _BRDC("Reg18"), _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
	//if( Buffer[0] )
	//{
	//	U32		reg18;
	//	reg18  = BRDC_strtol(Buffer, &endptr, 0);
	//	reg18 &= 0x1F;
	//	SpdWrite( pModule, SPDdev_ADC0, 0x18, reg18 );
	//	SpdWrite( pModule, SPDdev_ADC0, 0xFF, 1 );
	//	SpdWrite( pModule, SPDdev_ADC1, 0x18, reg18 );
	//	SpdWrite( pModule, SPDdev_ADC1, 0xFF, 1 );
	//	SpdWrite( pModule, SPDdev_ADC2, 0x18, reg18 );
	//	SpdWrite( pModule, SPDdev_ADC2, 0xFF, 1 );
	//	SpdWrite( pModule, SPDdev_ADC3, 0x18, reg18 );
	//	SpdWrite( pModule, SPDdev_ADC3, 0xFF, 1 );
	//}

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
int CFm814x250mSrv::SaveProperties( CModule *pModule, BRDCHAR *iniFilePath, BRDCHAR *SectionName )
{
	BRDCHAR				ParamName[128];
	FM814X250MSRV_Cfg	*pSrvCfg = (FM814X250MSRV_Cfg*)m_pConfig;

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
int CFm814x250mSrv::SetClkSource( CModule *pModule, ULONG nClkSrc )
{
	FM814X250MSRV_Cfg	*pSrvCfg = (FM814X250MSRV_Cfg*)m_pConfig;
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
	RealDelay(100,1);
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, fsrc.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::GetClkSource( CModule *pModule, ULONG &refClkSrc )
{
	FM814X250MSRV_Cfg	*pSrvCfg = (FM814X250MSRV_Cfg*)m_pConfig;
	ADM2IF_FSRC			fsrc;

	//
	// Определить режим работы тетрады: Single или Slave
	//
	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_FSRC, &fsrc.AsWhole );
	refClkSrc = fsrc.ByBits.Gen;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::SetClkValue( CModule *pModule, ULONG nClkSrc, double &refClkValue )
{
	FM814X250MSRV_Cfg	*pSrvCfg = (FM814X250MSRV_Cfg*)m_pConfig;
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
			pSrvCfg->nSubExtClk = ROUND(refClkValue);
			break;
		case BRDclks_ADC_BASEGEN:		// Base Unit Clock CLK0_C2M
			pSrvCfg->nBaseUnitClk = ROUND(refClkValue);
			break;
		default:
			refClkValue = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}
	return err;
}
	
//***************************************************************************************
int CFm814x250mSrv::GetClkValue( CModule *pModule, ULONG nClkSrc, double &refClkValue )
{
	FM814X250MSRV_Cfg	*pSrvCfg = (FM814X250MSRV_Cfg*)m_pConfig;
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
			refClkValue = pSrvCfg->nSubExtClk;
			break;
		case BRDclks_ADC_BASEGEN:		// Base Unit Clock CLK0_C2M
			refClkValue = pSrvCfg->nBaseUnitClk;
			break;
		default:
			refClkValue = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}

	return err;
}
	
//***************************************************************************************
int CFm814x250mSrv::SetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;
	CAdcSrv::SetClkLocation(pModule, refMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::GetClkLocation( CModule *pModule, ULONG &refMode )
{
	refMode = 1;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::SetExtClkThr(CModule* pModule, double& ClkThr)
{

return BRDerr_OK;

	FM814X250MSRV_Cfg  *pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;

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
int CFm814x250mSrv::GetExtClkThr(CModule* pModule, double& ClkThr)
{
	FM814X250MSRV_Cfg  *pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;

	ClkThr = pSrvCfg->ExtClkThr;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::SetRate( CModule *pModule, double &refRate, ULONG nClkSrc, double ClkValue )
{
	FM814X250MSRV_Cfg	*pSrvCfg = (FM814X250MSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;
	double				dClkValue;

	U32					regVal, cod = 0x0;
	double				divider = 1.0;

	if( refRate > pSrvCfg->AdcCfg.MaxRate )
		refRate = pSrvCfg->AdcCfg.MaxRate;
	if( refRate < pSrvCfg->AdcCfg.MinRate )
		refRate = pSrvCfg->AdcCfg.MinRate;

	SetClkSource( pModule, nClkSrc );
	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
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
				Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
				if( dClkValue != refRate )
				{
					dClkValue = refRate;
					err = Si571SetRate( pModule, &dClkValue ); // Запрограммировать Si570/Si571
					refRate = dClkValue;
				}
			}
			break;
		case BRDclks_ADC_EXTCLK:		// External clock
			//
			// Использовать дополнительный делитель частоты
			// только при тактировании от внешнего генератора
			//
			divider = pSrvCfg->nSubExtClk / refRate;
			if( divider <= 1.1 )		{divider = 1.0; cod = 0x0; }
			else if( divider <= 2.1 )	{divider = 2.0; cod = 0x1; }
			else						{divider = 4.0; cod = 0x2; }

			refRate = pSrvCfg->nSubExtClk;
			break;
		case BRDclks_ADC_BASEGEN:		// Base Unit Clock CLK0_C2M
			refRate = pSrvCfg->nBaseUnitClk;
			break;
		default:
			refRate = 0.0;
			err = BRDerr_BAD_PARAMETER;
			break;
	}
	//
	// Запрограммировать дополнительный делитель частоты
	//
	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regVal );
	regVal &= ~0x180;
	regVal |= cod<<7;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regVal );

	refRate /= divider;

	return err;
}

//***************************************************************************************
int CFm814x250mSrv::GetRate( CModule *pModule, double &refRate, double ClkValue )
{
	FM814X250MSRV_Cfg	*pSrvCfg = (FM814X250MSRV_Cfg*)m_pConfig;
	ULONG				nClkSrc;

	GetClkSource( pModule, nClkSrc );

	switch(nClkSrc)
	{
		case BRDclks_ADC_DISABLED:		// disabled clock
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
				double		dClkValue ;

				Si571GetRate( pModule, &dClkValue ); // Считать Si570/Si571
				refRate = dClkValue;
			}
			break;
		case BRDclks_ADC_EXTCLK:		// External clock

			refRate = pSrvCfg->nSubExtClk;
			break;
		case BRDclks_ADC_BASEGEN:		// Base Unit Clock CLK0_C2M
			refRate = pSrvCfg->nBaseUnitClk;
			break;
		default:
			refRate = 0.0;
			break;
	}

	//
	// Учксть дополнительный делитель частоты
	//
	U32		regVal, cod;
	double	divider;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regVal );
	cod = 0x3 & (regVal>>7);
	if( cod==0x2 )		divider = 4.0;
	else if( cod==0x1 )	divider = 2.0;
	else				divider = 1.0;

	refRate /= divider;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::SetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	FM814X250MSRV_Cfg  *pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;
	double				dacV;

	//
	// Для АЦП имеющего ШП 0.5 Вольта
	//
	if( pSrvCfg->nInpType == 0 && pSrvCfg->nVersion <= 0x11 )
	{
		//
		// Для закрытого входа 
		// Версия субмодуля 1.0, 1.1
		//
		// Нищерет намерял:
		//
		//		A = 0.258*D - 7000; 
		//      где A - смещение 0 в ЕМРах (-8192 .. +8191)
		//          D - код ЦАПа смещения нуля (0 .. +65535)
		//
		// При пересчете в Вольты с учетом что Uреф = 5.0 В
		//
		//   0.0  В в ЦАП --> -7000 ЕМР (-85% от ШП)
		//  +2.07 В в ЦАП --> 0 ЕМР
		//  +5.0  В в ЦАП --> +9908 ЕМР (+121% от ШП)
		//
		if( Chan > MAX_ADC_CHAN )
			return BRDerr_BAD_PARAMETER;

		if( refBias < -85.0 )
			refBias = -85.0;
		if( refBias > 121.0 )
			refBias = 121.0;

		dacV  = refBias + 85.0;
		dacV /= 85.0 + 121.0;
		dacV *= 5.0;
		//printf( "SetBias() close input, bias = %f %, thdac = %f V\n", refBias, dacV );
	}
	else
	if( pSrvCfg->nInpType == 1 && pSrvCfg->nVersion <= 0x11 )
	{
		//
		// Для открытого входа
		// Версия субмодуля 1.0, 1.1
		//
		// Нищерет намерял:
		//
		//		A = 0.237*D - 6685; 
		//      где A - смещение 0 в ЕМРах (-8192 .. +8191)
		//          D - код ЦАПа смещения нуля (0 .. +65535)
		//
		// При пересчете в Вольты с учетом что Uреф = 5.0 В
		//
		//   0.0  В в ЦАП --> -6685 ЕМР (-82% от ШП)
		//  +2.15 В в ЦАП --> 0 ЕМР
		//  +5.0  В в ЦАП --> +8847 ЕМР (+108% от ШП)
		//
		if( Chan > MAX_ADC_CHAN )
			return BRDerr_BAD_PARAMETER;

		if( refBias < -82.0 )
			refBias = -82.0;
		if( refBias > 108.0 )
			refBias = 108.0;

		dacV  = refBias + 82.0;
		dacV /= 82.0 + 108.0;
		dacV *= 5.0;
		//printf( "SetBias() open input, bias = %f %, thdac = %f V\n", refBias, dacV );
	}
	else
	if( pSrvCfg->nInpType == 0 )
	{
		//
		// Для закрытого входа 
		// Версия субмодуля 1.2 и выше
		//
		// Нищерет намерял:
		//
		//		A = 0.31162*D - 8780; 
		//      где A - смещение 0 в ЕМРах (-8192 .. +8191)
		//          D - код ЦАПа смещения нуля (0 .. +65535)
		//
		// При пересчете в Вольты с учетом что Uреф = 5.0 В
		//
		//   0.0  В в ЦАП --> -8780 ЕМР (-107% от ШП)
		//  +2.15 В в ЦАП --> 0 ЕМР
		//  +5.0  В в ЦАП --> +11642 ЕМР (+142% от ШП)
		//
		if( Chan > MAX_ADC_CHAN )
			return BRDerr_BAD_PARAMETER;

		if( refBias < -107.0 )
			refBias = -107.0;
		if( refBias > 142.0 )
			refBias = 142.0;

		dacV  = refBias + 107.0;
		dacV /= 107.0 + 142.0;
		dacV *= 5.0;
		//printf( "SetBias() close input V1.2, bias = %f %, thdac = %f V\n", refBias, dacV );
	}
	else
	{
		//
		// Для открытого входа
		// Версия субмодуля 1.2 и выше
		//
		// Нищерет намерял:
		//
		//		A = 0.3004*D - 9036; 
		//      где A - смещение 0 в ЕМРах (-8192 .. +8191)
		//          D - код ЦАПа смещения нуля (0 .. +65535)
		//
		// При пересчете в Вольты с учетом что Uреф = 5.0 В
		//
		//   0.0   В в ЦАП --> -9036 ЕМР (-110% от ШП)
		//  +2.295 В в ЦАП --> 0 ЕМР
		//  +5.0   В в ЦАП --> +10651 ЕМР (+130% от ШП)
		//
		if( Chan > MAX_ADC_CHAN )
			return BRDerr_BAD_PARAMETER;

		if( refBias < -110.0 )
			refBias = -110.0;
		if( refBias > 130.0 )
			refBias = 130.0;

		dacV  = refBias + 110.0;
		dacV /= 110.0 + 130.0;
		dacV *= 5.0;
		//printf( "SetBias() open input V1.2, bias = %f %, thdac = %f V\n", refBias, dacV );
	}

	//
	// Для АЦП имеющего ШП 1.0 Вольт
	//
	if( pSrvCfg->AdcCfg.InpRange >= 900 )
	{
		if( pSrvCfg->nInpType == 0 )
		{
			//
			// Для закрытого входа 
			// Версия субмодуля - любая
			//
			// Нищерет намерял:
			// При пересчете в Вольты с учетом что Uреф = 5.0 В
			//
			//   0.0  В в ЦАП --> -3507 ЕМР (-42.81% от ШП)
			//  +2.2  В в ЦАП --> 0 ЕМР
			//  +5.0  В в ЦАП --> +4470 ЕМР (+54.57% от ШП)
			//
			if( Chan > MAX_ADC_CHAN )
				return BRDerr_BAD_PARAMETER;

			if( refBias < -42.81 )
				refBias = -42.81;
			if( refBias > 54.57 )
				refBias = 54.57;

			dacV  = refBias + 42.81;
			dacV /= 42.81 + 54.57;
			dacV *= 5.0;
			//printf( "SetBias(Inp=1.0 Volt) close input , bias = %f %, thdac = %f V\n", refBias, dacV );
		}
		else
		{
			//
			// Для открытого входа
			//
		}
	}

	err = SetThdac( pModule, Chan, dacV );
	if( 0 <= err )
		pSrvCfg->aBias[Chan] = refBias;

	return err;
}

//***************************************************************************************
int CFm814x250mSrv::GetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	FM814X250MSRV_Cfg  *pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;

	if( Chan > MAX_ADC_CHAN )
		return BRDerr_BAD_PARAMETER;

	refBias = pSrvCfg->aBias[Chan];

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::SetStartMode( CModule *pModule, PVOID pStartStopMode )
{
	int Status = BRDerr_OK;
	BRD_AdcStartMode *pAdcStartMode = (BRD_AdcStartMode*)pStartStopMode;
	double thr = pAdcStartMode->level;

	Status = SetAdcStartMode(pModule, pAdcStartMode->src, pAdcStartMode->inv, &thr);
	if(Status != BRDerr_OK)
		return Status;
	pAdcStartMode->level = thr;

	BRD_StartMode *pStartMode = &pAdcStartMode->stndStart;
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
		IndRegWrite( pModule, m_MainTetrNum, ADM2IFnr_STMODE, stMode.AsWhole );
	}
	return Status;
}

//***************************************************************************************
int CFm814x250mSrv::GetStartMode( CModule *pModule, PVOID pStartStopMode )
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
int CFm814x250mSrv::SetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	refGain = 1.0;

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::GetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	refGain = 1.0;

	return BRDerr_OK;
	FM814X250MSRV_Cfg	*pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;
	int					status;
	double				inpRange;

	status = GetInpRange( pModule, inpRange, Chan );
	if( status >= 0 )
		refGain = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / inpRange;

	return status;
}

//***************************************************************************************
int CFm814x250mSrv::SetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	FM814X250MSRV_Cfg  *pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;

	refInpRange = (double)(pSrvCfg->AdcCfg.InpRange) / 1000.0;
	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::GetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	FM814X250MSRV_Cfg  *pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;

	refInpRange = (double)(pSrvCfg->AdcCfg.InpRange) / 1000.0;
	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::SetCode( CModule *pModule, ULONG type )
{
	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::GetCode( CModule *pModule, ULONG &refType )
{
	refType = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::SetMaster( CModule *pModule, ULONG mode )
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
int CFm814x250mSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	FM814X250MSRV_Cfg	*pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;
	int			err;
	U32					maskMax;

	//
	// Ограничить маску каналов только реально существующими каналами
	//
	maskMax = 1 << pSrvCfg->AdcCfg.MaxChan;
	maskMax -= 1;


	err = CAdcSrv::SetChanMask( pModule, mask & maskMax );

	return err;
}

//***************************************************************************************
int CFm814x250mSrv::ExtraInit(CModule* pModule)
{
	//
	// Начальная инициализация программируемых микросхем: АЦП, ИПН(ЦАП), генератора
	//
	FM814X250MSRV_Cfg	*pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;
	int					ii;
	int					regAdr;

	//
	// Инициализация ИПН(ЦАП)
	//
	for( ii=0; ii<THDAC_CNT; ii++ )
	{
		SetThdac( pModule, ii, 0.0 );
		//printf( "Set THDAC%d\n", ii );
	}
	SetThdac( pModule, 8, 2.02 );		// Подстройка Si571
	SetThdac( pModule, 10, 5.0 );		// Питание коммутатора клоков
	
	//for( int loop=0;;loop++ )
	//{
	//	SetThdac( pModule, 10, 5.0 );		// Питание коммутатора клоков
	//	printf( "Loop = %d. Ch10, ", loop );
	//	getch();
	//	SetThdac( pModule, 2, 2.063 );
	//	printf( "Ch2, ", loop );
	//	getch();
	//	SetThdac( pModule, 11, 1.015 );
	//	printf( "Ch11.\n", loop );
	//	getch();
	//}

	//
	// Определение частоты кварца для генератора Si570/Si571
	//
	pSrvCfg->dGenFxtal = 0.0;
	if( (pSrvCfg->nGenType == 1) && (pSrvCfg->nGenRef != 0) )
	{
		U32			regData[20];
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
		{
			SpdRead( pModule, SPDdev_GEN, regAdr, &regData[regAdr] );
			//printf( "Read Si571 reg %d, val=0x%X\n", regAdr, regData[regAdr] );
		}
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
int	CFm814x250mSrv::SetThdac( CModule* pModule, int chan, double voltage )
{
	FM814X250MSRV_Cfg	*pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;
	U32			spdType, regAdr, regVal;
	double dVmax = 5.0;

	if( chan<0 || chan>=THDAC_CNT )
		return BRDerr_BAD_PARAMETER;

	spdType = (chan<8) ? SPDdev_THDAC0 : SPDdev_THDAC1;
	regAdr  = 0x7 & chan;
	if ((chan == 11) || (chan == 12))
		dVmax = 4.096;
	if( voltage > dVmax)
		voltage = dVmax;
	if( voltage < 0.0 )
		voltage = 0.0;
	//BRDC_printf(_BRDC("SET DAC LEVEL %f\n"), voltage);
	regVal = (U32)(65535.0 * voltage / dVmax);

	SpdWrite( pModule, spdType, regAdr, regVal );
	pSrvCfg->aThdacVotage[chan] = voltage;

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm814x250mSrv::GetThdac( CModule* pModule, int chan, double *pVoltage )
{
	FM814X250MSRV_Cfg	*pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;

	if( chan<0 || chan>=THDAC_CNT )
		return BRDerr_BAD_PARAMETER;

	*pVoltage = pSrvCfg->aThdacVotage[chan];

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm814x250mSrv::SpdRead(  CModule* pModule, U32 spdType, U32 regAdr, U32 *pRegVal )
{
	//
	// Типы SPD (spdType): 
	//   0-ИПН0, 1-ИПН1, 2-генератор, 3-АЦП0, 4-АЦП1, 5-АЦП2, 6-АЦП3 
	//
	FM814X250MSRV_Cfg	*pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;
	U32			spdCtrl;
	U32			status;

	spdCtrl  = 0x1;
	switch(spdType)
	{
		case SPDdev_GEN:	spdCtrl |= (pSrvCfg->nGenAdr & 0xFF) << 4;	break;
		//default:			spdCtrl |= spdType & 0xF0;
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

	return BRDerr_OK;
}

//***************************************************************************************
int	CFm814x250mSrv::SpdWrite( CModule* pModule, U32 spdType, U32 regAdr, U32 regVal )
{
	FM814X250MSRV_Cfg	*pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;
	U32			spdCtrl;
	U32			status;

	spdCtrl  = 0x2;
	switch(spdType)
	{
		case SPDdev_GEN:	spdCtrl |= (pSrvCfg->nGenAdr & 0xFF) << 4;	break;
		//default:			spdCtrl |= spdType & 0xF0;
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
int	CFm814x250mSrv::Si571SetRate( CModule* pModule, double *pRate )
{
	FM814X250MSRV_Cfg	*pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;
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
	SpdWrite( pModule, SPDdev_GEN, 135, 0x40 );		// Assert the NewFreq bit

	return status;	
}

//***************************************************************************************
int	CFm814x250mSrv::Si571GetRate( CModule* pModule, double *pRate )
{
	FM814X250MSRV_Cfg	*pSrvCfg  = (FM814X250MSRV_Cfg*)m_pConfig;
	U32			regData[20];
	ULONG		clkSrc;
	int			regAdr;

	*pRate = 0.0;

	//
	// Проверить источник частоты
	//
	GetClkSource( pModule, clkSrc );
	if( clkSrc != BRDclks_ADC_SUBGEN )
        return BRDerr_ERROR;

	//
	// Считать регистры Si570/Si571
	//
	for( regAdr=7; regAdr<=12; regAdr++ )
		SpdRead( pModule, SPDdev_GEN, regAdr, &regData[regAdr] );

	SI571_GetRate( pRate, pSrvCfg->dGenFxtal, regData );

	return BRDerr_OK;	
}

//***************************************************************************************
int CFm814x250mSrv::SetAdcStartMode( CModule *pModule, ULONG source, ULONG inv, double *pCmpThr )
{
	FM814X250MSRV_Cfg *pSrvCfg = (FM814X250MSRV_Cfg*)m_pConfig;
	double			inputRange = (double)(pSrvCfg->AdcCfg.InpRange) / 1000.0;
	double			dacV;
	double			dCode;

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
	//

	//
	// Старт от канала АЦП0
	//
	if( BRDsts_ADC_CHAN0 == source )
	{
		if( pSrvCfg->nVersion <= 0x11 )
		{
			// Версия субмодуля 1.0, 1.1
			// Для АЦП имеющего ШП 0.5 Вольт
			//
			// Нищерет намерил:
			// ЦАП11 +1.53 В --> порог +ШП
			// ЦАП11 +1.015В --> порог 0 В
			// ЦАП11 +0.50 В --> порог -ШП
			//
			if( *pCmpThr < -inputRange )
				*pCmpThr = -inputRange;
			if( *pCmpThr > inputRange )
				*pCmpThr = inputRange;

			dacV  = *pCmpThr + inputRange;
			dacV /= 2 * inputRange;
			dacV *= 1.53 - 0.50;
			dacV += 0.50;
		}
		else
		{
			// Версия субмодуля 1.2 и выше
			// Для АЦП имеющего ШП 0.5 Вольт
			//
			// Нищерет намерил:
			// D = 3.3 * 10000 * Uвх + 32768

			dCode = 33000.0 * (*pCmpThr) + 32768.0;
			if( dCode < 0.0 ) 
				dCode = 0.0;
			if( dCode > 65535.0 ) 
				dCode = 65535.0;
			*pCmpThr = (dCode - 32768.0) / 33000.0;
			dacV  = dCode * 5.0 / 65535.0;
		}
		if( pSrvCfg->AdcCfg.InpRange >= 900 )
		{
			//
			// Версия субмодуля - любая
			// Для АЦП имеющего ШП 1.0 Вольт
			//
			// Нищерет намерил:
			// ЦАП11 48760 emr --> порог +7351 emr АЦП
			// ЦАП11 33686 emr --> порог     0 emr АЦП
			// ЦАП11 17563 emr --> порог -7990 emr АЦП
			//
			// Значит:
			//   0.0  В в ЦАП11 --> -16626 ЕМР (-203.6% от ШП)
			//  +5.0  В в ЦАП11 --> +15600 ЕМР (+190.4% от ШП)
			//
			double		percent = *pCmpThr * 100.0 / inputRange;

			if( percent < -203.6 )
				percent = -203.6;
			if( percent > 190.4 )
				percent = 190.4;
			
			*pCmpThr = inputRange * percent / 100.0;

			dacV  = percent + 203.6;
			dacV /= 203.6 + 190.4;
			dacV *= 5.0;
		}

		SetThdac( pModule, 11, dacV );
		pSrvCfg->StartThdac11 = *pCmpThr;
	}

	//
	// Старт от внешнего разъема START
	//
	if( BRDsts_ADC_EXT == source)
	{
		if( pSrvCfg->nVersion <= 0x11 )
		{
			// Версия субмодуля 1.0, 1.1
			//
			//
			// Нищерет намерил:
			// ЦАП12 +5.00 В --> порог -2.7 В
			// ЦАП12 +3.22 В --> порог 0 В
			// ЦАП12 +0.00 В --> порог 4.9 В
			//
			// Реальный выход ЦАП меньше на 20% (умножить на 0.8)
			//
			// Нищерет изменил формулу:
			// ЦАП12 +5.00 В --> порог +4.9 В
			// ЦАП12 +0.00 В --> порог -2.7 В
			//
			if( *pCmpThr < -2.7 )
				*pCmpThr = -2.7;
			if( *pCmpThr > 4.9 )
				*pCmpThr = 4.9;

			//dacV  = *pCmpThr - 4.9;
			//dacV *= -1.0;
			dacV  = *pCmpThr + 2.7;
			dacV /= 4.9 + 2.7;
			dacV *= 5.0;
			//printf( "ThDac12 = %f (%fV) (%d emr)\n", dacV, dacV*0.8, (int)(dacV*65535/5.0) );
		}
		else
		{
		//	// Версия субмодуля 1.2 и выше
		//	//
		//	// Нищерет намерил:
		//	// D = -1.413 * 10000 * Uвх +37264
		//	if( *pCmpThr < -2.64 )
		//		*pCmpThr = -2.64;
		//	if( *pCmpThr > 2.64 )
		//		*pCmpThr = 2.64;

		//	dCode = -14130.0 * (*pCmpThr) + 37264.0;
		//	dacV  = dCode * 5.0 / 65535.0;
		//	//printf( "ThDac12 = %f (%fV) (%d emr)\n", dacV, dacV*0.8, (int)(dacV*65535/5.0) );
		//}
		//if( pSrvCfg->AdcCfg.InpRange >= 900 )
		//{
		//	//
		//	// Версия субмодуля - любая
		//	// Для АЦП имеющего ШП 1.0 Вольт
		//	//
		//	// Нищерет намерил:
		//	// ЦАП12 24248 emr --> порог +8008 emr АЦП
		//	// ЦАП12 37724 emr --> порог     0 emr АЦП
		//	// ЦАП12 50463 emr --> порог -7853 emr АЦП
		//	//
		//	// Значит:
		//	//   0.0  В в ЦАП12 --> +22678 ЕМР (+276.8% от ШП)
		//	//  +5.0  В в ЦАП12 --> -16972 ЕМР (-207.2% от ШП)
		//	//
		//	double		percent = *pCmpThr * 100.0 / inputRange;

		//	if( percent < -207.2 )
		//		percent = -207.2;
		//	if( percent > 276.8 )
		//		percent = 276.8;

		//	*pCmpThr = inputRange * percent / 100.0;

		//	dacV  = percent + 207.2;
		//	dacV /= 207.2 + 276.8;
		//	dacV *= 5.0;
		//	dacV = 5.0 - dacV;			// Изменить наклон графика

			//reference 4V
			dacV = 0.46 * (*pCmpThr) + 2.285;
			*pCmpThr = dacV;
		}
		SetThdac( pModule, 12, dacV );
		pSrvCfg->StartThdac12 = *pCmpThr;
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::GetAdcStartMode( CModule *pModule, PULONG pSource, PULONG pInv, double *pCmpThr )
{
	FM814X250MSRV_Cfg	*pSrvCfg = (FM814X250MSRV_Cfg*)m_pConfig;
	ADC_CTRL regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	*pSource = regCtrl.ByBits.StartSrc;
	*pInv    = 0;	//regCtrl.ByBits.StartInv;

	switch( *pSource )
	{
		case 0: *pCmpThr = pSrvCfg->StartThdac11; break;
		case 1: *pCmpThr = pSrvCfg->StartThdac12; break;
		default: *pCmpThr = 0.0; break;
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CFm814x250mSrv::SetSpecific( CModule *pModule, BRD_AdcSpec *pSpec )
{
	int status;

	status = CAdcSrv::SetSpecific(pModule, pSpec);

	switch(pSpec->command)
	{
		case FM814X250Mcmd_SETMU:
			status = SetMu(pModule, pSpec->arg);
			break;
		case FM814X250Mcmd_GETMU:
			status = GetMu(pModule, pSpec->arg);
			break;
	}
	return status;
}

//***************************************************************************************
int CFm814x250mSrv::SetMu( CModule *pModule, void *args )
{
	FM814X250MSRV_Cfg	*pSrvCfg = (FM814X250MSRV_Cfg*)m_pConfig;
	FM814X250MSRV_MU	*pMU = (FM814X250MSRV_MU*)args;

	CAdcSrv::SetChanMask(pModule, pMU->chanMask);
	SetMaster(pModule, pMU->syncMode);

	CtrlFormat( pModule, NULL, BRDctrl_ADC_SETFORMAT, &pMU->format );

	SetClkSource(pModule, pMU->clockSrc);
	pSrvCfg->nSubExtClk = ROUND(pMU->clockValue);
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
int CFm814x250mSrv::GetMu( CModule *pModule, void *args )
{
	return BRDerr_OK;
}


// ***************** End of file Fm814x250mSrv.cpp ***********************
