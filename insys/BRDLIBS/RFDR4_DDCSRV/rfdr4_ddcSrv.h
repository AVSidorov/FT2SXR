/*
 ****************** File rfdr4_ddcSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : rfdr4_ddc service
 *
 * (C) InSys by Sklyarov A. Aug. 2014
 *
 **************************************************************
*/

#ifndef _RFDR4_DDC_H
 #define _RFDR4_DDC_H

#include "Ctrlrfdr4_ddc.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "rfdr4_ddcRegs.h"


#define SINGLE_DIVIDER 512

//const int DDC1G8_TETR_ID = 0x97; // ????
const int RFDR4_DDC_TETR_ID = 0xbf; // ????

#pragma pack(push,1)

typedef struct _RFDR4_DDCSRV_CFG {
	U32		isAlreadyInit;
	U32		FifoSize;


} RFDR4_DDCSRV_CFG, *PRFDR4_DDCSRV_CFG;

#pragma pack(pop)

class Crfdr4_ddcSrv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_Rfdr4_DdcTetrNum;
	U32	 m_SystemClock;

//-------------------------------------------------------------------------------------
	virtual void GetDdcTetrNum(CModule* pModule);
	virtual int  SetChanMask(CModule* pModule, void *args);
	virtual int  SetStartMode(CModule* pModule, void *args);
	virtual int  SetFrame(CModule* pModule, void *args);
	virtual int  StartDdc(CModule* pModule, void *args);
	virtual int  StopDdc(CModule* pModule, void *args);
	virtual int  SetDdcExt(CModule* pModule, void *args);
	virtual int  SetCorrectExt(CModule* pModule, void *args);
	virtual int  SetScan(CModule* pModule, void *args);
	virtual int  SelScan(CModule* pModule, void *args);
	virtual int  EnbScan(CModule* pModule, void *args);
	virtual int  SetLabel(CModule* pModule, void *args);
	virtual	int  readMsSpdDev(CModule* pModule,U32 addr, U32 dbl, U32 reg, ULONG* pVal);
	virtual int  writeMsSpdDev(CModule* pModule,U32 addr, U32 dbl, U32 reg, U32 val);
	virtual int  GetPower(CModule* pModule, void *args);
	virtual int  InitPower(CModule* pModule, void *args );
	virtual int  SetDetect(CModule* pModule, void *args );
	virtual int  GetDetect(CModule* pModule, void *args );
	virtual int  SetLed(CModule* pModule, void *args );


public:

	Crfdr4_ddcSrv(int idx, int srv_num, CModule* pModule, PRFDR4_DDCSRV_CFG pCfg); // constructor


	int CtrlCapture(
		void		*pDev,		// InfoSM or InfoBM
		void		*pServData,	// Specific Service Data
		ULONG		cmd,		// Command Code (from BRD_ctrl())
		void		*args 		// Command Arguments (from BRD_ctrl())
	);
	int CtrlRelease(
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

	int CtrlSetChanMask(
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
	int CtrlSetDdcExt(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSetScan(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSelScan(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlEnbScan(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSetLabel(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlGetPower(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlInitPower(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetLed(
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
	int CtrlSetCorrectExt(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetDetect(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlGetDetect(
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
	int	CtrlStartDdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int	CtrlStopDdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);


};

#endif // _RFDR4_ADCSRV_H

//
// End of file
//
