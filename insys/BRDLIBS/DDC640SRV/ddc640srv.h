/*
 ****************** File ddc640Srv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ddc1g8 service
 *
 * (C) InSys by Sklyarov A. Oct. 2014
 *
 **************************************************************
*/

#ifndef _DDC640_H
 #define _DDC640_H

#include "ctrlddc640.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "ddc640Regs.h"


#define SINGLE_DIVIDER 512

//const int DDC1G8_TETR_ID = 0x97; // ????
//const int DDC24_TETR_ID = 0xAB; // ????
const int DDC640_TETR_ID = 0xC1; // ????

#pragma pack(push,1)

typedef struct _DDC640SRV_CFG {
	U32		isAlreadyInit;
	U32		FifoSize;


} DDC640SRV_CFG, *PDDC640SRV_CFG;

#pragma pack(pop)

class Cddc640Srv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_ddc640TetrNum;
	U32	 m_SystemClock;
//	DDC_CONST m_AdmConst;

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
//-------------------------------------------------------------------------------------
	virtual void GetDdcTetrNum(CModule* pModule);
	virtual int  SetMode(CModule* pModule, void *args);
	virtual int  SetChans(CModule* pModule, void *args);
	virtual int  SetFcWb(CModule* pModule, void *args);
	virtual int  SetFcNb(CModule* pModule, void *args);
	virtual int  GetStat(CModule* pModule, void *args);
public:

	Cddc640Srv(int idx, int srv_num, CModule* pModule, PDDC640SRV_CFG pCfg); // constructor

	int CtrlIsAvailable(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
				   );
	int CtrlSetMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSetChans(
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
	int CtrlSetFcWb(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetFcNb(
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

#endif // _DDC640SRV_H

//
// End of file
//