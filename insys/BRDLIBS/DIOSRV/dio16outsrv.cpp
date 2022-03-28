/*
 ***************** File Dio16OutSrv.cpp ***********************
 *
 * CTRL-command for BRD Driver for DIO16OUT service on DIO-V submodule
 *
 * (C) InSys by Dorokhin A. Feb 2006
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "dio16outsrv.h"

#define	CURRFILE "DIO16OUTSRV"

static CMD_Info SETPHASE_CMD		= { BRDctrl_DIO16OUT_SETPHASE,	   0, 0, 0, NULL};
static CMD_Info GETPHASE_CMD		= { BRDctrl_DIO16OUT_GETPHASE,	   1, 0, 0, NULL};
static CMD_Info SETOUTDATA_CMD		= { BRDctrl_DIO16OUT_SETOUTDATA,   0, 0, 0, NULL};
static CMD_Info GETOUTDATA_CMD		= { BRDctrl_DIO16OUT_GETOUTDATA,   1, 0, 0, NULL};
static CMD_Info SETSKIPDATA_CMD		= { BRDctrl_DIO16OUT_SETSKIPDATA,  0, 0, 0, NULL};
static CMD_Info GETSKIPDATA_CMD		= { BRDctrl_DIO16OUT_GETSKIPDATA,  1, 0, 0, NULL};
static CMD_Info SETCYCLMODE_CMD		= { BRDctrl_DIO16OUT_SETCYCLMODE,  0, 0, 0, NULL};
static CMD_Info GETCYCLMODE_CMD		= { BRDctrl_DIO16OUT_GETCYCLMODE,  1, 0, 0, NULL};
static CMD_Info SETCLKMODE_CMD		= { BRDctrl_DIO16OUT_SETCLKMODE,   0, 0, 0, NULL};
static CMD_Info GETCLKMODE_CMD		= { BRDctrl_DIO16OUT_GETCLKMODE,   1, 0, 0, NULL};

static CMD_Info FIFORESET_CMD		= { BRDctrl_DIO16OUT_FIFORESET,	  0, 0, 0, NULL};
static CMD_Info ENABLE_CMD			= { BRDctrl_DIO16OUT_ENABLE,	  0, 0, 0, NULL};
static CMD_Info FIFOSTATUS_CMD		= { BRDctrl_DIO16OUT_FIFOSTATUS,  1, 0, 0, NULL};
static CMD_Info PUTDATA_CMD			= { BRDctrl_DIO16OUT_PUTDATA,	  0, 0, 0, NULL};
static CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_DIO16OUT_GETSRCSTREAM, 1, 0, 0, NULL};

//***************************************************************************************
CDio16OutSrv::CDio16OutSrv(int idx, int srv_num, CModule* pModule, PDIO16OUTSRV_CFG pCfg) :
	CService(idx, _T("DIO16OUT"), srv_num, pModule, pCfg, sizeof(DIO16OUTSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_OUT |
			BRDserv_ATTR_STREAMABLE_OUT |
			BRDserv_ATTR_EXCLUSIVE_ONLY;

	Init(&SETPHASE_CMD, (CmdEntry)&CDio16OutSrv::CtrlPhase);
	Init(&GETPHASE_CMD, (CmdEntry)&CDio16OutSrv::CtrlPhase);
	Init(&SETOUTDATA_CMD, (CmdEntry)&CDio16OutSrv::CtrlOutData);
	Init(&GETOUTDATA_CMD, (CmdEntry)&CDio16OutSrv::CtrlOutData);
	Init(&SETSKIPDATA_CMD, (CmdEntry)&CDio16OutSrv::CtrlSkipData);
	Init(&GETSKIPDATA_CMD, (CmdEntry)&CDio16OutSrv::CtrlSkipData);
	Init(&SETCYCLMODE_CMD, (CmdEntry)&CDio16OutSrv::CtrlCyclingMode);
	Init(&GETCYCLMODE_CMD, (CmdEntry)&CDio16OutSrv::CtrlCyclingMode);
	Init(&SETCLKMODE_CMD, (CmdEntry)&CDio16OutSrv::CtrlClkMode);
	Init(&GETCLKMODE_CMD, (CmdEntry)&CDio16OutSrv::CtrlClkMode);

	Init(&FIFORESET_CMD, (CmdEntry)&CDio16OutSrv::CtrlFifoReset);
	Init(&ENABLE_CMD, (CmdEntry)&CDio16OutSrv::CtrlEnable);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&CDio16OutSrv::CtrlFifoStatus);
	Init(&PUTDATA_CMD, (CmdEntry)&CDio16OutSrv::CtrlPutData);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&CDio16OutSrv::CtrlGetAddrData);
}

//***************************************************************************************
int CDio16OutSrv::CtrlIsAvailable(
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
	m_Dio16OutTetrNum = GetTetrNum(pModule, m_index, DIO16OUT_TETR_ID);

	if(m_MainTetrNum != -1 && m_Dio16OutTetrNum != -1)
	{
		m_isAvailable = 1;
		PDIO16OUTSRV_CFG pSrvCfg = (PDIO16OUTSRV_CFG)m_pConfig;
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;
			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_Dio16OutTetrNum;
			if(!pSrvCfg->FifoSize)
			{
				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				pSrvCfg->FifoSize = RegParam.val * widthFifo;
			}
			RegParam.tetr = m_Dio16OutTetrNum;
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
int CDio16OutSrv::CtrlGetAddrData(
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
	*(ULONG*)args = (AdmNum << 16) | m_Dio16OutTetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int CDio16OutSrv::CtrlPhase(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PLONG pValue = (PLONG)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16OutTetrNum;
	param.reg = DIO16OUTnr_MODE2;

	if(BRDctrl_DIO16OUT_SETPHASE == cmd)
	{
		if(*pValue > 255) *pValue = 255;
		if(*pValue < -256) *pValue = -256;
		PDIO16OUT_MODE2 pMode2 = (PDIO16OUT_MODE2)&param.val;
		pMode2->ByBits.PhaseVal = labs(*pValue);
		pMode2->ByBits.PhaseSign = (*pValue < 0) ? 1 : 0;
		pMode2->ByBits.PhaseEn = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		pMode2->ByBits.PhaseEn = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PDIO16OUT_MODE2 pMode2 = (PDIO16OUT_MODE2)&param.val;
		LONG value = pMode2->ByBits.PhaseVal;
		*pValue = (pMode2->ByBits.PhaseSign) ? (-value) : value;
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CDio16OutSrv::CtrlCyclingMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16OutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_DIO16OUT_SETCYCLMODE == cmd)
	{
		pMode0->ByBits.Cycle = *(ULONG*)args;
		pMode0->ByBits.Reset = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
		*(ULONG*)args = pMode0->ByBits.Cycle;
	return BRDerr_OK;
}

//***************************************************************************************
// ver. 2.00- !!!
//int CDio16OutSrv::CtrlClkMode(
//							void		*pDev,		// InfoSM or InfoBM
//							void		*pServData,	// Specific Service Data
//							ULONG		cmd,		// Command Code (from BRD_ctrl())
//							void		*args 		// Command Arguments (from BRD_ctrl())
//							)
//{
//	int Status = BRDerr_CMD_UNSUPPORTED;
//	CModule* pModule = (CModule*)pDev;
//	PULONG pMode = (PULONG)args;
//	DEVS_CMD_Reg param;
//	param.idxMain = m_index;
//	param.tetr = m_Dio16OutTetrNum;
//	param.reg = DIO16OUTnr_MODE2;
//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	PDIO16OUT_MODE2 pMode2 = (PDIO16OUT_MODE2)&param.val;
//
//	if(BRDctrl_DIO16OUT_SETCLKMODE == cmd)
//	{
//		pMode2->ByBits.ClkInv = *pMode;
//		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	}
//	else
//		*pMode = pMode2->ByBits.ClkInv;
//
//	return BRDerr_OK;
//}

//***************************************************************************************
// ver. 2.10 only !!!
int CDio16OutSrv::CtrlClkMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_DioClkMode pClkMode = (PBRD_DioClkMode)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16OutTetrNum;
	param.reg = ADM2IFnr_FMODE;

	if(BRDctrl_DIO16OUT_SETCLKMODE == cmd)
	{
		param.val = pClkMode->src;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.reg = DIO16OUTnr_MODE2;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PDIO16OUT_MODE2 pMode2 = (PDIO16OUT_MODE2)&param.val;
		pMode2->ByBits.ClkInv = pClkMode->inv;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pClkMode->src = param.val;
		param.reg = DIO16OUTnr_MODE2;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PDIO16OUT_MODE2 pMode2 = (PDIO16OUT_MODE2)&param.val;
		pClkMode->inv = pMode2->ByBits.ClkInv;
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CDio16OutSrv::CtrlOutData(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_EnVal pEnVal = (PBRD_EnVal)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16OutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_DIO16OUT_SETOUTDATA == cmd)
	{
		pMode0->ByBits.Cnt1En = pEnVal->enable;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.reg = ADM2IFnr_CNT1;
		param.val = pEnVal->value;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		pEnVal->enable = pMode0->ByBits.Cnt1En;
		param.reg = ADM2IFnr_CNT1;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pEnVal->value = param.val;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CDio16OutSrv::CtrlSkipData(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_EnVal pEnVal = (PBRD_EnVal)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16OutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_DIO16OUT_SETSKIPDATA == cmd)
	{
		pMode0->ByBits.Cnt2En = pEnVal->enable;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.reg = ADM2IFnr_CNT2;
		param.val = pEnVal->value;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		pEnVal->enable = pMode0->ByBits.Cnt2En;
		param.reg = ADM2IFnr_CNT2;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pEnVal->value = param.val;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CDio16OutSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16OutTetrNum;
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
int CDio16OutSrv::CtrlEnable (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16OutTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Start = *(PULONG)args;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDio16OutSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16OutTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CDio16OutSrv::CtrlPutData(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16OutTetrNum;

	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);

	return BRDerr_OK;
}

// ***************** End of file Dio16OutSrv.cpp ***********************
