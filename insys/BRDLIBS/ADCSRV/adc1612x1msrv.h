/*
 ****************** File Adc1612x1mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Ekkore Nov 2008
 *
 ************************************************************
*/

#ifndef _ADC1612x1MSRV_H
 #define _ADC1612x1MSRV_H

#include "CtrlAdc1612x1m.h"
#include "CtrlCmpSc.h"
#include "Service.h"
#include "Adm2If.h"
#include "AdcSrv.h"
#include "MainRegs.h"
#include "Adc1612x1mRegs.h"

#include "Adc1612x1mSrvInfo.h"

const int ADC1612x1M_TETR_ID = 0x1F; // tetrad id

// ADC1612x1M Threshold DAC number
enum {
// THDACs aren't used

//	BRDtdn_ADC1612x1M_BIAS0		= 3,	// bias of channel 0
//	BRDtdn_ADC1612x1M_BIAS1		= 4,	// bias of channel 1
};

#pragma pack(push,1)

typedef struct _ADC1612x1MSRV_Cfg 
{
	ADCSRV_CFG AdcCfg;
	U32		nVersion;					// версия субмодуля
} ADC1612x1MSRV_Cfg, *PADC1612x1MSRV_Cfg;

#pragma pack(pop)

class CAdc1612x1mSrv: public CAdcSrv
{
public:

	CAdc1612x1mSrv(int idx, int srv_num, CModule* pModule, PADC1612x1MSRV_Cfg pCfg); // constructor

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
//	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
//	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
//	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
//	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
//	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
//	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
//	virtual int SetBias(CModule* pModule, double& Bias, ULONG Chan);
//	virtual int GetBias(CModule* pModule, double& Bias, ULONG Chan);
//	virtual int SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan);
//	virtual int GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan);
//	virtual int SetStartMode(CModule* pModule, PVOID pStartMode);
//	virtual int GetStartMode(CModule* pModule, PVOID pStartMode);
//	virtual int SetClkLocation(CModule* pModule, ULONG& mode);
//	virtual int GetClkLocation(CModule* pModule, ULONG& mode);
	virtual int SetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int GetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int SetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int GetInpRange(CModule* pModule, double& InpRange, ULONG Chan);

	virtual int SetMaster(CModule* pModule, ULONG mode);
	virtual int ExtraInit(CModule* pModule);

//	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
//	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

//	virtual int SetClkInv(CModule* pModule, ULONG Inv);
//	virtual int GetClkInv(CModule* pModule, ULONG& Inv);
};

#endif // _ADC1612x1MSRV_H

//
// End of file
//