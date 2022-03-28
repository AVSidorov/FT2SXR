/*
 ***************** File Dr16Srv.cpp ************
 *
 *
 * (C) InSys by Sklyarov A. Dec 2009
 *
 ******************************************************
*/

#include <sys/ioctl.h>

#include "module.h"
#include "dr16srv.h"
#define	CURRFILE "DR16SRV"

//***************************************************************************************
int CDr16Srv::SetClkMode(CModule* pModule, PBRD_ClkModeDr16 pClkMode)
{

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dr16TetrNum;
	param.reg =	DDSnr_CLK_CONTROL;
	PDR16_CLK_CONTROL pClkControl=(PDR16_CLK_CONTROL)&param.val;
	pClkControl->ByBits.ClkSource = pClkMode->ClkSource;
	pClkControl->ByBits.DivVco	= pClkMode->DivVco;
	pClkControl->ByBits.DivClkAdc=pClkMode->DivClkAdc;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
//***************************************************************************************
int CDr16Srv::SetStartMode(CModule* pModule, PBRD_StartModeDr16 pStMode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dr16TetrNum;
	param.reg = DDSnr_START_SYNC;
	PDR16_START_SYNC pStrMode = (PDR16_START_SYNC )&param.val;;
	pStrMode->ByBits.Sel_ST0  = pStMode->Sel_ST0;
	pStrMode->ByBits.Sel_ST1  = pStMode->Sel_ST1;
	pStrMode->ByBits.Sel_ST2  = pStMode->Sel_ST2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;

}
//=**************************************************************************************
int CDr16Srv::RestartDiv(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dr16TetrNum;
	param.reg = DDSnr_START_SYNC;;
	PDR16_START_SYNC pStrMode = (PDR16_START_SYNC )&param.val;;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pStrMode->ByBits.RestartDiv=1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pStrMode->ByBits.RestartDiv=0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;

}
//***************************************************************************************
int CDr16Srv::ResetJtag(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dr16TetrNum;
	param.reg = DDSnr_START_SYNC;;
	PDR16_START_SYNC pStrMode = (PDR16_START_SYNC )&param.val;;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	param.val &=~(0x2000);
	pStrMode->ByBits.ResetJtag=0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;

}
//=**************************************************************************************
int CDr16Srv::UnresetJtag(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dr16TetrNum;
	param.reg = DDSnr_START_SYNC;;
	PDR16_START_SYNC pStrMode = (PDR16_START_SYNC )&param.val;;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	param.val |=0x2000;
	pStrMode->ByBits.ResetJtag= 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;

}
//=**************************************************************************************
int CDr16Srv::SetTpMode(CModule* pModule, PBRD_TpModeDr16 pTpMode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Dr16TetrNum;
	param.reg = DDSnr_TP_CONTROL_L;
	PDR16_TP_CONTROL_L pTpControl_L = (PDR16_TP_CONTROL_L )&param.val;;
	pTpControl_L->ByBits.PeriodTpL  = 0xffff & (pTpMode->PeriodTp);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = DDSnr_TP_CONTROL_H;
	PDR16_TP_CONTROL_H pTpControl_H = (PDR16_TP_CONTROL_H )&param.val;;
	pTpControl_H->ByBits.PeriodTpH  = 0x3fff & ((pTpMode->PeriodTp)>>16);
	pTpControl_H->ByBits.InvExtTp  = pTpMode->InvExtTp;
	pTpControl_H->ByBits.EnableTp  = pTpMode->EnableTp;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;

}
//=**************************************************************************************
// ***************** End of file Dr16Srv.cpp ***********************
