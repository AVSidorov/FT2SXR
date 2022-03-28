/*
 ****************** File Dsp201Srv.h *************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : 
 *
 * (C) InSys by Dorokhin S. May 2010
 *
 ************************************************************
*/

#ifndef _DSP201SRV_H
 #define _DSP201SRV_H

#include "ctrldsp.h"
#include "service.h"
#include "adm2if.h"

const int MSG_TETR_ID = 0x71; // tetrad id
const int TS201_TETR_ID = 0x72; // tetrad id


#pragma pack(push,1)

typedef struct _DSP201SRV_CFG {
	UCHAR	isAlreadyInit;		// флаг инициализации (чтобы делать ее однократно)
	UCHAR	Res[3];				// reserved
} DSP201SRV_CFG, *PDSP201SRV_CFG;

#pragma pack(pop)

class CDsp201Srv: public CService
{

protected:
	S32	ProcReset(U32 node);
//	S32	MsgInit(U32 node);
	S32	ProcStart(U32 node);
	S32 ProcDSPINT(U32 node);
	S32 ProcGrabIrq(U32 node, U32 *pSigCounter, U32 timeout);
	S32	ProcWaitIrq(U32 node, U32 *pSigCounter, U32 timeout );
	S32 ProcFreshIrq( U32 node, U32 *pSigCounter );
	S32	ProcSignalIack(U32 node);
	S32	DevReadMem( U32 node, U32 adr, U32 *dst, S32 sz );
	S32	DevWriteMem(U32 node , U32 adr, U32 src, S32 sz );
	S32	PeekDPRAM(U32 node, U32 offset, U32 *dst );
	S32	PokeDPRAM(U32 node, U32 offset, U32 src );
	S32	ReadDPRAM(U32 node, U32 offset, U32 *pSrc, U32 len );
	S32	WriteDPRAM(U32 node, U32 offset, U32 *pSrc, U32 len );
	S32	ProcLoad(U32 node, const char *filename, int argc, char *argv[] );
	S32	LinkBootLoad( ULONG node, const char *fname);
    S32	SendBootData(LONG node, void *swbuf);
	S32	LinkBootLoading(U32 cpuID, U32 node, int buffer, char *fname, U32 syscon, U32 sdrcon);
	S32	ReadFIFO(U32 node, U32 adr, U32 *pDst, U32 size, U32 timeout);
	S32	WriteFIFO(U32 node, U32 adr, U32 *pSrc, U32 size, U32 timeout);
	S32	PutMsg( U32 node, void *msg, U32 size, U32 timeout);
	S32	GetMsg(U32 node,  void *msg, U32 *size, U32 timeout);

public:

	S32	MsgInit(U32 node);

    LONG m_MainTetrNum;
    LONG m_MsgTetrNum;
    LONG m_DspTetrNum;

	PULONG m_BaseAdrOper;	// виртуальный базовый адрес пространства блоков устройства
	PULONG m_BaseAdrTetr;	// виртуальный базовый адрес пространства тетрад устройства

	// DSP201 data parameters
	U32			*pPexMemoryVA;			// Space PEX Memory Virt Adr
	U32			*pDevMemoryVA;			// Space Dev Memory Virt Adr
	SINT32		irq;					// IRQ line number 0-15 

	U32			argsAdr,  argsSize;		// .args Segment Adr and Size 

	SINT32		errcode; 		// Driver Last Error Number 
	SINT32		errcnt;	 		// Driver Error Counter 

	//	USER MEMBERS
	U32			BOOT;			// BOOT Mode
    U32		    SYSCONREG;      // syscon register value
    U32			SDRCONREG;      // sdrcon register value
	char		pldName[8][256];	// file name for PLD data config
	U32			pldVol[8];
	U32			pldSpeed[8];
	U32			pldPins[8];
	U08			eeprom[512];
	U32			OPENRESET;
    //  MODULE CONSTANTS
    U32         BLOCK_ID;       // Control block identifier
    U32         BLOCK_VER;      // Control block version
    U32         DeviceID;       // Module identifier
    U32         REV_MOD;        // Module version and modification
    U32         PLD_VER;        // Host PLD version
    U32         BLOCK_CNT;      // Control block counter
    U32         DPRAM_OFFSET;   // Start address for DPRAM
    U32         DPRAM_SIZE;     // Size of DPRAM
	//////////////////
	U32			EEPROM_ACCESS;   // флаг разрешения полного доступа к EEPROM (1 - полный доступ)

	U32			m_isMsgInit;

	CDsp201Srv(int idx, int srv_num, CModule* pModule, PDSP201SRV_CFG pCfg); // constructor

	void	SetAdr(PULONG adrOper, PULONG adrTetr);

	int CtrlIsAvailable(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
				   );
	int CtrlCapture(
					void			*pDev,		// InfoSM or InfoBM
					void			*pServData,	// Specific Service Data
					ULONG			cmd,		// Command Code (from BRD_ctrl())
					void			*args 		// Command Arguments (from BRD_ctrl())
					);
	int CtrlReadDir(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlReadsDir(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlReadInd(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlReadsInd(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlWriteDir(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlWritesDir(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlWriteInd(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlWritesInd(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSetStatIrq(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlClearStatIrq(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

	int CtrlProcReset(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	
	int CtrlProcStart(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

	int CtrlProcLoad(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

	int CtrlGetMsg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

	int CtrlPutMsg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

	int CtrlPeekDpram(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

	int CtrlPokeDpram(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

	int CtrlReadDpram(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

	int CtrlWriteDpram(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

	int CtrlExtension(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

	int CtrlSignalWait(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

	int CtrlSignalSend(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );

};

#endif // _DSP201SRV_H

//
// End of file
//
