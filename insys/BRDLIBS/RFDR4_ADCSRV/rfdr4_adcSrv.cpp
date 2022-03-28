/*
 ***************** File rfdr4_adcSrv.cpp ************
 *
 * BRD Driver for rfdr4_adc service 
 *
 * (C) InSys by Sklyarov A. Aug. 2014
 *
 ******************************************************
*/

//#include <windows.h>
//#include <winioctl.h>

#include "module.h"
#include "rfdr4_adcSrv.h"

#define	CURRFILE _BRDC("RFDR4_ADCSRV")

//***************************************************************************************
//***************************************************************************************
int	Crfdr4_adcSrv::IndRegRead( CModule* pModule, S32 tetrNo, S32 regNo, U32 *pVal )
{
	DEVS_CMD_Reg	param;

	param.idxMain = m_index;
	param.tetr = tetrNo;
	param.reg = regNo;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	*pVal = param.val;

	return BRDerr_OK;
}

//***************************************************************************************
int	Crfdr4_adcSrv::IndRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, U32 val )
{
	DEVS_CMD_Reg	param;

	param.idxMain = m_index;
	param.tetr = tetrNo;
	param.reg = regNo;
	param.val = val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int	Crfdr4_adcSrv::DirRegRead( CModule* pModule, S32 tetrNo, S32 regNo, U32 *pVal )
{
	DEVS_CMD_Reg	param;

	param.idxMain = m_index;
	param.tetr = tetrNo;
	param.reg = regNo;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	*pVal = param.val;

	return BRDerr_OK;
}

//***************************************************************************************
int	Crfdr4_adcSrv::DirRegWrite( CModule* pModule, S32 tetrNo, S32 regNo, U32 val )
{
	DEVS_CMD_Reg	param;

	param.idxMain = m_index;
	param.tetr = tetrNo;
	param.reg = regNo;
	param.val = val;
	pModule->RegCtrl(DEVScmd_REGWRITEDIR, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int	Crfdr4_adcSrv::SpdRead(  CModule* pModule, U32 spdType, U32 regAdr, U32 *pRegVal )
{
	//
	// Типы SPD (spdType): 0 -АЦП, 1 - ИПН(ЦАП), 3 - генератор 
	//
	PRFDR4_ADCSRV_CFG	*pSrvCfg  = (PRFDR4_ADCSRV_CFG*)m_pConfig;
	U32			spdCtrl;
	U32			status;
	//int			ii;

	spdCtrl  = 0x1;
//	if( spdType==SPDdev_GEN )
//		spdCtrl |= (pSrvCfg->nGenAdr & 0xFF) << 4;

	for(;;)
	{
		DirRegRead( pModule, m_Rfdr4_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_SPD_DEVICE, spdType );
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_SPD_ADDR, regAdr );
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_SPD_CTRL, spdCtrl );
	//RealDelay(5,1);
	//for( ii=0;; ii++ )
	for(;;)
	{
		DirRegRead( pModule, m_Rfdr4_AdcTetrNum, ADM2IFnr_STATUS, &status );
		//if( ii>1000 )			// организуем паузу
		//	continue;
		if( status & 0x01 )
			break;		
	}
	IndRegRead( pModule, m_Rfdr4_AdcTetrNum, ADCnr_SPD_DATA, pRegVal );

	return BRDerr_OK;
}

//***************************************************************************************
int	Crfdr4_adcSrv::SpdWrite( CModule* pModule, U32 spdType, U32 regAdr, U32 regVal )
{
	PRFDR4_ADCSRV_CFG	*pSrvCfg  = (PRFDR4_ADCSRV_CFG*)m_pConfig;
	U32			spdCtrl;
	U32			status;
	//int			ii;

	spdCtrl  = 0x2; // write to Adc

	for(;;)
	{
		DirRegRead( pModule, m_Rfdr4_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x1 )
			break;		
	}
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_SPD_CTRL, (U32)0 );
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_SPD_DEVICE, spdType );
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_SPD_ADDR, regAdr );
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_SPD_DATA, regVal );
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_SPD_CTRL, spdCtrl );
	for(;;)
	{
		DirRegRead( pModule, m_Rfdr4_AdcTetrNum, ADM2IFnr_STATUS, &status );
		if( status & 0x01 )
			break;		
	}

	return BRDerr_OK;
}
//********************************************************************
int Crfdr4_adcSrv::SetClock(CModule* pModule,  void* args)
{
	DEVS_CMD_Reg param;
	PBRD_SetClock setClock =(PBRD_SetClock)args;
	U32	source = setClock -> source;
	S32	value = setClock -> value;

	param.idxMain = m_index;
	param.tetr = m_Rfdr4_AdcTetrNum;
	param.reg =	ADCnr_CONTROL1;

	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
//	pControl1->ByBits.clkSource  = source;
	pControl1->ByBits.clkSource  = ~source;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************

//***************************************************************************************
int Crfdr4_adcSrv::SetChanMask(CModule* pModule,  void* args)
{
	ULONG mask = *(ULONG*)args;

	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADM2IFnr_CHAN1, mask );

	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_ADCMASK, mask );

	SpdWrite( pModule, SPDdev_ADC, 0x14, 0x09 ); // Output: enable; Format: twos complement
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 ); // Update

	return BRDerr_OK;
}
//***************************************************************************************
int Crfdr4_adcSrv::SetStartMode(CModule* pModule,void *args)
{
	PBRD_StartMode_adc pStartMode = (PBRD_StartMode_adc)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
//-----------------------------------------------------------------------
	param.tetr = m_Rfdr4_AdcTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0 -> ByBits.Master = 1; // Master
    pModule-> RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_STMODE;	
	PADM2IF_STMODE	pStrMode = (PADM2IF_STMODE)&param.val;
	pStrMode->ByBits.SrcStart  = pStartMode->startSrc;
	pStrMode->ByBits.SrcStop   = 0;
	pStrMode->ByBits.InvStart  = pStartMode->startInv;
	pStrMode->ByBits.InvStop   = 0;
	pStrMode->ByBits.TrigStart = 1;
	pStrMode->ByBits.Restart = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
   
	return BRDerr_OK;
}
//=**************************************************************************************

