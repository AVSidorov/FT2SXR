/*
 ***************** File DacTstSrv.cpp ************
 *
 * BRD Driver for DAС service
 *
 * (C) InSys by Dorokhin A. Oct 2009-2016
 *
 ******************************************************
*/

#include "module.h"
#include "dactstsrv.h"

#define	CURRFILE "DACTSTSRV"

#define DBG_printf	if(g_isPrintf)BRDC_printf

int						g_isPrintf = 0;		// Разрешает работу printf'ов
											
//***************************************************************************************
CDacTstSrv::CDacTstSrv(int idx, int srv_num, CModule* pModule, PDACSRV_CFG pCfg) :
	CDacSrv(idx, _BRDC("DACTST"), srv_num, pModule, pCfg, sizeof(DACSRV_CFG))
{
}

//***************************************************************************************
int CDacTstSrv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	CDacSrv::SetChanMask(pModule, 0);  // ?
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CDacTstSrv::GetDacTetrNum(CModule* pModule)
{
	m_DacTetrNum = GetTetrNum(pModule, m_index, DAC214X125M_TETR_ID);
	if(m_DacTetrNum != -1)
		return;
	m_DacTetrNum = GetTetrNum(pModule, m_index, DAC3224X192_TETR_ID);
	if(m_DacTetrNum != -1)
		return;
	m_DacTetrNum = GetTetrNum(pModule, m_index, DAC818X800_TETR_ID);
	if(m_DacTetrNum != -1)
		return;
	m_DacTetrNum = GetTetrNum(pModule, m_index, DAC1624X192_TETR_ID);
	if(m_DacTetrNum != -1)
		return;
	m_DacTetrNum = GetTetrNum(pModule, m_index, DAC216X400M_TETR_ID);
	if(m_DacTetrNum != -1)
		return;
	m_DacTetrNum = GetTetrNum(pModule, m_index, VK3DAC_TETR_ID);
	if(m_DacTetrNum != -1)
		return;
	m_DacTetrNum = GetTetrNum(pModule, m_index, ADMDAC216x400MT_TETR_ID);
	if(m_DacTetrNum != -1)
		return;
	m_DacTetrNum = GetTetrNum(pModule, m_index, DACFM416x1G5D_TETR_ID);
	if(m_DacTetrNum != -1)
		return;
	m_DacTetrNum = GetTetrNum(pModule, m_index, DACFM214x2G5D_TETR_ID);

}

