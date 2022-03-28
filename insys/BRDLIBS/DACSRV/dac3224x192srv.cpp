/*
 ***************** File Dac3224x192Srv.cpp ************
 *
 * BRD Driver for DAС service on ADM3224x192
 *
 * (C) InSys by Dorokhin A. Oct 2005
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "dac3224x192srv.h"

#define	CURRFILE "DAC3224X192SRV"

//***************************************************************************************
CDac3224x192Srv::CDac3224x192Srv(int idx, int srv_num, CModule* pModule, PDAC3224X192SRV_CFG pCfg) :
	CDacSrv(idx, _T("DAC3224X192"), srv_num, pModule, pCfg, sizeof(DAC3224X192SRV_CFG))
{
}

//***************************************************************************************
int CDac3224x192Srv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	CDacSrv::SetChanMask(pModule, 0);
//	return BRDerr_OK;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CDac3224x192Srv::GetDacTetrNum(CModule* pModule)
{
	m_DacTetrNum = GetTetrNum(pModule, m_index, DAC3224X192_TETR_ID);
}
/*
//***************************************************************************************
void CDac3224x192Srv::FreeInfoForDialog(PVOID pInfo)
{
//	PDAC3224X192SRV_INFO pSrvInfo = (PDAC3224X192SRV_INFO)pInfo;
	PDACSRV_INFO pSrvInfo = (PDACSRV_INFO)pInfo;
	CDacSrv::FreeInfoForDialog(pSrvInfo->pDacInfo);
	delete pSrvInfo;
}
*/
//***************************************************************************************
PVOID CDac3224x192Srv::GetInfoForDialog(CModule* pDev)
{
	PDAC3224X192SRV_CFG pSrvCfg = (PDAC3224X192SRV_CFG)m_pConfig;
//	PDACSRV_INFO pSrvInfo = new DACSRV_INFO;
//	pSrvInfo->Size = sizeof(DAC3224X192SRV_INFO);
	UCHAR code = pSrvCfg->DacCfg.Encoding;
	PDACSRV_INFO pSrvInfo = (PDACSRV_INFO)CDacSrv::GetInfoForDialog(pDev);
	pSrvInfo->Encoding = code;
	GetChanMask(pDev, pSrvInfo->ChanMask);
	GetOutRange(pDev, pSrvInfo->Range[0], 0);

	GetClkSource(pDev, pSrvInfo->ClockSrc);
	GetClkValue(pDev, pSrvInfo->ClockSrc, pSrvInfo->ClockValue);
	GetRate(pDev, pSrvInfo->SamplingRate, pSrvInfo->ClockValue);

	return pSrvInfo;
}

//***************************************************************************************
int CDac3224x192Srv::SetPropertyFromDialog(void	*pDev, void	*args)
{
	CModule* pModule = (CModule*)pDev;
	PDACSRV_INFO pInfo = (PDACSRV_INFO)args;
	CDacSrv::SetPropertyFromDialog(pDev, args);
	SetChanMask(pModule, pInfo->ChanMask);
	SetOutRange(pModule, pInfo->Range[0], 0);

	SetClkSource(pModule, pInfo->ClockSrc);
	SetClkValue(pModule, pInfo->ClockSrc, pInfo->ClockValue);
	SetRate(pModule, pInfo->SamplingRate, pInfo->ClockSrc, pInfo->ClockValue);

	return BRDerr_OK;
}

//***************************************************************************************
int CDac3224x192Srv::SetProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	CDacSrv::SetProperties(pDev, iniFilePath, SectionName);

	GetPrivateProfileString(SectionName, "ChannelMask", "1", Buffer, sizeof(Buffer), iniFilePath);
	ULONG chanMask = atoi(Buffer);
	SetChanMask(pDev, chanMask);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
	GetPrivateProfileString(SectionName, "SamplingRate", "200000.0", Buffer, sizeof(Buffer), iniFilePath);
	double rate = atof(Buffer);
	SetRate(pDev, rate, clkSrc, clkValue);

	return BRDerr_OK;
}

//***************************************************************************************
int CDac3224x192Srv::SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName)
{
	TCHAR Buffer[128];
	CDacSrv::SaveProperties(pDev, iniFilePath, SectionName);
	PDAC3224X192SRV_CFG pSrvCfg = (PDAC3224X192SRV_CFG)m_pConfig;
	sprintf(Buffer, "%u", pSrvCfg->DacCfg.Encoding);
	WritePrivateProfileString(SectionName, "CodeType", Buffer, iniFilePath);

	ULONG chanMask;
	GetChanMask(pDev, chanMask);
	sprintf(Buffer, "%u", chanMask);
	WritePrivateProfileString(SectionName, "ChannelMask", Buffer, iniFilePath);

	ULONG clkSrc;
	GetClkSource(pDev, clkSrc);
	double clkValue;
	GetClkValue(pDev, clkSrc, clkValue);
	double rate;
	GetRate(pDev, rate, clkValue);
	sprintf(Buffer, "%.2f", rate);
	WritePrivateProfileString(SectionName, "SamplingRate", Buffer, iniFilePath);

	// the function flushes the cache
	WritePrivateProfileString(NULL, NULL, NULL, iniFilePath);
	return BRDerr_OK;
}

//***************************************************************************************
void CDac3224x192Srv::WriteDacReg(CModule* pModule, U32 chipMask, U32 addr, U32 data)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDAC_MODE2 pMode2 = (PDAC_MODE2)&param.val;
	pMode2->ByBits.ProgMask = chipMask;		// выбираем микросхемы ЦАП для программирования
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);

	param.reg = DAC192nr_CHIPDATA;
	PDAC_CHIPDATA pData = (PDAC_CHIPDATA)&param.val;
	pData->ByBits.Data = data;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);

	param.reg = DAC192nr_CHIPADDRCTRL;
	PDAC_CHIPCTRL pCtrl = (PDAC_CHIPCTRL)&param.val;
	pCtrl->ByBits.Addr = addr;
	pCtrl->ByBits.Ctrl = DAC_MAGIC_CTRL;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	
}

