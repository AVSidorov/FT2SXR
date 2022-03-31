/*
 ****************** File dr16srv.h ***************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : DR16 section
 *
 * (C) InSys by Sklyarov A. Dec 2009
 *
 ************************************************************
*/

#ifndef _DR16_SRV_H
 #define _DR16_SRV_H

#include "ctrldr16.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "dr16regs.h"

const int DR16_CONTROL_TETR_ID = 0x6C; 

#pragma pack(push,1)

typedef struct _DR16SRV_CFG {
	UCHAR	isAlreadyInit;
	UCHAR	Res[2];
	UCHAR	DdsType;
	double	BaseRefClk;
	double	ExtRefClk;
} DR16SRV_CFG, *PDR16SRV_CFG;

#pragma pack(pop)

class CDr16Srv: public CService
{

protected:

	HINSTANCE m_hLib;
	
	long m_Dr16TetrNum;
	long m_MainTetrNum;

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual int SetClkMode(CModule* pModule, PBRD_ClkModeDr16 pClkMode);
	virtual int SetStartMode(CModule* pModule, PBRD_StartModeDr16 pStMode);
	virtual int SetTpMode(CModule* pModule, PBRD_TpModeDr16 pTpMode);
	virtual int RestartDiv(CModule* pModule);
	virtual int ResetJtag(CModule* pModule);
	virtual int UnresetJtag(CModule* pModule);
public:

	CDr16Srv(int idx, int srv_num, CModule* pModule, PDR16SRV_CFG pCfg); // constructor

	int CtrlClkMode(
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
	int CtrlTpMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlRestartDiv(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlResetJtag(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlUnresetJtag(
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
	int CtrlGetStatus(
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
	int  CtrlFifoReset (
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);


};
#endif // _DR16_SRV_H

//
// End of file
//