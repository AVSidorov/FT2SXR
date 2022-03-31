/*
 ****************** File Adc214x400mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Ekkore Aug 2008
 *
 ************************************************************
*/

#ifndef _ADC214X400MSRV_H
 #define _ADC214X400MSRV_H

#include "ctrladc214x400m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adc214x400mregs.h"

#include "adc214x400msrvinfo.h"

const int ADC214X400M_TETR_ID = 0x5d; // tetrad id

// ADC214x400M Threshold DAC number
enum {
	BRDtdn_ADC214X400M_GAINTUN0		= 1,	// gain tuning of channel 0
	BRDtdn_ADC214X400M_GAINTUN1		= 2,	// gain tuning of channel 1
	BRDtdn_ADC214X400M_BIAS0		= 3,	// bias of channel 0
	BRDtdn_ADC214X400M_BIAS1		= 4,	// bias of channel 1
	BRDtdn_ADC214X400M_PRECBIAS0	= 5,	// precision bias of channel 0
	BRDtdn_ADC214X400M_PRECBIAS1	= 6,	// precision bias of channel 1
	BRDtdn_ADC214X400M_STARTCMP		= 7,	// start comparator threshold

	//
	// WB HF
	//
	BRDtdn_ADC214X400M_GAIN0		= 3,	// gain of channel 0
	BRDtdn_ADC214X400M_GAIN1		= 4,	// gain of channel 1
	BRDtdn_ADC214X400M_PRECGAIN0	= 5,	// precision gain of channel 0
	BRDtdn_ADC214X400M_PRECGAIN1	= 6,	// precision gain of channel 1
};
/*
const double ADC214X400M_STARTCHANKOEF = 1.8; // 1.8 V

const int ADC214X400M_STARTCHANMAXCODE = 174;
const int ADC214X400M_STARTCHANMINCODE = 82;
const int ADC214X400M_STARTEXTMAXCODE = 179;//204;
const int ADC214X400M_STARTEXTMINCODE = 77;//52;
*/
#pragma pack(push,1)

typedef struct _ADC214X400MSRV_Cfg 
{
	ADCSRV_CFG AdcCfg;
	U32		aRefGen[2];					// опорные генераторы 0 и 1 (Гц)
	U32		SubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
	REAL64	aBias[MAX_CHAN];			// смещение нуля сигнала в каждом канале
	REAL64	StCmpThr;					// порог срабатывания компаратора старта
	REAL64	aGainDb[MAX_CHAN];			// коэффициенты передачи для WB (LF и HF) в каждом канале (дБ)
	REAL64	aGainTun[MAX_CHAN];			// подстройки коэффициентов передачи в каждом канале
	U32		nVersion;					// версия субмодуля
	U32		nSubType;					// тип субмодуля: 214x400M: 0x000-Standard, 0x001 -WB HF, 0x002-WB LF 
										//            или 212x500M: 0x100-Standard, 0x101 -WB HF, 0x102-WB LF
	double	adIcrRange[4];				// шкалы преобразования для Standard (мВ) (default 10000, 2000, 500, 100)
	S16	awRangeDeviation[2][2][2][4];	// отклонение ШП от номинала (разы) (default 10000)
										// [тип входа][Rвх/ФНЧ][номер АЦП][номер ШП]
	S16 awBiasDeviation[2][2][2][4];	// отклонения смещения нуля (разы) (default 0)
										// [тип входа][Rвх/ФНЧ][номер АЦП][номер ШП]
} ADC214X400MSRV_Cfg, *PADC214X400MSRV_Cfg;

#pragma pack(pop)

class CAdc214x400mSrv: public CAdcSrv
{
public:

	CAdc214x400mSrv(int idx, int srv_num, CModule* pModule, PADC214X400MSRV_Cfg pCfg); // constructor

protected:

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	virtual int SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
	virtual int SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
	virtual int GetCfg(PBRD_AdcCfg pCfg);
	virtual int SetCode(CModule* pModule, ULONG type);
	virtual int GetCode(CModule* pModule, ULONG& type);
	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetInpSrc(CModule* pModule, PVOID pCtrl);
	virtual int GetInpSrc(CModule* pModule, PVOID pCtrl);
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
	virtual int SetCnt(CModule* pModule, ULONG numreg, PBRD_EnVal pEnVal);
	virtual int GetCnt(CModule* pModule, ULONG numreg, PBRD_EnVal pEnVal);

	virtual int SetMaster(CModule* pModule, ULONG mode);

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

//	virtual int ExtraInit(CModule* pModule);

	virtual int SetClkInv(CModule* pModule, ULONG Inv);
	virtual int GetClkInv(CModule* pModule, ULONG& Inv);

	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
	int SetMu(CModule* pModule, void	*args);
	int GetMu(CModule* pModule, void	*args);
	int SetStartClkSl(CModule* pModule, void	*args);
	int GetStartClkSl(CModule* pModule, void	*args);
	int GetGainWB(CModule* pModule, double& refGain, ULONG Chan);
	int SmSetMode(CModule* pModule, void	*args);
	int SmLoadData(CModule* pModule, void	*args);
	int SmIsAvailable(CModule* pModule, void	*args);

//	S32	IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, U32 *pVal );
//	S32	IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, ULONG *pVal );
//	S32	IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, U32 val );
//	S32	IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, ULONG val );
};

#endif // _ADC214X400MSRV_H

//
// End of file
//