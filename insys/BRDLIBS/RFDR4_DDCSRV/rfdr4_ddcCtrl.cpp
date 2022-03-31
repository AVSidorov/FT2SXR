/*
 ***************** File rfdr4_adcCtrl.cpp *******************************************
 *
 * CTRL-command for BRD Driver for rfdr4_adc service 
 *
 * (C) InSys by Sclyarov A. Aug. 2014
 *
 ********************************************************************************************
*/

//#include <windows.h>
//#include <winioctl.h>

#include "module.h"
#include "rfdr4_ddcSrv.h"
#define CURRFILE _BRDC("RFDR4_DDCCTRL")
CMD_Info SETCHANMASK_CMD	= { BRDctrl_RFDR4_DDC_SETCHANMASK, 0, 0, 0, NULL};
CMD_Info SETSTARTMODE_CMD	= { BRDctrl_RFDR4_DDC_SETSTARTMODE, 0, 0, 0, NULL};
CMD_Info SETFRAME_CMD		= { BRDctrl_RFDR4_DDC_SETFRAME, 0, 0, 0, NULL};
CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_RFDR4_DDC_GETSRCSTREAM, 1, 0, 0, NULL};
CMD_Info FIFORESET_CMD		= { BRDctrl_RFDR4_DDC_FIFORESET,	 0, 0, 0, NULL};
CMD_Info START_CMD			= { BRDctrl_RFDR4_DDC_START,		 0, 0, 0, NULL};
CMD_Info STOP_CMD			= { BRDctrl_RFDR4_DDC_STOP,		 0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_RFDR4_DDC_FIFOSTATUS,	 1, 0, 0, NULL};
CMD_Info STARTDDC_CMD		= { BRDctrl_RFDR4_DDC_STARTDDC,		 0, 0, 0, NULL};
CMD_Info STOPDDC_CMD		= { BRDctrl_RFDR4_DDC_STOPDDC,		 0, 0, 0, NULL};
CMD_Info SETDDCEXT_CMD		= { BRDctrl_RFDR4_DDC_SETDDCEXT, 0, 0, 0, NULL};
CMD_Info SETCORRECTEXT_CMD	= { BRDctrl_RFDR4_DDC_SETCORRECTEXT, 0, 0, 0, NULL};
CMD_Info SETSCAN_CMD		= { BRDctrl_RFDR4_DDC_SETSCAN, 0, 0, 0, NULL};
CMD_Info SELSCAN_CMD		= { BRDctrl_RFDR4_DDC_SELSCAN, 0, 0, 0, NULL};
CMD_Info ENBSCAN_CMD		= { BRDctrl_RFDR4_DDC_ENBSCAN, 0, 0, 0, NULL};
CMD_Info SETLABEL_CMD		= { BRDctrl_RFDR4_DDC_SETLABEL, 0, 0, 0, NULL};
CMD_Info INITPOWER_CMD		= { BRDctrl_RFDR4_DDC_INITPOWER, 0, 0, 0, NULL};
CMD_Info GETPOWER_CMD		= { BRDctrl_RFDR4_DDC_GETPOWER, 1, 0, 0, NULL};
CMD_Info SETDETECT_CMD		= { BRDctrl_RFDR4_DDC_SETDETECT, 0, 0, 0, NULL};
CMD_Info GETDETECT_CMD		= { BRDctrl_RFDR4_DDC_GETDETECT, 0, 0, 0, NULL};
CMD_Info SETLED_CMD			= { BRDctrl_RFDR4_DDC_SETLED, 0, 0, 0, NULL};

