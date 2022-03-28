/*
 ***************** File GrAdcSrv.cpp ************
 *
 * BRD Driver for GRADC service 
 *
 * (C) InSys by Sklyarov A. Feb 2011
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "GrAdcSrv.h"

#define	CURRFILE "GRADCSRV"

//***************************************************************************************
//***************************************************************************************
int CGrAdcSrv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	return BRDerr_CMD_UNSUPPORTED; // для освобождения подслужб
}

//***************************************************************************************
void CGrAdcSrv::GetGrAdcTetrNum(CModule* pModule)
{
	m_GrAdcTetrNum = GetTetrNum(pModule, m_index, GRADC_TETR_ID);
}
//***************************************************************************************
int CGrAdcSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GrAdcTetrNum;
	param.reg = ADM2IFnr_CHAN1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_CHAN1 pChan1 = (PADM2IF_CHAN1)&param.val;
	pChan1->ByBits.ChanSel = mask;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CGrAdcSrv::SetMode(CModule* pModule,void *args)
{
	PBRD_GrAdcSetMode ptr = (PBRD_GrAdcSetMode)args;
	DEVS_CMD_Reg param;
	U32 addr_chip,reg_data;
	U32 mode1,mode2,mode3,cword,data[16];
	param.idxMain = m_index;
	param.tetr = m_GrAdcTetrNum;
	PGRADC_MODE2 pMode2 = (PGRADC_MODE2)&mode2;

	pMode2->AsWhole=0;
	pMode2->ByBits.CDIV = ptr->CDIV&1;
	pMode2->ByBits.SCR = ptr->SCR&1;
	pMode2->ByBits.SYNCA = 0;


	param.reg = ADM2IFnr_STATUS;
	param.val=0;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);

	

//-----------------------------------------------------
//		Программирование входного мультиплексора
//-----------------------------------------------------
	for(int i=0; i<8; i++)
	{
		data[i]= 0;
	}
	param.reg = INP_MUX;
	for(int i=0; i<8; i++)
	{
		param.val = data[7-i];
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	param.reg = ADM2IFnr_STATUS;
	param.val=0;
	//pModule->RegCtrl(DEVScmd_REGREADDIR, &param);

	//pMode2->ByBits.ProgMux =1; // старт программирования
	//param.val = mode2;
	//param.reg = ADM2IFnr_MODE2;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);



	//param.reg = ADM2IFnr_STATUS;
	//param.val=0;
 //   while((param.val & (1<<10))!=(1<<10))// ждать окончания программирования 
 //   {
 //			pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
 //   }

	//pMode2->ByBits.ProgMux =0; // стоп программирования
	//param.val = mode2;
	//param.reg = ADM2IFnr_MODE2;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	for(int i=0; i<8; i++)
	{
		cword =(1)<<(ptr->InputMux[2*i]&3);
		if((ptr->InputMux[2*i]&3)==2)cword |=0x8;
		cword |=(1)<<((ptr->InputAtten[2*i]&1) +4);
//		cword |=1<<(4);
		cword |= (1)<<((~ptr->InputLPF[2*i]&1) + 6);
		data[i]=cword;
		cword = (1)<< (ptr->InputMux[2*i+1]&3);
		if((ptr->InputMux[2*i+1]&3)==2)cword |=0x8;
		cword |= (1)<<((ptr->InputAtten[2*i+1]&1) +4);
		cword |= (1)<<((~ptr->InputLPF[2*i+1]&1) +6);
		data[i] |=cword<<8;

//		data[i] = 0x0;
	}

//data[0]=0x52;

	param.reg = INP_MUX;
	for(int i=0; i<8; i++)
	{
		param.val = data[7-i];
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	param.reg = ADM2IFnr_STATUS;
	param.val=0;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);

	pMode2->ByBits.ProgMux =1; // старт программирования
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);



	param.reg = ADM2IFnr_STATUS;
	param.val=0;
    while((param.val & (1<<10))!=(1<<10))// ждать окончания программирования 
    {
 			pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
    }

	pMode2->ByBits.ProgMux =0; // стоп программирования
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//-----------------------------------------------------
//		Программирование входного усилителя
//-----------------------------------------------------
//	data[0] =0;
//	data[1] =0;
	data[0] =0;
	data[1] =0;

	for(int i=0; i<16; i++)
	{
		data[0] |=(ptr->InputGain[i]&1)<<i;
		data[1] |=((ptr->InputGain[i]>>1)&1)<<i;
	}

	param.val = data[0];
	param.reg = GAIN_0KP;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val = data[1];
	param.reg = GAIN_1KP;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//----------------------------------------------------------
//		Программирование внутренних регистров микросхем АЦП
//----------------------------------------------------------

	pMode2->ByBits.Reseta =1; // сброс
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	pMode2->ByBits.Reseta =0; // сброс
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	for(int i = 0; i<16; i++)
	{
		addr_chip= i<<12;
			param.val = addr_chip | 1;  // CONTROL_REGISTER 1;
			param.reg = ADC_ADDR;		//--------------------
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

			reg_data = 8;
			reg_data |= ptr->DecimFlt2 +2;
			reg_data |= (~(ptr->BypassFlt3)&1)<<4;
			param.val = reg_data;
			param.reg = ADC_DATA;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	pMode2->ByBits.ProgAdc =1; // старт программирования
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);



	param.reg = ADM2IFnr_STATUS;
	param.val=0;
    while((param.val & (1<<11))!=(1<<11))// ждать окончания программирования 
    {
 			pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
    }

	pMode2->ByBits.ProgAdc =0; // стоп программирования
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


		param.val = addr_chip | 2;  // CONTROL_REGISTER 2;
		param.reg = ADC_ADDR;		//--------------------
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

//		param.val = 0x9b;
//		param.val = 0x3;
//		param.val = 0x7;
		param.val =((ptr->LowPower)&1)<<2;		//
		param.val  |= 3;		//

		param.reg = ADC_DATA;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADM2IFnr_STATUS;
	param.val=0;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);

	pMode2->ByBits.ProgAdc =1; // старт программирования
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);



	param.reg = ADM2IFnr_STATUS;
	param.val=0;
    while((param.val & (1<<11))!=(1<<11))// ждать окончания программирования 
    {
 			pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
    }

	pMode2->ByBits.ProgAdc =0; // стоп программирования
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val = addr_chip | 3;  // OFFSET_REGISTER;
	param.reg = ADC_ADDR;		//------------------
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val = ptr->Offset[i];
	param.reg = ADC_DATA;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	pMode2->ByBits.ProgAdc =1; // старт программирования
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_STATUS;
	param.val=0;
    while((param.val & (1<<11))!=(1<<11))// ждать окончания программирования 
    {
 			pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
    }

	pMode2->ByBits.ProgAdc =0; // стоп программирования
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val = addr_chip | 4;  // GAIN_REGISTER;
	param.reg = ADC_ADDR;		//----------------
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val = ptr->Gain[i];
	param.reg = ADC_DATA;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	pMode2->ByBits.ProgAdc =1; // старт программирования
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_STATUS;
	param.val=0;
    while((param.val & (1<<11))!=(1<<11))// ждать окончания программирования 
    {
 			pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
    }

	pMode2->ByBits.ProgAdc =0; // стоп программирования
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	}
	pMode2->ByBits.SYNCA =1; // синхронизация
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(2);
	pMode2->ByBits.SYNCA =0; // синхронизация
	param.val = mode2;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;

}
//***********************************************************************************
int CGrAdcSrv::SetModeDdc(CModule* pModule,void *args)
{
	PBRD_GrDdcSetMode ptr = (PBRD_GrDdcSetMode)args;
	DEVS_CMD_Reg param;
	U32 ddcMode,obzDecim=0;;
	param.idxMain = m_index;
	param.tetr = m_GrAdcTetrNum;
	//---------------------------------------------------
	//---------------------------------------------------
	param.val = ptr->MaskObz;
	param.reg = MASK_OBZ;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//----------------------------------------------------
	PGRADC_OBZ_DECIM pObzDecim = (PGRADC_OBZ_DECIM)&obzDecim;

	pObzDecim->ByBits.DecimCicObz= ((ptr->DecimCicObz)-1) & 0x1ff;
	pObzDecim->ByBits.DecimFirObz= ((ptr->DecimFirObz)-1) & 3;

	param.val = obzDecim;
	param.reg = OBZ_DECIM;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	for(int i = 0; i<16; i++)
	{
		param.val = i;
		param.reg = DDC_ADDR;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.val = (ptr->FreqNco[i]) & 0xffff;
		param.reg = NCO_LOW;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.val =((ptr->FreqNco[i]) >>16) & 0xffff;
		param.reg = NCO_HIGH;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	//----------------------------------------------------
	PGRADC_DDC_MODE pDdcMode = (PGRADC_DDC_MODE)&ddcMode;

	pDdcMode->AsWhole=0;
	pDdcMode->ByBits.Output = ptr->Output&3;
	pDdcMode->ByBits.DecimCic= ((ptr->DecimCic)-1) & 0x1ff;
	pDdcMode->ByBits.DecimFir= ((ptr->DecimFir)-1) & 3;
	pDdcMode->ByBits.Cic_bypass = ptr->BypassCic ;
	pDdcMode->ByBits.Fir_bypass = ptr->BypassFir ;;

	param.val = ddcMode;
	param.reg = DDC_MODE;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	 param.val = ptr->MaskChn;
	 param.reg = MASK_CHN;
	 pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	 param.val = ptr->GainDdc & 0xffff;
	 param.reg = GAIN_DDC;
	 pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	 param.val = ptr->GainDdcObz & 0xffff;
	 param.reg = OBZ_GAIN;
	 pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	 return BRDerr_OK;
	

}
//***********************************************************************************
int CGrAdcSrv::SetFilterDdc(CModule* pModule,void *args)
{
	PBRD_GrDdcSetFilter ptr = (PBRD_GrDdcSetFilter)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GrAdcTetrNum;
	//---------------------------------------------------
	param.val = ptr->nDdc;
	param.reg = DDC_ADDR;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	for(int i = 0; i<256; i++)
	{

		param.val = (ptr->Coeff[i]) & 0xffff;
		param.reg = FLT_LOW;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.val =((ptr->Coeff[i]) >>16) & 0xffff;
		param.reg = FLT_HIGH;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	//----------------------------------------------------
	return BRDerr_OK;
}
//*************************************************************************************
//*************************************************************************************

