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
#include "rfdr4_adcSrv.h"
#define CURRFILE _BRDC("RFDR4_ADCCTRL")
CMD_Info SETCLOCK_CMD		= { BRDctrl_RFDR4_ADC_SETCLOCK, 0, 0, 0, NULL};
CMD_Info SETCHANMASK_CMD	= { BRDctrl_RFDR4_ADC_SETCHANMASK, 0, 0, 0, NULL};
CMD_Info SETSTARTMODE_CMD		= { BRDctrl_RFDR4_ADC_SETSTARTMODE,	 0, 0, 0, NULL};
CMD_Info INITADC_CMD		= { BRDctrl_RFDR4_ADC_INITADC, 0, 0, 0, NULL};
CMD_Info SETBIAS_CMD		= { BRDctrl_RFDR4_ADC_SETBIAS, 0, 0, 0, NULL};
CMD_Info SETBUFCUR_CMD		= { BRDctrl_RFDR4_ADC_SETBUFCUR, 0, 0, 0, NULL};
CMD_Info SETGAIN_CMD		= { BRDctrl_RFDR4_ADC_SETGAIN, 0, 0, 0, NULL};
CMD_Info SETRF_CMD			= { BRDctrl_RFDR4_ADC_SETRF, 0, 0, 0, NULL};
CMD_Info GETSRCSTREAM_CMD	= { BRDctrl_RFDR4_ADC_GETSRCSTREAM, 1, 0, 0, NULL};
CMD_Info FIFORESET_CMD		= { BRDctrl_RFDR4_ADC_FIFORESET,	 0, 0, 0, NULL};
CMD_Info START_CMD			= { BRDctrl_RFDR4_ADC_START,		 0, 0, 0, NULL};
CMD_Info STOP_CMD			= { BRDctrl_RFDR4_ADC_STOP,		 0, 0, 0, NULL};
CMD_Info FIFOSTATUS_CMD		= { BRDctrl_RFDR4_ADC_FIFOSTATUS,	 1, 0, 0, NULL};
CMD_Info GETBITSOVERFLOW_CMD = { BRDctrl_RFDR4_ADC_GETBITSOVERFLOW, 1, 0, 0, NULL};
CMD_Info CLRBITSOVERFLOW_CMD = { BRDctrl_RFDR4_ADC_CLRBITSOVERFLOW, 1, 0, 0, NULL};
CMD_Info SETOUTDELAY_CMD	= { BRDctrl_RFDR4_ADC_SETOUTDELAY, 0, 0, 0, NULL };

