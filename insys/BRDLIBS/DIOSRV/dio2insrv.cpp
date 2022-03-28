/*
 ***************** File Dio2InSrv.cpp ***********************
 *
 * CTRL-command for BRD Driver for DIO2IN service on DIO2 submodule
 *
 * (C) InSys by Dorokhin A. Nov 2007
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "dio2insrv.h"

#define	CURRFILE "DIO2INSRV"

static CMD_Info SETRATE_CMD			= { BRDctrl_DIO2IN_SETRATE,	    0, 0, 0, NULL};
static CMD_Info GETRATE_CMD			= { BRDctrl_DIO2IN_GETRATE,	    1, 0, 0, NULL};
static CMD_Info SETCLKMODE_CMD		= { BRDctrl_DIO2IN_SETCLKMODE,	0, 0, 0, NULL};
static CMD_Info GETCLKMODE_CMD		= { BRDctrl_DIO2IN_GETCLKMODE,	1, 0, 0, NULL};
static CMD_Info SETSYNCMODE_CMD		= { BRDctrl_DIO2IN_SETSYNCMODE,	0, 0, 0, NULL};
static CMD_Info GETSYNCMODE_CMD		= { BRDctrl_DIO2IN_GETSYNCMODE,	1, 0, 0, NULL};
static CMD_Info SETMASTER_CMD		= { BRDctrl_DIO2IN_SETMASTER,	0, 0, 0, NULL};
static CMD_Info GETMASTER_CMD		= { BRDctrl_DIO2IN_GETMASTER,	1, 0, 0, NULL};
static CMD_Info SETSTARTMODE_CMD	= { BRDctrl_DIO2IN_SETSTARTMODE, 0, 0, 0, NULL};
static CMD_Info GETSTARTMODE_CMD	= { BRDctrl_DIO2IN_GETSTARTMODE, 1, 0, 0, NULL};
static CMD_Info SETCMPLEVEL_CMD		= { BRDctrl_DIO2IN_SETCMPLEVEL, 0, 0, 0, NULL};
static CMD_Info GETCMPLEVEL_CMD		= { BRDctrl_DIO2IN_GETCMPLEVEL, 1, 0, 0, NULL};

static CMD_Info SETSYMBSYNC_CMD	= { BRDctrl_DIO2IN_SETSYMBSYNC,	0, 0, 0, NULL};

static CMD_Info SETCNT_CMD		= { BRDctrl_DIO2IN_SETCNT, 0, 0, 0, NULL};
static CMD_Info GETCNT_CMD		= { BRDctrl_DIO2IN_GETCNT, 1, 0, 0, NULL};

static CMD_Info FIFORESET_CMD		= { BRDctrl_DIO2IN_FIFORESET,	0, 0, 0, NULL};
static CMD_Info ENABLE_CMD			= { BRDctrl_DIO2IN_ENABLE,		0, 0, 0, NULL};
static CMD_Info FIFOSTATUS_CMD		= { BRDctrl_DIO2IN_FIFOSTATUS,	1, 0, 0, NULL};
static CMD_Info GETDATA_CMD			= { BRDctrl_DIO2IN_GETDATA,		0, 0, 0, NULL};
static CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_DIO2IN_GETSRCSTREAM, 1, 0, 0, NULL};

//***************************************************************************************
CDio2InSrv::CDio2InSrv(int idx, int srv_num, CModule* pModule, PDIO2INSRV_CFG pCfg) :
	CService(idx, _T("DIO2IN"), srv_num, pModule, pCfg, sizeof(DIO2INSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_EXCLUSIVE_ONLY;

	Init(&SETRATE_CMD, (CmdEntry)&CDio2InSrv::CtrlRate);
	Init(&GETRATE_CMD, (CmdEntry)&CDio2InSrv::CtrlRate);
	Init(&SETCLKMODE_CMD, (CmdEntry)&CDio2InSrv::CtrlClkMode);
	Init(&GETCLKMODE_CMD, (CmdEntry)&CDio2InSrv::CtrlClkMode);
	Init(&SETSYNCMODE_CMD, (CmdEntry)&CDio2InSrv::CtrlSyncMode);
	Init(&GETSYNCMODE_CMD, (CmdEntry)&CDio2InSrv::CtrlSyncMode);
	Init(&SETMASTER_CMD, (CmdEntry)&CDio2InSrv::CtrlMaster);
	Init(&GETMASTER_CMD, (CmdEntry)&CDio2InSrv::CtrlMaster);
	Init(&SETSTARTMODE_CMD, (CmdEntry)&CDio2InSrv::CtrlStartMode);
	Init(&GETSTARTMODE_CMD, (CmdEntry)&CDio2InSrv::CtrlStartMode);
	Init(&SETCMPLEVEL_CMD, (CmdEntry)&CDio2InSrv::CtrlComparLevel);
	Init(&GETCMPLEVEL_CMD, (CmdEntry)&CDio2InSrv::CtrlComparLevel);

	Init(&SETSYMBSYNC_CMD, (CmdEntry)&CDio2InSrv::CtrlSymbSync);

	Init(&SETCNT_CMD, (CmdEntry)&CDio2InSrv::CtrlCounter );
	Init(&GETCNT_CMD, (CmdEntry)&CDio2InSrv::CtrlCounter );

	Init(&FIFORESET_CMD, (CmdEntry)&CDio2InSrv::CtrlFifoReset);
	Init(&ENABLE_CMD, (CmdEntry)&CDio2InSrv::CtrlEnable);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&CDio2InSrv::CtrlFifoStatus);
	Init(&GETDATA_CMD, (CmdEntry)&CDio2InSrv::CtrlGetData);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&CDio2InSrv::CtrlGetAddrData);
}

//***************************************************************************************
int CDio2InSrv::CtrlIsAvailable(
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
	m_Dio2InTetrNum = GetTetrNum(pModule, m_index, DIO2IN_TETR_ID);
	if(m_MainTetrNum != -1 && m_Dio2InTetrNum != -1)
	{
		m_isAvailable = 1;
		PDIO2INSRV_CFG pSrvCfg = (PDIO2INSRV_CFG)m_pConfig;
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;
			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_Dio2InTetrNum;
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
int CDio2InSrv::CtrlGetAddrData(
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
	*(ULONG*)args = (AdmNum << 16) | m_Dio2InTetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int CDio2InSrv::GetClkSource(CModule* pModule, ULONG& ClkSrc)
{
	ULONG source;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio2InTetrNum;
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
int CDio2InSrv::GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PDIO2INSRV_CFG pSrvCfg = (PDIO2INSRV_CFG)m_pConfig;
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
int CDio2InSrv::SetClkSource(CModule* pModule, ULONG ClkSrc)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio2InTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_FMODE;
		param.val = ClkSrc;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
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
int CDio2InSrv::SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue)
{
	PDIO2INSRV_CFG pSrvCfg = (PDIO2INSRV_CFG)m_pConfig;
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
int CDio2InSrv::SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue)
{
	ULONG RateDivider = ROUND(ClkValue / Rate);
	RateDivider = RateDivider ? RateDivider : 1;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio2InTetrNum;
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
int CDio2InSrv::GetRate(CModule* pModule, double& Rate, double ClkValue)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio2InTetrNum;
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
int CDio2InSrv::CtrlRate(
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
	if(BRDctrl_DIO2IN_SETRATE == cmd)
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
int CDio2InSrv::CtrlClkMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_ClkMode pClk = (PBRD_ClkMode)args;
	if(BRDctrl_DIO2IN_SETCLKMODE == cmd)
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
int CDio2InSrv::CtrlSyncMode(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
					   )
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_SyncMode pSyncMode = (PBRD_SyncMode)args;
	if(BRDctrl_DIO2IN_SETSYNCMODE == cmd)
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
int CDio2InSrv::CtrlMaster(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
					   )
{
	int Status = BRDerr_OK;
	CModule* pModule = (CModule*)pDev;
	ULONG* pMasterMode = (ULONG*)args;
	DEVS_CMD_Reg param;
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	param.idxMain = m_index;
	param.reg = ADM2IFnr_MODE0;
	if(BRDctrl_DIO2IN_SETMASTER == cmd)
	{
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0->ByBits.Master = *pMasterMode >> 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.tetr = m_Dio2InTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0->ByBits.Master = *pMasterMode & 0x1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		param.tetr = m_Dio2InTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		*pMasterMode = pMode0->ByBits.Master;
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		*pMasterMode |= (pMode0->ByBits.Master << 1);
	}
	return Status;
}

//***************************************************************************************
int CDio2InSrv::SetStartMode(CModule* pModule, PVOID pStMode)
{
	PBRD_StartMode pStartMode = (PBRD_StartMode)pStMode;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio2InTetrNum;
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
		//pStMode->ByBits.Res1	  = pStartMode->symbSync;
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

		param.tetr = m_Dio2InTetrNum;
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
int CDio2InSrv::GetStartMode(CModule* pModule, PVOID pStMode)
{
	PBRD_StartMode pStartMode = (PBRD_StartMode)pStMode;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio2InTetrNum;
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

int CDio2InSrv::CtrlCounter(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_CntParam pCntParam = (PBRD_CntParam)args;
	PDIO2INSRV_CFG pSrvCfg = (PDIO2INSRV_CFG)m_pConfig;

	if(BRDctrl_DIO2IN_GETCNT == cmd)
	{
		pCntParam->count = pSrvCfg->Count[pCntParam->num];
		return BRDerr_OK;
	}

	int nReg = 0;

	switch( pCntParam->num )
	{
	case 0:
		nReg = ADM2IFnr_CNT0;
		break;
	case 1:
		nReg = ADM2IFnr_CNT1;
		break;
	case 2:
		nReg = ADM2IFnr_CNT2;
		break;
	}

	if( !nReg )
		return 0;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = nReg;
	param.val = pCntParam->count;

	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	pSrvCfg->Count[pCntParam->num] = pCntParam->count;

	return BRDerr_OK;
}


//***************************************************************************************
int CDio2InSrv::CtrlStartMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	if(BRDctrl_DIO2IN_SETSTARTMODE == cmd)
		Status = SetStartMode(pModule, args);
	else
        Status = GetStartMode(pModule, args);
	return BRDerr_OK;
}

int CDio2InSrv::CtrlSymbSync(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   )
{
	U32 *flag = (U32*)args;
	CModule* pModule = (CModule*)pDev;
	
	DEVS_CMD_Reg param;
	
	param.idxMain = m_index;
	param.tetr = m_Dio2InTetrNum;
	param.reg = ADM2IFnr_STMODE;

	PADM2IF_STMODE pStMode = (PADM2IF_STMODE)&param.val;

	pModule->RegCtrl(DEVScmd_REGREADIND, &param);

	pStMode->ByBits.Res1 = *flag;

	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;

}

//***************************************************************************************
int CDio2InSrv::CtrlComparLevel(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
				   )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_CmpLevel pCmpLvl = (PBRD_CmpLevel)args;
	PDIO2INSRV_CFG pSrvCfg = (PDIO2INSRV_CFG)m_pConfig;
	if(BRDctrl_DIO2IN_GETCMPLEVEL == cmd)
	{
		pCmpLvl->level = pSrvCfg->Level[pCmpLvl->num];
		return BRDerr_OK;
	}
	int cmp_num = 3;
	if(pCmpLvl->num	== 0 || pCmpLvl->num == 1)
		cmp_num = 3 + pCmpLvl->num;
	else
		if(pCmpLvl->num	== 2 || pCmpLvl->num == 3)
			cmp_num = 5 + pCmpLvl->num;
		else
			return BRDerr_BAD_PARAMETER;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus;
	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		pStatus = (PADM2IF_STATUS)&param.val;
	} while(!pStatus->ByBits.CmdRdy);

	double max_thr = pSrvCfg->RefPVS / 1000.;
	USHORT thr_data;
	if(fabs(pCmpLvl->level) > max_thr)
	  thr_data = pCmpLvl->level > 0.0 ? 255 : 0;
	else
	  thr_data = (USHORT)floor((pCmpLvl->level / max_thr + 1.) * 128 + 0.5);
	if(thr_data > 255)
		thr_data = 255;
	pSrvCfg->Level[pCmpLvl->num] = max_thr * (thr_data / 128. - 1);
	pCmpLvl->level = pSrvCfg->Level[pCmpLvl->num];
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = thr_data;
	pThDac->ByBits.Num = cmp_num;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CDio2InSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio2InTetrNum;
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
int CDio2InSrv::CtrlEnable (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio2InTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Start = *(PULONG)args;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDio2InSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio2InTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CDio2InSrv::CtrlGetData(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio2InTetrNum;
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
	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);

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

// ***************** End of file Dio2InSrv.cpp ***********************
