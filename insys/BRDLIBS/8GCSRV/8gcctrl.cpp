/*
 ***************** File s8gcCtrl.cpp ***************************************
 *
 * CTRL-command for BRD Driver for 8GC service on ADMDDC216x250M submodule
 *
 * (C) InSys by Sclyarov A. Apr 2012
 *
 ***************************************************************************
*/

#include "module.h"
#include "8gcsrv.h"

#define	CURRFILE _BRDC("8GCCTRL")

CMD_Info SETMASTER_CMD		= { BRDctrl_8GC_SETMASTER,		0, 0, 0, NULL};
CMD_Info GETMASTER_CMD		= { BRDctrl_8GC_GETMASTER,		1, 0, 0, NULL};
CMD_Info SETCHANMASK_CMD	= { BRDctrl_8GC_SETCHANMASK,	0, 0, 0, NULL};
CMD_Info GETCHANMASK_CMD	= { BRDctrl_8GC_GETCHANMASK,	1, 0, 0, NULL};
CMD_Info SETCLKMODE_CMD		= { BRDctrl_8GC_SETCLKMODE,		0, 0, 0, NULL};
CMD_Info GETCLKMODE_CMD		= { BRDctrl_8GC_GETCLKMODE,		1, 0, 0, NULL};
CMD_Info SETSTARTMODE_CMD	= { BRDctrl_8GC_SETSTARTMODE,	0, 0, 0, NULL};
CMD_Info GETSTARTMODE_CMD	= { BRDctrl_8GC_GETSTARTMODE,	1, 0, 0, NULL};
CMD_Info FIFORESET_CMD		= { BRDctrl_8GC_FIFORESET,	 0, 0, 0, NULL};
CMD_Info START_CMD			= { BRDctrl_8GC_START,		 0, 0, 0, NULL};
CMD_Info STOP_CMD			= { BRDctrl_8GC_STOP,		 0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_8GC_FIFOSTATUS,	 1, 0, 0, NULL};
CMD_Info GETDATA_CMD		= { BRDctrl_8GC_GETDATA,	 0, 0, 0, NULL};
CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_8GC_GETSRCSTREAM, 1, 0, 0, NULL};
CMD_Info SETTARGET_CMD		= { BRDctrl_8GC_SETTARGET,		0, 0, 0, NULL};
CMD_Info GETTARGET_CMD		= { BRDctrl_8GC_GETTARGET,		1, 0, 0, NULL};
CMD_Info SETTESTMODE_CMD	= { BRDctrl_8GC_SETTESTMODE,	0, 0, 0, NULL};
CMD_Info GETTESTMODE_CMD	= { BRDctrl_8GC_GETTESTMODE,	1, 0, 0, NULL};
CMD_Info GCSETREG_CMD		= { BRDctrl_8GC_GCSETREG, 0, 0, 0, NULL};
CMD_Info GCGETREG_CMD		= { BRDctrl_8GC_GCGETREG, 1, 0, 0, NULL};
CMD_Info GETADCOVER_CMD		= { BRDctrl_8GC_GETADCOVER, 0, 0, 0, NULL};
CMD_Info SETDDCFC_CMD		= { BRDctrl_8GC_SETDDCFC, 0, 0, 0, NULL};
CMD_Info SETADMMODE_CMD		= { BRDctrl_8GC_SETADMMODE, 0, 0, 0, NULL};
CMD_Info SETDDCMODE_CMD		= { BRDctrl_8GC_SETDDCMODE, 0, 0, 0, NULL};
CMD_Info SETDDCSYNC_CMD		= { BRDctrl_8GC_SETDDCSYNC, 0, 0, 0, NULL};
CMD_Info PROGRAMDDC_CMD		= { BRDctrl_8GC_PROGRAMDDC, 0, 0, 0, NULL};
CMD_Info PROGRAMADC_CMD		= { BRDctrl_8GC_PROGRAMADC, 0, 0, 0, NULL};
CMD_Info STARTDDC_CMD		= { BRDctrl_8GC_STARTDDC, 0, 0, 0, NULL};
CMD_Info STOPDDC_CMD		= { BRDctrl_8GC_STOPDDC, 0, 0, 0, NULL};
CMD_Info ADMGETINFO_CMD		= { BRDctrl_8GC_ADMGETINFO, 1, 0, 0, NULL};
CMD_Info SETDELAY_CMD		= { BRDctrl_8GC_SETDELAY, 0, 0, 0, NULL};
CMD_Info SETTESTSEQUENCE_CMD = { BRDctrl_8GC_SETTESTSEQUENCE, 0, 0, 0, NULL};

CMD_Info READINIFILE_CMD	= { BRDctrl_8GC_READINIFILE, 0, 0, 0, NULL};
CMD_Info SETADCLAG_CMD		= { BRDctrl_8GC_SETADCLAG, 0, 0, 0, NULL};

