/*
 ***************** File mrfddcCtrl.cpp *******************************************
 *
 * CTRL-command for BRD Driver for adm416x130mrf_ddc service on ADM416x130MRF_DDC submodule
 *
 * (C) InSys by Sclyarov A. Apr. 2012
 *
 ********************************************************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "mrfddcSrv.h"

#define	CURRFILE "MRFDDC"

CMD_Info ADMGETINFO_CMD			= { BRDctrl_MRFDDC_ADMGETINFO, 1, 0, 0, NULL};
CMD_Info SETCLKMODE_CMD		= { BRDctrl_MRFDDC_SETCLKMODE,		0, 0, 0, NULL};
CMD_Info SETCHANMASK_CMD	= { BRDctrl_MRFDDC_SETCHANMASK,	0, 0, 0, NULL};
CMD_Info SETSTARTMODE_CMD	= { BRDctrl_MRFDDC_SETSTARTMODE,	0, 0, 0, NULL};
CMD_Info SETADMMODE_CMD		= { BRDctrl_MRFDDC_SETADMMODE, 0, 0, 0, NULL};
CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_MRFDDC_GETSRCSTREAM, 1, 0, 0, NULL};
CMD_Info SETADC_CMD			= { BRDctrl_MRFDDC_SETADC, 0, 0, 0, NULL};
CMD_Info SETDDCINP_CMD		= { BRDctrl_MRFDDC_SETDDCINP, 0, 0, 0, NULL};
CMD_Info SETDDCFC_CMD		= { BRDctrl_MRFDDC_SETDDCFC, 0, 0, 0, NULL};
CMD_Info SETDDCFLT_CMD		= { BRDctrl_MRFDDC_SETDDCFLT, 0, 0, 0, NULL};
CMD_Info SETDDCDECIM_CMD	= { BRDctrl_MRFDDC_SETDDCDECIM, 0, 0, 0, NULL};
CMD_Info FIFORESET_CMD		= { BRDctrl_MRFDDC_FIFORESET,	 0, 0, 0, NULL};
CMD_Info START_CMD			= { BRDctrl_MRFDDC_START,		 0, 0, 0, NULL};
CMD_Info STOP_CMD			= { BRDctrl_MRFDDC_STOP,		 0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_MRFDDC_FIFOSTATUS,	 1, 0, 0, NULL};

//***************************************************************************************
CmrfddcSrv::CmrfddcSrv(int idx, int srv_num, CModule* pModule, PMRFDDCSRV_CFG pCfg) :
	CService(idx, _T("MRFDDC"), srv_num, pModule, pCfg, sizeof(PMRFDDCSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_SDRAMABLE |
			BRDserv_ATTR_CMPABLE |
//			BRDserv_ATTR_DSPABLE |
			BRDserv_ATTR_EXCLUSIVE_ONLY;
	

	Init(&ADMGETINFO_CMD, (CmdEntry)&CmrfddcSrv::CtrlAdmGetInfo);
	Init(&SETCLKMODE_CMD, (CmdEntry)&CmrfddcSrv::CtrlClkMode);
	Init(&SETCHANMASK_CMD, (CmdEntry)&CmrfddcSrv::CtrlChanMask);
	Init(&SETSTARTMODE_CMD, (CmdEntry)&CmrfddcSrv::CtrlStartMode);
	Init(&SETADMMODE_CMD, (CmdEntry)&CmrfddcSrv::CtrlSetAdmMode);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&CmrfddcSrv::CtrlGetAddrData);
	Init(&SETADC_CMD, (CmdEntry)&CmrfddcSrv::CtrlSetAdc);
	Init(&SETDDCINP_CMD, (CmdEntry)&CmrfddcSrv::CtrlSetDdcInp);
	Init(&SETDDCFC_CMD, (CmdEntry)&CmrfddcSrv::CtrlSetDdcFc);
	Init(&SETDDCFLT_CMD, (CmdEntry)&CmrfddcSrv::CtrlSetDdcFlt);
	Init(&SETDDCDECIM_CMD, (CmdEntry)&CmrfddcSrv::CtrlSetDdcDecim);
	Init(&FIFORESET_CMD, (CmdEntry)&CmrfddcSrv::CtrlFifoReset);
	Init(&START_CMD, (CmdEntry)&CmrfddcSrv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&CmrfddcSrv::CtrlStart);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&CmrfddcSrv::CtrlFifoStatus);
}

//***************************************************************************************
int CmrfddcSrv::CtrlAdmGetInfo(
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
//***************************************************************************************
int CmrfddcSrv::CtrlClkMode(
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
int CmrfddcSrv::CtrlChanMask(
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
int CmrfddcSrv::CtrlStartMode(
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
int CmrfddcSrv::CtrlSetAdmMode(
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
int CmrfddcSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
//	*(ULONG*)args = (m_AdmNum << 16) | m_QmTetrNum;
	*(ULONG*)args = m_MrfDdcTetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int CmrfddcSrv::CtrlSetAdc(
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
int CmrfddcSrv::CtrlSetDdcInp(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status =SetDdcInp( pModule,args);
	return BRDerr_OK;
}
//***************************************************************************************
int CmrfddcSrv::CtrlSetDdcFc(
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
int CmrfddcSrv::CtrlSetDdcFlt(
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
int CmrfddcSrv::CtrlSetDdcDecim(
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
int CmrfddcSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;
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
int CmrfddcSrv::CtrlStart (
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
	param.tetr = m_MrfDdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_MRFDDC_START	 == cmd)
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
int CmrfddcSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CmrfddcSrv::CtrlIsAvailable(
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
	
	if(m_MainTetrNum != -1 && m_MrfDdcTetrNum != -1)
	{
		m_isAvailable = 1;
		PMRFDDCSRV_CFG pSrvCfg = (PMRFDDCSRV_CFG)m_pConfig; 
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_MrfDdcTetrNum;
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
			RegParam.tetr = m_MrfDdcTetrNum;
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
			RegParam.tetr = m_MrfDdcTetrNum;
			RegParam.reg = MRFDDC_DELAY_CTRL;
			RegParam.val = 0x100;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

//	pBrd->RegPokeInd( trd, TRDIND_DELAY_CTRL, 0x100 );	

			RegParam.reg = 	ADM2IFnr_MODE2; // 0x0A
			RegParam.val = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
//	pBrd->RegPokeInd( trd, TRDIND_MODE2, 0 );   // SLEEP
			Sleep( 1 );
			RegParam.val = 2;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

//	pBrd->RegPokeInd( trd, TRDIND_MODE2, 2 );  // Рабочий режим
			Sleep( 1 );


	Sleep( 100 );

			RegParam.reg = MRFDDC_DELAY_CTRL;
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
		RegParam.tetr = m_MrfDdcTetrNum;
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
// ***************** End of file mrfddcCtrl.cpp ***********************
