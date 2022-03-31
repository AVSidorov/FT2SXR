/*
 ***************** File SdramAmbvmeSrv.cpp ***********************
 *
 * BRD Driver for ADM-interface SdramAmbvme service
 *
 * (C) InSys by Dorokhin A. Jan 2005
 *
 ******************************************************
*/

#include "module.h"
#include "sdramambvmesrv.h"

#define	CURRFILE "SDRAMAMBVMESRV"

//***************************************************************************************
CSdramAmbvmeSrv::CSdramAmbvmeSrv(int idx, int srv_num, CModule* pModule, PSDRAMSRV_CFG pCfg) :
	CSdramSrv(idx, _BRDC("BASESDRAM"), srv_num, pModule, pCfg, sizeof(SDRAMSRV_CFG))
{
}

//***************************************************************************************
void CSdramAmbvmeSrv::GetSdramTetrNum(CModule* pModule)
{
	m_MemTetrNum = GetTetrNum(pModule, m_index, SDRAMAMBVME_TETR_ID);
}


// ***************** End of file SdramAmbvmeSrv.cpp ***********************
