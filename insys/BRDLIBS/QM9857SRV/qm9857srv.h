/*
 ****************** File Qm9857Srv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : QM9857 section
 *
 * (C) InSys by Sklyarov A. Sep 2006
 *
 ************************************************************
*/

#ifndef _QM9857SRV_H
 #define _QM9857SRV_H

#include "ctrlqm9857.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "qm9857regs.h"


#define SINGLE_DIVIDER 512

const int QM9857_TETR_ID = 0x22; // tetrad id

#pragma pack(push,1)
typedef struct _DASRV_CFG {
	U32		Bits;						// разрядность
	U32		Encoding;					// тип кодировки (0 - прямой, 1 - двоично-дополнительный, 2 - код Грея)
	U32		MinRate;					// минимальная частота дискретизации
	U32		MaxRate;					// максимальная частота дискретизации
	U16		OutRange;					// выходной диапазон
} DACSRV_CFG, *PDACSRV_CFG;

typedef struct _QM9857SRV_CFG {
	DACSRV_CFG DacCfg;
	U32		isAlreadyInit;
	U32		FifoSize;
	U32		MaxChans;
	U32		SubExtClk;
} QM9857SRV_CFG, *PQM9857SRV_CFG;

#pragma pack(pop)

class CQm9857Srv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_QmTetrNum;

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
//-------------------------------------------------------------------------------------
	virtual int  SetClkSource(CModule* pModule, U32 ClkSrc);
	virtual int  GetClkSource(CModule* pModule, U32& ClkSrc);
	virtual void GetQmTetrNum(CModule* pModule);
	virtual int	 WriteQmReg(CModule* pModule,void *args);
	virtual int  ReadQmReg(CModule* pModule, void *args);
	virtual int	 QmReset(CModule* pModule);
	virtual int  QmResetFull(CModule* pModule);
	virtual int  SelectProfile(CModule* pModule,U32 numProfile);
	virtual int  QmUpdateFreq(CModule* pModule, U32 chipMask);
	virtual int  SetProfile(CModule* pModule, void *args);
	virtual int  SetMainParam(CModule* pModule, void *args);
	virtual int  SetStartMode(CModule* pModule, PVOID pStMode);
	virtual	int  GetStartMode(CModule* pModule, PVOID pStMode);
//-------------------------------------------------------------------------------------
	virtual int  SetChanMask(CModule* pModule, ULONG mask);
	virtual int  GetChanMask(CModule* pModule, ULONG& mask);
	virtual int  QmProg(CModule* pModule);

public:

	CQm9857Srv(int idx, int srv_num, CModule* pModule, PQM9857SRV_CFG pCfg); // constructor

	int CtrlMaster(
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
	int CtrlChanMask(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlClkSource(
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
	int CtrlPutData(
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
	int CtrlQmSetReg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlQmGetReg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlQmReset(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlQmResetFull(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSelectProfile(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlQmUpdateFreq(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetProfile(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetMainParam(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlQmProg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
};

#endif // _QM9857SRV_H

//
// End of file
//