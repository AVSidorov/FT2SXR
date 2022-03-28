/*
 ****************** File TtlOutSrv.h *************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : TTLOUT section
 *
 * (C) InSys by Dorokhin A. Nov 2007
 *
 ************************************************************
*/

#ifndef _TTLOUTSRV_H
 #define _TTLOUTSRV_H

#include "ctrldio2in.h"
#include "ctrlttlout.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"

#define BASE_MAXREFS 2

#define TTLOUT_TETR_ID 0x45

#pragma pack(push,1)

typedef struct _TTLOUTSRV_CFG {
	UCHAR	isAlreadyInit;				// флаг инициализации (чтобы делать ее однократно)
	U32		BaseRefGen[BASE_MAXREFS];	// frequency of generators (значени€ опорных генераторов (√ц))
	U32		SysRefGen;					// frequency of system generator (частота системного генератора (√ц))
	U32		BaseExtClk;					// external clock frequency (внешн€€ частота тактировани€ (√ц))
	U32		FifoSize;					// размер FIFO (в байтах)
} TTLOUTSRV_CFG, *PTTLOUTSRV_CFG;

#pragma pack(pop)

class CTtlOutSrv: public CService
{

protected:

	long m_TtlOutTetrNum;
	long m_MainTetrNum;

	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetStartMode(CModule* pModule, PVOID pStartMode);
	virtual int GetStartMode(CModule* pModule, PVOID pStartMode);

public:

	CTtlOutSrv(int idx, int srv_num, CModule* pModule, PTTLOUTSRV_CFG pCfg); // constructor

	int CtrlIsAvailable(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
				   );
	int CtrlGetAddrData(
					void			*pDev,		// InfoSM or InfoBM
					void			*pServData,	// Specific Service Data
					ULONG			cmd,		// Command Code (from BRD_ctrl())
					void			*args 		// Command Arguments (from BRD_ctrl())
					);
	int CtrlMaster(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlRate(
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
	int CtrlSyncMode(
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
	int CtrlEnable(
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

};

#endif // _TTLINSRV_H

//
// End of file
//