//***************************************************************************************
C8gcSrv::C8gcSrv(int idx, int srv_num, CModule* pModule, PGCSRV_CFG pCfg) :
	CService(idx, _BRDC("8GC"), srv_num, pModule, pCfg, sizeof(PGCSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_SDRAMABLE |
			BRDserv_ATTR_CMPABLE |
//			BRDserv_ATTR_DSPABLE |
			BRDserv_ATTR_EXCLUSIVE_ONLY;
	

	Init(&SETMASTER_CMD, (CmdEntry)&C8gcSrv::CtrlMaster);
	Init(&GETMASTER_CMD, (CmdEntry)&C8gcSrv::CtrlMaster);
	Init(&SETCLKMODE_CMD, (CmdEntry)&C8gcSrv::CtrlClkMode);
	Init(&GETCLKMODE_CMD, (CmdEntry)&C8gcSrv::CtrlClkMode);
	Init(&SETCHANMASK_CMD, (CmdEntry)&C8gcSrv::CtrlChanMask);
	Init(&GETCHANMASK_CMD, (CmdEntry)&C8gcSrv::CtrlChanMask);
	Init(&SETSTARTMODE_CMD, (CmdEntry)&C8gcSrv::CtrlStartMode);
	Init(&GETSTARTMODE_CMD, (CmdEntry)&C8gcSrv::CtrlStartMode);
	Init(&FIFORESET_CMD, (CmdEntry)&C8gcSrv::CtrlFifoReset);
	Init(&START_CMD, (CmdEntry)&C8gcSrv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&C8gcSrv::CtrlStart);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&C8gcSrv::CtrlFifoStatus);
	Init(&GETDATA_CMD, (CmdEntry)&C8gcSrv::CtrlGetData);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&C8gcSrv::CtrlGetAddrData);
	Init(&SETTARGET_CMD, (CmdEntry)&C8gcSrv::CtrlTarget);
	Init(&GETTARGET_CMD, (CmdEntry)&C8gcSrv::CtrlTarget);
	Init(&SETTESTMODE_CMD, (CmdEntry)&C8gcSrv::CtrlTestMode);
	Init(&GETTESTMODE_CMD, (CmdEntry)&C8gcSrv::CtrlTestMode);
	Init(&GCSETREG_CMD, (CmdEntry)&C8gcSrv::CtrlGcSetReg);
	Init(&GCGETREG_CMD, (CmdEntry)&C8gcSrv::CtrlGcGetReg);
	Init(&GETADCOVER_CMD, (CmdEntry)&C8gcSrv::CtrlGetAdcOver);
	Init(&SETDDCFC_CMD, (CmdEntry)&C8gcSrv::CtrlSetDdcFc);
	Init(&SETADMMODE_CMD, (CmdEntry)&C8gcSrv::CtrlSetAdmMode);
	Init(&SETDDCMODE_CMD, (CmdEntry)&C8gcSrv::CtrlSetDdcMode);
	Init(&SETDDCSYNC_CMD, (CmdEntry)&C8gcSrv::CtrlSetDdcSync);
	Init(&PROGRAMDDC_CMD, (CmdEntry)&C8gcSrv::CtrlProgramDdc);
	Init(&PROGRAMADC_CMD, (CmdEntry)&C8gcSrv::CtrlProgramAdc);
	Init(&STARTDDC_CMD, (CmdEntry)&C8gcSrv::CtrlStartDdc);
	Init(&STOPDDC_CMD, (CmdEntry)&C8gcSrv::CtrlStopDdc);
	Init(&ADMGETINFO_CMD, (CmdEntry)&C8gcSrv::CtrlAdmGetInfo);
	Init(&SETDELAY_CMD, (CmdEntry)&C8gcSrv::CtrlSetDelay);
	Init(&SETTESTSEQUENCE_CMD, (CmdEntry)&C8gcSrv::CtrlSetTestSequence);

	Init(&READINIFILE_CMD, (CmdEntry)&C8gcSrv::CtrlReadIniFile);
	Init(&SETADCLAG_CMD, (CmdEntry)&C8gcSrv::CtrlSetAdcLag);
	
}

