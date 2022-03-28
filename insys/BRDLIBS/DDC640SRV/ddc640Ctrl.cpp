/*
 ***************** File ddc640Ctrl.cpp *******************************************
 *
 * CTRL-command for BRD Driver for ddc640 service 
 *
 * (C) InSys by Sclyarov A. Oct. 2014
 *
 ********************************************************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "ddc640Srv.h"
#define CURRFILE _BRDC("DDC640CTRL")
CMD_Info SETMODE_CMD		= { BRDctrl_DDC640_SETMODE, 0, 0, 0, NULL};
CMD_Info SETCHANS_CMD		= { BRDctrl_DDC640_SETCHANS,	0, 0, 0, NULL};
CMD_Info SETFCWB_CMD		= { BRDctrl_DDC640_SETFCWB, 0, 0, 0, NULL};
CMD_Info SETFCNB_CMD		= { BRDctrl_DDC640_SETFCNB, 0, 0, 0, NULL};
CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_DDC640_GETSRCSTREAM, 1, 0, 0, NULL};
CMD_Info FIFORESET_CMD		= { BRDctrl_DDC640_FIFORESET,	 0, 0, 0, NULL};
CMD_Info START_CMD			= { BRDctrl_DDC640_START,		 0, 0, 0, NULL};
CMD_Info STOP_CMD			= { BRDctrl_DDC640_STOP,		 0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_DDC640_FIFOSTATUS,	 1, 0, 0, NULL};
CMD_Info GETSTAT_CMD		= { BRDctrl_DDC640_GETSTAT,	 1, 0, 0, NULL};


//***************************************************************************************
Cddc640Srv::Cddc640Srv(int idx, int srv_num, CModule* pModule, PDDC640SRV_CFG pCfg) :
	CService(idx,  _BRDC("DDC640"), srv_num, pModule, pCfg, sizeof(PDDC640SRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_SDRAMABLE |
			BRDserv_ATTR_CMPABLE |
//			BRDserv_ATTR_DSPABLE |
			BRDserv_ATTR_EXCLUSIVE_ONLY;
	

	Init(&SETMODE_CMD, (CmdEntry)&Cddc640Srv::CtrlSetMode);
	Init(&SETCHANS_CMD, (CmdEntry)&Cddc640Srv::CtrlSetChans);
	Init(&SETFCWB_CMD, (CmdEntry)&Cddc640Srv::CtrlSetFcWb);
	Init(&SETFCNB_CMD, (CmdEntry)&Cddc640Srv::CtrlSetFcNb);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&Cddc640Srv::CtrlGetAddrData);
	Init(&FIFORESET_CMD, (CmdEntry)&Cddc640Srv::CtrlFifoReset);
	Init(&START_CMD, (CmdEntry)&Cddc640Srv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&Cddc640Srv::CtrlStart);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&Cddc640Srv::CtrlFifoStatus);
	Init(&GETSTAT_CMD, (CmdEntry)&Cddc640Srv::CtrlGetStat);
}
//***************************************************************************************
//***************************************************************************************
int Cddc640Srv::CtrlSetMode(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetMode(pModule, args);
	return Status;
}
//***************************************************************************************
int Cddc640Srv::CtrlSetChans(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetChans(pModule, args);
	return Status;
}
//***************************************************************************************
int Cddc640Srv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
//	*(ULONG*)args = (m_AdmNum << 16) | m_QmTetrNum;
	*(ULONG*)args = m_ddc640TetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int Cddc640Srv::CtrlSetFcWb(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetFcWb( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
//***************************************************************************************
int Cddc640Srv::CtrlSetFcNb(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetFcNb( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
//***************************************************************************************
int Cddc640Srv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc640TetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//Sleep(1);
	pMode0->ByBits.FifoRes = 0;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//Sleep(1);
	return BRDerr_OK;
}

//***************************************************************************************
int Cddc640Srv::CtrlStart (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc640TetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_DDC640_START	 == cmd)
	{
		pMode0->ByBits.Start = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		if(!pMode0->ByBits.Master)
		{ // Master/Slave
			param.tetr = m_MainTetrNum;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
			if(pMode0->ByBits.Master)
			{ // Master
				pMode0->ByBits.Start = 1;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			}
		}
	}else
	{
//		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0->ByBits.Start = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//		if(!pMode0->ByBits.Master)
		{ // Master/Slave
			param.tetr = m_MainTetrNum;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//			if(pMode0->ByBits.Master)
			{ // Master
				pMode0->ByBits.Start = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			}
		}
	}
	return BRDerr_OK;
}
//***************************************************************************************
int Cddc640Srv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc640TetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}
//***************************************************************************************
int Cddc640Srv::CtrlGetStat(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;

	CModule* pModule = (CModule*)pDev;
	Status = GetStat(pModule, args);
	return Status;
}
//***************************************************************************************
//***************************************************************************************
int Cddc640Srv::CtrlIsAvailable(
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
	GetDdcTetrNum(pModule);
	
	if(m_MainTetrNum != -1 && m_ddc640TetrNum != -1)
	{
		m_isAvailable = 1;
		PDDC640SRV_CFG pSrvCfg = (PDDC640SRV_CFG)m_pConfig; 
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_ddc640TetrNum;
			{
				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
	//			pDacSrvCfg->FifoSize = RegParam.val << 3;
				pSrvCfg->FifoSize = RegParam.val * widthFifo;
			}
			RegParam.tetr = m_ddc640TetrNum;
			RegParam.reg = ADM2IFnr_MODE0;
			RegParam.val = 0;
		//	pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
			PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&RegParam.val;
			pMode0->ByBits.Reset = 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			for(int i = 1; i < 32; i++)
//			for(int i = 1; i < 3; i++)
			{
				RegParam.reg = i;
				RegParam.val = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

			}
			RegParam.reg = ADM2IFnr_MODE0;
	//		pMode0->ByBits.AdmClk = 1;
			pMode0->ByBits.Reset = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

//-------------------------------------------------------------------------
//----------------------------------------------------------------------------
		}

	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}
//***************************************************************************************
// ***************** End of file ddc640Ctrl.cpp ***********************
