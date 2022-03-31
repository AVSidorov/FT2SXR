/*
 ****************** File Duc9957Srv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : QM9857 section
 *
 * (C) InSys by Sklyarov A. Dec 2009
 *
 ************************************************************
*/

#ifndef _DUC9957SRV_H
 #define _DUC9957SRV_H

#include "ctrlqm9957.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "qm9957Regs.h"
#include "ad9957.h"


#define SINGLE_DIVIDER 512

const int QM9957_TETR_ID = 0x7B; // tetrad id

#pragma pack(push,1)
typedef struct _DASRV_CFG {
	U32		Bits;						// разрядность
	U32		Encoding;					// тип кодировки (0 - прямой, 1 - двоично-дополнительный, 2 - код Грея)
	U32		MinRate;					// минимальная частота дискретизации
	U32		MaxRate;					// максимальная частота дискретизации
	U16		OutRange;					// выходной диапазон
} DACSRV_CFG, *PDACSRV_CFG;

typedef struct _QM9957SRV_CFG {
	DACSRV_CFG DacCfg;
	U32		isAlreadyInit;
	U32		FifoSize;
	U32		MaxChans;
	U32		SubExtClk;
} QM9957SRV_CFG, *PQM9957SRV_CFG;

#pragma pack(pop)

class CQm9957Srv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_QmTetrNum;
	AD9957_CFG CfgAD9957;

	virtual int  CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
//-------------------------------------------------------------------------------------
	virtual int  SetClkSource(CModule* pModule, U32 ClkSrc);
	virtual int  GetClkSource(CModule* pModule, U32& ClkSrc);
	virtual int  SetChanMask(CModule* pModule, ULONG mask);
	virtual int  GetChanMask(CModule* pModule, ULONG& mask);
	virtual void GetDucTetrNum(CModule* pModule);
	virtual int	 WriteQmReg(CModule* pModule,void *args);
	virtual int  ReadQmReg(CModule* pModule, void *args);
	virtual int	 QmMasterReset(CModule* pModule);
	virtual int  QmResetIO(CModule* pModule);
	virtual int  QmUpdateIO(CModule* pModule, U32 chipMask);
	virtual int  SelectProfile(CModule* pModule,U32 numProfile);
	virtual int  SetProfile(CModule* pModule, void *args);
	virtual int  SetMainParam(CModule* pModule, void *args);
	virtual int  SetStartMode(CModule* pModule, PVOID pStMode);
	virtual	int  GetStartMode(CModule* pModule, PVOID pStMode);
//-------------------------------------------------------------------------------------
	virtual	int  InitAD9957(void);
	virtual int	 WriteRegAD9957(CModule* pModule,int addr,int nswords,__int64 data,U32 chipMask);
	virtual int	 WriteRegsAD9957(CModule* pModule,PAD9957_CFG pcfg,U32 chipMask);
	virtual int	 ReadRegAD9957(CModule* pModule,int addr,int nswords,__int64 *data,U32 chipMask);
	virtual int	 ReadRegsAD9957(CModule* pModule,PAD9957_CFG pcfg, U32 chipMask);
	virtual	int	 CompareRegsAD9957(CModule* pModule,PAD9957_CFG pcfg1,PAD9957_CFG pcfg2);
	virtual int	 ProgAD9957(CModule* pModule,PAD9957_CFG pcfg,U32 chipMask);
	virtual int  SetSyncMode(CModule* pModule,void *args);
	virtual int  GetCfg(PVOID pCfg);

public:

	CQm9957Srv(int idx, int srv_num, CModule* pModule, PQM9957SRV_CFG pCfg); // constructor

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
	int CtrlClkSource(
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
	int CtrlQmMasterReset(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlQmResetIO(
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
	int CtrlQmUpdateIO(
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
	int CtrlDucProg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetSyncMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetCyclingMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlGetCfg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);

};

#endif // _DUC9957SRV_H

//
// End of file
//
