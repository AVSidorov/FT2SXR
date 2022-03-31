/*
 ***************** File Adc214x10mSrv.cpp ************
 *
 * BRD Driver for ADС service on ADM214x10M(WB) and ADM212x500M(WB)
 *
 * (C) InSys by Ekkore Aug 2008
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "Adc214x10mSrv.h"

#define	CURRFILE "ADC214x10MSRV"

const double BRD_ADC_MAXGAINTUN	= 1.; // not used

//***************************************************************************************
CAdc214x10mSrv::CAdc214x10mSrv(int idx, int srv_num, CModule* pModule, PADC214x10MSRV_Cfg pCfg) :
	CAdcSrv(idx, _BRDC("ADC214X10M"), srv_num, pModule, pCfg, sizeof(ADC214x10MSRV_Cfg))
{
}

//***************************************************************************************
int CAdc214x10mSrv::CtrlRelease( void *pDev, void *pServData, ULONG cmd, void *args )
{
	CModule* pModule = (CModule*)pDev;

	CAdcSrv::SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);
//	return BRDerr_OK;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CAdc214x10mSrv::GetAdcTetrNum(CModule* pModule)
{
	//
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	//

	//
	// Поиск номера терады. Если тетрада не найдена, то -1.
	//
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC214x10M_TETR_ID);
	if( -1 == m_AdcTetrNum )
		m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC214x10MX_TETR_ID);
}

//***************************************************************************************
void CAdc214x10mSrv::FreeInfoForDialog(PVOID pInfo)
{
	ADC214x10MSRV_Info *pSrvInfo = (ADC214x10MSRV_Info*)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc214x10mSrv::GetInfoForDialog(CModule* pDev)
{
	ADC214x10MSRV_Cfg  *pSrvCfg  = (ADC214x10MSRV_Cfg*)m_pConfig;
	ADC214x10MSRV_Info *pSrvInfo = new ADC214x10MSRV_Info;

	pSrvInfo->Size = sizeof(ADC214x10MSRV_Info);

	UCHAR code = pSrvCfg->AdcCfg.Encoding;
	
	pSrvInfo->pAdcInfo = (ADCSRV_INFO*)CAdcSrv::GetInfoForDialog(pDev);
/*
	if(pSrvInfo->pAdcInfo->SyncMode == 3)
		pSrvInfo->pAdcInfo->SyncMode = 1;
	pSrvInfo->pAdcInfo->Encoding = code;
	//pSrvInfo->ExtClk = pSrvCfg->SubExtClk;
	GetClkSource(pDev, pSrvInfo->pAdcInfo->ClockSrc);
	GetClkValue(pDev, pSrvInfo->pAdcInfo->ClockSrc, pSrvInfo->pAdcInfo->ClockValue);
	GetRate(pDev, pSrvInfo->pAdcInfo->SamplingRate, pSrvInfo->pAdcInfo->ClockValue);
	if(pSrvInfo->pAdcInfo->ClockSrc == BRDclks_ADC_EXTCLK)
		pSrvInfo->ExtClk = pSrvInfo->pAdcInfo->ClockValue;

	ULONG mode;
	
	GetClkInv(pDev, mode);
	pSrvInfo->InvClk = (UCHAR)mode;
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

	pSrvInfo->Version  = pSrvCfg->nVersion;
	//pSrvInfo->nSubType = pSrvCfg->nSubType;
	//pSrvInfo->aRefGen[0] = pSrvCfg->aRefGen[0];
	//pSrvInfo->aRefGen[1] = pSrvCfg->aRefGen[1];
*/

	return pSrvInfo;
}