int Crfdr4_adcSrv::InitAdc(CModule* pModule)
{
	//
	// Начальная инициализация программируемых микросхем АЦП (AD9467)
	//
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADM2IFnr_CHAN1, (U32)1 );

	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_ADCMASK, (U32)0xFF );
	SpdWrite( pModule, SPDdev_ADC, 0x10, 0 );
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );
	SpdWrite( pModule, SPDdev_ADC, 0x14, 0x09 );
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );
	SpdWrite( pModule, SPDdev_ADC, 0x18, 0x0A );
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );
	return BRDerr_OK;
}

//***************************************************************************************
int Crfdr4_adcSrv::SetBias( CModule *pModule,void* args )
{
//	double &refBias;
	PBRD_SetBias setBias =(PBRD_SetBias)args;
	U32	chan = setBias -> chan;
	S32	biascode = setBias -> bias;


	//
	// refBias указывает коррекцию смещ. нуля в ЕМРах (-128..+127)
	// При завершении функции записываем в refBias смещение в Вольтах
	//

	if( chan > 3 )
		return BRDerr_BAD_PARAMETER;

//	biascode = (S32)floor( refBias );
	if( biascode > 127 )
		biascode = 127;
	if( biascode < -128 )
		biascode = -128;

	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_ADCMASK, (U32)(1<<chan) );
	SpdWrite( pModule, SPDdev_ADC, 0x10, (U32)biascode );
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );

