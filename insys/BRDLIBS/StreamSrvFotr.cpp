/*
 ***************** File StreamSrvFotr.cpp ***********************
 *
 * BRD Driver for Stream service into techno DLL
 *
 * (C) InSys by Dorokhin A. Sep 2007
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "basemodule.h"
#include "streamsrvfotr.h"

#define	CURRFILE "STREAMSRVFOTR"

CMD_Info ALLOCBUF_CMD		= { BRDctrl_STREAM_CBUF_ALLOC,		0, 0, 0, NULL};
CMD_Info FREEBUF_CMD		= { BRDctrl_STREAM_CBUF_FREE,		0, 0, 0, NULL};
CMD_Info ATTACHBUF_CMD		= { BRDctrl_STREAM_CBUF_ATTACH,		1, 0, 0, NULL};
CMD_Info DETACHBUF_CMD		= { BRDctrl_STREAM_CBUF_DETACH,		1, 0, 0, NULL};
CMD_Info STARTBUF_CMD		= { BRDctrl_STREAM_CBUF_START,		0, 0, 0, NULL};
CMD_Info STOPBUF_CMD		= { BRDctrl_STREAM_CBUF_STOP,		0, 0, 0, NULL};
CMD_Info STATEBUF_CMD		= { BRDctrl_STREAM_CBUF_STATE,		1, 0, 0, NULL};
CMD_Info WAITBUF_CMD		= { BRDctrl_STREAM_CBUF_WAITBUF,	1, 0, 0, NULL};
CMD_Info WAITBUFEX_CMD		= { BRDctrl_STREAM_CBUF_WAITBUFEX,	1, 0, 0, NULL};
CMD_Info WAITBLK_CMD		= { BRDctrl_STREAM_CBUF_WAITBLOCK,	1, 0, 0, NULL};
CMD_Info WAITBLKEX_CMD		= { BRDctrl_STREAM_CBUF_WAITBLOCKEX,1, 0, 0, NULL};
CMD_Info SETDIR_CMD			= { BRDctrl_STREAM_SETDIR,    0, 0, 0, NULL};
CMD_Info SETSRC_CMD			= { BRDctrl_STREAM_SETSRC,    0, 0, 0, NULL};
CMD_Info SETDRQ_CMD			= { BRDctrl_STREAM_SETDRQ,    0, 0, 0, NULL};
CMD_Info RESETFIFO_CMD		= { BRDctrl_STREAM_RESETFIFO,    0, 0, 0, NULL};
CMD_Info GETDIR_CMD			= { BRDctrl_STREAM_GETDIR,    0, 0, 0, NULL};

//***************************************************************************************
CStreamSrv::CStreamSrv(int idx, int srv_num, CModule* pModule, PSTREAMSRV_CFG pCfg) :
	CService(idx, _BRDC("BASESTREAM"), srv_num, pModule, pCfg, sizeof(STREAMSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_STREAM |
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_DIRECTION_OUT |
//			BRDserv_ATTR_UNVISIBLE |
//			BRDserv_ATTR_SUBSERVICE_ONLY |
			BRDserv_ATTR_EXCLUSIVE_ONLY;

	m_StreamNum = srv_num;

	Init(&ALLOCBUF_CMD, (CmdEntry)&CStreamSrv::CtrlAllocBufIo);
	Init(&FREEBUF_CMD, (CmdEntry)&CStreamSrv::CtrlFreeBufIo);
	Init(&ATTACHBUF_CMD, (CmdEntry)&CStreamSrv::CtrlAllocBufIo);
	Init(&DETACHBUF_CMD, (CmdEntry)&CStreamSrv::CtrlFreeBufIo);
	Init(&STARTBUF_CMD, (CmdEntry)&CStreamSrv::CtrlStartBufIo);
	Init(&STOPBUF_CMD, (CmdEntry)&CStreamSrv::CtrlStopBufIo);
	Init(&STATEBUF_CMD, (CmdEntry)&CStreamSrv::CtrlStateBufIo);
	Init(&WAITBUF_CMD, (CmdEntry)&CStreamSrv::CtrlWaitBufIo);
	Init(&WAITBUFEX_CMD, (CmdEntry)&CStreamSrv::CtrlWaitBufIoEx);
	Init(&WAITBLK_CMD, (CmdEntry)&CStreamSrv::CtrlWaitBlockIo);
	Init(&WAITBLKEX_CMD, (CmdEntry)&CStreamSrv::CtrlWaitBlockIoEx);

	Init(&SETDIR_CMD, (CmdEntry)&CStreamSrv::CtrlSetDir);
	Init(&SETSRC_CMD, (CmdEntry)&CStreamSrv::CtrlSetSrc);
	Init(&SETDRQ_CMD, (CmdEntry)&CStreamSrv::CtrlSetDrq);

	Init(&RESETFIFO_CMD, (CmdEntry)&CStreamSrv::CtrlResetFifo);

	m_blkNum = 0;
}

//***************************************************************************************
CStreamSrv::~CStreamSrv()
{
//	BRD_StreamState State;
//	State.timeout = 0;
//	ULONG Status = Stop(m_pModule, &State);
	ULONG Status = Stop(m_pModule);
	Status = Free(m_pModule);
}

//***************************************************************************************
int CStreamSrv::CtrlIsAvailable(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PSERV_CMD_IsAvailable pServAvailable = (PSERV_CMD_IsAvailable)args;
	pServAvailable->attr = m_attribute;
	m_isAvailable = 1;
	m_blkNum = 0;
	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
// размещение буфера
//***************************************************************************************
int CStreamSrv::CtrlAllocBufIo(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
//	Status = pModule->StreamCtrl(STRMcmd_SETMEMORY, args, NULL, NULL);
	AMBF_DMA_CHANNEL Descrip;
	Descrip.DmaChanNum = m_StreamNum;
	Descrip.args = args;
	Status = pModule->StreamCtrl(STRMcmd_SETMEMORY, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);

	PBRDctrl_StreamCBufAlloc pStream = (PBRDctrl_StreamCBufAlloc)args;
	m_dir = pStream->dir;
	m_isSysMem = pStream->isCont;
	m_blkNum = pStream->blkNum;
	m_blkSize = pStream->blkSize;

	return Status;
}

//***************************************************************************************
ULONG CStreamSrv::Free(void* pDev)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	if(m_blkNum)
	{
		//Status = pModule->StreamCtrl(STRMcmd_FREEMEMORY, NULL, NULL, NULL);
		AMBF_DMA_CHANNEL Descrip;
		Descrip.DmaChanNum = m_StreamNum;
		Descrip.args = NULL;
		Status = pModule->StreamCtrl(STRMcmd_FREEMEMORY, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
		m_blkNum = 0;
	}
	return Status;
}	

//***************************************************************************************
int CStreamSrv::CtrlFreeBufIo(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	if(!m_blkNum)
		return BRDerr_STREAM_NOT_ALLOCATED_YET;
	ULONG Status = Stop(m_pModule);
//	if(Status == BRDerr_OK)
	{
		Status = Free(pDev);
		if(Status == BRDerr_OK && pServData)
		{
			PUNDERSERV_Cmd pReleaseCmd = (PUNDERSERV_Cmd)pServData;
			pReleaseCmd->code = SERVcmd_SYS_RELEASE;
		}
	}
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlStartBufIo(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	if(m_blkNum)
	{
		//Status = pModule->StreamCtrl(STRMcmd_STARTMEMORY, args, m_dir, NULL);
		AMBF_DMA_CHANNEL Descrip;
		Descrip.DmaChanNum = m_StreamNum;
		Descrip.args = args;
		Status = pModule->StreamCtrl(STRMcmd_STARTMEMORY, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
	}
	else
		return BRDerr_STREAM_NOT_ALLOCATED_YET;
	return Status;
}

//***************************************************************************************
ULONG CStreamSrv::Stop(void	*pDev)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	if(m_blkNum)
	{
		//Status = pModule->StreamCtrl(STRMcmd_STOPMEMORY, NULL, NULL, NULL);
		AMBF_DMA_CHANNEL Descrip;
		Descrip.DmaChanNum = m_StreamNum;
		Descrip.args = NULL;
		Status = pModule->StreamCtrl(STRMcmd_STOPMEMORY, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
	}
	else
		return BRDerr_STREAM_NOT_ALLOCATED_YET;
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlStopBufIo(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	ULONG Status = Stop(pDev);
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlStateBufIo(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	if(m_blkNum)
	{
		AMBF_DMA_CHANNEL Descrip;
		Descrip.DmaChanNum = m_StreamNum;
		Descrip.args = args;
		Status = pModule->StreamCtrl(STRMcmd_STATEMEMORY, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
		//Status = pModule->StreamCtrl(STRMcmd_STATEMEMORY, args, NULL, NULL);
	}
	else
	{
		PBRDctrl_StreamCBufState pState = (PBRDctrl_StreamCBufState)args;
		pState->blkNum = 0;
		pState->blkNumTotal = 0;
		pState->offset = 0;
		pState->state = 0;
		Status = BRDerr_STREAM_NOT_ALLOCATED_YET;
	}
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlWaitBufIo(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	if(m_blkNum)
	{
		AMBF_DMA_CHANNEL Descrip;
		Descrip.DmaChanNum = m_StreamNum;
		Descrip.args = args;
		Status = pModule->StreamCtrl(STRMcmd_WAITBUF, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
		//Status = pModule->StreamCtrl(STRMcmd_WAITBUF, args, NULL, NULL);
	}
	else
		Status = BRDerr_STREAM_NOT_ALLOCATED_YET;
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlWaitBufIoEx(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	if(m_blkNum)
	{
		AMBF_DMA_CHANNEL Descrip;
		Descrip.DmaChanNum = m_StreamNum;
		Descrip.args = args;
		Status = pModule->StreamCtrl(STRMcmd_WAITBUFEX, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
		//Status = pModule->StreamCtrl(STRMcmd_WAITBUFEX, args, NULL, NULL);
	}
	else
		Status = BRDerr_STREAM_NOT_ALLOCATED_YET;
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlWaitBlockIo(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	if(m_blkNum)
	{
		AMBF_DMA_CHANNEL Descrip;
		Descrip.DmaChanNum = m_StreamNum;
		Descrip.args = args;
		Status = pModule->StreamCtrl(STRMcmd_WAITBLK, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
		//Status = pModule->StreamCtrl(STRMcmd_WAITBLK, args, NULL, NULL);
	}
	else
		Status = BRDerr_STREAM_NOT_ALLOCATED_YET;
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlWaitBlockIoEx(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	if(m_blkNum)
	{
		AMBF_DMA_CHANNEL Descrip;
		Descrip.DmaChanNum = m_StreamNum;
		Descrip.args = args;
		Status = pModule->StreamCtrl(STRMcmd_WAITBLKEX, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
		//Status = pModule->StreamCtrl(STRMcmd_WAITBLKEX, args, NULL, NULL);
	}
	else
		Status = BRDerr_STREAM_NOT_ALLOCATED_YET;
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlSetDir(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	m_dir = *(PULONG)args;
	ULONG Status = BRDerr_OK;
	//Status = pModule->StreamCtrl(STRMcmd_SETDIR, args, NULL, NULL);
	AMBF_DMA_CHANNEL Descrip;
	Descrip.DmaChanNum = m_StreamNum;
	Descrip.args = args;
	Status = pModule->StreamCtrl(STRMcmd_SETDIR, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlSetSrc(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	//Status = pModule->StreamCtrl(STRMcmd_SETSRC, args, NULL, NULL);
	AMBF_DMA_CHANNEL Descrip;
	Descrip.DmaChanNum = m_StreamNum;
	Descrip.args = args;
	Status = pModule->StreamCtrl(STRMcmd_SETSRC, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlSetDrq(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	//Status = pModule->StreamCtrl(STRMcmd_SETDRQ, args, NULL, NULL);
	AMBF_DMA_CHANNEL Descrip;
	Descrip.DmaChanNum = m_StreamNum;
	Descrip.args = args;
	Status = pModule->StreamCtrl(STRMcmd_SETDRQ, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlResetFifo(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	AMBF_DMA_CHANNEL Descrip;
	Descrip.DmaChanNum = m_StreamNum;
	Descrip.args = NULL;
	Status = pModule->StreamCtrl(STRMcmd_RESETFIFO, &Descrip, sizeof(AMBF_DMA_CHANNEL), NULL);
	//Status = pModule->StreamCtrl(STRMcmd_RESETFIFO, NULL, NULL, NULL);
	return Status;
}

//***************************************************************************************
int CStreamSrv::CtrlGetDir(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CBaseModule* pModule = (CBaseModule*)pDev;
	ULONG Status = BRDerr_OK;
	//AMB_GET_DMA_INFO InfoDescrip;
	//InfoDescrip.DmaChanNum = m_StreamNum;
	//InfoDescrip.Direction = m_dir;
	//Status = pModule->StreamCtrl(STRMcmd_GETDMACHANINFO, &InfoDescrip, sizeof(AMB_GET_DMA_INFO), NULL);
	//*(PULONG)args = InfoDescrip.Direction;
	return Status;
}

// ***************** End of file StreamSrv.cpp ***********************
