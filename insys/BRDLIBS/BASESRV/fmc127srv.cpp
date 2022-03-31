/*
 ***************** File fmc127srv.cpp ***********************
 *
 * BRD Driver for BASE of FMC-modules service
 *
 * (C) InSys by Dorokhin A. Dec 2015
 *
 *********************************************************
*/

#include "module.h"
#include "fmc127srv.h"

#define	CURRFILE _BRDC("FMC127SRV")

static CMD_Info SETWITCH_CMD	= { BRDctrl_FMC127P_SETSWITCH,	0, 0, 0, NULL};
static CMD_Info GETSWITCH_CMD	= { BRDctrl_FMC127P_GETSWITCH,	1, 0, 0, NULL};
static CMD_Info SENSMON_CMD		= { BRDctrl_FMC127P_SENSMON,	1, 0, 0, NULL};
//static CMD_Info ONSWITCH_CMD	= { BRDctrl_FMC127P_SWITCHONOFF,	0, 0, 0, NULL};
//static CMD_Info READSTAT_CMD	= { BRDctrl_FMC127P_GETONOFF,		1, 0, 0, NULL};
static CMD_Info SETCLKMODE_CMD	= { BRDctrl_FMC127P_SETCLKMODE,	0, 0, 0, NULL};
static CMD_Info GETCLKMODE_CMD	= { BRDctrl_FMC127P_GETCLKMODE,	1, 0, 0, NULL};
//static CMD_Info SWITCHDEV_CMD	= { BRDctrl_FMC127P_SWITCHDEV,	0, 0, 0, NULL};
static CMD_Info WRITEUROM_CMD	= { BRDctrl_FMC127P_WRITEUROM,	0, 0, 0, NULL};
static CMD_Info READUROM_CMD	= { BRDctrl_FMC127P_READUROM,	1, 0, 0, NULL};

