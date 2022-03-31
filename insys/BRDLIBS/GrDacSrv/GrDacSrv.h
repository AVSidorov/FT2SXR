/*
 ****************** File GrDacSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : DAC section ???
 *
 * (C) InSys by Sklyarov A. Feb 2011
 *
 ************************************************************
*/

#ifndef _DGRDAC_H
 #define _GRDACRV_H

#include "CtrlGrDac.h"
#include "Service.h"
#include "Adm2If.h"
#include "MainRegs.h"
#include "GrDacRegs.h"


#define SINGLE_DIVIDER 512

const int GRDAC_TETR_ID = 0x7E; // tetrad id

#pragma pack(push,1)
typedef struct _DASRV_CFG {
	U32		Bits;						// разрядность
	U32		Encoding;					// тип кодировки (0 - прямой, 1 - двоично-дополнительный, 2 - код Грея)
	U32		MinRate;					// минимальная частота дискретизации
	U32		MaxRate;					// максимальная частота дискретизации
	U16		OutRange;					// выходной диапазон
} DACSRV_CFG, *PDACSRV_CFG;

typedef struct _GRDACSRV_CFG {
	DACSRV_CFG DacCfg;
	U32		isAlreadyInit;
	U32		FifoSize;
	U32		MaxChans;
	U32		SubExtClk;
} GRDACSRV_CFG, *PGRDACSRV_CFG;

#pragma pack(pop)

class CGrDacSrv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_GrDacTetrNum;

	virtual int  CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
//-------------------------------------------------------------------------------------
	virtual void GetGrDacTetrNum(CModule* pModule);
	virtual int  SetMode(CModule* pModule, void *args);
//-------------------------------------------------------------------------------------


public:

	CGrDacSrv(int idx, int srv_num, CModule* pModule, PGRDACSRV_CFG pCfg); // constructor

	int CtrlIsAvailable(
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
	int CtrlSetMode(
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

#endif // _GRDACSRV_H

//
// End of file
//