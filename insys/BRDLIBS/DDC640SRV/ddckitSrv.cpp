/*
 ***************** File ddckitSrv.cpp ************
 *
 * BRD Driver for ddckit service 
 *
 * (C) InSys by Sklyarov A. Feb. 2015
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "ddckitSrv.h"

#define	CURRFILE "DDCKITSRV"

//***************************************************************************************
//***************************************************************************************
int CddckitSrv::CtrlRelease(
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

void CddckitSrv::GetDdcTetrNum(CModule* pModule)
{
	m_ddckitTetrNum = GetTetrNum(pModule, m_index,  DDCKIT_TETR_ID  );

}
//***************************************************************************************
//*************************************************************************************
int CddckitSrv::SetFc(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	PBRD_SetFc setFc = (PBRD_SetFc)args;
	U32 Fc= setFc ->Fc;
	double SampRate = setFc->SamplingRate;
	U32 CodFc;
	double dFc;
	double d_Fc;

	param.idxMain = m_index;
	param.tetr = m_ddckitTetrNum;

//	param.reg = DDC640_CONTROL1;
//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
//
//
//	pControl1->ByBits.nco_rst= 1;	// 
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//
//	pControl1->ByBits.nco_rst= 0;	// 
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//
////	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
////	pControl1->ByBits.nco_rst= 0;

//	for(int i=0; i<16; i++)
//	for(int i=0; i< 40; i++)
	{
		d_Fc = (double)Fc;
		dFc=0x100000000*d_Fc/SampRate;  
		CodFc=(U32)dFc;
		CodFc &=0xffffffff;

		param.reg = DDCKIT_NCO;
		param.val = CodFc >> 16;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
		param.val = CodFc;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		//param.reg = DDCKIT_NCO_NB_ADDR;
		//param.val = i;
		//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		//param.reg = DDCKIT_WRNCO_NB;
		//param.val = 0;
		//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	


	}

 
	return BRDerr_OK;

 
}
//***************************************************************************************

// End Of File