//***************************************************************************************
Cfmc127srv::Cfmc127srv(int idx, int srv_num, CModule* pModule, PFMC127SRV_CFG pCfg) :
	CService(idx, _BRDC("FMC127PSRV"), srv_num, pModule, pCfg, sizeof(FMC127SRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_DIRECTION_OUT;	//|
			//BRDserv_ATTR_EXCLUSIVE_ONLY;

	Init(&SETWITCH_CMD, (CmdEntry)&Cfmc127srv::CtrlClkSwitch);
	Init(&GETSWITCH_CMD, (CmdEntry)&Cfmc127srv::CtrlClkSwitch);
	Init(&SENSMON_CMD, (CmdEntry)&Cfmc127srv::CtrlSensMon);
	//Init(&ONSWITCH_CMD, (CmdEntry)&CBasefSrv::CtrlOnOffSwitch);
	//Init(&READSTAT_CMD, (CmdEntry)&CBasefSrv::CtrlReadStatus);
	//Init(&SWITCHDEV_CMD, (CmdEntry)&CBasefSrv::CtrlSwitchDev);

	Init(&SETCLKMODE_CMD, (CmdEntry)&Cfmc127srv::CtrlClkMode);
	Init(&GETCLKMODE_CMD, (CmdEntry)&Cfmc127srv::CtrlClkMode);

	Init(&WRITEUROM_CMD, (CmdEntry)&Cfmc127srv::CtrlUserEeprom);
	Init(&READUROM_CMD, (CmdEntry)&Cfmc127srv::CtrlUserEeprom);
}

//***************************************************************************************
int Cfmc127srv::CtrlIsAvailable(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	PSERV_CMD_IsAvailable pServAvailable = (PSERV_CMD_IsAvailable)args;
	pServAvailable->attr = m_attribute;
	m_MainTetrNum = GetTetrNum(pModule, m_index, MAIN_TETR_ID);

	m_isAvailable = 0;

	if(m_MainTetrNum != -1)
	{
		DEVS_CMD_Reg param;
		param.idxMain = m_index;
		param.tetr = m_MainTetrNum;
		param.reg = ADM2IFnr_IDMOD;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(param.val >= 17)
			m_isAvailable = 1;
	}
	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
int Cfmc127srv::CtrlCapture(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	if(m_isAvailable)
	{
		PFMC127SRV_CFG pSrvCfg = (PFMC127SRV_CFG)m_pConfig;
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;
			//pSrvCfg->SwitchDevId = FMC127Psd_CLOCK;
			pSrvCfg->dGenFxtal = 0.0;
			if(pSrvCfg->Gen0Type)
			{	
				DEVS_CMD_Reg param;
				param.idxMain = m_index;
				param.tetr = m_MainTetrNum;
				param.reg = MAINnr_CMPMUX;
				pModule->RegCtrl(DEVScmd_REGREADIND, &param);
				PMAIN_MUX pMux = (PMAIN_MUX)&param.val;
				ULONG prev = pMux->ByBits.GenEn;
				pMux->ByBits.GenEn = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
				pMux->ByBits.GenEn = 1;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
				IPC_delay(10);
				IPC_delay(10);

				writeSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen0, 0, 135, 0x80); // Reset
				IPC_delay(100);

				U32	regAdr, regData[20];
				// Считать регистры Si570/Si571
				for( regAdr=7; regAdr<=12; regAdr++ )
					readSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen0, 0, regAdr, &regData[regAdr]);

				// Рассчитать частоту кварца
				SI571_CalcFxtal( &(pSrvCfg->dGenFxtal), (double)(pSrvCfg->RefGen0), (U32*)regData );
				//printf( "After reset Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n", regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );
				//printf( ">> XTAL = %f kHz\n", pSrvCfg->dGenFxtal/1000.0 );
				//printf( ">> GREF = %f kHz\n", ((double)(pSrvCfg->RefGen0))/1000.0 );

				// из-за I2C-коммутатора интерфейс виснет при отключении питания генератора, поэтому коммутатор переключаем на другое устройство
				writeSpdDev(pModule, SENSMON_DEVID, pSrvCfg->AdrMonitor, 0, 0x6, 0); 
				
				IPC_delay(300);
				pMux->ByBits.GenEn = prev;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
				IPC_delay(300);
			}
/*			U08 buf[512];
			//for(int i = 0; i < 512; i++)
			//	buf[i] = 0xff;
			//WriteUserEeprom( pModule, buf, 512, 0);
			//ReadUserEeprom( pModule, buf, 512, 0);
			//for(int i = 0; i < 256; i++)
			//{
			//	buf[i] = i;
			//	buf[256+i] = 255-i;
			//}
			//buf[0] = 0x53;
			//buf[1] = 0x49;
			//WriteUserEeprom( pModule, buf, 512, 0);
			for(int i = 0; i < 512; i++)
				buf[i] = 0;
			ReadUserEeprom( pModule, buf, 256, 256);
			ReadUserEeprom( pModule, buf, 256, 0);*/
		}
	}
	return BRDerr_OK;
}