//***************************************************************************************
Crfdr4_ddcSrv::Crfdr4_ddcSrv(int idx, int srv_num, CModule* pModule, PRFDR4_DDCSRV_CFG pCfg) :
	CService(idx,  _BRDC("RFDR4_DDC"), srv_num, pModule, pCfg, sizeof(PRFDR4_DDCSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_SDRAMABLE |
			BRDserv_ATTR_CMPABLE |
//			BRDserv_ATTR_DSPABLE |
			BRDserv_ATTR_EXCLUSIVE_ONLY;
	

	Init(&SETCHANMASK_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlSetChanMask);
	Init(&SETSTARTMODE_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlStartMode);
	Init(&SETFRAME_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlSetFrame);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlGetAddrData);
	Init(&FIFORESET_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlFifoReset);
	Init(&START_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlStart);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlFifoStatus);
	Init(&STARTDDC_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlStartDdc);
	Init(&STOPDDC_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlStopDdc);
	Init(&STOPDDC_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlStopDdc);
	Init(&SETDDCEXT_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlSetDdcExt);
	Init(&SETCORRECTEXT_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlSetCorrectExt);
	Init(&SETSCAN_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlSetScan);
	Init(&SELSCAN_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlSelScan);
	Init(&ENBSCAN_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlEnbScan);
	Init(&SETLABEL_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlSetLabel);
	Init(&INITPOWER_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlInitPower);
	Init(&GETPOWER_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlGetPower);
	Init(&SETDETECT_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlSetDetect);
	Init(&GETDETECT_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlGetDetect);
	Init(&SETLED_CMD, (CmdEntry)&Crfdr4_ddcSrv::CtrlSetLed);

}
//***************************************************************************************
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlSetChanMask(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetChanMask(pModule, args);
	return Status;
}

//***************************************************************************************
int Crfdr4_ddcSrv::CtrlStartMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetStartMode(pModule, args);
	return Status;
}

//***************************************************************************************
int Crfdr4_ddcSrv::CtrlSetDdcExt(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetDdcExt(pModule, args);
	return Status;
}
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlSetScan(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetScan(pModule, args);
	return Status;
}
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlSelScan(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SelScan(pModule, args);
	return Status;
}
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlEnbScan(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = EnbScan(pModule, args);
	return Status;
}
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlSetLabel(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetLabel(pModule, args);
	return Status;
}

//***************************************************************************************
int Crfdr4_ddcSrv:: CtrlInitPower(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = InitPower( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************

//***************************************************************************************
int Crfdr4_ddcSrv::CtrlGetPower(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = GetPower(pModule, args);
	return Status;
}
//***************************************************************************************


int Crfdr4_ddcSrv::CtrlStartDdc(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = StartDdc(pModule, args);
	return Status;
}
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlStopDdc(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = StopDdc(pModule, args);
	return Status;
}
//***************************************************************************************

int Crfdr4_ddcSrv::CtrlSetFrame(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetFrame(pModule, args);
	return Status;
}
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlSetCorrectExt(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetCorrectExt(pModule, args);
	return Status;
}
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlSetDetect(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetDetect(pModule, args);
	return Status;
}
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlGetDetect(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = GetDetect(pModule, args);
	return Status;
}
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlSetLed(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetLed(pModule, args);
	return Status;
}
//***************************************************************************************
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
//	*(ULONG*)args = (m_AdmNum << 16) | m_QmTetrNum;
	*(ULONG*)args = m_Rfdr4_DdcTetrNum;
	return BRDerr_OK;
}
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;
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
int Crfdr4_ddcSrv::CtrlStart (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_RFDR4_DDC_START	 == cmd)
	{
		pMode0->ByBits.Start = 1;
//		pMode0->ByBits.Master=1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//---------------------
//		if(!pMode0->ByBits.Master)
		{ // Master/Slave
			param.tetr = m_MainTetrNum;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
			if(pMode0->ByBits.Master)
			{ // Master
				pMode0->ByBits.Start = 1;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			}else
			{
				pMode0->ByBits.Start = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

			}

		}
//------------------------
	}else

	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0->ByBits.Start = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//----------------------
		if(!pMode0->ByBits.Master)
		{ // Master/Slave
			param.tetr = m_MainTetrNum;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
			if(pMode0->ByBits.Master)
			{ // Master
				pMode0->ByBits.Start = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			}
		}
//----------------------------------
	}
	return BRDerr_OK;
}
//***************************************************************************************
//***************************************************************************************
int Crfdr4_ddcSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}


