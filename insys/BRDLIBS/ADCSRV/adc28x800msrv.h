/*
 ****************** File Adc28x800mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Dorokhin Andrey Dec 2004
 *
 ************************************************************
*/

#ifndef _ADC28X800MSRV_H
 #define _ADC28X800MSRV_H

#include "ctrladc28x800m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adc28x800mregs.h"

#include "adc28x800msrvinfo.h"

//#define BASE_MAXREFS 2
//const int BASE_MAXREFS = 2; // Number of referens generators on base module

const int ADC28X800M_TETR_ID = 0x23; // tetrad id

//const double ADC28X800M_STARTTHREXT = 2.5; // 2.5 V
//const double ADC28X800M_STARTTHRCHAN = 0.25; // 0.25 V
const double ADC28X800M_STARTCHANKOEF = 0.25; // 0.25 V

const int ADC28X800M_STARTCHANMAXCODE = 140;
const int ADC28X800M_STARTCHANMINCODE = 116;
const int ADC28X800M_STARTEXTMAXCODE = 255;
const int ADC28X800M_STARTEXTMINCODE = 0;

// ADC28x800M Threshold DAC number
enum {
	BRDtdn_ADC28x800M_BIAS0		= 3,	// bias of channel 0
	BRDtdn_ADC28x800M_BIAS1		= 4,	// bias of channel 1
	BRDtdn_ADC28x800M_GAINTUN0	= 5,	// clock phase control of channel 0
	BRDtdn_ADC28x800M_GAINTUN1	= 6,	// clock phase control of channel 1
	BRDtdn_ADC28x800M_STARTCMP	= 7,	// start comparator threshold
};

#pragma pack(push,1)

typedef struct _ADC28X800MSRV_CFG {
	ADCSRV_CFG AdcCfg;
	U32		SubMainGen;				// frequency of main generator (значение основного опорного генератора (√ц))
	USHORT	DacMainGen;				// DAC frequency trim for main generator (значение ÷јѕ подстройки частоты дл€ основного генератора (√ц))
	U32		MaxVcoGenFreq;			// maximum frequency of auxiliary generator (максимальное значение частоты дополнительного перестраиваемого генератора (√ц))
	U32		MinVcoGenFreq;			// minimum frequency of auxiliary generator (минимальное значение частоты дополнительного перестраиваемого генератора (√ц))
	U32		SubVcoClk;				// frequency of auxiliary generator (значение частоты дополнительного перестраиваемого генератора (√ц))
	U32		SubExtClk;				// frequency of external clock (внешн€€ частота тактировани€ (√ц))
//	REAL64	Bias[BRD_CHANCNT];		// смещение нул€ сигнала в каждом канале
//	REAL64	GainTun[BRD_CHANCNT];	// подстройка коэффициента передачи в каждом канале
	REAL64	StCmpThr;				// порог срабатывани€ компаратора старта
} ADC28X800MSRV_CFG, *PADC28X800MSRV_CFG;

#pragma pack(pop)

class CAdc28x800mSrv: public CAdcSrv
//class CAdc28x800mSrv: public CService
{

protected:

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	int GetGainMask(CModule* pModule, int chan);

	int SetVcoClkValue(CModule* pModule, double& ClkValue);

	virtual int SetProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	virtual int SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetStartMode(CModule* pModule, PVOID pStartMode);
	virtual int GetStartMode(CModule* pModule, PVOID pStartMode);
	virtual int SetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int GetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int SetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int GetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int SetClkLocation(CModule* pModule, ULONG& mode);
	virtual int GetClkLocation(CModule* pModule, ULONG& mode);
	virtual int SetClkThr(CModule* pModule, double& ClkThr); // SetClkTuning
	virtual int GetClkThr(CModule* pModule, double& ClkThr); // GetClkTuning
	virtual int SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan);
	virtual int GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan);

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

	int SetClkTuning(CModule* pModule, double& ClkTuning) {return SetClkThr(pModule, ClkTuning);};
	int GetClkTuning(CModule* pModule, double& ClkTuning) {return GetClkThr(pModule, ClkTuning);};

public:

	CAdc28x800mSrv(int idx, int srv_num, CModule* pModule, PADC28X800MSRV_CFG pCfg); // constructor

};

#endif // _ADC28X800MSRV_H

//
// End of file
//