//***************************************************************************************
int Cfmc127srv::CtrlSensMon(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_SensMonVal pMonVal = (PBRD_SensMonVal)args;
	PFMC127SRV_CFG pBasefCfg = (PFMC127SRV_CFG)m_pConfig;

	if(pMonVal->chip > 2)	// на модуле установлено 3 микросхемы LTC2991 (Voltage, Current and Temperature Monitor)
		return BRDerr_BAD_PARAMETER;

	ULONG numb = pBasefCfg->AdrMonitor;
	if(pMonVal->chip >= 1)
		numb++;
	if(pMonVal->chip >= 2)
		numb++;

	U32 ctrl_val = 0x0000;
	U32 flten = 0;
	if(pMonVal->mode > 1)
	{ // режимы 2 и 3 - это режимы с усреднением (Filter Enabled)
		flten = 1;
		ctrl_val |= 0x8888;
	}
	U32 difmode = 0;
	if(pMonVal->mode == 1 || pMonVal->mode == 3)
	{ // режимы 1 и 3 - это дифференциальные режимы (Differential)
		difmode = 1;
		if(pMonVal->chip)
			ctrl_val |= 0x1111; // в чипах 1 и 2 в дифференциальный режим переводим все входы (V1-V8)
		else
			ctrl_val |= 0x0011; // в чипе 0 в дифференциальный режим переводим только входы V1,V2 и V3,V4
	}

	double _vchan[8];
	double _tempr;
	double _vcc;
	double _curchan[4];

	if(pBasefCfg->ModeMonitor[pMonVal->chip] != pMonVal->mode)
	{
		pBasefCfg->ModeMonitor[pMonVal->chip] = pMonVal->mode;
		setModeSensors(pModule, numb, flten, ctrl_val & 0xFF, ctrl_val >> 8);
	}

	readSensors(pModule, numb, ctrl_val, _vchan, _curchan, &_tempr, &_vcc);

	if(pMonVal->chip == 0)
	{
		for(int i=0; i < 8; i++ )
		{
			double out = _vchan[i];

			if( i == 0 || i == 1)
				out *= 6;
			//if( i == 2 || i == 3)
				//out *= 2;

			pMonVal->inp[i] = out;
		}
		if(difmode)
		{
			pMonVal->cur[0] = _curchan[0]/0.05 * 6;
			pMonVal->cur[1] = _curchan[1]/0.004;// * 2;
		}
		else
		{
			pMonVal->cur[0] = fabs(_vchan[0] - _vchan[1])/0.05 * 6;
			pMonVal->cur[1] = fabs(_vchan[2] - _vchan[3])/0.004;
		}
	}

	if(pMonVal->chip == 1 || pMonVal->chip == 2)
	{
		for(int i=0; i < 8; i++ )
		{
			double out = _vchan[i];// * 2;

			if( i == 2 || i == 3)
				out *= 6;

			pMonVal->inp[i] = out;
		}
		if(difmode)
		{
			pMonVal->cur[0] = _curchan[0]/0.004;// * 2;
			pMonVal->cur[1] = _curchan[1]/0.05 * 6;
			pMonVal->cur[2] = _curchan[2]/0.004;// * 2;
		}
		else
		{
			pMonVal->cur[0] = fabs(_vchan[0] - _vchan[1])/0.004;
			pMonVal->cur[1] = fabs(_vchan[2] - _vchan[3])/0.05 * 6;
			pMonVal->cur[2] = fabs(_vchan[4] - _vchan[5])/0.004;
		}
	}

	pMonVal->tint = _tempr;
	pMonVal->vcc = _vcc;

	return BRDerr_OK;
}

void Cfmc127srv::setModeSensors(CModule* pModule, U32 numb, U32 flt, U32 ctrl_reg6, U32 ctrl_reg7)
{
	writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x6, ctrl_reg6); // устанавливаем нужный режим для V1-V4
	IPC_delay(10);
	writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x7, ctrl_reg7); // устанавливаем нужный режим для V5-V8
	IPC_delay(10);
	writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x8, 0x10);	//  устанавливаем режим Repeated Mode

	U32 stH = 0xF8;
	writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x1, stH); // разрешаем измерение всего (V1-V8, Vcc и Tinternal)

	if(flt)
	{ // если включено усреднение, то требуется до 100 итераций, пока переходный процесс завершится
		U32 MSB,LSB;
		for( int i=0; i<8; i++ )
		{
			for( int j=0; j<16; j++ )
			{
				do{
					readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0xA+i*2, &MSB);
				}while(!(MSB & 0x80));
				readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0xB+i*2, &LSB);
			}
		}
	}
}

void Cfmc127srv::readSensors(CModule* pModule, U32 numb, U32 ctrl_mode, double* v_chan, double* cur_chan, double* tempr, double* vcc)
{
	int icnt = 0;

	U32 MSB,LSB;

	// Inputs V1-V8 (format D0-D13)
	for( int i=0; i<8; i++ )
	{
		icnt = 0;
		do{
			IPC_delay(10);
			readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0xA+i*2, &MSB);
			icnt++;
		}while(!(MSB & 0x80) && (icnt < 500));
		readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0xB+i*2, &LSB);
			
		//BRDC_printf(_BRDC("Chan%d (0x%X, %d): 0x%02X%02X\n"), i , numb, icnt, MSB, LSB);

		S32 D = LSB | ((MSB & 0x3F) << 8);
		double V = (MSB & 0x40) ? (16384 - D) : D;

		if(i & 1) 
		{
			cur_chan[i>>1] = (V * 0.019075)/1000;
		}
		v_chan[i] = (V * 0.30518)/1000;
	}
	
	// temperature internal (format D0-D12)
	icnt = 0;
	do{
		IPC_delay(10);
		readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x1A, &MSB);
		icnt++;
	}while(!(MSB & 0x80) && (icnt < 500));
	readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x1B, &LSB);

	//BRDC_printf(_BRDC("Tin (0x%X, %d): 0x%02X%02X\n"), numb, icnt, MSB, LSB);
		
	MSB = MSB & 0x1F;
	MSB = (MSB << 27) >>27;
		
	double Tin =  (S32)(LSB | (MSB << 8));
	*tempr = Tin * 0.0625;					

	//printf( "    Tin = %f \n", *tempr );

	// Vcc (format D0-D13)
	icnt = 0;
	do{
		IPC_delay(10);
		readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x1C, &MSB);
		icnt++;
	}while(!(MSB & 0x80) && (icnt < 500));
	readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x1D, &LSB);

	//BRDC_printf(_BRDC("Vcc (0x%X, %d): 0x%02X%02X\n"), numb, icnt, MSB, LSB);

	double V = LSB | ((MSB & 0x3F) << 8);
	V = (V * 0.30518)/1000;
	*vcc = 2.5 + V;

}

