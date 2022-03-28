/****************** File FotrDataSrv.cpp **********************************
 *
 * CTRL-command for BRD Driver for FOTRDATA optional service for streams on ADM-FOTR submodule
 *
 * (C) InSys by Dorokhin A. Feb 2007
 *
 **********************************************************************/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "fotrdatasrv.h"

#define	CURRFILE "FOTRDATASRV"

static CMD_Info INFIFORESET_CMD		= { BRDctrl_FOTR_INFIFORESET,	  0, 0, 0, NULL};
static CMD_Info INENABLE_CMD		= { BRDctrl_FOTR_INENABLE,		  0, 0, 0, NULL};
static CMD_Info INFIFOSTATUS_CMD	= { BRDctrl_FOTR_INFIFOSTATUS,	  1, 0, 0, NULL};
static CMD_Info GETDATA_CMD			= { BRDctrl_FOTR_GETDATA,		  0, 0, 0, NULL};
static CMD_Info GETINSRCSTREAM_CMD	= { BRDctrl_FOTR_GETINSRCSTREAM,  1, 0, 0, NULL};

static CMD_Info OUTFIFORESET_CMD	= { BRDctrl_FOTR_OUTFIFORESET,	  0, 0, 0, NULL};
static CMD_Info OUTENABLE_CMD		= { BRDctrl_FOTR_OUTENABLE,		  0, 0, 0, NULL};
static CMD_Info OUTFIFOSTATUS_CMD	= { BRDctrl_FOTR_OUTFIFOSTATUS,	  1, 0, 0, NULL};
static CMD_Info PUTDATA_CMD			= { BRDctrl_FOTR_PUTDATA,		  0, 0, 0, NULL};
static CMD_Info GETOUTSRCSTREAM_CMD	= { BRDctrl_FOTR_GETOUTSRCSTREAM, 1, 0, 0, NULL};