//***************************************************************************************
int Crfdr4_ddcSrv::CtrlRelease(
	void			*pDev,		// InfoSM or InfoBM
	void			*pServData,	// Specific Service Data
	ULONG			cmd,		// Command Code (from BRD_ctrl())
	void			*args 		// Command Arguments (from BRD_ctrl())
)
{

	PRFDR4_DDCSRV_CFG pSrvCfg = (PRFDR4_DDCSRV_CFG)m_pConfig;
	pSrvCfg->isAlreadyInit = 0;

	return BRDerr_CMD_UNSUPPORTED; // for free subservice
}

//***************************************************************************************
int Crfdr4_ddcSrv::CtrlIsAvailable(
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
	
	if(m_MainTetrNum != -1 && m_Rfdr4_DdcTetrNum != -1)
	{
		m_isAvailable = 1;


//		PRFDR4_DDCSRV_CFG pSrvCfg = (PRFDR4_DDCSRV_CFG)m_pConfig; 
//		if(!pSrvCfg->isAlreadyInit)
//		{
//			pSrvCfg->isAlreadyInit = 1;
//
//			DEVS_CMD_Reg RegParam;
//			RegParam.idxMain = m_index;
//			RegParam.tetr = m_Rfdr4_DdcTetrNum;
//			{
//				RegParam.reg = ADM2IFnr_FTYPE;
//				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
//				int widthFifo = RegParam.val >> 3;
//				RegParam.reg = ADM2IFnr_FSIZE;
//				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
//	//			pDacSrvCfg->FifoSize = RegParam.val << 3;
//				pSrvCfg->FifoSize = RegParam.val * widthFifo;
//			}
//			RegParam.tetr = m_Rfdr4_DdcTetrNum;
//			RegParam.reg = ADM2IFnr_MODE0;
//			RegParam.val = 0;
//		//	pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
//			PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&RegParam.val;
//			pMode0->ByBits.Reset = 1;
//			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
//			for(int i = 1; i < 32; i++)
////			for(int i = 1; i < 3; i++)
//			{
//				RegParam.reg = i;
//				RegParam.val = 0;
//				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
//
//			}
//			RegParam.reg = ADM2IFnr_MODE0;
//	//		pMode0->ByBits.AdmClk = 1;
//			pMode0->ByBits.Reset = 0;
//			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
//
////-------------------------------------------------------------------------
////----------------------------------------------------------------------------
//		}

	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}
//***************************************************************************************

int Crfdr4_ddcSrv::CtrlCapture(
	void		*pDev,		// InfoSM or InfoBM
	void		*pServData,	// Specific Service Data
	ULONG		cmd,		// Command Code (from BRD_ctrl())
	void		*args 		// Command Arguments (from BRD_ctrl())
)
{
	CModule* pModule = (CModule*)pDev;
	PRFDR4_DDCSRV_CFG pSrvCfg = (PRFDR4_DDCSRV_CFG)m_pConfig;
	if (!pSrvCfg->isAlreadyInit)
	{
		pSrvCfg->isAlreadyInit = 1;

		DEVS_CMD_Reg RegParam;
		RegParam.idxMain = m_index;
		RegParam.tetr = m_Rfdr4_DdcTetrNum;
		{
			RegParam.reg = ADM2IFnr_FTYPE;
			pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
			int widthFifo = RegParam.val >> 3;
			RegParam.reg = ADM2IFnr_FSIZE;
			pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
			//			pDacSrvCfg->FifoSize = RegParam.val << 3;
			pSrvCfg->FifoSize = RegParam.val * widthFifo;
		}
		RegParam.tetr = m_Rfdr4_DdcTetrNum;
		RegParam.reg = ADM2IFnr_MODE0;
		RegParam.val = 0;
		//	pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
		PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&RegParam.val;
		pMode0->ByBits.Reset = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
		for (int i = 1; i < 32; i++)
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

		//---------------------------------------------------------------------------
		//----------------------------------------------------------------------------
	}
	return BRDerr_OK;
}


// ***************** End of file rfdr4_ddcCtrl.cpp ***********************
