/*
 ***************** File 8gcSrv.cpp ************
 *
 * BRD Driver for 8GC service on ADMDDC216x250M
 *
 * (C) InSys by Sklyarov A. Apr. 2012
 *
 ******************************************************
*/

#include "module.h"
#include "8gcsrv.h"

#define	CURRFILE _BRDC("8GCSRV")

//***************************************************************************************
//***************************************************************************************
int C8gcSrv::CtrlRelease(
								void			*pDev,		// InfoSM or InfoBM
								void			*pServData,	// Specific Service Data
								ULONG			cmd,		// Command Code (from BRD_ctrl())
								void			*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	return BRDerr_CMD_UNSUPPORTED; 
}

//***************************************************************************************
void C8gcSrv::GetGcTetrNum(CModule* pModule)
{
	m_GcTetrNum = GetTetrNum(pModule, m_index,  SUBUNIT_TETR_ID );

}
//***************************************************************************************
//***************************************************************************************

int C8gcSrv::AdmGetInfo(CModule* pModule, PBRD_AdmGetInfo pAdmInfo)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	param.reg = CONST_SUBADM_ID;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->admId = param.val;

	param.reg = CONST_SUBPLD_ID;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->pldId = param.val;

	param.reg = CONST_SUBPLD_MODE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->pldModification = param.val;

	param.reg = CONST_SUBPLD_VER;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->pldVersion = param.val;

	param.reg = CONST_SUBPLD_BUILD;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->pldBuild = param.val;

	param.reg = CONST_SUBTRD_ID;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->trdId = param.val;

	param.reg = CONST_SUBTRD_MOD;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->trdIdModification = param.val;

	param.reg = CONST_SUBTRD_VER;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pAdmInfo->trdVersion = param.val;

	return BRDerr_OK;
}

//***************************************************************************************

