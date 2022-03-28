/*
 ***************** File Sync8chSrv.cpp ************
 *
 * BRD Driver for Sync8ch service
 *
 * (C) InSys by Dorokhin A. Apr 2009
 *
 ******************************************************
*/

#include <windows.h>
#include <winioctl.h>

#include "module.h"
#include "Sync8chSrv.h"

#define	CURRFILE "SYNC8CHSRV"

//***************************************************************************************
int CSync8chSrv::SetClkMode(CModule* pModule, PBRD_SyncClkMode pClkMode)
{
	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = SYNC8CHnr_FSRC;
	param.val = pClkMode->src;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	#if defined(__IPC_WIN__) || defined(__IPC_LINUX__)
		IPC_delay(250);
		IPC_delay(250);
	#else
		Sleep(250);
		Sleep(250);
	#endif
	param.reg = SYNC8CHnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PSYNC8CH_CTRL1 pControl1 = (PSYNC8CH_CTRL1)&param.val;
	pControl1->ByBits.ClkInv = pClkMode->clkInv;
	pControl1->ByBits.ClkSl = pClkMode->clkSlave;
	pControl1->ByBits.ClkSel = pClkMode->clkSel;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	switch(pClkMode->src)
	{
	case BRDclks_SYNC8CH_GEN1:		// Generator 1
		if( pSrvCfg->Gen1Type == 0 || pSrvCfg->DeviceID == 0x50A2)
		{
			// Генератор на субмодуле непрограммируемый
			pClkMode->value = (REAL64)pSrvCfg->Gen1;
			//refClkValue = pSrvCfg->nRefGen0;
		}
		else
		{
			//#if defined(__IPC_WIN__) || defined(__IPC_LINUX__)
			//	IPC_delay(300);
			//#else
			//	Sleep(300);
			//#endif
			double	dClkValue;
			Si571GetClkVal( pModule, &dClkValue ); // Считать Si570/Si571
			if( dClkValue != pClkMode->value )
			{
				dClkValue = pClkMode->value;
				Si571SetClkVal( pModule, &dClkValue ); // Запрограммировать Si570/Si571
				Si571GetClkVal( pModule, &dClkValue ); // Считать Si570/Si571
			}
			pClkMode->value = dClkValue;
		}
		break;
	case BRDclks_SYNC8CH_GEN2:		// Generator 2
		pClkMode->value = (REAL64)pSrvCfg->Gen2;
		break;
	case BRDclks_SYNC8CH_OCXO:		// OCXO Generator
		pClkMode->value = (REAL64)pSrvCfg->OcxoGen;
		break;
	case BRDclks_SYNC8CH_GEN1REFPLL:	// PLL (ref gen1)
		if(pSrvCfg->DeviceID == 0x50A2)
		{
			pClkMode->value = pSrvCfg->PllFreq;
			OnPll(pModule, pClkMode->value, pSrvCfg->Gen1);
		}
		else
		{
			if( pSrvCfg->Gen1Type == 0)
			{
				// Генератор на субмодуле непрограммируемый
				pClkMode->value = (REAL64)pSrvCfg->Gen1;
			}
			else
			{
				//#if defined(__IPC_WIN__) || defined(__IPC_LINUX__)
				//	IPC_delay(300);
				//#else
				//	Sleep(300);
				//#endif
				double	dClkValue;
				Si571GetClkVal( pModule, &dClkValue ); // Считать Si570/Si571
				if( dClkValue != pClkMode->value )
				{
					dClkValue = pClkMode->value;
					Si571SetClkVal( pModule, &dClkValue ); // Запрограммировать Si570/Si571
					Si571GetClkVal( pModule, &dClkValue ); // Считать Si570/Si571
				}
				//pClkMode->value = dClkValue;
			}
			pSrvCfg->PllFreq = (ULONG)pClkMode->pllFreq;
			OnPll(pModule, pClkMode->pllFreq, pClkMode->value);
		}
		break;
	case BRDclks_SYNC8CH_GEN2REFPLL:	// PLL (ref gen2)
		if(pSrvCfg->DeviceID == 0x50A2)
			pClkMode->value = pSrvCfg->PllFreq;
		else
		{
			pClkMode->value = pClkMode->pllFreq;
			pSrvCfg->PllFreq = (ULONG)pClkMode->value;
		}
		OnPll(pModule, pClkMode->value, pSrvCfg->Gen2);
		break;
	case BRDclks_SYNC8CH_OCXOREFPLL:	// PLL (ref OCXO)
		if(pSrvCfg->DeviceID == 0x50A2)
			pClkMode->value = pSrvCfg->PllFreq;
		else
		{
			pClkMode->value = pClkMode->pllFreq;
			pSrvCfg->PllFreq = (ULONG)pClkMode->value;
		}
		OnPll(pModule, pClkMode->value, pSrvCfg->OcxoGen);
		break;
	case BRDclks_SYNC8CH_EXTCLK:	// External clock
		pSrvCfg->ExtClk = (ULONG)pClkMode->value;
		break;
	case BRDclks_SYNC8CH_EXTREFPLL:	// PLL (ref ext clock)
		{
		double value = pSrvCfg->PllFreq;
		if(pSrvCfg->DeviceID == 0x551B)
		{
			value = pClkMode->pllFreq;
			pSrvCfg->PllFreq = (ULONG)value;
		}
		pSrvCfg->ExtClk = (ULONG)pClkMode->value;
		OnPll(pModule, value, pSrvCfg->ExtClk);
		}
		break;
	default:
		pClkMode->value = 0.0;
		break;
	}

	param.reg = SYNC8CHnr_FDVR0; 
	param.val = powof2(pClkMode->preDiv);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pClkMode->preDiv = (U32)pow(2., (int)param.val);
	param.reg = SYNC8CHnr_FDVR1;
	param.val = pClkMode->auxDiv;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::GetClkMode(CModule* pModule, PBRD_SyncClkMode pClkMode)
{
	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = SYNC8CHnr_FSRC;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pClkMode->src = param.val;
	param.reg = SYNC8CHnr_FDVR0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pClkMode->preDiv = (U32)pow(2., (int)param.val);
	param.reg = SYNC8CHnr_FDVR1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pClkMode->auxDiv = param.val;
	param.reg = SYNC8CHnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PSYNC8CH_CTRL1 pControl1 = (PSYNC8CH_CTRL1)&param.val;
	pClkMode->clkInv = pControl1->ByBits.ClkInv;
	pClkMode->clkSlave = pControl1->ByBits.ClkSl;
	pClkMode->clkSel = pControl1->ByBits.ClkSel;

	switch(pClkMode->src)
	{
	case BRDclks_SYNC8CH_GEN1:		// Generator 1
		if( pSrvCfg->Gen1Type == 0 || pSrvCfg->DeviceID == 0x50A2)
		{
			// Генератор на субмодуле непрограммируемый
			pClkMode->value = (REAL64)pSrvCfg->Gen1;
		}
		else
		{
			double	dClkValue;
			Si571GetClkVal( pModule, &dClkValue ); // Считать Si570/Si571
			pClkMode->value = dClkValue;
		}
		break;
	case BRDclks_SYNC8CH_GEN2:		// Generator 2
		pClkMode->value = (REAL64)pSrvCfg->Gen2;
		break;
	case BRDclks_SYNC8CH_OCXO:		// OCXO Generator
		pClkMode->value = (REAL64)pSrvCfg->OcxoGen;
		break;
	case BRDclks_SYNC8CH_GEN1REFPLL:	// PLL (ref gen1)
	case BRDclks_SYNC8CH_GEN2REFPLL:	// PLL (ref gen2)
	case BRDclks_SYNC8CH_OCXOREFPLL:	// PLL (ref OCXO)
	case BRDclks_SYNC8CH_EXTREFPLL:	// PLL (ref ext clock)
		pClkMode->value = pSrvCfg->PllFreq;
		break;
	case BRDclks_SYNC8CH_EXTCLK:	// External clock
		pClkMode->value = pSrvCfg->ExtClk;
		break;
	default:
		pClkMode->value = 0.0;
		break;
	}
	return BRDerr_OK;
}

//	chan - number of clock (0 - E, 1 - F, 2 - G)
//***************************************************************************************
int CSync8chSrv::SetOutClk(CModule* pModule, PBRD_SyncOutClk pOutClk)
{
	//ULONG fdvr_reg = SYNC8CHnr_FDVRE;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	//param.reg = fdvr_reg;
	param.reg = SYNC8CHnr_FDVRE + pOutClk->chan;
	param.val = powof2(pOutClk->divider);
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pOutClk->divider = (U32)pow(2., (int)param.val);

	param.reg = SYNC8CHnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PSYNC8CH_CTRL1 pControl1 = (PSYNC8CH_CTRL1)&param.val;
	switch(pOutClk->chan)
	{
	case 0:
		pControl1->ByBits.ClkEnE = pOutClk->enable;
		//fdvr_reg = SYNC8CHnr_FDVRE;
		break;
	case 1:
		pControl1->ByBits.ClkEnF = pOutClk->enable;
		//fdvr_reg = SYNC8CHnr_FDVRF;
		break;
	case 2:
		pControl1->ByBits.ClkEnG = pOutClk->enable;
		//fdvr_reg = SYNC8CHnr_FDVRG;
		break;
	}
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//	chan - number of clock (0 - E, 1 - F, 2 - G)
//***************************************************************************************
int CSync8chSrv::GetOutClk(CModule* pModule, PBRD_SyncOutClk pOutClk)
{
	ULONG fdvr_reg = SYNC8CHnr_FDVRE;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = SYNC8CHnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PSYNC8CH_CTRL1 pControl1 = (PSYNC8CH_CTRL1)&param.val;
	switch(pOutClk->chan)
	{
	case 0:
		pOutClk->enable = pControl1->ByBits.ClkEnE;
		fdvr_reg = SYNC8CHnr_FDVRE;
		break;
	case 1:
		pOutClk->enable = pControl1->ByBits.ClkEnF;
		fdvr_reg = SYNC8CHnr_FDVRF;
		break;
	case 2:
		pOutClk->enable = pControl1->ByBits.ClkEnG;
		fdvr_reg = SYNC8CHnr_FDVRG;
		break;
	}
	param.reg = fdvr_reg;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pOutClk->divider = (U32)pow(2., (int)param.val);
	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::SetStbMode(CModule* pModule, PBRD_SyncStbMode pStbMode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg =	SYNC8CHnr_STBCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PSYNC8CH_STBCTRL pStbCtrl = (PSYNC8CH_STBCTRL)&param.val;
	pStbCtrl->ByBits.ClkDelay = pStbMode->clkDelay;
	pStbCtrl->ByBits.ClkDlySign = pStbMode->signDelay;
	pStbCtrl->ByBits.StbClkInv = pStbMode->stbClkInv;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::GetStbMode(CModule* pModule, PBRD_SyncStbMode pStbMode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg =	SYNC8CHnr_STBCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PSYNC8CH_STBCTRL pStbCtrl = (PSYNC8CH_STBCTRL)&param.val;
	pStbMode->clkDelay = pStbCtrl->ByBits.ClkDelay;
	pStbMode->signDelay = pStbCtrl->ByBits.ClkDlySign;
	pStbMode->stbClkInv = pStbCtrl->ByBits.StbClkInv;
	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::SetOutStb(CModule* pModule, PBRD_SyncOutStb pStbOut)
{
	SYNC8CH_STBCTRL StbCtrl;
	SYNC8CH_STBEN StbEn;
	SYNC8CH_STBPOL StbPol;
	ULONG reg = SYNC8CHnr_DELSTB_AL;
	ULONG lvl_tdn = BRDtdn_SYNC8CH_HILVLCHA;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = SYNC8CHnr_STBCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	StbCtrl.AsWhole = param.val;
	param.reg = SYNC8CHnr_STBEN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	StbEn.AsWhole = param.val;
	param.reg = SYNC8CHnr_STBPOL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	StbPol.AsWhole = param.val;
	switch(pStbOut->chan)
	{
	case 0:
		StbCtrl.ByBits.StbAByPass = pStbOut->byPass;
		StbEn.ByBits.StbAEn = pStbOut->enable;
		StbPol.ByBits.StbAPol = pStbOut->polarity;
		reg = SYNC8CHnr_DELSTB_AL;
		lvl_tdn = BRDtdn_SYNC8CH_HILVLCHA;
		break;
	case 1:
		StbCtrl.ByBits.StbBByPass = pStbOut->byPass;
		StbEn.ByBits.StbBEn = pStbOut->enable;
		StbPol.ByBits.StbBPol = pStbOut->polarity;
		reg = SYNC8CHnr_DELSTB_BL;
		lvl_tdn = BRDtdn_SYNC8CH_HILVLCHB;
		break;
	case 2:
		StbCtrl.ByBits.StbCByPass = pStbOut->byPass;
		StbEn.ByBits.StbCEn = pStbOut->enable;
		StbPol.ByBits.StbCPol = pStbOut->polarity;
		reg = SYNC8CHnr_DELSTB_CL;
		break;
	case 3:
		StbCtrl.ByBits.StbDByPass = pStbOut->byPass;
		StbEn.ByBits.StbDEn = pStbOut->enable;
		StbPol.ByBits.StbDPol = pStbOut->polarity;
		reg = SYNC8CHnr_DELSTB_DL;
		lvl_tdn = BRDtdn_SYNC8CH_HILVLCHD;
		break;
	}
	param.reg = SYNC8CHnr_STBCTRL;
	param.val = StbCtrl.AsWhole;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = SYNC8CHnr_STBEN;
	param.val = StbEn.AsWhole;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = SYNC8CHnr_STBPOL;
	param.val = StbPol.AsWhole;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = reg;
	param.val = pStbOut->delay & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = reg+1;
	param.val = (pStbOut->delay >> 16) & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = reg+2;
	param.val = pStbOut->width & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = reg+3;
	param.val = (pStbOut->width >> 16) & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = reg+4;
	param.val = pStbOut->period & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = reg+5;
	param.val = (pStbOut->period >> 16) & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	if(pStbOut->chan != 2)
	{
		if(pStbOut->hiLevel < pStbOut->loLevel)
			return BRDerr_BAD_PARAMETER;
		PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
		double value = pStbOut->hiLevel;	// high level of strobe
		double max_thr = (fabs(SYNC8CH_MAXCHANLEVEL) + fabs(SYNC8CH_MINCHANLEVEL)) / 2;
		double delta_thr = SYNC8CH_MAXCHANLEVEL - max_thr;
		int max_code = 255;
		int min_code = 0;
		USHORT dac_data;
		if(value > SYNC8CH_MAXCHANLEVEL)
		{
			value = SYNC8CH_MAXCHANLEVEL;
			dac_data = 255;
		}
		else
			if(value < SYNC8CH_MINCHANLEVEL)
			{
				value = SYNC8CH_MINCHANLEVEL;
				dac_data = 0;
			}
			else
			{
				dac_data = (USHORT)floor(((value-delta_thr) / max_thr + 1.) * 128. + 0.5);
				value = max_thr * (dac_data / 128. - 1.) + delta_thr;
			}
		pStbOut->hiLevel = value;
		//printf("%d: HL=%f code=%d ", pStbOut->chan, pStbOut->hiLevel, dac_data);
/*
		double max_thr = pSrvCfg->RefPVS / 1000.;
		//value = max_thr * value / max_range;
		USHORT dac_data = (USHORT)floor(value);
		//if(fabs(value) > max_thr)
		//	dac_data = value > 0.0 ? max_code : min_code;
		//else
		//	dac_data = (USHORT)floor((value / max_thr + 1.) * 128. + 0.5);
		if(dac_data > max_code)
			dac_data = max_code;

		//value = max_thr * (dac_data / 128. - 1.);
		//pStbOut->hiLevel = value * max_range / max_thr;
		pStbOut->hiLevel = dac_data;
*/
		pSrvCfg->ThrDac[lvl_tdn - 1] = (UCHAR)dac_data;
		param.reg = MAINnr_THDAC;
		PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
		pThDac->ByBits.Data = dac_data;
		pThDac->ByBits.Num = lvl_tdn;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		value = pStbOut->loLevel;	// low level of strobe
		if(value > SYNC8CH_MAXCHANLEVEL)
		{
			value = SYNC8CH_MAXCHANLEVEL;
			dac_data = 255;
		}
		else
			if(value < SYNC8CH_MINCHANLEVEL)
			{
				value = SYNC8CH_MINCHANLEVEL;
				dac_data = 0;
			}
			else
			{
				dac_data = (USHORT)floor(((value-delta_thr) / max_thr + 1.) * 128. + 0.5);
				value = max_thr * (dac_data / 128. - 1.) + delta_thr;
			}
		pStbOut->loLevel = value;
		//printf("LL=%f code=%d\n", pStbOut->loLevel, dac_data);
/*
		//value = max_thr * value / max_range;
		dac_data = (USHORT)floor(value);
		//if(fabs(value) > max_thr)
		//	dac_data = value > 0.0 ? max_code : min_code;
		//else
		//	dac_data = (USHORT)floor((value / max_thr + 1.) * 128. + 0.5);
		if(dac_data > max_code)
			dac_data = max_code;

		//value = max_thr * (dac_data / 128. - 1.);
		//pStbOut->loLevel = value * max_range / max_thr;
*/
		pSrvCfg->ThrDac[lvl_tdn] = (UCHAR)dac_data;
		param.reg = MAINnr_THDAC;
		pThDac->ByBits.Data = dac_data;
		pThDac->ByBits.Num = lvl_tdn+1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::GetOutStb(CModule* pModule, PBRD_SyncOutStb pStbOut)
{
	SYNC8CH_STBCTRL StbCtrl;
	SYNC8CH_STBEN StbEn;
	SYNC8CH_STBPOL StbPol;
	ULONG reg = SYNC8CHnr_DELSTB_AL;
	ULONG lvl_tdn = BRDtdn_SYNC8CH_HILVLCHA;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = SYNC8CHnr_STBCTRL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	StbCtrl.AsWhole = param.val;
	param.reg = SYNC8CHnr_STBEN;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	StbEn.AsWhole = param.val;
	param.reg = SYNC8CHnr_STBPOL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	StbPol.AsWhole = param.val;
	switch(pStbOut->chan)
	{
	case 0:
		pStbOut->byPass = StbCtrl.ByBits.StbAByPass;
		pStbOut->enable = StbEn.ByBits.StbAEn;
		pStbOut->polarity = StbPol.ByBits.StbAPol;
		reg = SYNC8CHnr_DELSTB_AL;
		lvl_tdn = BRDtdn_SYNC8CH_HILVLCHA;
		break;
	case 1:
		pStbOut->byPass = StbCtrl.ByBits.StbBByPass;
		pStbOut->enable = StbEn.ByBits.StbBEn;
		pStbOut->polarity = StbPol.ByBits.StbBPol;
		reg = SYNC8CHnr_DELSTB_BL;
		lvl_tdn = BRDtdn_SYNC8CH_HILVLCHB;
		break;
	case 2:
		pStbOut->byPass = StbCtrl.ByBits.StbCByPass;
		pStbOut->enable = StbEn.ByBits.StbCEn;
		pStbOut->polarity = StbPol.ByBits.StbCPol;
		reg = SYNC8CHnr_DELSTB_CL;
		break;
	case 3:
		pStbOut->byPass = StbCtrl.ByBits.StbDByPass;
		pStbOut->enable = StbEn.ByBits.StbDEn;
		pStbOut->polarity = StbPol.ByBits.StbDPol;
		reg = SYNC8CHnr_DELSTB_DL;
		lvl_tdn = BRDtdn_SYNC8CH_HILVLCHD;
		break;
	}

	param.reg = reg;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pStbOut->delay = param.val & 0xFFFF;
	param.reg = reg+1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pStbOut->delay = ((param.val & 0xFFFF) << 16) + pStbOut->delay;
	param.reg = reg+2;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pStbOut->width = param.val & 0xFFFF;
	param.reg = reg+3;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pStbOut->width = ((param.val & 0xFFFF) << 16) + pStbOut->width;
	param.reg = reg+4;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pStbOut->period = param.val & 0xFFFF;
	param.reg = reg+5;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pStbOut->period = ((param.val & 0xFFFF) << 16) + pStbOut->period;

	if(pStbOut->chan != 2)
	{
		PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
		double max_thr = (fabs(SYNC8CH_MAXCHANLEVEL) + fabs(SYNC8CH_MINCHANLEVEL)) / 2;
		double delta_thr = SYNC8CH_MAXCHANLEVEL - max_thr;

		USHORT dac_data = pSrvCfg->ThrDac[lvl_tdn - 1];	// high level of strobe
		double value = max_thr * (dac_data / 128 - 1.) + delta_thr;
		pStbOut->hiLevel = value;

		dac_data = pSrvCfg->ThrDac[lvl_tdn];	// low level of strobe
		value = max_thr * (dac_data / 128 - 1.) + delta_thr;
		pStbOut->loLevel = value;

		//pStbOut->hiLevel = pSrvCfg->ThrDac[lvl_tdn - 1];	// high level of strobe
		//pStbOut->loLevel = pSrvCfg->ThrDac[lvl_tdn];	// low level of strobe
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::StartStb(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Start = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::StopStb(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Start = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::StartClk(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = SYNC8CHnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PSYNC8CH_CTRL1 pCtrl = (PSYNC8CH_CTRL1)&param.val;
	pCtrl->ByBits.ClkStart = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	#if defined(__IPC_WIN__) || defined(__IPC_LINUX__)
		IPC_delay(100);
		IPC_delay(100);
	#else
		Sleep(100);
		Sleep(100);
	#endif
	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::StopClk(CModule* pModule)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = SYNC8CHnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PSYNC8CH_CTRL1 pCtrl = (PSYNC8CH_CTRL1)&param.val;
	pCtrl->ByBits.ClkStart = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CSync8chSrv::SetStartMode(CModule* pModule, PBRD_SyncStartMode pStartMode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = ADM2IFnr_STMODE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_STMODE pStrMode = (PADM2IF_STMODE)&param.val;
	pStrMode->ByBits.SrcStart  = pStartMode->startSrc;
	pStrMode->ByBits.SrcStop   = pStartMode->trigStopSrc;
	pStrMode->ByBits.InvStart  = pStartMode->startInv;
	pStrMode->ByBits.InvStop   = pStartMode->stopInv;
	pStrMode->ByBits.TrigStart = pStartMode->trigOn;
	pStrMode->ByBits.Restart   = pStartMode->reStartMode;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = SYNC8CHnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PSYNC8CH_CTRL1 pControl1 = (PSYNC8CH_CTRL1)&param.val;
	pControl1->ByBits.StartSl = pStartMode->startSlave;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	//param.tetr = m_MainTetrNum;
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus;
	do {
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		pStatus = (PADM2IF_STATUS)&param.val;
	} while(!pStatus->ByBits.CmdRdy);

	double max_range = SYNC8CH_EXTSTARTLEVEL;
	double value = pStartMode->levelExt;
	int max_code = 255;
	int min_code = 0;

	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
	double max_thr = pSrvCfg->RefPVS / 1000.;
	value = max_thr * value / max_range;
	USHORT dac_data;
	if(fabs(value) > max_thr)
		dac_data = value > 0.0 ? max_code : min_code;
	else
		dac_data = (USHORT)floor((value / max_thr + 1.) * 128. + 0.5);
	if(dac_data > max_code)
		dac_data = max_code;

	value = max_thr * (dac_data / 128. - 1.);
	pStartMode->levelExt = value * max_range / max_thr;

	pSrvCfg->StCmpThr = pStartMode->levelExt;
	pSrvCfg->ThrDac[BRDtdn_SYNC8CH_EXTSTARTLVL - 1] = (UCHAR)dac_data;
	param.reg = MAINnr_THDAC;
	PMAIN_THDAC pThDac = (PMAIN_THDAC)&param.val;
	pThDac->ByBits.Data = dac_data;
	pThDac->ByBits.Num = BRDtdn_SYNC8CH_EXTSTARTLVL;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//printf("EXTSTARTLVL = %d (%f Вольт)\n", dac_data, pStartMode->levelExt);

	#if defined(__IPC_WIN__) || defined(__IPC_LINUX__)
		IPC_delay(150);
		IPC_delay(150);
	#else
		Sleep(150);
		Sleep(150);
	#endif
	return BRDerr_OK;
}

//=**************************************************************************************
int CSync8chSrv::GetStartMode(CModule* pModule, PBRD_SyncStartMode pStartMode)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = ADM2IFnr_STMODE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_STMODE pStrMode = (PADM2IF_STMODE)&param.val;
	pStartMode->startSrc	= pStrMode->ByBits.SrcStart;
	pStartMode->trigStopSrc	= pStrMode->ByBits.SrcStop;
	pStartMode->startInv	= pStrMode->ByBits.InvStart;
	pStartMode->stopInv		= pStrMode->ByBits.InvStop;
	pStartMode->trigOn		= pStrMode->ByBits.TrigStart;
	pStartMode->reStartMode = pStrMode->ByBits.Restart;

	param.reg = SYNC8CHnr_CTRL1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PSYNC8CH_CTRL1 pControl1 = (PSYNC8CH_CTRL1)&param.val;
	pStartMode->startSlave = pControl1->ByBits.StartSl;

	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
	pStartMode->levelExt = pSrvCfg->StCmpThr;

	return BRDerr_OK;
}

//****************************************************************************************
int CSync8chSrv::writePllData(CModule* pModule, USHORT HiWord, USHORT LoWord)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;
	param.reg = SYNC8CHnr_HIPLL;
	param.val = HiWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = SYNC8CHnr_LOWPLL;
	param.val = LoWord;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//****************************************************************************************
int CSync8chSrv::InitPLL(CModule* pModule)
{
	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
	pSrvCfg->Pll_R_CNT.AsWhole = 0;
	pSrvCfg->Pll_N_CNT.AsWhole = 0;
	pSrvCfg->Pll_Func.AsWhole = 0;

	pSrvCfg->Pll_R_CNT.ByBits.Addr = 0;
	pSrvCfg->Pll_N_CNT.ByBits.Addr = 1;
	pSrvCfg->Pll_Func.ByBits.Addr = 2;

	pSrvCfg->Pll_R_CNT.ByBits.AB_WIDTH = 1; //1.3 ns
	pSrvCfg->Pll_R_CNT.ByBits.TEST_MODE = 0;
	pSrvCfg->Pll_R_CNT.ByBits.LDP = 0;
	writePllData(pModule, pSrvCfg->Pll_R_CNT.ByWord.HiWord, pSrvCfg->Pll_R_CNT.ByWord.LoWord);

	pSrvCfg->Pll_N_CNT.ByBits.CP_GAIN = 0;
	writePllData(pModule, pSrvCfg->Pll_N_CNT.ByWord.HiWord, pSrvCfg->Pll_N_CNT.ByWord.LoWord);

	pSrvCfg->Pll_Func.ByBits.CNT_RST = 0;
	pSrvCfg->Pll_Func.ByBits.PWDN_1 = 0;
	pSrvCfg->Pll_Func.ByBits.PWDN_2 = 0;
	pSrvCfg->Pll_Func.ByBits.MUX_OUT = 2;
	pSrvCfg->Pll_Func.ByBits.PD_POL = 1;
	pSrvCfg->Pll_Func.ByBits.CP_3STATE = 0;
	pSrvCfg->Pll_Func.ByBits.FL_ENBL = 0;
	pSrvCfg->Pll_Func.ByBits.FL_MODE = 0;
	pSrvCfg->Pll_Func.ByBits.TIMER_CNT = 0;
	pSrvCfg->Pll_Func.ByBits.CUR_SET_1 = 3;
	pSrvCfg->Pll_Func.ByBits.CUR_SET_2 = 7;

	writePllData(pModule, pSrvCfg->Pll_Func.ByWord.HiWord, pSrvCfg->Pll_Func.ByWord.LoWord);

	return BRDerr_OK;
}

int P_DIM[4] = {8, 16, 32, 64};

//****************************************************************************************
int CSync8chSrv::OnPll(CModule* pModule, double& ClkValue, double PllRefValue)
{
	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;

	if(PllRefValue > ClkValue)
		return -1;

	double	dFout = ClkValue; // Частота вых. сигнала, Fvco (Гц)
	double	dFin = PllRefValue; // Частота вх. сигнала, Fosc (Гц)
	double	dFoutreal, dFoutrealAux; // Частота вых. сигнала, полученная врезультате вычислений (Гц)
	double	dFrelative, dFrelativeAux;		// Отностельная погрешность чатоты
	int		nR, nP, nA, nB;
	int		nRcnt, nAcnt, nBcnt;
	int		nPidx;

	double	dFcmax = 101000000.0;	// Максимально допустимая частота сравнения 100 МГц
	double	dFinmax = 301000000.0;	// Максимально допустимая частота вх. сигнала 300 МГц
	double	dFrelativemax = 0.0;	// Допустимая отностельная погрешность чатоты
	int		nRmin = 1, nRmax = 16383;
	int		aP[] = { 8, 16, 32, 64 };
	int		nAmin = 0, nAmax = 63, nAmaxTmp;
	int		nBmin = 3, nBmax = 8191;

	int		ii;

	BRDC_printf( _BRDC(">>> OnPll() p1\n") );
	if( dFin > dFinmax )
		return -1;

	BRDC_printf( _BRDC(">>> OnPll() p2\n" ));
	//
	// Определяем делитель P
	//
	for( ii=0; ii<sizeof(aP)/sizeof(aP[0]); ii++ )
		if( (dFout/aP[ii]) <= dFcmax )
		{
			nP = aP[ii];
			nPidx = ii;
			break;
		}
	if( ii >= sizeof(aP)/sizeof(aP[0]) )
		return -2;

	BRDC_printf( _BRDC(">>> OnPll() p3\n" ));
	//
	// Определяем Rmin
	//
	nRmin = (int)floor( dFin / dFcmax );
	if( nRmin < 1 )
		nRmin = 1;

	//
	// Определяем Bmin
	//
	nBmin = (int)floor( (dFin * (double)nP) / (dFout * (double)nRmin) );
	if( nBmin < 3 )
		nBmin = 3;

	dFrelative = 1.0;

	//
	// Поиск в цикле R, A, B
	//
	for( nRcnt=nRmin; nRcnt<=nRmax; nRcnt++ )
	{
		for( nBcnt=nBmin; nBcnt<=nBmax; nBcnt++ )
		{
			nAmaxTmp = min( nAmax, nBcnt-1 );
			for( nAcnt=nAmin; nAcnt<=nAmaxTmp; nAcnt++ )
			{
				dFoutrealAux  = dFin / (double)nRcnt;
				dFoutrealAux *= (double)(nP * nBcnt + nAcnt);
				dFrelativeAux = (dFoutrealAux - dFout) / dFout;
				dFrelativeAux = fabs( dFrelativeAux );
				if( dFrelativeAux < dFrelative )
				{
					dFrelative = dFrelativeAux;
					dFoutreal  = dFoutrealAux;
					nR = nRcnt;
					nA = nAcnt;
					nB = nBcnt;
				}
				if( dFrelative <= dFrelativemax )
					break;
			}
			if( dFrelative <= dFrelativemax )
				break;
		}
		if( dFrelative <= dFrelativemax )
			break;
	}

	BRDC_printf( _BRDC(">>> Fosc=%.3f MHz, Fvco req=%.3f MHz, Fvco=%.3f MHz, dF=%.3f\n"),
				   dFin/1000000.0, dFout/1000000.0, dFoutreal/1000000.0, dFrelative );
	BRDC_printf( _BRDC(">>> A=%d, B=%d, R=%d, P[%d]=%d\n"), nA, nB, nR, nPidx, nP );

	pSrvCfg->Pll_R_CNT.ByBits.R_CNTR = nR;
	writePllData(pModule, pSrvCfg->Pll_R_CNT.ByWord.HiWord, pSrvCfg->Pll_R_CNT.ByWord.LoWord);
	pSrvCfg->Pll_N_CNT.ByBits.A_CNTR = nA;
	pSrvCfg->Pll_N_CNT.ByBits.B_CNTR = nB;
	writePllData(pModule, pSrvCfg->Pll_N_CNT.ByWord.HiWord, pSrvCfg->Pll_N_CNT.ByWord.LoWord);
	pSrvCfg->Pll_Func.ByBits.PRESCALER = nPidx;
	writePllData(pModule, pSrvCfg->Pll_Func.ByWord.HiWord, pSrvCfg->Pll_Func.ByWord.LoWord);

	#if defined(__IPC_WIN__) || defined(__IPC_LINUX__)
		IPC_delay(50);
		IPC_delay(50);
	#else
		Sleep(50);
		Sleep(50);
	#endif
	return BRDerr_OK;
}

//****************************************************************************************
//int CSync8chSrv::OnPll(CModule* pModule, double& ClkValue, double PllRefValue)
//{
//	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
//
//	double f_vco = ClkValue;
//	double f_osc = PllRefValue;
//
//	double f_min = 325000000.; // 325 MHz
//	int A_max = 63;
//	int B_max = 8191;
//	int P_min = int(floor(f_vco / f_min));
//	int P = P_DIM[3];
//	for(int i = 0; i < 4; i++)
//		if(P_min <= P_DIM[i]) {
//			P = P_DIM[i];
//			break;
//		}
//
//	int R_max = int(floor(f_osc / (f_vco / (P * B_max + A_max)) ));
//	if(R_max > 16383) R_max = 16383;
//
//	int R_min = int(ceil(f_osc / PLL_F_CMP_MAX));
//	if(R_min < 1) R_min = 1;
//
//	int R, B_tmp, B, A = 0;
//	//int delta_min = 100; // 1G
//	double delta_min = 10000000; // 2G
//
//	for(int A = 0; A <= A_max; A++)
//	{
//		for(int R_tmp = R_min; R_tmp <= R_max; R_tmp++)
//		{
//			B_tmp = int(((f_vco * R_tmp / f_osc) - A) / P);
//			double f_new = (P * B_tmp + A) * (f_osc / R_tmp);
//			//int delta = int((fabs(pSrvCfg->PllFreq - f_new) / pSrvCfg->PllFreq) * 100); // 1G
//	//		double delta = (fabs(pSrvCfg->PllFreq - f_new) / pSrvCfg->PllFreq) * 10000000; // 2G
//			double delta = (fabs(pSrvCfg->PllFreq - f_new) / pSrvCfg->PllFreq); // 200M
//			if(delta < delta_min)
//			{
//				ClkValue = f_new;
//				delta_min = delta;
//				B = B_tmp;
//				R = R_tmp;
//				if(delta_min == 0)
//					break;
//			}
//		}
//		if(delta_min == 0)
//			break;
//	}
//	pSrvCfg->Pll_R_CNT.ByBits.R_CNTR = R;
//	writePllData(pModule, pSrvCfg->Pll_R_CNT.ByWord.HiWord, pSrvCfg->Pll_R_CNT.ByWord.LoWord);
//	pSrvCfg->Pll_N_CNT.ByBits.A_CNTR = A;
//	pSrvCfg->Pll_N_CNT.ByBits.B_CNTR = B;
//	writePllData(pModule, pSrvCfg->Pll_N_CNT.ByWord.HiWord, pSrvCfg->Pll_N_CNT.ByWord.LoWord);
//	pSrvCfg->Pll_Func.ByBits.PRESCALER = P;
//	writePllData(pModule, pSrvCfg->Pll_Func.ByWord.HiWord, pSrvCfg->Pll_Func.ByWord.LoWord);
//
//	Sleep(50);
//	return BRDerr_OK;
//}

//****************************************************************************************
int CSync8chSrv::SetPllMode(CModule* pModule, PBRD_PllMode pPllMode)
{
	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
	pSrvCfg->Pll_Func.ByBits.MUX_OUT = pPllMode->MuxOut;
	writePllData(pModule, pSrvCfg->Pll_Func.ByWord.HiWord, pSrvCfg->Pll_Func.ByWord.LoWord);
	return BRDerr_OK;
}

//int CSync8chSrv::SetMuxOut(CModule* pModule, U08 muxout)
//{
//	if(muxout < 0 || muxout > 7)
//		return BRDerr_BAD_PARAMETER;
//	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
//	pSrvCfg->Pll_Func.ByBits.MUX_OUT = muxout;
//	writePllData(pModule, pSrvCfg->Pll_Func.ByWord.HiWord, pSrvCfg->Pll_Func.ByWord.LoWord);
//	return BRDerr_OK;
//}

//***************************************************************************************
int CSync8chSrv::writeSpdDev(CModule* pModule, ULONG dev, ULONG num, ULONG synchr, ULONG reg, ULONG val)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// выбираем устройство
	param.reg = SYNC8Pnr_SPDDEVICE; // 0x220
	param.val = dev;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем адрес
	param.reg = SYNC8Pnr_SPDADDR; // 0x222
	param.val = reg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем данные
	param.reg = SYNC8Pnr_SPDDATAL; // 0x223  
	param.val = val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// посылаем команду записи
	param.val = (synchr << 12) | (num << 4) | 0x2; // write
	param.reg = SYNC8Pnr_SPDCTRL; // 0x221
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
int CSync8chSrv::readSpdDev(CModule* pModule, ULONG dev, ULONG num, ULONG synchr, ULONG reg, ULONG* pVal)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_SyncTetrNum;

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// выбираем устройство
	param.reg = SYNC8Pnr_SPDDEVICE; // 0x220
	param.val = dev;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем адрес
	param.reg = SYNC8Pnr_SPDADDR; // 0x222
	param.val = reg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// посылаем команду чтения
	param.val = (synchr << 12) | (num << 4) | 0x1; // read
	param.reg = SYNC8Pnr_SPDCTRL; // 0x221
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// считываем данные
	param.reg = SYNC8Pnr_SPDDATAL; // 0x223;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	*pVal = param.val;

	return BRDerr_OK;
}

//***************************************************************************************
int	CSync8chSrv::Si571SetClkVal( CModule* pModule, double *pRate )
{
	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
	ULONG			regData[20];
	int			regAdr;
	int			status = BRDerr_OK;

	// Скорректировать частоту, если необходимо
	if( *pRate > (double)pSrvCfg->RefMaxGen1 )
	{
		*pRate = (double)pSrvCfg->RefMaxGen1;
		status = BRDerr_WARN;
	}

    SI571_SetRate( pRate, pSrvCfg->dGen1Fxtal, (U32*)regData );
	//if(ret < 0)
	//	printf( "Error SI571_SetRate\n");

	// Запрограммировать генератор
	writeSpdDev(pModule, GENERATOR1_DEVID, pSrvCfg->AdrGen1, 0, 137, 0x10); // Freeze DCO
	for( regAdr=7; regAdr<=18; regAdr++ )
		writeSpdDev(pModule, GENERATOR1_DEVID, pSrvCfg->AdrGen1, 0, regAdr, regData[regAdr]);
	writeSpdDev(pModule, GENERATOR1_DEVID, pSrvCfg->AdrGen1, 0, 137, 0x0); // Unfreeze DCO
	writeSpdDev(pModule, GENERATOR1_DEVID, pSrvCfg->AdrGen1, 0, 135, 0x40 ); // Assert the NewFreq bit
	BRDC_printf( _BRDC(">>> Write Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n"), regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );
	BRDC_printf( _BRDC(">>> XTAL = %f kHz\n"), pSrvCfg->dGen1Fxtal/1000.0 );
	BRDC_printf( _BRDC(">>> Rate = %f kHz\n"), *pRate/1000.0 );

	#if defined(__IPC_WIN__) || defined(__IPC_LINUX__)
		IPC_delay(50);
		IPC_delay(50);
	#else
		Sleep(50);
		Sleep(50);
	#endif
	return status;	
}

//***************************************************************************************
int	CSync8chSrv::Si571GetClkVal( CModule* pModule, double *pRate )
{
	PSYNC8CHSRV_CFG pSrvCfg = (PSYNC8CHSRV_CFG)m_pConfig;
	ULONG			regData[20];
    //ULONG		clkSrc;
	int			regAdr;

	*pRate = 0.0;

	// Считать регистры Si570/Si571
	for( regAdr=7; regAdr<=12; regAdr++ )
		readSpdDev(pModule, GENERATOR1_DEVID, pSrvCfg->AdrGen1, 0, regAdr, &regData[regAdr]);

	SI571_GetRate( pRate, pSrvCfg->dGen1Fxtal, (U32*)regData );
	BRDC_printf( _BRDC(">>> Read Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n"), regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );

	return BRDerr_OK;	
}

// ***************** End of file Sync8chSrv.cpp ***********************
