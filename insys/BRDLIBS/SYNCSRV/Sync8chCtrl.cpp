/*
 ***************** File Sync8chCtrl.cpp *******************************
 *
 * CTRL-command for BRD Driver for Sync8ch service
 *
 * (C) InSys by Dorokhin A. Apr 2009
 *
 **********************************************************************
*/
#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "Sync8chSrv.h"

#define	CURRFILE "SYNC8CHCTRL"

static CMD_Info SETMASTER_CMD		= { BRDctrl_SYNC8CH_SETMASTER,		0, 0, 0, NULL};
static CMD_Info GETMASTER_CMD		= { BRDctrl_SYNC8CH_GETMASTER,		1, 0, 0, NULL};
static CMD_Info SETCLKMODE_CMD		= { BRDctrl_SYNC8CH_SETCLKMODE,		0, 0, 0, NULL};
static CMD_Info GETCLKMODE_CMD		= { BRDctrl_SYNC8CH_GETCLKMODE,		1, 0, 0, NULL};
static CMD_Info SETOUTCLK_CMD		= { BRDctrl_SYNC8CH_SETOUTCLK,		0, 0, 0, NULL};
static CMD_Info GETOUTCLK_CMD		= { BRDctrl_SYNC8CH_GETOUTCLK,		1, 0, 0, NULL};
static CMD_Info SETSTBMODE_CMD		= { BRDctrl_SYNC8CH_SETSTBMODE,		0, 0, 0, NULL};
static CMD_Info GETSTBMODE_CMD		= { BRDctrl_SYNC8CH_GETSTBMODE,		1, 0, 0, NULL};
static CMD_Info SETOUTSTB_CMD		= { BRDctrl_SYNC8CH_SETOUTSTB,		0, 0, 0, NULL};
static CMD_Info GETOUTSTB_CMD		= { BRDctrl_SYNC8CH_GETOUTSTB,		1, 0, 0, NULL};
static CMD_Info SETSTARTMODE_CMD	= { BRDctrl_SYNC8CH_SETSTARTMODE,	0, 0, 0, NULL};
static CMD_Info GETSTARTMODE_CMD	= { BRDctrl_SYNC8CH_GETSTARTMODE,	1, 0, 0, NULL};
static CMD_Info STARTSTROBE_CMD		= { BRDctrl_SYNC8CH_STARTSTROBE,	0, 0, 0, NULL};
static CMD_Info STOPSTROBE_CMD		= { BRDctrl_SYNC8CH_STOPSTROBE,		0, 0, 0, NULL};
static CMD_Info STARTCLOCK_CMD		= { BRDctrl_SYNC8CH_STARTCLOCK,		0, 0, 0, NULL};
static CMD_Info STOPCLOCK_CMD		= { BRDctrl_SYNC8CH_STOPCLOCK,		0, 0, 0, NULL};
static CMD_Info SETPLLMODE_CMD		= { BRDctrl_SYNC8CH_SETPLLMODE,		0, 0, 0, NULL};

