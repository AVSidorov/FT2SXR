/*
 ***************** File Dr16Ctrl.cpp *******************************
 *
 *
 * (C) InSys by Sclyarov A. Dec 2009
 *
 **********************************************************************
*/
#include <sys/ioctl.h>

#include "module.h"
#include "dr16srv.h"

#define	CURRFILE "DR16CTRL"
static CMD_Info SETCLKMODE_CMD		= { BRDctrl_DR16_SETCLKMODE,		0, 0, 0, NULL};
static CMD_Info SETSTARTMODE_CMD	= { BRDctrl_DR16_SETSTARTMODE,	0, 0, 0, NULL};
static CMD_Info SETTPMODE_CMD	=	  { BRDctrl_DR16_SETTPMODE,	0, 0, 0, NULL};
static CMD_Info GETSTATUS_CMD		= { BRDctrl_DR16_GETSTATUS,		1, 0, 0, NULL};
static CMD_Info RESTARTDIV_CMD		= { BRDctrl_DR16_RESTARTDIV,	0, 0, 0, NULL};
static CMD_Info RESETJTAG_CMD		= { BRDctrl_DR16_RESETJTAG,	0, 0, 0, NULL};
static CMD_Info UNRESETJTAG_CMD		= { BRDctrl_DR16_UNRESETJTAG,	0, 0, 0, NULL};
static CMD_Info PUTDATA_CMD			= { BRDctrl_DR16_PUTDATA,	 0, 0, 0, NULL};
static CMD_Info FIFORESET_CMD		= { BRDctrl_DR16_FIFORESET,	 0, 0, 0, NULL};

//***************************************************************************************

CDr16Srv::CDr16Srv(int idx, int srv_num, CModule* pModule, PDR16SRV_CFG pCfg) :
	CService(idx, _BRDC("DR16"), srv_num, pModule, pCfg, sizeof(DR16SRV_CFG))
{
	m_attribute = BRDserv_ATTR_CMPABLE | BRDserv_ATTR_EXCLUSIVE_ONLY;
	
	Init(&SETCLKMODE_CMD,	(CmdEntry)&CDr16Srv::CtrlClkMode);
	Init(&SETSTARTMODE_CMD, (CmdEntry)&CDr16Srv::CtrlStartMode);
	Init(&SETTPMODE_CMD,    (CmdEntry)&CDr16Srv::CtrlTpMode);
	Init(&GETSTATUS_CMD,    (CmdEntry)&CDr16Srv::CtrlGetStatus);
	Init(&RESTARTDIV_CMD,    (CmdEntry)&CDr16Srv::CtrlRestartDiv);
	Init(&RESETJTAG_CMD,    (CmdEntry)&CDr16Srv::CtrlResetJtag);
	Init(&UNRESETJTAG_CMD,  (CmdEntry)&CDr16Srv::CtrlUnresetJtag);
	Init(&PUTDATA_CMD,		(CmdEntry)&CDr16Srv::CtrlPutData);
	Init(&FIFORESET_CMD,	(CmdEntry)&CDr16Srv::CtrlFifoReset);
}

//***************************************************************************************
int CDr16Srv::CtrlClkMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_ClkModeDr16 pClkMode = (PBRD_ClkModeDr16)args;
	Status = SetClkMode(pModule, pClkMode);
	return Status;
}

//***************************************************************************************
int CDr16Srv::CtrlStartMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_StartModeDr16 pStrMode = (PBRD_StartModeDr16)args;
	Status = SetStartMode(pModule, pStrMode);
	return Status;
}
//***************************************************************************************
int CDr16Srv::CtrlTpMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_TpModeDr16 pTpMode = (PBRD_TpModeDr16)args;
	Status = SetTpMode(pModule, pTpMode);
	return Status;
}
//***************************************************************************************

int CDr16Srv::CtrlRestartDiv(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = RestartDiv(pModule);
	return Status;
}
//***************************************************************************************
int CDr16Srv::CtrlResetJtag(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = ResetJtag(pModule);
	return Status;
}
//***************************************************************************************
int CDr16Srv::CtrlUnresetJtag(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = UnresetJtag(pModule);
	return Status;
}
//***************************************************************************************
int CDr16Srv::CtrlGetStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dr16TetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}
//***************************************************************************************
int CDr16Srv::CtrlPutData(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dr16TetrNum;

	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int  CDr16Srv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dr16TetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	IPC_delay(1);
	pMode0->ByBits.FifoRes = 0;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	IPC_delay(1);
	return BRDerr_OK;
}

//***************************************************************************************
//***************************************************************************************
int CDr16Srv::CtrlRelease(
						void			*pDev,		// InfoSM or InfoBM
						void			*pServData,	// Specific Service Data
						ULONG			cmd,		// Command Code (from BRD_ctrl())
						void			*args 		// Command Arguments (from BRD_ctrl())
						)
{
	PDR16SRV_CFG pDdsSrvCfg = (PDR16SRV_CFG)m_pConfig;
	pDdsSrvCfg->isAlreadyInit = 0;
	return BRDerr_CMD_UNSUPPORTED; // äëÿ îñâîáîæäåíèÿ ïîäñëóæá
}

//***************************************************************************************
int CDr16Srv::CtrlIsAvailable(
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
	m_Dr16TetrNum = GetTetrNum(pModule, m_index, DR16_CONTROL_TETR_ID);
	if(m_MainTetrNum != -1 && m_Dr16TetrNum != -1)
	{
		m_isAvailable = 1;
		PDR16SRV_CFG pDr16SrvCfg = (PDR16SRV_CFG)m_pConfig;
		if(!pDr16SrvCfg->isAlreadyInit)
		{
			pDr16SrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_Dr16TetrNum;
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

		}
	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

// ***************** End of file DdsCtrl.cpp ***********************