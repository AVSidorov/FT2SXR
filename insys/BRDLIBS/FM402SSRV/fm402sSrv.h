#ifndef _FM402SSRV_H
#define _FM402SSRV_H

#include "service.h"
#include "adm2if.h"
#include "fm402sRegs.h"
#include "ctrlfm402s.h"
#include "si571.h"

#define FM402S_TETR_ID 0xA8

#pragma pack(push,1)
typedef struct _FM402SSRV_CFG {
	UCHAR isAlreadyInit;
	U08 bSfpCnt;	//количество SFP
	U08 bGen1Type;	//тип генератора 0 - непрограммируемый, 1 - Si571
	U08 bGen1Addr;	//адрес генератора
	U32 nGen1Ref;	//заводска€ установка √ц
	U32 nGen1RefMax;//максимальна€ частота √ц
	U32 nGen1Rate;	//частота дл€ установки √ц
	double dGenFxtal;// частота кварца (Fxtal) внутреннего генератора (√ц)
} FM402SSRV_CFG, *PFM402SSRV_CFG;

#pragma pack(pop)

class CFm402sSrv : public CService
{
protected:
	long m_Fm402sTetrNum;
	long m_MainTetrNum;
	long m_SrvNum;

	int ReadErrData16(void* pDev, U08 nChan, U32 nReg, U32& nVal);
	int ReadErrData32(void* pDev, U08 nChan, U32 nReg, U32& nVal);
	int ReadErrData64(void* pDev, U08 nChan, U32 nReg, unsigned long long& nVal);
public:

	CFm402sSrv(int idx, int srv_num, CModule* pModule, PFM402SSRV_CFG pCfg);

	int CtrlIsAvailable(
		void* pDev,		// InfoSM or InfoBM
		void* pServData,	// Specific Service Data
		ULONG cmd,		// Command Code (from BRD_ctrl())
		void* args 		// Command Arguments (from BRD_ctrl())
	);

	int CtrlChanMask(
		void* pDev,		// InfoSM or InfoBM
		void* pServData,	// Specific Service Data
		ULONG cmd,		// Command Code (from BRD_ctrl())
		void* args 		// Command Arguments (from BRD_ctrl())
	);

	int CtrlClkMode(
		void* pDev,		// InfoSM or InfoBM
		void* pServData,	// Specific Service Data
		ULONG cmd,		// Command Code (from BRD_ctrl())
		void* args 		// Command Arguments (from BRD_ctrl())
	);

	int CtrlStatus(
		void* pDev,		// InfoSM or InfoBM
		void* pServData,	// Specific Service Data
		ULONG cmd,		// Command Code (from BRD_ctrl())
		void* args 		// Command Arguments (from BRD_ctrl())
	);

	int CtrlTestMode(
		void* pDev,		// InfoSM or InfoBM
		void* pServData,	// Specific Service Data
		ULONG cmd,		// Command Code (from BRD_ctrl())
		void* args 		// Command Arguments (from BRD_ctrl())
	);

	int CtrlStart(
		void* pDev,		// InfoSM or InfoBM
		void* pServData,	// Specific Service Data
		ULONG cmd,		// Command Code (from BRD_ctrl())
		void* args 		// Command Arguments (from BRD_ctrl())
	);

	int CtrlTestResult(
		void* pDev,		// InfoSM or InfoBM
		void* pServData,	// Specific Service Data
		ULONG cmd,		// Command Code (from BRD_ctrl())
		void* args 		// Command Arguments (from BRD_ctrl())
	);

	int CtrlDir(
		void* pDev,		// InfoSM or InfoBM
		void* pServData,	// Specific Service Data
		ULONG cmd,		// Command Code (from BRD_ctrl())
		void* args 		// Command Arguments (from BRD_ctrl())
	);

	int CtrlRxTx(
		void* pDev,		// InfoSM or InfoBM
		void* pServData,	// Specific Service Data
		ULONG cmd,		// Command Code (from BRD_ctrl())
		void* args 		// Command Arguments (from BRD_ctrl())
	);

	int CtrlPrepare(
		void* pDev,		// InfoSM or InfoBM
		void* pServData,	// Specific Service Data
		ULONG cmd,		// Command Code (from BRD_ctrl())
		void* args 		// Command Arguments (from BRD_ctrl())
	);

	virtual int CtrlRelease(
		void* pDev,		// InfoSM or InfoBM
		void* pServData,	// Specific Service Data
		ULONG		cmd,		// Command Code (from BRD_ctrl())
		void* args 		// Command Arguments (from BRD_ctrl())
	);

	virtual void SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
	virtual int SetRate(CModule* pModule, double& Rate, double ClkValue);
	int	SpdRead(CModule* pModule, U32 spdType, U32 regAdr, U32* pRegVal);
	int	SpdWrite(CModule* pModule, U32 spdType, U32 regAdr, U32 regVal);
	int	Si571SetRate(CModule* pModule, double* pRate);
	int	Si571GetRate(CModule* pModule, double* pRate);
	int Si571GetFxTal(CModule* pModule, double* dGenFxTal);
};

#endif //_FM402SSRV_H