//***************************************************************************************
CSync8chSrv::CSync8chSrv(int idx, int srv_num, CModule* pModule, PSYNC8CHSRV_CFG pCfg) :
	CService(idx, _BRDC("SYNC8CH"), srv_num, pModule, pCfg, sizeof(SYNC8CHSRV_CFG))
{
	m_attribute = BRDserv_ATTR_EXCLUSIVE_ONLY;
	
	Init(&SETMASTER_CMD, (CmdEntry)&CSync8chSrv::CtrlMaster);
	Init(&GETMASTER_CMD, (CmdEntry)&CSync8chSrv::CtrlMaster);
	Init(&SETCLKMODE_CMD, (CmdEntry)&CSync8chSrv::CtrlClkMode);
	Init(&GETCLKMODE_CMD, (CmdEntry)&CSync8chSrv::CtrlClkMode);
	Init(&SETOUTCLK_CMD, (CmdEntry)&CSync8chSrv::CtrlOutClk);
	Init(&GETOUTCLK_CMD, (CmdEntry)&CSync8chSrv::CtrlOutClk);
	Init(&SETSTBMODE_CMD, (CmdEntry)&CSync8chSrv::CtrlStbMode);
	Init(&GETSTBMODE_CMD, (CmdEntry)&CSync8chSrv::CtrlStbMode);
	Init(&SETOUTSTB_CMD, (CmdEntry)&CSync8chSrv::CtrlOutStb);
	Init(&GETOUTSTB_CMD, (CmdEntry)&CSync8chSrv::CtrlOutStb);
	Init(&SETSTARTMODE_CMD, (CmdEntry)&CSync8chSrv::CtrlStartMode);
	Init(&GETSTARTMODE_CMD, (CmdEntry)&CSync8chSrv::CtrlStartMode);
	Init(&STARTSTROBE_CMD,(CmdEntry)&CSync8chSrv::CtrlStartStopStb);
	Init(&STOPSTROBE_CMD, (CmdEntry)&CSync8chSrv::CtrlStartStopStb);
	Init(&STARTCLOCK_CMD,(CmdEntry)&CSync8chSrv::CtrlStartStopClk);
	Init(&STOPCLOCK_CMD, (CmdEntry)&CSync8chSrv::CtrlStartStopClk);
	Init(&SETPLLMODE_CMD, (CmdEntry)&CSync8chSrv::CtrlPllMode);
}

