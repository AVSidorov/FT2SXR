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
#include "mrfddcSrv.h"

#define	CURRFILE "MRFDDCSRV"

//***************************************************************************************
//***************************************************************************************
int CmrfddcSrv::CtrlRelease(
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
void CmrfddcSrv::GetMrfTetrNum(CModule* pModule)
{
	m_MrfDdcTetrNum = GetTetrNum(pModule, m_index,  MRFDDC_TETR_ID  );
//	if(m_DdcMrfTetrNum==(-1))m_DdcMrfTetrNum = GetTetrNum(pModule, m_index,  MRFDDC_TETR_ID );

}
//***************************************************************************************
int CmrfddcSrv::AdmGetInfo(CModule* pModule, PBRD_AdmGetInfo pAdmInfo)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;

	param.reg = CONST_SUBADM_ID;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->admId = param.val & 0xffff;

	param.reg = CONST_SUBPLD_ID;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->pldId = param.val & 0xffff;

	param.reg = CONST_SUBPLD_MODE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->pldModification = param.val & 0xffff;

	param.reg = CONST_SUBPLD_VER;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->pldVersion = param.val & 0xffff;

	param.reg = CONST_SUBPLD_BUILD;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->pldBuild = param.val & 0xffff;

	param.reg = CONST_SUBTRD_ID;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->trdId = param.val & 0xffff;

	param.reg = CONST_SUBTRD_MOD;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->trdIdModification = param.val & 0xffff;

	param.reg = CONST_SUBTRD_VER;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->trdVersion = param.val & 0xffff;

	return BRDerr_OK;
}
//***************************************************************************************
int CmrfddcSrv::SetClkMode(CModule* pModule,PBRD_ClkMode pClkMode)
{
	PMRFDDCSRV_CFG pDdcSrvCfg= (PMRFDDCSRV_CFG)m_pConfig;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;
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
int CmrfddcSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;
	param.reg = ADM2IFnr_CHAN1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_CHAN1 pChan1 = (PADM2IF_CHAN1)&param.val;
	pChan1->ByBits.ChanSel = mask;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	return BRDerr_OK;
}
//***************************************************************************************
int CmrfddcSrv::SetStartMode(CModule* pModule, PVOID pStMode)
{
	PBRD_StartModeMRFDDC pStartMode = (PBRD_StartModeMRFDDC)pStMode;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;

	param.reg = ADM2IFnr_STMODE;
	PADM2IF_STMODE pStrMode = (PADM2IF_STMODE)&param.val;
	pStrMode->ByBits.SrcStart	= pStartMode->startSrc;
	pStrMode->ByBits.InvStart	= pStartMode->startInv;
	pStrMode->ByBits.SrcStop	= 0;
	pStrMode->ByBits.InvStop	= 0;
	pStrMode->ByBits.TrigStart	= 1;
	pStrMode->ByBits.Restart	= 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;

}
//=**************************************************************************************
int CmrfddcSrv::SetAdmMode(CModule* pModule,void *args)
{
	U32			*ptr = (U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;
	param.reg = MRFDDC_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.Source= *ptr&3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//=******************************************************************************************
int CmrfddcSrv::SetAdc(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	PBRD_SetAdc ptr=(PBRD_SetAdc)args;
	U32	gain;
	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;

	param.reg = MRFDDC_CONTROL1;

	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pControl1->ByBits.Dith= ptr->dith&1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = MRFDDC_GAIN;

	gain  =   ptr->gain[0] & 0x1;
	gain |=  (ptr->gain[1] & 0x1)<<1;
	gain |=  (ptr->gain[2] & 0x1)<<2;
	gain |=  (ptr->gain[3] & 0x1)<<3;
	param.val = gain & 0xf;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//=******************************************************************************************
int CmrfddcSrv::SetDdcInp(CModule* pModule, void *args)
{
	
	PBRD_SetDdcInp pinp = (PBRD_SetDdcInp)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;
	param.reg = MRFDDC_INP;

	param.val =   pinp->Inp0 & 0x3;
	param.val |=  (pinp->Inp1 & 0x3)<<2;
	param.val |=  (pinp->Inp2 & 0x3)<<4;
	param.val |=  (pinp->Inp3 & 0x3)<<6;

	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	return BRDerr_OK;
}
//***************************************************************************************
int CmrfddcSrv::SetDdcFc(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	PBRD_SetDdcFc pDdcSetFc=(PBRD_SetDdcFc)args;
	U32	Fc ;
	double dFc;
	U32 CodFc;
	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;
	param.reg = MRFDDC_NCO;

	Fc = pDdcSetFc->Fc3;
	dFc=0x100000000*Fc/m_SystemClock;
	CodFc=(U32)dFc;
	param.val = CodFc >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
	param.val = CodFc;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	Fc = pDdcSetFc->Fc2;
	dFc=0x100000000*Fc/m_SystemClock;
	CodFc=(U32)dFc;
	param.val = CodFc >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
	param.val = CodFc;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	Fc = pDdcSetFc->Fc1;
	dFc=0x100000000*Fc/m_SystemClock;
	CodFc=(U32)dFc;
	param.val = CodFc >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
	param.val = CodFc;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	Fc = pDdcSetFc->Fc0;
	dFc=0x100000000*Fc/m_SystemClock;
	CodFc=(U32)dFc;
	param.val = CodFc >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
	param.val = CodFc;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	param.reg = MRFDDC_WRNCO;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	return BRDerr_OK;
}
//***************************************************************************************
int CmrfddcSrv::SetDdcFlt(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	U32* coeff = (U32*)args;

	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;
	param.reg = MRFDDC_COE;

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
int CmrfddcSrv::SetDdcDecim(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	PBRD_SetDdcDecim ptr = (PBRD_SetDdcDecim)args;
	U32	dcic= (ptr->deccic) & 0x3ff;
	U32	dfir= ((ptr->decfir)-1) & 0x3f;
	U32 decim;
	param.idxMain = m_index;
	param.tetr = m_MrfDdcTetrNum;
	param.reg = MRFDDC_DECIM;
	decim = (dfir<<10) | dcic;
	param.val = decim;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	return BRDerr_OK;
}
//*************************************************************************************
//***************************************************************************************

// End Of File