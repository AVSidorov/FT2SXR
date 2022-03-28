/*
 ***************** File Dio162InSrv.cpp ***********************
 *
 * CTRL-command for BRD Driver for DIO16IN service on DIO-V submodule
 *
 * (C) InSys by Dorokhin A. Feb 2006
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "dio16insrv.h"

#define	CURRFILE "DIO16INSRV"

static CMD_Info SETPHASE_CMD		= { BRDctrl_DIO16IN_SETPHASE,	   0, 0, 0, NULL};
static CMD_Info GETPHASE_CMD		= { BRDctrl_DIO16IN_GETPHASE,	   1, 0, 0, NULL};
static CMD_Info SETSINGLEMODE_CMD	= { BRDctrl_DIO16IN_SETSINGLEMODE, 0, 0, 0, NULL};
static CMD_Info GETSINGLEMODE_CMD	= { BRDctrl_DIO16IN_GETSINGLEMODE, 1, 0, 0, NULL};
static CMD_Info SINGLEWRITE_CMD		= { BRDctrl_DIO16IN_SINGLEWRITE,   0, 0, 0, NULL};
static CMD_Info SINGLEREAD_CMD		= { BRDctrl_DIO16IN_SINGLEREAD,	   0, 0, 0, NULL};
static CMD_Info SETTESTMODE_CMD		= { BRDctrl_DIO16IN_SETTESTMODE,   0, 0, 0, NULL};
static CMD_Info GETTESTMODE_CMD		= { BRDctrl_DIO16IN_GETTESTMODE,   1, 0, 0, NULL};
static CMD_Info SETADDRDATA_CMD		= { BRDctrl_DIO16IN_SETADDRDATA,   0, 0, 0, NULL};
static CMD_Info GETADDRDATA_CMD		= { BRDctrl_DIO16IN_GETADDRDATA,   1, 0, 0, NULL};
static CMD_Info SETCLKMODE_CMD		= { BRDctrl_DIO16IN_SETCLKMODE,    0, 0, 0, NULL};
static CMD_Info GETCLKMODE_CMD		= { BRDctrl_DIO16IN_GETCLKMODE,    1, 0, 0, NULL};

static CMD_Info FIFORESET_CMD		= { BRDctrl_DIO16IN_FIFORESET,	  0, 0, 0, NULL};
static CMD_Info ENABLE_CMD			= { BRDctrl_DIO16IN_ENABLE,		  0, 0, 0, NULL};
static CMD_Info FIFOSTATUS_CMD		= { BRDctrl_DIO16IN_FIFOSTATUS,	  1, 0, 0, NULL};
static CMD_Info GETDATA_CMD			= { BRDctrl_DIO16IN_GETDATA,	  0, 0, 0, NULL};
static CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_DIO16IN_GETSRCSTREAM, 1, 0, 0, NULL};

static CMD_Info FPGALOAD_CMD		= { BRDctrl_DIO16IN_FPGALOAD,	  0, 0, 0, NULL};
static CMD_Info FPGACFG_CMD			= { BRDctrl_DIO16IN_FPGACFG,	  1, 0, 0, NULL};

//***************************************************************************************
CDio16InSrv::CDio16InSrv(int idx, int srv_num, CModule* pModule, PDIO16INSRV_CFG pCfg) :
	CService(idx, _T("DIO16IN"), srv_num, pModule, pCfg, sizeof(DIO16INSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_EXCLUSIVE_ONLY;

	Init(&SETPHASE_CMD, (CmdEntry)&CDio16InSrv::CtrlPhase);
	Init(&GETPHASE_CMD, (CmdEntry)&CDio16InSrv::CtrlPhase);
	Init(&SETSINGLEMODE_CMD, (CmdEntry)&CDio16InSrv::CtrlSingleMode);
	Init(&GETSINGLEMODE_CMD, (CmdEntry)&CDio16InSrv::CtrlSingleMode);
	Init(&SINGLEWRITE_CMD, (CmdEntry)&CDio16InSrv::CtrlSingleWrite);
	Init(&SINGLEREAD_CMD, (CmdEntry)&CDio16InSrv::CtrlSingleRead);
	Init(&SETTESTMODE_CMD, (CmdEntry)&CDio16InSrv::CtrlTestMode);
	Init(&GETTESTMODE_CMD, (CmdEntry)&CDio16InSrv::CtrlTestMode);
	Init(&SETADDRDATA_CMD, (CmdEntry)&CDio16InSrv::CtrlAddrData);
	Init(&GETADDRDATA_CMD, (CmdEntry)&CDio16InSrv::CtrlAddrData);
	Init(&SETCLKMODE_CMD, (CmdEntry)&CDio16InSrv::CtrlClkMode);
	Init(&GETCLKMODE_CMD, (CmdEntry)&CDio16InSrv::CtrlClkMode);

	Init(&FIFORESET_CMD, (CmdEntry)&CDio16InSrv::CtrlFifoReset);
	Init(&ENABLE_CMD, (CmdEntry)&CDio16InSrv::CtrlEnable);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&CDio16InSrv::CtrlFifoStatus);
	Init(&GETDATA_CMD, (CmdEntry)&CDio16InSrv::CtrlGetData);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&CDio16InSrv::CtrlGetAddrData);

	Init(&FPGALOAD_CMD, (CmdEntry)&CDio16InSrv::CtrlFpgaLoad);
	Init(&FPGACFG_CMD, (CmdEntry)&CDio16InSrv::CtrlFpgaCfg);
}

//***************************************************************************************
int CDio16InSrv::CtrlIsAvailable(
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
	m_Dio16InTetrNum = GetTetrNum(pModule, m_index, DIO16IN_TETR_ID);
	m_AdmFPGATetrNum = GetTetrNum(pModule, m_index, ADMFPGA_TETR_ID);
	if(m_MainTetrNum != -1 && m_Dio16InTetrNum != -1 && m_AdmFPGATetrNum != -1)
	{
		m_isAvailable = 1;
		PDIO16INSRV_CFG pSrvCfg = (PDIO16INSRV_CFG)m_pConfig;
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;
			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_Dio16InTetrNum;
			PDIO16INSRV_CFG pSrvCfg = (PDIO16INSRV_CFG)m_pConfig;
			if(!pSrvCfg->FifoSize)
			{
				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				pSrvCfg->FifoSize = RegParam.val * widthFifo;
			}
			RegParam.tetr = m_Dio16InTetrNum;
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

			RegParam.tetr = m_AdmFPGATetrNum;
			RegParam.reg = ADM2IFnr_MODE0;
			RegParam.val = 0;
			pMode0 = (PADM2IF_MODE0)&RegParam.val;
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
int CDio16InSrv::CtrlGetAddrData(
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
	*(ULONG*)args = (AdmNum << 16) | m_Dio16InTetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int CDio16InSrv::CtrlPhase(
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
	param.tetr = m_Dio16InTetrNum;
	param.reg = DIO16INnr_MODE2;

	if(BRDctrl_DIO16IN_SETPHASE == cmd)
	{
		if(*pValue > 255) *pValue = 255;
		if(*pValue < -256) *pValue = -256;
		PDIO16IN_MODE2 pMode2 = (PDIO16IN_MODE2)&param.val;
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
		PDIO16IN_MODE2 pMode2 = (PDIO16IN_MODE2)&param.val;
		LONG value = pMode2->ByBits.PhaseVal;
		*pValue = (pMode2->ByBits.PhaseSign) ? (-value) : value;
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CDio16InSrv::CtrlSingleMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PULONG pMode = (PULONG)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16InTetrNum;
	param.reg = DIO16INnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDIO16IN_CTRL pCtrl = (PDIO16IN_CTRL)&param.val;

	if(BRDctrl_DIO16IN_SETSINGLEMODE == cmd)
	{
		pCtrl->ByBits.SingleMode = *pMode;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
		*pMode = pCtrl->ByBits.SingleMode;

	return BRDerr_OK;
}

//***************************************************************************************
int CDio16InSrv::CtrlSingleWrite(
								void		*pDev,		// InfoSM or InfoBM
								void		*pServData,	// Specific Service Data
								ULONG		cmd,		// Command Code (from BRD_ctrl())
								void		*args 		// Command Arguments (from BRD_ctrl())
								)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	ULONG data = *(PULONG)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16InTetrNum;
	param.reg = DIO16INnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDIO16IN_CTRL pCtrl = (PDIO16IN_CTRL)&param.val;
	pCtrl->ByBits.ReadWrite = 1; // 1 - write
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = DIO16INnr_WRITE;
	param.val = data;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CDio16InSrv::CtrlSingleRead(
								void		*pDev,		// InfoSM or InfoBM
								void		*pServData,	// Specific Service Data
								ULONG		cmd,		// Command Code (from BRD_ctrl())
								void		*args 		// Command Arguments (from BRD_ctrl())
								)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16InTetrNum;
	param.reg = DIO16INnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDIO16IN_CTRL pCtrl = (PDIO16IN_CTRL)&param.val;
	pCtrl->ByBits.ReadWrite = 0; // 0 - read 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = DIO16INnr_RDCTRL;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = DIO16INnr_READ;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	
	*(PULONG)args = param.val;

	return BRDerr_OK;
}

//***************************************************************************************
int CDio16InSrv::CtrlTestMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PULONG pMode = (PULONG)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16InTetrNum;
	param.reg = DIO16INnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDIO16IN_CTRL pCtrl = (PDIO16IN_CTRL)&param.val;

	if(BRDctrl_DIO16IN_SETTESTMODE == cmd)
	{
		pCtrl->ByBits.TestMode = *pMode;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
		*pMode = pCtrl->ByBits.TestMode;

	return BRDerr_OK;
}

//***************************************************************************************
int CDio16InSrv::CtrlAddrData(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PULONG pMode = (PULONG)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16InTetrNum;
	param.reg = DIO16INnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PDIO16IN_CTRL pCtrl = (PDIO16IN_CTRL)&param.val;

	if(BRDctrl_DIO16IN_SETADDRDATA == cmd)
	{
		pCtrl->ByBits.AddrData = *pMode;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
		*pMode = pCtrl->ByBits.AddrData;

	return BRDerr_OK;
}

//***************************************************************************************
// ver. 2.00- !!!
//int CDio16InSrv::CtrlClkMode(
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
//	param.tetr = m_Dio16InTetrNum;
//	param.reg = DIO16INnr_MODE2;
//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	PDIO16IN_MODE2 pMode2 = (PDIO16IN_MODE2)&param.val;
//
//	if(BRDctrl_DIO16IN_SETCLKMODE == cmd)
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
int CDio16InSrv::CtrlClkMode(
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
	param.tetr = m_Dio16InTetrNum;
	param.reg = ADM2IFnr_FMODE;

	if(BRDctrl_DIO16IN_SETCLKMODE == cmd)
	{
		param.val = pClkMode->src;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.reg = DIO16INnr_MODE2;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PDIO16IN_MODE2 pMode2 = (PDIO16IN_MODE2)&param.val;
		pMode2->ByBits.ClkInv = pClkMode->inv;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pClkMode->src = param.val;
		param.reg = DIO16INnr_MODE2;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PDIO16IN_MODE2 pMode2 = (PDIO16IN_MODE2)&param.val;
		pClkMode->inv = pMode2->ByBits.ClkInv;
	}
	return BRDerr_OK;
}
//***************************************************************************************
int CDio16InSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16InTetrNum;
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
int CDio16InSrv::CtrlEnable (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16InTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Start = *(PULONG)args;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDio16InSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16InTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CDio16InSrv::CtrlGetData(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio16InTetrNum;
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

//***************************************************************************************
ULONG CDio16InSrv::ReadPldFile(PCTSTR PldFileName, LPVOID& fileBuffer, DWORD& fileSize)
{
	char *FirstChar;
	char FullFileName[MAX_PATH];
	GetFullPathName(PldFileName, MAX_PATH, FullFileName, &FirstChar);
	HANDLE hFile = CreateFile(FullFileName,
								GENERIC_READ, 
								FILE_SHARE_READ, NULL, 
								OPEN_EXISTING, 
								FILE_ATTRIBUTE_NORMAL,
								NULL);
    if(hFile == INVALID_HANDLE_VALUE)
	{
	    return BRDerr_BAD_FILE;
	}
	fileSize = SetFilePointer(hFile, 0, NULL, FILE_END);
	if(fileSize == 0xffffffff)
	{
	    return BRDerr_BAD_FILE;
	}
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	fileBuffer = VirtualAlloc(NULL, fileSize, MEM_COMMIT, PAGE_READWRITE);
	if(fileBuffer == NULL)
	{
		return BRDerr_NOT_ENOUGH_MEMORY;
	}
	DWORD dwNumBytesRead;
	if(ReadFile(hFile, fileBuffer, fileSize, &dwNumBytesRead, NULL) == 0)
	{
		VirtualFree(fileBuffer, 0, MEM_RELEASE);
	    return BRDerr_BAD_FILE;
	}
	if(CloseHandle(hFile) == 0)
	{
		VirtualFree(fileBuffer, 0, MEM_RELEASE);
	    return BRDerr_BAD_FILE;
	}
	return BRDerr_OK;
}

//****************************************************************************************
void KrnlSscanf(PUCHAR buf, ULONG n, PVOID digit)
{
	PUCHAR d = (PUCHAR)digit;
	for(ULONG i = 0; i < n; i++)
	{
		UCHAR tetrada = (buf[i] >= '0' && buf[i] <= '9') ? buf[i] - '0':
						(buf[i] >= 'A' && buf[i] <= 'F') ? buf[i] - 'A' + 10:
						(buf[i] >= 'a' && buf[i] <= 'f') ? buf[i] - 'a' + 10:0;
		if (i%2 == 0)
			d[i/2] = tetrada << 4;
		else
			d[i/2] |= tetrada;
	}
}

//****************************************************************************************
ULONG CDio16InSrv::PldGetHexRecord(PUCHAR& pPldBuf, PLD_RECORD *rec)
{
	UCHAR buf[PLD_MAXRECORDSIZE];

	// Get HEX record Header
	memcpy(buf, pPldBuf, PLD_HEADERSIZE);
	pPldBuf += PLD_HEADERSIZE;
	if(buf[0] != ':') 
		return BRDerr_DIO16IN_PLDFORMAT;
	buf[PLD_HEADERSIZE] = '\0';

	KrnlSscanf(buf + 1, 2, &rec->len);
	KrnlSscanf(buf + 3, 4, &rec->addr);
	KrnlSscanf(buf + 7, 2, &rec->type);

	UCHAR chkSum = rec->len + ((UCHAR)(((USHORT)(rec->addr) >> 8) & 0xFF)) + 
			(UCHAR)(rec->addr) + rec->type;

	// Get HEX record Data
	ULONG DataLength = (rec->len + 1)*sizeof(USHORT) + lstrlen("\r\n");
	memcpy(buf, pPldBuf, DataLength);
	pPldBuf += DataLength;
	buf[DataLength] = '\0';
	for(UCHAR i = 0; i < rec->len + 1; i++)
	{
		KrnlSscanf(buf + 2*i, 2, &rec->data[i]);
		chkSum = chkSum + rec->data[i];
	}

	if(chkSum != 0)
		return BRDerr_DIO16IN_PLDFORMAT;

	return BRDerr_OK;
}

//***************************************************************************************
ULONG CDio16InSrv::LoadHexFile(CModule* pModule, BYTE* pldDataBuf)
{
	ULONG status = BRDerr_DIO16IN_PLDNOTRDY;
	PLD_RECORD Record;
	bool BreakDo = false;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdmFPGATetrNum;
	do
	{
		status = PldGetHexRecord(pldDataBuf, &Record);
		if(status != BRDerr_OK)
			break;
		switch(Record.type)
		{
			case PLD_rectEND:
			{
				 // Write 100 bits to zero counter (PLD CAD bug ?)
				int i = 0;
				for(i = 0; i < 100; i++)
				{
					param.reg = ADM2IFnr_STATUS;
					do {
						pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
					}while(!(param.val & 0x4000));
					param.reg = ADMFPGAnr_MODE2;
					param.val = 0x0000;
					pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
				}
				for(i = 0; i < 200; i++)
				{
					param.reg = ADM2IFnr_STATUS;
					do {
						pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
					}while(!(param.val & 0x4000));
					if(param.val & 0x8000)
						break;
					param.reg = ADMFPGAnr_MODE2;
					param.val = 0x0000;
					pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
				}
				if(i == 200)
					status = BRDerr_DIO16IN_PLDNOTRDY;
				else
					status = BRDerr_OK;
				break;
			}
			case PLD_rectDATA:
			{
				for(UCHAR i = 0; i < Record.len; i++)
				{
					param.reg = ADM2IFnr_STATUS;
					do {
						pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
					}while(!(param.val & 0x4000));
					param.reg = ADMFPGAnr_MODE2;
					BYTE tmp = Record.data[i];
					param.val = (ULONG)tmp;
					param.val &= 0xff;
			//		param.val = (ULONG)(pldDataBuf[i]);
					pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
				}
				break;
			}
			default:
				break;
		}
		if(BreakDo)
			break;
	} while (Record.len != 0);
	return status;
}

//#include	<conio.h>
//***************************************************************************************
int CDio16InSrv::CtrlFpgaLoad(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PCTSTR PldFileName = (PCTSTR)args;
	LPVOID fileBuffer = NULL;
	DWORD fileSize;
	ULONG Status = ReadPldFile(PldFileName, fileBuffer, fileSize);
	if(Status != BRDerr_OK)
		return Status;

	BYTE* pldDataBuf = (BYTE*)fileBuffer;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdmFPGATetrNum;

	param.reg = ADM2IFnr_STATUS;
	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	}while(!(param.val & 0x4000)); // waiting FPGA_READY == 1 (STATUS[14])
	param.reg = ADMFPGAnr_MODE2;
	param.val = 0x0001;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	param.reg = ADM2IFnr_STATUS;
	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	}while(!(param.val & 0x4000)); // waiting FPGA_READY == 1 (STATUS[14])
	param.reg = ADMFPGAnr_MODE2;
	param.val = 0x0301;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	param.reg = ADM2IFnr_STATUS;
	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	}while(param.val & 0x2000); // waiting INIT == 0 (STATUS[13])

	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	}while(!(param.val & 0x4000)); // waiting FPGA_READY == 1 (STATUS[14])
	param.reg = ADMFPGAnr_MODE2;
	param.val = 0x0201;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	param.reg = ADM2IFnr_STATUS;
	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	}while(!(param.val & 0x2000)); // waiting INIT == 1 (STATUS[13])

	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	}while(!(param.val & 0x4000));
	param.reg = ADMFPGAnr_MODE2;
	param.val = 0x0201;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	param.reg = ADM2IFnr_STATUS;
	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	}while(!(param.val & 0x4000));
	param.reg = ADMFPGAnr_MODE2;
	param.val = 0x0601;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);

	if(pldDataBuf[0] == ':')
		LoadHexFile(pModule, pldDataBuf);
	else
	{
		ULONG i = 0;
		for(i = 0; i < fileSize; i++)
		{
			param.reg = ADM2IFnr_STATUS;
			do {
				pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
			}while(!(param.val & 0x4000));
			param.reg = ADMFPGAnr_MODE2;
			BYTE tmp = pldDataBuf[i];
			param.val = (ULONG)tmp;
			param.val &= 0xff;
			//if(i < 10)
			//{
			//	printf("ADMFPGA[%d] = %d\n", i, param.val);
			//	_getch();
			//}
	//		param.val = (ULONG)(pldDataBuf[i]);
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
		for(i = 0; i < 100; i++)
		{
			param.reg = ADM2IFnr_STATUS;
			do {
				pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
			}while(!(param.val & 0x4000));
	//		if(param.val & 0x8000)
	//			break;
			param.reg = ADMFPGAnr_MODE2;
			param.val = 0x0000;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}

		for(i = 0; i < 200; i++)
		{
			param.reg = ADM2IFnr_STATUS;
			do {
				pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
			}while(!(param.val & 0x4000));
			if(param.val & 0x8000)
				break;
			param.reg = ADMFPGAnr_MODE2;
			param.val = 0x0000;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
	}
	VirtualFree(fileBuffer, 0, MEM_RELEASE);

	return BRDerr_OK;
}

//***************************************************************************************
int CDio16InSrv::CtrlFpgaCfg(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_AdmFpgaCfg pCfg = (PBRD_AdmFpgaCfg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_AdmFPGATetrNum;

	param.reg = ADMFPGAnr_SIG;
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pCfg->sig = param.val & 0xffff;

	param.reg = ADMFPGAnr_ID;
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pCfg->id = param.val & 0xffff;

	param.reg = ADMFPGAnr_MOD;
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pCfg->mod = param.val & 0xffff;

	param.reg = ADMFPGAnr_VER;
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pCfg->ver = param.val & 0xffff;

	return BRDerr_OK;
}

// ***************** End of file Dio16InSrv.cpp ***********************