//***************************************************************************************
int CAdc214x10mSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
/*
	CModule* pModule = (CModule*)pDev;
	PADC214x10MSRV_Info pInfo = (PADC214x10MSRV_Info)args;

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

	ADC214x10MSRV_Cfg *pAdcSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;

	//pAdcSrvCfg->SubExtClk = ROUND(pInfo->ExtClk);
	SetClkSource(pModule, pInfo->pAdcInfo->ClockSrc);
	SetClkValue(pModule, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);
	SetRate(pModule, pInfo->pAdcInfo->SamplingRate, pInfo->pAdcInfo->ClockSrc, pInfo->pAdcInfo->ClockValue);

	SetClkInv(pModule, pInfo->InvClk);

	for(int i = 0; i < MAX_CHAN; i++)
	{
		SetInpRange(pModule, pInfo->pAdcInfo->Range[i], i);
		SetBias(pModule, pInfo->pAdcInfo->Bias[i], i);
		SetInpResist(pModule, pInfo->pAdcInfo->Resist[i], i);
		SetDcCoupling(pModule, pInfo->pAdcInfo->DcCoupling[i], i);
		SetGainTuning(pModule, pInfo->GainTuning[i], 0);
	}

	BRD_AdcStartMode start_mode;

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
*/

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::SetProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	ADC214x10MSRV_Cfg	*pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	TCHAR	Buffer[128];
	TCHAR	ParamName[128];
	TCHAR	*endptr;
	int		ii;
	int		inputMax = 8;		// 8 или 16. Количество входов у АЦП (ширина мультиплексора) 
	int		ret = BRDerr_OK;
	int		err0, err1;

	if( pSrvCfg->nSubType != 0 )
		inputMax = 16;				// если ADP214x10MX

	//
	// Извлечение параметров работы субмодуля из INI-файла 
	// и инициализация субмодуля в соответствии с этими параметрами
	//


	//
	// Извлечение стандартных параметров
	//
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);

	//
	// Извлечение маски каналов
	//
	U32		chanMask;

	GetPrivateProfileString(SectionName, "ChannelMask", "1", Buffer, sizeof(Buffer), iniFilePath);
	chanMask = strtoul(Buffer, &endptr, 0);
	err0 = SetChanMask(pDev, chanMask);
	
	//
	// Проверить маску каналов. Выбрать канал 0
	//
	if( err0<0 )
	{
		chanMask = 0x1;
		SetChanMask(pDev, chanMask);
	}


	//
	// Извлечение частоты дискретизации на приведенной к 1 входу
	//
	ULONG clkSrc;
	double clkValue;
	double rate;

	err0 = GetClkSource( pDev, clkSrc);
	err1 = GetClkValue( pDev, clkSrc, clkValue);

	GetPrivateProfileString(SectionName, "SamplingRate", "100000.0", Buffer, sizeof(Buffer), iniFilePath);
	rate = atof(Buffer);
	if( err0>=0 && err1>=0 )
		SetRate( pDev, rate, clkSrc, clkValue );

	//
	// Извлечение для каждого входа требуемой шкалы преобразования
	//
	double			inpRange;
	double			gain;

	for( ii = 0; ii < (2*inputMax); ii++ )
	{
		sprintf_s( ParamName, "InputRange%d", ii );
		GetPrivateProfileString( SectionName, ParamName, "", Buffer, sizeof(Buffer), iniFilePath );
		if( Buffer[0] != '\0' )
		{
			inpRange = atof(Buffer);
			SetInpRange( pDev, inpRange, ii );
		}
		else
		{
			sprintf_s( ParamName, "Gain%d", ii );
			GetPrivateProfileString( SectionName, ParamName, "1", Buffer, sizeof(Buffer), iniFilePath );
			gain = atof(Buffer);
			SetGain( pDev, gain, ii );
		}
	}

	//
	// Извлечение смещений нуля для АЦП0 и АЦП1
	//
	double	bias;

	for( ii = 0; ii < 2; ii++ )
	{
		sprintf_s( ParamName, "Bias%d", ii );
		GetPrivateProfileString( SectionName, ParamName, "0.0", Buffer, sizeof(Buffer), iniFilePath);
		bias = atof(Buffer);
		SetBias( pDev, bias, ii * inputMax );
	}


	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	//
	// Эта функция вызывается для формирования ini-файла, содержащего параметры
	// работы субмодуля. Обычно ей пользуется только пример adc_ctrl.exe
	//
	ADC214x10MSRV_Cfg *pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	TCHAR	ParamName[128];
	int		ii;
	int		inputMax = 8;		// 8 или 16. Количество входов у АЦП (ширина мультиплексора) 

	if( pSrvCfg->nSubType != 0 )
		inputMax = 16;				// если ADP214x10MX

	//
	// Формирование стандартных параметров
	//
	// Вызов этой функции приведет к тому, что в ini-файле появятся не только
	// нужные параметры, но и много ненужных параметров
	//
	CAdcSrv::SaveProperties(pDev, iniFilePath, SectionName);

	//
	// Самостоятельное формирование специфических нужных параметров
	//

	//
	// Маска каналов
	//
	ULONG		chanMask;
	
	GetChanMask( pDev, chanMask );
	WriteInifileParam(iniFilePath, SectionName, "ChannelMask", chanMask, 16, NULL);
	
	//
	// Смещение нуля на каждого АЦП
	//
	double	bias;

	for(int ii = 0; ii < 2; ii++ )
	{
		GetBias( pDev, bias, ii * inputMax );
		sprintf_s( ParamName, "Bias%d", ii );
		WriteInifileParam( iniFilePath, SectionName, ParamName, bias, 4, NULL );
	}
	
	//
	// Шкала преобразования для каждого входа
	//
	double	inpRange;

	for( ii = 0; ii < (2*inputMax); ii++ )
	{
		GetInpRange(pDev, inpRange, ii );
		sprintf_s(ParamName, "InputRange%d_%d", ii, ii+1 );
		WriteInifileParam(iniFilePath, SectionName, ParamName, inpRange, 2, NULL);
	}

	// the function flushes the cache
	WritePrivateProfileString(NULL, NULL, NULL, iniFilePath);

	return BRDerr_OK;
}

	
//***************************************************************************************
int CAdc214x10mSrv::SetChanMask( CModule *pModule, ULONG chanMask )
{
	ADC214x10MSRV_Cfg	*pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	U32				adcMask = 0;
	U32				mask;
	U32				val;
	U32				inputsOfAdc0[16], inputsOfAdc1[16];
	int				ii;
	int				inputMax = 8;	// 8 или 16. Количество входов у АЦП (ширина мультиплексора) 
	int				inputCnt[2];	// Количество выбраных входов у каждого АЦП

	if( pSrvCfg->nSubType != 0 )
		inputMax = 16;				// если ADP214x10MX

	for( ii=0; ii<16; ii++ )
		inputsOfAdc0[ii] = inputsOfAdc1[ii] = 0;

	//
	// Пранализировать маску каналов для АЦП0
	//
	mask = (0x1 << inputMax) - 1;
	val = chanMask & mask;
	if( 0 != val )
		adcMask |= 0x1;

	inputCnt[0] = 0;
	for( ii=0; ii<inputMax; ii++ )
		if( val & (1<<ii) )
		{
			inputsOfAdc0[inputCnt[0]] = ii;
			inputCnt[0]++;
		}

	//
	// Пранализировать маску каналов для АЦП1
	//
	mask = (0x1 << inputMax) - 1;
	val = (chanMask>>inputMax) & mask;
	if( 0 != val )
		adcMask |= 0x2;

	inputCnt[1] = 0;
	for( ii=0; ii<inputMax; ii++ )
		if( val & (1<<ii) )
		{
			inputsOfAdc1[inputCnt[1]] = ii;
			inputCnt[1]++;
		}

	//
	// Проверить правильность входных данных
	//
	if( 0x0 == adcMask )
		return BRDerr_BAD_PARAMETER;
	if( (0x3 == adcMask) && (inputCnt[0] != inputCnt[1]) )
		return BRDerr_BAD_PARAMETER;

	//
	// Записать в регистр CONTROL1
	//
	int			cnt;
	ADC_CTRL	regCtrl;

	cnt	= inputCnt[0];
	if( 0 == cnt )
		cnt	= inputCnt[1];

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	regCtrl.ByBits.Cnt = cnt-1;
	IndRegWrite( pModule, m_AdcTetrNum, ADCnr_CTRL1, regCtrl.AsWhole );

	//
	// Записать в регистры MUXDATA
	//
	ADC_MUXDATA	regMuxData;

	for( ii=0; ii<cnt; ii++ )
	{
		regMuxData.AsWhole = 0;

		regMuxData.ByBits.InAdc0   = inputsOfAdc0[ii];
		regMuxData.ByBits.GainAdc0 = 0;
		regMuxData.ByBits.InAdc1   = inputsOfAdc1[ii];
		regMuxData.ByBits.GainAdc1 = 0;
		IndRegWrite( pModule, m_AdcTetrNum, ADC214x10Mnr_MUXDATA0+ii, regMuxData.AsWhole );
		pSrvCfg->aMuxData[ii] = regMuxData.AsWhole;
	}

	//
	// Записать в регистр CHAN1
	//
	ADM2IF_CHAN1	regChan;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_CHAN1, &regChan.AsWhole );
	regChan.ByBits.ChanSel = adcMask;
	IndRegWrite( pModule, m_AdcTetrNum, ADM2IFnr_CHAN1, regChan.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::GetChanMask(CModule *pModule, ULONG &refChanMask)
{
	ADC214x10MSRV_Cfg	*pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	U32				adcMask = 0;
	int				ii;
	int				inputMax = 8;	// 8 или 16. Количество входов у АЦП (ширина мультиплексора) 
	int				inputCnt;		// Количество выбраных входов у каждого АЦП

	if( pSrvCfg->nSubType != 0 )
		inputMax = 16;				// если ADP214x10MX

	//
	// Считать маску выбранных АЦП
	//
	ADM2IF_CHAN1	regChan;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_CHAN1, &regChan.AsWhole );
	adcMask = regChan.ByBits.ChanSel;

	//
	// Считать выбранное количество входов
	//
	ADC_CTRL	regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	inputCnt = 1+regCtrl.ByBits.Cnt;

	//
	// Формировать маску каналов
	//
	ADC_MUXDATA	regMuxData;

	refChanMask = 0;
	for( ii=0; ii<inputCnt; ii++ )
	{
		//IndRegRead( pModule, m_AdcTetrNum, ADC214x10Mnr_MUXDATA0+ii, &regMuxData.AsWhole );
		regMuxData.AsWhole = pSrvCfg->aMuxData[ii];

		//
		// Если разрешен АЦП0
		//
		if( adcMask & 0x1 )
			refChanMask |= 1 << regMuxData.ByBits.InAdc0;

		//
		// Если разрешен АЦП1
		//
		if( adcMask & 0x2 )
			refChanMask |= 1 << (regMuxData.ByBits.InAdc1 + inputMax);
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::GetChanOrder(CModule* pModule, BRD_ItemArray *pItemArray )
{
	ADC214x10MSRV_Cfg	*pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	U32				adcMask = 0;
	U32				chanMask = 0;
	int				ii;
	int				inputMax = 8;	// 8 или 16. Количество входов у АЦП (ширина мультиплексора) 
	int				inputCnt;		// Количество выбраных входов у каждого АЦП
	U32				itemIdx;
	U32				*pItem = (U32*)pItemArray->pItem;

	if( pSrvCfg->nSubType != 0 )
		inputMax = 16;				// если ADP214x10MX

	//
	// Считать маску выбранных АЦП
	//
	ADM2IF_CHAN1	regChan;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_CHAN1, &regChan.AsWhole );
	adcMask = regChan.ByBits.ChanSel;

	//
	// Считать выбранное количество входов
	//
	ADC_CTRL	regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	inputCnt = 1+regCtrl.ByBits.Cnt;

	//
	// Определить количество выбранныхв каналов
	//
	pItemArray->itemReal = 0;
	if( 0x1 & adcMask )
		pItemArray->itemReal += inputCnt;
	if( 0x2 & adcMask )
		pItemArray->itemReal += inputCnt;

	//
	// Формировать список  каналов
	//
	ADC_MUXDATA	regMuxData;

	chanMask = 0;
	itemIdx  = 0;
	for( ii=0; ii<inputCnt; ii++ )
	{
		//IndRegRead( pModule, m_AdcTetrNum, ADC214x10Mnr_MUXDATA0+ii, &regMuxData.AsWhole );
		regMuxData.AsWhole = pSrvCfg->aMuxData[ii];

		//
		// Если разрешен АЦП0
		//
		if( itemIdx > pItemArray->item )
			break;
		if( adcMask & 0x1 )
		{
			chanMask |= 1 << regMuxData.ByBits.InAdc0;
			pItem[ itemIdx++ ] = regMuxData.ByBits.InAdc0;
		}

		//
		// Если разрешен АЦП1
		//
		if( itemIdx > pItemArray->item )
			break;
		if( adcMask & 0x2 )
		{
			chanMask |= 1 << (regMuxData.ByBits.InAdc1 + inputMax);
			pItem[ itemIdx++ ] = regMuxData.ByBits.InAdc1 + inputMax;
		}
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::SetRate( CModule *pModule, double &refRate, ULONG ClkSrc, double ClkValue )
{
	int			inputCnt;

	//
	// Считать выбранное количество входов
	//
	ADC_CTRL	regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	inputCnt = 1+regCtrl.ByBits.Cnt;

	//
	// Пересчитать частоту
	//
	refRate *= inputCnt;
	CAdcSrv::SetRate( pModule, refRate, ClkSrc, ClkValue );
	refRate /= inputCnt;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::GetRate( CModule *pModule, double &refRate, double ClkValue )
{
	int			inputCnt;

	//
	// Считать выбранное количество входов
	//
	ADC_CTRL	regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	inputCnt = 1+regCtrl.ByBits.Cnt;

	//
	// Пересчитать частоту
	//
	CAdcSrv::GetRate( pModule, refRate, ClkValue );
	refRate /= inputCnt;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::SetBias( CModule *pModule, double &Bias, ULONG Chan )
{
	ADC214x10MSRV_Cfg *pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	int				err = BRDerr_OK;
	int				inputMax = 8;	// 8 или 16. Количество входов у АЦП (ширина мультиплексора) 
	int				adcNo;

	if( pSrvCfg->nSubType != 0 )
		inputMax = 16;				// если ADP214x10MX

	adcNo = (Chan < (ULONG)inputMax) ? 0 : 1;

	//
	// Считать с устройства текущую шкалу преобразования
	//
	double inp_range;

	err = GetInpRange(pModule, inp_range, Chan); 
	if( err != BRDerr_OK )
		inp_range = (double)pSrvCfg->AdcCfg.InpRange / 1000.0;	// If this Chan is Masked

	//
	// Рассчитать коды для ИПН
	//
	double	prec_range = inp_range / 100; // точное смещение нуля = 1% от полной шкалы
	int		max_dac_value = 255;
	int		min_dac_value = 0;
	double	half_dac_value = 128.;

	//
	// Рассчитать код грубого ИПН
	//
	USHORT	approx_data;			// код грубого ИПН

	if(fabs(Bias) > inp_range)
		approx_data = Bias > 0.0 ? max_dac_value : min_dac_value;
	else
		approx_data = (USHORT)floor((Bias / inp_range + 1.) * half_dac_value + 0.5);
	if(approx_data > max_dac_value)
		approx_data = max_dac_value;

	//
	// Рассчитать код точного ИПН
	//
	double approx_bias = inp_range * (approx_data / half_dac_value - 1.);
	double delta_bias = Bias - approx_bias;
	USHORT prec_data;			// код точного ИПН

	if(fabs(delta_bias) > prec_range)
		prec_data = delta_bias > 0.0 ? max_dac_value : min_dac_value;
	else
		prec_data = (USHORT)floor((delta_bias / prec_range + 1.) * half_dac_value + 0.5);
	if(prec_data > max_dac_value)
		prec_data = max_dac_value;
	double prec_bias = prec_range * (prec_data / half_dac_value - 1.);

	//
	// Скорректировать смещение нуля
	//
	Bias = approx_bias + prec_bias;

	//
	// Сохранить величину смещения нуля и коды ИПН в структуре m_pConfig
	// Перевести смещение нуля из Вольтов в проценты
	// 
	pSrvCfg->aBias[adcNo] = Bias/inp_range;
	pSrvCfg->AdcCfg.ThrDac[adcNo + BRDtdn_ADC214x10M_BIAS0 - 1] = (UCHAR)approx_data;
	pSrvCfg->AdcCfg.ThrDac[adcNo + BRDtdn_ADC214x10M_PRECBIAS0 - 1] = (UCHAR)prec_data;

	//
	// Выполнить запись в ИПН для установки грубого смещения нуля
	//
	MAIN_THDAC	regThdac;

	regThdac.ByBits.Data = approx_data;
	regThdac.ByBits.Num = adcNo + BRDtdn_ADC214x10M_BIAS0;
	IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, regThdac.AsWhole );


	//
	// Выполнить запись в ИПН для установки точного смещения нуля
	//
	regThdac.ByBits.Data = prec_data;
	regThdac.ByBits.Num = adcNo + BRDtdn_ADC214x10M_PRECBIAS0;
	IndRegWrite( pModule, m_MainTetrNum, MAINnr_THDAC, regThdac.AsWhole );

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::GetBias( CModule *pModule, double &refBias, ULONG Chan )
{
	ADC214x10MSRV_Cfg	*pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;
	int					inputMax = 8;	// 8 или 16. Количество входов у АЦП (ширина мультиплексора) 
	int					adcNo;


	if( pSrvCfg->nSubType != 0 )
		inputMax = 16;				// если ADP214x10MX

	adcNo = (Chan < (ULONG)inputMax) ? 0 : 1;

	//
	// Считать с устройства текущую шкалу преобразования
	//
	double inpRange;

	err = GetInpRange(pModule, inpRange, Chan); 
	if( err != BRDerr_OK )
		inpRange = (double)pSrvCfg->AdcCfg.InpRange;	// If this Chan is Masked

	//
	// Перевести смещегие нуля из процентов в Вольты
	//
	refBias = pSrvCfg->aBias[adcNo] * inpRange;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::GetDcCoupling(CModule* pModule, ULONG& InpType, ULONG Chan)
{
	ADC214x10MSRV_Cfg	*pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	int					err = BRDerr_OK;
	int					inputMax = 8;	// 8 или 16. Количество входов у АЦП (ширина мультиплексора) 


	if( pSrvCfg->nSubType != 0 )
		inputMax = 16;				// если ADP214x10MX
	if( Chan > 2*(ULONG)inputMax )
		return BRDerr_BAD_PARAMETER;

	InpType = 1;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::SetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	ADC214x10MSRV_Cfg	*pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	U32				adcMask = 0;
	U32				adcNo;
	int				status = BRDerr_OK;
	U32				gainCode;
	int				ii;
	int				inputMax = 8;	// 8 или 16. Количество входов у АЦП (ширина мультиплексора) 
	int				inputCnt;		// Количество выбраных входов у каждого АЦП

	if( pSrvCfg->nSubType != 0 )
		inputMax = 16;				// если ADP214x10MX


	//
	// Определить код, загружаемый в регистр и 
	// соответствующий заданному коэффициенту передачи
	//
	if(refGain >= BRD_Gain[BRD_GAINCNT - 1])
	{
		if(refGain > BRD_Gain[BRD_GAINCNT - 1])
			status = BRDerr_WARN;
		gainCode = BRD_GAINCNT - 1;
	}
	else if(refGain <= BRD_Gain[0])
	{
		if(refGain < BRD_Gain[0])
			status = BRDerr_WARN;
		gainCode = 0;
	}
	else 
	{
		for( ii = 0; ii < BRD_GAINCNT-1; ii++ )
		{
			if( (refGain >= BRD_Gain[ii]) && 
				(refGain <= BRD_Gain[ii+1]) )
			{
				if( refGain <= (BRD_Gain[ii] + BRD_Gain[ii+1]) / 2 )
					gainCode = ii;
				else
					gainCode = ii+1;
				break;
			}
		}
	}

	//
	// Скорректировать коэффициент передачи
	//
	refGain = BRD_Gain[gainCode];

	//
	// Считать маску выбранных АЦП
	//
	ADM2IF_CHAN1	regChan;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_CHAN1, &regChan.AsWhole );
	adcMask = regChan.ByBits.ChanSel;

	//
	// Считать выбранное количество входов
	//
	ADC_CTRL	regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	inputCnt = 1+regCtrl.ByBits.Cnt;

	//
	// Проверить номер программируемого канала
	//
	if( Chan >= 2*(U32)inputMax )
		return BRDerr_ADC_ILLEGAL_CHANNEL;

	//
	// Определить номер программируемого АЦП
	//
	adcNo = (Chan < (U32)inputMax) ? 0 : 1;
	if( 0 == (adcMask & (1<<adcNo)) )
		return BRDerr_ADC_ILLEGAL_CHANNEL;

	//
	// Найти канал в массиве регистров MUXDATA и установить коэф. передачи
	//
	ADC_MUXDATA	regMuxData;
	int			isOk = 0;
	int			chanNo;

	for( ii=0; ii<inputCnt; ii++ )
	{
		//
		// Найти канал в массиве регистров MUXDATA
		//
		//IndRegRead( pModule, m_AdcTetrNum, ADC214x10Mnr_MUXDATA0+ii, &regMuxData.AsWhole );
		regMuxData.AsWhole = pSrvCfg->aMuxData[ii];

		if( 0 == adcNo )
			chanNo = regMuxData.ByBits.InAdc0;
		else
			chanNo = regMuxData.ByBits.InAdc1 + inputMax;

		if( chanNo != Chan )
			continue;

		//
		// Установить коэф. передачи
		//
		if( 0 == adcNo )
			regMuxData.ByBits.GainAdc0 = gainCode;
		else
			regMuxData.ByBits.GainAdc1 = gainCode;
		IndRegWrite( pModule, m_AdcTetrNum, ADC214x10Mnr_MUXDATA0+ii, regMuxData.AsWhole );
		pSrvCfg->aMuxData[ii] = regMuxData.AsWhole;
		isOk = 1;
	}
	if( 0 == isOk )
		return BRDerr_ADC_ILLEGAL_CHANNEL;

	return status;
}

//***************************************************************************************
int CAdc214x10mSrv::GetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	ADC214x10MSRV_Cfg	*pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	U32				adcMask = 0;
	U32				adcNo;
	int				status = BRDerr_OK;
	U32				gainCode;
	int				ii;
	int				inputMax = 8;	// 8 или 16. Количество входов у АЦП (ширина мультиплексора) 
	int				inputCnt;		// Количество выбраных входов у каждого АЦП

	if( pSrvCfg->nSubType != 0 )
		inputMax = 16;				// если ADP214x10MX

	//
	// Считать маску выбранных АЦП
	//
	ADM2IF_CHAN1	regChan;

	IndRegRead( pModule, m_AdcTetrNum, ADM2IFnr_CHAN1, &regChan.AsWhole );
	adcMask = regChan.ByBits.ChanSel;

	//
	// Считать выбранное количество входов
	//
	ADC_CTRL	regCtrl;

	IndRegRead( pModule, m_AdcTetrNum, ADCnr_CTRL1, &regCtrl.AsWhole );
	inputCnt = 1+regCtrl.ByBits.Cnt;

	//
	// Проверить номер требуемого канала
	//
	if( Chan >= 2*(U32)inputMax )
		return BRDerr_ADC_ILLEGAL_CHANNEL;

	//
	// Определить номер требуемого АЦП
	//
	adcNo = (Chan < (U32)inputMax) ? 0 : 1;
	if( 0 == (adcMask & (1<<adcNo)) )
		return BRDerr_ADC_ILLEGAL_CHANNEL;

	//
	// Найти канал в массиве регистров MUXDATA и установить коэф. передачи
	//
	ADC_MUXDATA	regMuxData;
	int			isOk = 0;
	int			chanNo;

	for( ii=0; ii<inputCnt; ii++ )
	{
		//
		// Найти канал в массиве регистров MUXDATA
		//
		//IndRegRead( pModule, m_AdcTetrNum, ADC214x10Mnr_MUXDATA0+ii, &regMuxData.AsWhole );
		regMuxData.AsWhole = pSrvCfg->aMuxData[ii];

		if( 0 == adcNo )
			chanNo = regMuxData.ByBits.InAdc0;
		else
			chanNo = regMuxData.ByBits.InAdc1 + inputMax;

		if( chanNo != Chan )
			continue;

		//
		// Установить коэф. передачи
		//
		if( 0 == adcNo )
			gainCode = regMuxData.ByBits.GainAdc0;
		else
			gainCode = regMuxData.ByBits.GainAdc1;
		isOk = 1;
		break;
	}
	if( 0 == isOk )
		return BRDerr_ADC_ILLEGAL_CHANNEL;

	//
	// Сформировать коэффициент передачи
	//
	refGain = BRD_Gain[gainCode];

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::SetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	ADC214x10MSRV_Cfg	*pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	int					status;
	double				fGain;
	double				fAdcInpRange;

	//
	// Определить коэффициент передачи,
	// соответствующий заданной шкале преобразования
	//
	fAdcInpRange = (double)pSrvCfg->AdcCfg.InpRange;	// ADC Chip Input Range (mV)
	fAdcInpRange /= 1000.0;								// ADC Chip Input Range (V)
	fGain = fAdcInpRange / refInpRange;					// Subunit Input Gain

	status = SetGain( pModule, fGain, Chan );

	//
	// Скорректировать шкалу преобразования
	//
	if( status >= 0 )
		refInpRange = fAdcInpRange / fGain;

	return status;
}

//***************************************************************************************
int CAdc214x10mSrv::GetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
{
	ADC214x10MSRV_Cfg	*pSrvCfg = (ADC214x10MSRV_Cfg*)m_pConfig;
	int					status;
	double				fGain;
	double				fAdcInpRange;

	//
	// Узнать текущий коэффициент передачи
	//
	status = GetGain( pModule, fGain, Chan );
	if( status < 0 )
		return status;

	//
	// Пересчитать коэффициент передачи в шкалу преобразования
	//
	fAdcInpRange = (double)pSrvCfg->AdcCfg.InpRange;	// ADC Chip Input Range (mV)
	fAdcInpRange /= 1000.0;								// ADC Chip Input Range (V)
	refInpRange  = fAdcInpRange / fGain;
		
	return status;
}

//***************************************************************************************
int CAdc214x10mSrv::SetCode(CModule* pModule, ULONG type)
{
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::GetCode(CModule* pModule, ULONG& type)
{
	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::SetMaster(CModule* pModule, ULONG mode)
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
		DEVS_CMD_Reg	param;
		ADM2IF_MODE0	*pMode0;

		param.idxMain = m_index;
		param.reg = ADM2IFnr_MODE0;
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0 = (ADM2IF_MODE0*)&param.val;

		pMode0->ByBits.Start = 1;
		pMode0->ByBits.AdmClk = 1;
		pMode0->ByBits.Master = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc214x10mSrv::ExtraInit(CModule* pModule)
{
	DEVS_CMD_Reg param;

	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);

	ADM2IF_MODE0 *pMode0 = (ADM2IF_MODE0*)&param.val;

	pMode0->ByBits.AdmClk = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
S32		CAdc214x10mSrv::IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, U32 *pVal )
{
	DEVS_CMD_Reg	param;

	param.idxMain = m_index;
	param.tetr = tetrNo;
	param.reg = regNo;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	*pVal = param.val;

	return BRDerr_OK;
}

//***************************************************************************************
S32		CAdc214x10mSrv::IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, ULONG *pVal )
{
	return IndRegRead( pModule, tetrNo, regNo, (U32*)pVal );
}

//***************************************************************************************
S32		CAdc214x10mSrv::IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, U32 val )
{
	DEVS_CMD_Reg	param;

	param.idxMain = m_index;
	param.tetr = tetrNo;
	param.reg = regNo;
	param.val = val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
S32		CAdc214x10mSrv::IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, ULONG val )
{
	return IndRegWrite( pModule, tetrNo, regNo, (U32)val );
}

// ***************** End of file Adc214x10mSrv.cpp ***********************
