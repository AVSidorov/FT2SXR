/*
 ****************** File Fm814x125mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Ekkore Aug 2011
 *
 ************************************************************
*/

#ifndef _FM814x125MSRV_H
 #define _FM814x125MSRV_H

#include "ctrlfm814x125m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "AdcFm814x125mRegs.h"

#include "AdcFm814x125msrvInfo.h"

const int FM814X125M_TETR_ID = 0x82; // tetrad id

// ADC210x1G Threshold DAC number
enum 
{
	BRDtdn_ADC210X1G_BIAS0		= 3,	// bias of channel 0
	BRDtdn_ADC210X1G_BIAS1		= 4,	// bias of channel 1
	BRDtdn_ADC210X1G_STARTCMP	= 7,	// start comparator threshold
};

#pragma pack(push,1)

typedef struct _FM814X125MSRV_Cfg 
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
	double	adRange[8];	// шкалы преобразования АЦП (В)
	double	dGenFxtal;	// частота кварца (Fxtal) внутреннего генератора (Гц)
	U32		SubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
	REAL64	aBias[MAX_ADC_CHAN];			// хранит последнее смещение нуля сигнала в каждом канале
	REAL64	StCmpThr;					// хранит последний порог срабатывания компаратора старта (ИПН9/ИПН10)
	REAL64	ExtClkThr;					// хранит последний порог внешнего такта (ИПН8)
} FM814X125MSRV_Cfg, *PFM814X125MSRV_Cfg;

#pragma pack(pop)

class CFm814x125mSrv: public CAdcSrv
{
public:

	CFm814x125mSrv(int idx, int srv_num, CModule* pModule, PFM814X125MSRV_Cfg pCfg); // constructor

protected:

	virtual int  CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
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
	virtual int SetDcCoupling(CModule* pModule, ULONG InpType, ULONG Chan);
	virtual int GetDcCoupling(CModule* pModule, ULONG& InpType, ULONG Chan);
	virtual int SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan);
	virtual int GetInpResist(CModule* pModule, ULONG& InpRes, ULONG Chan);
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
	int	SpdRead(  CModule* pModule, U32 spdType, U32 regAdr, U32 *pRegVal );
	int	SpdWrite( CModule* pModule, U32 spdType, U32 regAdr, U32 regVal );
	int	Si571SetRate( CModule* pModule, double *pRate );
	int	Si571GetRate( CModule* pModule, double *pRate );
	int	Si571CalcDiv( double dRate, double dFXtal, U32 *pRfreqLo, U32 *pRfreqHi, U32 *pHsdivCode, U32 *pN1Code );

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
	int SetMu(CModule* pModule, void *args);
	int GetMu(CModule* pModule, void *args);
};

#endif // _FM814x125MSRV_H

//
// End of file
//