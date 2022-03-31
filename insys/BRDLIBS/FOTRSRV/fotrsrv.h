/******************* File FotrSrv.h *************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : FOTR section
 *
 * (C) InSys by Dorokhin Andrey Jun, 2006
 *
 * - support ADMFOTR3G
 *
 ************************************************************/

#ifndef _FOTRSRV_H
 #define _FOTRSRV_H

#include "ctrlfotr.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "fotrregs.h"

#define FOTR_TETR_ID		0x2E	// ADMFOTR2G
#define FOTR3G_TETR_ID		0x49	// ADMFOTR3G
#define DATA32IN_TETR_ID	0x34	// DATA32_IN
#define DATA32OUT_TETR_ID	0x35	// DATA32_OUT
#define DATA64IN_TETR_ID	0x13	// DATA64_IN
#define DATA64OUT_TETR_ID	0x12	// DATA64_OUT

#define RIO_CTRL_TETR_ID		0x57
#define RIO_MSG_TETR_ID			0x54
#define RIO_DATA64IN_TETR_ID	0x56
#define RIO_DATA64OUT_TETR_ID	0x55

#include "si571.h"

#define FM401S_TETR_ID		0x8A	// FM401S
#define FM402S_TETR_ID		0xA8	// FM402S

#define REGISTER_DEVID 1
#define GENERATOR_DEVID 2

#define REGISTER_NUMB 0x22
//#define GENERATOR_NUMB 0x55

#define DLGDLLNAME_SIZE 64

const U32 TRANS_TAG = 0xCD4594AA;

#pragma pack(push,1)

typedef struct _FOTRSRV_CFG {
	BRDCHAR	DlgDllName[DLGDLLNAME_SIZE];// название диалоговой dll
	U08		isAlreadyInit;				// флаг инициализации (чтобы делать ее однократно)
	U08		Res[3];						// резерв
	U32		RefGen;						// frequency of generator (значение опорного генератора (√ц))
	U08		MaxChan;					// максимальное число каналов
	U08		TrancieverType;				// тип приемо-передатчика
	U32		DataInFifoSize;				// размер DATA32_IN FIFO (в байтах)
	U32		DataOutFifoSize;			// размер DATA32_OUT FIFO (в байтах)
	U08		GenType;					// тип внутреннего генератора (0-непрограммируемый, 1-Si571)
	U32		RefMaxGen;					// максимальна€ частота внутр. генератора (√ц)
	U08		AdrGen;						// адресный код внутр. генератора: 0x49 по умолчанию
	REAL64	dGenFxtal;					// частота кварца (Fxtal) внутреннего генератора Si570/571 (√ц)
	U32		FreqGen;					// frequency of generator (желаема€ частота (√ц))
//	U08		flProgGen;					// флаг программировани€ генератора
} FOTRSRV_CFG, *PFOTRSRV_CFG;

// FOTR Command
typedef union _FOTR_CMD {
	ULONG AsWhole; // FOTR Status Register as a Whole Word
	struct { // Status Register as Bit Pattern
		ULONG	PacketNum		: 8, // Number of packet
				PacketSize		: 8, // Size of packet
				ReadCmd			: 1, // 1 - read command
				WriteCmd		: 1, // 1 - write command
				ReplyCmd		: 1, // 1 - reply of command
				RetSig			: 1, // 1 - return of signature
				Reserved		: 4, // Reserved
				BusNum			: 4, // Number of Bus
				TypeCmd			: 4; // type of command (1 - fixed address, 2 - serial address, 3 - specifed address)
	} ByBits;
} FOTR_CMD, *PFOTR_CMD;

#pragma pack(pop)

class CFotrSrv: public CService
{

protected:

	long m_MainTetrNum;
	long m_FotrTetrNum;
	long m_DataInTetrNum;
	long m_DataOutTetrNum;
	long m_RioCtrlTetrNum;
	long m_InitTetrNum;

	int m_ModuleType; // 0 - FOTR2G, 1 - FOTR3G, 2 - Rapid IO
	int ReadSfpEeprom(CModule* pModule, ULONG idrom, ULONG addr, UCHAR& data, ULONG timeout);
	int WriteSfpEeprom(CModule* pModule, ULONG idrom, ULONG addr, UCHAR data, ULONG timeout);
	int Sync3G(CModule* pModule, ULONG timeoutcnt);
	int InitRio(CModule* pModule, ULONG timeoutcnt);

	int writeSpdDev(CModule* pModule, ULONG dev, ULONG num, ULONG synchr, ULONG reg, ULONG val);
	int readSpdDev(CModule* pModule, ULONG dev, ULONG num, ULONG synchr, ULONG reg, ULONG* pVal);
	//int	Si571SetClkVal( CModule* pModule, double *pRate );
	int	Si571SetClkVal( CModule* pModule );
	int Sync401S(CModule* pModule, ULONG timeoutcnt);


public:

	CFotrSrv(int idx, int srv_num, CModule* pModule, PFOTRSRV_CFG pCfg); // constructor

	int CtrlIsAvailable(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
				   );
	int CtrlReset(
					void			*pDev,		// InfoSM or InfoBM
					void			*pServData,	// Specific Service Data
					ULONG			cmd,		// Command Code (from BRD_ctrl())
					void			*args 		// Command Arguments (from BRD_ctrl())
					);
	int CtrlGetAddrData(
					void			*pDev,		// InfoSM or InfoBM
					void			*pServData,	// Specific Service Data
					ULONG			cmd,		// Command Code (from BRD_ctrl())
					void			*args 		// Command Arguments (from BRD_ctrl())
					);
	int CtrlMode(
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
	int CtrlCfg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlMsgFifoRst(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSync(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlDisconnect(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlConnect(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlCheckConnect(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSendMsg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlReceiveMsg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlWriteRmReg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlReadRmReg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlWriteRmRegs(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlReadRmRegs(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlFifoAddr(
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
	int CtrlPutData(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
};

#endif // _FOTRSRV_H

//
// End of file
//