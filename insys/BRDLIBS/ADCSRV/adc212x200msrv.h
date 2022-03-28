/*
 ****************** File Adc212x200mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Dorokhin Andrey Apr 2004
 *
 ************************************************************
*/

#ifndef _ADC212X200MSRV_H
 #define _ADC212X200MSRV_H

#include "ctrladc212x200m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adc212x200mregs.h"

#include "adc212x200msrvinfo.h"

//#define BASE_MAXREFS 2
//const int BASE_MAXREFS = 2; // Number of referens generators on base module

const int ADC212X200M_TETR_ID = 0x14; // tetrad id

//const int ADC212x200M_THRDACCNT = 8; // Number of threshold DACs

// ADC212x200M Threshold DAC number
enum {
	BRDtdn_ADC212X200M_BIAS0		= 3,	// bias of channel 0
	BRDtdn_ADC212X200M_BIAS1		= 4,	// bias of channel 1
	BRDtdn_ADC212X200M_CLKPHASE0	= 5,	// clock phase control of channel 0
	BRDtdn_ADC212X200M_CLKPHASE1	= 6,	// clock phase control of channel 1
	BRDtdn_ADC212X200M_STARTCMP		= 7,	// start cpmparator threshold
	BRDtdn_ADC212X200M_GAINTUN0		= 8,	// gain tuning of channel 0
};

//const double ADC212x200M_STARTTHR = 1.5; // 1.5 V

//const double ADC212x200M_STARTTHREXT = 1.5; // 1.5 V
//const double ADC212x200M_STARTTHRCHAN = 0.85; // 0.85 V
const double ADC212X200M_STARTCHANKOEF = 1.8; // 1.8 V

const int ADC212X200M_STARTCHANMAXCODE = 174;
const int ADC212X200M_STARTCHANMINCODE = 82;
const int ADC212X200M_STARTEXTMAXCODE = 179;//204;
const int ADC212X200M_STARTEXTMINCODE = 77;//52;

#pragma pack(push,1)

typedef struct _ADC212X200MSRV_CFG {
	ADCSRV_CFG AdcCfg;
	U32		SubRefGen;					// frequency of generators (значения опорных генераторов (Гц))
	U32		SubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
	REAL64	Bias[MAX_CHAN];			// смещение нуля сигнала в каждом канале
	REAL64	ClkPhase[MAX_CHAN];		// подстройка фазы такта сигнала в каждом канале
	REAL64	StCmpThr;					// порог срабатывания компаратора старта
	REAL64	GainTun;					// подстройка коэффициента передачи в канале 0
} ADC212X200MSRV_CFG, *PADC212X200MSRV_CFG;

#pragma pack(pop)

class CAdc212x200mSrv: public CAdcSrv
//class CAdc212x200mSrv: public CService
{

protected:

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	int GetGainMask(CModule* pModule, int chan);

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
	virtual int SetClkPhase(CModule* pModule, double& Phase, ULONG Chan);
	virtual int GetClkPhase(CModule* pModule, double& Phase, ULONG Chan);
	virtual int SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan);
	virtual int GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan);
	virtual int SetStartMode(CModule* pModule, PVOID pStartMode);
	virtual int GetStartMode(CModule* pModule, PVOID pStartMode);
	virtual int SetInpSrc(CModule* pModule, PVOID pCtrl);
	virtual int GetInpSrc(CModule* pModule, PVOID pCtrl);
	virtual int SetDblClk(CModule* pModule, ULONG& mode);
	virtual int GetDblClk(CModule* pModule, ULONG& mode);
	virtual int SetClkLocation(CModule* pModule, ULONG& mode);
	virtual int GetClkLocation(CModule* pModule, ULONG& mode);
	virtual int SetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int GetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int SetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int GetInpRange(CModule* pModule, double& InpRange, ULONG Chan);

	virtual int SetMaster(CModule* pModule, ULONG mode);

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

//	inline int SetDblClk(CModule* pModule, ULONG mode) { return SetSrcClk(pModule, mode);};
//	inline int GetDblClk(CModule* pModule, ULONG& mode) { return GetSrcClk(pModule, mode);};
public:

	CAdc212x200mSrv(int idx, int srv_num, CModule* pModule, PADC212X200MSRV_CFG pCfg); // constructor

};

#endif // _ADC212X200MSRV_H

//
// End of file
//