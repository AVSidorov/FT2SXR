/*
 ***************** File ddc24Ctrl.cpp *******************************************
 *
 * CTRL-command for BRD Driver for ddc24 service 
 *
 * (C) InSys by Sclyarov A. Oct. 2013
 *
 ********************************************************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "ddc24Srv.h"
#define CURRFILE _BRDC("DDC24CTRL")
CMD_Info SETCLOCK_CMD		= { BRDctrl_DDC24_SETCLOCK, 0, 0, 0, NULL};
CMD_Info SETFC_CMD			= { BRDctrl_DDC24_SETFC, 0, 0, 0, NULL};
CMD_Info SETCHAN_CMD		= { BRDctrl_DDC24_SETCHAN,	0, 0, 0, NULL};
CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_DDC24_GETSRCSTREAM, 1, 0, 0, NULL};
CMD_Info FIFORESET_CMD		= { BRDctrl_DDC24_FIFORESET,	 0, 0, 0, NULL};
CMD_Info START_CMD			= { BRDctrl_DDC24_START,		 0, 0, 0, NULL};
CMD_Info STOP_CMD			= { BRDctrl_DDC24_STOP,		 0, 0, 0, NULL};
CMD_Info STARTDDC_CMD		= { BRDctrl_DDC24_STARTDDC,		 0, 0, 0, NULL};
CMD_Info STOPDDC_CMD		= { BRDctrl_DDC24_STOPDDC,		 0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_DDC24_FIFOSTATUS,	 1, 0, 0, NULL};


//***************************************************************************************
Cddc24Srv::Cddc24Srv(int idx, int srv_num, CModule* pModule, PDDC24SRV_CFG pCfg) :
	CService(idx,  _BRDC("DDC24"), srv_num, pModule, pCfg, sizeof(PDDC24SRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_SDRAMABLE |
			BRDserv_ATTR_CMPABLE |
//			BRDserv_ATTR_DSPABLE |
			BRDserv_ATTR_EXCLUSIVE_ONLY;
	

	Init(&SETCLOCK_CMD, (CmdEntry)&Cddc24Srv::CtrlSetClock);
	Init(&SETFC_CMD, (CmdEntry)&Cddc24Srv::CtrlSetFc);
	Init(&SETCHAN_CMD, (CmdEntry)&Cddc24Srv::CtrlSetChan);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&Cddc24Srv::CtrlGetAddrData);
	Init(&FIFORESET_CMD, (CmdEntry)&Cddc24Srv::CtrlFifoReset);
	Init(&START_CMD, (CmdEntry)&Cddc24Srv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&Cddc24Srv::CtrlStart);
	Init(&STARTDDC_CMD, (CmdEntry)&Cddc24Srv::CtrlStartDdc);
	Init(&STOPDDC_CMD, (CmdEntry)&Cddc24Srv::CtrlStartDdc);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&Cddc24Srv::CtrlFifoStatus);
}
//***************************************************************************************
int Cddc24Srv::CtrlSetClock(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetClock(pModule, args);
	return Status;
}
//***************************************************************************************
//***************************************************************************************
int Cddc24Srv::CtrlSetChan(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetChan(pModule, args);
	return Status;
}
//***************************************************************************************
int Cddc24Srv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
//	*(ULONG*)args = (m_AdmNum << 16) | m_QmTetrNum;
	*(ULONG*)args = m_ddc24TetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int Cddc24Srv::CtrlSetFc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetFc( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
//***************************************************************************************
int Cddc24Srv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc24TetrNum;
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
int Cddc24Srv::CtrlStart (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	ULONG start;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc24TetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_DDC24_START	 == cmd)
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
int Cddc24Srv::CtrlStartDdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	if(BRDctrl_DDC24_STARTDDC	 == cmd)
	{
		Status =StartDdc( pModule,args);
	}else
	{
		Status =StopDdc( pModule,args);
	}
	return BRDerr_OK;

}

//***************************************************************************************
int Cddc24Srv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc24TetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int Cddc24Srv::CtrlIsAvailable(
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
	
	if(m_MainTetrNum != -1 && m_ddc24TetrNum != -1)
	{
		m_isAvailable = 1;
		PDDC24SRV_CFG pSrvCfg = (PDDC24SRV_CFG)m_pConfig; 
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_ddc24TetrNum;
			{
				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
	//			pDacSrvCfg->FifoSize = RegParam.val << 3;
				pSrvCfg->FifoSize = RegParam.val * widthFifo;
			}
			RegParam.tetr = m_ddc24TetrNum;
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
// ***************** End of file ddc24Ctrl.cpp ***********************
