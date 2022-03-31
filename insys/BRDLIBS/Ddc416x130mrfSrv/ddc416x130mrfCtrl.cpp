/*
 ***************** File ddc416x130mrfCtrl.cpp ***************************************
 *
 * CTRL-command for BRD Driver for ddc416x130mrf service on ADM416x130MRF submodule
 *
 * (C) InSys by Sclyarov A. Dec 2011
 *
 ************************************************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "ddc416x130mrfSrv.h"

#define	CURRFILE "DDC416x130MRF"

CMD_Info GETCFG_CMD			= { BRDctrl_DDCMRF_GETCFG, 1, 0, 0, NULL};
CMD_Info SETCLKMODE_CMD		= { BRDctrl_DDCMRF_SETCLKMODE,		0, 0, 0, NULL};
CMD_Info SETCHANMASK_CMD	= { BRDctrl_DDCMRF_SETCHANMASK,	0, 0, 0, NULL};
CMD_Info SETSTARTMODE_CMD	= { BRDctrl_DDCMRF_SETSTARTMODE,	0, 0, 0, NULL};
CMD_Info SETADMMODE_CMD		= { BRDctrl_DDCMRF_SETADMMODE, 0, 0, 0, NULL};
CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_DDCMRF_GETSRCSTREAM, 1, 0, 0, NULL};
CMD_Info SETADC_CMD			= { BRDctrl_DDCMRF_SETADC, 0, 0, 0, NULL};
CMD_Info SETDDCFC_CMD		= { BRDctrl_DDCMRF_SETDDCFC, 0, 0, 0, NULL};
CMD_Info SETDDCFLT_CMD		= { BRDctrl_DDCMRF_SETDDCFLT, 0, 0, 0, NULL};
CMD_Info SETDDCDECIM_CMD	= { BRDctrl_DDCMRF_SETDDCDECIM, 0, 0, 0, NULL};
CMD_Info SETFRAME_CMD		= { BRDctrl_DDCMRF_SETFRAME, 0, 0, 0, NULL};
CMD_Info FIFORESET_CMD		= { BRDctrl_DDCMRF_FIFORESET,	 0, 0, 0, NULL};
CMD_Info START_CMD			= { BRDctrl_DDCMRF_START,		 0, 0, 0, NULL};
CMD_Info STOP_CMD			= { BRDctrl_DDCMRF_STOP,		 0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_DDCMRF_FIFOSTATUS,	 1, 0, 0, NULL};
CMD_Info STARTDDC_CMD		= { BRDctrl_DDCMRF_STARTDDC, 0, 0, 0, NULL};
CMD_Info STOPDDC_CMD		= { BRDctrl_DDCMRF_STOPDDC, 0, 0, 0, NULL};
CMD_Info SETMARKER_CMD		= { BRDctrl_DDCMRF_SETMARKER, 0, 0, 0, NULL};

//***************************************************************************************
CDdc416x130mrfSrv::CDdc416x130mrfSrv(int idx, int srv_num, CModule* pModule, PDDCMRFSRV_CFG pCfg) :
	CService(idx, _T("DDCMRF"), srv_num, pModule, pCfg, sizeof(PDDCMRFSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_SDRAMABLE |
			BRDserv_ATTR_CMPABLE |
//			BRDserv_ATTR_DSPABLE |
			BRDserv_ATTR_EXCLUSIVE_ONLY;
	

	Init(&GETCFG_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlGetCfg);
	Init(&SETCLKMODE_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlClkMode);
	Init(&SETCHANMASK_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlChanMask);
	Init(&SETSTARTMODE_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlStartMode);
	Init(&SETADMMODE_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlSetAdmMode);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlGetAddrData);
	Init(&SETADC_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlSetAdc);
	Init(&SETDDCFC_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlSetDdcFc);
	Init(&SETDDCFLT_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlSetDdcFlt);
	Init(&SETDDCDECIM_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlSetDdcDecim);
	Init(&SETFRAME_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlSetFrame);
	Init(&FIFORESET_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlFifoReset);
	Init(&START_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlStart);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlFifoStatus);
	Init(&STARTDDC_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlStartDdc);
	Init(&STOPDDC_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlStopDdc);
	Init(&SETMARKER_CMD, (CmdEntry)&CDdc416x130mrfSrv::CtrlSetMarker);
}

//***************************************************************************************
int CDdc416x130mrfSrv::CtrlGetCfg(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	PBRD_DDCMRFCfg pCfg = (PBRD_DDCMRFCfg)args;
	Status = GetCfg(pCfg);
	return Status;
}
//***************************************************************************************
//***************************************************************************************
int CDdc416x130mrfSrv::CtrlClkMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_ClkMode pClkMode = (PBRD_ClkMode)args;
	Status = SetClkMode(pModule, pClkMode);
	return Status;
}
//***************************************************************************************
int CDdc416x130mrfSrv::CtrlChanMask(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	ULONG mask = *(ULONG*)args;
	Status = SetChanMask(pModule, mask);
	return Status;
}
//***************************************************************************************
int CDdc416x130mrfSrv::CtrlStartMode(
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
int CDdc416x130mrfSrv::CtrlSetAdmMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetAdmMode( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
int CDdc416x130mrfSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
//	*(ULONG*)args = (m_AdmNum << 16) | m_QmTetrNum;
	*(ULONG*)args = m_DdcMrfTetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int CDdc416x130mrfSrv::CtrlSetAdc(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetAdc(pModule,args);
	return Status;
}
//***************************************************************************************
int CDdc416x130mrfSrv::CtrlSetDdcFc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetDdcFc( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
int CDdc416x130mrfSrv::CtrlSetDdcFlt(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetDdcFlt( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
int CDdc416x130mrfSrv::CtrlSetDdcDecim(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetDdcDecim( pModule,args);
	return BRDerr_OK;
}

//***************************************************************************************
int CDdc416x130mrfSrv::CtrlSetFrame(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_SetFrame pSetFrame = (PBRD_SetFrame)args;
		Status = SetFrame(pModule, pSetFrame);
	return Status;
}

//***************************************************************************************
int CDdc416x130mrfSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;
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
int CDdc416x130mrfSrv::CtrlStart (
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
	param.tetr = m_DdcMrfTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_DDCMRF_START	 == cmd)
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
int CDdc416x130mrfSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CDdc416x130mrfSrv::CtrlIsAvailable(
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
	GetMrfTetrNum(pModule);
	
	if(m_MainTetrNum != -1 && m_DdcMrfTetrNum != -1)
	{
		m_isAvailable = 1;
		PDDCMRFSRV_CFG pSrvCfg = (PDDCMRFSRV_CFG)m_pConfig; 
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_DdcMrfTetrNum;
			if(!pSrvCfg->FifoSize)
			{
				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
	//			pDacSrvCfg->FifoSize = RegParam.val << 3;
				pSrvCfg->FifoSize = RegParam.val * widthFifo;
			}
			RegParam.tetr = m_DdcMrfTetrNum;
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
			RegParam.tetr = m_DdcMrfTetrNum;
			RegParam.reg = DDCMRF_DELAY;
			RegParam.val = 0x100;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

//	pBrd->RegPokeInd( trd, TRDIND_DELAY_CTRL, 0x100 );	

			RegParam.reg = DDCMRF_MODE2;
			RegParam.val = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
//	pBrd->RegPokeInd( trd, TRDIND_MODE2, 0 );   // SLEEP
			Sleep( 1 );
			RegParam.val = 2;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

//	pBrd->RegPokeInd( trd, TRDIND_MODE2, 2 );  // Рабочий режим
			Sleep( 1 );


	Sleep( 100 );

			RegParam.reg = DDCMRF_DELAY;
			RegParam.val = 0x14;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			RegParam.val = 0x10;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

//	pBrd->RegPokeInd( trd, TRDIND_DELAY_CTRL, 0x14 );	 // 
//	pBrd->RegPokeInd( trd, TRDIND_DELAY_CTRL, 0x10 );	 // 
	

//	for( ii=6; ii>2; ii-- )
//	{
//		pBrd->RegPokeInd( trd, TRDIND_DELAY_CTRL, 0x12 );	 
//		pBrd->RegPokeInd( trd, TRDIND_DELAY_CTRL, 0x10 );	  
//		g_DelayCnt--;
//
//	}
	for(int i=6; i>2; i--)
	{

			RegParam.val = 0x12;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			RegParam.val = 0x10;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

	}

//----------------------------------------------------------------------------
		}
		DEVS_CMD_Reg RegParam;
		RegParam.idxMain = m_index;
		RegParam.tetr = m_DdcMrfTetrNum;
		RegParam.reg = DDCMRF_ADMCONST;
		pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
		PDDC_CONST adm_const = (PDDC_CONST)&RegParam.val;
		m_AdmConst.AsWhole = adm_const->AsWhole & 0xffff;

	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}
//***************************************************************************************
int CDdc416x130mrfSrv::CtrlStartDdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =StartDdc( pModule);
	return BRDerr_OK;
}
//***************************************************************************************
int CDdc416x130mrfSrv::CtrlStopDdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =StopDdc( pModule);
	return BRDerr_OK;
}
//***************************************************************************************
int CDdc416x130mrfSrv::CtrlSetMarker(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetMarker( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
// ***************** End of file ddc416x130mrfCtrl.cpp ***********************
