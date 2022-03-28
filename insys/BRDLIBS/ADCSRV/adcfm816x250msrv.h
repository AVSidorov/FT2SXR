/*
 ****************** File Fm816x250mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Ekkore Aug 2011
 *
 ************************************************************
*/

#ifndef _FM816x250MSRV_H
 #define _FM816x250MSRV_H

#include "ctrlfm816x250m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adcfm816x250mregs.h"

#include "adcfm816x250msrvinfo.h"
#include "si571.h"

const int FM816x250M_TETR_ID = 0x8B; // tetrad id
const int FM416x250M_TETR_ID = 0x93; // tetrad id

// ADC210x1G Threshold DAC number
enum 
{
	//BRDtdn_ADC210X1G_BIAS0		= 3,	// bias of channel 0
	//BRDtdn_ADC210X1G_BIAS1		= 4,	// bias of channel 1
	//BRDtdn_ADC210X1G_STARTCMP	= 7,	// start comparator threshold
};

//
// Перечень устройств, управляемых по SPD
//
enum
{
	SPDdev_ADC		= 0x00,		// АЦП AD9467
	SPDdev_THDAC	= 0x01,		// кристалл ЦАП(ИПН) AD5541
	SPDdev_GEN		= 0x02,		// внутренний генератор (Si570/Si571)
};
#pragma pack(push,1)

typedef struct _FM816x250MSRV_Cfg 
{
	ADCSRV_CFG AdcCfg;

	U32	nVersion;	// версия субмодуля
	U32	nSubunitId;	// ID типа субмодуля: 0x1050 - FM816x250M, 0x1051 - FM416x250M
	U32	nAdcType;	// тип кристалла АЦП: 0-всегда
	U32	nDacType;	// тип кристалла ЦАП: 0-всегда
	U32	nDacRange;	// шкала преобразования ЦАП: (мВ)
	U32	nGenType;	// тип кристалла внутр. генератора: 0-не програм-ый, 1-Si571
	U32	nGenAdr;	// адресный код внутр. генератора: 0x49 по умолчанию
	U32	nGenRef;    // заводская установка частоты внутр. генератора (Гц)
	U32	nGenRefMax; // максимальная частота внутр. генератора (Гц)
	double	dGenFxtal;	// частота кварца (Fxtal) внутреннего генератора (Гц)
	U32		SubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
	REAL64	aBias[MAX_ADC_CHAN];		// хранит последнее смещение нуля сигнала в каждом канале
//	REAL64	aInpRange[MAX_ADC_CHAN];	// хранит последнюю шкалу преобразования в каждом канале
	REAL64	aGain[MAX_ADC_CHAN];		// хранит последний коэффициент передачи в каждом канале
	REAL64	StCmpThr;					// хранит последний порог срабатывания компаратора старта (ИПН9/ИПН10)
	REAL64	ExtClkThr;					// хранит последний порог внешнего такта (ИПН8)
} FM816x250MSRV_Cfg, *PFM816x250MSRV_Cfg;

#pragma pack(pop)

class CFm816x250mSrv: public CAdcSrv
{
public:

	CFm816x250mSrv(int idx, int srv_num, CModule* pModule, PFM816x250MSRV_Cfg pCfg); // constructor

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
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int GetBias(CModule* pModule, double& Bias, ULONG Chan);
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

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
	int SetMu(CModule* pModule, void *args);
	int GetMu(CModule* pModule, void *args);
	int SetBufCur(CModule* pModule, S32 *bufCur, ULONG Chan );
};

#endif // _FM816x250MSRV_H

//
// End of file
//