//***************************************************************************************
int C8gcSrv::WriteGcReg(CModule* pModule, void *args)
{
	PBRD_DdcSetReg ptr=(PBRD_DdcSetReg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	param.reg = GCnr_MASK_DEVICE;
	param.val= 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	param.reg = GCnr_DDC_ADR_SHIFT+2;
	param.val = ptr->page;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// set page
	

	param.reg = GCnr_DDC_ADR_SHIFT+ptr->reg;
	param.val = ptr->data;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// write val to reg

    IPC_delay(1);
	return BRDerr_OK;

}
//***************************************************************************************
int C8gcSrv::ReadGcReg(CModule* pModule,  void *args)
{
	PBRD_DdcSetReg ptr=(PBRD_DdcSetReg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	param.reg = GCnr_MASK_DEVICE;
	param.val= 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	param.reg = GCnr_DDC_ADR_SHIFT+2;
	param.val = ptr->page;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// set page


	param.reg = param.reg = GCnr_DDC_ADR_SHIFT_R+ptr->reg;
	param.val= 0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	ptr->data= param.val;

	return BRDerr_OK;
}

//***************************************************************************************
//***************************************************************************************
int C8gcSrv::GetAdcOver(CModule* pModule,void *args)
{
	U32 *ptr=(U32*)args; 
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = GCnr_ADCOVER;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	*ptr=param.val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;

}
//***************************************************************************************
int C8gcSrv::SetDdcFc(CModule* pModule, void *args)
{
	
	PBRD_DdcSetFc pDdcSetFc=(PBRD_DdcSetFc)args;
	U32	 frPage,addr, chMask = pDdcSetFc->maskChans,ddcmode,chM;
	__int64	codefreq =0;
	unsigned int codeDphase;
	unsigned int codephase;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = GCnr_DDCMODE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	ddcmode=param.val;
	if(ddcmode>0)	// SplitIQ, DoubleRate
	{
		chM=0;
		if(chMask&1)chM=0x3;
		if(chMask&2)chM |=0xc;
	}else
	{
		chM=chMask;
	}
    codefreq=CalcCodeNcoFreq(ddcmode, pDdcSetFc->Fc,&codeDphase );

	/*setting mask_device DDC device*/
	param.reg = GCnr_MASK_DEVICE;
	param.val=0x1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);



	for(int i=0; i<4;i++)
	{
		if( (chM >> i) & 1 )
		{
			if(i<2)frPage=0x80;
			else frPage=0xa0;
/*		
			param.reg = GCnr_ADDR;
			param.val=(2<<8) | 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			param.reg = GCnr_WRITEDATA;
			param.val = frPage;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// set page
*/
			param.reg = GCnr_DDC_ADR_SHIFT+2;
			param.val = frPage;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// set page



/*
			if((i&1)==0)addr = (0x12<<8) | 1;
			else addr = (0x1a<<8) | 1;

			param.reg = GCnr_ADDR;
			param.val=addr;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			param.reg = GCnr_WRITEDATA;
			param.val =(U32)( codefreq&0xffff);						
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// 16 lsb
*/
			if((i&1)==0)addr = 0x12;
			else 	addr = 0x1a;

			param.reg = GCnr_DDC_ADR_SHIFT+addr;
			param.val = (U32)( codefreq&0xffff);
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// 16 lsb


/*
			param.reg = GCnr_ADDR;
			param.val=addr+(1<<8);
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			param.reg = GCnr_WRITEDATA;
			param.val =(U32)( (codefreq>>16)&0xffff);						
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// 16 mid
*/

			param.reg = GCnr_DDC_ADR_SHIFT+addr+1;
			param.val = (U32)( (codefreq>>16)&0xffff);
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// 16 mid
/*
			param.reg = GCnr_ADDR;
			param.val=addr+(2<<8);
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			param.reg = GCnr_WRITEDATA;
			param.val = (U32)((codefreq>>32)&0xffff);						
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// 16 msb
*/

			param.reg = GCnr_DDC_ADR_SHIFT+addr+2;
			param.val = (U32)( (codefreq>>32)&0xffff);
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// 16 msb




			if(ddcmode==2)// DoubleRate
			{
				if((i%2)==0)
				{
/*					addr = (0x11<<8) | 1;
					codephase=0;
					param.reg = GCnr_ADDR;
					param.val=addr;
					pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
					param.reg = GCnr_WRITEDATA;
					param.val = (U32)(codephase & 0xffff);						
					pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// 16 lsb
*/
				addr = 0x11;
				codephase=0;
				param.reg = GCnr_DDC_ADR_SHIFT+addr;
				param.val = (U32)( codephase&0xffff);
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// 16 lsb



				}
				if((i%2)==1)
				{
/*
					addr = (0x19<<8) | 1;
					codephase=codeDphase;
					param.reg = GCnr_ADDR;
					param.val=addr;
					pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
					param.reg = GCnr_WRITEDATA;
					param.val = (U32)(codephase & 0xffff);						
					pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// 16 lsb
*/
				addr = 0x19;
				codephase=codeDphase;
				param.reg = GCnr_DDC_ADR_SHIFT+addr;
				param.val = (U32)( codephase&0xffff);
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// 16 lsb


				}		
			}
		}
	}
	return BRDerr_OK;

}
//*************************************************************************************
//*************************************************************************************
int C8gcSrv::SetAdmMode(CModule* pModule,void *args)
{
	U32			*ptr = (U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = GCnr_ADMMODE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
//	param.val |= (*ptr)&1;

	param.val = (*ptr)&1;
//	param.val = (*ptr)&3;  // new 5.3.2011
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;

}
//*************************************************************************************
int C8gcSrv::SetDdcMode(CModule* pModule,void *args)
{
	U32			*ptr = (U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = GCnr_DDCMODE;
	param.val = (*ptr)&3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;

}
//*************************************************************************************
//=*********************************************************************************************************
__int64 C8gcSrv::CalcCodeNcoFreq(int ddcmode,U32 freq, unsigned int *codeDphase)
{
  	  __int64 code=0x1000000000000LL;
      unsigned int mph=0x8000;
	  long double SystemClock=200000000;
	  long double fh  =(-1)*(long double)freq;
	  unsigned int hexDph=0;
      long double dph;
	  if(ddcmode==2)// DoubleRate
	  {
			fh *=2;

            dph=fh/(long double)m_SystemClock;
            hexDph=dph *mph;

	  }
	  fh /= (long double)m_SystemClock;
//	  fh /= SystemClock;
  	  code=code*fh;
	  *codeDphase=hexDph;
	  return code;
}

//***************************************************************************************
//***************************************************************************************
int C8gcSrv::SetChanMask(CModule* pModule, ULONG mask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = ADM2IFnr_CHAN1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_CHAN1 pChan1 = (PADM2IF_CHAN1)&param.val;
	pChan1->ByBits.ChanSel = mask;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = GCnr_ADR_HIGH;///* ZEROing _ADR_HIGH*/
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	/*ADC0 On always on!*/
	param.reg = GCnr_MASK_DEVICE;
	param.val = ( 0x1 )<<4;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg =  GCnr_DDC_ADR_SHIFT + Reg_ADC_MODES;
	param.val =  0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	/* Off\On ADC1 */
	param.reg = GCnr_MASK_DEVICE;
	param.val = ( 0x2 )<<4;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = GCnr_DDC_ADR_SHIFT + Reg_ADC_MODES;
	param.val =  1; // off
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	if( ((mask &0x2)) > 0)
	{
		param.val =  0; // on
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}

	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::GetChanMask(CModule* pModule, ULONG& mask)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = ADM2IFnr_CHAN1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_CHAN1 pChan1 = (PADM2IF_CHAN1)&param.val;
	mask = pChan1->ByBits.ChanSel;



	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::GetStartMode(CModule* pModule, PVOID pStMode)
{
	PBRD_StartMode pStartMode = (PBRD_StartMode)pStMode;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
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
		pStartMode->reStartMode = pStrMode->ByBits.Restart;
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
			pStartMode->reStartMode = pStrMode->ByBits.Restart;
		}else
			return BRDerr_NOT_ACTION; // функция в режиме Slave не выполнима
	}

	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::SetStartMode(CModule* pModule, PVOID pStMode)
{
	PBRD_StartMode pStartMode = (PBRD_StartMode)pStMode;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
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
//=**************************************************************************************

int C8gcSrv::SetClkMode(CModule* pModule, PBRD_ClkMode pClkMode)
{
	PGCSRV_CFG pDdcSrvCfg= (PGCSRV_CFG)m_pConfig;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = ADM2IFnr_FSRC;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);

	if(pClkMode->src==0)	// internal
	{ 
		param.val = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		pClkMode->value = pDdcSrvCfg->SubIntClk;
		m_SystemClock=pClkMode->value;
	//	pDdcSrvCfg->SubRefGen=pClkMode->value;
		return BRDerr_OK;
	}
	if(pClkMode->src==1)	//external
	{ 
		param.val = 0x81;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		pDdcSrvCfg->SubExtClk=(U32)pClkMode->value;
		m_SystemClock=(U32)pClkMode->value;
		return BRDerr_OK;
	}
		return BRDerr_OK;
}

//=**************************************************************************************
int C8gcSrv::GetClkMode(CModule* pModule, PBRD_ClkMode pClkMode)
{
	PGCSRV_CFG pDdcSrvCfg= (PGCSRV_CFG)m_pConfig;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = ADM2IFnr_FSRC;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	if(param.val==0x81)			// external
	{ 
		pClkMode->src = 1;		
		pClkMode->value=pDdcSrvCfg->SubExtClk;
	}else						  //internal
	{
		pClkMode->src = param.val;
		pClkMode->value = pDdcSrvCfg->SubIntClk;

	}
		return BRDerr_OK;
}

//=**************************************************************************************
int C8gcSrv::SetDdcSync(CModule* pModule,void* args)
{
	PBRD_DdcSetSync DdcSetSync=(PBRD_DdcSetSync)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = GCnr_DDCSYNC;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PGC_DDCSYNC pDdcSync = (PGC_DDCSYNC)&param.val;

	pDdcSync->ByBits.SyncMode=DdcSetSync->syncMode;
	pDdcSync->ByBits.Slave=DdcSetSync->slave;
	pDdcSync->ByBits.ProgSync=0;
	pDdcSync->ByBits.ResDiv=0;

	pDdcSync->ByBits.ResDiv=DdcSetSync->resDiv;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pDdcSync->ByBits.ResDiv=0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
    IPC_delay(1);
	pDdcSync->ByBits.ProgSync=DdcSetSync->progSync;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::ProgramDdc(CModule* pModule,void* args)
{
	PBRD_DdcProgram pDdcProgram=(PBRD_DdcProgram)args;
	U32	addr;
	U32	nItems = pDdcProgram->nItems;
	U32	*pData = pDdcProgram->pData;
	int	nPage = 0xFF, nCurPage = 0xFF;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	/*setting mask_device DDC device*/
	param.reg = GCnr_MASK_DEVICE;
	param.val=0x1;

	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	for(int i=0; i<nItems; i++ )
	{
		nPage = (pData[i]>>24) & 0xFF;
		if( nCurPage!=nPage )
		{
			nCurPage = nPage;

/*
			addr = ((0x2)<<8)  | 1;					// Register 2 (address page) 
			param.reg = GCnr_ADDR;
			param.val=addr;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			param.reg = GCnr_WRITEDATA;
			param.val = nCurPage;						
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
*/
			
			addr = 0x2;
			param.reg = GCnr_DDC_ADR_SHIFT+addr;
			param.val = nCurPage;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);// Register 2 
														  // (address page)




		}
/*
		addr = ((pData[i]>>8)&0xFF00) | 1;
		param.reg = GCnr_ADDR;
		param.val=addr;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.reg = GCnr_WRITEDATA;
		param.val = pData[i]&0xFFFF;						
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
*/
			addr = ((pData[i]>>8)&0xFF00);
			addr >>= 8;
			param.reg = GCnr_DDC_ADR_SHIFT+addr;
			param.val = pData[i]&0xFFFF;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);// 
	}
	if( 0 > VerifyProgDdc(pModule,pData, nItems ) )
				return BRDerr_ERROR;
	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::VerifyProgDdc(CModule* pModule,U32 *regs,int nItems)
{

	U32		addr, reg;
	U32		nPage=0xFF, nCurPage=0xFF;
	PGCSRV_CFG pDdcSrvCfg = (PGCSRV_CFG)m_pConfig;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	/*setting mask_device DDC device*/
	param.reg = GCnr_MASK_DEVICE;
	param.val=0x1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	for( int i=0; i<nItems; i++ )
	{
		nPage = (regs[i] >> 24) & 0xFF;
		if( nCurPage!=nPage )
		{
			nCurPage = nPage;

/*
			addr = ((0x2)<<8)  | 1;					// Register 2 (address page) 
			param.reg = GCnr_ADDR;
			param.val=addr;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			param.reg = GCnr_WRITEDATA;
			param.val = nCurPage;						
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
*/

			addr = 0x2;
			param.reg = GCnr_DDC_ADR_SHIFT+addr;
			param.val = nCurPage;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	// 

			
		}
/*
		addr = ((regs[i]>>8)&0xFF00) | 1; 
		reg = (addr>>8)& 0xFF;
		param.reg = GCnr_ADDR;
		param.val=addr;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.reg = GCnr_READCOMMAND;	// set read mode
		param.val= 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.reg = GCnr_READDATA;	
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
*/	
		
		addr = ((regs[i]>>8)&0xFF00) | 1; 
		reg = (addr>>8)& 0xFF;
		addr >>= 8;

		param.reg = GCnr_DDC_ADR_SHIFT_R+addr;
		param.val = 0;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);	// 


		param.val &=0xffff;
		if(reg==0)param.val &=0xfffc; 
		if(reg==2)continue; 
		if( ((nPage==0x13) || (nPage==0x33) || (nPage==0x53) || (nPage==0x73)) && ((reg==0x1c )|| (reg==0x1a ) || (reg==0x1b )))continue; 
		if( ((nPage==0x80) || (nPage==0xa0)) && ((reg==0x10) || (reg==0x18)))continue; 
		if((nPage==0x10) || (nPage==0x30)|| (nPage==0x50)|| (nPage==0x70))continue; 

		if(param.val!=(regs[i]&0xffff))
		{
			return BRDerr_ERROR;

		}
	}

	return BRDerr_OK; 

}
//***************************************************************************************
int C8gcSrv::StartDdc(CModule* pModule)
{
	PGCSRV_CFG pDdcSrvCfg = (PGCSRV_CFG)m_pConfig;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	param.reg = GCnr_MASK_DEVICE;
	param.val=0x1;	// Select DDC		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = GCnr_DDC_ADR_SHIFT;
	param.val=0x100;	// Unreset DDC		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = GCnr_DDC_ADR_SHIFT+3;
	param.val=0x1ff;		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	return BRDerr_OK; 
}
//***************************************************************************************
int C8gcSrv::StopDdc(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	param.reg = GCnr_MASK_DEVICE;
	param.val=0x1;	// Select DDC		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = GCnr_DDC_ADR_SHIFT;
	param.val=0xfff0;	// Reset DDC		
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK; 
}
//***************************************************************************************
int C8gcSrv::SetTarget(CModule* pModule, ULONG target)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE1 pMode1 = (PADM2IF_MODE1)&param.val;
	pMode1->ByBits.Out = target;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::GetTarget(CModule* pModule, ULONG& target)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE1 pMode1 = (PADM2IF_MODE1)&param.val;
	target = pMode1->ByBits.Out;
	return BRDerr_OK;
}
//***************************************************************************************
int C8gcSrv::SetTestMode(CModule* pModule, ULONG mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE1 pMode1 = (PADM2IF_MODE1)&param.val;
	pMode1->ByBits.Test = mode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::GetTestMode(CModule* pModule, ULONG& mode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE1 pMode1 = (PADM2IF_MODE1)&param.val;
	mode = pMode1->ByBits.Test;
	return BRDerr_OK;
}
//***************************************************************************************
int C8gcSrv::ProgramAdc(CModule* pModule,void* args)
{
	PBRD_AdcProgram ptr=(PBRD_AdcProgram)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	/* ZEROing TRDIND_ADR_HIGH*/
	param.reg = GCnr_ADR_HIGH;
	param.val= 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	/*setting mask_device*/
	param.reg = GCnr_MASK_DEVICE;
	param.val= (ptr->maskAdc)<<4;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	/*setting offset*/
	param.reg = GCnr_DDC_ADR_SHIFT + Reg_ADC_OFFSET;
	param.val= (ptr->offset);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	/*setting vref*/
	param.reg = GCnr_DDC_ADR_SHIFT + Reg_ADC_VREF;
	param.val= (ptr->vref); 
	if(param.val>0)param.val +=5;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	/*setting buffcurrent1*/
	param.reg = GCnr_DDC_ADR_SHIFT + Reg_ADC_BUFCURRENT1;
	param.val= ((((ptr->bufCur1)/10)& 0x3f)<<2) | 0x2; 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	/*setting buffcurrent2*/
	param.reg = GCnr_ADR_HIGH;
	param.val= Reg_ADC_BUFCURRENT2>>7; 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = GCnr_DDC_ADR_SHIFT + (Reg_ADC_BUFCURRENT2 & 0x7f);
	param.val= ((((ptr->bufCur1)/10)& 0x3f)<<2) ; 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	/*Update*/
	param.reg = GCnr_ADR_HIGH;
	param.val= Reg_ADC_DEV_UPDATE>>7; 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = GCnr_DDC_ADR_SHIFT + (Reg_ADC_DEV_UPDATE & 0x7f);
	param.val= 0 ; 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= 1 ; 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val= 0 ; 
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::SetDelay(CModule* pModule,void* args)
{
	PBRD_SetDelay ptr=(PBRD_SetDelay)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;
	S32	nRegVal=0, nBitSlip=0;

	param.reg = GCnr_DELAY_CTRL;
	param.val= 0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);

	nBitSlip = param.val & 0xFF00;

	nRegVal= 0;
	if( ptr->nInc > 0 )
		nRegVal = 0x1;
	else
		ptr->nInc = - ptr->nInc;


	nRegVal |= ((ptr->nId) << 4);
	nRegVal |= nBitSlip;

	if(ptr->isRst)
	{
		param.val= nRegVal;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.val= nRegVal |4;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.val= nRegVal;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	for(int i=0; i<ptr->nInc; i++)
	{
		param.val= nRegVal;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.val= nRegVal | 0x2;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}

	param.val= nRegVal;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************
int C8gcSrv::SetTestSequence(CModule* pModule,void* args)
{
	U32 *ptr=(U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	param.reg = GCnr_TESTSEQUENCE;
	param.val= *ptr;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************
int C8gcSrv::SetBitSlip(CModule* pModule,void* args)
{
	U32 *ptr=(U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	param.reg = GCnr_DELAY_CTRL;
	param.val= *ptr;
	param.val <<= 8;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************
int C8gcSrv::RestNearFifo(CModule* pModule,void* args)
{
	U32 *ptr=(U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	U32		mode2;
	U32	    mode0;

	if( *ptr )
	{
		param.reg = GCnr_MODE0;
		param.val = 0;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		param.val &=~8;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	param.reg = GCnr_MODE2;
	param.val= 0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	param.val &=~1;
	param.val |= (*ptr) & 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{	
	BRDCHAR Buffer[128];
	//BRDCHAR ParamName[128];
	//BRDCHAR* endptr;

	BRD_SetAdcLag setlag;

	{
		IPC_getPrivateProfileString(SectionName, _BRDC("Adc0Lag"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
		
		setlag.chan = 0;
		setlag.lag = BRDC_atoi(Buffer);	

		SetAdcLag( pDev, &setlag );
	}
	
	{
		IPC_getPrivateProfileString(SectionName, _BRDC("Adc1Lag"), _BRDC("0"), Buffer, sizeof(Buffer), iniFilePath);
		
		setlag.chan = 1;
		setlag.lag = BRDC_atoi(Buffer);

		SetAdcLag( pDev, &setlag );

	}

	return BRDerr_OK;
}

//***************************************************************************************
int C8gcSrv::SetAdcLag(CModule* pModule,void* args)
{
	PBRD_SetAdcLag ptr=(PBRD_SetAdcLag)args;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_GcTetrNum;

	param.reg = GCnr_ADC_DELAY;

	pModule->RegCtrl(DEVScmd_REGREADIND, &param);

	if( ptr->chan == 0 )
		param.val = (param.val & 0xFF00) | ptr->lag&0xFF;
	else if( ptr->chan == 1 )
		param.val = (param.val & 0x00FF) | ((ptr->lag&0xFF)<<8);

	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************
