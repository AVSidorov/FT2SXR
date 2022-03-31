/*
 ****************** File ddckitSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ddckit service
 *
 * (C) InSys by Sklyarov A. Feb. 2015
 *
 **************************************************************
*/

#ifndef _DDCKIT_H
 #define _DDCKIT_H

#include "ctrlddckit.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "ddckitRegs.h"


#define SINGLE_DIVIDER 512

//const int DDC1G8_TETR_ID = 0x97; // ????
//const int DDC24_TETR_ID = 0xAB; // ????
//const int DDC640_TETR_ID = 0xC1; // ????
const int DDCKIT_TETR_ID = 0xAB; // ????

#pragma pack(push,1)

typedef struct _DDCKITSRV_CFG {
	U32		isAlreadyInit;
	U32		FifoSize;


} DDCKITSRV_CFG, *PDDCKITSRV_CFG;

#pragma pack(pop)

class CddckitSrv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_ddckitTetrNum;
	U32	 m_SystemClock;
//	DDC_CONST m_AdmConst;

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
//-------------------------------------------------------------------------------------
	virtual void GetDdcTetrNum(CModule* pModule);
	virtual int  SetFc(CModule* pModule, void *args);
public:

	CddckitSrv(int idx, int srv_num, CModule* pModule, PDDCKITSRV_CFG pCfg); // constructor

	int CtrlIsAvailable(
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
	int CtrlSetFc(
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

	int CtrlFifoStatus(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlGetStat(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);

};

#endif // _DDCKITSRV_H

//
// End of file
//