//***************************************************************************************
CFotrDataSrv::CFotrDataSrv(int idx, int srv_num, CModule* pModule, PFOTRDATASRV_CFG pCfg) :
	CService(idx, _BRDC("FOTRDATA"), srv_num, pModule, pCfg, sizeof(FOTRDATASRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_DIRECTION_OUT |
			BRDserv_ATTR_STREAMABLE_OUT |
			BRDserv_ATTR_EXCLUSIVE_ONLY;

	Init(&INFIFORESET_CMD, (CmdEntry)&CFotrDataSrv::CtrlFifoReset);
	Init(&INENABLE_CMD, (CmdEntry)&CFotrDataSrv::CtrlEnable);
	Init(&INFIFOSTATUS_CMD, (CmdEntry)&CFotrDataSrv::CtrlFifoStatus);
	Init(&GETDATA_CMD, (CmdEntry)&CFotrDataSrv::CtrlGetData);
	Init(&GETINSRCSTREAM_CMD, (CmdEntry)&CFotrDataSrv::CtrlGetAddrData);

	Init(&OUTFIFORESET_CMD, (CmdEntry)&CFotrDataSrv::CtrlFifoReset);
	Init(&OUTENABLE_CMD, (CmdEntry)&CFotrDataSrv::CtrlEnable);
	Init(&OUTFIFOSTATUS_CMD, (CmdEntry)&CFotrDataSrv::CtrlFifoStatus);
	Init(&PUTDATA_CMD, (CmdEntry)&CFotrDataSrv::CtrlPutData);
	Init(&GETOUTSRCSTREAM_CMD, (CmdEntry)&CFotrDataSrv::CtrlGetAddrData);
}

//***************************************************************************************
int CFotrDataSrv::CtrlIsAvailable(
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

	//	2 канала FOTRDATA - это 2 экземпляра тетрад
	// канал 0 - служба FOTRDATA0, канал 1 - служба FOTRDATA1
	ULONG TetrInstantNum = 1;
	BRDCHAR* pBuf = m_name + (BRDC_strlen(m_name) - 2);
	if(BRDC_strchr(pBuf, _BRDC('1')))
		TetrInstantNum = 2;
	
	//m_FotrTetrNum = GetTetrNumEx(pModule, m_index, FOTR_TETR_ID, TetrInstantNum);
	m_FotrTetrNum = GetTetrNumEx(pModule, m_index, FOTR3G_TETR_ID, TetrInstantNum);
	m_DataInTetrNum = GetTetrNumEx(pModule, m_index, DATA32IN_TETR_ID, TetrInstantNum);
	if(m_DataInTetrNum == -1)
		m_DataInTetrNum = GetTetrNumEx(pModule, m_index, DATA64IN_TETR_ID, TetrInstantNum);
	m_DataOutTetrNum = GetTetrNumEx(pModule, m_index, DATA32OUT_TETR_ID, TetrInstantNum);
	if(m_DataOutTetrNum == -1)
		m_DataOutTetrNum = GetTetrNumEx(pModule, m_index, DATA64OUT_TETR_ID, TetrInstantNum);
	if(m_MainTetrNum != -1 && m_FotrTetrNum != -1 && m_DataInTetrNum != -1 && m_DataOutTetrNum != -1)
	{
		m_isAvailable = 1;
		PFOTRDATASRV_CFG pSrvCfg = (PFOTRDATASRV_CFG)m_pConfig;
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;
			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_DataInTetrNum;
			if(!pSrvCfg->DataInFifoSize)
			{
				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				pSrvCfg->DataInFifoSize = RegParam.val * widthFifo;
			}

			RegParam.tetr = m_DataOutTetrNum;
			if(!pSrvCfg->DataOutFifoSize)
			{
				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				pSrvCfg->DataOutFifoSize = RegParam.val * widthFifo;
			}
		}
	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrDataSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	ULONG AdmNum;
	//TCHAR buf[SERVNAME_SIZE];
	//_tcscpy_s(buf, m_name);
	//PTCHAR pBuf = buf + (strlen(buf) - 2);
	//if(_tcschr(pBuf, '1'))
	//	AdmNum = 1;
	//else
		AdmNum = 0;
//	if(BRDctrl_FOTR_GETINSRCSTREAM == cmd)
		*(ULONG*)args = (AdmNum << 16) | m_DataInTetrNum;
	if(BRDctrl_FOTR_GETOUTSRCSTREAM == cmd)
		if(m_DataOutTetrNum !=  -1)
			*(ULONG*)args = (AdmNum << 16) | m_DataOutTetrNum;
		else
		{
			*(ULONG*)args = 0;
			return BRDerr_INSUFFICIENT_RESOURCES;
		}
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrDataSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DataInTetrNum;
	if(BRDctrl_FOTR_OUTFIFORESET == cmd)
		if(m_DataOutTetrNum != -1)
			param.tetr = m_DataOutTetrNum;
		else
			return BRDerr_INSUFFICIENT_RESOURCES;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pMode0->ByBits.FifoRes = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrDataSrv::CtrlEnable (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DataInTetrNum;
	if(BRDctrl_FOTR_OUTENABLE == cmd)
		if(m_DataOutTetrNum != -1)
			param.tetr = m_DataOutTetrNum;
		else
			return BRDerr_INSUFFICIENT_RESOURCES;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Start = *(PULONG)args;
	pMode0->ByBits.Master = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrDataSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DataInTetrNum;
	if(BRDctrl_FOTR_OUTFIFOSTATUS == cmd)
		if(m_DataOutTetrNum != -1)
			param.tetr = m_DataOutTetrNum;
		else
			return BRDerr_INSUFFICIENT_RESOURCES;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrDataSrv::CtrlGetData(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DataInTetrNum;
/*	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pMode0->ByBits.FifoRes = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
*/
	//PADM2IF_STATUS pStatus;
	//do
	//{
	//	param.reg = ADM2IFnr_STATUS;
	//	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//	pStatus = (PADM2IF_STATUS)&param.val;
	//} while(pStatus->ByBits.HalfFull);

	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);

	//PULONG buf = (PULONG)pBuf->pData;
	//param.pBuf = (PULONG)0x01020304;
	//param.bytes = 0x05060708;
	//for(ULONG i = 0; i < pBuf->size / sizeof(ULONG); i++)
	//{
	//	param.val = 0xff0ff0ff;
	//	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//	buf[i] = param.val;
	//}
	
	//param.reg = ADM2IFnr_STATUS;
	//pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//pStatus = (PADM2IF_STATUS)&param.val;

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrDataSrv::CtrlPutData(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	if(m_DataOutTetrNum == -1)
		return BRDerr_INSUFFICIENT_RESOURCES;

	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DataOutTetrNum;

	//param.reg = ADM2IFnr_MODE0;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	//pMode0->ByBits.FifoRes = 1;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//pMode0->ByBits.FifoRes = 0;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	//param.reg = ADM2IFnr_STATUS;
	//pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;

	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
//	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);

	PULONG buf = (PULONG)pBuf->pData;
	for(ULONG i = 0; i < pBuf->size / sizeof(ULONG); i++)
	{
		param.val = buf[i];
		pModule->RegCtrl(DEVScmd_REGWRITEDIR, &param);
	}
	
	//param.reg = ADM2IFnr_STATUS;
	//pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//pStatus = (PADM2IF_STATUS)&param.val;

	return BRDerr_OK;
}

// ***************** End of file FotrDataSrv.cpp ***********************
