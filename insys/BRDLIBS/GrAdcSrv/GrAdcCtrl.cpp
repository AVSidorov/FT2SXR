/*
 ***************** File GrDacCtrl.cpp *******************************
 *
 * CTRL-command for BRD Driver for GRDAC service 
 *
 * (C) InSys by Sclyarov A. Feb, 2011
 *
 **********************************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "GrAdcSrv.h"

#define	CURRFILE "GRDACCTRL"

CMD_Info FIFORESET_CMD		= { BRDctrl_GRADC_FIFORESET,		0, 0, 0, NULL};
CMD_Info START_CMD			= { BRDctrl_GRADC_START,			0, 0, 0, NULL};
CMD_Info STOP_CMD			= { BRDctrl_GRADC_STOP,				0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_GRADC_FIFOSTATUS,		1, 0, 0, NULL};
CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_GRADC_GETSRCSTREAM,		1, 0, 0, NULL};
CMD_Info SETCHANMASK_CMD	= { BRDctrl_GRADC_SETCHANMASK,		0, 0, 0, NULL};
CMD_Info SETMODE_CMD		= { BRDctrl_GRADC_SETMODE,			0, 0, 0, NULL};
CMD_Info SETMODEDDC_CMD		= { BRDctrl_GRADC_SETMODEDDC,		0, 0, 0, NULL};
CMD_Info SETFLTDDC_CMD		= { BRDctrl_GRADC_SETFLTDDC,		0, 0, 0, NULL};


//***************************************************************************************
CGrAdcSrv::CGrAdcSrv(int idx, int srv_num, CModule* pModule, PGRADCSRV_CFG pCfg) :
	CService(idx, _T("GRADC"), srv_num, pModule, pCfg, sizeof(PGRADCSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_SDRAMABLE |
			BRDserv_ATTR_CMPABLE |
//			BRDserv_ATTR_DSPABLE |
			BRDserv_ATTR_EXCLUSIVE_ONLY;
	

	Init(&FIFORESET_CMD, (CmdEntry)&CGrAdcSrv::CtrlFifoReset);
	Init(&START_CMD, (CmdEntry)&CGrAdcSrv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&CGrAdcSrv::CtrlStart);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&CGrAdcSrv::CtrlFifoStatus);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&CGrAdcSrv::CtrlGetAddrData);
	Init(&SETCHANMASK_CMD, (CmdEntry)&CGrAdcSrv::CtrlSetChanMask);
	Init(&SETMODE_CMD, (CmdEntry)&CGrAdcSrv::CtrlSetMode);
	Init(&SETMODEDDC_CMD, (CmdEntry)&CGrAdcSrv::CtrlSetModeDdc);
	Init(&SETFLTDDC_CMD, (CmdEntry)&CGrAdcSrv::CtrlSetFilterDdc);

}

//***************************************************************************************
//***************************************************************************************
int CGrAdcSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GrAdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(2);
//	Sleep(10);
	pMode0->ByBits.FifoRes = 0;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	return BRDerr_OK;
}

//***************************************************************************************
int CGrAdcSrv::CtrlStart (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GrAdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_GRADC_START == cmd)
	{
		pMode0->ByBits.Start = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}else
	{
		pMode0->ByBits.Start = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CGrAdcSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GrAdcTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
//	ULONG data = pStatus->ByBits.Underflow;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************

//***************************************************************************************
int CGrAdcSrv::CtrlIsAvailable(
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
	GetGrAdcTetrNum(pModule);

	if(m_MainTetrNum != -1 && m_GrAdcTetrNum != -1)
	{
		m_isAvailable = 1;
		PGRADCSRV_CFG pAdcSrvCfg = (PGRADCSRV_CFG)m_pConfig;
		if(!pAdcSrvCfg->isAlreadyInit)
		{
			pAdcSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_GrAdcTetrNum;
			if(!pAdcSrvCfg->FifoSize)
			{

				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
	//			pDacSrvCfg->FifoSize = RegParam.val << 3;
				pAdcSrvCfg->FifoSize = RegParam.val * widthFifo;
			}

			RegParam.tetr = m_GrAdcTetrNum;
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
	//		pMode0->ByBits.AdmClk = 1;
			pMode0->ByBits.Reset = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

			RegParam.reg = ADM2IFnr_FDIV;
			RegParam.val = 2;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			RegParam.tetr = m_MainTetrNum;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
		}

	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
int CGrAdcSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	*(ULONG*)args = m_GrAdcTetrNum;
	return BRDerr_OK;
}


//***************************************************************************************
//***************************************************************************************
int CGrAdcSrv::CtrlSetChanMask(
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
//***************************************************************************************
int CGrAdcSrv::CtrlSetMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetMode( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
//***************************************************************************************
int CGrAdcSrv::CtrlSetModeDdc(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetModeDdc( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
//***************************************************************************************
int CGrAdcSrv::CtrlSetFilterDdc(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetFilterDdc( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************

//***************************************************************************************
//***************************************************************************************
// ***************** End of file GrAdcCtrl.cpp ******************************************
