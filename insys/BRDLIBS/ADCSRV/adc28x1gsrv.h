/*
 ****************** File Adc28x1gSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Dorokhin Andrey Jan 2007
 *
 ************************************************************
*/

#ifndef _ADC28X1GSRV_H
 #define _ADC28X1GSRV_H

#include "ctrladc28x1g.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adc28x1gregs.h"

#include "adc28x1gsrvinfo.h"

const int ADC28X1G_TETR_ID = 0x2F; // tetrad id

// ADC28x1G Threshold DAC number
enum {
	BRDtdn_ADC28X1G_BIAS0		= 3,	// bias of channel 0
	BRDtdn_ADC28X1G_BIAS1		= 4,	// bias of channel 1
	BRDtdn_ADC28X1G_STARTCMP	= 7,	// start cpmparator threshold
};

const double ADC28X1G_STARTTHREXT = 3.0; // 3.0 V

#pragma pack(push,1)

typedef struct _ADC28X1GSRV_CFG {
	ADCSRV_CFG AdcCfg;
	U32		SubExtClk;			// frequency of external clock (внешняя частота тактирования (Гц))
	REAL64	Bias[MAX_CHAN];		// смещение нуля сигнала в каждом канале
	REAL64	StCmpThr;			// порог срабатывания компаратора старта
	U32		PllRefGen;			// frequency of PLL reference generator (значение внутреннего опорного генератора для ФАПЧ (Гц))
//	U32		ExtPllRef;			// external frequency of PLL reference (значение внешней опорной частоты для ФАПЧ (Гц))
	U32		PllFreq;			// frequency of PLL (частота, выдаваемая ФАПЧ (Гц))
	U08		FracMode;			// 0 - лучше точность частоты, 1 - лучше качество сигнала
	U08		OutCurrent;			// выходной ток
	PLL_IF_R Pll_IF_R;
	PLL_RF_R Pll_RF_R;
	PLL_RF_N Pll_RF_N;
	U16		AdcReg[8];			// internal ADC registers 0-7
	REAL64	GainTun[MAX_CHAN];	// подстройка коэффициента передачи в каждом канале
} ADC28X1GSRV_CFG, *PADC28X1GSRV_CFG;

#pragma pack(pop)

class CAdc28x1gSrv: public CAdcSrv
{

protected:

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	//int GetGainMask(CModule* pModule, int chan);
//	int SetPllMode(CModule* pModule, PBRD_Adc10x2gPll pMode);

	virtual int SetProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	virtual int SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	virtual int SetCode(CModule* pModule, ULONG type);
//	virtual int GetCode(CModule* pModule, ULONG& type);
	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int GetBias(CModule* pModule, double& Bias, ULONG Chan);
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
	virtual int SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan);
	virtual int GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan);
	virtual int SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan);
	virtual int GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan);

	virtual int SetMaster(CModule* pModule, ULONG mode);
	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

	virtual int ExtraInit(CModule* pModule);
	int InitPLL(CModule* pModule);
	int OffPLL(CModule* pModule);
	int OnPll(CModule* pModule, double& ClkValue, double RefValue);
	int SetFold(CModule* pModule, int fold);

//	int readPllData(CModule* pModule, USHORT& HiWord, USHORT& LoWord);
	int writePllData(CModule* pModule, USHORT HiWord, USHORT LoWord);

	int SetAdcReg(CModule* pModule, int regNum, int regVal);

public:

	CAdc28x1gSrv(int idx, int srv_num, CModule* pModule, PADC28X1GSRV_CFG pCfg); // constructor

};

#endif // _ADC28X1GSRV_H

//
// End of file
//