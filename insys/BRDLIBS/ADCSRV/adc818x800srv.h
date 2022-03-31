/*
 ****************** File Adc818X800Srv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Dorokhin Andrey Jun 2007
 *
 ************************************************************
*/

#ifndef _ADC818X800SRV_H
 #define _ADC818X800SRV_H

#include "ctrladc818x800.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adc818x800regs.h"

#include "adc818x800srvinfo.h"

const int ADC818X800_TETR_ID = 0x29; // tetrad id

const int ADC818X800_THRDACCNT = 16; // Number of threshold DACs

//const double MAX_SINGLE_RATE = 50000.; // 50 kHz
//const double MAX_DOUBLE_RATE = 100000.; // 100 kHz
//
//typedef enum _MODE_RATE
//{
//	SINGLE_MODE_RATE,
//	DOUBLE_MODE_RATE,
//	QUAD_MODE_RATE
//} MODE_RATE;

#pragma pack(push,1)

typedef struct _ADC818X800SRV_CFG {
	ADCSRV_CFG AdcCfg;
	REAL64	Bias[MAX_ADC_CHAN];			// смещение нуля сигнала в каждом канале
} ADC818X800SRV_CFG, *PADC818X800SRV_CFG;

#pragma pack(pop)

class CAdc818X800Srv: public CAdcSrv
{

protected:

	double m_Range[BRD_GAINCNT];

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	int ResetAdc(CModule* pModule, ULONG mode);
//	int SyncPulse(CModule* pModule, double msPeriod);
//	int SetHpfOff(CModule* pModule, ULONG mode);
//	int GetHpfOff(CModule* pModule, ULONG& mode);
//	int SetSpeedMode(CModule* pModule, ULONG mode);
//	int GetSpeedMode(CModule* pModule, ULONG& mode);

    virtual int SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
    virtual int SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
	virtual int SetCode(CModule* pModule, ULONG type);
	virtual int GetCode(CModule* pModule, ULONG& type);
//	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
//	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
//	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
//	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int GetBias(CModule* pModule, double& Bias, ULONG Chan);
//	virtual int SetInpSrc(CModule* pModule, PVOID pCtrl);
//	virtual int GetInpSrc(CModule* pModule, PVOID pCtrl);
//	virtual int SetClkLocation(CModule* pModule, ULONG& mode);
//	virtual int GetClkLocation(CModule* pModule, ULONG& mode);
	virtual int SetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int GetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int SetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int GetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
//	virtual int SetTestMode(CModule* pModule, ULONG mode);
//	virtual int GetTestMode(CModule* pModule, ULONG& mode);
//	virtual int SelfClbr(CModule* pModule);
	virtual int SetWarp(CModule* pModule, PVOID pCtrl);
	virtual int GetWarp(CModule* pModule, PVOID pCtrl);
	
	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
	virtual int ExtraInit(CModule* pModule);
public:

	CAdc818X800Srv(int idx, int srv_num, CModule* pModule, PADC818X800SRV_CFG pCfg); // constructor

};

#endif // _ADC818X800SRV_H

//
// End of file
//
