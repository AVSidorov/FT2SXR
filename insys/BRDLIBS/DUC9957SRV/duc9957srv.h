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

#include "ctrlduc9957.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "duc9957regs.h"
#include "ad9957.h"


#define SINGLE_DIVIDER 512

const int DUC9957_TETR_ID = 0x6D; // tetrad id

#pragma pack(push,1)
typedef struct _DASRV_CFG {
	U32		Bits;						// ðàçðÿäíîñòü
	U32		Encoding;					// òèï êîäèðîâêè (0 - ïðÿìîé, 1 - äâîè÷íî-äîïîëíèòåëüíûé, 2 - êîä Ãðåÿ)
	U32		MinRate;					// ìèíèìàëüíàÿ ÷àñòîòà äèñêðåòèçàöèè
	U32		MaxRate;					// ìàêñèìàëüíàÿ ÷àñòîòà äèñêðåòèçàöèè
	U16		OutRange;					// âûõîäíîé äèàïàçîí
} DACSRV_CFG, *PDACSRV_CFG;

typedef struct _DUC9957SRV_CFG {
	DACSRV_CFG DacCfg;
	U32		isAlreadyInit;
	U32		FifoSize;
	U32		MaxChans;
	U32		SubExtClk;
} DUC9957SRV_CFG, *PDUC9957SRV_CFG;

#pragma pack(pop)

class CDuc9957Srv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_DucTetrNum;
	AD9957_CFG CfgAD9957;

	virtual int  CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
//-------------------------------------------------------------------------------------
	virtual int  SetClkSource(CModule* pModule, U32 ClkSrc);
	virtual int  GetClkSource(CModule* pModule, U32& ClkSrc);
	virtual void GetDucTetrNum(CModule* pModule);
	virtual int	 WriteDucReg(CModule* pModule,void *args);
	virtual int  ReadDucReg(CModule* pModule, void *args);
	virtual int	 DucMasterReset(CModule* pModule);
	virtual int  DucResetIO(CModule* pModule);
	virtual int  DucUpdateIO(CModule* pModule, U32 chipMask);
	virtual int  SelectProfile(CModule* pModule,U32 numProfile);
	virtual int  SetProfile(CModule* pModule, void *args);
	virtual int  SetMainParam(CModule* pModule, void *args);
	virtual int  SetStartMode(CModule* pModule, PVOID pStMode);
	virtual	int  GetStartMode(CModule* pModule, PVOID pStMode);
//-------------------------------------------------------------------------------------
	virtual	int  InitAD9957(void);
	virtual int	 WriteRegAD9957(CModule* pModule,int addr,int nswords,__int64 data);
	virtual int	 WriteRegsAD9957(CModule* pModule,PAD9957_CFG pcfg);
	virtual int	 ReadRegAD9957(CModule* pModule,int addr,int nswords,__int64 *data);
	virtual int	 ReadRegsAD9957(CModule* pModule,PAD9957_CFG pcfg);
	virtual	int	 CompareRegsAD9957(CModule* pModule,PAD9957_CFG pcfg1,PAD9957_CFG pcfg2);
	virtual int	 ProgAD9957(CModule* pModule,PAD9957_CFG pcfg);
	virtual int  SetSyncMode(CModule* pModule, U32 syncMode);


public:

	CDuc9957Srv(int idx, int srv_num, CModule* pModule, PDUC9957SRV_CFG pCfg); // constructor

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
	int CtrlDucSetReg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlDucGetReg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlDucMasterReset(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlDucResetIO(
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
	int CtrlDucUpdateIO(
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

};

#endif // _DUC9957SRV_H

//
// End of file
//