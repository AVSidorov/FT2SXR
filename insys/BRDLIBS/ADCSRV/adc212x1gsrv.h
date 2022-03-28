/*
 ****************** File Adc212x1gSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Ekkore Okt 2010
 *
 ************************************************************
*/

#ifndef _ADC212x1GSRV_H
 #define _ADC212x1GSRV_H

#include "ctrladc212x1g.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"
#include "adc212x1gregs.h"

#include "adc212x1gsrvinfo.h"
#include "si571.h"

const int ADC212X1G_TETR_ID = 0x78; // tetrad id

// ADC212x1G Threshold DAC number
enum 
{
	BRDtdn_ADC212X1G_BIAS0		= 3,	// bias of channel 0
	BRDtdn_ADC212X1G_BIAS1		= 4,	// bias of channel 1
	BRDtdn_ADC212X1G_STARTCMP	= 7,	// start comparator threshold
};

#pragma pack(push,1)

typedef struct _ADC212X1GSRV_Cfg 
{
	ADCSRV_CFG AdcCfg;
	U32	nSubunitId;           // ID типа ADM-субмодуля
	U32	nSubType;             // подтип субмодуля: 0-Standard, 1 -WB HF, 2-WB LF 
	U32	nRefGen0;             // опорный генератор 0 (Гц)
	U32	nRefGen1;             // опорный генератор 1 (Гц)
	U32	nRefGen0Max;          // опорный генератор 1 (Гц)
	U32	nRefGen0Type;         // тип опорного генератора 1
	U32	nRefGen0Adr;          // адрес опорного генератора 1
	double	dRefGen1Fxtal;	// частота кварца (Fxtal) внутреннего генератора (Гц)
	U32	nRefGen2Type;         // тип опорного генератора 2
	U32	nLpfPassBand;         // частота среза ФНЧ (Гц)
	U32		SubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
	REAL64	aBias[BRD_CHANCNT];			// хранит последнее смещение нуля сигнала в каждом канале
	REAL64	StCmpThr;					// хранит последний порог срабатывания компаратора старта
	REAL64	aGainDb[BRD_CHANCNT];			// коэффициенты передачи для WB (LF и HF) в каждом канале (дБ)
	U32		nVersion;					// версия субмодуля
	double	adIcrRange[4];				// шкалы преобразования для Standard (мВ) (default 10000, 2000, 500, 100)
	S16	awRangeDeviation[2][2][2][4];	// отклонение ШП от номинала (разы) (default 10000)
										// [тип входа][Rвх/ФНЧ][номер АЦП][номер ШП]
	S16 awBiasDeviation[2][2][2][4];	// отклонения смещения нуля (разы) (default 0)
										// [тип входа][Rвх/ФНЧ][номер АЦП][номер ШП]
	double	dSlclkinClk;	// частота сигнала на разъеме SLCLKIN (Гц)
	U32		nDoubleFreq;	// режим удвоения частоты: 0-нет, 1-канал 0, 2-канал 1
	
	U32	PllFreq;					// частота, выдаваемая ФАПЧ in Hz (default 0 GHz)
	PLL_RCNT Pll_R_CNT;					//
	PLL_NCNT Pll_N_CNT;					//
	PLL_FUNC Pll_Func;					//
} ADC212X1GSRV_Cfg, *PADC212X1GSRV_Cfg;

#pragma pack(pop)

class CAdc212x1gSrv: public CAdcSrv
{
public:

	CAdc212x1gSrv(int idx, int srv_num, CModule* pModule, PADC212X1GSRV_Cfg pCfg); // constructor

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
	int	AdcRegWrite( CModule* pModule, U32 adcType, U32 regAdr, U32 regVal );
	int	I2cRead(  CModule* pModule, U32 i2cType, U32 regAdr, U32 *pRegVal );
	int	I2cWrite( CModule* pModule, U32 i2cType, U32 regAdr, U32 regVal );
	int	Si571SetRate( CModule* pModule, double *pRate );
	int	Si571GetRate( CModule* pModule, double *pRate );

	int SetAdcStartMode(CModule* pModule, ULONG source, ULONG inv, double* pCmpThr);
	int GetAdcStartMode(CModule* pModule, PULONG pSource, PULONG pInv, double* pCmpThr);


	virtual int SetSpecific(CModule* pModule, PBRD_AdcSpec pSpec);
	int SetMu(CModule* pModule, void *args);
	int GetMu(CModule* pModule, void *args);
	int SetStartClkSl(CModule* pModule, void	*args);
	int GetStartClkSl(CModule* pModule, void	*args);

	int SetStartDisCntr( CModule *pModule, void *args );  //спец. прошивка для руднева

	int ADF4106_WritePllData(CModule* pModule, U16 hiWord, U16 loWord);
	int ADF4106_InitPLL(CModule* pModule);
	int ADF4106_OnPll(CModule* pModule, double& ClkValue, double PllRefValue);
	int ADF4106_SetPllMode(CModule* pModule, BRD_PllMode *pPllMode);
};

#endif // _ADC212x1GSRV_H

//
// End of file
//
