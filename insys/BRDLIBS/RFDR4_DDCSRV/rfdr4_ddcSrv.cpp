/*
 ***************** File rfdr4_ddcSrv.cpp ************
 *
 * BRD Driver for rfdr4_ddc service 
 *
 * (C) InSys by Sklyarov A. Aug. 2014
 *
 ******************************************************
*/

//#include <windows.h>
//#include <winioctl.h>
#include "gipcy.h"

#include "module.h"
#include "rfdr4_ddcSrv.h"
//#include "pow_mon.h"
#include "pow_mon_rfdr4.h"

#define	CURRFILE _BRDC("RFDR4_DDCSRV")

//***************************************************************************************
//***************************************************************************************
void Crfdr4_ddcSrv::GetDdcTetrNum(CModule* pModule)
{
	m_Rfdr4_DdcTetrNum = GetTetrNum(pModule, m_index,  RFDR4_DDC_TETR_ID );

}
//***************************************************************************************
int Crfdr4_ddcSrv::SetChanMask(CModule* pModule,  void* args)
{
	ULONG mask = *(ULONG*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;
	param.reg = ADM2IFnr_CHAN1;
	param.val = mask;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************
int Crfdr4_ddcSrv::SetStartMode(CModule* pModule,void *args)
{
	PBRD_StartMode_ddc pStartMode = (PBRD_StartMode_ddc)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
//-----------------------------------------------------------------------
	param.tetr = m_Rfdr4_DdcTetrNum;
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


	param.reg = DDCnr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.start_rst_nco = pStartMode->restartNco;
	pControl1->ByBits.start_rst_cic = pStartMode->restartCic;
	pControl1->ByBits.start_rst_fir = pStartMode->restartFir;
	pControl1->ByBits.start_wr = pStartMode->restartNco;
    pModule-> RegCtrl(DEVScmd_REGWRITEIND, &param);

   
	return BRDerr_OK;
}
//=**************************************************************************************
int Crfdr4_ddcSrv::SetDdcExt(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	PBRD_SetDdcExt setDdcExt= (PBRD_SetDdcExt)args;
	U32 *Fc= setDdcExt->Fc;
	U32 SampleRate= setDdcExt->SampleRate;
	U32 ChanMask= setDdcExt->ChanMask;
	U32 CodFc, mask;
	double dFc;
	double dSampleRate= ((double)SampleRate);

	
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;

	param.reg = ADM2IFnr_CHAN1;
	param.val = ChanMask;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	mask = 0x1;
	for(int i=0; i<8; i++)
	{
		param.reg = DDCnr_NCO;

		dFc=0x100000000LL*Fc[i]/dSampleRate;
		CodFc=(U32)dFc;
		param.val = CodFc >> 16;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
		param.val = CodFc;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		param.reg = DDCnr_NCO_MASK;
		param.val = mask;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
		
		param.reg = DDCnr_WRNCO;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		mask = mask << 1;

	}

	param.reg = DDCnr_PROG;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
	return BRDerr_OK;

	return BRDerr_OK;

 
}
//***************************************************************************************
int Crfdr4_ddcSrv::SetScan(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	PBRD_SetScan setScan= (PBRD_SetScan)args;
	U32 *Fc= setScan->Fc;
	U32 SampleRate= setScan->SampleRate;
	U32 Nscan= setScan->Nscan;
	U32 CodFc, mask;
	double dFc;
	double dSampleRate= ((double)SampleRate);

	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;

	param.reg = DDCnr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.wrscaner =setScan->wrscaner;
    pModule-> RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = DDCnr_MEM_RST; // сброс адресного счётчика записи в пвмять
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	for(int j=0; j < Nscan; j++)
	{	
//		for (int i = 0; i<8; i++)
		for (int i = 0; i<4; i++)
			{
			param.reg = DDCnr_NCO;


//			dFc = 0x100000000ULL * Fc[i + 8 * j] / dSampleRate;
			dFc = 0x100000000ULL * Fc[i + 4 * j] / dSampleRate;
			CodFc=(U32)dFc;
			param.val = CodFc >> 16;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
			param.val = CodFc;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

		
			param.reg = DDCnr_MEM_WR;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
		}

	}

	param.reg = DDCnr_NSCAN; 
	param.val = Nscan-1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	


	return BRDerr_OK;

}
//******************************************************
int Crfdr4_ddcSrv::SelScan(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	U32* selScan= (U32*)args;
	
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;

	param.reg = DDCnr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.rdscaner = *selScan;
    pModule-> RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;

}
//***************************************************************************************
int Crfdr4_ddcSrv::EnbScan(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	U32* enbScan= (U32*)args;
	
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;
	
	param.reg = DDCnr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.scan_enb = *enbScan;
    pModule-> RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;

}
//***************************************************************************************
int Crfdr4_ddcSrv::SetLabel(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	U32* userword= (U32*)args;
	
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;
	
	param.reg = DDCnr_USERWORD;
	param.val =(*userword) >> 16;
    pModule-> RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = *userword; 
    pModule-> RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;

}
//***************************************************************************************
int Crfdr4_ddcSrv::SetFrame(CModule* pModule, void *args)
{
	PBRD_SetFrame pSetFrame = (PBRD_SetFrame)args;
	DEVS_CMD_Reg param;
	S32 dstart;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;

	param.reg = DDCnr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.frameOn = pSetFrame->FrameOn;
	pControl1->ByBits.headerOn = pSetFrame->HeaderOn;
    pModule-> RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val = (pSetFrame->SkipSamples - 1) & 0xffff;;
//	param.val = pSetFrame->SkipSamples & 0xffff;
	param.reg = DDCnr_SKIPSAMP;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val=( pSetFrame -> GetSamples -1) & 0xffff;
	param.reg = DDCnr_GETSAMP;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = DDCnr_SYNCWORD;
	param.val= (( pSetFrame -> SyncWord)>> 16) & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val= ( pSetFrame -> SyncWord) & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = DDCnr_USERWORD;
	param.val= (( pSetFrame -> UserWord)>> 16) & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val= ( pSetFrame -> UserWord) & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	dstart= pSetFrame -> DelayStart ;
	dstart *= 250;
	param.val= dstart >> 16;
	param.reg = DDCnr_DELAYSTART;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= dstart ;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;

}
//***************************************************************************************
int Crfdr4_ddcSrv::SetCorrectExt(CModule* pModule, void *args)

{

	PBRD_SetCorrectExt pSetCorrect = (PBRD_SetCorrectExt)args;
	DEVS_CMD_Reg param;
	U32 dstart;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;

	param.reg = DDCnr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.correctOn = pSetCorrect -> correctOn;
    pModule-> RegCtrl(DEVScmd_REGWRITEIND, &param);

	for(int j = 0; j < 8; j++)
	{
		param.reg = DDCnr_COR_COEF;
		for(int i=0; i<4; i++)
		{
			param.val= pSetCorrect->rcoef[i+4*j] & 0xffff;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			param.val= pSetCorrect->icoef[i+4*j] & 0xffff;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		}

		param.reg = DDCnr_COEF_WR;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}

	return BRDerr_OK;
}
//***************************************************************************************
int Crfdr4_ddcSrv::SetDetect(CModule* pModule, void *args)

{

	PBRD_SetDetect pSetDetect = (PBRD_SetDetect)args;
	DEVS_CMD_Reg param;
	U32 dstart;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;

	param.reg = DDCnr_CONTROL0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);

	PREG_CONTROL0 pControl0 = (PREG_CONTROL0)&param.val;
	pControl0->ByBits.porog_det_clk = ~( pSetDetect ->porog);
	pControl0->ByBits.mode_det_clk = pSetDetect ->mode;
    pModule-> RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************
int Crfdr4_ddcSrv::GetDetect(CModule* pModule, void *args)

{
	U32* par = (U32*)args;
	DEVS_CMD_Reg param;

	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;

	param.reg = DDCnr_GETDETECT;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	IPC_delay(20);

	param.reg = 0x220;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);

	*par = param.val;
	return BRDerr_OK;
}
//***************************************************************************************
int Crfdr4_ddcSrv::SetLed(CModule* pModule, void *args)

{

	PBRD_SetLed pSetLed = (PBRD_SetLed)args;
	DEVS_CMD_Reg param;
	U32 dstart;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;

	param.reg = DDCnr_CONTROL0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL0 pControl0 = (PREG_CONTROL0)&param.val;
	pControl0->ByBits.led_control = pSetLed ->control;
	pControl0->ByBits.led_level = ~(pSetLed ->level);
    pModule-> RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}
//***************************************************************************************
int Crfdr4_ddcSrv::StartDdc(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;

	param.reg = DDCnr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.startDdc = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}
//***************************************************************************************
int Crfdr4_ddcSrv::StopDdc(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_Rfdr4_DdcTetrNum;

	param.reg = DDCnr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.startDdc = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}
///*******************************************************************************************************************
int Crfdr4_ddcSrv::GetPower(CModule* pModule, void *args)
{
	int	i=0;
	PBRD_GetPower pGetPow = (PBRD_GetPower)args;
	U32 err=0;

//------------------------------------- For  INA219 --------------------------------------
		for( i = 0 ; i < SENSOR_COUNT; ++i )	
		{
		    g_anStatus[i] = readMsSpdDev(pModule, SENSOR_ADDR[i], 1, BUS_VOLTAGE_REGISTER_ADDR, &g_arBusVoltReg[i].AsWhole);
			if(g_anStatus[i] < 0 )if(g_anStatus[i] < 0 ) err |= (1<<i); 
		    g_anStatus[i] = readMsSpdDev(pModule, SENSOR_ADDR[i], 1, SHUNT_VOLTAGE_REGISTER_ADDR, &g_arShuntVoltReg[i].AsWhole);
			if(g_anStatus[i] < 0 )if(g_anStatus[i] < 0 ) err |= (1<<i); 
		}

			pGetPow -> v_5_0vx= float(g_arBusVoltReg[0].ByBits.BVR) * 4 / 1000;	 // Напряжение цепи +5.0 VX		
			pGetPow -> v_3_3vx= float(g_arBusVoltReg[1].ByBits.BVR) * 4 / 1000;	 // Напряжение цепи +3.3 VX			
			pGetPow -> v_12_0vx= float(g_arBusVoltReg[2].ByBits.BVR) * 4 / 1000; // Напряжение цепи +12.0 VX		

			pGetPow -> i_5_0vx= float(g_arShuntVoltReg[0].AsWhole/100)/(R_MAX[0]*1000);	 // Ток в  цепи +5.0 VX		
			pGetPow -> i_3_3vx= float(g_arShuntVoltReg[1].AsWhole/100)/(R_MAX[1]*1000);  // Ток в  цепи +3.3 VX			
			pGetPow -> i_12_0vx= float(g_arShuntVoltReg[2].AsWhole/100)/(R_MAX[2]*1000); // Ток в  цепи +12.0 VX			

			pGetPow -> p_5_0vx=  pGetPow->v_5_0vx * pGetPow->i_5_0vx;	  // Мощность в  цепи +5.0 VX		
			pGetPow -> p_3_3vx=  pGetPow->v_3_3vx * pGetPow->i_3_3vx;	  // Мощность в  цепи +3.3 VX			
			pGetPow -> p_12_0vx= pGetPow->v_12_0vx * pGetPow->i_12_0vx;   // Мощность в  цепи +12.0 VX			


//----------------------------------------- For LTC219 ----------------------------


	U32 Status;
	ULONG MSB,LSB, TMSB,TLSB;
	int icnt;
	float vchan[8];
	for( int i=0; i<7; i++ )
	{
		icnt = 0;
			do{
			IPC_delay(10);
			Status = readMsSpdDev(pModule, LTC_ADDR, 0,  V1_MSB +i*2,&MSB);
				icnt++;
			}while(!(MSB & 0x80) && (icnt < 500));

			Status = readMsSpdDev(pModule, LTC_ADDR, 0,  V1_LSB+i*2, &LSB);
			
			MSB = MSB & 0x3F;
			double V = LSB | (MSB << 8);
			V = (V * 0.305)/1000;
			vchan[i] = V;
	}


	icnt = 0;
		do{
			IPC_delay(10);
			Status = readMsSpdDev(pModule, LTC_ADDR, 0,  TINT_MSB ,&TMSB);
				icnt++;
			}while(!(MSB & 0x80) && (icnt < 50));

		Status = readMsSpdDev(pModule, LTC_ADDR, 0,TINT_LSB, &TLSB);

	
		//---------------------------------------------------------------------------										
		//	TMSB = TMSB & 0x3F;							// Было(не учитывался знак!)
		//	double T = TLSB | (TMSB << 8);
		//	T = (T *62.5)/1000;
		//---------------------------------------------------------------------------
														// Стало  10.02.2017
	//	TMSB = 0x1d;
	//	TLSB = 0x80;
		TMSB = TMSB & 0x1F;
		S16  DT = TLSB | (TMSB << 8);
		DT = (DT << 3);
		DT = DT >> 3;
		double T = DT;
		T = (T *62.5) / 1000;
		//-----------------------------------------------------------------------------	


	pGetPow -> Tc= T;		//Температура	
	pGetPow -> v_1_0v= vchan[0];	// Напряжение цепи +1.0 V		
	pGetPow -> v_1_8v= vchan[1];	// Напряжение цепи +1.8 V	
	pGetPow -> v_2_5v= vchan[2];	// Напряжение цепи +2.5 V	
	pGetPow -> v_3_3vcco= 2* vchan[3];	// Напряжение цепи +3.3 VA	
	pGetPow -> v_3_3va= 2* vchan[4];	// Напряжение цепи +3.3 VA	
	pGetPow -> v_5_0va= 2* vchan[5];	// Напряжение цепи +5.0 VA
	pGetPow -> v_12_0va= 4.8 * vchan[6] ;	// Напряжение цепи +12.0 VA	


 return BRDerr_OK;
}
//--------------------------------------------------------------------
int Crfdr4_ddcSrv::readMsSpdDev(CModule* pModule,U32 addr, U32 dbl, U32 reg, ULONG* pVal)
{	
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;

 // ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	do {
	 pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);


	// выбираем устройство
	param.reg = MAIN17nr_SPDDEVICE; // 0x203
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем адрес
	param.reg = MAIN17nr_SPDADDR; // 0x205
	param.val =  reg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// посылаем команду чтения
	param.val = (dbl << 13) | (addr << 4) | 0x1; // read
	param.reg = MAIN17nr_SPDCTRL; // 0x204
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	pStatus = (PADM2IF_STATUS)&param.val;
	do {
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// считываем данные
	 param.reg = MAIN17nr_SPDDATAL; // 0x206;
	 pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	 *pVal = param.val;
	 return BRDerr_OK;
}
//**************************************************************************************
int Crfdr4_ddcSrv::writeMsSpdDev(CModule* pModule, U32 addr, U32 dbl, U32 reg, U32 val)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

		// выбираем устройство
	param.reg = MAIN17nr_SPDDEVICE; // 0x203

	static int xx=0;
	param.val = 0;//xx++;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		// записываем адрес регистра
	param.reg = MAIN17nr_SPDADDR; // 0x205
	param.val = reg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем данные для записи в регистр
	param.reg = MAIN17nr_SPDDATAL; // 0x206  
	param.val = val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// посылаем команду записи
	param.val = (dbl << 13) | (addr << 4) | 0x2; // write
	param.reg = MAIN17nr_SPDCTRL; // 0x204
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	pStatus = (PADM2IF_STATUS)&param.val;
	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	   } while(!pStatus->ByBits.CmdRdy);

	return BRDerr_OK;
}
//****************************************************************************************
int Crfdr4_ddcSrv::InitPower(CModule* pModule,void *args)
{
	size_t i;
	U32* par = (U32*)args;
	//********************************* For INA219
	U32 Status,err=0;
	int icnt=0;
	//------------------------------------ 1 --------------------------------------------
	g_CLBR_MAX[_5VX]  = 6000;	
	g_CLBR_MAX[_3VX] = 6000;
	g_CLBR_MAX[_12VX] = 4096;		


	//------------------------------------ 2 --------------------------------------------

//for( ;;)
{

		for( i = 0 ; i < SENSOR_COUNT; ++i )
	{
		//установка буфера
		g_arClbrReg[i].ByBits.CLBR = g_CLBR_MAX[i];

			//запись в устройство
		g_anStatus[i] = writeMsSpdDev(pModule, SENSOR_ADDR[i], 1, CALIBRATION_REGISTER_ADDR, g_arClbrReg[i].AsWhole);
		if(g_anStatus[i] < 0 ) err |= (1<<i); 
	}
}
	//-------------------------------------3 -----------------------------------------------
	for( i = 0 ; i < SENSOR_COUNT; ++i )
	{
		g_aPG_MAX[i] = g_CLBR_MAX[i] * R_MAX[i];
	}	

	//корректировка в сторону большего: 320, 160, 80, 40
	for( i = 0 ; i < SENSOR_COUNT; ++i )
	{
		if(g_aPG_MAX[i] < 40 ) 
		{
			g_aPG_MAX[i] = 40;
			continue;
		}
		if(g_aPG_MAX[i] < 80 ) 
		{
			g_aPG_MAX[i] = 80;
			continue;
		}
		if(g_aPG_MAX[i] < 160 ) 
		{
			g_aPG_MAX[i] = 160;
			continue;
		}
		g_aPG_MAX[i] = 320;
	}
	//-------------------------------------4 -----------------------------------------------
			//коррекция CLBR_MAX
	for( i = 0 ; i < SENSOR_COUNT; ++i )
	{
		g_CLBR_MAX[i] = g_aPG_MAX[i] / R_MAX[i];
	}
	//----------------------------------- 5 ---------------------------------------------------
	for( i = 0 ; i < SENSOR_COUNT; ++i )
	{
		g_anStatus[i] = readMsSpdDev(pModule, SENSOR_ADDR[i], 1, CONFIGURATION_REGISTER_ADDR, &g_arConfReg[i].AsWhole);
		if(g_anStatus[i] < 0 ) err |= (1<<i); 
	}
	//------------------------------------- 6 ------------------------------------------------
	for( i = 0 ; i < SENSOR_COUNT; ++i )
	{
		g_arConfReg[i].ByBits.BRNG = 0;
	}	
	//------------------------------------ 7 - 10 --------------------------------------------
	for( i = 0 ; i < SENSOR_COUNT; ++i )
	{
		if(g_aPG_MAX[i] <= 41 ) g_arConfReg[i].ByBits.PG = 0x0;
		if(g_aPG_MAX[i] > 41  ) g_arConfReg[i].ByBits.PG = 0x1;
		if(g_aPG_MAX[i] > 81  ) g_arConfReg[i].ByBits.PG = 0x2;
		if(g_aPG_MAX[i] > 161 ) g_arConfReg[i].ByBits.PG = 0x3; 
	}
	//--------------------------------------- 11 --------------------------------------------
	// время в миллисекундах
	double TBADC[4] = {0,0,0,0};	
	double TSADC[4] = {0,0,0,0};

	g_TCsum = 0;

	for( i = 0 ; i < SENSOR_COUNT; ++i )	
	{
		//определение TBADC
		switch (g_arConfReg[i].ByBits.BADC)
		{
		case 0x0: TBADC[i] = 0.084; break;//0000
		case 0x4: TBADC[i] = 0.084; break;//0100
		case 0x1: TBADC[i] = 0.148; break;//0001
		case 0x5: TBADC[i] = 0.148; break;//0101
		case 0x2: TBADC[i] = 0.276; break;//0010
		case 0x6: TBADC[i] = 0.276; break;//0110
		case 0x3: TBADC[i] = 0.532; break;//0011
		case 0x7: TBADC[i] = 0.532; break;//0111
		case 0x8: TBADC[i] = 0.532; break;//1000
		case 0x9: TBADC[i] =  1.06; break;//1001
		case 0xA: TBADC[i] =  1.06; break;//1010
		case 0xB: TBADC[i] =  2.13; break;//1011
		case 0xC: TBADC[i] =  4.26; break;//1100
		case 0xD: TBADC[i] =  8.52; break;//1101
		case 0xE: TBADC[i] = 17.04; break;//1110
		case 0xF: TBADC[i] = 34.04; break;//1111
		default: //msg("Error!","Ошибка определения TBADC"); 
			return -1;
		}
				//определение TSADC
		switch (g_arConfReg[i].ByBits.SADC)
		{
		case 0x0: TSADC[i] = 0.084; break;//0000
		case 0x4: TSADC[i] = 0.084; break;//0100
		case 0x1: TSADC[i] = 0.148; break;//0001
		case 0x5: TSADC[i] = 0.148; break;//0101
		case 0x2: TSADC[i] = 0.276; break;//0010
		case 0x6: TSADC[i] = 0.276; break;//0110
		case 0x3: TSADC[i] = 0.532; break;//0011
		case 0x7: TSADC[i] = 0.532; break;//0111
		case 0x8: TSADC[i] = 0.532; break;//1000
		case 0x9: TSADC[i] =  1.06; break;//1001
		case 0xA: TSADC[i] =  1.06; break;//1010
		case 0xB: TSADC[i] =  2.13; break;//1011
		case 0xC: TSADC[i] =  4.26; break;//1100
		case 0xD: TSADC[i] =  8.52; break;//1101
		case 0xE: TSADC[i] = 17.04; break;//1110
		case 0xF: TSADC[i] = 34.04; break;//1111
		default: //msg("Error!","Ошибка определения TSADC"); 
			return -1;
		}
		// время преобразования для каждой микросхемы
		g_aTC[i] = TBADC[i] + TSADC[i];
		// суммарное время преобразования
		g_TCsum += g_aTC[i];
	}

	//------------------------------------------ 12 --------------------------------------
	for( i = 0 ; i < SENSOR_COUNT; ++i )
	{
		g_arClbrReg[i].ByBits.CLBR = (U32)g_CLBR_MAX[i];	
	}

	//----------------------------------------- 13 ----------------------------------------
	for( i = 0 ; i < SENSOR_COUNT; ++i )
	{
		//запись в устройство
		g_anStatus[i] = writeMsSpdDev(pModule, SENSOR_ADDR[i], 1, CALIBRATION_REGISTER_ADDR, g_arClbrReg[i].AsWhole);
		if(g_anStatus[i] < 0 ) err |= (1<<i); 
	}

	// 14
	for( i = 0 ; i < SENSOR_COUNT; ++i )
	{
		//запись в устройство

//		g_arConfReg[i].AsWhole = 0x019f;
	//	g_arConfReg[i].AsWhole = 0x019f;
		g_anStatus[i] = writeMsSpdDev(pModule, SENSOR_ADDR[i], 1, CONFIGURATION_REGISTER_ADDR, g_arConfReg[i].AsWhole);
		if(g_anStatus[i] < 0 ) err |= (1<<i); 
	}


	*par = err;

	//*********************************** For LTC219 **************************************


//		Status = writeMsSpdDev(pModule, LTC_ADDR, 0, 6, 0x88);
		Status = writeMsSpdDev(pModule, LTC_ADDR, 0, 6, 0x0);
		IPC_delay(10);
//		Status = writeMsSpdDev(pModule, LTC_ADDR, 0, 7, 0x88);
		Status = writeMsSpdDev(pModule, LTC_ADDR, 0, 7, 0x0);
		IPC_delay(10);

		U32	stH = 0xF8,st;
		Status = writeMsSpdDev(pModule, LTC_ADDR, 0, CHANNEL_ENABLE, stH);
		do{
			IPC_delay(10);
			Status = readMsSpdDev(pModule, LTC_ADDR, 0, CHANNEL_ENABLE, (ULONG*)&stH);
			icnt++;
		}while((stH & 0x04) && (icnt < 50));
		Status = writeMsSpdDev(pModule, LTC_ADDR, 0, PWM_MSB, 0x10);

	IPC_delay(10);


	if(err==0)  return BRDerr_OK;
	else	    return BRDerr_ERROR;
}
//**************************************************************************************

//****************************************************************************************
// End Of File
//****************************************************************************************
