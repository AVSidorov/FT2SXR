/*
 ****************** File Fm412x500mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Ekkore Feb 2012
 *
 ************************************************************
*/

#ifndef _FM412x500MSRV_H
 #define _FM412x500MSRV_H

#include "ctrlfm412x500m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adcfm412x500mregs.h"

//#include "AdcFm412x500mSrvInfo.h"
#include "si571.h"
#include "ad9518.h"

const int FM412X500M_TETR_ID = 0x89; // tetrad id
S32 Sub_Printf(void* ptr, S32 errorCode, const BRDCHAR *title, const BRDCHAR *format, ...);

// ADC210x1G Threshold DAC number
//enum 
//{
//	BRDtdn_ADC210X1G_BIAS0		= 3,	// bias of channel 0
//	BRDtdn_ADC210X1G_BIAS1		= 4,	// bias of channel 1
//	BRDtdn_ADC210X1G_STARTCMP	= 7,	// start comparator threshold
//};

//
// Перечень устройств, управляемых по SPD
//
enum
{
	SPDdev_THDAC	= 0x01,		// кристалл ЦАП(ИПН) AD5541
	SPDdev_GEN		= 0x02,		// внутренний генератор (Si570/Si571)
	SPDdev_SYNT		= 0x03		// синтезатор AD9518
};

#pragma pack(push,1)

typedef struct _FM412X500MSRV_Cfg 
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
	
	REAL64	dGenFxtal;		// частота кварца (Fxtal) внутреннего генератора Si570/571 (Гц)
	U32		nSubExtClk;		// хранит внешнюю частоту тактирования от CLKIN (Гц))
	U32		nBaseUnitClk;	// хранит частоту от базового модуля (Гц))
	REAL64	dStCmpThr;		// хранит последний порог срабатывания компаратора старта (ИПН9/ИПН10)
	U32		nClkSrc;		// хранит источник тактовой частоты: 0-выкл., 1-внутренний, 2-внутренний+PLL, 4-от базмодуля, 0x81-внешний, 0x82-внешний+PLL
	REAL64	dSamplingRate;	// хранит частоту дискретизации, установленную функцией SetRate()

	S16	awRangeDeviation[BRD_CHANCNT];	// отклонение ШП от номинала (разы) (default 10000)
	S16 awBiasDeviation[4];		// отклонения смещения нуля (разы) (default 0)

} FM412X500MSRV_Cfg, *PFM412X500MSRV_Cfg;

#pragma pack(pop)

class CFm412x500mSrv: public CAdcSrv
{
public:

	CFm412x500mSrv( int idx, int srv_num, CModule* pModule, FM412X500MSRV_Cfg *pCfg ); // constructor

protected:

	virtual int  CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void GetAdcTetrNumEx(CModule* pModule, ULONG TetrInstNum);
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
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int GetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int SetInpResist(CModule* pModule, ULONG InpRes, ULONG Chan);
	virtual int GetInpResist(CModule* pModule, ULONG &InpRes, ULONG Chan);
	virtual int SetStartMode(CModule* pModule, PVOID pStartMode);
	virtual int GetStartMode(CModule* pModule, PVOID pStartMode);
	virtual int SetGain(CModule* pModule, double& refGain, ULONG Chan);
	virtual int GetGain(CModule* pModule, double& refGain, ULONG Chan);
	virtual int SetInpRange(CModule* pModule, double& refInpRange, ULONG Chan);
	virtual int GetInpRange(CModule* pModule, double& refInpRange, ULONG Chan);
	virtual int SetCode(CModule* pModule, ULONG type);
	virtual int GetCode(CModule* pModule, ULONG& type);

	virtual int SetMaster(CModule* pModule, ULONG mode);
	virtual int ExtraInit(CModule* pModule);
	int	SpdRead(  CModule* pModule, U32 spdType, U32 regAdr, U32 *pRegVal );
	int	SpdWrite( CModule* pModule, U32 spdType, U32 regAdr, U32 regVal );
	int	Si571SetRate( CModule* pModule, double *pRate );
	int	Si571GetRate( CModule* pModule, double *pRate );

	int	Ad9518SetDividerMode( CModule* pModule, double *pClk, double *pRate );
	int	Ad9518SetPllMode( CModule* pModule, double *pClk, double *pRate );
	int	Ad9518SetDirectPllMode( CModule* pModule, AD9518_TDividers *pDividers );

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

	virtual int SetStartSlave(CModule* pModule, ULONG StartSlave);
	virtual int GetStartSlave(CModule* pModule, ULONG& refStartSlave);

//	virtual int ExtraInit(CModule* pModule);

	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
	int SetMu(CModule* pModule, void *args);
	int GetMu(CModule* pModule, void *args);
};

#endif // _FM412x500MSRV_H

//
// End of file
//