//***************************************************************************************
int C8gcSrv::CtrlMaster(
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
	if(BRDctrl_8GC_SETMASTER == cmd)
	{
		param.tetr = m_GcTetrNum;
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
		param.tetr = m_GcTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		*pMasterMode = pMode0->ByBits.Master;
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		*pMasterMode |= (pMode0->ByBits.Master << 1);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::CtrlChanMask(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	if(BRDctrl_8GC_SETCHANMASK == cmd)
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
int C8gcSrv::CtrlClkMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_ClkMode pClkMode = (PBRD_ClkMode)args;
	if(BRDctrl_8GC_SETCLKMODE == cmd)
	{
		Status = SetClkMode(pModule, pClkMode);
	}
	else
	{
		
		Status = GetClkMode(pModule, pClkMode);
	}
	return Status;
}
//***************************************************************************************
int C8gcSrv::CtrlStartMode(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	if(BRDctrl_8GC_SETSTARTMODE == cmd)
		Status = SetStartMode(pModule, args);
	else
        Status = GetStartMode(pModule, args);
	return Status;
}

//***************************************************************************************

//***************************************************************************************
int C8gcSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    //IPC_delay(1);
	pMode0->ByBits.FifoRes = 0;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    //IPC_delay(1);
	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::CtrlStart (
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
	param.tetr = m_GcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_8GC_START == cmd)
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
int C8gcSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::CtrlGetData(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);

//	PULONG buf = (PULONG)pBuf->pData;
//	for(ULONG i = 0; i < pBuf->size / sizeof(ULONG); i++)
//	while(1)
	{
//		buf[i]=0x2000;
//			param.val = buf[i];
//		param.val = buf[0];
//		pModule->RegCtrl(DEVScmd_REGWRITEDIR, &param);
	}
	
	//param.reg = ADM2IFnr_STATUS;
	//pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//pStatus = (PADM2IF_STATUS)&param.val;

	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::CtrlIsAvailable(
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
	GetGcTetrNum(pModule);
	
	if(m_MainTetrNum != -1 && m_GcTetrNum != -1)
	{
		m_isAvailable = 1;
		PGCSRV_CFG pDacSrvCfg = (PGCSRV_CFG)m_pConfig;
		if(!pDacSrvCfg->isAlreadyInit)
		{
			pDacSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_GcTetrNum;
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

			RegParam.tetr = m_GcTetrNum;
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

//-------------------------------------------------------------------------
			RegParam.tetr = m_GcTetrNum;
			RegParam.reg = GCnr_DELAY_CTRL;
			RegParam.val = 0x100;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

			RegParam.reg = ADM2IFnr_MODE2;
			RegParam.val = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
            IPC_delay( 1 );
			RegParam.val = 2;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
            IPC_delay( 1 );


            IPC_delay( 100 );

			RegParam.reg = GCnr_DELAY_CTRL;
			RegParam.val = 0x14;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			RegParam.val = 0x10;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

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
		RegParam.tetr = m_GcTetrNum;
		RegParam.reg = GCnr_ADMCONST;
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
int C8gcSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
//	*(ULONG*)args = (m_AdmNum << 16) | m_QmTetrNum;
	*(ULONG*)args = m_GcTetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::CtrlTarget(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	ULONG target = *(ULONG*)args;
	if(BRDctrl_8GC_SETTARGET == cmd)
	{
		Status = SetTarget(pModule, target);
	}
	else
	{
		Status = GetTarget(pModule, target);
		*(ULONG*)args = target;
	}
	return Status;
}
//***************************************************************************************
int C8gcSrv::CtrlTestMode(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	ULONG mode = *(PULONG)args;
	if(BRDctrl_8GC_SETTESTMODE == cmd)
		Status = SetTestMode(pModule, mode);
	else
		Status = GetTestMode(pModule, mode);
	*(PULONG)args = mode;
	return Status;
}
//***************************************************************************************
int C8gcSrv::CtrlGcSetReg(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = WriteGcReg(pModule,args);

	return Status;
}

//***************************************************************************************
int C8gcSrv::CtrlGcGetReg(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = ReadGcReg(pModule,args);

	return Status;
}
//***************************************************************************************
int C8gcSrv::CtrlGetAdcOver(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = GetAdcOver(pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
int C8gcSrv::CtrlSetDdcFc(
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
//***************************************************************************************
int C8gcSrv::CtrlSetAdmMode(
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

int C8gcSrv::CtrlSetDdcMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetDdcMode( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
int C8gcSrv::CtrlSetDdcSync(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetDdcSync(pModule,args);

	return Status;
}
//***************************************************************************************
//***************************************************************************************
int C8gcSrv::CtrlProgramDdc(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = ProgramDdc(pModule,args);

	return Status;
}
//***************************************************************************************
//***************************************************************************************
int C8gcSrv::CtrlProgramAdc(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = ProgramAdc(pModule,args);

	return Status;
}
//***************************************************************************************
int C8gcSrv::CtrlStartDdc(
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
//***************************************************************************************
int C8gcSrv::CtrlStopDdc(
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
int C8gcSrv::CtrlAdmGetInfo(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_AdmGetInfo pAdmInfo = (PBRD_AdmGetInfo)args;
	Status = AdmGetInfo(pModule, pAdmInfo);
	return Status;
}
//***************************************************************************************
int C8gcSrv::CtrlSetDelay(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetDelay( pModule,args);
	return Status;
}
//***************************************************************************************
int C8gcSrv::CtrlSetTestSequence(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetTestSequence( pModule,args);
	return Status;
}	

int C8gcSrv::CtrlSetAdcLag(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{	
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetAdcLag( pModule,args);
	return Status;
}

int C8gcSrv::CtrlReadIniFile(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{	
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_IniFile pFile = (PBRD_IniFile)args;

	Status = SetProperties(pModule, pFile->fileName, pFile->sectionName);

	return Status;
}
// ***************** End of file 8gcCtrl.cpp ***********************
