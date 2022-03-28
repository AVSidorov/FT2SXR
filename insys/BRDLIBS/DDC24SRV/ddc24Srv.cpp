/*
 ***************** File ddc24Srv.cpp ************
 *
 * BRD Driver for ddc1g8 service 
 *
 * (C) InSys by Sklyarov A. Oct. 2013
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "ddc24Srv.h"

#define	CURRFILE "DDC24SRV"

//***************************************************************************************
//***************************************************************************************
int Cddc24Srv::CtrlRelease(
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
void Cddc24Srv::GetDdcTetrNum(CModule* pModule)
{
	m_ddc24TetrNum = GetTetrNum(pModule, m_index,  DDC24_TETR_ID  );

}
//***************************************************************************************
int Cddc24Srv::SetClock(CModule* pModule, void *args)
{
   
	m_SystemClock	= *(U32*)args;

	return BRDerr_OK;
}
//***************************************************************************************
int Cddc24Srv::SetChan(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
//	U32* ptr=(U32*)args;
	U32 mask= *(U32*)args;
	param.idxMain = m_index;
	param.tetr = m_ddc24TetrNum;

	param.reg = DDC24_MASK;
	param.val= (mask >> 16)& 0xff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= mask & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************
int Cddc24Srv::SetFc(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	PBRD_SetFc setFc= (PBRD_SetFc)args;
	U32 *pFc= setFc->pFc;
	U32 ClockAdc= setFc->ClockAdc;
	U32 CodFc;
	U32 CodPh;
	U32 nco_mask;
	double dFc;
	double d_Fc;
	double d_ClockAdc= (double)ClockAdc;

	param.idxMain = m_index;
	param.tetr = m_ddc24TetrNum;

	param.reg = DDC24_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;


	pControl1->ByBits.nco_enb= 0;	// 
	pControl1->ByBits.nco_rst= 1;	// 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	pControl1->ByBits.nco_rst= 0;	// 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	nco_mask = 1;
	for(int i=0; i<24; i++)
	{
		d_Fc = (double)pFc[i];
		dFc=0x100000000*d_Fc/d_ClockAdc;  
		CodFc=(U32)dFc;
		CodFc &=0xffffffff;

		param.reg = DDC24_NCO;
		param.val = CodFc >> 16;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
		param.val = CodFc;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		param.reg = DDC24_NCO_MASK;
		param.val = (nco_mask >> 16) & 0xff;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		param.val = nco_mask & 0xffff;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	



		param.reg = DDC24_WRNCO;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		nco_mask = nco_mask <<1;
	}

	param.reg = DDC24_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pControl1->ByBits.nco_enb= 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
 
	return BRDerr_OK;

 
}
//*************************************************************************************
//***************************************************************************************
int Cddc24Srv::StartDdc(CModule* pModule, void *args)
{
	U32	*ptr = (U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc24TetrNum;
	param.reg = DDC24_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.startDdc= 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	return BRDerr_OK;
}
//***************************************************************************************
int Cddc24Srv::StopDdc(CModule* pModule, void *args)
{
	U32	*ptr = (U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc24TetrNum;
	param.reg = DDC24_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.startDdc= 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	return BRDerr_OK;
}
//***************************************************************************************
// End Of File