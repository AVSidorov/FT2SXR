/*
 ***************** File TestExSrv.cpp ***********************
 *
 * BRD Driver for ADM-interface TestEx service
 *
 * (C) InSys by Dorokhin A. Feb 2008
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "testexsrv.h"

#define	CURRFILE "TESTEXSRV"

static CMD_Info SETMUXCTRL_CMD		= { BRDctrl_TESTEX_SETMUXCTRL,	0, 0, 0, NULL};
static CMD_Info GETMUXCTRL_CMD		= { BRDctrl_TESTEX_GETMUXCTRL,	1, 0, 0, NULL};
static CMD_Info SETCHKMODE_CMD		= { BRDctrl_TESTEX_SETCHKMODE,	0, 0, 0, NULL};
static CMD_Info GETCHKMODE_CMD		= { BRDctrl_TESTEX_GETCHKMODE,	1, 0, 0, NULL};
static CMD_Info SETCHKSIZE_CMD		= { BRDctrl_TESTEX_SETCHKSIZE,	0, 0, 0, NULL};
static CMD_Info GETCHKSIZE_CMD		= { BRDctrl_TESTEX_GETCHKSIZE,	1, 0, 0, NULL};
static CMD_Info SETGENMODE_CMD		= { BRDctrl_TESTEX_SETGENMODE,	0, 0, 0, NULL};
static CMD_Info GETGENMODE_CMD		= { BRDctrl_TESTEX_GETGENMODE,	1, 0, 0, NULL};
static CMD_Info SETGENSIZE_CMD		= { BRDctrl_TESTEX_SETGENSIZE,	0, 0, 0, NULL};
static CMD_Info GETGENSIZE_CMD		= { BRDctrl_TESTEX_GETGENSIZE,	1, 0, 0, NULL};
static CMD_Info SETGENENCNT_CMD		= { BRDctrl_TESTEX_SETGENENCNT,	0, 0, 0, NULL};
static CMD_Info GETGENENCNT_CMD		= { BRDctrl_TESTEX_GETGENENCNT,	1, 0, 0, NULL};
static CMD_Info SETGENDSCNT_CMD		= { BRDctrl_TESTEX_SETGENDSCNT,	0, 0, 0, NULL};
static CMD_Info GETGENDSCNT_CMD		= { BRDctrl_TESTEX_GETGENDSCNT,	1, 0, 0, NULL};

static CMD_Info INFIFORESET_CMD		= { BRDctrl_TESTEX_INFIFORESET,		0, 0, 0, NULL};
static CMD_Info INENABLE_CMD		= { BRDctrl_TESTEX_INENABLE,   		0, 0, 0, NULL};
static CMD_Info INFIFOSTATUS_CMD	= { BRDctrl_TESTEX_INFIFOSTATUS,	1, 0, 0, NULL};
static CMD_Info GETDATA_CMD			= { BRDctrl_TESTEX_GETDATA,			0, 0, 0, NULL};
static CMD_Info GETINSRCSTREAM_CMD	= { BRDctrl_TESTEX_GETINSRCSTREAM,	1, 0, 0, NULL};

static CMD_Info OUTFIFORESET_CMD	= { BRDctrl_TESTEX_OUTFIFORESET,	0, 0, 0, NULL};
static CMD_Info OUTENABLE_CMD		= { BRDctrl_TESTEX_OUTENABLE,   	0, 0, 0, NULL};
static CMD_Info OUTFIFOSTATUS_CMD	= { BRDctrl_TESTEX_OUTFIFOSTATUS,	1, 0, 0, NULL};
static CMD_Info PUTDATA_CMD			= { BRDctrl_TESTEX_PUTDATA,			0, 0, 0, NULL};
static CMD_Info GETOUTSRCSTREAM_CMD = { BRDctrl_TESTEX_GETOUTSRCSTREAM,	1, 0, 0, NULL};

static CMD_Info CHKRESET_CMD	= { BRDctrl_TESTEX_CHKRESET, 0, 0, 0, NULL};
static CMD_Info CHKSTART_CMD	= { BRDctrl_TESTEX_CHKSTART, 0, 0, 0, NULL};
static CMD_Info CHKSTOP_CMD		= { BRDctrl_TESTEX_CHKSTOP, 0, 0, 0, NULL};
static CMD_Info CHKSTATUS_CMD	= { BRDctrl_TESTEX_CHKSTATUS, 1, 0, 0, NULL};
static CMD_Info GETCHKERROR_CMD	= { BRDctrl_TESTEX_GETCHKERROR, 1, 0, 0, NULL};
static CMD_Info GETCHKERMEM_CMD	= { BRDctrl_TESTEX_GETCHKERMEM, 1, 0, 0, NULL};

static CMD_Info GENRESET_CMD	= { BRDctrl_TESTEX_GENRESET, 0, 0, 0, NULL};
static CMD_Info GENSTART_CMD	= { BRDctrl_TESTEX_GENSTART, 0, 0, 0, NULL};
static CMD_Info GENSTOP_CMD		= { BRDctrl_TESTEX_GENSTOP, 0, 0, 0, NULL};
static CMD_Info GENCNTEN_CMD	= { BRDctrl_TESTEX_GENCNTEN, 0, 0, 0, NULL};
static CMD_Info GENCNTDS_CMD	= { BRDctrl_TESTEX_GENCNTDS, 0, 0, 0, NULL};
static CMD_Info GENSTATUS_CMD	= { BRDctrl_TESTEX_GENSTATUS, 1, 0, 0, NULL};

static CMD_Info READDIR_CMD			= { BRDctrl_TESTEX_READDIR,	1, 0, 0, NULL};
static CMD_Info READSDIR_CMD		= { BRDctrl_TESTEX_READSDIR,	1, 0, 0, NULL};
static CMD_Info READIND_CMD			= { BRDctrl_TESTEX_READIND,	1, 0, 0, NULL};
static CMD_Info READSIND_CMD		= { BRDctrl_TESTEX_READSIND,	1, 0, 0, NULL};
static CMD_Info WRITEDIR_CMD		= { BRDctrl_TESTEX_WRITEDIR,	0, 0, 0, NULL};
static CMD_Info WRITESDIR_CMD		= { BRDctrl_TESTEX_WRITESDIR,	0, 0, 0, NULL};
static CMD_Info WRITEIND_CMD		= { BRDctrl_TESTEX_WRITEIND,	0, 0, 0, NULL};
static CMD_Info WRITESIND_CMD		= { BRDctrl_TESTEX_WRITESIND,	0, 0, 0, NULL};

//***************************************************************************************
CTestExSrv::CTestExSrv(int idx, int srv_num, CModule* pModule, PTESTEXSRV_CFG pCfg) :
	CService(idx, _T("TESTEX"), srv_num, pModule, pCfg, sizeof(TESTEXSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_DIRECTION_OUT |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_STREAMABLE_OUT;// |
//			BRDserv_ATTR_UNVISIBLE |
//			BRDserv_ATTR_SUBSERVICE_ONLY |
//			BRDserv_ATTR_EXCLUSIVE_ONLY;

	Init(&SETMUXCTRL_CMD, (CmdEntry)&CTestExSrv::CtrlMux);
	Init(&GETMUXCTRL_CMD, (CmdEntry)&CTestExSrv::CtrlMux);

	Init(&SETCHKMODE_CMD, (CmdEntry)&CTestExSrv::CtrlChkMode);
	Init(&GETCHKMODE_CMD, (CmdEntry)&CTestExSrv::CtrlChkMode);
	Init(&SETCHKSIZE_CMD, (CmdEntry)&CTestExSrv::CtrlChkSize);
	Init(&GETCHKSIZE_CMD, (CmdEntry)&CTestExSrv::CtrlChkSize);
	Init(&SETGENMODE_CMD, (CmdEntry)&CTestExSrv::CtrlGenMode);
	Init(&GETGENMODE_CMD, (CmdEntry)&CTestExSrv::CtrlGenMode);
	Init(&SETGENSIZE_CMD, (CmdEntry)&CTestExSrv::CtrlGenSize);
	Init(&GETGENSIZE_CMD, (CmdEntry)&CTestExSrv::CtrlGenSize);
	Init(&SETGENENCNT_CMD, (CmdEntry)&CTestExSrv::CtrlGenEnblCnt);
	Init(&GETGENENCNT_CMD, (CmdEntry)&CTestExSrv::CtrlGenEnblCnt);
	Init(&SETGENDSCNT_CMD, (CmdEntry)&CTestExSrv::CtrlGenDisCnt);
	Init(&GETGENDSCNT_CMD, (CmdEntry)&CTestExSrv::CtrlGenDisCnt);

	Init(&INFIFORESET_CMD, (CmdEntry)&CTestExSrv::CtrlFifoReset);
	Init(&INENABLE_CMD, (CmdEntry)&CTestExSrv::CtrlEnable);
	Init(&INFIFOSTATUS_CMD, (CmdEntry)&CTestExSrv::CtrlFifoStatus);
	Init(&GETDATA_CMD, (CmdEntry)&CTestExSrv::CtrlGetData);
	Init(&GETINSRCSTREAM_CMD, (CmdEntry)&CTestExSrv::CtrlGetAddrData);

	Init(&OUTFIFORESET_CMD, (CmdEntry)&CTestExSrv::CtrlFifoReset);
	Init(&OUTENABLE_CMD, (CmdEntry)&CTestExSrv::CtrlEnable);
	Init(&OUTFIFOSTATUS_CMD, (CmdEntry)&CTestExSrv::CtrlFifoStatus);
	Init(&PUTDATA_CMD, (CmdEntry)&CTestExSrv::CtrlPutData);
	Init(&GETOUTSRCSTREAM_CMD, (CmdEntry)&CTestExSrv::CtrlGetAddrData);

	Init(&CHKRESET_CMD, (CmdEntry)&CTestExSrv::CtrlCheckReset);
	Init(&CHKSTART_CMD, (CmdEntry)&CTestExSrv::CtrlCheckStartStop);
	Init(&CHKSTOP_CMD, (CmdEntry)&CTestExSrv::CtrlCheckStartStop);
	Init(&CHKSTATUS_CMD, (CmdEntry)&CTestExSrv::CtrlCheckStatus);
	Init(&GETCHKERROR_CMD, (CmdEntry)&CTestExSrv::CtrlGetCheckError);
	Init(&GETCHKERMEM_CMD, (CmdEntry)&CTestExSrv::CtrlGetCheckErMem);
	
	Init(&GENRESET_CMD, (CmdEntry)&CTestExSrv::CtrlGenReset);
	Init(&GENSTART_CMD, (CmdEntry)&CTestExSrv::CtrlGenStartStop);
	Init(&GENSTOP_CMD, (CmdEntry)&CTestExSrv::CtrlGenStartStop);
	Init(&GENCNTEN_CMD, (CmdEntry)&CTestExSrv::CtrlGenCnt);
	Init(&GENCNTDS_CMD, (CmdEntry)&CTestExSrv::CtrlGenCnt);
	Init(&GENSTATUS_CMD, (CmdEntry)&CTestExSrv::CtrlGenStatus);

	Init(&READDIR_CMD, (CmdEntry)&CTestExSrv::CtrlReadDir);
	Init(&READSDIR_CMD, (CmdEntry)&CTestExSrv::CtrlReadsDir);
	Init(&READIND_CMD, (CmdEntry)&CTestExSrv::CtrlReadInd);
	Init(&READSIND_CMD, (CmdEntry)&CTestExSrv::CtrlReadsInd);
	Init(&WRITEDIR_CMD, (CmdEntry)&CTestExSrv::CtrlWriteDir);
	Init(&WRITESDIR_CMD, (CmdEntry)&CTestExSrv::CtrlWritesDir);
	Init(&WRITEIND_CMD, (CmdEntry)&CTestExSrv::CtrlWriteInd);
	Init(&WRITESIND_CMD, (CmdEntry)&CTestExSrv::CtrlWritesInd);
}

//***************************************************************************************
void CTestExSrv::TetradInit(CModule* pModule, ULONG tetrNum)
{
		DEVS_CMD_Reg RegParam;
		RegParam.idxMain = m_index;
		RegParam.tetr = tetrNum;

		RegParam.reg = ADM2IFnr_MODE0;
		RegParam.val = 0;
	//	pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
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

		RegParam.reg = ADM2IFnr_MODE0;
		pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
//		PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&RegParam.val;
		pMode0->ByBits.Master = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
}

//***************************************************************************************
ULONG CTestExSrv::GetFifoSize(CModule* pModule, ULONG tetrNum)
{
		DEVS_CMD_Reg RegParam;
		RegParam.idxMain = m_index;
		RegParam.tetr = tetrNum;
		RegParam.reg = ADM2IFnr_FTYPE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
		int widthFifo = RegParam.val >> 3;
		RegParam.reg = ADM2IFnr_FSIZE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
		return (RegParam.val * widthFifo);
}

//***************************************************************************************
int CTestExSrv::CtrlIsAvailable(
							  void		*pDev,		// InfoSM or InfoBM
							  void		*pServData,	// Specific Service Data
							  ULONG		cmd,		// Command Code (from BRD_ctrl())
							  void		*args 		// Command Arguments (from BRD_ctrl())
							  )
{
	CModule* pModule = (CModule*)pDev;
	PSERV_CMD_IsAvailable pServAvailable = (PSERV_CMD_IsAvailable)args;
	pServAvailable->attr = m_attribute;
	pServAvailable->isAvailable = 0;

	m_MainTetrNum = GetTetrNum(pModule, m_index, MAIN_TETR_ID);
	m_TestCtrlTetrNum = GetTetrNum(pModule, m_index, TESTCTRL_TETR_ID);
	m_Dio64OutTetrNum = GetTetrNum(pModule, m_index, DIO64OUT_TETR_ID);
	m_Dio64InTetrNum = GetTetrNum(pModule, m_index, DIO64IN_TETR_ID);
	if(m_MainTetrNum != -1 && m_TestCtrlTetrNum && m_Dio64OutTetrNum != -1 && m_Dio64InTetrNum != -1)
	{
		m_isAvailable = 1;

		PTESTEXSRV_CFG pSrvCfg = (PTESTEXSRV_CFG)m_pConfig;
		pSrvCfg->OutFifoSize = GetFifoSize(pModule, m_Dio64OutTetrNum);
		pSrvCfg->InFifoSize = GetFifoSize(pModule, m_Dio64InTetrNum);

		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;
			TetradInit(pModule, m_TestCtrlTetrNum);
			TetradInit(pModule, m_Dio64OutTetrNum);
			TetradInit(pModule, m_Dio64InTetrNum);
		}
	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	ULONG m_AdmNum;
	//TCHAR buf[SERVNAME_SIZE];
	//_tcscpy_s(buf, m_name);
	//PTCHAR pBuf = buf + (strlen(buf) - 2);
	//if(_tcschr(pBuf, '1'))
	//	m_AdmNum = 1;
	//else
		m_AdmNum = 0;

		*(ULONG*)args = (m_AdmNum << 16) | m_Dio64OutTetrNum;
	if(BRDctrl_TESTEX_GETINSRCSTREAM == cmd)
		*(ULONG*)args = (m_AdmNum << 16) | m_Dio64InTetrNum;

	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlMux(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_MUXCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PTESTEX_MUX pMux = (PTESTEX_MUX)&param.val;
	if(BRDctrl_TESTEX_SETMUXCTRL == cmd)
	{
		pMux->ByBits.OutMux = *(ULONG*)args;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		*(ULONG*)args = pMux->ByBits.OutMux;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlChkMode(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						 )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_TestExMode pMode = (PBRD_TestExMode)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_CHKCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PTESTEX_CHECK pCheck = (PTESTEX_CHECK)&param.val;
	if(BRDctrl_TESTEX_SETCHKMODE == cmd)
	{
		pCheck->ByBits.FixMode = pMode->FixMode;
		pCheck->ByBits.BlockType = pMode->BlockType;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		pMode->FixMode = pCheck->ByBits.FixMode;
		pMode->BlockType = pCheck->ByBits.BlockType;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlChkSize(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_CHKSIZE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	if(BRDctrl_TESTEX_SETCHKSIZE == cmd)
	{
		param.val = *(ULONG*)args;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		*(ULONG*)args = param.val;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGenMode(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						 )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_TestExMode pMode = (PBRD_TestExMode)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_GENCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PTESTEX_GEN pGen = (PTESTEX_GEN)&param.val;
	if(BRDctrl_TESTEX_SETGENMODE == cmd)
	{
		pGen->ByBits.FixMode = pMode->FixMode;
		pGen->ByBits.BlockType = pMode->BlockType;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		pMode->FixMode = pGen->ByBits.FixMode;
		pMode->BlockType = pGen->ByBits.BlockType;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGenSize(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_GENSIZE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	if(BRDctrl_TESTEX_SETGENSIZE == cmd)
	{
		param.val = *(ULONG*)args;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		*(ULONG*)args = param.val;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGenEnblCnt(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_GENENBLCNT;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	if(BRDctrl_TESTEX_SETGENENCNT == cmd)
	{
		param.val = *(ULONG*)args;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		*(ULONG*)args = param.val;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGenDisCnt(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_GENDISCNT;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	if(BRDctrl_TESTEX_SETGENDSCNT == cmd)
	{
		param.val = *(ULONG*)args;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		*(ULONG*)args = param.val;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlFifoReset(
								void		*pDev,		// InfoSM or InfoBM
								void		*pServData,	// Specific Service Data
								ULONG		cmd,		// Command Code (from BRD_ctrl())
								void		*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio64OutTetrNum;
	if(BRDctrl_TESTEX_INFIFORESET == cmd)
		param.tetr = m_Dio64InTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pMode0->ByBits.FifoRes = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlEnable(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
						   )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio64OutTetrNum;
	if(BRDctrl_TESTEX_INENABLE == cmd)
		param.tetr = m_Dio64InTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Start = *(PULONG)args;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlFifoStatus(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio64OutTetrNum;
	if(BRDctrl_TESTEX_INFIFOSTATUS == cmd)
		param.tetr = m_Dio64InTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
//	ULONG data = pStatus->ByBits.Underflow;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGetData(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio64InTetrNum;
	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlPutData(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dio64OutTetrNum;
	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlCheckReset(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_CHKCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PTESTEX_CHECK pCheck = (PTESTEX_CHECK)&param.val;
	pCheck->ByBits.Reset = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pCheck->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlCheckStartStop(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_CHKCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PTESTEX_CHECK pCheck = (PTESTEX_CHECK)&param.val;
	if(BRDctrl_TESTEX_CHKSTART == cmd)
		pCheck->ByBits.Start = 1;
	else
		pCheck->ByBits.Start = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlCheckStatus(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_CHKRDBLKL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	ULONG blk_cnt = param.val;
	param.reg = TESTEXnr_CHKRDBLKH;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	blk_cnt |= (param.val << 16);
	*(PULONG)args = blk_cnt;
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGetCheckError(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							 )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_CheckError pError = (PBRD_CheckError)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_CHKOKBLKL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	ULONG data = param.val;
	param.reg = TESTEXnr_CHKOKBLKH;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	data |= (param.val << 16);
	pError->BlockOKCnt = data;

	param.reg = TESTEXnr_CHKERRBLKL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	data = param.val;
	param.reg = TESTEXnr_CHKERRBLKH;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	data |= (param.val << 16);
	pError->BlockErrCnt = data;

	param.reg = TESTEXnr_CHKTOTALERRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	data = param.val;
	param.reg = TESTEXnr_CHKTOTALERRH;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	data |= (param.val << 16);
	pError->TotalError = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGetCheckErMem(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							 )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_CheckErMem pError = (PBRD_CheckErMem)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_CHKERRADDR;
	PTESTEX_CHECKERADR pChkAdr = (PTESTEX_CHECKERADR)&param.val;
	pChkAdr->ByBits.AdrWord = pError->AdrWord;
	pChkAdr->ByBits.NumError = pError->NumError;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = TESTEXnr_CHKERRDATA;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pError->Data = param.val;

	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGenReset(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_GENCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PTESTEX_GEN pGen = (PTESTEX_GEN)&param.val;
	pGen->ByBits.Reset = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pGen->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGenStartStop(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_GENCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PTESTEX_GEN pGen = (PTESTEX_GEN)&param.val;
	if(BRDctrl_TESTEX_GENSTART == cmd)
		pGen->ByBits.Start = 1;
	else
		pGen->ByBits.Start = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGenCnt(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_GENCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PTESTEX_GEN pGen = (PTESTEX_GEN)&param.val;
	if(BRDctrl_TESTEX_GENCNTEN == cmd)
		pGen->ByBits.CntEnbl = 1;
	else
		pGen->ByBits.CntEnbl = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlGenStatus(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						 )
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_TestCtrlTetrNum;
	param.reg = TESTEXnr_GENWRBLKL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	ULONG blk_cnt = param.val;
	param.reg = TESTEXnr_GENWRBLKH;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	blk_cnt |= (param.val << 16);
	*(PULONG)args = blk_cnt;
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlReadDir(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	pReg->val = param.val;
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlReadsDir(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.pBuf = pReg->pBuf;
	param.bytes = pReg->bytes;
	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlReadInd(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pReg->val = param.val;
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlReadsInd(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.pBuf = pReg->pBuf;
	param.bytes = pReg->bytes;
	pModule->RegCtrl(DEVScmd_REGREADSIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlWriteDir(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.val = pReg->val;
	pModule->RegCtrl(DEVScmd_REGWRITEDIR, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlWritesDir(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.pBuf = pReg->pBuf;
	param.bytes = pReg->bytes;
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlWriteInd(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.val = pReg->val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CTestExSrv::CtrlWritesInd(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.pBuf = pReg->pBuf;
	param.bytes = pReg->bytes;
	pModule->RegCtrl(DEVScmd_REGWRITESIND, &param);
	return BRDerr_OK;
}


// ***************** End of file TestExSrv.cpp ***********************
