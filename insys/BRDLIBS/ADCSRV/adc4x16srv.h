/*
 ****************** File Adc4x16Srv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Dorokhin Andrey Jan 2006
 *
 ************************************************************
*/

#ifndef _ADC4x16SRV_H
 #define _ADC4x16SRV_H

#include "ctrladc4x16.h"
#include "adcsrv.h"
#include "ddc4x16srv.h"
#include "adc4x16srvinfo.h"

const int ADC4x16_TETR_ID = 0x16; // ADC tetrad id
//const int DDC4x16_TETR_ID = 0x07; // DDC tetrad id

//const double ADC28X800M_STARTTHREXT = 2.5; // 2.5 V
//const double ADC28X800M_STARTTHRCHAN = 0.25; // 0.25 V
//const double ADC28X800M_STARTCHANKOEF = 0.25; // 0.25 V

// ADC4x16 Threshold DAC number
//enum {
//	BRDtdn_ADC4x16_BIAS0	= 3,	// bias of channel 0
//	BRDtdn_ADC4x16_BIAS1	= 4,	// bias of channel 1
//	BRDtdn_ADC4x16_GAINTUN0	= 5,	// clock phase control of channel 0
//	BRDtdn_ADC4x16_GAINTUN1	= 6,	// clock phase control of channel 1
//	BRDtdn_ADC4x16_STARTCMP	= 7,	// start comparator threshold
//};

#pragma pack(push,1)

typedef struct _ADC4X16SRV_CFG {
	ADCSRV_CFG AdcCfg;
	U32		SubRefGen;				// frequency of submodule reference generator (значение субмодульного опорного генератора (√ц))
	U32		SubExtClk;				// frequency of external clock (субмодульна€ внешн€€ частота тактировани€ (√ц))
} ADC4X16SRV_CFG, *PADC4X16SRV_CFG;

#pragma pack(pop)

class CAdc4x16Srv: public CAdcSrv
{

protected:

	long m_DdcTetrNum;

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	virtual int SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
	virtual int SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int GetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan);
	virtual int GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan);
	virtual int SetDcCoupling(CModule* pModule, ULONG InpType, ULONG Chan);
	virtual int GetDcCoupling(CModule* pModule, ULONG& InpType, ULONG Chan);
	virtual int SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan);
	virtual int GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan);

	virtual int FifoReset(CModule* pModule);
	virtual int ClrBitsOverflow(CModule* pModule, ULONG flags);
	virtual int IsBitsOverflow(CModule* pModule, ULONG& OverBits);

public:

	CAdc4x16Srv(int idx, int srv_num, CModule* pModule, PADC4X16SRV_CFG pCfg); // constructor

};

#endif // _ADC4x16SRV_H

//
// End of file
//