//***************************************************************************************
Crfdr4_adcSrv::Crfdr4_adcSrv(int idx, int srv_num, CModule* pModule, PRFDR4_ADCSRV_CFG pCfg) :
	CService(idx,  _BRDC("RFDR4_ADC"), srv_num, pModule, pCfg, sizeof(PRFDR4_ADCSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_SDRAMABLE |
			BRDserv_ATTR_CMPABLE |
//			BRDserv_ATTR_DSPABLE |
			BRDserv_ATTR_EXCLUSIVE_ONLY;
	

	Init(&SETCLOCK_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlSetClock);
	Init(&SETCHANMASK_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlSetChanMask);
	Init(&SETSTARTMODE_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlSetStartMode);
	Init(&INITADC_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlInitAdc);
	Init(&SETBIAS_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlSetBias);
	Init(&SETBUFCUR_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlSetBufCur);
	Init(&SETGAIN_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlSetGain);
	Init(&SETRF_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlSetRf);
	Init(&GETSRCSTREAM_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlGetAddrData);
	Init(&FIFORESET_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlFifoReset);
	Init(&START_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlStart);
	Init(&FIFOSTATUS_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlFifoStatus);
	Init(&GETBITSOVERFLOW_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlGetBitsOverflow);
	Init(&CLRBITSOVERFLOW_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlClrBitsOverflow);
	Init(&SETOUTDELAY_CMD, (CmdEntry)&Crfdr4_adcSrv::CtrlSetOutDelay);

}
//***************************************************************************************
int Crfdr4_adcSrv::CtrlSetClock(
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
	return BRDerr_OK;
}

//***************************************************************************************
int Crfdr4_adcSrv::CtrlSetChanMask(
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
int Crfdr4_adcSrv::CtrlSetStartMode (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetStartMode(pModule,args);
	return Status;
}
//***************************************************************************************

int Crfdr4_adcSrv::CtrlInitAdc(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = InitAdc(pModule);
	return Status;
}
//***************************************************************************************
int Crfdr4_adcSrv::CtrlSetBias(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetBias(pModule,args);
	return Status;
}
//***************************************************************************************
int Crfdr4_adcSrv::CtrlSetBufCur(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetBufCur(pModule,args);
	return Status;
}
//***************************************************************************************
int Crfdr4_adcSrv::CtrlSetGain(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetGain(pModule,args);
	return Status;
}
//***************************************************************************************
int Crfdr4_adcSrv::CtrlSetRf(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetRf(pModule,args);
	return Status;
}

//***************************************************************************************
int Crfdr4_adcSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	*(ULONG*)args = m_Rfdr4_AdcTetrNum;
	return BRDerr_OK;
}

//***************************************************************************************
int Crfdr4_adcSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_AdcTetrNum;
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
int Crfdr4_adcSrv::CtrlStart (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_AdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(BRDctrl_RFDR4_ADC_START	 == cmd)			// START
	{
		pMode0->ByBits.Start = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//---------------------
	}else											// STOP

	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		pMode0->ByBits.Start = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	return BRDerr_OK;
}
//***************************************************************************************
int Crfdr4_adcSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_AdcTetrNum;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}
//***************************************************************************************
int Crfdr4_adcSrv::CtrlClrBitsOverflow(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = ClrBitsOverflow(pModule,args);
	return Status;
}
//***************************************************************************************
int Crfdr4_adcSrv::CtrlGetBitsOverflow(
	void		*pDev,		// InfoSM or InfoBM
	void		*pServData,	// Specific Service Data
	ULONG		cmd,		// Command Code (from BRD_ctrl())
	void		*args 		// Command Arguments (from BRD_ctrl())
)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = GetBitsOverflow(pModule, args);
	return Status;
}
//***************************************************************************************
int Crfdr4_adcSrv::CtrlSetOutDelay(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	Status = SetOutDelay(pModule,args);
	return Status;
}
//***************************************************************************************
//***************************************************************************************
int Crfdr4_adcSrv::CtrlRelease(
	void			*pDev,		// InfoSM or InfoBM
	void			*pServData,	// Specific Service Data
	ULONG			cmd,		// Command Code (from BRD_ctrl())
	void			*args 		// Command Arguments (from BRD_ctrl())
)
{

	PRFDR4_ADCSRV_CFG pSrvCfg = (PRFDR4_ADCSRV_CFG)m_pConfig;
	pSrvCfg->isAlreadyInit = 0;

	return BRDerr_CMD_UNSUPPORTED; // for free subservice
}
//-------------------------------------------------------------------------------------------
int Crfdr4_adcSrv::CtrlIsAvailable(
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
	GetAdcTetrNum(pModule);
	
	if(m_MainTetrNum != -1 && m_Rfdr4_AdcTetrNum != -1)
	{
		m_isAvailable = 1;
//		PRFDR4_ADCSRV_CFG pSrvCfg = (PRFDR4_ADCSRV_CFG)m_pConfig; 
//		if(!pSrvCfg->isAlreadyInit)
//		{
//			pSrvCfg->isAlreadyInit = 1;
//
//			DEVS_CMD_Reg RegParam;
//			RegParam.idxMain = m_index;
//			RegParam.tetr = m_Rfdr4_AdcTetrNum;
//			{
//				RegParam.reg = ADM2IFnr_FTYPE;
//				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
//				int widthFifo = RegParam.val >> 3;
//				RegParam.reg = ADM2IFnr_FSIZE;
//				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
//	//			pDacSrvCfg->FifoSize = RegParam.val << 3;
//				pSrvCfg->FifoSize = RegParam.val * widthFifo;
//			}
//			RegParam.tetr = m_Rfdr4_AdcTetrNum;
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
//----------------------------------------------------------------------------------
int Crfdr4_adcSrv::CtrlCapture(
		void		*pDev,		// InfoSM or InfoBM
		void		*pServData,	// Specific Service Data
		ULONG		cmd,		// Command Code (from BRD_ctrl())
		void		*args 		// Command Arguments (from BRD_ctrl())
)
{
		CModule* pModule = (CModule*)pDev;

		PRFDR4_ADCSRV_CFG pSrvCfg = (PRFDR4_ADCSRV_CFG)m_pConfig;

		if (!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_Rfdr4_AdcTetrNum;
			{
				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				//			pDacSrvCfg->FifoSize = RegParam.val << 3;
				pSrvCfg->FifoSize = RegParam.val * widthFifo;
			}
			RegParam.tetr = m_Rfdr4_AdcTetrNum;
			RegParam.reg = ADM2IFnr_MODE0;
			RegParam.val = 0;
			//	pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
			PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&RegParam.val;
			pMode0->ByBits.Reset = 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			for (int i = 1; i < 32; i++)
			{
				RegParam.reg = i;
				RegParam.val = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			}
			RegParam.reg = ADM2IFnr_MODE0;
			//		pMode0->ByBits.AdmClk = 1;
			pMode0->ByBits.Reset = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

		}
		return BRDerr_OK;
}
//***************************************************************************************
// ***************** End of file rfdr4_adcCtrl.cpp ***********************
