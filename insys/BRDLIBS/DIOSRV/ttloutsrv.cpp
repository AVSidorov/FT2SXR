/*
 ***************** File TtlOutSrv.cpp ***********************
 *
 * CTRL-command for BRD Driver for TTLOUT service on DIO2 submodule
 *
 * (C) InSys by Dorokhin A. Nov 2007
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "ttloutsrv.h"

#define	CURRFILE "TTLOUTSRV"

static CMD_Info SETRATE_CMD			= { BRDctrl_TTLOUT_SETRATE,	    0, 0, 0, NULL};
static CMD_Info GETRATE_CMD			= { BRDctrl_TTLOUT_GETRATE,	    1, 0, 0, NULL};
static CMD_Info SETCLKMODE_CMD		= { BRDctrl_TTLOUT_SETCLKMODE,	0, 0, 0, NULL};
static CMD_Info GETCLKMODE_CMD		= { BRDctrl_TTLOUT_GETCLKMODE,	1, 0, 0, NULL};
static CMD_Info SETSYNCMODE_CMD		= { BRDctrl_TTLOUT_SETSYNCMODE,	0, 0, 0, NULL};
static CMD_Info GETSYNCMODE_CMD		= { BRDctrl_TTLOUT_GETSYNCMODE,	1, 0, 0, NULL};
static CMD_Info SETMASTER_CMD		= { BRDctrl_TTLOUT_SETMASTER,	0, 0, 0, NULL};
static CMD_Info GETMASTER_CMD		= { BRDctrl_TTLOUT_GETMASTER,	1, 0, 0, NULL};
static CMD_Info SETSTARTMODE_CMD	= { BRDctrl_TTLOUT_SETSTARTMODE, 0, 0, 0, NULL};
static CMD_Info GETSTARTMODE_CMD	= { BRDctrl_TTLOUT_GETSTARTMODE, 1, 0, 0, NULL};

static CMD_Info FIFORESET_CMD		= { BRDctrl_TTLOUT_FIFORESET,	0, 0, 0, NULL};
static CMD_Info ENABLE_CMD			= { BRDctrl_TTLOUT_ENABLE,		0, 0, 0, NULL};
static CMD_Info FIFOSTATUS_CMD		= { BRDctrl_TTLOUT_FIFOSTATUS,	1, 0, 0, NULL};
static CMD_Info PUTDATA_CMD			= { BRDctrl_TTLOUT_PUTDATA,		0, 0, 0, NULL};
static CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_TTLOUT_GETSRCSTREAM, 1, 0, 0, NULL};

//***************************************************************************************
CTtlOutSrv::CTtlOutSrv(int idx, int srv_num, CModule* pModule, PTTLOUTSRV_CFG pCfg) :
	CService(idx, _T("TTLOUT"), srv_num, pModule, pCfg, sizeof(TTLOUTSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_OUT |
			BRDserv_ATTR_STREAMABLE_OUT |
			BRDserv_ATTR_EXCLUSIVE_ONLY;

	Init(&SETRATE_CMD, (CmdEntry)&CTtlOutSrv::CtrlRate);
	Init(&GETRATE_CMD, (CmdEntry)&CTtlOutSrv::CtrlRate);
	Init(&SETCLKMODE_CMD, (CmdEntry)&CTtlOutSrv::CtrlClkMode);
	Init(&GETCLKMODE_CMD, (CmdEntry)&CTtlOutSrv::CtrlClkMode);
	Init(&SETSYNCMODE_CMD, (CmdEntry)&CTtlOutSrv::CtrlSyncMode);
	Init(&GETSYNCMODE_CMD, (CmdEntry)&CTtlOutSrv::CtrlSyncMode);
	Init(&SETMASTER_CMD, (CmdEntry)&CTtlOutSrv::CtrlMaster);
	Init(&GETMASTER_CMD, (CmdEntry)&CTtlOutSrv::CtrlMaster);
	Init(&SETSTARTMODE_CMD, (CmdEntry)&CTtlOutSrv::CtrlStartMode);
	Init(&GETSTARTMODE_CMD, (CmdEntry)&CTtlOutSrv::CtrlStartMode);

	Init(&FIFORESET_CMD, (CmdEntry)&CTtlOutSrv::CtrlFifoReset);
	Init(&ENABLE_CMD, (CmdEntry)&CTtlOutSrv::CtrlEnable);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&CTtlOutSrv::CtrlFifoStatus);
	Init(&PUTDATA_CMD, (CmdEntry)&CTtlOutSrv::CtrlGetData);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&CTtlOutSrv::CtrlGetAddrData);
}

//***************************************************************************************
int CTtlOutSrv::CtrlIsAvailable(
								void		*pDev,		// InfoSM or InfoBM
								void		*pServData,	// Specific Service Data
								ULONG		cmd,		// Command Code (from BRD_ctrl())
								void		*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	PSERV_CMD_IsAvailable pServAvailable = (PSERV_CMD_IsAvailable)args;
	pServAvailable->attr = m_attribute;
	m_MainTetrNum = GetTetrNum(pModule, m_index, MAIN_TETR_ID);
	m_TtlOutTetrNum = GetTetrNum(pModule, m_index, TTLOUT_TETR_ID);
	if(m_MainTetrNum != -1 && m_TtlOutTetrNum != -1)
	{
		m_isAvailable = 1;
		PTTLOUTSRV_CFG pSrvCfg = (PTTLOUTSRV_CFG)m_pConfig;
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;
			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_TtlOutTetrNum;
			if(!pSrvCfg->FifoSize)
			{
				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				pSrvCfg->FifoSize = RegParam.val * widthFifo;
			}
			RegParam.reg = ADM2IFnr_MODE0;
			RegParam.val = 0;
			PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&RegParam.val;
			pMode0->ByBits.Reset = 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			for(int i = 1; i < 32; i++)
			{
				RegParam.reg = i;
				RegParam.val = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			}
			RegParam.reg = ADM2IFnr_MODE0;
			pMode0->ByBits.Reset = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
		}
	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
int CTtlOutSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	ULONG AdmNum;
	TCHAR buf[SERVNAME_SIZE];
	_tcscpy_s(buf, m_name);
	PTCHAR pBuf = buf + (strlen(buf) - 2);
	if(_tcschr(pBuf, '1'))
		AdmNum = 1;
	else
		AdmNum = 0;
	*(ULONG*)args = (AdmNum << 16) | m_TtlOutTetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int CTtlOutSrv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
{
	ULONG source;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TtlOutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_FMODE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		source = param.val;
	}
	else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.reg = ADM2IFnr_FMODE;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
			source = param.val;
		}
		else
		{ // Slave
			source = BRDclks_EXTSYNX;
		}
	}
	ClkSrc = source;
	return BRDerr_OK;
}

//***************************************************************************************
int CTtlOutSrv::GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PTTLOUTSRV_CFG pSrvCfg = (PTTLOUTSRV_CFG)m_pConfig;
	double Clk;
	switch(ClkSrc)
	{
	case BRDclks_SYSGEN:		// System Generator
		Clk = pSrvCfg->SysRefGen;
		break;
	case BRDclks_BASEGEN0:		// Generator 0
		Clk = pSrvCfg->BaseRefGen[0];
		break;
	case BRDclks_BASEGEN1:		// Generator 1
		Clk = pSrvCfg->BaseRefGen[1];
		break;
	case BRDclks_EXTSDX:		// External clock
		Clk = pSrvCfg->BaseExtClk;
		break;
	case BRDclks_EXTSYNX:	// External clock
		Clk = pSrvCfg->BaseExtClk;
		break;
	case BRDclks_BASEDDS:
		Clk = pSrvCfg->BaseExtClk;
		break;
	//case BRDclks_SMCLK:		// 
	//case BRDclks_INFREQ:		// 
	//case BRDclks_SMOUTFREQ:	//
	//	Clk = pSrvCfg->BaseExtClk;
	//	break;
	default:
		Clk = 0.0;
		break;
	}
	ClkValue = Clk;
	return BRDerr_OK;
}
	
//***************************************************************************************
int CTtlOutSrv::SetClkSource(CModule* pModule, ULONG ClkSrc)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TtlOutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_FMODE;
		param.val = ClkSrc;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.reg = ADM2IFnr_FMODE;
			param.val = ClkSrc;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		else
			return BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CTtlOutSrv::SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PTTLOUTSRV_CFG pSrvCfg = (PTTLOUTSRV_CFG)m_pConfig;
	switch(ClkSrc)
	{
	case BRDclks_SYSGEN:		// System Generator
		ClkValue = pSrvCfg->SysRefGen;
		break;
	case BRDclks_BASEGEN0:		// Generator 0
		ClkValue = pSrvCfg->BaseRefGen[0];
		break;
	case BRDclks_BASEGEN1:		// Generator 1
		ClkValue = pSrvCfg->BaseRefGen[1];
		break;
	case BRDclks_EXTSDX:	// External clock from SDX
		pSrvCfg->BaseExtClk = ROUND(ClkValue);
		break;
	case BRDclks_EXTSYNX:	// External clock from SYNX
		pSrvCfg->BaseExtClk = ROUND(ClkValue);
		break;
	case BRDclks_BASEDDS: // External for tetrad, but internal for board
		pSrvCfg->BaseExtClk = ROUND(ClkValue);
		break;
//	case BRDclks_SMCLK:		// 
//	case BRDclks_INFREQ:	// Sinchro with input
//	case BRDclks_SMOUTFREQ:	// Sinchro with output
//		pSrvCfg->BaseExtClk = ROUND(ClkValue);
//		break;
	default:
//		ClkValue = 0.0;
		break;
	}
	return BRDerr_OK;
}
	
//***************************************************************************************
int CTtlOutSrv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
	ULONG RateDivider = ROUND(ClkValue / Rate);
	RateDivider = RateDivider ? RateDivider : 1;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TtlOutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_FDIV;
		param.val = RateDivider;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.reg = ADM2IFnr_FDIV;
			param.val = RateDivider;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		else
			return BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
	}
	Rate = ClkValue / RateDivider;
	return BRDerr_OK;
}

//***************************************************************************************
int CTtlOutSrv::GetRate(CModule* pModule, double& Rate, double ClkValue)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TtlOutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_FDIV;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	}
	else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.reg = ADM2IFnr_FDIV;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		}
		else
		{ // Slave
			param.val = 1;
		}
	}
	Rate = ClkValue / param.val;
	return BRDerr_OK;
}

//***************************************************************************************
int CTtlOutSrv::CtrlRate(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	ULONG ClkSrc;
	Status = GetClkSource(pModule, ClkSrc);
	double ClkValue;
	Status = GetClkValue(pModule, ClkSrc, ClkValue);
	double Rate = *(double*)args;
	if(BRDctrl_TTLOUT_SETRATE == cmd)
	{
		Status = SetRate(pModule, Rate, ClkSrc, ClkValue);
		*(double*)args = Rate;
	}
	else
	{
		Status = GetRate(pModule, Rate, ClkValue);
		*(double*)args = Rate;
	}
	return Status;
}

//***************************************************************************************
int CTtlOutSrv::CtrlClkMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_ClkMode pClk = (PBRD_ClkMode)args;
	if(BRDctrl_TTLOUT_SETCLKMODE == cmd)
	{
		Status = SetClkValue(pModule, pClk->src, pClk->value);
		Status = SetClkSource(pModule, pClk->src);
	}
	else
	{
		ULONG src = pClk->src;
		Status = GetClkSource(pModule, src);
		pClk->src = src;
		Status = GetClkValue(pModule, pClk->src, pClk->value);
	}
	return Status;
}

//***************************************************************************************
int CTtlOutSrv::CtrlSyncMode(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
					   )
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_SyncMode pSyncMode = (PBRD_SyncMode)args;
	if(BRDctrl_TTLOUT_SETSYNCMODE == cmd)
	{
		Status = SetClkValue(pModule, pSyncMode->clkSrc, pSyncMode->clkValue);
		Status = SetClkSource(pModule, pSyncMode->clkSrc);
		if(BRDerr_OK == Status)
			Status = SetRate(pModule, pSyncMode->rate, pSyncMode->clkSrc, pSyncMode->clkValue);
	}
	else
	{
		ULONG src = pSyncMode->clkSrc;
		Status = GetClkSource(pModule, src);
		pSyncMode->clkSrc = src;
		Status = GetClkValue(pModule, pSyncMode->clkSrc, pSyncMode->clkValue);
		Status = GetRate(pModule, pSyncMode->rate, pSyncMode->clkValue);
	}
	return Status;
}

//***************************************************************************************
int CTtlOutSrv::CtrlMaster(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
					   )
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	ULONG* pMasterMode = (ULONG*)args;
	DEVS_CMD_Reg param;
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	param.idxMain = m_index;
	param.reg = ADM2IFnr_MODE0;
	if(BRDctrl_TTLOUT_SETMASTER == cmd)
	{
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0->ByBits.Master = *pMasterMode >> 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.tetr = m_TtlOutTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0->ByBits.Master = *pMasterMode & 0x1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		param.tetr = m_TtlOutTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		*pMasterMode = pMode0->ByBits.Master;
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		*pMasterMode |= (pMode0->ByBits.Master << 1);
	}
	return Status;
}

//***************************************************************************************
int CTtlOutSrv::SetStartMode(CModule* pModule, PVOID pStMode)
{
	PBRD_StartMode pStartMode = (PBRD_StartMode)pStMode;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TtlOutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_STMODE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADM2IF_STMODE pStMode = (PADM2IF_STMODE)&param.val;
		pStMode->ByBits.SrcStart  = pStartMode->startSrc;
		pStMode->ByBits.SrcStop   = pStartMode->trigStopSrc;
		pStMode->ByBits.InvStart  = pStartMode->startInv;
		pStMode->ByBits.InvStop   = pStartMode->stopInv;
		pStMode->ByBits.TrigStart = pStartMode->trigOn;
		pStMode->ByBits.Restart   = pStartMode->reStartMode;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		ULONG master = pMode0->ByBits.Master;
		param.reg = ADM2IFnr_STMODE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADM2IF_STMODE pStMode = (PADM2IF_STMODE)&param.val;
		pStMode->ByBits.SrcStart = pStartMode->startSrc;
		if(!master)
			pStMode->ByBits.SrcStart = BRDsts_EXTSYNX; // SLAVE
		pStMode->ByBits.SrcStop   = pStartMode->trigStopSrc;
		pStMode->ByBits.InvStart  = pStartMode->startInv;
		pStMode->ByBits.InvStop   = pStartMode->stopInv;
		pStMode->ByBits.TrigStart = pStartMode->trigOn;
		pStMode->ByBits.Restart   = pStartMode->reStartMode;
		if(master && pStMode->ByBits.Restart == 1)
			pStMode->ByBits.SrcStop   = BRDsts_TRDADC;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.tetr = m_TtlOutTetrNum;
		pStMode->ByBits.SrcStart  = BRDsts_EXTSYNX; // SLAVE
		pStMode->ByBits.SrcStop   = BRDsts_PRG;
		pStMode->ByBits.InvStart  = 0;
		pStMode->ByBits.InvStop   = 0;
		pStMode->ByBits.TrigStart = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CTtlOutSrv::GetStartMode(CModule* pModule, PVOID pStMode)
{
	PBRD_StartMode pStartMode = (PBRD_StartMode)pStMode;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TtlOutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_STMODE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADM2IF_STMODE pStMode = (PADM2IF_STMODE)&param.val;
		pStartMode->startSrc	= pStMode->ByBits.SrcStart;
		pStartMode->trigStopSrc	= pStMode->ByBits.SrcStop;
		pStartMode->startInv	= pStMode->ByBits.InvStart;
		pStartMode->stopInv		= pStMode->ByBits.InvStop;
		pStartMode->trigOn		= pStMode->ByBits.TrigStart;
		pStartMode->reStartMode	= pStMode->ByBits.Restart;

		//param.reg = ADM2IFnr_PRTMODE;
		//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		//PADM2IF_PRTMODE pPreMode = (PADM2IF_PRTMODE)&param.val;
		//pStartMode->pretrig	 = pPreMode->ByBits.Enable;

	}
	else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;//ADM2IF_ntMAIN;
//		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//		ULONG master = pMode0->ByBits.Master;
		param.reg = ADM2IFnr_STMODE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADM2IF_STMODE pStMode = (PADM2IF_STMODE)&param.val;
		pStartMode->startSrc	= pStMode->ByBits.SrcStart;
//		if(!master)
//			pStartMode->startSrc = BRDsts_EXTSYNX;
		pStartMode->trigStopSrc	= pStMode->ByBits.SrcStop;
		pStartMode->startInv	= pStMode->ByBits.InvStart;
		pStartMode->stopInv		= pStMode->ByBits.InvStop;
		pStartMode->trigOn		= pStMode->ByBits.TrigStart;
		pStartMode->reStartMode	= pStMode->ByBits.Restart;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CTtlOutSrv::CtrlStartMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	if(BRDctrl_TTLOUT_SETSTARTMODE == cmd)
		Status = SetStartMode(pModule, args);
	else
        Status = GetStartMode(pModule, args);
	return Status;
}

//***************************************************************************************
int CTtlOutSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TtlOutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pMode0->ByBits.FifoRes = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTtlOutSrv::CtrlEnable (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TtlOutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Start = *(PULONG)args;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTtlOutSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TtlOutTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CTtlOutSrv::CtrlGetData(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TtlOutTetrNum;
/*	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pMode0->ByBits.FifoRes = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
*/
	//PADM2IF_STATUS pStatus;
	//do
	//{
	//	param.reg = ADM2IFnr_STATUS;
	//	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//	pStatus = (PADM2IF_STATUS)&param.val;
	//} while(pStatus->ByBits.HalfFull);

	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);

	//PULONG buf = (PULONG)pBuf->pData;
	//for(ULONG i = 0; i < pBuf->size / sizeof(ULONG); i++)
	//{
	//	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//	buf[i] = param.val;
	//}
	
	//param.reg = ADM2IFnr_STATUS;
	//pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//pStatus = (PADM2IF_STATUS)&param.val;

	return BRDerr_OK;
}

// ***************** End of file TtlOutSrv.cpp ***********************
