/*
 ***************** File ddc416x130mrfSrv.cpp ************
 *
 * BRD Driver for ddc416x130mrf service on ADMD416x130MRF
 *
 * (C) InSys by Sklyarov A. Dec. 2011
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "ddc416x130mrfSrv.h"

#define	CURRFILE "DDC416x130MRFRV"

//***************************************************************************************
//***************************************************************************************
int CDdc416x130mrfSrv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	return BRDerr_CMD_UNSUPPORTED; 
}
//***************************************************************************************
void CDdc416x130mrfSrv::GetMrfTetrNum(CModule* pModule)
{
	m_DdcMrfTetrNum = GetTetrNum(pModule, m_index,  DDCMRF_TETR_ID  );
	if(m_DdcMrfTetrNum==(-1))m_DdcMrfTetrNum = GetTetrNum(pModule, m_index,  DDDMRF_TETR_ID );

}
//***************************************************************************************
int CDdc416x130mrfSrv::GetCfg(PBRD_DDCMRFCfg pCfg)
{
	PDDCMRFSRV_CFG pSrvCfg = (PDDCMRFSRV_CFG)m_pConfig;
	pCfg->SubType = pSrvCfg->SubType;
	pCfg->SubVer = pSrvCfg->SubVer;
	pCfg->AdmConst = m_AdmConst.AsWhole;
	return BRDerr_OK;
}
//***************************************************************************************
int CDdc416x130mrfSrv::SetClkMode(CModule* pModule,PBRD_ClkMode pClkMode)
{
	PDDCMRFSRV_CFG pDdcSrvCfg= (PDDCMRFSRV_CFG)m_pConfig;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;
	param.reg = ADM2IFnr_FSRC;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);

	if(pClkMode->src == 0)	// internal
	{ 
		param.val = 1;

		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		//	pClkMode->value = pDdcSrvCfg->SubIntClk;
		m_SystemClock = pClkMode->value;
		pDdcSrvCfg->SubExtClk=(U32)pClkMode->value;
		param.val = 0;
		//	pDdcSrvCfg->SubRefGen=pClkMode->value;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		return BRDerr_OK;
	}
	if(pClkMode->src == 1)	//external
	{ 
		param.val = 0x81;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		pDdcSrvCfg->SubExtClk=(U32)pClkMode->value;
		m_SystemClock=(U32)pClkMode->value;
		return BRDerr_OK;
	}
}
//=**************************************************************************************
int CDdc416x130mrfSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;
	param.reg = ADM2IFnr_CHAN1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_CHAN1 pChan1 = (PADM2IF_CHAN1)&param.val;
	pChan1->ByBits.ChanSel = mask;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************
int CDdc416x130mrfSrv::SetStartMode(CModule* pModule, PVOID pStMode)
{
	PBRD_StartModeDDCMRF pStartMode = (PBRD_StartModeDDCMRF)pStMode;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;
	param.reg = DDCMRF_STMODE;
	//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	param.val=0;
	PREG_STMODEM pStrMode	   = (PREG_STMODEM)&param.val;
	if( pStartMode->admMode==0 || pStartMode->EnbFrame==0)pStrMode->ByBits.SrcStart  = pStartMode->startSrc;
	else pStrMode->ByBits.SrcStart  = 0;
	pStrMode->ByBits.InvStart  = pStartMode->startInv;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	
	param.reg = DDCMRF_CONTROL1;
	PREG_CONTROL1 pControl1	   = (PREG_CONTROL1)&param.val;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	if(pStartMode->startSrc==0)pControl1->ByBits.StartTrig= 1;
//	else pControl1->ByBits.StartTrig= 0;
	pControl1->ByBits.ExtStartDdc = pStartMode->startSrc;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;


//	PBRD_StartMode pStartMode = (PBRD_StartMode)pStMode;
//	DEVS_CMD_Reg param;
//	param.idxMain = m_index;
//	param.tetr = m_DdcMrfTetrNum;
//	param.reg = DDCMRF_STMODE;
//	//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	param.val=0;
//	PREG_STMODEM pStrMode	   = (PREG_STMODEM)&param.val;
//	pStrMode->ByBits.SrcStart  = pStartMode->startSrc;
////	pStrMode->ByBits.SrcStart  = 0;
//	pStrMode->ByBits.InvStart  = pStartMode->startInv;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//
//	param.reg = DDCMRF_CONTROL1;
//	PREG_CONTROL1 pControl1	   = (PREG_CONTROL1)&param.val;
//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	if(pStartMode->startSrc==0)pControl1->ByBits.StartTrig= 1;
//	else pControl1->ByBits.StartTrig= 0;
//	pControl1->ByBits.ExtStartDdc = pStartMode->startSrc;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	return BRDerr_OK;
}
//=**************************************************************************************
int CDdc416x130mrfSrv::SetAdmMode(CModule* pModule,void *args)
{
	U32			*ptr = (U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;
	param.reg = DDCMRF_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	param.val |= (*ptr)&1;
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.Source= *ptr&3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//=******************************************************************************************
int CDdc416x130mrfSrv::SetAdc(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	PBRD_SetAdc pSetAdc=(PBRD_SetAdc)args;
	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;
	param.reg = DDCMRF_CONTROL1;

	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	param.val=0;
	pControl1->ByBits.Dith= pSetAdc->dith&1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = DDCMRF_GAIN;
	param.val = pSetAdc->gain;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDdc416x130mrfSrv::SetDdcFc(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	PBRD_SetDdcFc pDdcSetFc=(PBRD_SetDdcFc)args;
	U32	 chMask = pDdcSetFc -> mask;
	U32	 Fc = pDdcSetFc->Fc;

	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;

	double dFc=0x100000000*Fc/m_SystemClock;
	U32 CodFc=(U32)dFc;

	param.reg = DDCMRF_MASK;
	param.val= chMask;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = DDCMRF_NCO;
	param.val = CodFc >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
	param.val = CodFc;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	return BRDerr_OK;
}
//***************************************************************************************
int CDdc416x130mrfSrv::SetDdcFlt(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
//	U32* coeff = *args;
	U32* coeff = (U32*)args;

	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;
	param.reg = DDCMRF_COE;

	for( int i=0; i<256; i++ )
	{
		param.val = coeff[i] >> 16;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
		param.val = coeff[i] & 0xffff;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
	}

	return BRDerr_OK;
}
//*************************************************************************************
int CDdc416x130mrfSrv::SetDdcDecim(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
//	U32* coeff = *args;
	U32 decim = *(U32*)args;

	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;
	param.reg = DDCMRF_DECIM;

	param.val = decim;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	return BRDerr_OK;
}
//*************************************************************************************

int CDdc416x130mrfSrv::SetFrame(CModule* pModule, PBRD_SetFrame pSetFrame)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;


	param.reg = ADM2IFnr_STMODE;

	PREG_STMODEM pStmodem = (PREG_STMODEM)&param.val;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pStmodem->ByBits.EnbFrame = pSetFrame ->EnbFrame&1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = DDCMRF_CONTROL1;
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pControl1->ByBits.EnbHeader = pSetFrame ->EnbHeader&1;
	pControl1->ByBits.EnbTestCount = pSetFrame ->EnbTestCount&1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	param.val= pSetFrame ->Marker & 0xffff;
	param.reg = DDCMRF_HEAD1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= pSetFrame -> StartDelay & 0xffff;
	param.reg = DDCMRF_CNT0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= pSetFrame -> GetSamples & 0xffff;
	param.reg = DDCMRF_CNT1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= pSetFrame -> SkipSamples & 0xffff;
	param.reg = DDCMRF_CNT2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= pSetFrame -> NumStrings & 0xffff;
	param.reg = DDCMRF_CNT3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************
int CDdc416x130mrfSrv::StartDdc(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;

	param.reg = DDCMRF_CONTROL1;
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pControl1->ByBits.StartDdc = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK; 
}
//****************************************************************************************
int CDdc416x130mrfSrv::StopDdc(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;

	param.reg = DDCMRF_CONTROL1;
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pControl1->ByBits.StartDdc = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK; 
}
//***************************************************************************************
int CDdc416x130mrfSrv::SetMarker(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	U32 marker = *(U32*)args;

	param.idxMain = m_index;
	param.tetr = m_DdcMrfTetrNum;

	param.reg = DDCMRF_HEAD1;
	param.val = marker;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK; 
}
//***************************************************************************************
//***************************************************************************************

// End Of File