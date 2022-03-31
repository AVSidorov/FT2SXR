/*
 ***************** File ddc8x32Srv.cpp ************
 *
 * BRD Driver for ddc8x32 service 
 *
 * (C) InSys by Sklyarov A. Jul. 2012
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "ddc8x32Srv.h"

#define	CURRFILE "DDC8XSRV"

//***************************************************************************************
//***************************************************************************************
int Cddc8x32Srv::CtrlRelease(
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
void Cddc8x32Srv::GetDdcTetrNum(CModule* pModule)
{
	m_ddc8x32TetrNum = GetTetrNum(pModule, m_index,  DDC8X32_TETR_ID  );

}
//***************************************************************************************
int Cddc8x32Srv::SetClock(CModule* pModule, void *args)
{
   
	m_SystemClock	= *(U32*)args;

	return BRDerr_OK;
}
//***************************************************************************************
int Cddc8x32Srv::SetChan(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	PBRD_MaskChan ptr=(PBRD_MaskChan)args;
	U32 mask= ptr->mask;
	U32 maskchan;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;
	param.reg = DDC8X32_MASK;
	maskchan = mask;
//	if(ptr->mode==0)	// широкополосный режим
//	{
//		maskchan = 0;
//		for(int i=0;i<8;i++)
//		{
//			if((mask>>i) & 1)maskchan |= 1<<(i*4); 
//		}
//	}

	param.val= (maskchan>>16) & 0xffff;
//	param.val= maskchan & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val= maskchan & 0xffff;
//	param.val= (maskchan>>16) & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	return BRDerr_OK;
}
//***************************************************************************************
int Cddc8x32Srv::SetMode(CModule* pModule, void *args)
{
	PBRD_SetMode ptr=(PBRD_SetMode)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;
	param.reg = DDC8X32_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.mode= ptr->wide&1;
	pControl1->ByBits.nslot= ptr->nslot&3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	return BRDerr_OK;
}
//***************************************************************************************
int Cddc8x32Srv::SetFc(CModule* pModule, void *args)
{
	
	DEVS_CMD_Reg param;
	PBRD_SetFc ptr=(PBRD_SetFc)args;
	U32 *pFc = ptr->pFc;
	U32	Fc,CodFc;
	int nc=1;
	double dFc;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;
	param.reg = DDC8X32_NCO;
//	if(ptr->mode==1)nc = 4;	// широкополосный режим
	if(ptr->mode== 0)
	{
		for(int i=0;i<32;i++)
		{
			Fc = pFc[31-i];
			dFc=0x100000000*Fc/m_SystemClock;
			CodFc=(U32)dFc;
			param.val = CodFc >> 16;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
			param.val = CodFc;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
		}
	}
	if(ptr->mode== 1)
	{
		for(int i=0;i<8;i++)
		{
			Fc = pFc[7-i];
			dFc=0x100000000*Fc/m_SystemClock;
			CodFc=(U32)dFc;
			for(int i=0;i<4;i++)
			{
				param.val = CodFc >> 16;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
				param.val = CodFc;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
			}
		}
	}

	param.reg = DDC8X32_WRNCO;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	


	return BRDerr_OK;
}
//*************************************************************************************
int Cddc8x32Srv::SetFir(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	PBRD_SetFir ptr=(PBRD_SetFir)args;
	S32* coeff = ptr->coef;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;
	param.reg = DDC8X32_FIR;

//	for( int i=0; i<256; i++ )
	for( int i=0; i<128; i++ )
	{
		param.val = coeff[i] >> 16;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
		param.val = coeff[i] & 0xffff;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	
	}
	return BRDerr_OK;
}
//*************************************************************************************
int Cddc8x32Srv::SetDecim(CModule* pModule, void *args)
{
	DEVS_CMD_Reg param;
	PBRD_SetDecim ptr = (PBRD_SetDecim)args;
	U32	dcic= (ptr->dec_cic) & 0xff;
//	U32	dfir= ((ptr->dec_fir)-1) & 0xff;
	U32	dfir= (ptr->dec_fir) & 0xff;
	U32 decim;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;
	param.reg = DDC8X32_DECIM;
//	decim = (dfir<<8) | dcic;
	decim = (dfir<<10) | dcic;
	param.val = decim;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);	

	return BRDerr_OK;
}
//***************************************************************************************
int Cddc8x32Srv::StartDdc(CModule* pModule, void *args)
{
	U32			*ptr = (U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;
	param.reg = DDC8X32_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.startDdc= 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	return BRDerr_OK;
}
//***************************************************************************************
int Cddc8x32Srv::StopDdc(CModule* pModule, void *args)
{
	U32			*ptr = (U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;
	param.reg = DDC8X32_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;
	pControl1->ByBits.startDdc= 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);


	return BRDerr_OK;
}
//***************************************************************************************
//***************************************************************************************
int Cddc8x32Srv::SetDebug(CModule* pModule, void *args)
{
	U32	*ptr = (U32*)args;
	DEVS_CMD_Reg param;
	U32 debug = *ptr;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;
	param.reg = DDC8X32_DEBUG0;
	param.val = *ptr & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = DDC8X32_DEBUG1;
	param.val = ((*ptr)>>16) & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	

	return BRDerr_OK;
}
//***************************************************************************************
int Cddc8x32Srv::SetOver(CModule* pModule, void *args)
{
	PBRD_SetOver setover = (PBRD_SetOver)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;

	param.reg = DDC8X32_POROG;
	param.val = (setover->porog7 & 0x7f)<<8 | (setover->porog6 & 0x7f);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = (setover->porog5 & 0x7f)<<8 | (setover->porog4 & 0x7f);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = (setover->porog3 & 0x7f)<<8 | (setover->porog2 & 0x7f);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = (setover->porog1 & 0x7f)<<8 | (setover->porog0 & 0x7f);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = DDC8X32_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;

	pControl1->ByBits.rstover= 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pControl1->ByBits.rstover= 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}
//***************************************************************************************
int Cddc8x32Srv::GetOver(CModule* pModule, void *args)
{
	U32	*ptr = (U32*)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;
	U32 hi,low;
	param.reg = DDC8X32_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;

	pControl1->ByBits.fixover= 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pControl1->ByBits.fixover= 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	pControl1->ByBits.rstover= 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pControl1->ByBits.rstover= 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = DDC8X32_ADC_OVER;
	for(int i=0; i<64; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		low = param.val;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		hi = param.val;
		ptr[i] = ((hi & 0xffff)<< 16) | (low & 0xffff);
	}

	return BRDerr_OK;

}
//***************************************************************************************
//***************************************************************************************
int Cddc8x32Srv::SetFrame(CModule* pModule, void *args)
{
	PBRD_SetFrame	ptr = (PBRD_SetFrame)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_ddc8x32TetrNum;


	param.reg = DDC8X32_CONTROL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PREG_CONTROL1 pControl1 = (PREG_CONTROL1)&param.val;

	pControl1->ByBits.enbframe = ptr ->enb;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	U32 lfr = (ptr ->lframe -1)*4-1;
	param.reg = DDC8X32_DEBUG1;				// Запись длины кадра
	param.val = (lfr>>16) & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = lfr & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = DDC8X32_DEBUG0;	

	param.val = ((ptr->sword4)>>16) & 0xffff; // Запись 4 синхрослова
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val =( ptr->sword4) & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val = ((ptr->sword3)>>16) & 0xffff; // Запись 3 синхрослова
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val =( ptr->sword3) & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	
	param.val = ((ptr->sword2)>>16) & 0xffff; // Запись 2 синхрослова
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val =( ptr->sword2) & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.val = ((ptr->sword1)>>16) & 0xffff; // Запись 2 синхрослова
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val =( ptr->sword1) & 0xffff;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;

}
//***************************************************************************************

// End Of File