//***************************************************************************************
//int Cfmc127srv::CtrlSensMon(
//							void		*pDev,		// InfoSM or InfoBM
//							void		*pServData,	// Specific Service Data
//							ULONG		cmd,		// Command Code (from BRD_ctrl())
//							void		*args 		// Command Arguments (from BRD_ctrl())
//							)
//{
//	CModule* pModule = (CModule*)pDev;
//	PBRD_SensMonVal pMonVal = (PBRD_SensMonVal)args;
//	PFMC127SRV_CFG pBasefCfg = (PFMC127SRV_CFG)m_pConfig;
//
//	ULONG numb = pBasefCfg->AdrMonitor;
//	if(pMonVal->chip >= 1)
//		numb++;
//	if(pMonVal->chip >= 2)
//		numb++;
//
//	double _vchan[8];
//	double _tempr;
//	double _vcc;
//	double _curchan[4];
//
//	readSensors(pModule, numb, _vchan, &_tempr, &_vcc, _curchan);
//
//	if(pMonVal->chip == 0)
//	{
//		for(int i=0; i < 8; i++ )
//		{
//			double out = _vchan[i];
//
//			if( i == 0 || i == 1)
//				out *= 6;
//			//if( i == 2 || i == 3)
//				//out *= 2;
//
//			pMonVal->inp[i] = out;
//		}
//		pMonVal->cur[0] = _curchan[0]/0.05 * 6;
//		pMonVal->cur[1] = _curchan[1]/0.004;// * 2;
//		
//		// +++++++++++++++++++++++++++++++++
////		pMonVal->cur[0] = fabs(_vchan[0] - _vchan[1])/0.05;
////		pMonVal->cur[1] = fabs(_vchan[2] - _vchan[3])/0.004;
//	}
//
//	if(pMonVal->chip == 1 || pMonVal->chip == 2)
//	{
//		for(int i=0; i < 8; i++ )
//		{
//			double out = _vchan[i];// * 2;
//
//			if( i == 2 || i == 3)
//				out *= 6;
//
//			pMonVal->inp[i] = out;
//		}
//		pMonVal->cur[0] = _curchan[0]/0.004;// * 2;
//		pMonVal->cur[1] = _curchan[1]/0.05 * 6;
//		pMonVal->cur[2] = _curchan[2]/0.004;// * 2;
//
//	//	pMonVal->cur[0] = fabs(_vchan[0] - _vchan[1])/0.004;
//	//	pMonVal->cur[1] = fabs(_vchan[2] - _vchan[3])/0.05;
//	//	pMonVal->cur[2] = fabs(_vchan[4] - _vchan[5])/0.004;
//
//	}
//
//	pMonVal->tint = _tempr;
//	pMonVal->vcc = _vcc;
//
//	return BRDerr_OK;
//}
//
//#define STATUS_LOW 0
//#define STATUS_HIGH 1
//
//static bool init = false;

