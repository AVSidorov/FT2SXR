/*
 ***************** File qm9957Srv.cpp ************
 *
 * BRD Driver for QM service 
 *
 * (C) InSys by Sklyarov A. Nov 2010
 *
 ******************************************************
*/

#include "gipcy.h"
#include "module.h"
#include "qm9957Srv.h"

#define	CURRFILE "QM9957SRV"

//***************************************************************************************
//***************************************************************************************
int CQm9957Srv::CtrlRelease(
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
void CQm9957Srv::GetDucTetrNum(CModule* pModule)
{
	m_QmTetrNum = GetTetrNum(pModule, m_index, QM9957_TETR_ID);
}
//***************************************************************************************
//***************************************************************************************
int CQm9957Srv::InitAD9957(void)
{

	CfgAD9957.RegCFR1 = 0x00002000;
//	CfgAD9957.RegCFR2 = 0x00400820;
	CfgAD9957.RegCFR2 = 0x01400820;
//	CfgAD9957.RegCFR2 = 0x00400860;
	CfgAD9957.RegCFR3 = 0x1f3fc000;
	CfgAD9957.RegADCR = 0x00007f7f;
	CfgAD9957.RegIOUR = 0xffffffff;
	CfgAD9957.RegRSR1 = 0x00000000;
	CfgAD9957.RegRSR2 = 0x00000000;
	CfgAD9957.RegASFR = 0x00000000;
	CfgAD9957.RegMSR =  0x0c000000; //0x00000000;//0x08000000;
	CfgAD9957.RegProfile[0] = 0x0000000000000000;
	CfgAD9957.RegProfile[1] = 0x0000000000000000;
	CfgAD9957.RegProfile[2] = 0x0000000000000000;
	CfgAD9957.RegProfile[3] = 0x0000000000000000;
	CfgAD9957.RegProfile[4] = 0x0000000000000000;
	CfgAD9957.RegProfile[5] = 0x0000000000000000;
	CfgAD9957.RegProfile[6] = 0x0000000000000000;
	CfgAD9957.RegProfile[7] = 0x0000000000000000;
	CfgAD9957.RegRAMR = 0x00000000;
	CfgAD9957.RegGPIOC = 0x0000;
	CfgAD9957.RegGPIOD = 0x0000;

	return BRDerr_OK;
}
//***************************************************************************************
int CQm9957Srv::SetClkSource(CModule* pModule, U32 ClkSrc)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_FSRC;
	if(ClkSrc==0)	// internal
	{ 
		param.val = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
	{				// external
		param.val = 0x81;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CQm9957Srv::GetClkSource(CModule* pModule, U32& ClkSrc)
{
	ULONG source;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_FSRC;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	source = param.val&1;
	if(source==0)ClkSrc = 0;
	else ClkSrc=1;
	return BRDerr_OK;
}

//***************************************************************************************

//***************************************************************************************
int CQm9957Srv::SetChanMask(CModule* pModule, ULONG mask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_CHAN1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_CHAN1 pChan1 = (PADM2IF_CHAN1)&param.val;
	pChan1->ByBits.ChanSel = mask;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CQm9957Srv::GetChanMask(CModule* pModule, ULONG& mask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_CHAN1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_CHAN1 pChan1 = (PADM2IF_CHAN1)&param.val;
	mask = pChan1->ByBits.ChanSel;

	return BRDerr_OK;
}

//***************************************************************************************

int	CQm9957Srv::WriteRegAD9957(CModule* pModule,int addr,int nswords,__int64 data,U32 chipMask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_MODE3 pMode3 = (PQM_MODE3)&param.val;
	pMode3->ByBits.chipMask = chipMask;		// выбираем микросхемы QM для записи
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);

	param.reg = QM9957nr_DATAREG3; 
	param.val = (data>>48)& 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = QM9957nr_DATAREG2; 
	param.val = (data>>32)& 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = QM9957nr_DATAREG1; 
	param.val = (data>>16)& 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = QM9957nr_DATAREG0; 
	param.val = data & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	U32 nbits;
	if(nswords==2)nbits=32;
	else nbits= 64;

	param.reg = QM9957nr_ADDRREG; 
	param.val=addr  | (nbits <<8);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);
	return 1;
}
//***************************************************************************************
int	CQm9957Srv::WriteRegsAD9957(CModule* pModule,PAD9957_CFG pcfg,U32 chipMask)
{
 
	WriteRegAD9957(pModule,0,2,(__int64)pcfg->RegCFR1,chipMask);	
	WriteRegAD9957(pModule,1,2,(__int64)pcfg->RegCFR2,chipMask);	
	WriteRegAD9957(pModule,2,2,(__int64)pcfg->RegCFR3,chipMask);	
//	WriteRegAD9957(pModule,3,2,(__int64)pcfg->RegADCR,chipMask);	
//	WriteRegAD9957(pModule,4,2,(__int64)pcfg->RegIOUR,chipMask);	
//	WriteRegAD9957(pModule,5,2,(__int64)pcfg->RegRSR1,chipMask);	
//	WriteRegAD9957(pModule,6,2,(__int64)pcfg->RegRSR2,chipMask);	
//	WriteRegAD9957(pModule,9,2,(__int64)pcfg->RegASFR,chipMask);	
	WriteRegAD9957(pModule,0xa,2,(__int64)pcfg->RegMSR,chipMask);	
//	for(int i=0; i<8; i++)
//	{
//		WriteRegAD9957(pModule,0xE+i,4,pcfg->RegProfile[i]);	
		WriteRegAD9957(pModule,0xe,4,pcfg->RegProfile[0],chipMask);	
//	}
//	WriteRegAD9957(pModule,0x16,2,(__int64)pcfg->RegRAMR, chipMask);	
//	WriteRegAD9957(pModule,0x18,1,(__int64)pcfg->RegGPIOC,chipMask);	
//	WriteRegAD9957(pModule,0x19,1,(__int64)pcfg->RegGPIOD,chipMask);	
	return 1;
}
//***************************************************************************************

int	CQm9957Srv::ReadRegAD9957(CModule* pModule,int addr,int nswords,__int64 *data,U32 chipMask)
{
	__int64 Data;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE3;

	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_MODE3 pMode3 = (PQM_MODE3)&param.val;
	pMode3->ByBits.chipMask = chipMask;		// выбираем микросхемы QM для записи
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	
    IPC_delay(1);
	U32 nbits;
	if(nswords==2)nbits=32;
	else nbits= 64;

	param.reg = QM9957nr_ADDRREG; 
	param.val=addr | 0x80 | (nbits <<8);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	param.reg = QM9957nr_DATAREG0; 
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	Data = param.val;
	param.reg = QM9957nr_DATAREG1; 
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	Data |=((__int64)param.val)<<16;
	param.reg = QM9957nr_DATAREG2; 
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	Data |=((__int64)param.val)<<32;
	param.reg = QM9957nr_DATAREG3; 
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	Data |=((__int64)param.val)<<48;
	*data = Data;
	return 1;
}
//***************************************************************************************
int	CQm9957Srv::ReadRegsAD9957(CModule* pModule,PAD9957_CFG pcfg, U32 chipMask)
{
  
	ReadRegAD9957(pModule,0,2,(__int64*)&pcfg->RegCFR1,chipMask);	
	ReadRegAD9957(pModule,1,2,(__int64*)&pcfg->RegCFR2,chipMask);	
	ReadRegAD9957(pModule,2,2,(__int64*)&pcfg->RegCFR3,chipMask);	
//	ReadRegAD9957(pModule,3,2,(__int64*)&pcfg->RegADCR,chipMask);	
//	ReadRegAD9957(pModule,4,2,(__int64*)&pcfg->RegIOUR,chipMask);	
//	ReadRegAD9957(pModule,5,2,(__int64*)&pcfg->RegRSR1,chipMask);	
//	ReadRegAD9957(pModule,6,2,(__int64*)&pcfg->RegRSR2,chipMask);	
//	ReadRegAD9957(pModule,9,2,(__int64*)&pcfg->RegASFR,chipMask);	
//	ReadRegAD9957(pModule,0xa,2,(__int64*)&pcfg->RegMSR,chipMask);	
//	for(int i=0; i<8; i++)
	{
		ReadRegAD9957(pModule,0xe,4,&pcfg->RegProfile[0],chipMask);	
	}
//	ReadRegAD9957(pModule,0x16,2,(__int64*)&pcfg->RegRAMR,chipMask);	
	//ReadRegAD9957(pModule,0x18,1,(__int64*)&pcfg->RegGPIOC,chipMask);	
	//ReadRegAD9957(pModule,0x19,1,(__int64*)&pcfg->RegGPIOD,chipMask);	
	return 1;
}
//***************************************************************************************
int	CQm9957Srv::CompareRegsAD9957(CModule* pModule,PAD9957_CFG pcfg1,PAD9957_CFG pcfg2)
{
	int ret=0;
	U32 MaskCFR1=0xffffffff;
	U32 MaskCFR2=0xffffffff;
	U32 MaskCFR3=0xffffffff;
	U32 MaskADCR=0xffffffff;
	U32 MaskIOUR=0xffffffff;
	U32 MaskRSR1=0xffffffff;
	U32 MaskRSR2=0xffffffff;
	U32 MaskASFR=0xffffffff;
	U32 MaskMSR=0xffffffff;
	__int64 MaskProfile=0xffffffffffffffffULL;
	U32 MaskRAMR=0xffffffff;
	U32 MaskGPIOC=0xffffffff;
	U32 MaskGPIOD=0xffffffff;

	if((pcfg1->RegCFR1 & MaskCFR1)!= (pcfg2->RegCFR1 & MaskCFR1)) ret |=1<<0;
	if((pcfg1->RegCFR2 & MaskCFR2)!= (pcfg2->RegCFR2 & MaskCFR2)) ret |=1<<1;
	if((pcfg1->RegCFR3 & MaskCFR3)!= (pcfg2->RegCFR3 & MaskCFR3)) ret |=1<<2;
	if((pcfg1->RegADCR & MaskADCR)!= (pcfg2->RegADCR & MaskADCR)) ret |=1<<3;
	if((pcfg1->RegIOUR & MaskIOUR)!= (pcfg2->RegIOUR & MaskIOUR)) ret |=1<<4;
	if((pcfg1->RegRSR1 & MaskRSR1)!= (pcfg2->RegRSR1 & MaskRSR1)) ret |=1<<5;
	if((pcfg1->RegRSR2 & MaskRSR2)!= (pcfg2->RegRSR2 & MaskRSR2)) ret |=1<<6;
	if((pcfg1->RegASFR & MaskASFR)!= (pcfg2->RegASFR & MaskASFR)) ret |=1<<7;
	if((pcfg1->RegMSR & MaskMSR)!= (pcfg2->RegMSR & MaskMSR)) ret |=1<<8;
	
//	for(int i=0; i<8; i++)
	{
	 if((pcfg1->RegProfile[0] & MaskProfile)!= (pcfg2->RegProfile[0] & MaskProfile)) ret |=1<<(9);
	}	
	if((pcfg1->RegRAMR & MaskRAMR)!= (pcfg2->RegRAMR & MaskRAMR)) ret |=1<<16;
//	if((pcfg1->RegGPIOC & MaskGPIOC)!= (pcfg2->RegGPIOC & MaskGPIOC)) ret |=1<<17;
//	if((pcfg1->RegGPIOD & MaskGPIOD)!= (pcfg2->RegGPIOD & MaskGPIOD)) ret |=1<<18;
	return ret;
}
//***************************************************************************************
int	CQm9957Srv::ProgAD9957(CModule* pModule,PAD9957_CFG pcfg,U32 chipMask)
{
//	int ret;
	DEVS_CMD_Reg param;
//	AD9957_CFG cfgr;

	WriteRegsAD9957(pModule, pcfg,chipMask);

	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = QM9957nr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_CONTROL1 pCtrl = (PQM_CONTROL1)&param.val;
	pCtrl->ByBits.IO_Updata = chipMask;		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pCtrl->ByBits.IO_Updata = 0;		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);

	//ReadRegsAD9957(pModule,&cfgr);
	//ret=CompareRegsAD9957(pModule,pcfg,&cfgr);
	return 1;
}

//***************************************************************************************
int CQm9957Srv::WriteQmReg(CModule* pModule, void *args)
{
	PBRD_QmSetReg ptr=(PBRD_QmSetReg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_MODE3 pMode3 = (PQM_MODE3)&param.val;
	pMode3->ByBits.chipMask = ptr->chipMask;		// выбираем микросхемы QM для записи
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);


	param.reg = QM9957nr_DATAREG3; 
	param.val = ((ptr->dataH)>>16)& 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = QM9957nr_DATAREG2; 
	param.val = (ptr->dataH)& 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = QM9957nr_DATAREG1; 
	param.val = ((ptr->dataL)>>16)& 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = QM9957nr_DATAREG0; 
	param.val = (ptr->dataL)& 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	U32 nbits;
	if(ptr->nword==1)nbits=32;
	else nbits= 64;
	param.reg = QM9957nr_ADDRREG; 
	param.val=(ptr->addrReg & 0x1f)  | (nbits<<8);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);
	return BRDerr_OK;

}

