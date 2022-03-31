/*
 ****************** File Dio16InSrv.h *************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : DIO16_IN section
 *
 * (C) InSys by Dorokhin Andrey Feb, 2006
 *
 ************************************************************
*/

#ifndef _DIO16INSRV_H
 #define _DIO16INSRV_H

#include "ctrldio16in.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "dio16inregs.h"
#include "fpgaregs.h"

#define BASE_MAXREFS 2
#define PLD_MAXDATASIZE 256
#define PLD_HEADERSIZE 9

#define DIO16IN_TETR_ID 0x1B
#define ADMFPGA_TETR_ID 0x20

#pragma pack(push,1)

typedef struct _DIO16INSRV_CFG {
	UCHAR	isAlreadyInit;				// флаг инициализации (чтобы делать ее однократно)
	UCHAR	Res[3];						// резерв
	U32		ExtClk;						// external clock frequency (внешн€€ частота тактировани€ (√ц))
	U32		FifoSize;					// размер FIFO (в байтах)
} DIO16INSRV_CFG, *PDIO16INSRV_CFG;

typedef struct _PLD_RECORD {
  UCHAR  len;
  USHORT addr;
  UCHAR  type;
  UCHAR  data[PLD_MAXDATASIZE];
} PLD_RECORD, *PPLD_RECORD;

#pragma pack(pop)

typedef enum _PldRecordTypes {
  PLD_rectDATA,
  PLD_rectEND
} PldRecordTypes;

#define PLD_MAXRECORDSIZE (sizeof(PLD_RECORD) + 4)

class CDio16InSrv: public CService
{

protected:

	long m_Dio16InTetrNum;
	long m_MainTetrNum;
	long m_AdmFPGATetrNum;

	ULONG ReadPldFile(PCTSTR PldFileName, LPVOID& fileBuffer, DWORD& fileSize);
	ULONG PldGetHexRecord(PUCHAR& pPldBuf, PLD_RECORD *rec);
	ULONG LoadHexFile(CModule* pModule, BYTE* pldDataBuf);

public:

	CDio16InSrv(int idx, int srv_num, CModule* pModule, PDIO16INSRV_CFG pCfg); // constructor

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
	int CtrlPhase(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSingleMode(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSingleWrite(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSingleRead(
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
	int CtrlAddrData(
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

	int CtrlFpgaLoad(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlFpgaCfg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
};

#endif // _DIO16INSRV_H

//
// End of file
//