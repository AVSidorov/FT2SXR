/*
 ****************** File mrfddcSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : MRFDDC section
 *
 * (C) InSys by Sklyarov A. Apr. 2012
 *
 **************************************************************
*/

#ifndef _MRFDDC_H
 #define _MRFDDC_H

#include "ctrlmrfddc.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "mrfddcRegs.h"


#define SINGLE_DIVIDER 512

const int MRFDDC_TETR_ID = 0x7C; //  Tetrad Subunit ID ADM416x130MRF_DDC ???
//const int MRFDDC_TETR_ID = 0x31; //  Tetrad ID ADM416x130MRF_DDD ????

typedef enum {
	ADMDDC	= 0x0A0B,
	ADMDDD	= 0x0A0C,
};

#pragma pack(push,1)

typedef struct _MRFDDC_CFG {
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

} MRFDDCSRV_CFG, *PMRFDDCSRV_CFG;

#pragma pack(pop)

class CmrfddcSrv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_MrfDdcTetrNum;
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
	virtual int  SetDdcInp(CModule* pModule, void *args);
	virtual int  SetDdcFc(CModule* pModule, void *args);
	virtual int  SetDdcFlt(CModule* pModule, void *args);
	virtual int  SetDdcDecim(CModule* pModule, void *args);
 	virtual int  AdmGetInfo(CModule* pModule, PBRD_AdmGetInfo pAdmInfo);
public:

	CmrfddcSrv(int idx, int srv_num, CModule* pModule, PMRFDDCSRV_CFG pCfg); // constructor

    int  CtrlAdmGetInfo(
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
	int CtrlSetDdcInp(
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


};

#endif // _MRFDDC_H

//
// End of file
//