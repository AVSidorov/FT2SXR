/*
 ***************** File ddc8x32Ctrl.cpp *******************************************
 *
 * CTRL-command for BRD Driver for ddc8x32 service 
 *
 * (C) InSys by Sclyarov A. Apr. 2012
 *
 ********************************************************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "ddc8x32Srv.h"
#define CURRFILE _BRDC("DDC8X32CTRL")
CMD_Info SETCLOCK_CMD		= { BRDctrl_DDC8X32_SETCLOCK, 0, 0, 0, NULL};
CMD_Info SETMODE_CMD		= { BRDctrl_DDC8X32_SETMODE, 0, 0, 0, NULL};
CMD_Info SETFC_CMD			= { BRDctrl_DDC8X32_SETFC, 0, 0, 0, NULL};
CMD_Info SETFIR_CMD			= { BRDctrl_DDC8X32_SETFIR, 0, 0, 0, NULL};
CMD_Info SETDECIM_CMD		= { BRDctrl_DDC8X32_SETDECIM, 0, 0, 0, NULL};
CMD_Info SETCHAN_CMD		= { BRDctrl_DDC8X32_SETCHAN,	0, 0, 0, NULL};
CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_DDC8X32_GETSRCSTREAM, 1, 0, 0, NULL};
CMD_Info FIFORESET_CMD		= { BRDctrl_DDC8X32_FIFORESET,	 0, 0, 0, NULL};
CMD_Info START_CMD			= { BRDctrl_DDC8X32_START,		 0, 0, 0, NULL};
CMD_Info STOP_CMD			= { BRDctrl_DDC8X32_STOP,		 0, 0, 0, NULL};
CMD_Info STARTDDC_CMD		= { BRDctrl_DDC8X32_STARTDDC,		 0, 0, 0, NULL};
CMD_Info STOPDDC_CMD		= { BRDctrl_DDC8X32_STOPDDC,		 0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_DDC8X32_FIFOSTATUS,	 1, 0, 0, NULL};
CMD_Info SETDEBUG_CMD		= { BRDctrl_DDC8X32_SETDEBUG,	 0, 0, 0, NULL};
CMD_Info SETOVER_CMD		= { BRDctrl_DDC8X32_SETOVER,	 0, 0, 0, NULL};
CMD_Info GETOVER_CMD		= { BRDctrl_DDC8X32_GETOVER,	 1, 0, 0, NULL};
CMD_Info SETFRAME_CMD		= { BRDctrl_DDC8X32_SETFRAME,	 0, 0, 0, NULL};

//***************************************************************************************
Cddc8x32Srv::Cddc8x32Srv(int idx, int srv_num, CModule* pModule, PDDC8X32SRV_CFG pCfg) :
	CService(idx,  _BRDC("DDC8X32"), srv_num, pModule, pCfg, sizeof(PDDC8X32SRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_SDRAMABLE |
			BRDserv_ATTR_CMPABLE |
//			BRDserv_ATTR_DSPABLE |
			BRDserv_ATTR_EXCLUSIVE_ONLY;
	

	Init(&SETCLOCK_CMD, (CmdEntry)&Cddc8x32Srv::CtrlSetClock);
	Init(&SETMODE_CMD, (CmdEntry)&Cddc8x32Srv::CtrlSetMode);
	Init(&SETFC_CMD, (CmdEntry)&Cddc8x32Srv::CtrlSetFc);
	Init(&SETFIR_CMD, (CmdEntry)&Cddc8x32Srv::CtrlSetFir);
	Init(&SETDECIM_CMD, (CmdEntry)&Cddc8x32Srv::CtrlSetDecim);
	Init(&SETCHAN_CMD, (CmdEntry)&Cddc8x32Srv::CtrlSetChan);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&Cddc8x32Srv::CtrlGetAddrData);
	Init(&FIFORESET_CMD, (CmdEntry)&Cddc8x32Srv::CtrlFifoReset);
	Init(&START_CMD, (CmdEntry)&Cddc8x32Srv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&Cddc8x32Srv::CtrlStart);
	Init(&STARTDDC_CMD, (CmdEntry)&Cddc8x32Srv::CtrlStartDdc);
	Init(&STOPDDC_CMD, (CmdEntry)&Cddc8x32Srv::CtrlStartDdc);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&Cddc8x32Srv::CtrlFifoStatus);
	Init(&SETDEBUG_CMD, (CmdEntry)&Cddc8x32Srv::CtrlSetDebug);
	Init(&SETOVER_CMD, (CmdEntry)&Cddc8x32Srv::CtrlSetOver);
	Init(&GETOVER_CMD, (CmdEntry)&Cddc8x32Srv::CtrlGetOver);
	Init(&SETFRAME_CMD, (CmdEntry)&Cddc8x32Srv::CtrlSetFrame);
}
//***************************************************************************************
int Cddc8x32Srv::CtrlSetClock(
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
int Cddc8x32Srv::CtrlSetMode(
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
int Cddc8x32Srv::CtrlSetChan(
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
int Cddc8x32Srv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
//	*(ULONG*)args = (m_AdmNum << 16) | m_QmTetrNum;
	*(ULONG*)args = m_ddc8x32TetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int Cddc8x32Srv::CtrlSetFc(
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
int Cddc8x32Srv::CtrlSetFir(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetFir( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
int Cddc8x32Srv::CtrlSetDecim(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetDecim( pModule,args);
	return BRDerr_OK;
}

//***************************************************************************************
int Cddc8x32Srv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;
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
int Cddc8x32Srv::CtrlStart (
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
	param.tetr = m_ddc8x32TetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_DDC8X32_START	 == cmd)
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
int Cddc8x32Srv::CtrlStartDdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	if(BRDctrl_DDC8X32_STARTDDC	 == cmd)
	{
		Status =StartDdc( pModule,args);
	}else
	{
		Status =StopDdc( pModule,args);
	}
	return BRDerr_OK;

}

//***************************************************************************************
int Cddc8x32Srv::CtrlSetDebug(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetDebug( pModule,args);
	return BRDerr_OK;

}

//***************************************************************************************
int Cddc8x32Srv::CtrlSetOver(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetOver( pModule,args);
	return BRDerr_OK;

}
//***************************************************************************************
int Cddc8x32Srv::CtrlGetOver(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = GetOver( pModule,args);
	return BRDerr_OK;

}
//***************************************************************************************
int Cddc8x32Srv::CtrlSetFrame(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetFrame( pModule,args);
	return BRDerr_OK;

}
//***************************************************************************************

int Cddc8x32Srv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int Cddc8x32Srv::CtrlIsAvailable(
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
	
	if(m_MainTetrNum != -1 && m_ddc8x32TetrNum != -1)
	{
		m_isAvailable = 1;
		PDDC8X32SRV_CFG pSrvCfg = (PDDC8X32SRV_CFG)m_pConfig; 
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_ddc8x32TetrNum;
			{
				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
	//			pDacSrvCfg->FifoSize = RegParam.val << 3;
				pSrvCfg->FifoSize = RegParam.val * widthFifo;
			}
			RegParam.tetr = m_ddc8x32TetrNum;
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
			//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

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
// ***************** End of file ddc8x32Ctrl.cpp ***********************
