/*
 ****************** File Adc210x1gSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Ekkore Okt 2009
 *
 ************************************************************
*/

#ifndef _ADC210x1GSRV_H
 #define _ADC210x1GSRV_H

#include "CtrlAdc210x1g.h"
#include "CtrlCmpSc.h"
#include "Service.h"
#include "Adm2If.h"
#include "AdcSrv.h"
#include "MainRegs.h"
#include "Adc210x1gRegs.h"

#include "Adc210x1gSrvInfo.h"

const int ADC210X1G_TETR_ID = 0x69; // tetrad id

// ADC210x1G Threshold DAC number
enum 
{
	BRDtdn_ADC210X1G_BIAS0		= 3,	// bias of channel 0
	BRDtdn_ADC210X1G_BIAS1		= 4,	// bias of channel 1
	BRDtdn_ADC210X1G_STARTCMP	= 7,	// start comparator threshold
};

#pragma pack(push,1)

typedef struct _ADC210X1GSRV_Cfg 
{
	ADCSRV_CFG AdcCfg;
	U32	nRefGen0;             // опорный генератор 0 (Гц)
	U32	nRefGen1;             // опорный генератор 1 (Гц)
	U32	nRefGen1Min;          // опорный генератор 1 (Гц)
	U32	nRefGen1Max;          // опорный генератор 1 (Гц)
	U32	nRefGen1Type;         // тип опорного генератора 1
	U32	nRefGen2;             // опорный генератор 2 (Гц)
	U32	nRefGen2Type;         // тип опорного генератора 2
	U32	nLpfPassBand;         // частота среза ФНЧ (Гц)
	U32		SubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
	REAL64	aBias[MAX_CHAN];			// хранит последнее смещение нуля сигнала в каждом канале
	REAL64	StCmpThr;					// хранит последний порог срабатывания компаратора старта
	REAL64	aGainDb[MAX_CHAN];			// коэффициенты передачи для WB (LF и HF) в каждом канале (дБ)
//	REAL64	aGainTun[MAX_CHAN];			// подстройки коэффициентов передачи в каждом канале
	U32		nVersion;					// версия субмодуля
	double	adIcrRange[4];				// шкалы преобразования для Standard (мВ) (default 10000, 2000, 500, 100)
	S16	awRangeDeviation[2][2][2][4];	// отклонение ШП от номинала (разы) (default 10000)
										// [тип входа][Rвх/ФНЧ][номер АЦП][номер ШП]
	S16 awBiasDeviation[2][2][2][4];	// отклонения смещения нуля (разы) (default 0)
										// [тип входа][Rвх/ФНЧ][номер АЦП][номер ШП]
	double	dSlclkinClk;	// частота сигнала на разъеме SLCLKIN (Гц)
	U32		nDoubleFreq;	// режим удвоения частоты: 0-нет, 1-канал 0, 2-канал 1
	double	dDoubleFreqBias1;// корректирующее значение смещения нуля для АЦП1 в режиме удвоения частоты (%): +100 .. -100
	double	dDoubleFreqRange1;// корректирующее значение шкалы для АЦП1 в режиме удвоения частоты (%): +100 .. -100
} ADC210X1GSRV_Cfg, *PADC210X1GSRV_Cfg;

#pragma pack(pop)

class CAdc210x1gSrv: public CAdcSrv
{
public:

	CAdc210x1gSrv(int idx, int srv_num, CModule* pModule, PADC210X1GSRV_Cfg pCfg); // constructor

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
	virtual int SetClkInv(CModule* pModule, ULONG Inv);
	virtual int GetClkInv(CModule* pModule, ULONG& Inv);
	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int SetClkLocation(CModule* pModule, ULONG& mode);
	virtual int GetClkLocation(CModule* pModule, ULONG& mode);
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetInpSrc(CModule* pModule, PVOID pCtrl);

	virtual int GetInpSrc(CModule* pModule, PVOID pCtrl);
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

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);

//	virtual int ExtraInit(CModule* pModule);

	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
	int SetMu(CModule* pModule, void *args);
	int GetMu(CModule* pModule, void *args);
	int SetStartClkSl(CModule* pModule, void	*args);
	int GetStartClkSl(CModule* pModule, void	*args);
	int SetDblClkEx(CModule* pModule, void *args);
	int GetDblClkEx(CModule* pModule, void *args);
	int GetDblClk(CModule* pModule, void *args);
	virtual int GetDblClk(CModule* pModule, ULONG& mode);

	//S32	IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, U32 *pVal );
	//S32	IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, ULONG *pVal );
	//S32	IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, U32 val );
	//S32	IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, ULONG val );
};

#endif // _ADC210x1GSRV_H

//
// End of file
//