//***************************************************************************************
int CDacTstSrv::SetProperties(CModule* pModule, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	DACTSTSRV_CFG		*pSrvCfg = (DACTSTSRV_CFG*)m_pConfig;
	BRDCHAR			Buffer[128];
	BRDCHAR			*endptr;

	//
	// Извлечение параметров работы субмодуля из INI-файла 
	// и инициализация субмодуля в соответствии с этими параметрами
	//
	GetPrivateProfileString(SectionName, _BRDC("IsPrintf"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
	g_isPrintf = BRDC_strtol(Buffer, &endptr, 0);


	// Извлечение стандартных параметров
	CDacSrv::SetProperties(pModule, iniFilePath, SectionName);

	// Извлечение источника тактовой частоты
	U32			clkSrc;

	GetPrivateProfileString(SectionName, _BRDC("ClockSource"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
	clkSrc = BRDC_strtol(Buffer, &endptr, 0);
	SetClkSource(pModule, clkSrc);

	// Извлечение частоты внешнего генератора

	double				clkValue;

	GetPrivateProfileString(SectionName, _BRDC("ExternalClockValue"), _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
	if (Buffer[0])
	{
		clkValue = BRDC_atof(Buffer);
		SetClkValue(pModule, BRDclks_DAC_EXTCLK, clkValue);
	}

	GetPrivateProfileString(SectionName, _BRDC("BaseClockValue"), _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
	if (Buffer[0])
	{
		clkValue = BRDC_atof(Buffer);
		SetClkValue(pModule, BRDclks_DAC_BASEGEN, clkValue);
	}

	GetPrivateProfileString(SectionName, _BRDC("SubClockValue"), _BRDC(""), Buffer, sizeof(Buffer), iniFilePath);
	if (Buffer[0])
	{
		clkValue = BRDC_atof(Buffer);
		SetClkValue(pModule, BRDclks_DAC_SUBGEN, clkValue);
	}

	//
	// Извлечение требуемой частоты дискретизации
	//
	double			rate;
	S32				kint;

	GetPrivateProfileString(SectionName, _BRDC("KINT"), _BRDC("1"), Buffer, sizeof(Buffer), iniFilePath);
	kint = BRDC_atoi(Buffer);
	SetInterpFactor(pModule, kint);

	GetPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("100000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	rate = BRDC_atof(Buffer);
	SetRate(pModule, rate, 0, 0.0);

	return BRDerr_OK;
}

//***************************************************************************************
int CDacTstSrv::SetClkSource(CModule* pModule, ULONG nClkSrc)
{
	DACTSTSRV_CFG *pSrvCfg = (DACTSTSRV_CFG*)m_pConfig;
	pSrvCfg->ClkSrc = nClkSrc;
	return BRDerr_OK;
}

//***************************************************************************************
int CDacTstSrv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
{
	DACTSTSRV_CFG *pSrvCfg = (DACTSTSRV_CFG*)m_pConfig;
	ClkSrc = pSrvCfg->ClkSrc;
	return BRDerr_OK;
}

//***************************************************************************************
int CDacTstSrv::SetClkValue(CModule* pModule, ULONG clkSrc, double& refClkValue)
{
	DACTSTSRV_CFG *pSrvCfg = (DACTSTSRV_CFG*)m_pConfig;
	pSrvCfg->dClkValue = refClkValue;
	return BRDerr_OK;
}
//***************************************************************************************
int CDacTstSrv::GetClkValue(CModule* pModule, ULONG clkSrc, double& refClkValue)
{
	DACTSTSRV_CFG *pSrvCfg = (DACTSTSRV_CFG*)m_pConfig;
	refClkValue = pSrvCfg->dClkValue;
	return BRDerr_OK;
}

//***************************************************************************************
int CDacTstSrv::SetRate(CModule* pModule, double& refRate, ULONG clkSrc, double clkValue)
{
	DACTSTSRV_CFG *pSrvCfg = (DACTSTSRV_CFG*)m_pConfig;
	pSrvCfg->dSamplingRate = refRate;
	return BRDerr_OK;
}

//***************************************************************************************
int CDacTstSrv::GetRate(CModule* pModule, double& refRate, double ClkValue)
{
	DACTSTSRV_CFG *pSrvCfg = (DACTSTSRV_CFG*)m_pConfig;
	refRate = pSrvCfg->dSamplingRate;
	refRate /= (double)pSrvCfg->nDacInterpFactor;
	return BRDerr_OK;
}

//***************************************************************************************
int CDacTstSrv::SetInterpFactor(CModule *pModule, ULONG nInterpFactor)
{
	DACTSTSRV_CFG *pSrvCfg = (DACTSTSRV_CFG*)m_pConfig;

	if (nInterpFactor != 2 && nInterpFactor != 4 &&
		nInterpFactor != 8 && nInterpFactor != 16)
		nInterpFactor = 1;
	pSrvCfg->nDacInterpFactor = nInterpFactor;

	return BRDerr_OK;
}

//***************************************************************************************
int CDacTstSrv::GetInterpFactor(CModule *pModule, ULONG &refInterpFactor)
{
	DACTSTSRV_CFG *pSrvCfg = (DACTSTSRV_CFG*)m_pConfig;

	refInterpFactor = pSrvCfg->nDacInterpFactor;

	return BRDerr_OK;
}

// ***************** End of file DacTstSrv.cpp ***********************
