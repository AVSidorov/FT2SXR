/*
 ****************** File StreamSrv.h *************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : Stream section
 *
 * (C) InSys by Dorokhin Andrey Feb, 2004
 *
 * 27.12.2006 - add functions SysBuf(), FileMapBuf(), VirtualBuf() - implement non-auto component buffer
 *
 ************************************************************
*/

#ifndef _STREAMSRV_H
 #define _STREAMSRV_H

#include "ctrlstrm.h"
#include "service.h"
#include "streamll.h"

enum {
	STRMcmd_SETMEMORY	= 1,
	STRMcmd_FREEMEMORY,
	STRMcmd_STARTMEMORY,
	STRMcmd_STOPMEMORY,
	STRMcmd_STATEMEMORY,
	STRMcmd_WAITBUF,
	STRMcmd_WAITBUFEX,
	STRMcmd_WAITBLK,
	STRMcmd_WAITBLKEX,
	STRMcmd_SETDIR,
	STRMcmd_SETSRC,
	STRMcmd_SETDRQ,
	STRMcmd_RESETFIFO,
	STRMcmd_GETDMACHANINFO
};

#pragma pack(push,1)

typedef struct _STREAMSRV_CFG {
	U32		dummy;
} STREAMSRV_CFG, *PSTREAMSRV_CFG;

typedef struct _AMBF_DMA_CHANNEL {
	ULONG	DmaChanNum;		// IN
	PVOID	args;			// IN
} AMBF_DMA_CHANNEL, *PAMBF_DMA_CHANNEL;

#pragma pack(pop)

class CStreamSrv: public CService
{

protected:

	long	m_StreamNum;
	ULONG	m_isCycling;
	ULONG	m_dir;
	ULONG	m_isSysMem;
	ULONG	m_blkNum;
	ULONG	m_blkSize;

	ULONG Stop(void* pDev);
	ULONG Free(void* pDev);

public:

	CStreamSrv(int idx, int srv_num, CModule* pModule, PSTREAMSRV_CFG pCfg); // constructor
	~CStreamSrv(); // destructor

	int CtrlIsAvailable(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlAllocBufIo(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlAttachBufIo(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlFreeBufIo(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlStartBufIo(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlStopBufIo(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlStateBufIo(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlWaitBufIo(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlWaitBufIoEx(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlWaitBlockIo(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlWaitBlockIoEx(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlSetDir(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlSetSrc(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlSetDrq(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlResetFifo(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

	int CtrlGetDir(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);

};

#endif // _STREAMSRV_H

//
// End of file
//