//***************************************************************************************
int CQm9957Srv::ReadQmReg(CModule* pModule,  void *args)
{
	PBRD_QmSetReg ptr=(PBRD_QmSetReg)args;
//	U32 cm;
	U32 dataL,dataH;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_MODE3 pMode3 = (PQM_MODE3)&param.val;
	pMode3->ByBits.chipMask = ptr->chipMask;		// выбираем микросхемы QM для записи
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);

	U32 nbits;
	if(ptr->nword==1)nbits=32;
	else nbits= 64;

	param.reg = QM9957nr_ADDRREG; 
	param.val=(ptr->addrReg & 0x1f) | 0x80 | (nbits<<8);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);



	param.reg = QM9957nr_DATAREG0; 
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	dataL=param.val;
	param.reg = QM9957nr_DATAREG1; 
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	dataH=param.val;
	ptr->dataL=(dataL&0xffff) | ((dataH<<16)& 0xffff0000);
	param.reg = QM9957nr_DATAREG2; 
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	dataL=param.val;
	param.reg = QM9957nr_DATAREG3; 
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	dataH=param.val;
	ptr->dataH=(dataL&0xffff) | ((dataH<<16)& 0xffff0000);
	return BRDerr_OK;
	
}

//***************************************************************************************

int CQm9957Srv::QmMasterReset(CModule* pModule)
{

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = QM9957nr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_CONTROL1 pCtrl = (PQM_CONTROL1)&param.val;
	pCtrl->ByBits.MasterReset = 3;		// подаем сброс  на микросхему
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);
	pCtrl->ByBits.MasterReset = 0;		// снимаем  сброс  с микросхемы
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);
	return BRDerr_OK;

}
//***************************************************************************************
int CQm9957Srv::QmResetIO(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;

	param.reg = QM9957nr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_CONTROL1 pCtrl = (PQM_CONTROL1)&param.val;
	pCtrl->ByBits.IO_Reset = 3;		// подаем сброс  
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);
	pCtrl->ByBits.IO_Reset = 0;		// снимаем  сброс  
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);
	return BRDerr_OK;

}