//	refBias  = (double)biascode / 32768.0;
//	refBias *= pSrvCfg->AdcCfg.InpRange / pSrvCfg->aGain[Chan];
//	pSrvCfg->aBias[Chan] = refBias;

	return BRDerr_OK;
}
//******************************************************************************************
int Crfdr4_adcSrv::SetBufCur(CModule* pModule, void* args)

{	
	PBRD_SetBufCur setBufCur =(PBRD_SetBufCur)args;
	U32	code = 0;
	U32	chan = setBufCur -> chan;
	S32	*bufCur = setBufCur -> bufCur;
	
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_ADCMASK, (U32)(1<<chan) );

	if( (bufCur[0]>=-100) && (bufCur[0]<=530) )
	{
		code   = bufCur[0] / 10;
		code <<= 2;
		code  |= 2;
		code  &= 0xFF;
		SpdWrite( pModule, SPDdev_ADC, 0x36 , code );
//		printf( "\nADC Buffer Current 1 Code = 0x%X (ch%d reg 0x36)\n", code, chanNo );
	}

	if( (bufCur[1]>=-100) && (bufCur[1]<=530) )
	{
		code   = bufCur[1] / 10;
		code <<= 2;
		code  &= 0xFF;
		SpdWrite( pModule, SPDdev_ADC, 0x107 , code );
//		printf( "ADC Buffer Current 2 Code = 0x%X (ch%d reg 0x107)\n\n", code, chanNo );
	}

	SpdWrite( pModule, SPDdev_ADC, 0xFF , 1 );
	SpdWrite( pModule, SPDdev_ADC, 0xFF , 0 );


	return BRDerr_OK;
}
//******************************************************************************************
int Crfdr4_adcSrv::SetGain(CModule *pModule, void* args)
{
	//
	// Коэффициент 1.0  соответствует шкале 1.25 Вольта
	// Коэффициент 1.25 соответствует шкале 1.00 Вольт
	//
	PBRD_SetGain setGain = (PBRD_SetGain)args;
	U32	code = 0;
	U32	chan = setGain->chan;
	float	refGain = setGain->refGain;

	//	if( chan > pSrvCfg->AdcCfg.MaxChan )
	//		return BRDerr_BAD_PARAMETER;

/*
if (refGain >= 1.25)
	{
		refGain = 1.25;	code = 0x00;
	}
	else
	{
		refGain = 1.00; code = 0x0A;
	}
*/	
	refGain -= 0.001;
	if (refGain > 1.25)refGain = 1.25;
	if (refGain <= 1.25)code = 0x00; 
	if (refGain <= 1.20)code = 0x06; 
	if (refGain <= 1.15)code = 0x07; 
	if (refGain <= 1.10)code = 0x08;
	if (refGain <= 1.05)code = 0x09;
	if (refGain <= 1.00)code = 0x0a;
	
	
	


	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_ADCMASK, (U32)(1<<chan) );
	SpdWrite( pModule, SPDdev_ADC, 0x18, code );
	SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );

	return BRDerr_OK;
}
//******************************************************************************************
int Crfdr4_adcSrv::SetOutDelay(CModule *pModule, void* args)
{
	//
	DEVS_CMD_Reg param;
	PBRD_SetOutDelay setDelay = (PBRD_SetOutDelay)args;
	U32	chan = 0x3 & setDelay->chan;
	U32	outdelay = 0x1f & setDelay->outdelay;
	U32 val;

	val =  ( 1<<14) | (chan<<8) | (1<<7) | outdelay;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_AdcTetrNum;
	param.reg = ADCnr_STD_DELAY;
	param.val = val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	//outdelay = outdelay | 0x80;
	//IndRegWrite(pModule, m_Rfdr4_AdcTetrNum, ADCnr_ADCMASK, (U32)(1 << chan));
	//SpdWrite(pModule, SPDdev_ADC, 0x17, outdelay);
	//SpdWrite(pModule, SPDdev_ADC, 0xFF, 0x01);


	return BRDerr_OK;
}
//**********************************************************************************
//int Crfdr4_adcSrv::SetInpRange( CModule *pModule, double &refInpRange, ULONG Chan )
//{
//	PRFDR4_ADCSRV_CFG	*pSrvCfg  = (PRFDR4_ADCSRV_CFG*)m_pConfig;
//	int					status;
//	double				gain;
//
////	if( Chan > pSrvCfg->AdcCfg.MaxChan )
////		return BRDerr_BAD_PARAMETER;
//
//	gain = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / refInpRange;
//	status = SetGain( pModule, gain, Chan );
//	refInpRange = ((double)(pSrvCfg->AdcCfg.InpRange)) / 1000.0 / gain;
//
//	//U32					aCode[]   = { 0x00, 0x06, 0x07, 0x08, 0x09, 0x0A };
//	//double				aRanges[] = { 1.00, 1.05, 1.10, 1.15, 1.20, 1.25 };
//	//int					idx;
//
//
//	//if( Chan > MAX_ADC_CHAN )
//	//	return BRDerr_BAD_PARAMETER;
//
//	//for( idx=0; idx<5; idx++ )
//	//	if( refInpRange <= aRanges[idx] )
//	//		break;
//
//	//IndRegWrite( pModule, m_AdcTetrNum, ADCnr_ADCMASK, (U32)(1<<Chan) );
//	//SpdWrite( pModule, SPDdev_ADC, 0x18, aCode[idx] );
//	//SpdWrite( pModule, SPDdev_ADC, 0xFF, 0x01 );
//
//	//pSrvCfg->aInpRange[Chan] = refInpRange;
//
//	return status;
//}
//***********************************************************************************************
int Crfdr4_adcSrv::SetRf( CModule *pModule,void* args)
{

	DEVS_CMD_Reg param;
	PBRD_SetRf setRf =(PBRD_SetRf)args;
	U32	*atten = setRf -> atten;
	U32	calibr = setRf -> calibr;
	U32	enbblkrf = setRf -> enbblkrf;

	param.idxMain = m_index;
	param.tetr = m_Rfdr4_AdcTetrNum;
	param.reg =	ADCnr_CONTROL1;

	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.calibr  = (calibr&1)^1;
	pControl1->ByBits.enbblkrf  = enbblkrf & 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_ATTEN0, atten[0] );
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_ATTEN1, atten[1] );
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_ATTEN2, atten[2] );
	IndRegWrite( pModule, m_Rfdr4_AdcTetrNum, ADCnr_ATTEN3, atten[3] );

	return BRDerr_OK;
}

