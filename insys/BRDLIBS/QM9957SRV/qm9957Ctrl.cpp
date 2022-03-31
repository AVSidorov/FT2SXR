/*
 ***************** File qm9957Ctrl.cpp *******************************
 *
 * CTRL-command for BRD Driver for QM9957 service 
 *
 * (C) InSys by Sclyarov A. Nov, 2010
 *
 **********************************************************************
*/

#include "gipcy.h"
#include "module.h"
#include "qm9957Srv.h"

#define	CURRFILE "QM9957CTRL"

CMD_Info SETMASTER_CMD		= { BRDctrl_QM_SETMASTER,		0, 0, 0, NULL};
CMD_Info GETMASTER_CMD		= { BRDctrl_QM_GETMASTER,		1, 0, 0, NULL};
CMD_Info SETCLKSOURCE_CMD	= { BRDctrl_QM_SETCLKSOURCE,	0, 0, 0, NULL};
CMD_Info GETCLKSOURCE_CMD	= { BRDctrl_QM_GETCLKSOURCE,	1, 0, 0, NULL};
CMD_Info SETCHANMASK_CMD	= { BRDctrl_QM_SETCHANMASK,		0, 0, 0, NULL};
CMD_Info GETCHANMASK_CMD	= { BRDctrl_QM_GETCHANMASK,		1, 0, 0, NULL};
CMD_Info SETSTARTMODE_CMD	= { BRDctrl_QM_SETSTARTMODE,	0, 0, 0, NULL};
CMD_Info GETSTARTMODE_CMD	= { BRDctrl_QM_GETSTARTMODE,	1, 0, 0, NULL};
CMD_Info FIFORESET_CMD		= { BRDctrl_QM_FIFORESET,		0, 0, 0, NULL};
CMD_Info START_CMD			= { BRDctrl_QM_START,			0, 0, 0, NULL};
CMD_Info STOP_CMD			= { BRDctrl_QM_STOP,			0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_QM_FIFOSTATUS,		1, 0, 0, NULL};
CMD_Info PUTDATA_CMD		= { BRDctrl_QM_PUTDATA,			0, 0, 0, NULL};
CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_QM_GETSRCSTREAM,	1, 0, 0, NULL};

CMD_Info QMSETREG_CMD		= { BRDctrl_QM_QMSETREG,		0, 0, 0, NULL};
CMD_Info QMGETREG_CMD		= { BRDctrl_QM_QMGETREG,		1, 0, 0, NULL};
CMD_Info QMMASTERRESET_CMD	= { BRDctrl_QM_QMMASTERRESET,	0, 0, 0, NULL};
CMD_Info QMRESETIO_CMD		= { BRDctrl_QM_QMRESETIO,		0, 0, 0, NULL};
CMD_Info SELECTPROFILE_CMD	= { BRDctrl_QM_SELECTPROFILE,	0, 0, 0, NULL};
CMD_Info QMUPDATEIO_CMD		= { BRDctrl_QM_QMUPDATEIO,		0, 0, 0, NULL};
CMD_Info SETPROFILE_CMD		= { BRDctrl_QM_SETPROFILE,		0, 0, 0, NULL};
CMD_Info SETMAINPARAM_CMD	= { BRDctrl_QM_SETMAINPARAM,	0, 0, 0, NULL};
CMD_Info SETSYNCMODE_CMD	= { BRDctrl_QM_SETSYNCMODE,		0, 0, 0, NULL};
CMD_Info SETCYCLEMODE_CMD	= { BRDctrl_QM_SETCYCLEMODE,	0, 0, 0, NULL};
CMD_Info GETCFG_CMD			= { BRDctrl_QM_GETCFG,			1, 0, 0, NULL};


