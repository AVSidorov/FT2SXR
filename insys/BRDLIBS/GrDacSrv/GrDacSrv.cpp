/*
 ***************** File GrDacSrv.cpp ************
 *
 * BRD Driver for GRDAC service 
 *
 * (C) InSys by Sklyarov A. Feb 2011
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "GrDacSrv.h"

#define	CURRFILE "GRDACSRV"

//***************************************************************************************
//***************************************************************************************
int CGrDacSrv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CGrDacSrv::GetGrDacTetrNum(CModule* pModule)
{
	m_GrDacTetrNum = GetTetrNum(pModule, m_index, GRDAC_TETR_ID);
}
//***************************************************************************************

int CGrDacSrv::SetMode(CModule* pModule,void *args)
{
	PBRD_GrDacSetMode pSetMode= (PBRD_GrDacSetMode)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GrDacTetrNum;
	param.reg = ADM2IFnr_MODE1;
	PGRDAC_MODE1 pMode1 = (PGRDAC_MODE1)&param.val;
	pMode1->AsWhole=0;
	pMode1->ByBits.mute = (~pSetMode->EnableDac)&1;
	pMode1->ByBits.fmt = 3;// !!!
	pMode1->ByBits.mono = 0;
	pMode1->ByBits.chsl = 0;
	pMode1->ByBits.dem  = pSetMode->DeEmphasis;
	pMode1->ByBits.rate = pSetMode->SampleRate;
//	pMode1->ByBits.rate = 2;
	pMode1->ByBits.test = pSetMode->TestSig;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;

}
//*************************************************************************************
//***************************************************************************************

