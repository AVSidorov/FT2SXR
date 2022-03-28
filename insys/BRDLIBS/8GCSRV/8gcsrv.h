/*
 ****************** File 8gcSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : GC5016 section
 *
 * (C) InSys by Sklyarov A. Apr. 2012
 *
 ************************************************************
*/

#ifndef _8GCRV_H
 #define _8GCSRV_H

#include "ctrl8gc.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "8gcregs.h"


#define SINGLE_DIVIDER 512

const int SUBUNIT_TETR_ID = 0x7C;

enum {

	ADMDDC216x250M	= 0x0A0D,// ??
};

#pragma pack(push,1)

typedef struct _8GCSRV_CFG {
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

} GCSRV_CFG, *PGCSRV_CFG;

#pragma pack(pop)

class C8gcSrv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_GcTetrNum;
	U32	 m_SystemClock;
	DDC_CONST m_AdmConst;

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
//-------------------------------------------------------------------------------------
	virtual int  SetClkMode(CModule* pModule, PBRD_ClkMode pClkMode);
	virtual int  GetClkMode(CModule* pModule, PBRD_ClkMode pClkMode);
	virtual void GetGcTetrNum(CModule* pModule);
 	virtual int  AdmGetInfo(CModule* pModule, PBRD_AdmGetInfo pAdmInfo);
	virtual int	 WriteGcReg(CModule* pModule,void *args);
	virtual int  ReadGcReg(CModule* pModule, void *args);
	virtual int  GetAdcOver(CModule* pModule,void *args);
	virtual int  SetDdcFc(CModule* pModule, void *args);
	virtual __int64 CalcCodeNcoFreq(int ddcmode,U32 freq, unsigned int *codeDphase);
	virtual int  SetAdmMode(CModule* pModule, void *args);
	virtual int  SetDdcMode(CModule* pModule, void *args);
	virtual int  SetStartMode(CModule* pModule, PVOID pStMode);
	virtual	int  GetStartMode(CModule* pModule, PVOID pStMode);
	virtual int  SetTarget(CModule* pModule, ULONG target);
	virtual int  GetTarget(CModule* pModule, ULONG& target);
	virtual int	 SetTestMode(CModule* pModule, ULONG mode);
	virtual int	 GetTestMode(CModule* pModule, ULONG& mode);
	 //-------------------------------------------------------------------------------------
	virtual int  SetChanMask(CModule* pModule, ULONG mask);
	virtual int  GetChanMask(CModule* pModule, ULONG& mask);
	virtual int  SetDdcSync(CModule* pModule, void *args);
	virtual int  ProgramDdc(CModule* pModule, void *args);
	virtual int  ProgramAdc(CModule* pModule, void *args);
	virtual int  VerifyProgDdc(CModule* pModule,U32 *regs,int nItems);
	virtual int  StartDdc(CModule* pModule);
	virtual int  StopDdc(CModule* pModule);
	virtual int  SetDelay(CModule* pModule,void* args);
	virtual int  SetTestSequence(CModule* pModule,void* args);
	virtual int  SetBitSlip(CModule* pModule,void* args);
	virtual int  RestNearFifo(CModule* pModule,void* args);
	virtual int  SetAdcLag(CModule* pModule,void* args);
	virtual int  SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);

public:

	C8gcSrv(int idx, int srv_num, CModule* pModule, PGCSRV_CFG pCfg); // constructor

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
	int CtrlTarget(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlTestMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlGcSetReg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlGcGetReg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlGetAdcOver(
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
	int CtrlSetAdmMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetDdcMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetDdcSync(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlProgramDdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlProgramAdc(
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
    int  CtrlSpecific(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
    int  CtrlAdmGetInfo(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetDelay(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetTestSequence(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);

	int CtrlSetAdcLag(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlReadIniFile(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
};

#endif // _GC5016SRV_H

//
// End of file
//
