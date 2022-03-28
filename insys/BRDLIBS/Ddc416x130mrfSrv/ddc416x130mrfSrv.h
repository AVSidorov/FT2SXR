/*
 ****************** File ddc416x130mrfSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : DDC416x130MRF section
 *
 * (C) InSys by Sklyarov A. Dec. 2011
 *
 **************************************************************
*/

#ifndef _DDC416x130MRF_H
 #define _DDC416x130MRF_H

#include "ctrlDdc416x130mrf.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "adm416x130mrf_ddcRegs.h"


#define SINGLE_DIVIDER 512

const int DDCMRF_TETR_ID = 0x7C; //  Tetrad ID ADM416x130MRF_DDC ???
const int DDDMRF_TETR_ID = 0x31; //  Tetrad ID ADM416x130MRF_DDD ????

typedef enum {
	ADMDDC	= 0x0A0B,
	ADMDDD	= 0x0A0C,
};

#pragma pack(push,1)

typedef struct _DDC416x130MRFSRV_CFG {
	U32		isAlreadyInit;
	U32		FifoSize;
	U32		SubIntClk;
	U32		SubExtClk;
	U32		AdcBits;
	U32		AdcEncoding;
	U32		AdcMinRate;
	U32		AdcMaxRate;
	U32		AdcInpRange;
	U32		AdcCnt;
	U32		DdcBits;
	U32		DdcCnt;
	U32		SubType;	// тип субмодуля (ADM416x130MRF - 0x0A0B ???)
	U32		SubVer;

} DDCMRFSRV_CFG, *PDDCMRFSRV_CFG;

#pragma pack(pop)

class CDdc416x130mrfSrv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_DdcMrfTetrNum;
	U32	 m_SystemClock;
	DDC_CONST m_AdmConst;

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
//-------------------------------------------------------------------------------------
	virtual void GetMrfTetrNum(CModule* pModule);
	virtual int  SetClkMode(CModule* pModule,PBRD_ClkMode pClkMode);
	virtual int  SetChanMask(CModule* pModule, ULONG mask);
	virtual int  SetStartMode(CModule* pModule, PVOID pStMode);
	virtual int  SetAdmMode(CModule* pModule, void *args);
	virtual int	 SetAdc(CModule* pModule,void *args);
	virtual int  SetDdcFc(CModule* pModule, void *args);
	virtual int  SetDdcFlt(CModule* pModule, void *args);
	virtual int  SetDdcDecim(CModule* pModule, void *args);
	virtual int  SetFrame(CModule* pModule, PBRD_SetFrame pSetFrame);
 	virtual int  GetCfg( PBRD_DDCMRFCfg pCfg );
	virtual int  StartDdc(CModule* pModule);
	virtual int  StopDdc(CModule* pModule);
	virtual int  SetMarker(CModule* pModule, void *args);
public:

	CDdc416x130mrfSrv(int idx, int srv_num, CModule* pModule, PDDCMRFSRV_CFG pCfg); // constructor

    int  CtrlGetCfg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlIsAvailable(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
				   );
	int CtrlClkMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlChanMask(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlStartMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSetAdmMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlGetData(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlGetAddrData(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetAdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetDdcFc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetDdcFlt(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetDdcDecim(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetFrame(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlFifoReset(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlStart(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlStop(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlFifoStatus(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlStartDdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlStopDdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetMarker(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);


};

#endif // _DDC416x130MRF_H

//
// End of file
//