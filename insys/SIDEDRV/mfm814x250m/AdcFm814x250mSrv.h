/*
 ****************** File Fm814x250mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Ekkore Aug 2011
 *
 ************************************************************
*/

#ifndef _FM814x250MSRV_H
 #define _FM814x250MSRV_H

#include "ctrlfm814x250m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "AdcFm814x250mRegs.h"

#include "AdcFm814x250msrvInfo.h"
#include "si571.h"

const int FM814X250M_TETR_ID = 0xAC; // tetrad id

// ADC210x1G Threshold DAC number
enum 
{
	BRDtdn_ADC814x250M_BIAS0		= 0,	// bias of channel 0
	BRDtdn_ADC814x250M_BIAS1		= 1,	// bias of channel 1
	BRDtdn_ADC814x250M_BIAS2		= 2,	// bias of channel 2
	BRDtdn_ADC814x250M_BIAS3		= 3,	// bias of channel 3
	BRDtdn_ADC814x250M_BIAS4		= 4,	// bias of channel 4
	BRDtdn_ADC814x250M_BIAS5		= 5,	// bias of channel 5
	BRDtdn_ADC814x250M_BIAS6		= 6,	// bias of channel 6
	BRDtdn_ADC814x250M_BIAS7		= 7,	// bias of channel 7
	BRDtdn_ADC814x250M_GENTUNE		= 8,	// Si571 Generator Tuning
	BRDtdn_ADC814x250M_CLKOUTPUT	= 9,	// 
	BRDtdn_ADC814x250M_CLKPOWER		= 10,	// 
	BRDtdn_ADC814x250M_CH0CMP	= 11,	// start comparator threshold
	BRDtdn_ADC814x250M_STARTCMP	= 12,	// start comparator threshold
};

//
// Перечень устройств, управляемых по SPD
//
enum
{
	SPDdev_THDAC0	= 0x00,		// кристалл ЦАП0(ИПН0) AD5628/AD5648/AD5668
	SPDdev_THDAC1	= 0x01,		// кристалл ЦАП1(ИПН1) AD5628/AD5648/AD5668
	SPDdev_GEN		= 0x02,		// внутренний генератор (Si570/Si571)
	SPDdev_ADC0		= 0x03,		// кристалл АЦП0 (AD9643)
	SPDdev_ADC1		= 0x04,		// кристалл АЦП1 (AD9643)
	SPDdev_ADC2		= 0x05,		// кристалл АЦП2 (AD9643)
	SPDdev_ADC3		= 0x06,		// кристалл АЦП3 (AD9643)
};

#define		THDAC_CNT		16

#pragma pack(push,1)

typedef struct _FM814X250MSRV_Cfg 
{
	ADCSRV_CFG AdcCfg;

	U32	nVersion;	// версия субмодуля
	U32	nAdcType;	// тип кристалла АЦП: 0-всегда
	U32	nDacType;	// тип кристалла ЦАП: 0-всегда
	U32	nDacRange;	// шкала преобразования ЦАП: (мВ)
	U32	nGenType;	// тип кристалла внутр. генератора: 0-не програм-ый, 1-Si571
	U32	nGenAdr;	// адресный код внутр. генератора: 0x49 по умолчанию
	U32	nGenRef;    // заводская установка частоты внутр. генератора (Гц)
	U32	nGenRefMax; // максимальная частота внутр. генератора (Гц)
	U32	nInpType;	// тип входа: 0-закр., 1-откр.
	//double	adRange[8];	// шкалы преобразования АЦП (В)
	double	dGenFxtal;	// частота кварца (Fxtal) внутреннего генератора (Гц)
	U32		nSubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
	U32		nBaseUnitClk;	// хранит частоту от базового модуля (Гц))
	REAL64	aBias[MAX_ADC_CHAN];		// хранит последнее смещение нуля сигнала в каждом канале
	REAL64	aThdacVotage[THDAC_CNT];		// хранит значения ИПН0-ИПН16 (В)

	REAL64	ExtClkThr;
	REAL64	StartThdac11;			// порог старта от CH0 
	REAL64	StartThdac12;			// порог старта от EXTSTART 
} FM814X250MSRV_Cfg, *PFM814X250MSRV_Cfg;

#pragma pack(pop)

class CFm814x250mSrv: public CAdcSrv
{
public:

	CFm814x250mSrv(int idx, int srv_num, CModule* pModule, PFM814X250MSRV_Cfg pCfg); // constructor

protected:

	virtual int  CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void GetAdcTetrNumEx(CModule* pModule, ULONG TetrInstNum);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual void *GetInfoForDialog(CModule* pModule);
	virtual int  SetPropertyFromDialog(void	*pDev, void	*args);

	virtual int SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
	virtual int SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);

	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int SetClkLocation(CModule* pModule, ULONG& mode);
	virtual int GetClkLocation(CModule* pModule, ULONG& mode);
	virtual int SetExtClkThr(CModule* pModule, double& ExtClkThr);
	virtual int GetExtClkThr(CModule* pModule, double& ExtClkThr);
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int GetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int SetStartMode(CModule* pModule, PVOID pStartMode);
	virtual int GetStartMode(CModule* pModule, PVOID pStartMode);
	virtual int SetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int GetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int SetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int GetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int SetCode(CModule* pModule, ULONG type);
	virtual int GetCode(CModule* pModule, ULONG& type);

	virtual int SetMaster(CModule* pModule, ULONG mode);
	virtual int SetChanMask(CModule* pModule, ULONG mask);
	virtual int ExtraInit(CModule* pModule);
	int	SetThdac( CModule* pModule, int chan, double voltage );
	int	GetThdac( CModule* pModule, int chan, double *pVoltage );
	int	SpdRead(  CModule* pModule, U32 spdType, U32 regAdr, U32 *pRegVal );
	int	SpdWrite( CModule* pModule, U32 spdType, U32 regAdr, U32 regVal );
	int	Si571SetRate( CModule* pModule, double *pRate );
	int	Si571GetRate( CModule* pModule, double *pRate );

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
	int SetMu(CModule* pModule, void *args);
	int GetMu(CModule* pModule, void *args);
};

#endif // _FM814x250MSRV_H

//
// End of file
//