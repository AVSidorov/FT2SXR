/*
 ***************** File Qm9857Srv.cpp ************
 *
 * BRD Driver for QM service on ADMQM9857
 *
 * (C) InSys by Sklyarov A. Sep 2006
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "qm9857srv.h"

#define	CURRFILE "QM9857SRV"

//***************************************************************************************
//***************************************************************************************
int CQm9857Srv::CtrlRelease(
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
void CQm9857Srv::GetQmTetrNum(CModule* pModule)
{
	m_QmTetrNum = GetTetrNum(pModule, m_index, QM9857_TETR_ID);
}
//***************************************************************************************
//***************************************************************************************
int CQm9857Srv::WriteQmReg(CModule* pModule, void *args)
{
	PBRD_QmSetReg ptr=(PBRD_QmSetReg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_MODE3 pMode3 = (PQM_MODE3)&param.val;
	pMode3->ByBits.ProgMask = ptr->progMask;		// выбираем микросхемы QM для записи
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);

	param.reg = QM9857nr_WRITEDATA;
	PQM_CHIPDATA pData =(PQM_CHIPDATA)&param.val;
	pData->ByBits.Data = ptr->data;
	pData->ByBits.Addr = ptr->addrReg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	return BRDerr_OK;

}

//***************************************************************************************
int CQm9857Srv::ReadQmReg(CModule* pModule,  void *args)
{
	PBRD_QmSetReg ptr=(PBRD_QmSetReg)args;
	U32 cm;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_MODE3 pMode3 = (PQM_MODE3)&param.val;
	cm=ptr->progMask&3;
	if(cm==3)cm=2;
	pMode3->ByBits.ProgMask = cm;		// выбираем микросхемы QM для чтения
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);

	param.reg = QM9857nr_WRITEDATA;
	PQM_CHIPDATA pData =(PQM_CHIPDATA)&param.val;
	pData->ByBits.Data = 0;
	pData->ByBits.Addr = ptr->addrReg|0x80;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = QM9857nr_READDATA;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	ptr->data=(param.val>>8)&0xff;
	return BRDerr_OK;
	
}

//***************************************************************************************

int CQm9857Srv::QmReset(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = QM9857nr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_CONTROL1 pCtrl = (PQM_CONTROL1)&param.val;
	pCtrl->ByBits.ChipReset = 1;		// подаем сброс  на микросхему
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);

	pCtrl->ByBits.ChipReset = 0;		// снимаем  сброс  с микросхемы
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	return BRDerr_OK;

}
//***************************************************************************************
int CQm9857Srv::QmResetFull(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = QM9857nr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_MODE0 pCtrl = (PQM_MODE0)&param.val;
	pCtrl->ByBits.Reset = 1;		// подаем сброс  
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);

	pCtrl->ByBits.Reset = 0;		// снимаем  сброс  
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	return BRDerr_OK;

}


//***************************************************************************************
int CQm9857Srv::SelectProfile(CModule* pModule, U32 numProfile)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = QM9857nr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_CONTROL1 pCtrl = (PQM_CONTROL1)&param.val;
	pCtrl->ByBits.SelProf = numProfile;		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	return BRDerr_OK;
}

//***************************************************************************************
int CQm9857Srv::QmUpdateFreq(CModule* pModule, U32 chipMask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = QM9857nr_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_CONTROL1 pCtrl = (PQM_CONTROL1)&param.val;
	pCtrl->ByBits.FreqUpd = chipMask;		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);
	return BRDerr_OK;
}
//*************************************************************************************
int CQm9857Srv::SetProfile(CModule* pModule,void *args)
{
	PBRD_QmSetProfile ptr=(PBRD_QmSetProfile)args;
	U32	reg0, reg1, reg2, reg3, reg4, reg5;
	U32	val0, val1, val2, val3, val4, val5;
	U32	prof = (ptr->profNum)&3;
	U32	freq = ptr->freqNco;
	ULONG	ptef;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_MODE3 pMode3 = (PQM_MODE3)&param.val;
	pMode3->ByBits.ProgMask = ptr->progMask;		// выбираем микросхемы QM для записи
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);

	reg0 = 2 + prof*6;
	reg1 = 3 + prof*6;
	reg2 = 4 + prof*6;
	reg3 = 5 + prof*6;
	reg4 = 6 + prof*6;
	reg5 = 7 + prof*6;

	val0 = (freq >> 0)& 0xFF;
	val1 = (freq >> 8)& 0xFF;
	val2 = (freq >> 16)&0xFF;
	val3 = (freq >> 24)&0xFF;

	val4 = (ptr->cicRate&0x3F) << 2;
	val4 |= (ptr->spectrInv&1)    << 1;
	val4 |= (ptr->invCicBypass&1)   << 0;

	val5 = ptr->outputScale&0xFF;

	param.reg = QM9857nr_WRITEDATA;
	param.val = (reg0<<8)|val0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = (reg1<<8)|val1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val =  (reg2<<8)|val2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val =  (reg3<<8)|val3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val =  (reg4<<8)|val4;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val =  (reg5<<8)|val5;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;

}
//*************************************************************************************
int CQm9857Srv::SetMainParam(CModule* pModule,void *args)
{
	PBRD_QmSetMainParam ptr=(PBRD_QmSetMainParam)args;
	U32		mode3, val0, val1, wr;
	U32		rcm = ptr->clockMult;
	ULONG	ptef;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PQM_MODE3 pMode3 = (PQM_MODE3)&param.val;
	pMode3->ByBits.ProgMask = ptr->progMask;		// выбираем микросхемы QM для записи
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);


	val0 = (ptr->pllLock &1)  <<  5;
	
	if( rcm<4 )			rcm = 1;
	else if( rcm>20 )	rcm = 20;

	val0 |= rcm;
	val0 |= 0x80;

	val1 = (ptr->cicClear&1)		<< 7;
	val1 |= (ptr->invSincBypass&1)	<< 6;
	val1 |= (ptr->fullSleep&1)		<< 3;
	val1 |= (ptr->autoPowerDown&1)	<< 2;
	val1 |= (ptr->operatingMode&3)  << 0;

	param.reg = QM9857nr_WRITEDATA;
	param.val = val0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = 0x100 | val1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;

}
//*************************************************************************************


//***************************************************************************************
//***************************************************************************************
int CQm9857Srv::SetChanMask(CModule* pModule, ULONG mask)
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
int CQm9857Srv::GetChanMask(CModule* pModule, ULONG& mask)
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
int CQm9857Srv::GetStartMode(CModule* pModule, PVOID pStMode)
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
int CQm9857Srv::SetStartMode(CModule* pModule, PVOID pStMode)
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
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			return BRDerr_OK;
		}else
			return BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
	}

}

//***************************************************************************************
int CQm9857Srv::SetClkSource(CModule* pModule, U32 ClkSrc)
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
int CQm9857Srv::GetClkSource(CModule* pModule, U32& ClkSrc)
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
int CQm9857Srv::QmProg(CModule* pModule)
{
	DEVS_CMD_Reg param;
	ULONG pref=1;
	param.idxMain = m_index;
	param.tetr = ADM2IFnt_MAIN;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	param.val |=1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val &=~1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.tetr = m_QmTetrNum;
	param.reg = ADM2IFnr_STATUS;
	while(pref==1)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		pref=(param.val>>12)&1;
	}
	return BRDerr_OK;
}

//***************************************************************************************
