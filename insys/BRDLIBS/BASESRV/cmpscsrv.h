/*
 ****************** File CmpScSrv.h *************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : CmpSc section (start & clock comparators)
 *
 * (C) InSys by Dorokhin Andrey Feb, 2004
 *
 ************************************************************
*/

#ifndef _CMPSCSRV_H
 #define _CMPSCSRV_H

#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"

#include "cmpscsrvinfo.h"

#define DLGDLLNAME_SIZE 64

#pragma pack(push,1)

typedef struct _CMPSCSRV_CFG {
	BRDCHAR	DlgDllName[DLGDLLNAME_SIZE];// название диалоговой dll
	UCHAR	isAlreadyInit;				// флаг инициализации (чтобы делать ее однократно)
	U32		RefPVS;					// Basic Voltage (опорное напряжение источников программируемых напряжений (мВольт))
	REAL64	Threshold[BRD_CMPNUM];		//
	U08		ComparBits;					// разрядность компараторов
} CMPSCSRV_CFG, *PCMPSCSRV_CFG;

#pragma pack(pop)

class CCmpScSrv: public CService
{

protected:

	long m_MainTetrNum;
//	long m_TetradNum;

	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

public:

//	CCmp12Srv(int idx, int srv_num, PHOST_REGISTERS pRegs, PCMP12SRV_CFG pCfg); // constructor
	CCmpScSrv(int idx, int srv_num, CModule* pModule, PCMPSCSRV_CFG pCfg); // constructor
//	~CCmp12Srv() { delete m_pConfig; };

	int CtrlIsAvailable(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
				   );
	int CtrlGetPages(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);
	int CtrlSet(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlGet(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetExt(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlGetExt(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlCfg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlDacReg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
};

#endif // _CMPSCSRV_H

//
// End of file
//