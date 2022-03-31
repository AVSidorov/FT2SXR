/*
 ****************** File DspNodeSrv.h *************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : DSP Node section
 *
 * (C) InSys by Dorokhin Andrey Jun, 2005
 *
 ************************************************************
*/

#ifndef _DSPNODESRV_H
 #define _DSPNODESRV_H

#include "ctrldspnode.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "dspnoderegs.h"

#define DLGDLLNAME_SIZE 64

const int OUT_TETR_ID = 0x12; // tetrad id
const int IN_TETR_ID = 0x13; // tetrad id
const int SBSRAM_TETR_ID = 0x27; // tetrad id

#pragma pack(push,1)

typedef struct _DSPNODESRV_CFG {
	TCHAR	DlgDllName[DLGDLLNAME_SIZE];	// название диалоговой dll
	UCHAR	isAlreadyInit;					// флаг инициализации (чтобы делать ее однократно)
	UCHAR	Res[3];							// резерв
	U32		InFifoSize;						// размер FIFO тетрады IN (в байтах)
	U32		OutFifoSize;					// размер FIFO тетрады OUT (в байтах)
	UCHAR	SramChips;						// количество микросхем статической памяти
	UCHAR	SramChipSize;					// размер (глубина) микросхемы статической памяти
	UCHAR	SramChipWidth;					// ширина (в битах) микросхемы статической памяти
} DSPNODESRV_CFG, *PDSPNODESRV_CFG;

#pragma pack(pop)

class CDspNodeSrv : public CService
{

protected:

	long m_MainTetrNum;
	long m_OutTetrNum;
	long m_InTetrNum;
	long m_SbsramTetrNum;
	
	ULONG m_dwPhysMemSize; // physical memory size on device in 32-bit words

	ULONG GetFifoSize(CModule* pModule, ULONG tetrNum);
	void TetradInit(CModule* pModule, ULONG tetrNum);

	virtual int GetCfg(PBRD_DspNodeCfg pCfg);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	int SetMode(CModule* pModule, ULONG mode);
	int GetMode(CModule* pModule, ULONG& mode);

public:

	CDspNodeSrv(int idx, int srv_num, CModule* pModule, PDSPNODESRV_CFG pCfg); // constructor

	int CtrlIsAvailable(
					void		*pSub,		// InfoSM or InfoBM
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
	int CtrlCfg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlMode(
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
	int CtrlSetData(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);
	int CtrlReadData(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					);
	int CtrlWriteData(
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
};

#endif // _DSPNODESRV_H

//
// End of file
//