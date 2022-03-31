/*
 ****************** File Adc216x100mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Dorokhin Andrey Jan 2007
 *
 ************************************************************
*/

#ifndef _ADC216X100MSRV_H
 #define _ADC216X100MSRV_H

#include "ctrladc216x100m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adc216x100mregs.h"

#include "adc216x100msrvinfo.h"

const int ADC216X100M_TETR_ID = 0x40; // tetrad id

// ADC216x100M Threshold DAC number
enum {
	BRDtdn_ADC216X100M_BIAS0		= 3,	// bias of channel 0
	BRDtdn_ADC216X100M_BIAS1		= 4,	// bias of channel 1
	BRDtdn_ADC216X100M_THRCLK		= 5,	// threshold of clock
	BRDtdn_ADC216X100M_PRECBIAS0	= 7,	// precision bias of channel 0
	BRDtdn_ADC216X100M_PRECBIAS1	= 8,	// precision bias of channel 1
};

#pragma pack(push,1)

typedef struct _ADC216X100MSRV_CFG {
	ADCSRV_CFG AdcCfg;
	U32		SubRefGen[3];				// frequency of generators (значения опорных генераторов (Гц))
	U32		SubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
	REAL64	Bias[MAX_CHAN];				// смещение нуля сигнала в каждом канале
	REAL64	ThrClk;						// порог для сигнала тактирования
} ADC216X100MSRV_CFG, *PADC216X100MSRV_CFG;

#pragma pack(pop)

class CAdc216x100mSrv: public CAdcSrv
{

protected:

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	int SetDither(CModule* pModule, ULONG dither);
	int GetDither(CModule* pModule, ULONG& dither);

	virtual int SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
	virtual int SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
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
	virtual int SetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int GetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int SetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int GetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int SetClkLocation(CModule* pModule, ULONG& mode);
	virtual int GetClkLocation(CModule* pModule, ULONG& mode);

	virtual int SetMaster(CModule* pModule, ULONG mode);

	int SetMu(CModule* pModule, void	*args);
	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);

public:

	CAdc216x100mSrv(int idx, int srv_num, CModule* pModule, PADC216X100MSRV_CFG pCfg); // constructor

};

#endif // _ADC216X100MSRV_H

//
// End of file
//