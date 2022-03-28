/*
 ****************** File Dio2InSrv.h *************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : DIO2IN section
 *
 * (C) InSys by Dorokhin A. Nov 2007
 *
 ************************************************************
*/

#ifndef _DIO2INSRV_H
 #define _DIO2INSRV_H

#include "ctrldio2in.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"

#define BASE_MAXREFS 2
#define BRD_COMPARNUM 4
#define DIO2IN_TETR_ID 0x43
#define BRD_CNTNUM 4

#pragma pack(push,1)

typedef struct _DIO2INSRV_CFG {
	UCHAR	isAlreadyInit;				// флаг инициализации (чтобы делать ее однократно)
	U32		BaseRefGen[BASE_MAXREFS];	// frequency of generators (значени€ опорных генераторов (√ц))
	U32		SysRefGen;					// frequency of system generator (частота системного генератора (√ц))
	U32		BaseExtClk;					// external clock frequency (внешн€€ частота тактировани€ (√ц))
	U32		FifoSize;					// размер FIFO (в байтах)
	U32		RefPVS;						// Basic Voltage (опорное напр€жение источников программируемых напр€жений (м¬ольт))
	REAL64	Level[BRD_COMPARNUM];		//
	U32		Count[BRD_CNTNUM];		//
} DIO2INSRV_CFG, *PDIO2INSRV_CFG;

#pragma pack(pop)

class CDio2InSrv: public CService
{

protected:

	long m_Dio2InTetrNum;
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

	CDio2InSrv(int idx, int srv_num, CModule* pModule, PDIO2INSRV_CFG pCfg); // constructor

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
	int CtrlComparLevel(
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

	int CtrlCounter(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);

	int CtrlSymbSync(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

};

#endif // _DIO2INSRV_H

//
// End of file
//