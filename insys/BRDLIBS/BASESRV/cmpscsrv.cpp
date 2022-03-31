/*
 ***************** File CmpScSrv.cpp ***********************
 *
 * BRD Driver for CmpSC (Comparators of Start/Clock) service
 *
 * (C) InSys by Dorokhin A. Feb 2004
 *
 ******************************************************
*/

#include <math.h>

#include "module.h"
#include "cmpscsrv.h"

#define	CURRFILE "CMPSCSRV"

static CMD_Info CMPSC_SET_CMD		= { BRDctrl_CMPSC_SET,			0, 0, 0, NULL};
static CMD_Info CMPSC_GET_CMD		= { BRDctrl_CMPSC_GET,			1, 0, 0, NULL};
static CMD_Info CMPSC_SETEXT_CMD	= { BRDctrl_CMPSC_SETEXT,		0, 0, 0, NULL};
static CMD_Info CMPSC_GETEXT_CMD	= { BRDctrl_CMPSC_GETEXT,		1, 0, 0, NULL};
static CMD_Info CMPSC_WRDACREG_CMD	= { BRDctrl_CMPSC_WRDACREG,		0, 0, 0, NULL};
static CMD_Info CMPSC_GETCFG_CMD	= { BRDctrl_CMPSC_GETCFG,		1, 0, 0, NULL};

static CMD_Info GETDLGPAGES_CMD	= { BRDctrl_CMPSC_GETDLGPAGES,	0, 0, 0, NULL};

