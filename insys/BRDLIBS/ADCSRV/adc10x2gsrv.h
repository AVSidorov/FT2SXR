/*
 ****************** File Adc10x2gSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Dorokhin Andrey Jun 2005
 *
 ************************************************************
*/

#ifndef _ADC10X2GSRV_H
 #define _ADC10X2GSRV_H

#include "ctrladc10x2g.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adc10x2gregs.h"

#include "adc10x2gsrvinfo.h"

const int ADC10X2G_TETR_ID = 0x2B; // tetrad id

const double ADC10X2G_STARTTHREXT = 5.0; // 5.0 V
//const double ADC10X2G_STARTTHRCHAN = 0.25; // 0.25 V
//const int ADC10X2G_STARTCHANMAXCODE = 140;
//const int ADC10X2G_STARTCHANMINCODE = 116;
//const int ADC10X2G_STARTEXTMAXCODE = 255;
//const int ADC10X2G_STARTEXTMINCODE = 0;

// ADC10X2G Threshold DAC number
enum {
	BRDtdn_ADC10X2G_BIAS		= 3,	// bias of channel
	BRDtdn_ADC10X2G_PRECBIAS	= 4,	// precision bias of channel
	BRDtdn_ADC10X2G_CLKPHASE	= 5,	// clock phase control
	BRDtdn_ADC10X2G_STARTCMP	= 7,	// start comparator threshold
	BRDtdn_ADC10X2G_GAINTUN		= 8,	// gain tuning
};

#pragma pack(push,1)

typedef struct _ADC10X2GSRV_CFG {
	ADCSRV_CFG AdcCfg;
	U32		SubExtClk;				// frequency of external clock (внешняя частота тактирования (Гц))
//	U32		AuxSubExtClk;			// frequency of auxiliary external clock (внешняя частота тактирования (Гц))
	REAL64	Bias;					// смещение нуля сигнала в канале
	REAL64	ClkPhase;				// подстройка фазы тактового сигнала
	REAL64	GainTun;				// подстройка коэффициента передачи в канале
	REAL64	StCmpThr;				// порог срабатывания компаратора старта
	U32		PllRefGen;				// frequency of PLL reference generator (значение внутреннего опорного генератора для ФАПЧ (Гц))
	U32		ExtPllRef;				// external frequency of PLL reference (значение внешней опорноq частоты для ФАПЧ (Гц))
	U32		PllFreq;				// frequency of PLL (частота, выдаваемая ФАПЧ (Гц))
	U08		FracMode;				// (v. 2.0) 0 - лучше точность частоты, 1 - лучше качество сигнала
	U08		OutCurrent;				// (v. 2.0) выходной ток
	PLL_IF_R Pll_IF_R;				// (v. 2.0)
	PLL_RF_R Pll_RF_R;				// (v. 2.0)
	PLL_RF_N Pll_RF_N;				// (v. 2.0)
	U08		Version;				// версия модуля
	U08		PllMuxOut;				// (v. 2.1) 
	PLL_RCNT Pll_R_CNT;				// (v. 2.1)
	PLL_NCNT Pll_N_CNT;				// (v. 2.1)
	PLL_FUNC Pll_Func;				// (v. 2.1)
} ADC10X2GSRV_CFG, *PADC10X2GSRV_CFG;

#pragma pack(pop)

class CAdc10x2gSrv: public CAdcSrv
{

protected:

	long m_TetrModif;
	long m_TetrVer;
	
	ULONG m_MasterMode;

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	int GetGainMask(CModule* pModule, int chan);
	int SetPllMode(CModule* pModule, PBRD_Adc10x2gPll pMode);

	virtual int SetProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	virtual int SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	virtual int SetMaster(CModule* pModule, ULONG mode);
	virtual int GetMaster(CModule* pModule, ULONG& mode);
	//virtual int SetClkMode(CModule* pModule, PVOID pClkMode);
	//virtual int GetClkMode(CModule* pModule, PVOID pClkMode);
	//virtual int SetSyncMode(CModule* pModule, PVOID pSyncMode);
	//virtual int GetSyncMode(CModule* pModule, PVOID pSyncMode);
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
	virtual int SetPretrigMode(CModule* pModule, PBRD_PretrigMode pPreMode);
	virtual int GetPretrigMode(CModule* pModule, PBRD_PretrigMode pPreMode);
	virtual int GetPretrigEvent(CModule* pModule, ULONG& EventStart);
	virtual int SetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int GetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int SetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int GetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int SetTestMode(CModule* pModule, ULONG mode);
	virtual int GetTestMode(CModule* pModule, ULONG& mode);
	virtual int SetClkLocation(CModule* pModule, ULONG& mode);
	virtual int GetClkLocation(CModule* pModule, ULONG& mode);

	int SetSpecParam(CModule* pModule, ULONG StartSl, ULONG ClkSl, ULONG TimerSync);
	int GetSpecParam(CModule* pModule, ULONG& StartSl, ULONG& ClkSl, ULONG& TimerSync);

	int SetAccumulator(CModule* pModule, ULONG AccEn, ULONG AccCnt);

	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
	//virtual int GetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
	virtual int SetCnt(CModule* pModule, ULONG numreg, PBRD_EnVal pEnVal);
	virtual int GetCnt(CModule* pModule, ULONG numreg, PBRD_EnVal pEnVal);

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

	virtual int ExtraInit(CModule* pModule);
	int CheckCfg(CModule* pModule);
	int InitPLL(CModule* pModule);
	int OffPLL(CModule* pModule);
	int OnPll(CModule* pModule, double& ClkValue, double RefValue);
	int OnPll_v21(CModule* pModule, double& ClkValue, double PllRefValue);
	int SetFold(CModule* pModule, int fold);
	int SetMuxOut(CModule* pModule, U08 muxout); // v. 2.1

//	int readPllData(CModule* pModule, USHORT& HiWord, USHORT& LoWord);
	int writePllData(CModule* pModule, USHORT HiWord, USHORT LoWord);

public:

	CAdc10x2gSrv(int idx, int srv_num, CModule* pModule, PADC10X2GSRV_CFG pCfg); // constructor

};

#endif // _ADC10X2GSRV_H

//
// End of file
//