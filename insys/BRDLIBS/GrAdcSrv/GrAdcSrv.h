/*
 ****************** File GrDacSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section ???
 *
 * (C) InSys by Sklyarov A. Feb 2011
 *
 ************************************************************
*/

#ifndef _GRADCSRV_H
 #define _GRDACRV_H

#include "CtrlGrAdc.h"
#include "Service.h"
#include "Adm2If.h"
#include "MainRegs.h"
#include "GrAdcRegs.h"


#define SINGLE_DIVIDER 512

const int GRADC_TETR_ID = 0x7D; //  tetrad id

#pragma pack(push,1)
typedef struct _GRADCSRV_CFG {
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
	U32		SubType;	// тип субмодуля (ADMDDC5016 - 0x0A0B, ADMDDC416x100M - 0x0A0C, ADMDDC214x400M - 0x0A0D)
	U32		SubVer;
} GRADCSRV_CFG, *PGRADCSRV_CFG;


#pragma pack(pop)

class CGrAdcSrv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_GrAdcTetrNum;

	virtual int  CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
//-------------------------------------------------------------------------------------
	virtual void GetGrAdcTetrNum(CModule* pModule);
	virtual int  SetChanMask(CModule* pModule, ULONG mask);
	virtual int  SetMode(CModule* pModule, void *args);
	virtual int  SetModeDdc(CModule* pModule, void *args);
	virtual int  SetFilterDdc(CModule* pModule, void *args);
//-------------------------------------------------------------------------------------


public:

	CGrAdcSrv(int idx, int srv_num, CModule* pModule, PGRADCSRV_CFG pCfg); // constructor

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
	int CtrlGetAddrData(
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
	int CtrlSetMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetModeDdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetFilterDdc(
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