//******************************************************************************************
int Crfdr4_adcSrv::ClrBitsOverflow(CModule* pModule, void* args)
{
	ULONG flags = *(ULONG*)args;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_AdcTetrNum;
	param.reg = ADCnr_CLRFLG;
	param.val = flags;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int Crfdr4_adcSrv::GetBitsOverflow(CModule* pModule, void* args)
{
	ULONG OverBits;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_AdcTetrNum;
	param.reg = ADCnr_ADC_OVR;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	OverBits = param.val;
	*(PULONG)args = OverBits & 0xf;

	return BRDerr_OK;
}
//***************************************************************************************
//
//int Crfdr4_adcSrv::CtrlRelease(
//								void			*pDev,		// InfoSM or InfoBM
//								void			*pServData,	// Specific Service Data
//								ULONG			cmd,		// Command Code (from BRD_ctrl())
//								void			*args 		// Command Arguments (from BRD_ctrl())
//								)
//{
//	CModule* pModule = (CModule*)pDev;
//	return BRDerr_CMD_UNSUPPORTED; 
//}
//***************************************************************************************
void Crfdr4_adcSrv::GetAdcTetrNum(CModule* pModule)
{
	m_Rfdr4_AdcTetrNum = GetTetrNum(pModule, m_index,  RFDR4_ADC_TETR_ID );

}
//***************************************************************************************


//****************************************************************************************
// End Of File
//****************************************************************************************