//void Cfmc127srv::readSensors(CModule* pModule, U32 numb, double* vchan, double* tempr, double* vcc, double* cur_chan)
//{
//	U32 regVal;
//
////	readSpdDev(pModule, SENSMON_DEVID, numb, 0, STATUS_LOW, &regVal );
//	
//	U32 stH;
//
////	if( !init )
//	{
//		//Sleep(10);
//		//writeSpdDev(pModule, SENSMON_DEVID, numb, 0, STATUS_HIGH, 0);
//		//init = true;
//		//Sleep(10);
//
//		writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x6, 0x88);
//		//IPC_delay(10);
//		writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x7, 0x88);
//		//IPC_delay(10);
//		//IPC_delay(100);
///*		for( int i=0; i<5; i++ )
//		{
//			IPC_delay(1000);
//			//BRDC_printf(_BRDC("delay %d sec\r"), i);		
//		}*/
//		////writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x6, 0);
//		//IPC_delay(10);
//		//writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x7, 0);
//		//IPC_delay(10);
//		//IPC_delay(100);
//		writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x8, 0x10);
//		//IPC_delay(10);
//
//	}
//	
////		Sleep(10);
////	readSpdDev(pModule, SENSMON_DEVID, numb, 0, STATUS_HIGH, &stH);
////		Sleep(10);
//
//	U32 stH_val = 0;
//	int icnt = 0;
////	if( (stH & 0xF8) != 0xF8 )
//	{
//		stH = 0xF8;
//		writeSpdDev(pModule, SENSMON_DEVID, numb, 0, STATUS_HIGH, stH);
//		//do{
//		//	Sleep(10);
//		//	readSpdDev(pModule, SENSMON_DEVID, numb, 0, STATUS_HIGH, &stH);
//		//	icnt++;
//		//}while((stH & 0xF8) && (icnt < 500));
//	}	
//	//BRDC_printf(_BRDC("Waiting Status High: %d\n"), icnt);
//
//	IPC_delay(10);
//	readSpdDev(pModule, SENSMON_DEVID, numb, 0, STATUS_LOW, &regVal );
//		//Sleep(1000);
////	if( (regVal & 0xFF) == 0 )
//	//	return;
//
//	U32 MSB,LSB;
//
//	for( int i=0; i<8; i++ )
//	{
//		for( int j=0; j<16; j++ )
//		{
//			//icnt = 0;
//			//if( regVal & (1 <<i) )
//			{
//				do{
//					//IPC_delay(10);
//					readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0xA+i*2, &MSB);
//					//icnt++;
//				//}while(!(MSB & 0x80) && (icnt < 500));
//				}while(!(MSB & 0x80));
//				readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0xB+i*2, &LSB);
//			
//				//BRDC_printf(_BRDC("Try%d Chan%d (0x%X, %d): 0x%02X%02X\n"), j, i , numb, icnt, MSB, LSB);
//			
//			}
//		}
//	}
//
//	for( int i=0; i<8; i++ )
//	{
//		icnt = 0;
//		//if( regVal & (1 <<i) )
//		{
//			do{
//				IPC_delay(10);
//				readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0xA+i*2, &MSB);
//				icnt++;
//			}while(!(MSB & 0x80) && (icnt < 500));
//			readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0xB+i*2, &LSB);
//			
//			//BRDC_printf(_BRDC("Chan%d (0x%X, %d): 0x%02X%02X\n"), i , numb, icnt, MSB, LSB);
//
//			MSB = MSB & 0x3F;
//			
//			double V = LSB | (MSB << 8);
//
//			V = (V * 0.30518)/1000;
//
//			vchan[i] = V;
//		}
//	}
//	
//	//int ch = IPC_getch(); // получает клавишу
//
//	{	// temperature internal
//		icnt = 0;
//		do{
//			IPC_delay(10);
//			readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x1A, &MSB);
//			icnt++;
//		}while(!(MSB & 0x80) && (icnt < 500));
//		readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x1B, &LSB);
//
//		//BRDC_printf(_BRDC("Tin (0x%X, %d): 0x%02X%02X\n"), numb, icnt, MSB, LSB);
//		
//		MSB = MSB & 0x1F;
//		MSB = (MSB << 27)>>27;
//		
//		double Tin =  (S32)(LSB | (MSB << 8));
//
//		Tin = Tin * 0.0625;					
//
//		*tempr = Tin;
//		//printf( "    Tin = %f \n", Tin );
//	}
//
//	{	// Vcc
//		icnt = 0;
//		do{
//			IPC_delay(10);
//			readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x1C, &MSB);
//			icnt++;
//		}while(!(MSB & 0x80) && (icnt < 500));
//		readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x1D, &LSB);
//
//		//BRDC_printf(_BRDC("Vcc (0x%X, %d): 0x%02X%02X\n"), numb, icnt, MSB, LSB);
//
//		MSB = MSB & 0x3F;
//			
//		double V = LSB | (MSB << 8);
//
//		V = (V * 0.30518)/1000;
//		
//		*vcc = 2.5 + V;
//	}
//
//	writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x6, 0x99);
//	IPC_delay(10);
//	writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x7, 0x99);
//	IPC_delay(10);
//	//IPC_delay(1000);
//	for( int i=0; i<5; i++ )
//	{
//		IPC_delay(1000);
//		//BRDC_printf(_BRDC("delay %d sec\r"), i);		
//	}
//
//	//writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x6, 0x11);
//	//IPC_delay(10);
//	//writeSpdDev(pModule, SENSMON_DEVID, numb, 0, 0x7, 0x11);
//	//IPC_delay(10);
//	//IPC_delay(100);
//
//	for( int i=0; i<4; i++ )
//	{
//		icnt = 0;
//		//if( regVal & (1 <<i) )
//		{
//			do{
//				IPC_delay(10);
//				readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0xC+i*4, &MSB);
//				icnt++;
//			}while(!(MSB & 0x80) && (icnt < 500));
//			readSpdDev(pModule, SENSMON_DEVID, numb, 0, 0xD+i*4, &LSB);
//			
//			//BRDC_printf(_BRDC("DifChan%d (0x%X, %d): 0x%02X%02X\n"), i , numb, icnt, MSB, LSB);
//			
//			S32 D = LSB | ((MSB & 0x3F) << 8);
//
//			double V = (MSB & 0x40) ? (16384 - D) : D;
//
//			V = (V * 0.019075)/1000;
//
//			cur_chan[i] = V;
//		}
//	}
//	//stH = 0;
//	//	Sleep(10);
//	//readSpdDev(pModule, SENSMON_DEVID, numb, 0, STATUS_HIGH, &stH);
//	//	Sleep(10);
//
//}

