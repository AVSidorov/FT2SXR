/*
 ***************** File ddc640Srv.cpp ************
 *
 * BRD Driver for ddc640 service 
 *
 * (C) InSys by Sklyarov A. Oct. 2014
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "ddc640Srv.h"

#define	CURRFILE "DDC640SRV"

//***************************************************************************************
//***************************************************************************************
int Cddc640Srv::CtrlRelease(
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

void Cddc640Srv::GetDdcTetrNum(CModule* pModule)
{
	m_ddc640TetrNum = GetTetrNum(pModule, m_index,  DDC640_TETR_ID  );

}
//***************************************************************************************
int Cddc640Srv::SetMode(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	U32 mode= *(U32*)args;
	param.idxMain = m_index;
	param.tetr = m_ddc640TetrNum;
	param.reg = DDC640_CONTROL1;


	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;

	pControl1->ByBits.adc= mode&1;	// 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************
int Cddc640Srv::SetChans(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	PBRD_SetChans setCh = (PBRD_SetChans)args;

	param.idxMain = m_index;
	param.tetr = m_ddc640TetrNum;

	param.reg = DDC640_CONTROL1;

	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;

	pControl1->ByBits.stream=( setCh->NumStreams & 0x3)-1;	// 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);



	param.reg = DDC640_MASK;

	param.val= 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= setCh->Mask3 & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= setCh->Mask2 & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= setCh->Mask1 & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= setCh->Mask0 & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	return BRDerr_OK;
}
//***************************************************************************************
int Cddc640Srv::SetFcWb(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	PBRD_SetFcWb setFcWb = (PBRD_SetFcWb)args;
	U32 *pFc= setFcWb -> pFc;
	double SampRateAdc = setFcWb->SamplingRate;
	U32 CodFc, mask;
	double dFc;
	double d_Fc;

	param.idxMain = m_index;
	param.tetr = m_ddc640TetrNum;

	param.reg = DDC640_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;


	pControl1->ByBits.nco_rst= 1;	// 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	pControl1->ByBits.nco_rst= 0;	// 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	mask = 0x1;
	for(int i=0; i<16; i++)
//	for(int i=0; i<1; i++)
	{
//		d_Fc = (double)pFc[15-i];
		d_Fc = (double)pFc[i];
		dFc=0x100000000*d_Fc/SampRateAdc;  
		CodFc=(U32)dFc;
		CodFc &=0xffffffff;

		param.reg = DDC640_NCO;
		param.val = CodFc >> 16;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
		param.val = CodFc;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		param.reg = DDC640_NCO_MASK;
		param.val = mask;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		param.reg = DDC640_WRNCO_WB;
		param.val = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	


		mask = mask <<1;

	}

	param.reg = DDC640_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pControl1->ByBits.nco_rst= 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
 
	return BRDerr_OK;

 
}
//*************************************************************************************
int Cddc640Srv::SetFcNb(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	PBRD_SetFcNb setFcNb = (PBRD_SetFcNb)args;
	U32 *pFc= setFcNb -> pFc;
	double SampRate = setFcNb->SamplingRate;
	U32 CodFc;
	double dFc;
	double d_Fc;

	param.idxMain = m_index;
	param.tetr = m_ddc640TetrNum;

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
	for(int i=0; i< 40; i++)
	{
		d_Fc = (double)pFc[i];
		dFc=0x100000000*d_Fc/SampRate;  
		CodFc=(U32)dFc;
		CodFc &=0xffffffff;

		param.reg = DDC640_NCO;
		param.val = CodFc >> 16;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
		param.val = CodFc;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		param.reg = DDC640_NCO_NB_ADDR;
		param.val = i;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		param.reg = DDC640_WRNCO_NB;
		param.val = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	


	}

 
	return BRDerr_OK;

 
}
//***************************************************************************************
//***************************************************************************************
int Cddc640Srv::GetStat(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	U32 *stat= (U32*)args;
	param.idxMain = m_index;
	int	get=0;
	param.tetr = m_ddc640TetrNum;
	param.reg = DDC640_STATISTIC;
	param.val = 0xffff;

	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	for(int i= 0; i<1000; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if((param.val & 0x8000) == 0)
		{
			get = 1;
			break;
		}
	}
	if(get== 0)
	{
		*stat = 0xffff;
		return BRDerr_OK;
	}
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	*stat = param.val;
	return BRDerr_OK;
}
//***************************************************************************************

// End Of File