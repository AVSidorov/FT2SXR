/*
 ****************** File Fm214x250mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Ekkore Nov 2011
 *
 ************************************************************
*/

#ifndef _FM214x250MSRV_H
 #define _FM214x250MSRV_H

#include "ctrlfm214x250m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "AdcFm214x250mRegs.h"
#include "gipcy.h"

#include "AdcFm214x250msrvInfo.h"
#include "si571.h"
#include "ad9518.h"

const int FM214X250M_TETR_ID = 0x88; // tetrad id

// ADC210x1G Threshold DAC number
enum 
{
	BRDtdn_ADC210X1G_BIAS0		= 2,	// bias of channel 0
	BRDtdn_ADC210X1G_BIAS1		= 3,	// bias of channel 1
	BRDtdn_ADC210X1G_STARTCMP	= 1,	// start comparator threshold
};

//
// Перечень устройств, управляемых по SPD
//
enum
{
	SPDdev_ADC0		= 0x00,		// кристалл АЦП0 (ADS41B49)
	SPDdev_ADC1		= 0x10,		// кристалл АЦП1 (ADS41B49)
	SPDdev_THDAC	= 0x01,		// кристалл ЦАП(ИПН) AD57x4
	SPDdev_GEN		= 0x02,		// внутренний генератор (Si570/Si571)
	SPDdev_SYNT		= 0x03,		// синтезатор AD9518
	SPDdev_ATT0		= 0x04,		// аттенюатор канала 0 (DAT-31R5-SN)
	SPDdev_ATT1		= 0x14		// аттенюатор канала 1 (DAT-31R5-SN)
};

#pragma pack(push,1)

typedef struct _FM214X250MSRV_Cfg 
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
	U32	nSyntType;	// тип кристалла синтезатора: 0-отсутствует, 1-AD9518
	U32	nAttType;	// тип кристалла аттенюатора: 0-отсутствует, 1-DAT-31R5
	
	double	dGenFxtal;		// частота кварца (Fxtal) внутреннего генератора (Гц)
	U32		nSubExtClk;		// хранит внешнюю частоту тактирования от CLKIN (Гц))
	U32		nBaseUnitClk;	// хранит частоту от базового модуля (Гц))
	REAL64	aBias[MAX_ADC_CHAN];		// хранит последнее смещение нуля сигнала в каждом канале
	REAL64	adGainDb[MAX_ADC_CHAN];		// хранит последний коэфф. передачи в каждом канале
	REAL64	dStCmpThr;					// хранит последний порог срабатывания компаратора старта (ИПН9/ИПН10)
	REAL64	dExtClkThr;					// хранит последний порог внешнего такта (ИПН8)
	U32		nClkSrc;		// хранит источник тактовой частоты: 0-выкл., 1-внутренний, 2-внутренний+PLL, 4-от базмодуля, 0x81-внешний, 0x82-внешний+PLL
	REAL64	dSamplingRate;	// хранит частоту дискретизации, установленную функцией SetRate()

	S16	awRangeDeviation[2][2][2][4];	// отклонение ШП от номинала (разы) (default 10000)
										// [тип входа][Rвх/ФНЧ][номер АЦП][номер ШП]
	S16 awBiasDeviation[2][2][2][4];	// отклонения смещения нуля (разы) (default 0)
										// [тип входа][Rвх/ФНЧ][номер АЦП][номер ШП]
} FM214X250MSRV_Cfg, *PFM214X250MSRV_Cfg;

#pragma pack(pop)

class CFm214x250mSrv: public CAdcSrv
{
public:

	CFm214x250mSrv(int idx, int srv_num, CModule* pModule, PFM214X250MSRV_Cfg pCfg); // constructor

protected:

	virtual int  CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual void *GetInfoForDialog(CModule* pModule);
	virtual int  SetPropertyFromDialog(void	*pDev, void	*args);

	virtual int SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
	virtual int SaveProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);

	virtual int GetCfg(PBRD_AdcCfg pCfg);
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
	//int	Si571CalcDiv( double dRate, double dFXtal, U32 *pRfreqLo, U32 *pRfreqHi, U32 *pHsdivCode, U32 *pN1Code );

	int	Ad9518SetDividerMode( CModule* pModule, double *pClk, double *pRate );
	int	Ad9518SetPllMode( CModule* pModule, double *pClk, double *pRate );
	int	Ad9518SetDirectPllMode( CModule* pModule, AD9518_TDividers *pDividers );

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

//	virtual int ExtraInit(CModule* pModule);

	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
	int SetMu(CModule* pModule, void *args);
	int GetMu(CModule* pModule, void *args);
};

#endif // _FM214x250MSRV_H

//
// End of file
//