// Clock Mux register (0x218)
typedef union _MAIN_CLKMUX {
  ULONG AsWhole; // Main MUX Register as a Whole Word
  struct { // Main MUX Register as Bit Pattern
   ULONG Out		: 3, // output (0-7)
		 Res		: 1, // reserved
		 Inp		: 3, // input (0-7)
		 Res1		: 1, // reserved
		 Res2		: 8; // reserved
  } ByBits;
} MAIN_CLKMUX, *PMAIN_CLKMUX;

#define MAINnr_CLKMUX 0x218

//***************************************************************************************
int Cfmc127srv::CtrlClkSwitch(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
						   )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Fmc127pSwitch pIoSwitch = (PBRD_Fmc127pSwitch)args;
	PFMC127SRV_CFG pBasefCfg = (PFMC127SRV_CFG)m_pConfig;
	
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;

	param.reg = MAINnr_CLKMUX;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	if(param.val != 0x5858)
		return BRDerr_CMD_UNSUPPORTED;

	param.val = 0;
	PMAIN_CLKMUX pMux = (PMAIN_CLKMUX)&param.val;

	if(BRDctrl_FMC127P_SETSWITCH == cmd)
	{
		if(pIoSwitch->out < 4 && pIoSwitch->inp > 3)
			return BRDerr_BAD_PARAMETER; // на выходы 0-3 коммутироваться могут только входы 0-3
		if(pIoSwitch->out > 3 && pIoSwitch->inp < 4)
			return BRDerr_BAD_PARAMETER; // на выходы 4-7 коммутироваться могут только входы 4-7

		pMux->ByBits.Out = pIoSwitch->out;
		pMux->ByBits.Inp = pIoSwitch->inp;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		pBasefCfg->SwichtInp[pIoSwitch->out] = pIoSwitch->inp;
	}
	else
	{
		pIoSwitch->inp = pBasefCfg->SwichtInp[pIoSwitch->out];
	}
	return BRDerr_OK;
}

