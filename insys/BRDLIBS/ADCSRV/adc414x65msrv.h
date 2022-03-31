/*
 ****************** File Adc414x65mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Dorokhin Andrey Aug 2006
 *
 ************************************************************
*/

#ifndef _ADC414X65MSRV_H
 #define _ADC414X65MSRV_H

#include "ctrladc414x65m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adc414x65mregs.h"

#include "adc414x65msrvinfo.h"

const int ADC414X65M_TETR_ID = 0x19; // tetrad id

// ADC414X65M Threshold DAC number
enum {
	BRDtdn_ADC414X65M_BIAS0		= 3,	// bias of channel 0
	BRDtdn_ADC414X65M_BIAS1		= 4,	// bias of channel 1
	BRDtdn_ADC414X65M_THRCLK	= 5,	// threshold of clock
	BRDtdn_ADC414X65M_THREXTCLK	= 6,	// threshold of external clock
	BRDtdn_ADC414X65M_BIAS2		= 7,	// bias of channel 2
	BRDtdn_ADC414X65M_BIAS3		= 8,	// bias of channel 3
};

#pragma pack(push,1)

typedef struct _ADC414X65MSRV_CFG {
	ADCSRV_CFG AdcCfg;
	U32		SubRefGen[3];				// frequency of generators (значения опорных генераторов (Гц))
	U32		SubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
//	REAL64	Range[MAX_ADC_CHAN];		// входной диапазон для каждого канала
	REAL64	Bias[MAX_ADC_CHAN];			// смещение нуля сигнала в каждом канале
	REAL64	ThrClk;						// порог для сигнала тактирования
	REAL64	ThrExtClk;					// порог для сигнала внешнего тактирования
} ADC414X65MSRV_CFG, *PADC414X65MSRV_CFG;

#pragma pack(pop)

class CAdc414x65mSrv: public CAdcSrv
{

protected:

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	virtual int SetProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	virtual int SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	virtual int SetCode(CModule* pModule, ULONG type);
	virtual int GetCode(CModule* pModule, ULONG& type);
	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int GetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int SetClkThr(CModule* pModule, double& Thr);
	virtual int GetClkThr(CModule* pModule, double& Thr);
	virtual int SetExtClkThr(CModule* pModule, double& Thr);
	virtual int GetExtClkThr(CModule* pModule, double& Thr);
	virtual int SetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int GetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int SetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int GetInpRange(CModule* pModule, double& InpRange, ULONG Chan);

public:

	CAdc414x65mSrv(int idx, int srv_num, CModule* pModule, PADC414X65MSRV_CFG pCfg); // constructor

};

#endif // _ADC414X65MSRV_H

//
// End of file
//