//***************************************************************************************
int CQm9957Srv::SelectProfile(CModule* pModule, U32 numProfile)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = QM9957nr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_CONTROL1 pCtrl = (PQM_CONTROL1)&param.val;
	pCtrl->ByBits.SelProf = numProfile;		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);
	return BRDerr_OK;
}

//***************************************************************************************
int CQm9957Srv::QmUpdateIO(CModule* pModule, U32 chipMask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = QM9957nr_CONTROL1;
	
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_MODE3 pMode3 = (PQM_MODE3)&param.val;
	pMode3->ByBits.chipMask = chipMask;		// выбираем микросхемы QM для записи
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);

	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_CONTROL1 pCtrl = (PQM_CONTROL1)&param.val;
	pCtrl->ByBits.IO_Updata = chipMask;		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pCtrl->ByBits.IO_Updata = 0;		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);
	return 1;
}
//*************************************************************************************
int CQm9957Srv::SetSyncMode(CModule* pModule,void *args)
{
	PBRD_QmSetSyncMode ptr=(PBRD_QmSetSyncMode)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = QM9957nr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_CONTROL1 pCtrl = (PQM_CONTROL1)&param.val;
	pCtrl->ByBits.SyncMode = ptr->SyncMode;		
	pCtrl->ByBits.ipdclk = ptr->ipdclk;		
	pCtrl->ByBits.Master_Slave = ptr->Master_Slave;		
	pCtrl->ByBits.SyncEnable = ptr->SyncEnable;		
	pCtrl->ByBits.SyncSelect = ptr->SyncSelect;		
	pCtrl->ByBits.FD_ST = ptr->FD_ST;		
	pCtrl->ByBits.FD_CLK = ptr->FD_CLK;		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


    IPC_delay(1);
	return 1;
}
//*************************************************************************************
int CQm9957Srv::SetProfile(CModule* pModule,void *args)
{
	PBRD_QmSetProfile ptr=(PBRD_QmSetProfile)args;
	U32	prof = (ptr->profNum)&7;
	U32	phase = ptr->phaseNco;
	U32	freq = ptr->freqNco;
	U32	chipMask = ptr->chipMask;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
    IPC_delay(1);

	CfgAD9957.RegProfile[prof] = 0x0;
	CfgAD9957.RegProfile[prof] |= freq;
	CfgAD9957.RegProfile[prof] |= ((__int64)(phase & 0xffff))<<32;
	if(ptr->operMode==1)
	{
		CfgAD9957.RegProfile[prof] |= ((__int64)(ptr->ampScale & 0x3fff))<<48;

	}else
	{
		CfgAD9957.RegProfile[prof] |= ((__int64)(ptr->outScale & 0xff))<<48;
		CfgAD9957.RegProfile[prof] |= ((__int64)(ptr->invCicBypass & 1))<<56;
		CfgAD9957.RegProfile[prof] |= ((__int64)(ptr->spectrInv & 1))<<57;
		CfgAD9957.RegProfile[prof] |= ((__int64)(ptr->cicRate & 0x3f))<<58;
	}
	ptr->errors=ProgAD9957(pModule,&CfgAD9957,chipMask);
	if(ptr->errors!=0)return BRDerr_ERROR;
	return BRDerr_OK;

}
//*************************************************************************************
int CQm9957Srv::SetMainParam(CModule* pModule,void *args)
{
	PBRD_QmSetMainParam ptr=(PBRD_QmSetMainParam)args;
	U32	rcm = ptr->clockMult & 0x7f;
	U32	chipMask = ptr->chipMask;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
    IPC_delay(1);

	CfgAD9957.RegCFR1 &= ~(1<<22);
	CfgAD9957.RegCFR1 |= (ptr->invSyncEnb&1)<<22;

	CfgAD9957.RegCFR1 &= ~(3<<24);
	CfgAD9957.RegCFR1 |= (ptr->operMode&3)<<24;

	CfgAD9957.RegCFR3 &= ~(0xfe);
	CfgAD9957.RegCFR3 |= rcm<<1;

	CfgAD9957.RegCFR3 &= ~(0x7 << 24);
	CfgAD9957.RegCFR3 |= (ptr->vcoSel&7)<<24;

	CfgAD9957.RegCFR3 &= ~(0x7 << 19);
	CfgAD9957.RegCFR3 |= (ptr->chargePumpSel&7)<<19;

	CfgAD9957.RegCFR3 &= ~(1 << 8);
	CfgAD9957.RegCFR3 |= (ptr->pllEnb&1)<< 8;

	CfgAD9957.RegMSR &= ~(0x1f << 3);
	CfgAD9957.RegMSR |= (ptr->syncDelay & 0x1f)<< 3;

	ptr->errors=ProgAD9957(pModule,&CfgAD9957,chipMask);
	//if(ptr->errors!=0)return BRDerr_ERROR;

	return BRDerr_OK;

}
//*************************************************************************************


