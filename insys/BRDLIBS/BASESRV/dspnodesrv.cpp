/*
 ***************** File DspNodeSrv.cpp ***********************
 *
 * BRD Driver for DSP Node service
 *
 * (C) InSys by Dorokhin A. Jun 2005
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "dspnodesrv.h"

#define	CURRFILE "DSPNODESRV"

CMD_Info SETMODE_CMD		= { BRDctrl_DSPNODE_SETMODE,		0, 0, 0, NULL};
CMD_Info GETMODE_CMD		= { BRDctrl_DSPNODE_GETMODE,		1, 0, 0, NULL};

CMD_Info GETCFG_CMD			= { BRDctrl_DSPNODE_GETCFG,			1, 0, 0, NULL};

CMD_Info GETDLGPAGES_CMD	= { BRDctrl_DSPNODE_GETDLGPAGES,	1, 0, 0, NULL};

CMD_Info FIFORESET_CMD		= { BRDctrl_DSPNODE_FIFORESET,		0, 0, 0, NULL};
CMD_Info ENABLE_CMD			= { BRDctrl_DSPNODE_ENABLE,   		0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_DSPNODE_FIFOSTATUS,		1, 0, 0, NULL};
CMD_Info GETDATA_CMD		= { BRDctrl_DSPNODE_GETDATA,		0, 0, 0, NULL};
CMD_Info SETDATA_CMD		= { BRDctrl_DSPNODE_SETDATA,		0, 0, 0, NULL};
CMD_Info READMEMDATA_CMD	= { BRDctrl_DSPNODE_READMEMDATA,	0, 0, 0, NULL};
CMD_Info WRITEMEMDATA_CMD	= { BRDctrl_DSPNODE_WRITEMEMDATA,	0, 0, 0, NULL};

CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_DSPNODE_GETSRCSTREAM,	1, 0, 0, NULL};

//***************************************************************************************
CDspNodeSrv::CDspNodeSrv(int idx, int srv_num, CModule* pModule, PDSPNODESRV_CFG pCfg) :
	CService(idx, _T("DSPNODE"), srv_num, pModule, pCfg, sizeof(DSPNODESRV_CFG))
//CDspNodeSrv::CDspNodeSrv(int idx, LPTSTR name, int srv_num, CModule* pModule, PVOID pCfg, ULONG cfgSize) :
//	CService(idx, name, srv_num, pModule, pCfg, cfgSize)
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_DIRECTION_OUT |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_UNVISIBLE |
			BRDserv_ATTR_SUBSERVICE_ONLY |
			BRDserv_ATTR_EXCLUSIVE_ONLY;

	Init(&GETCFG_CMD, (CmdEntry)&CDspNodeSrv::CtrlCfg);

	Init(&GETDLGPAGES_CMD, (CmdEntry)&CDspNodeSrv::CtrlGetPages);

	Init(&SETMODE_CMD, (CmdEntry)&CDspNodeSrv::CtrlMode);
	Init(&GETMODE_CMD, (CmdEntry)&CDspNodeSrv::CtrlMode);

	Init(&FIFORESET_CMD, (CmdEntry)&CDspNodeSrv::CtrlFifoReset);
	Init(&ENABLE_CMD, (CmdEntry)&CDspNodeSrv::CtrlEnable);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&CDspNodeSrv::CtrlFifoStatus);
	Init(&GETDATA_CMD, (CmdEntry)&CDspNodeSrv::CtrlGetData);
	Init(&SETDATA_CMD, (CmdEntry)&CDspNodeSrv::CtrlSetData);
	Init(&READMEMDATA_CMD, (CmdEntry)&CDspNodeSrv::CtrlReadData);
	Init(&WRITEMEMDATA_CMD, (CmdEntry)&CDspNodeSrv::CtrlWriteData);

	Init(&GETSRCSTREAM_CMD, (CmdEntry)&CDspNodeSrv::CtrlGetAddrData);
}

//***************************************************************************************
void CDspNodeSrv::TetradInit(CModule* pModule, ULONG tetrNum)
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
ULONG CDspNodeSrv::GetFifoSize(CModule* pModule, ULONG tetrNum)
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
int CDspNodeSrv::CtrlIsAvailable(
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
	m_OutTetrNum = GetTetrNum(pModule, m_index, OUT_TETR_ID);
	m_InTetrNum = GetTetrNum(pModule, m_index, IN_TETR_ID);
	m_SbsramTetrNum = GetTetrNum(pModule, m_index, SBSRAM_TETR_ID);

	if(m_MainTetrNum != -1 && m_OutTetrNum != -1 && m_InTetrNum != -1 && m_SbsramTetrNum != -1)
	{
		m_isAvailable = 1;
		PDSPNODESRV_CFG pSrvCfg = (PDSPNODESRV_CFG)m_pConfig;
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;
			pSrvCfg->OutFifoSize = GetFifoSize(pModule, m_OutTetrNum);
			pSrvCfg->InFifoSize = GetFifoSize(pModule, m_InTetrNum);

			TetradInit(pModule, m_OutTetrNum);
			TetradInit(pModule, m_InTetrNum);
			TetradInit(pModule, m_SbsramTetrNum);
		}
	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	ULONG m_AdmNum;
	TCHAR buf[SERVNAME_SIZE];
	_tcscpy_s(buf, m_name);
	PTCHAR pBuf = buf + (strlen(buf) - 2);
	if(_tcschr(pBuf, '1'))
//	if(_tcschr(m_name, '1'))
		m_AdmNum = 3;
	else
		m_AdmNum = 1;
	*(ULONG*)args = (m_AdmNum << 16) | m_InTetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::CtrlGetPages(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_PropertyList pList = (PBRD_PropertyList)args;
//	ULONG dlg_mode = *(PULONG)args;

	if(cmd == BRDctrl_DSPNODE_GETDLGPAGES)
	{
		// Open Library
		PDSPNODESRV_CFG pSrvCfg = (PDSPNODESRV_CFG)m_pConfig;
		pList->hLib = LoadLibrary(pSrvCfg->DlgDllName);
		if(pList->hLib <= (HINSTANCE)HINSTANCE_ERROR)
			return BRDerr_BAD_DEVICE_VITAL_DATA;

		//INFO_InitPages_Type* pDlgFunc; 
		//pDlgFunc = (INFO_InitPages_Type*)GetProcAddress(pList->hLib, _T("INFO_InitPages"));
		//if(!pDlgFunc)
		//{
		//	FreeLibrary(pList->hLib);
		//	return BRDerr_BAD_DEVICE_VITAL_DATA;
		//}
		PVOID pInfo = GetInfoForDialog(pModule);

		//ULONG num_dlg;
		//int num_pages = (pDlgFunc)(pDev, this, &num_dlg, pInfo);
		//pList->PagesCnt = num_pages;
		//pList->NumDlg = num_dlg;

		FreeInfoForDialog(pInfo);
	}
	//if(cmd == BRDctrl_SDRAM_ENDPAGESDLG)
	//{
	//	FreeLibrary(pList->hLib);
	//}
	//if (nResponse == IDOK)
	//{
	//	// TODO: Place code here to handle when the dialog is
	//	//  dismissed with OK
	//	SetPropertyFromDialog(pDev, pInfo);
	//}
	//else if (nResponse == IDCANCEL)
	//{
	//	// TODO: Place code here to handle when the dialog is
	//	//  dismissed with Cancel
	//}

	return BRDerr_OK;
}

//***************************************************************************************
void CDspNodeSrv::FreeInfoForDialog(PVOID pInfo)
{
//	PSDRAMSRV_INFO pSrvInfo = (PSDRAMSRV_INFO)pInfo;
//	delete pSrvInfo;
}

//***************************************************************************************
PVOID CDspNodeSrv::GetInfoForDialog(CModule* pDev)
{
	PDSPNODESRV_CFG pSrvCfg = (PDSPNODESRV_CFG)m_pConfig;
//	PSDRAMSRV_INFO pSrvInfo = new SDRAMSRV_INFO;
//	pSrvInfo->Size = sizeof(SDRAMSRV_INFO);
//	pSrvInfo->SlotCnt = pSrvCfg->SlotCnt;			// количество установленных слотов
//	pSrvInfo->ModuleCnt = pSrvCfg->ModuleCnt;		// количество установленных DIMM-модулей(занятых слотов)
//	pSrvInfo->RowAddrBits = pSrvCfg->RowAddrBits;	// количество разрядов адреса строк
//	pSrvInfo->ColAddrBits = pSrvCfg->ColAddrBits;	// количество разрядов адреса столбцов
//	pSrvInfo->ModuleBanks = pSrvCfg->ModuleBanks;	// количество банков в DIMM-модуле
//	pSrvInfo->ChipBanks = pSrvCfg->ChipBanks;		// количество банков в микросхемах DIMM-модуля
//
//	CDspNodeSrv::GetMode(pDev, pSrvInfo->ReadMode); // режим чтения памяти (авто/произвольный)
//
//	TCHAR module_name[40];
////	sprintf(module_name, " (%s)", pDev->m_name);
//	sprintf(module_name, " (%s)", pDev->GetName());
////	PBASE_Info binfo = pModule->GetBaseInfo();
////	sprintf(module_name, " (%s)", binfo->boardName);
//	_tcscpy(pSrvInfo->Name, m_name);
//	_tcscat(pSrvInfo->Name, module_name);
//
//	return pSrvInfo;
	return pSrvCfg;
//	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::SetPropertyFromDialog(void	*pDev, void	*args)
{
	CModule* pModule = (CModule*)pDev;
//	PSDRAMSRV_INFO pInfo = (PSDRAMSRV_INFO)args;
//	CDspNodeSrv::SetMode(pModule, pInfo->ReadMode);

	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::GetCfg(PBRD_DspNodeCfg pCfg)
{
	PDSPNODESRV_CFG pSrvCfg = (PDSPNODESRV_CFG)m_pConfig;
	pCfg->SramChips = pSrvCfg->SramChips;
	pCfg->SramChipSize = pSrvCfg->SramChipSize;
	pCfg->SramChipWidth = pSrvCfg->SramChipWidth;
	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::CtrlCfg(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	Status = GetCfg((PBRD_DspNodeCfg)args);
	return Status;
}

//***************************************************************************************
int CDspNodeSrv::CtrlMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	if(BRDctrl_DSPNODE_SETMODE == cmd)
	{
		ULONG mode = *(ULONG*)args;
		Status = SetMode(pModule, mode);
		if(!mode && pServData)
		{
			PUNDERSERV_Cmd pReleaseCmd = (PUNDERSERV_Cmd)pServData;
			pReleaseCmd->code = SERVcmd_SYS_RELEASE;
		}
	}
	else
	{
		ULONG mode;
		Status = GetMode(pModule, mode);
		*(ULONG*)args = mode;
	}
	return Status;
}

//***************************************************************************************
int CDspNodeSrv::SetMode(CModule* pModule, ULONG mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_InTetrNum;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PIN_MODE1 pMode = (PIN_MODE1)&param.val;
	pMode->ByBits.Flow = mode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::GetMode(CModule* pModule, ULONG& mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_InTetrNum;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PIN_MODE1 pMode = (PIN_MODE1)&param.val;
	mode = pMode->ByBits.Flow;
	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::CtrlFifoReset(
								void		*pDev,		// InfoSM or InfoBM
								void		*pServData,	// Specific Service Data
								ULONG		cmd,		// Command Code (from BRD_ctrl())
								void		*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_InTetrNum;
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
int CDspNodeSrv::CtrlEnable(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
						   )
{
	//CModule* pModule = (CModule*)pDev;
	//DEVS_CMD_Reg param;
	//param.idxMain = m_index;
	//param.tetr = m_MemTetrNum;
	//param.reg = ADM2IFnr_MODE0;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	//pMode0->ByBits.Start = *(PULONG)args;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::CtrlFifoStatus(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
//	param.tetr = m_OutTetrNum;
	param.tetr = m_InTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
//	ULONG data = pStatus->ByBits.Underflow;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::CtrlGetData(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_InTetrNum;
	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::CtrlSetData(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_OutTetrNum;
	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::CtrlReadData(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	CModule* pModule = (CModule*)pDev;

	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	PUSHORT buf = (PUSHORT)pBuf->pData;
	ULONG num = pBuf->size / sizeof(ULONG);

	ULONG addr = 0;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SbsramTetrNum;

	int j = 0;
	for(ULONG i = 0; i < num; i++, addr++)
	{
		param.reg = SBSRAMnr_ADDRL;
		param.val = addr & 0xFFFF;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.reg = SBSRAMnr_ADDRH;
		PSBSRAM_ADDRH pAddrH = (PSBSRAM_ADDRH)&param.val;
		pAddrH->ByBits.Addr = (addr >> 16) & 0xFFFF;
		pAddrH->ByBits.Bank = 0;
		pAddrH->ByBits.Cmd = SBSRAMcmd_READ;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.reg = 0x200;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		buf[j++] = param.val;

		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		buf[j++] = param.val;

	}

	return BRDerr_OK;
}

//***************************************************************************************
int CDspNodeSrv::CtrlWriteData(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	CModule* pModule = (CModule*)pDev;

	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	PUSHORT buf = (PUSHORT)pBuf->pData;
	ULONG num = pBuf->size / sizeof(ULONG);

	ULONG addr = 0;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SbsramTetrNum;

	int j = 0;
	for(ULONG i = 0; i < num; i++, addr++)
	{
		param.reg = SBSRAMnr_DATAL;
		param.val = buf[j++];
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.reg = SBSRAMnr_DATAH;
		param.val = buf[j++];
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.reg = SBSRAMnr_ADDRL;
		param.val = addr & 0xFFFF;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.reg = SBSRAMnr_ADDRH;
		PSBSRAM_ADDRH pAddrH = (PSBSRAM_ADDRH)&param.val;
		pAddrH->ByBits.Addr = (addr >> 16) & 0xFFFF;
		pAddrH->ByBits.Bank = 0;
		pAddrH->ByBits.Cmd = SBSRAMcmd_WRITE;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	}

	return BRDerr_OK;
}

// ***************** End of file DspNodeSrv.cpp ***********************