//***************************************************************************************
int Cfmc127srv::writeSpdDev(CModule* pModule, ULONG dev, ULONG num, ULONG synchr, ULONG reg, U32 val)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// выбираем устройство
	param.reg = MAIN17nr_SPDDEVICE; // 0x203
	param.val = dev;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем адрес
	param.reg = MAIN17nr_SPDADDR; // 0x205
	param.val = reg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем данные
	param.reg = MAIN17nr_SPDDATAL; // 0x206  
	param.val = val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// посылаем команду записи
	param.val = (synchr << 12) | (num << 4) | 0x2; // write
	param.reg = MAIN17nr_SPDCTRL; // 0x204
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	return BRDerr_OK;
}

//***************************************************************************************
int Cfmc127srv::readSpdDev(CModule* pModule, ULONG dev, ULONG num, ULONG synchr, ULONG reg, U32* pVal)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// выбираем устройство
	param.reg = MAIN17nr_SPDDEVICE; // 0x203
	param.val = dev;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем адрес
	param.reg = MAIN17nr_SPDADDR; // 0x205
	param.val = reg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// посылаем команду чтения
	param.val = (synchr << 12) | (num << 4) | 0x1; // read
	param.reg = MAIN17nr_SPDCTRL; // 0x204
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// считываем данные
	param.reg = MAIN17nr_SPDDATAL; // 0x206;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	*pVal = param.val;

	return BRDerr_OK;
}

//***************************************************************************************
int Cfmc127srv::CtrlClkMode(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
				   )
{
	CModule* pModule = (CModule*)pDev;
	PFMC127SRV_CFG pBasefCfg = (PFMC127SRV_CFG)m_pConfig;
	PBRD_Fmc127pClkMode clk_mode = (PBRD_Fmc127pClkMode)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MainTetrNum;

	param.reg = MAINnr_CMPMUX;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PMAIN_MUX pMux = (PMAIN_MUX)&param.val;

	if(BRDctrl_FMC127P_SETCLKMODE == cmd)
	{
		pMux->ByBits.GenEn = clk_mode->genEn;
		pMux->ByBits.CmpEn = clk_mode->clkInCmpEn;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//		if(pBasefCfg->Gen0Type && clk_mode->clkInValue && (clk_mode->genEn == 1))
//			Si571SetClkVal( pModule, &clk_mode->clkInValue);
		if (!clk_mode->clkInValue)
		{	// если задано НУЛЕВОЕ значение частоты
			clk_mode->clkInValue = (REAL64)pBasefCfg->RefGen0; // возвращаем частоту, записанную в ППЗУ
		}
		else
		{	// если задано НЕНУЛЕВОЕ значение частоты
			if (pBasefCfg->Gen0Type)
			{ // если установлен программируемый внутренний генератор
				Si571SetClkVal(pModule, &clk_mode->clkInValue); // программируем внутренний генератор
			}
			else
			{ // если установлен НЕпрограммируемый внутренний генератор
				if (clk_mode->clkInValue != (REAL64)pBasefCfg->RefGen0)
				{
					clk_mode->clkInValue = (REAL64)pBasefCfg->RefGen0; // возвращаем частоту, записанную в ППЗУ
					return BRDerr_PARAMETER_CHANGED;
				}
			}
		}
	}
	else
	{
		clk_mode->genEn = pMux->ByBits.GenEn;
		clk_mode->clkInCmpEn = pMux->ByBits.CmpEn;
//		if(pBasefCfg->Gen0Type && (clk_mode->genEn == 1))
//			Si571GetClkVal( pModule, &clk_mode->clkInValue);
		if (pBasefCfg->Gen0Type)
			Si571GetClkVal(pModule, &clk_mode->clkInValue);
		else
			clk_mode->clkInValue = pBasefCfg->RefGen0;
	}
	return BRDerr_OK;
}