//***************************************************************************************
CQm9957Srv::CQm9957Srv(int idx, int srv_num, CModule* pModule, PQM9957SRV_CFG pCfg) :
    CService(idx, _BRDC("QM9957"), srv_num, pModule, pCfg, sizeof(PQM9957SRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_SDRAMABLE |
			BRDserv_ATTR_CMPABLE |
//			BRDserv_ATTR_DSPABLE |
			BRDserv_ATTR_EXCLUSIVE_ONLY;
	

	Init(&SETMASTER_CMD, (CmdEntry)&CQm9957Srv::CtrlMaster);
	Init(&GETMASTER_CMD, (CmdEntry)&CQm9957Srv::CtrlMaster);

	Init(&SETCLKSOURCE_CMD, (CmdEntry)&CQm9957Srv::CtrlClkSource);
	Init(&GETCLKSOURCE_CMD, (CmdEntry)&CQm9957Srv::CtrlClkSource);
	Init(&SETCHANMASK_CMD, (CmdEntry)&CQm9957Srv::CtrlChanMask);
	Init(&GETCHANMASK_CMD, (CmdEntry)&CQm9957Srv::CtrlChanMask);
	Init(&SETSTARTMODE_CMD, (CmdEntry)&CQm9957Srv::CtrlStartMode);
	Init(&GETSTARTMODE_CMD, (CmdEntry)&CQm9957Srv::CtrlStartMode);
	Init(&FIFORESET_CMD, (CmdEntry)&CQm9957Srv::CtrlFifoReset);
	Init(&START_CMD, (CmdEntry)&CQm9957Srv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&CQm9957Srv::CtrlStart);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&CQm9957Srv::CtrlFifoStatus);
	Init(&PUTDATA_CMD, (CmdEntry)&CQm9957Srv::CtrlPutData);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&CQm9957Srv::CtrlGetAddrData);

	Init(&QMSETREG_CMD, (CmdEntry)&CQm9957Srv::CtrlQmSetReg);
	Init(&QMGETREG_CMD, (CmdEntry)&CQm9957Srv::CtrlQmGetReg);
	Init(&QMMASTERRESET_CMD, (CmdEntry)&CQm9957Srv::CtrlQmMasterReset);
	Init(&QMRESETIO_CMD, (CmdEntry)&CQm9957Srv::CtrlQmResetIO);
	Init(&SELECTPROFILE_CMD, (CmdEntry)&CQm9957Srv::CtrlSelectProfile);
	Init(&QMUPDATEIO_CMD, (CmdEntry)&CQm9957Srv::CtrlQmUpdateIO);
	Init(&SETPROFILE_CMD, (CmdEntry)&CQm9957Srv::CtrlSetProfile);
	Init(&SETMAINPARAM_CMD, (CmdEntry)&CQm9957Srv::CtrlSetMainParam);
	Init(&SETSYNCMODE_CMD, (CmdEntry)&CQm9957Srv::CtrlSetSyncMode);
	Init(&SETCYCLEMODE_CMD, (CmdEntry)&CQm9957Srv::CtrlSetCyclingMode);
	Init(&GETCFG_CMD, (CmdEntry)&CQm9957Srv::CtrlGetCfg);

}

//***************************************************************************************
int CQm9957Srv::CtrlMaster(
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
	if(BRDctrl_QM_SETMASTER == cmd)
	{
		param.tetr = m_QmTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0->ByBits.Master = *pMasterMode & 0x1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0->ByBits.Master = *pMasterMode >> 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		if(pMode0->ByBits.Master)
		{
			pMode0->ByBits.AdmClk=0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		    param.reg = ADM2IFnr_FMODE;
		    param.val =12 |0x8000;
//		    param.val =7 |0x8000;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		    param.reg = ADM2IFnr_FDIV;
		    param.val =1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}
	}
	else
	{
		param.tetr = m_QmTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		*pMasterMode = pMode0->ByBits.Master;
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		*pMasterMode |= (pMode0->ByBits.Master << 1);
	}
	return BRDerr_OK;
}

//***************************************************************************************


//***************************************************************************************
int CQm9957Srv::CtrlClkSource(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	U32 *pClkSrc = (U32*)args;
	U32  ClkSrc=*pClkSrc;
	if(BRDctrl_QM_SETCLKSOURCE == cmd)
	{
		Status = SetClkSource(pModule, ClkSrc);
	}
	else
	{
		Status = GetClkSource(pModule, ClkSrc);
	}
	return Status;
}