//***************************************************************************************
CCmpScSrv::CCmpScSrv(int idx, int srv_num, CModule* pModule, PCMPSCSRV_CFG pCfg) :
	CService(idx, _BRDC("CMPSC"), srv_num, pModule, pCfg, sizeof(CMPSCSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_UNVISIBLE |
			BRDserv_ATTR_SUBSERVICE_ONLY |
			BRDserv_ATTR_EXCLUSIVE_ONLY;

//	m_MainTetrNum = GetTetrNum(pModule, MAIN_TETR_ID);
//	m_TetradNum = GetTetrNum(pModule, PIOSTD_TETR_ID);

	//CService::Init(&CMPSC_SET_CMD, (CmdEntry)&CCmpScSrv::CtrlSet);
	//m_Cmd.push_back(&CMPSC_SET_CMD);
	//CService::Init(&CMPSC_GET_CMD, (CmdEntry)&CCmpScSrv::CtrlGet);
	//m_Cmd.push_back(&CMPSC_GET_CMD);

	Init(&CMPSC_SET_CMD, (CmdEntry)&CCmpScSrv::CtrlSet);
	Init(&CMPSC_GET_CMD, (CmdEntry)&CCmpScSrv::CtrlGet);
	Init(&CMPSC_SETEXT_CMD, (CmdEntry)&CCmpScSrv::CtrlSetExt);
	Init(&CMPSC_GETEXT_CMD, (CmdEntry)&CCmpScSrv::CtrlGetExt);
	Init(&CMPSC_WRDACREG_CMD, (CmdEntry)&CCmpScSrv::CtrlDacReg);
	Init(&CMPSC_GETCFG_CMD, (CmdEntry)&CCmpScSrv::CtrlCfg);

	Init(&GETDLGPAGES_CMD, (CmdEntry)&CCmpScSrv::CtrlGetPages);
}
/*
// ***************************************************************************************
ULONG CCmpScSrv::IsAvailable(PVOID pModule)
{
	m_MainTetrNum = GetTetrNum(pModule, MAIN_TETR_ID);
	if(m_MainTetrNum != -1)
		m_isAvailable = 1;
	else
		m_isAvailable = 0;
	return m_isAvailable;
}
*/
//***************************************************************************************
int CCmpScSrv::CtrlIsAvailable(
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
	if(m_MainTetrNum != -1)
		m_isAvailable = 1;
	else
		m_isAvailable = 0;
	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
int CCmpScSrv::CtrlGetPages(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
#ifdef _WIN32
	CModule* pModule = (CModule*)pDev;
	PBRD_PropertyList pList = (PBRD_PropertyList)args;
//	ULONG dlg_mode = *(PULONG)args;

	if(cmd == BRDctrl_CMPSC_GETDLGPAGES)
	{
		// Open Library
		PCMPSCSRV_CFG pSrvCfg = (PCMPSCSRV_CFG)m_pConfig;
		pList->hLib = LoadLibrary(pSrvCfg->DlgDllName);
		if(pList->hLib <= (HINSTANCE)HINSTANCE_ERROR)
			return BRDerr_BAD_DEVICE_VITAL_DATA;

		INFO_InitPages_Type* pDlgFunc; 
		pDlgFunc = (INFO_InitPages_Type*)GetProcAddress(pList->hLib, "INFO_InitPages");
		if(!pDlgFunc)
		{
			FreeLibrary(pList->hLib);
			return BRDerr_BAD_DEVICE_VITAL_DATA;
		}
		PVOID pInfo = GetInfoForDialog(pModule);

		ULONG num_dlg;
//		int num_pages = (pDlgFunc)(pDev, pSrvCfg->pPropDlg, this, &num_dlg, pInfo);
		int num_pages = (pDlgFunc)(pDev, m_pPropDlg, this, &num_dlg, pInfo);
		pList->PagesCnt = num_pages;
		pList->NumDlg = num_dlg;

		FreeInfoForDialog(pInfo);
	}
#else
	fprintf(stderr, "%s, %d: - %s not implemented\n", __FILE__, __LINE__, __FUNCTION__);
	return -ENOSYS;
#endif
	return BRDerr_OK;
}

//***************************************************************************************
void CCmpScSrv::FreeInfoForDialog(PVOID pInfo)
{
	PCMPSCSRV_INFO pSrvInfo = (PCMPSCSRV_INFO)pInfo;
	delete pSrvInfo;
}

//***************************************************************************************
PVOID CCmpScSrv::GetInfoForDialog(CModule* pDev)
{
	PCMPSCSRV_CFG pSrvCfg = (PCMPSCSRV_CFG)m_pConfig;
	PCMPSCSRV_INFO pSrvInfo = new CMPSCSRV_INFO;
	pSrvInfo->Size = sizeof(CMPSCSRV_INFO);

	pSrvInfo->RefPVS = pSrvCfg->RefPVS;
	pSrvInfo->Threshold[0] = pSrvCfg->Threshold[0];
	pSrvInfo->Threshold[1] = pSrvCfg->Threshold[1];
	pSrvInfo->ComparBits = pSrvCfg->ComparBits;

	BRD_CmpSC cmp_data;
	CtrlGet(pDev, NULL, BRDctrl_CMPSC_GET, &cmp_data);
	pSrvInfo->Source = cmp_data.src;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	PMAIN_MRES pMres;
	param.reg = MAINnr_MRES;
	pDev->RegCtrl(DEVScmd_REGREADIND, &param);
	pMres = (PMAIN_MRES)&param.val;
	if(pMres->ByBits.MuxType && pMres->ByBits.ThdacType)
	{
		BRD_CmpNum cmp_num;
		for(int i = 0; i < BRD_CMPNUM; i++)
		{
			cmp_num.compar = i;
			CtrlGetExt(pDev, NULL, BRDctrl_CMPSC_GETEXT, &cmp_num);
			pSrvInfo->SourceExt[i] = cmp_num.src;
		}
	}
	else
	{
		pSrvInfo->SourceExt[0] = pSrvInfo->SourceExt[1] = 0xFF;
	}

	BRDCHAR module_name[40];
//	sprintf(module_name, " (%s)", pDev->m_name);
	BRDC_sprintf(module_name, _BRDC(" (%s)"), pDev->GetName());
//	PBASE_Info binfo = pModule->GetBaseInfo();
//	sprintf(module_name, " (%s)", binfo->boardName);
	BRDC_strcpy(pSrvInfo->Name, m_name);
	BRDC_strcat(pSrvInfo->Name, module_name);

	return pSrvInfo;
//	return BRDerr_OK;
}

//***************************************************************************************
int CCmpScSrv::SetPropertyFromDialog(void *pDev, void *args)
{
	CModule* pModule = (CModule*)pDev;
	PCMPSCSRV_INFO pSrvInfo = (PCMPSCSRV_INFO)args;

	BRD_CmpSC cmp_data;
	cmp_data.src = pSrvInfo->Source;
	cmp_data.thr[0] = pSrvInfo->Threshold[0];
	cmp_data.thr[1] = pSrvInfo->Threshold[1];
	CtrlSet(pDev, NULL, BRDctrl_CMPSC_SET, &cmp_data);

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	PMAIN_MRES pMres;
	param.reg = MAINnr_MRES;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pMres = (PMAIN_MRES)&param.val;
	if(pMres->ByBits.MuxType && pMres->ByBits.ThdacType)
	{
		BRD_CmpNum cmp_num;
		for(int i = 0; i < BRD_CMPNUM; i++)
		{
			cmp_num.compar = i;
			cmp_num.src = pSrvInfo->SourceExt[i];
			cmp_num.thr = pSrvInfo->Threshold[i];
			CtrlSetExt(pDev, NULL, BRDctrl_CMPSC_SETEXT, &cmp_num);
		}
	}	
	return BRDerr_OK;
}

//***************************************************************************************
int CCmpScSrv::CtrlSet(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
				   )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_CmpSC pCmpSc = (PBRD_CmpSC)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_CMPMUX;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	param.val &= 0xFFFFFFFC;
	param.val |= pCmpSc->src;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	for(int i = 0; i < BRD_CMPNUM; i++)
	{
		param.reg = ADM2IFnr_STATUS;
		PADM2IF_STATUS pStatus;
		do {
			pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
			pStatus = (PADM2IF_STATUS)&param.val;
		} while(!pStatus->ByBits.CmdRdy);
		PCMPSCSRV_CFG pSrvCfg = (PCMPSCSRV_CFG)m_pConfig;
		double max_thr = pSrvCfg->RefPVS / 1000.;
		USHORT thr_data;
		if(fabs(pCmpSc->thr[i]) > max_thr)
		  thr_data = pCmpSc->thr[i] > 0.0 ? 255 : 0;
		else
		  thr_data = (USHORT)floor((pCmpSc->thr[i] / max_thr + 1.) * 128 + 0.5);
		if(thr_data > 255)
			thr_data = 255;
		pSrvCfg->Threshold[i] = max_thr * (thr_data / 128. - 1);
		param.reg = MAINnr_THDAC;
		PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
		pThDac->ByBits.Data = thr_data;
		pThDac->ByBits.Num = i + 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CCmpScSrv::CtrlGet(	
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_CmpSC pCmpSc = (PBRD_CmpSC)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;
	param.reg = MAINnr_CMPMUX;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pCmpSc->src = param.val & 0x3;
	for(int i = 0; i < BRD_CMPNUM; i++)
	{
		PCMPSCSRV_CFG pSrvCfg = (PCMPSCSRV_CFG)m_pConfig;
		pCmpSc->thr[i] = pSrvCfg->Threshold[i];
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CCmpScSrv::CtrlSetExt(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
				   )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_CmpNum pCmpSc = (PBRD_CmpNum)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;

	PMAIN_MRES pMres;
	param.reg = MAINnr_MRES;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pMres = (PMAIN_MRES)&param.val;
	if(!pMres->ByBits.MuxType || !pMres->ByBits.ThdacType)
		return BRDerr_CMD_UNSUPPORTED;

	param.reg = MAINnr_CMPMUX;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PMAIN_MUX pMux = (PMAIN_MUX)&param.val;
	pMux->ByBits.Mode = 1;
	if(pCmpSc->compar)
		pMux->ByBits.Compar1 = pCmpSc->src;
	else
		pMux->ByBits.Compar0 = pCmpSc->src;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus;
	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		pStatus = (PADM2IF_STATUS)&param.val;
	} while(!pStatus->ByBits.CmdRdy);
	PCMPSCSRV_CFG pSrvCfg = (PCMPSCSRV_CFG)m_pConfig;
	int max_data = (int)pow(2., (int)pSrvCfg->ComparBits - 1);

	double max_thr = 2 * (pSrvCfg->RefPVS / 1000.) ;
	short thr_data;
	if(fabs(pCmpSc->thr) > max_thr)
	  thr_data = pCmpSc->thr > 0.0 ? max_data : -max_data;
	else
	  thr_data = (short)floor((pCmpSc->thr / max_thr) * (double)max_data + 0.5);
	if(thr_data > max_data)
		thr_data = max_data;
	if(thr_data < -max_data)
		thr_data = -max_data;
	pSrvCfg->Threshold[pCmpSc->compar] = max_thr * (thr_data / (double)max_data );

	param.reg = MAINnr_THDADDR;
	param.val = pCmpSc->compar;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = MAINnr_THDDATA;
	//PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	param.val = thr_data;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CCmpScSrv::CtrlGetExt(	
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_CmpNum pCmpSc = (PBRD_CmpNum)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;

	PMAIN_MRES pMres;
	param.reg = MAINnr_MRES;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pMres = (PMAIN_MRES)&param.val;
	if(!pMres->ByBits.MuxType || !pMres->ByBits.ThdacType)
		return BRDerr_CMD_UNSUPPORTED;


	param.reg = MAINnr_CMPMUX;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PMAIN_MUX pMux = (PMAIN_MUX)&param.val;
	if(pCmpSc->compar)
		pCmpSc->src = pMux->ByBits.Compar1;
	else
		pCmpSc->src = pMux->ByBits.Compar0;

	PCMPSCSRV_CFG pSrvCfg = (PCMPSCSRV_CFG)m_pConfig;
	pCmpSc->thr = pSrvCfg->Threshold[pCmpSc->compar];

	return BRDerr_OK;
}

//***************************************************************************************
int CCmpScSrv::CtrlCfg(	
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_ComparCfg pCmpCfg = (PBRD_ComparCfg)args;
	PCMPSCSRV_CFG pSrvCfg = (PCMPSCSRV_CFG)m_pConfig;

	pCmpCfg->RefPVS = pSrvCfg->RefPVS;
	pCmpCfg->Bits = pSrvCfg->ComparBits;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;

	PMAIN_MRES pMres;
	param.reg = MAINnr_MRES;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pMres = (PMAIN_MRES)&param.val;
	pCmpCfg->ThdacType = pMres->ByBits.ThdacType;
	pCmpCfg->MuxType = pMres->ByBits.MuxType;

	return BRDerr_OK;
}

//***************************************************************************************
int CCmpScSrv::CtrlDacReg(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
				   )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_DacReg pDacReg = (PBRD_DacReg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;

	PMAIN_MRES pMres;
	param.reg = MAINnr_MRES;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pMres = (PMAIN_MRES)&param.val;
	if(!pMres->ByBits.MuxType || !pMres->ByBits.ThdacType)
		return BRDerr_CMD_UNSUPPORTED;

	param.val = 0;
	PMAIN_THDACADR pThDac = (PMAIN_THDACADR)&param.val;
	if(pDacReg->num == 0xF)
	{
		pThDac->ByBits.Addr = 0;
		pThDac->ByBits.AllAdr = 1;
	}
	else
	{
		pThDac->ByBits.Addr = pDacReg->num;
		pThDac->ByBits.AllAdr = 0;
	}
	pThDac->ByBits.RegNum = pDacReg->reg;

	param.reg = MAINnr_THDADDR;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = MAINnr_THDDATA;
	param.val = pDacReg->data;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

// ***************** End of file CmpSCSrv.cpp ***********************
