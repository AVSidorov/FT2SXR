/*
 ****************** File Adc214x200mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Dorokhin Andrey Jan 2007
 *
 ************************************************************
*/

#ifndef _ADC214X200MSRV_H
 #define _ADC214X200MSRV_H

#include "ctrladc214x200m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adc214x200mregs.h"

#include "adc214x200msrvinfo.h"

const int ADC214X200M_TETR_ID = 0x3F; // tetrad id

// ADC214x200M Threshold DAC number
enum {
	BRDtdn_ADC214X200M_GAINTUN0		= 1,	// gain tuning of channel 0
	BRDtdn_ADC214X200M_GAINTUN1		= 2,	// gain tuning of channel 1
	BRDtdn_ADC214X200M_BIAS0		= 3,	// bias of channel 0
	BRDtdn_ADC214X200M_BIAS1		= 4,	// bias of channel 1
	BRDtdn_ADC214X200M_PRECBIAS0	= 5,	// precision bias of channel 0
	BRDtdn_ADC214X200M_PRECBIAS1	= 6,	// precision bias of channel 1
	BRDtdn_ADC214X200M_STARTCMP		= 7,	// start comparator threshold
};
/*
const double ADC214X200M_STARTCHANKOEF = 1.8; // 1.8 V

const int ADC214X200M_STARTCHANMAXCODE = 174;
const int ADC214X200M_STARTCHANMINCODE = 82;
const int ADC214X200M_STARTEXTMAXCODE = 179;//204;
const int ADC214X200M_STARTEXTMINCODE = 77;//52;
*/
#pragma pack(push,1)

typedef struct _ADC214X200MSRV_CFG {
	ADCSRV_CFG AdcCfg;
	U32		PllRefGen;					// frequency of PLL reference generator (значение внутреннего опорного генератора для ФАПЧ (Гц))
//	U32		ExtPllRef;					// external frequency of PLL reference (значение внешней опорной частоты для ФАПЧ (Гц))
	U32		PllFreq;					// frequency of PLL (частота, выдаваемая ФАПЧ (Гц))
	U32		SubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
	REAL64	Bias[MAX_CHAN];				// смещение нуля сигнала в каждом канале
	REAL64	StCmpThr;					// порог срабатывания компаратора старта
	REAL64	GainTun[MAX_CHAN];			// подстройка коэффициента передачи в каждом канале
	U08		PllMuxOut;					// 
	PLL_RCNT Pll_R_CNT;
	PLL_NCNT Pll_N_CNT;
	PLL_FUNC Pll_Func;
	U08		AdcReg[16];					// internal ADC registers
	U08		Version;					// версия субмодуля
	U08		IsRF;						// 1 - радиочастотный канал
	U08		AdcRegGain;					// value GAIN for internal ADC register for ADM214x250M
	U08		AdcRegBias;					// value BIAS for internal ADC register for ADM214x250M
} ADC214X200MSRV_CFG, *PADC214X200MSRV_CFG;

#pragma pack(pop)

class CAdc214x200mSrv: public CAdcSrv
{

protected:

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

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
	virtual int SetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan);
	virtual int GetGainTuning(CModule* pModule, double& GainTuning, ULONG Chan);
	virtual int SetStartMode(CModule* pModule, PVOID pStartMode);
	virtual int GetStartMode(CModule* pModule, PVOID pStartMode);
	virtual int SetClkLocation(CModule* pModule, ULONG& mode);
	virtual int GetClkLocation(CModule* pModule, ULONG& mode);
	virtual int SetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int GetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int SetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int GetInpRange(CModule* pModule, double& InpRange, ULONG Chan);

	virtual int SetDcCoupling(CModule* pModule, ULONG InpType, ULONG Chan);
	virtual int SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan);
	virtual int GetCfg(PBRD_AdcCfg pCfg);

	int SetGainRF(CModule* pModule, double& Gain, ULONG Chan);
	int GetGainRF(CModule* pModule, double& Gain, ULONG Chan);

	virtual int SetMaster(CModule* pModule, ULONG mode);
	virtual int SetChanMask(CModule* pModule, ULONG mask);

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

	virtual int ExtraInit(CModule* pModule);
	int InitPLL(CModule* pModule);
//	int OffPLL(CModule* pModule);
	int OnPll(CModule* pModule, double& ClkValue, double RefValue);
	int SetMuxOut(CModule* pModule, U08 muxout);

	virtual int ClrBitsOverflow(CModule* pModule, ULONG flags);
	virtual int IsBitsOverflow(CModule* pModule, ULONG& OverBits);

//	int readPllData(CModule* pModule, USHORT& HiWord, USHORT& LoWord);
	int writePllData(CModule* pModule, USHORT HiWord, USHORT LoWord);

	int SetAdcReg(CModule* pModule, int regNum, UCHAR regVal);
	int SetAdcRegVal(CModule* pModule, int adcNum, int regNum, UCHAR val);

	virtual int SetClkInv(CModule* pModule, ULONG Inv);
	virtual int GetClkInv(CModule* pModule, ULONG& Inv);

	int SetMu(CModule* pModule, void	*args);
	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
public:

	CAdc214x200mSrv(int idx, int srv_num, CModule* pModule, PADC214X200MSRV_CFG pCfg); // constructor

};

#endif // _ADC214X200MSRV_H

//
// End of file
//