//***************************************************************************************
int CQm9957Srv::CtrlChanMask(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	if(BRDctrl_QM_SETCHANMASK == cmd)
	{
		ULONG mask = *(ULONG*)args;
		Status = SetChanMask(pModule, mask);
	}
	else
	{
		ULONG mask;
		Status = GetChanMask(pModule, mask);
		*(ULONG*)args = mask;
	}
	return Status;
}

//***************************************************************************************
int CQm9957Srv::CtrlStartMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
//	PBRD_DacStartMode pDacStartMode = (PBRD_DacStartMode)args;
	if(BRDctrl_QM_SETSTARTMODE == cmd)
		Status = SetStartMode(pModule, args);
	else
        Status = GetStartMode(pModule, args);
	return Status;
}

//***************************************************************************************

//***************************************************************************************
int CQm9957Srv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
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
int CQm9957Srv::CtrlStart (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
//	ULONG start;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_QM_START == cmd)
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
int CQm9957Srv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
//	ULONG data = pStatus->ByBits.Underflow;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CQm9957Srv::CtrlPutData(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;

	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);
/*
	PULONG buf = (PULONG)pBuf->pData;
	for(ULONG i = 0; i < pBuf->size / sizeof(ULONG); i++)
//	while(1)
	{
		param.val = buf[i];
//		param.val = buf[0];
		pModule->RegCtrl(DEVScmd_REGWRITEDIR, &param);
	}
*/	
	//param.reg = ADM2IFnr_STATUS;
	//pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//pStatus = (PADM2IF_STATUS)&param.val;

	return BRDerr_OK;
}

//***************************************************************************************
int CQm9957Srv::CtrlIsAvailable(
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
	GetDucTetrNum(pModule);

	if(m_MainTetrNum != -1 && m_QmTetrNum != -1)
	{
		m_isAvailable = 1;
		PQM9957SRV_CFG pDacSrvCfg = (PQM9957SRV_CFG)m_pConfig;
		if(!pDacSrvCfg->isAlreadyInit)
		{
			pDacSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_QmTetrNum;
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

			RegParam.tetr = m_QmTetrNum;
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
				InitAD9957();

	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
int CQm9957Srv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	*(ULONG*)args = m_QmTetrNum;
	return BRDerr_OK;
}


//***************************************************************************************
int CQm9957Srv::CtrlQmSetReg(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = WriteQmReg(pModule,args);

	return Status;
}

//***************************************************************************************
int CQm9957Srv::CtrlQmGetReg(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = ReadQmReg(pModule,args);

	return Status;
}
//***************************************************************************************
int CQm9957Srv::CtrlQmMasterReset(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = QmMasterReset(pModule);

	return Status;
}

//***************************************************************************************
//***************************************************************************************
int CQm9957Srv::CtrlQmResetIO(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = QmResetIO(pModule);
	return BRDerr_OK;
}
//***************************************************************************************
int CQm9957Srv::CtrlSelectProfile(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	U32 *ptr=(U32*)args;
	U32 numProfile=*ptr;
	CModule* pModule = (CModule*)pDev;
	Status =SelectProfile( pModule,numProfile);
	return BRDerr_OK;
}
//***************************************************************************************
int CQm9957Srv::CtrlQmUpdateIO(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	U32 *ptr=(U32*)args;
	U32 chipMask=*ptr;
	CModule* pModule = (CModule*)pDev;
	Status =QmUpdateIO( pModule,chipMask);
	return BRDerr_OK;
}
//***************************************************************************************
int CQm9957Srv::CtrlSetProfile(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetProfile( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************

int CQm9957Srv::CtrlSetMainParam(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetMainParam( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
int CQm9957Srv::CtrlSetSyncMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetSyncMode(pModule,args);

	return BRDerr_OK;
}
//***************************************************************************************
//***************************************************************************************
int CQm9957Srv::CtrlSetCyclingMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Cycle = *(ULONG*)args;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}
//***************************************************************************************
int CQm9957Srv::CtrlGetCfg(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	Status = GetCfg(args);

	return BRDerr_OK;
}

//***************************************************************************************
//***************************************************************************************
// ***************** End of file duc9957Ctrl.cpp ***********************
