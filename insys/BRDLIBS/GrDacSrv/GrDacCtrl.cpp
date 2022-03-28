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
#include "GrDacSrv.h"

#define	CURRFILE "GRDACCTRL"

CMD_Info FIFORESET_CMD		= { BRDctrl_GRDAC_FIFORESET,		0, 0, 0, NULL};
CMD_Info START_CMD			= { BRDctrl_GRDAC_START,			0, 0, 0, NULL};
CMD_Info STOP_CMD			= { BRDctrl_GRDAC_STOP,				0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_GRDAC_FIFOSTATUS,		1, 0, 0, NULL};
CMD_Info PUTDATA_CMD		= { BRDctrl_GRDAC_PUTDATA,			0, 0, 0, NULL};
CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_GRDAC_GETSRCSTREAM,		1, 0, 0, NULL};
CMD_Info SETMODE_CMD		= { BRDctrl_GRDAC_SETMODE,			0, 0, 0, NULL};
CMD_Info SETCYCLEMODE_CMD	= { BRDctrl_GRDAC_SETCYCLEMODE,		0, 0, 0, NULL};


//***************************************************************************************
CGrDacSrv::CGrDacSrv(int idx, int srv_num, CModule* pModule, PGRDACSRV_CFG pCfg) :
	CService(idx, _T("GRDAC"), srv_num, pModule, pCfg, sizeof(PGRDACSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_SDRAMABLE |
			BRDserv_ATTR_CMPABLE |
//			BRDserv_ATTR_DSPABLE |
			BRDserv_ATTR_EXCLUSIVE_ONLY;
	

	Init(&FIFORESET_CMD, (CmdEntry)&CGrDacSrv::CtrlFifoReset);
	Init(&START_CMD, (CmdEntry)&CGrDacSrv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&CGrDacSrv::CtrlStart);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&CGrDacSrv::CtrlFifoStatus);
	Init(&PUTDATA_CMD, (CmdEntry)&CGrDacSrv::CtrlPutData);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&CGrDacSrv::CtrlGetAddrData);
	Init(&SETMODE_CMD, (CmdEntry)&CGrDacSrv::CtrlSetMode);
	Init(&SETCYCLEMODE_CMD, (CmdEntry)&CGrDacSrv::CtrlSetCyclingMode);

}

//***************************************************************************************
//***************************************************************************************
int CGrDacSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GrDacTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	pMode0->ByBits.FifoRes = 0;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	return BRDerr_OK;
}

//***************************************************************************************
int CGrDacSrv::CtrlStart (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GrDacTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_GRDAC_START == cmd)
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
int CGrDacSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GrDacTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
//	ULONG data = pStatus->ByBits.Underflow;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CGrDacSrv::CtrlPutData(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GrDacTetrNum;

	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CGrDacSrv::CtrlIsAvailable(
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
	GetGrDacTetrNum(pModule);

	if(m_MainTetrNum != -1 && m_GrDacTetrNum != -1)
	{
		m_isAvailable = 1;
		PGRDACSRV_CFG pDacSrvCfg = (PGRDACSRV_CFG)m_pConfig;
		if(!pDacSrvCfg->isAlreadyInit)
		{
			pDacSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_GrDacTetrNum;
			if(!pDacSrvCfg->FifoSize)
			{

				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
	//			pDacSrvCfg->FifoSize = RegParam.val << 3;
				pDacSrvCfg->FifoSize = RegParam.val * widthFifo;
			}

			RegParam.tetr = m_GrDacTetrNum;
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
int CGrDacSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	*(ULONG*)args = m_GrDacTetrNum;
	return BRDerr_OK;
}


//***************************************************************************************
//***************************************************************************************
int CGrDacSrv::CtrlSetMode(
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
int CGrDacSrv::CtrlSetCyclingMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GrDacTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Cycle = *(ULONG*)args;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
//***************************************************************************************
// ***************** End of file GrDacCtrl.cpp ***********************
