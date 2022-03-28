/*
 ***************** File Adc1612x1mSrv.cpp ************
 *
 * BRD Driver for ADС service on ADM1612x1M(WB) and ADM212x500M(WB)
 *
 * (C) InSys by Ekkore Aug 2008
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "Adc1612x1mSrv.h"

#define	CURRFILE "ADC1612x1MSRV"

const double BRD_ADC_MAXGAINTUN = 1.0; // not used


//***************************************************************************************
CAdc1612x1mSrv::CAdc1612x1mSrv(int idx, int srv_num, CModule* pModule, PADC1612x1MSRV_Cfg pCfg) :
	CAdcSrv(idx, _BRDC("ADC1612X1M"), srv_num, pModule, pCfg, sizeof(ADC1612x1MSRV_Cfg))
{
}

//***************************************************************************************
int CAdc1612x1mSrv::CtrlRelease( void *pDev, void *pServData, ULONG cmd, void *args )
{
	CModule* pModule = (CModule*)pDev;

	CAdcSrv::SetChanMask(pModule, 0);
	CAdcSrv::CtrlRelease(pDev, pServData, cmd, args);
//	return BRDerr_OK;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CAdc1612x1mSrv::GetAdcTetrNum(CModule* pModule)
{
	//
	// Эта функция вызывается только при обработке команды ..._ISAVALIABLE
	//

	//
	// Поиск номера терады. Если тетрада не найдена, то -1.
	//
	m_AdcTetrNum = GetTetrNum(pModule, m_index, ADC1612x1M_TETR_ID);
}

//***************************************************************************************
void CAdc1612x1mSrv::FreeInfoForDialog(PVOID pInfo)
{
	ADC1612x1MSRV_Info *pSrvInfo = (ADC1612x1MSRV_Info*)pInfo;
	CAdcSrv::FreeInfoForDialog(pSrvInfo->pAdcInfo);
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CAdc1612x1mSrv::GetInfoForDialog(CModule* pDev)
{
	ADC1612x1MSRV_Cfg  *pSrvCfg  = (ADC1612x1MSRV_Cfg*)m_pConfig;
	ADC1612x1MSRV_Info *pSrvInfo = new ADC1612x1MSRV_Info;

	pSrvInfo->Size = sizeof(ADC1612x1MSRV_Info);

	UCHAR code = pSrvCfg->AdcCfg.Encoding;
	
	pSrvInfo->pAdcInfo = (ADCSRV_INFO*)CAdcSrv::GetInfoForDialog(pDev);
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

	return pSrvInfo;
}

//***************************************************************************************
int CAdc1612x1mSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{

	CModule* pModule = (CModule*)pDev;
	PADC1612x1MSRV_Info pInfo = (PADC1612x1MSRV_Info)args;

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

	ADC1612x1MSRV_Cfg *pAdcSrvCfg = (ADC1612x1MSRV_Cfg*)m_pConfig;

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

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1612x1mSrv::SetProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	TCHAR ParamName[128];

	//
	// Извлечение параметров работы субмодуля из INI-файла 
	// и инициализация субмодуля в соответствии с этими параметрами
	//


	//
	// Извлечение стандартных параметров
	//
	CAdcSrv::SetProperties(pDev, iniFilePath, SectionName);

	//
	// Извлечение для каждого канала требуемой шкалы преобразования
	//
	double			inpRange;

	for(int ii = 0; ii < 16; ii+=2 )
	{
		sprintf_s( ParamName, "InputRange%d_%d", ii, ii+1 );
		GetPrivateProfileString( SectionName, ParamName, "2.5", Buffer, sizeof(Buffer), iniFilePath );
		inpRange = atof(Buffer);
		SetInpRange( pDev, inpRange, ii );
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1612x1mSrv::SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	//
	// Эта функция вызывается для формирования ini-файла, содержащего параметры
	// работы субмодуля. Обычно ей пользуется только пример adc_ctrl.exe
	//
	TCHAR ParamName[128];
	ADC1612x1MSRV_Cfg *pSrvCfg = (ADC1612x1MSRV_Cfg*)m_pConfig;

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
	double	inpRange;
	int		ii;

	for( ii = 0; ii < 16; ii+=2 )
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
int CAdc1612x1mSrv::SetGain( CModule *pModule, double &refGain, ULONG Chan )
{
	int		status = BRDerr_OK;
	ULONG	mask;
	int		ii;

	//
	// Определить код, загружаемый в регистр и 
	// соответствующий заданному коэффициенту передачи
	//
	if( refGain >= BRD_Gain[BRD_GAINCNT-1] )
	{
		if( refGain > BRD_Gain[BRD_GAINCNT-1] )
			status = BRDerr_WARN;
		mask = BRD_GAINCNT - 1;
	}
	else if( refGain <= BRD_Gain[0] )
	{
		if( refGain < BRD_Gain[0] )
			status = BRDerr_WARN;
		mask = 0;
	}
	else 
	{
		for( ii = 0; ii < BRD_GAINCNT-1; ii++ )
		{
			if( (refGain >= BRD_Gain[ii]) && 
				(refGain <= BRD_Gain[ii+1]) )
			{
				if( refGain <= (BRD_Gain[ii] + BRD_Gain[ii+1]) / 2 )
					mask = ii;
				else
					mask = ii+1;
				break;
			}
		}
	}
	
	//
	// Скорректировать коэффициент передачи
	//
	refGain = BRD_Gain[mask];

	//
	// Загрузить код в регистр
	//
	DEVS_CMD_Reg	param;
	ADC_GAIN		*pGain = (ADC_GAIN*)&param.val;

	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);

	switch(Chan)
	{
		case 0: 
		case 1:
			pGain->ByBits.Chan0_1 = mask;
			break;
		case 2: 
		case 3:
			pGain->ByBits.Chan2_3 = mask;
			break;
		case 4: 
		case 5:
			pGain->ByBits.Chan4_5 = mask;
			break;
		case 6: 
		case 7:
			pGain->ByBits.Chan6_7 = mask;
			break;
		case 8: 
		case 9:
			pGain->ByBits.Chan8_9 = mask;
			break;
		case 10: 
		case 11:
			pGain->ByBits.Chan10_11 = mask;
			break;
		case 12: 
		case 13:
			pGain->ByBits.Chan12_13 = mask;
			break;
		case 14: 
		case 15:
			pGain->ByBits.Chan14_15 = mask;
			break;
		default:
			return BRDerr_BAD_PARAMETER;
	}
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return status;
}

//***************************************************************************************
int CAdc1612x1mSrv::GetGain(CModule* pModule, double& refGain, ULONG Chan)
{
	DEVS_CMD_Reg	param;
	ADC_GAIN		*pGain = (ADC_GAIN*)&param.val;
	double			value;

	param.idxMain = m_index;
	param.tetr = m_AdcTetrNum;
	param.reg = ADM2IFnr_GAIN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);

	switch(Chan)
	{
		case 0:
		case 1:
			value = BRD_Gain[pGain->ByBits.Chan0_1];
			break;
		case 2:
		case 3:
			value = BRD_Gain[pGain->ByBits.Chan2_3];
			break;
		case 4:
		case 5:
			value = BRD_Gain[pGain->ByBits.Chan4_5];
			break;
		case 6:
		case 7:
			value = BRD_Gain[pGain->ByBits.Chan6_7];
			break;
		case 8:
		case 9:
			value = BRD_Gain[pGain->ByBits.Chan8_9];
			break;
		case 10:
		case 11:
			value = BRD_Gain[pGain->ByBits.Chan10_11];
			break;
		case 12:
		case 13:
			value = BRD_Gain[pGain->ByBits.Chan12_13];
			break;
		case 14:
		case 15:
			value = BRD_Gain[pGain->ByBits.Chan14_15];
			break;
		default:
			return BRDerr_BAD_PARAMETER;
	}
	refGain = value;

	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1612x1mSrv::SetInpRange(CModule* pModule, double& refInpRange, ULONG Chan)
{
	ADC1612x1MSRV_Cfg	*pSrvCfg = (ADC1612x1MSRV_Cfg*)m_pConfig;
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
int CAdc1612x1mSrv::GetInpRange(CModule* pModule, double& refInpRange, ULONG Chan)
{
	ADC1612x1MSRV_Cfg	*pSrvCfg = (ADC1612x1MSRV_Cfg*)m_pConfig;
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
int CAdc1612x1mSrv::SetCode(CModule* pModule, ULONG type)
{
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1612x1mSrv::GetCode(CModule* pModule, ULONG& type)
{
	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CAdc1612x1mSrv::SetMaster(CModule* pModule, ULONG mode)
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
int CAdc1612x1mSrv::ExtraInit(CModule* pModule)
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



// ***************** End of file Adc1612x1mSrv.cpp ***********************
