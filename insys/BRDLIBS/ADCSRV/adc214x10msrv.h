/*
 ****************** File Adc214x10mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Ekkore Nov 2008
 *
 ************************************************************
*/

#ifndef _ADC214x10MSRV_H
 #define _ADC214x10MSRV_H

#include "CtrlAdc214x10m.h"
#include "CtrlCmpSc.h"
#include "Service.h"
#include "Adm2If.h"
#include "AdcSrv.h"
#include "MainRegs.h"
#include "Adc214x10mRegs.h"

#include "Adc214x10mSrvInfo.h"

const int ADC214x10M_TETR_ID  = 0x2C; // tetrad id
const int ADC214x10MX_TETR_ID = 0x17; // tetrad id

// ADC214x10M Threshold DAC number
enum {
	BRDtdn_ADC214x10M_BIAS0		= 3,	// draft (100%) bias of channel 0
	BRDtdn_ADC214x10M_BIAS1		= 4,	// draft (100%)bias of channel 1
	BRDtdn_ADC214x10M_PRECBIAS0		= 7,	// precision (10%) bias of channel 0
	BRDtdn_ADC214x10M_PRECBIAS1		= 8,	// precision (10%) bias of channel 1
};

#pragma pack(push,1)

#define		MAX_MUXDATA		16

typedef struct _ADC214x10MSRV_Cfg 
{
	ADCSRV_CFG AdcCfg;
	U32		aRefGen[2];					// опорные генераторы 0 и 1 (Гц)
	U32		SubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
	REAL64	aBias[MAX_CHAN];			// смещение нуля сигнала в каждом АЦП (проценты от шкалы преобразования)
	REAL64	dStCmpThr;					// порог срабатывания компаратора старта
	REAL64	dGainTun0;					// подстройка коэффициента передачи в каждом канале
	U32		nVersion;					// версия субмодуля
	U32		nSubType;					// тип субмодуля: 214x10M: 0x0-10M, 0x1 - 10MX
	U32		aMuxData[MAX_MUXDATA];		// содержимое памяти мультиплексора
} ADC214x10MSRV_Cfg, *PADC214x10MSRV_Cfg;

#pragma pack(pop)

class CAdc214x10mSrv: public CAdcSrv
{
public:

	CAdc214x10mSrv(int idx, int srv_num, CModule* pModule, PADC214x10MSRV_Cfg pCfg); // constructor

protected:

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetAdcTetrNum(CModule* pModule);
	virtual void* GetInfoForDialog(CModule* pModule);
	virtual void FreeInfoForDialog(PVOID pInfo);
	virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	virtual int SetProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	virtual int SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName);
//	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
//	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
//	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
//	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int SetChanMask(CModule* pModule, ULONG mask);
	virtual int GetChanMask(CModule* pModule, ULONG& mask);
	virtual int GetChanOrder(CModule* pModule, BRD_ItemArray *pItemArray );
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int GetBias(CModule* pModule, double& Bias, ULONG Chan);
	virtual int GetDcCoupling(CModule* pModule, ULONG& InpType, ULONG Chan);
//	virtual int SetStartMode(CModule* pModule, PVOID pStartMode);
//	virtual int GetStartMode(CModule* pModule, PVOID pStartMode);
//	virtual int SetClkLocation(CModule* pModule, ULONG& mode);
//	virtual int GetClkLocation(CModule* pModule, ULONG& mode);
	virtual int SetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int GetGain(CModule* pModule, double& Gain, ULONG Chan);
	virtual int SetInpRange(CModule* pModule, double& InpRange, ULONG Chan);
	virtual int GetInpRange(CModule* pModule, double& InpRange, ULONG Chan);

	virtual int SetCode(CModule* pModule, ULONG type);
	virtual int GetCode(CModule* pModule, ULONG& type);
	virtual int SetMaster(CModule* pModule, ULONG mode);
	virtual int ExtraInit(CModule* pModule);

//	virtual int SetClkInv(CModule* pModule, ULONG Inv);
//	virtual int GetClkInv(CModule* pModule, ULONG& Inv);

	S32	IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, U32 *pVal );
	S32	IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, ULONG *pVal );
	S32	IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, U32 val );
	S32	IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, ULONG val );
};

#endif // _ADC214x10MSRV_H

//
// End of file
//