//***************************************************************************************
int CQm9957Srv::GetStartMode(CModule* pModule, PVOID pStMode)
{
	PBRD_StartMode pStartMode = (PBRD_StartMode)pStMode;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_STMODE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADM2IF_STMODE pStrMode = (PADM2IF_STMODE)&param.val;
		pStartMode->startSrc	= pStrMode->ByBits.SrcStart;
		pStartMode->trigStopSrc	= pStrMode->ByBits.SrcStop;
		pStartMode->startInv	= pStrMode->ByBits.InvStart;
		pStartMode->stopInv		= pStrMode->ByBits.InvStop;
		pStartMode->trigOn		= pStrMode->ByBits.TrigStart;

	}else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.reg = ADM2IFnr_STMODE;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
			PADM2IF_STMODE pStrMode = (PADM2IF_STMODE)&param.val;
			pStartMode->startSrc	= pStrMode->ByBits.SrcStart;
			pStartMode->trigStopSrc	= pStrMode->ByBits.SrcStop;
			pStartMode->startInv	= pStrMode->ByBits.InvStart;
			pStartMode->stopInv		= pStrMode->ByBits.InvStop;
			pStartMode->trigOn		= pStrMode->ByBits.TrigStart;
		}else
			return BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CQm9957Srv::SetStartMode(CModule* pModule, PVOID pStMode)
{

	PBRD_StartMode pStartMode = (PBRD_StartMode)pStMode;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	if(pMode0->ByBits.Master)
	{ // Single
		param.reg = ADM2IFnr_STMODE;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		PADM2IF_STMODE pStrMode = (PADM2IF_STMODE)&param.val;
		pStrMode->ByBits.SrcStart  = pStartMode->startSrc;
		pStrMode->ByBits.SrcStop   = pStartMode->trigStopSrc;
		pStrMode->ByBits.InvStart  = pStartMode->startInv;
		pStrMode->ByBits.InvStop   = pStartMode->stopInv;
		pStrMode->ByBits.TrigStart = pStartMode->trigOn;
		pStrMode->ByBits.Restart = pStartMode->reStartMode;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		return BRDerr_OK;
	}else
	{ // Master/Slave
		param.tetr = m_MainTetrNum;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pMode0->ByBits.Master)
		{ // Master
			param.reg = ADM2IFnr_STMODE;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
			PADM2IF_STMODE pStrMode = (PADM2IF_STMODE)&param.val;
			pStrMode->ByBits.SrcStart  = pStartMode->startSrc;
			pStrMode->ByBits.SrcStop   = pStartMode->trigStopSrc;
			pStrMode->ByBits.InvStart  = pStartMode->startInv;
			pStrMode->ByBits.InvStop   = pStartMode->stopInv;
			pStrMode->ByBits.TrigStart = pStartMode->trigOn;
			pStrMode->ByBits.Restart = pStartMode->reStartMode;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			return BRDerr_OK;
		}else
			return BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
	}

}
//***************************************************************************************
int CQm9957Srv::GetCfg(PVOID pCfg)
{
	PQM9957SRV_CFG pSrvCfg = (PQM9957SRV_CFG)m_pConfig;
	PBRD_QmCfg pQmCfg = (PBRD_QmCfg)pCfg;
	pQmCfg->FifoSize  = pSrvCfg->FifoSize;
	pQmCfg->MaxChans   = pSrvCfg->MaxChans;
	pQmCfg->SubIntClk = 10000000;// pSrvCfg->SubExtClk; Вернуться!!
	return BRDerr_OK;

}

//***************************************************************************************
//***************************************************************************************