//***************************************************************************************
int	Cfmc127srv::Si571SetClkVal( CModule* pModule, double *pRate )
{
	PFMC127SRV_CFG pSrvCfg = (PFMC127SRV_CFG)m_pConfig;
	U32			regData[20];
	int			regAdr;
	int			status = BRDerr_OK;

	IPC_delay(10);
	IPC_delay(10);

	// Скорректировать частоту, если необходимо
	if( *pRate > (double)pSrvCfg->RefMaxGen0 )
	{
		*pRate = (double)pSrvCfg->RefMaxGen0;
		status = BRDerr_WARN;
	}

    SI571_SetRate( pRate, pSrvCfg->dGenFxtal, (U32*)regData );
	//if(ret < 0)
	//	printf( "Error SI571_SetRate\n");

	// Запрограммировать генератор
	writeSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen0, 0, 137, 0x10); // Freeze DCO
	for( regAdr=7; regAdr<=18; regAdr++ )
		writeSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen0, 0, regAdr, regData[regAdr]);
	writeSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen0, 0, 137, 0x0); // Unfreeze DCO
	writeSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen0, 0, 135, 0x40 ); // Assert the NewFreq bit
	IPC_delay(300);

	// Считать регистры Si570/Si571
	//for( regAdr=7; regAdr<=12; regAdr++ )
	//	readSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen0, 0, regAdr, &regData[regAdr]);
	//printf( "Read Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n", regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );
	//SI571_SetRate( pRate, pSrvCfg->dGenFxtal, (U32*)regData );

//	printf( "Write Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n", regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );
//	printf( ">> XTAL = %f kHz\n", pSrvCfg->dGenFxtal/1000.0 );
//	printf( ">> Rate = %f kHz\n", *pRate/1000.0 );

	// из-за I2C-коммутатора интерфейс виснет при отключении питания генератора, поэтому коммутатор переключаем на другое устройство
	writeSpdDev(pModule, SENSMON_DEVID, pSrvCfg->AdrMonitor, 0, 0x6, 0);

	return status;	
}

//***************************************************************************************
int	Cfmc127srv::Si571GetClkVal( CModule* pModule, double *pRate )
{
	PFMC127SRV_CFG pSrvCfg = (PFMC127SRV_CFG)m_pConfig;
	U32			regData[20];
    //ULONG		clkSrc;
	int			regAdr;

	*pRate = 0.0;

	// Считать регистры Si570/Si571
	for( regAdr=7; regAdr<=12; regAdr++ )
		readSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen0, 0, regAdr, &regData[regAdr]);

	SI571_GetRate( pRate, pSrvCfg->dGenFxtal, (U32*)regData );
	//printf( "Read Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n", regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );

	return BRDerr_OK;	
}

//***************************************************************************************
int Cfmc127srv::CtrlUserEeprom(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PFMC127SRV_CFG pSrvCfg = (PFMC127SRV_CFG)m_pConfig;
	if(!pSrvCfg->isUserEeprom)
		return BRDerr_INSUFFICIENT_RESOURCES;
	PBRD_UserEeprom pUserRom = (PBRD_UserEeprom)args;
	if(BRDctrl_FMC127P_WRITEUROM == cmd)
	{
		WriteUserEeprom(pModule, pUserRom->pBuf, pUserRom->size, pUserRom->offset);
	}
	else
	{
		ReadUserEeprom(pModule, pUserRom->pBuf, pUserRom->size, pUserRom->offset);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int	Cfmc127srv::WriteUserEeprom( CModule* pModule, PVOID pBuffer, ULONG BufferSize, ULONG Offset)
{
	ULONG adr = Offset;
	U08* memdata =  (U08*)pBuffer;
	for(ULONG i = 0; i < BufferSize; i++ )
	{
		int page = adr / 256;
		U32 buf = memdata[i];
		writeSpdDev(pModule, EEPROM_DEVID, EEPROM_NUMB+page, 0, adr++, buf);
		IPC_delay(1);
		IPC_delay(4);
	}
	return BRDerr_OK;	
}

//***************************************************************************************
int	Cfmc127srv::ReadUserEeprom( CModule* pModule, PVOID pBuffer, ULONG BufferSize, ULONG Offset)
{
	ULONG adr = Offset;
	U08* memdata =  (U08*)pBuffer;
	for(int i = 0; i < BufferSize; i++ )
	{
		int page = adr / 256;
		U32 buf;
		readSpdDev(pModule, EEPROM_DEVID, EEPROM_NUMB+page, 0, adr++, &buf);
		memdata[i] = (U08)buf;
	}
	return BRDerr_OK;	
}

// ***************** End of file BasefSrv.cpp ***********************