//***************************************************************************************
int CSync8chSrv::CtrlRelease(
						void			*pDev,		// InfoSM or InfoBM
						void			*pServData,	// Specific Service Data
						ULONG			cmd,		// Command Code (from BRD_ctrl())
						void			*args 		// Command Arguments (from BRD_ctrl())
						)
{
	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
	pSrvCfg->isAlreadyInit = 0;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
int CSync8chSrv::CtrlIsAvailable(
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
	m_SyncTetrNum = GetTetrNum(pModule, m_index, SYNC8CH_TETR_ID);
	if(m_MainTetrNum != -1 && m_SyncTetrNum != -1)
	{
		m_isAvailable = 1;
		//PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
		//if(!pSrvCfg->isAlreadyInit)
		//{
		//	pSrvCfg->isAlreadyInit = 1;

		//	DEVS_CMD_Reg RegParam;
		//	RegParam.idxMain = m_index;
		//	RegParam.tetr = m_SyncTetrNum;
		//	RegParam.reg = ADM2IFnr_MODE0;
		//	RegParam.val = 0;
		//	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&RegParam.val;
		//	pMode0->ByBits.Reset = 1;
		//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
		//	for(int i = 1; i < 32; i++)
		//	{
		//		RegParam.reg = i;
		//		RegParam.val = 0;
		//		pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
		//	}
		//	RegParam.reg = ADM2IFnr_MODE0;
		//	pMode0->ByBits.Reset = 0;
		//	pMode0->ByBits.AdmClk = 1;
		//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

		//	for(int i = 1; i < 32; i++)
		//	{
		//		RegParam.reg = 0x200 + i;
		//		RegParam.val = 0;
		//		pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
		//	}
		//	InitPLL(pModule);
		//}
	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::CtrlCapture(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	if(m_isAvailable)
	{
		PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_SyncTetrNum;
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
			pMode0->ByBits.AdmClk = 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

			for(int i = 1; i < 32; i++)
			{
				RegParam.reg = 0x200 + i;
				RegParam.val = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			}
			InitPLL(pModule);
			pSrvCfg->dGen1Fxtal = 0.0;
			if(pSrvCfg->Gen1Type)
			{	
				RegParam.reg = SYNC8CHnr_FSRC;
				RegParam.val = 1;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
#if defined(__IPC_WIN__) || defined(__IPC_LINUX__)
				IPC_delay(300);
#else
				Sleep(300);
#endif
				writeSpdDev(pModule, GENERATOR1_DEVID, pSrvCfg->AdrGen1, 0, 135, 0x80); // Reset
				//SpdWrite( pModule, SPDdev_GEN, 135, 0x80 );		
#if defined(__IPC_WIN__) || defined(__IPC_LINUX__)
				IPC_delay(100);
#else
				Sleep(100);
#endif
				ULONG	regAdr, regData[20];

				// Считать регистры Si570/Si571
				for( regAdr=7; regAdr<=12; regAdr++ )
					readSpdDev(pModule, GENERATOR1_DEVID, pSrvCfg->AdrGen1, 0, regAdr, &regData[regAdr]);

				// Рассчитать частоту кварца
				SI571_CalcFxtal( &(pSrvCfg->dGen1Fxtal), (double)(pSrvCfg->RefGen1), (U32*)regData );
				BRDC_printf( _BRDC(">>> After reset Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n"), regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );
				BRDC_printf( _BRDC(">>> XTAL = %f kHz\n"), pSrvCfg->dGen1Fxtal/1000.0 );
				BRDC_printf( _BRDC(">>> GREF = %f kHz\n"), ((double)(pSrvCfg->RefGen1))/1000.0 );

				RegParam.val = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			}
		}
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::CtrlMaster(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
					   )
{
	CModule* pModule = (CModule*)pDev;
	ULONG* pMasterMode = (ULONG*)args;
	DEVS_CMD_Reg param;
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	param.idxMain = m_index;
	param.reg = ADM2IFnr_MODE0;
	param.tetr = m_SyncTetrNum;
	if(BRDctrl_SYNC8CH_SETMASTER == cmd)
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0->ByBits.Master = *pMasterMode & 0x1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0->ByBits.Master = *pMasterMode >> 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		*pMasterMode = pMode0->ByBits.Master;
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		*pMasterMode |= (pMode0->ByBits.Master << 1);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::CtrlClkMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_SyncClkMode pClkMode = (PBRD_SyncClkMode)args;
	if(BRDctrl_SYNC8CH_SETCLKMODE == cmd)
		Status = SetClkMode(pModule, pClkMode);
	else
		Status = GetClkMode(pModule, pClkMode);
	return Status;
}

//***************************************************************************************
int CSync8chSrv::CtrlOutClk(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_SyncOutClk pOutClk = (PBRD_SyncOutClk)args;
	if(BRDctrl_SYNC8CH_SETOUTCLK == cmd)
		Status = SetOutClk(pModule, pOutClk);
	else
		Status = GetOutClk(pModule, pOutClk);
	return Status;
}

//***************************************************************************************
int CSync8chSrv::CtrlStbMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_SyncStbMode pStbMode = (PBRD_SyncStbMode)args;
	if(BRDctrl_SYNC8CH_SETSTBMODE == cmd)
		Status = SetStbMode(pModule, pStbMode);
	else
		Status = GetStbMode(pModule, pStbMode);
	return Status;
}

//***************************************************************************************
int CSync8chSrv::CtrlOutStb(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_SyncOutStb pOutStb = (PBRD_SyncOutStb)args;
	if(BRDctrl_SYNC8CH_SETOUTSTB == cmd)
		Status = SetOutStb(pModule, pOutStb);
	else
		Status = GetOutStb(pModule, pOutStb);
	return Status;
}

//***************************************************************************************
int CSync8chSrv::CtrlStartStopStb(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	if(BRDctrl_SYNC8CH_STARTSTROBE == cmd)
		Status = StartStb(pModule);
	else
		Status = StopStb(pModule);
	return Status;
}

//***************************************************************************************
int CSync8chSrv::CtrlStartStopClk(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	if(BRDctrl_SYNC8CH_STARTCLOCK == cmd)
		Status = StartClk(pModule);
	else
		Status = StopClk(pModule);
	return Status;
}

//***************************************************************************************
//***************************************************************************************
int CSync8chSrv::CtrlStartMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_SyncStartMode pStMode = (PBRD_SyncStartMode)args;
	if(BRDctrl_SYNC8CH_SETSTARTMODE == cmd)
		Status = SetStartMode(pModule, pStMode);
	else
        Status = GetStartMode(pModule, pStMode);
	return Status;
}

//***************************************************************************************
//***************************************************************************************
int CSync8chSrv::CtrlPllMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_PllMode pPllMode = (PBRD_PllMode)args;
	Status = SetPllMode(pModule, pPllMode);
	return Status;
}

// ***************** End of file Sync8chCtrl.cpp ***********************