//***************************************************************************************
void CDac3224x192Srv::ChipsInit(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = DACnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDAC_CTRL pCtrl = (PDAC_CTRL)&param.val;
	pCtrl->ByBits.Sync = 1;		// подаем импульс синхронизации
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	pCtrl->ByBits.Sync = 0;		// снимаем импульс синхронизации
	pCtrl->ByBits.ChipsReset = 1; // подаем сброс на микросхемы ЦАП
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	pCtrl->ByBits.ChipsReset = 0; // снимаем сброс с микросхем ЦАП
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);

	WriteDacReg(pModule, 0xf, 0x01, 0x80); // разрешаем управляющие порты всех микросхем ЦАП
}

//***************************************************************************************
int CDac3224x192Srv::SetClkSource(CModule* pModule, ULONG ClkSrc)
{
	CDacSrv::SetClkSource(pModule, ClkSrc);
	Sleep(10);
	ChipsInit(pModule);

	return BRDerr_OK;
}

//***************************************************************************************
int CDac3224x192Srv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
	ULONG DacDivideMode;
	PDAC3224X192SRV_CFG pSrvCfg = (PDAC3224X192SRV_CFG)m_pConfig;
	if(Rate < pSrvCfg->DacCfg.MinRate)
		Rate = pSrvCfg->DacCfg.MinRate;
	if(Rate > pSrvCfg->DacCfg.MaxRate)
		Rate = pSrvCfg->DacCfg.MaxRate;

	if(Rate < MAX_SINGLE_RATE)
		DacDivideMode = 0; // SINGLE
	else
		if(Rate < MAX_DOUBLE_RATE)
			DacDivideMode = 1; // DOUBLE
		else
			DacDivideMode = 2; // QUAD

	ULONG ClkDivider = SINGLE_DIVIDER >> DacDivideMode;

	double ClkVal = ClkValue / ClkDivider;
	ULONG DacRateDivider = ROUND(ClkVal / Rate);
	DacRateDivider = DacRateDivider ? DacRateDivider : 1;
	while(1)
	{
		Rate = ClkVal / DacRateDivider;
		if(Rate < pSrvCfg->DacCfg.MinRate)
			DacRateDivider--;
		else
			if(Rate > pSrvCfg->DacCfg.MaxRate)
				DacRateDivider++;
			else
				break;
	}
	ULONG status = CDacSrv::SetRate(pModule, Rate, ClkSrc, ClkVal);
	if(status == BRDerr_OK || status == BRDerr_NOT_ACTION)
	{
		Sleep(10);

		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_DacTetrNum;
		param.reg = DACnr_CTRL1;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PDAC_CTRL pCtrl = (PDAC_CTRL)&param.val;
		pCtrl->ByBits.ClkDivMode = DacDivideMode;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		ChipsInit(pModule);

//		Rate = ClkValue / DacRateDivider;
		return BRDerr_OK;
	}
	else
		return status;
}

//***************************************************************************************
int CDac3224x192Srv::GetRate(CModule* pModule, double& Rate, double ClkValue)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = DACnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDAC_CTRL pCtrl = (PDAC_CTRL)&param.val;
	ULONG DacDivideMode = pCtrl->ByBits.ClkDivMode;
	ULONG ClkDivider = SINGLE_DIVIDER >> DacDivideMode;
	CDacSrv::GetRate(pModule, Rate, ClkValue / ClkDivider);
	return BRDerr_OK;
}

//***************************************************************************************
int CDac3224x192Srv::SetChanMask(CModule* pModule, ULONG mask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = ADM2IFnr_CHAN1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_CHAN1 pChan1 = (PADM2IF_CHAN1)&param.val;
	pChan1->ByBits.ChanSel = mask;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_CHAN2;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_CHAN2 pChan2 = (PADM2IF_CHAN2)&param.val;
	pChan2->ByBits.ChanSel = mask >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CDac3224x192Srv::GetChanMask(CModule* pModule, ULONG& mask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DacTetrNum;
	param.reg = ADM2IFnr_CHAN1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_CHAN1 pChan1 = (PADM2IF_CHAN1)&param.val;
	mask = pChan1->ByBits.ChanSel;

	param.reg = ADM2IFnr_CHAN2;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_CHAN2 pChan2 = (PADM2IF_CHAN2)&param.val;
	mask = mask | (pChan2->ByBits.ChanSel << 16);

	return BRDerr_OK;
}

//***************************************************************************************
int CDac3224x192Srv::SetCode(CModule* pModule, ULONG type)
{
	return BRDerr_OK;
}

//***************************************************************************************
int CDac3224x192Srv::GetCode(CModule* pModule, ULONG& type)
{
	type = BRDcode_TWOSCOMPLEMENT;
	return BRDerr_OK;
}

//***************************************************************************************
int CDac3224x192Srv::SetSpecific(CModule* pModule, PBRD_DacSpec pSpec)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	if(pSpec->command != DAC3224X192cmd_WRITE)
		return Status;
	PBRD_DacRegWrite pRegWrite = (PBRD_DacRegWrite)pSpec->arg;
	WriteDacReg(pModule, pRegWrite->progMask, pRegWrite->addr, pRegWrite->data);
	return BRDerr_OK;
}

// ***************** End of file Dac3224x192Srv.cpp ***********************
