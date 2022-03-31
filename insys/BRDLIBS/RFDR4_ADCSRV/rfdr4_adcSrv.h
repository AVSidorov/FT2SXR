/*
 ****************** File rfdr4_adcSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : stone service
 *
 * (C) InSys by Sklyarov A. Aug. 2014
 *
 **************************************************************
*/

#ifndef _RFDR4_ADC_H
 #define _RFDR4_ADC_H

#include "ctrlrfdr4_adc.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "rfdr4_adcRegs.h"


#define SINGLE_DIVIDER 512

const int RFDR4_ADC_TETR_ID = 0xBE; 

#pragma pack(push,1)

// Перечень устройств, управляемых по SPD
//
enum
{
	SPDdev_ADC		= 0x00,		// АЦП AD9467
	SPDdev_THDAC	= 0x01,		// кристалл ЦАП(ИПН) AD5541
	SPDdev_GEN		= 0x02,		// внутренний генератор (Si570/Si571)
};

typedef struct _RFDR4_ADCSRV_CFG {
	U32		isAlreadyInit;
	U32		FifoSize;


} RFDR4_ADCSRV_CFG, *PRFDR4_ADCSRV_CFG;

#pragma pack(pop)

class Crfdr4_adcSrv: public CService 
{

protected:

	long m_MainTetrNum;
	long m_Rfdr4_AdcTetrNum;
	U32	 m_SystemClock;

	virtual int IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, U32 *pVal );
	virtual int	IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, U32 val );
	virtual int DirRegRead( CModule* pModule, S32 tetrNo, S32 regNo, U32 *pVal );
	virtual int DirRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, U32 val );
	virtual int	SpdRead(  CModule* pModule, U32 spdType, U32 regAdr, U32 *pRegVal );
	virtual int	SpdWrite( CModule* pModule, U32 spdType, U32 regAdr, U32 regVal );
//-------------------------------------------------------------------------------------
	virtual int SetClock(CModule* pModule, void* args);
	virtual int SetChanMask(CModule* pModule, void* args);
	virtual int SetStartMode(CModule* pModule, void *args);
	virtual int InitAdc(CModule* pModule);
	virtual int SetBias( CModule *pModule,void* args );
	virtual int SetBufCur(CModule* pModule, void* args);
	virtual int SetGain( CModule *pModule,void* args);
	virtual int SetRf( CModule *pModule,void* args);
	virtual int ClrBitsOverflow(CModule* pModule, void* args);
	virtual int GetBitsOverflow(CModule* pModule, void* args);
	virtual int SetOutDelay(CModule* pModule, void* args);

//-------------------------------------------------------------------------------------
	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
//-------------------------------------------------------------------------------------
	virtual void GetAdcTetrNum(CModule* pModule);


public:

	Crfdr4_adcSrv(int idx, int srv_num, CModule* pModule, PRFDR4_ADCSRV_CFG pCfg); // constructor

	int CtrlSetClock(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSetChanMask(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSetStartMode (
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlInitAdc(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSetBias(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSetBufCur(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSetGain(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   );
	int CtrlSetRf(
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
	int CtrlCapture(
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
	int CtrlFifoStatus(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlGetBitsOverflow(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlClrBitsOverflow(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);
	int CtrlSetOutDelay(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
				);

};

#endif // _RFDR4_ADCSRV_H

//
// End of file
//
