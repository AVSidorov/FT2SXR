#include "module.h"
#include "fm401sSrv.h"

#define CURRFILE "FM401SSRV"

static CMD_Info SETCHANMASK_CMD = { BRDctrl_FM401S_SETCHANMASK, 0, 0, 0, NULL };
static CMD_Info GETCHANMASK_CMD = { BRDctrl_FM401S_GETCHANMASK, 1, 0, 0, NULL };
static CMD_Info SETCLKMODE_CMD = { BRDctrl_FM401S_SETCLKMODE, 0, 0, 0, NULL };
static CMD_Info GETCLKMODE_CMD = { BRDctrl_FM401S_GETCLKMODE, 1, 0, 0, NULL };
static CMD_Info GETSTATUS_CMD = { BRDctrl_FM401S_GETSTATUS, 1, 0, 0, NULL };
static CMD_Info SETTESTMODE_CMD = { BRDctrl_FM401S_SETTESTMODE, 0, 0, 0, NULL };
static CMD_Info GETTESTMODE_CMD = { BRDctrl_FM401S_GETTESTMODE, 1, 0, 0, NULL };
static CMD_Info START_CMD = { BRDctrl_FM401S_START, 0, 0, 0, NULL };
static CMD_Info STOP_CMD = { BRDctrl_FM401S_STOP, 0, 0, 0, NULL };
static CMD_Info GETTESTRESULT_CMD = { BRDctrl_FM401S_GETTESTRESULT, 1, 0, 0, NULL };
static CMD_Info SETDIR_CMD = { BRDctrl_FM401S_SETDIR, 0, 0, 0, NULL };
static CMD_Info GETDIR_CMD = { BRDctrl_FM401S_GETDIR, 1, 0, 0, NULL };
static CMD_Info GETRXTX_CMD = { BRDctrl_FM401S_GETRXTXSTATUS, 1, 0, 0, NULL };
static CMD_Info SETCLKVAL_CMD = { BRDctrl_FM401S_SETCLKVALUE, 0, 0, 0, NULL };
static CMD_Info GETCLKVAL_CMD = { BRDctrl_FM401S_GETCLKVALUE, 1, 0, 0, NULL };
static CMD_Info PREPARE_CMD = { BRDctrl_FM401S_PREPARE, 0, 0, 0, NULL };
static CMD_Info SETDECIM_CMD = { BRDctrl_FM401S_SETDECIM, 0, 0, 0, NULL };
static CMD_Info GETDECIM_CMD = { BRDctrl_FM401S_GETDECIM, 1, 0, 0, NULL };

int CFm401sSrv::ReadErrData16(void* pDev, U08 nChan, U32 nReg, U32& nVal)
{
	CModule* pModule = (CModule*)pDev;
	if (m_AuroraTetrNum[nChan] != 0xFF)
	{
		IndRegWrite(pModule, m_AuroraTetrNum[nChan], FM401S_REG_ERR_ADR, nReg);
		IndRegRead(pModule, m_AuroraTetrNum[nChan], FM401S_SPD_REG_ERR_DATA, &nVal);
	}
	else
		return BRDerr_BAD_PARAMETER;
	return 0;
}

int CFm401sSrv::ReadErrData32(void* pDev, U08 nChan, U32 nReg, U32& nVal)
{
	U32 tmpVal=0;
	S32 nStatus = 0;
	nStatus = ReadErrData16(pDev, nChan, nReg, tmpVal);
	nVal = tmpVal;
	nStatus = ReadErrData16(pDev, nChan, nReg+1, tmpVal);
	nVal += (tmpVal << 16);
	return nStatus;
}

int CFm401sSrv::ReadErrData64(void* pDev, U08 nChan, U32 nReg, unsigned long long& nVal)
{
	U32 tmpVal = 0;
	S32 nStatus = 0;
	nStatus = ReadErrData32(pDev, nChan, nReg, tmpVal);
	nVal = tmpVal;
	nStatus = ReadErrData32(pDev, nChan, nReg + 2, tmpVal);
	nVal += ((unsigned long long)tmpVal << 32);
	return nStatus;
}

CFm401sSrv::CFm401sSrv(int idx, int srv_num, CModule* pModule, PFM401SSRV_CFG pCfg) :
	CService(idx, _BRDC("FM401S"), -1, pModule, pCfg, sizeof(PFM401SSRV_CFG))
{
	m_attribute =
		BRDserv_ATTR_DIRECTION_IN |
		BRDserv_ATTR_DIRECTION_OUT |
		BRDserv_ATTR_STREAMABLE_IN |
		BRDserv_ATTR_STREAMABLE_OUT /* |
		BRDserv_ATTR_SDRAMABLE*/;

	m_SrvNum = srv_num;
	for (int i = 0; i < 8; i++)
		m_AuroraTetrNum[i] = 0xFF;

	Init(&SETCHANMASK_CMD, (CmdEntry)&CFm401sSrv::CtrlChanMask);
	Init(&GETCHANMASK_CMD, (CmdEntry)&CFm401sSrv::CtrlChanMask);
	Init(&SETCLKMODE_CMD, (CmdEntry)&CFm401sSrv::CtrlClkMode);
	Init(&GETCLKMODE_CMD, (CmdEntry)&CFm401sSrv::CtrlClkMode);
	Init(&GETSTATUS_CMD, (CmdEntry)&CFm401sSrv::CtrlStatus);
	Init(&SETTESTMODE_CMD, (CmdEntry)&CFm401sSrv::CtrlTestMode);
	Init(&GETTESTMODE_CMD, (CmdEntry)&CFm401sSrv::CtrlTestMode);
	Init(&START_CMD, (CmdEntry)&CFm401sSrv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&CFm401sSrv::CtrlStart);
	Init(&GETTESTRESULT_CMD, (CmdEntry)&CFm401sSrv::CtrlTestResult);
	Init(&SETDIR_CMD, (CmdEntry)&CFm401sSrv::CtrlDir);
	Init(&GETDIR_CMD, (CmdEntry)&CFm401sSrv::CtrlDir);
	Init(&GETRXTX_CMD, (CmdEntry)&CFm401sSrv::CtrlRxTx);
	Init(&SETCLKVAL_CMD, (CmdEntry)&CFm401sSrv::CtrlRate);
	Init(&GETCLKVAL_CMD, (CmdEntry)&CFm401sSrv::CtrlRate);
	Init(&PREPARE_CMD, (CmdEntry)&CFm401sSrv::CtrlPrepare);	
	Init(&SETDECIM_CMD, (CmdEntry)&CFm401sSrv::CtrlDecim);
	Init(&GETDECIM_CMD, (CmdEntry)&CFm401sSrv::CtrlDecim);
}

int CFm401sSrv::CtrlIsAvailable(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	PSERV_CMD_IsAvailable pServAvailable = (PSERV_CMD_IsAvailable)args;
	pServAvailable->attr = m_attribute;
	m_MainTetrNum = GetTetrNum(pModule, m_index, MAIN_TETR_ID);
	m_Fm401sTetrNum = GetTetrNumEx(pModule, m_index, FM401S_TETR_ID, m_SrvNum+1);
	U32 nVal = 0;
	m_nAuroras = 0;
	for (int i = 0; i < 16; i++)
	{
		IndRegRead(pModule, i, ADM2IFnr_ID, &nVal);
		if (nVal == FM401S_AURORA_TETR_ID)
		{
			m_AuroraTetrNum[m_nAuroras] = i;
			m_nAuroras++;
		}
	}

	m_isAvailable = 0;
	if ((m_MainTetrNum != -1) && (m_Fm401sTetrNum != -1))
	{
		m_isAvailable = 1;
		PFM401SSRV_CFG pSrvCfg = (PFM401SSRV_CFG)m_pConfig;
		if (!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;
			ADM2IF_MODE0 pMode0 = { 0 };
			pMode0.ByBits.Reset = 1;
			pMode0.ByBits.FifoRes = 1;
			IndRegWrite(pModule, m_Fm401sTetrNum, ADM2IFnr_MODE0, pMode0.AsWhole);
			for (int i = 1; i < 32; i++)
				IndRegWrite(pModule, m_Fm401sTetrNum, i, 0);
			pMode0.ByBits.Reset = 0;
			pMode0.ByBits.FifoRes =0;
			IndRegWrite(pModule, m_Fm401sTetrNum, ADM2IFnr_MODE0, pMode0.AsWhole);
			for (int j = 0; j < 8; j++)
			{
				if (m_AuroraTetrNum[j] != 0xFF)
				{
					pMode0.ByBits.Reset = 1;
					pMode0.ByBits.FifoRes = 1;
					IndRegWrite(pModule, m_AuroraTetrNum[j], ADM2IFnr_MODE0, pMode0.AsWhole);
					for (int i = 1; i < 32; i++)
						IndRegWrite(pModule, m_AuroraTetrNum[j], i, 0);
					pMode0.ByBits.Reset = 0;
					pMode0.ByBits.FifoRes = 0;
					IndRegWrite(pModule, m_AuroraTetrNum[j], ADM2IFnr_MODE0, pMode0.AsWhole);
				}
			}
			FM401S_CONTROL pControl = { 0 };
			pControl.ByBits.PhyEnable = 1;
			pControl.ByBits.LinkEnable = 1;
			//IndRegWrite(pModule, m_Fm401sTetrNum, FM401S_REG_CONTROL, pControl.AsWhole);
			Si571GetFxTal(pModule, &(pSrvCfg->dGenFxtal));
		}
	}
	else
		m_isAvailable = 0;
	pServAvailable->isAvailable = m_isAvailable;
	return 0;
}

int CFm401sSrv::CtrlChanMask(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	PFM401S_CHANMASK rChanMask = (PFM401S_CHANMASK)args;
	PFM401SSRV_CFG pSrvCfg = (PFM401SSRV_CFG)m_pConfig;
	FM401S_MODE1 pMode1 = { 0 };
	IndRegRead(pModule, m_Fm401sTetrNum, ADM2IFnr_MODE1, (U32*)&(pMode1.AsWhole));
	if (BRDctrl_FM401S_SETCHANMASK == cmd)
	{		
		pSrvCfg->nChanMask = rChanMask->nChanMask;
		//nStatus = pModule->RegCtrl(DEVScmd_REGWRITEIND, &rParam);
		if (rChanMask->nStreamChan > 0)
			pMode1.AsWhole = (1 << (rChanMask->nStreamChan-1));
		IndRegWrite(pModule, m_Fm401sTetrNum, ADM2IFnr_MODE1, pMode1.AsWhole);
	}
	else
		if (BRDctrl_FM401S_GETCHANMASK == cmd)
		{
			rChanMask->nChanMask = pSrvCfg->nChanMask;
			rChanMask->nStreamChan = 0;
			for (int i = 0; i < 8; i++)
				rChanMask->nStreamChan += (i+1)*(pMode1.AsWhole >> i);
			//rChanMask->nStreamChan = pMode1.ByBits.Rx0En + (pMode1.ByBits.Rx1En * 2) + (pMode1.ByBits.Rx2En * 3) + (pMode1.ByBits.Rx3En * 4);
		}
	return BRDerr_OK;
}

int CFm401sSrv::CtrlClkMode(void* pDev, void* pServData, ULONG cmd, void* args)
{
	return 0;
}

int CFm401sSrv::CtrlStatus(void* pDev, void* pServData, ULONG cmd, void* args)
{
	IPC_delay(50);
	CModule* pModule = (CModule*)pDev;	
	PFM401S_GETSTATUS rStatus = (PFM401S_GETSTATUS)args;	
	FM401S_AURORA_STATUS pStatusReg = { 0 };
	if (BRDctrl_FM401S_GETSTATUS == cmd)
	{
		for (int i = 0; i < 8; i++)
		{
			if (m_AuroraTetrNum[i] != 0xFF)
			{
				DirRegRead(pModule, m_AuroraTetrNum[i], ADM2IFnr_STATUS, (U32*)&(pStatusReg.AsWhole));
				rStatus->isLaneUp[i] = pStatusReg.ByBits.LaneUp;
				rStatus->isPLL_Lock[i] = pStatusReg.ByBits.GT_PLL_Lock;
			}
		}
		//rParam.tetr = m_Fm401sTetrNum;
		//rParam.reg = FM401S_REG_LINE_STATUS;
		//pModule->RegCtrl(DEVScmd_REGREADIND, &rParam);
		//for (int i = 0; i < 4; i++)
		//	rStatus->isLineUp[i] = ~((rParam.val >> i) & 1);
		//rParam.reg = FM401S_REG_LINK_STATUS;
		//pModule->RegCtrl(DEVScmd_REGREADIND, &rParam);
		//for (int i = 0; i < 4; i++)
		//	rStatus->isLinkUp[i] = ((rParam.val >> i) & 1);
		//rParam.reg = FM401S_REG_DATA_STATUS;
		//pModule->RegCtrl(DEVScmd_REGREADIND, &rParam);
		//for (int i = 0; i < 4; i++)
		//	rStatus->isData[i] = ((rParam.val >> i) & 1);
		//for (int i = 0; i < 4; i++)
		//{
		//	rParam.reg = FM401S_REG_LINK0_ERR_CORR + i;
		//	pModule->RegCtrl(DEVScmd_REGREADIND, &rParam);
		//	rStatus->wLinkErrCorr[i] = rParam.val;
		//}
	}
	return BRDerr_OK;
}

int CFm401sSrv::CtrlTestMode(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg rParam = {0};
	PFM401S_TESTMODE pTestMode = (PFM401S_TESTMODE)args;	
	FM401S_TEST_SEQUENCE pTestReg = {0};
	if (BRDctrl_FM401S_SETTESTMODE == cmd)
	{
		if (pTestMode->isCntEnable)
			pTestReg.ByBits.Cnt_Enable = 1;
		if (pTestMode->isGenEnable)
			pTestReg.ByBits.Gen_Enable = 1;
		for (int i = 0; i < 8; i++)
			if (m_AuroraTetrNum[i] != 0xFF)
				IndRegWrite(pModule, m_AuroraTetrNum[i], FM401S_REG_TEST_SEQUENCE, pTestReg.AsWhole);
	}
	else
		if (BRDctrl_FM401S_GETTESTMODE == cmd)
		{
			if (m_AuroraTetrNum[0] != 0xFF)
				IndRegRead(pModule, m_AuroraTetrNum[0], FM401S_REG_TEST_SEQUENCE, (U32*)&(pTestReg.AsWhole));
			//*(PULONG)args = rParam.val;
			if (pTestReg.ByBits.Cnt_Enable == 1)
				pTestMode->isCntEnable = 1;
			else
				pTestMode->isCntEnable = 0;
			if (pTestReg.ByBits.Gen_Enable == 1)
				pTestMode->isGenEnable = 1;
			else
				pTestMode->isGenEnable = 0;
		}
	return BRDerr_OK;
}

int CFm401sSrv::CtrlStart(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	PFM401SSRV_CFG pSrvCfg = (PFM401SSRV_CFG)m_pConfig;
	FM401S_AURORA_MODE1 pMode1 = { 0 };	
	if (BRDctrl_FM401S_START == cmd)
	{		
		for (int i = 0; i < 8; i++)
			if ((pSrvCfg->nChanMask >> i) & 1)
			{
				if (m_AuroraTetrNum[i] != 0xFF)
				{
					IndRegRead(pModule, m_AuroraTetrNum[i], ADM2IFnr_MODE1, (U32*)&(pMode1.AsWhole));
					pMode1.ByBits.StartTx = 1;
					if ((pSrvCfg->nDirChanMask >> i) & 1)
						IndRegWrite(pModule, m_AuroraTetrNum[i], ADM2IFnr_MODE1, pMode1.AsWhole);
				}
			}
	}
	else
	{
		ADM2IF_MODE0 pMode0 = { 0 };
		IndRegRead(pModule, m_Fm401sTetrNum, ADM2IFnr_MODE0, &(pMode0.AsWhole));
		pMode0.ByBits.Start = 0;
		IndRegWrite(pModule, m_Fm401sTetrNum, ADM2IFnr_MODE0, pMode0.AsWhole);
		for (int i = 0; i < 8; i++)
			if ((pSrvCfg->nChanMask >> i) & 1)
			{
				if (m_AuroraTetrNum[i] != 0xFF)
				{
					IndRegRead(pModule, m_AuroraTetrNum[i], ADM2IFnr_MODE0, &(pMode0.AsWhole));
					pMode0.ByBits.Start = 0;
					IndRegWrite(pModule, m_AuroraTetrNum[i], ADM2IFnr_MODE0, pMode0.AsWhole);
					IndRegRead(pModule, m_AuroraTetrNum[i], ADM2IFnr_MODE1, (U32*)&(pMode1.AsWhole));
					pMode1.ByBits.StartTx = 0;
					IndRegWrite(pModule, m_AuroraTetrNum[i], ADM2IFnr_MODE1, pMode1.AsWhole);
				}
			}
	}
	return BRDerr_OK;
}

int CFm401sSrv::CtrlTestResult(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	PFM401S_GETTESTRESULT pTest = (PFM401S_GETTESTRESULT)args;
	if (m_AuroraTetrNum[pTest->nChan] == 0xFF)
		return BRDerr_BAD_PARAMETER;
	U32 nErrors = 0;
	S32 nStatus = 0;
	nStatus = ReadErrData32(pDev, pTest->nChan, FM401S_ERR_DATA_TOTAL_ERR_L, nErrors);

	if (nErrors > 32)
		nErrors = 32;
	for (U32 i = 0; i < nErrors; i++)
	{
		nStatus = ReadErrData64(pDev, pTest->nChan, FM401S_ERR_DATA_READ_D0 + (i * 0x10), pTest->lReadWord[i]);
		nStatus = ReadErrData64(pDev, pTest->nChan, FM401S_ERR_DATA_EXPECT_D0 + (i * 0x10), pTest->lExpectWord[i]);
		nStatus = ReadErrData16(pDev, pTest->nChan, FM401S_ERR_DATA_INDEX + (i * 0x10), pTest->nIndex[i]);
		nStatus = ReadErrData32(pDev, pTest->nChan, FM401S_ERR_DATA_BLOCK_D0 + (i * 0x10), pTest->nBlock[i]);
	}
	nStatus = ReadErrData32(pDev, pTest->nChan, FM401S_ERR_DATA_TOTAL_ERR_L, pTest->nTotalError);
	
	return BRDerr_OK;
}

int CFm401sSrv::CtrlDir(void* pDev, void* pServData, ULONG cmd, void* args)
{	
	PFM401SSRV_CFG pSrvCfg = (PFM401SSRV_CFG)m_pConfig;
	PFM401S_DIR rParam = (PFM401S_DIR)args;
	if (BRDctrl_FM401S_SETDIR == cmd)
	{
		pSrvCfg->nDirChanMask = rParam->nChan[0] & 1;
		for (int i = 1; i < 8; i++)
			pSrvCfg->nDirChanMask |= ((rParam->nChan[i] & 1) << i);
	}
	return 0;
}

int CFm401sSrv::CtrlRxTx(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	PFM401S_GETRXTXSTATUS pTest = (PFM401S_GETRXTXSTATUS)args;
	S32 nStatus = 0;
	for (int i = 0; i < 8; i++)
	{
		if (m_AuroraTetrNum[i] != 0xFF)
		{
			nStatus = ReadErrData32(pDev, i, FM401S_ERR_ADDR_BLOCK_RD_L, pTest->nBlockRead[i]);
			nStatus = ReadErrData32(pDev, i, FM401S_ERR_ADDR_BLOCK_WR_L, pTest->nBlockWrite[i]);
			nStatus = ReadErrData32(pDev, i, FM401S_ERR_ADDR_BLOCK_ERR_L, pTest->nBlockErr[i]);
			nStatus = ReadErrData32(pDev, i, FM401S_ERR_ADDR_BLOCK_OK_L, pTest->nBlockOk[i]);
		}
	}
	return 0;
}

int CFm401sSrv::CtrlRate(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	double* dRate = (double*)args;
	if (BRDctrl_FM401S_SETCLKVALUE == cmd)
	{
		Si571SetRate(pModule, dRate);
	}
	else
		if (BRDctrl_FM401S_GETCLKVALUE == cmd)
		{
			Si571GetRate(pModule, dRate);
		}
	return 0;
}

int CFm401sSrv::CtrlPrepare(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	S32 nStatus = 0;
	FM401S_CONTROL rCtrl = { 0 };
	U32 nVal = 0;
	bool isReady = true, isCont = false;
	PFM401SSRV_CFG pSrvCfg = (PFM401SSRV_CFG)m_pConfig;
	SpdWrite(pModule, SPDdev_FM401S_TCA62425, 0xC, 0xF);
	SpdWrite(pModule, SPDdev_FM401S_TCA62425, 0xD, 0xF0);
	SpdWrite(pModule, SPDdev_FM401S_TCA62425, 0xE, 0xFF);
	SpdWrite(pModule, SPDdev_FM401S_TCA62425, 0x5, 7);
	U32 clos = -1;
	U32 abs = -1;
	IPC_TIMEVAL tStart, tStop;
	IPC_getTime(&tStart);
	do
	{
		clos = -1;
		abs = -1;
		SpdRead(pModule, SPDdev_FM401S_TCA62425, 0, &nVal);
		clos &= (nVal & 1) ? -1 : ~0x10; // clos(4)
		abs &= (nVal & 2) ? -1 : ~0x10; // abs(4)
		clos &= (nVal & 4) ? -1 : ~0x20; // clos(5)
		abs &= (nVal & 8) ? -1 : ~0x20; // abs(5)
		clos &= (nVal & 0x10) ? -1 : ~0x40; // clos(6)
		abs &= (nVal & 0x20) ? -1 : ~0x40; // abs(6)
		clos &= (nVal & 0x80) ? -1 : ~0x80; // clos(7)
		abs &= (nVal & 0x40) ? -1 : ~0x80; // abs(7)
		SpdRead(pModule, SPDdev_FM401S_TCA62425, 1, &nVal);
		clos &= (nVal & 0x10) ? -1 : ~1; // clos(0)
		abs &= (nVal & 0x40) ? -1 : ~1; // abs(0)
		clos &= (nVal & 0x80) ? -1 : ~2; // clos(1)
		SpdRead(pModule, SPDdev_FM401S_TCA62425, 2, &nVal);
		abs &= (nVal & 2) ? -1 : ~2; // abs(1)
		abs &= (nVal & 0x10) ? -1 : ~4; // abs(2)
		clos &= (nVal & 0x20) ? -1 : ~4; // clos(2)
		abs &= (nVal & 0x40) ? -1 : ~8; // abs(3)
		clos &= (nVal & 0x80) ? -1 : ~8; // clos(3)
		nVal = ~(abs | clos);
		IPC_getTime(&tStop);
		if (IPC_getDiffTime(&tStart, &tStop) > 1000)
			break;
	} while (abs != clos);
	IndRegWrite(pModule, m_Fm401sTetrNum, 0xB, nVal);
	if (abs != clos)
		BRDC_printf(_BRDC("ABS : 0x%2X, CLOS : 0x%2X\n"), abs & 0xFF, clos & 0xFF);
	ADM2IF_MODE0 pMode0 = { 0 };
	IndRegRead(pModule, m_Fm401sTetrNum, ADM2IFnr_MODE0, &(pMode0.AsWhole));
	pMode0.ByBits.Start = 1;
	IndRegWrite(pModule, m_Fm401sTetrNum, ADM2IFnr_MODE0, pMode0.AsWhole);
	for (int i = 0; i < 8; i++)
		if ((pSrvCfg->nChanMask >> i) & 1)
		{
			if (m_AuroraTetrNum[i] != 0xFF)
			{
				IndRegRead(pModule, m_AuroraTetrNum[i], ADM2IFnr_MODE0, &(pMode0.AsWhole));
				pMode0.ByBits.Start = 1;
				IndRegWrite(pModule, m_AuroraTetrNum[i], ADM2IFnr_MODE0, pMode0.AsWhole);
			}
		}

	return nStatus;
}

int CFm401sSrv::CtrlDecim(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	PFM401SSRV_CFG pSrvCfg = (PFM401SSRV_CFG)m_pConfig;
	PU32 nDecim = (PU32)args;
	if (BRDctrl_FM401S_SETDECIM == cmd)
	{		
		for (int i = 0; i < 8; i++)
			if ((pSrvCfg->nChanMask >> i) & 1)
			{
				if (m_AuroraTetrNum[i] != 0xFF)
					IndRegWrite(pModule, m_AuroraTetrNum[i], FM401S_REG_DECIM, *nDecim);
			}
	}
	else
		if (BRDctrl_FM401S_GETDECIM == cmd)
		{
			for (int i = 0; i < 8; i++)
				if ((pSrvCfg->nChanMask >> i) & 1)
				{
					if (m_AuroraTetrNum[i] != 0xFF)
						IndRegRead(pModule, m_AuroraTetrNum[i], FM401S_REG_DECIM, nDecim);
				}
		}
		else
			return BRDerr_ERROR;
	return BRDerr_OK;
}

int CFm401sSrv::CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args)
{
	PFM401SSRV_CFG pSrvCfg = (PFM401SSRV_CFG)m_pConfig;
	pSrvCfg->isAlreadyInit = 0;
	return 0;
}

int CFm401sSrv::SpdRead(CModule* pModule, U32 spdType, U32 regAdr, U32* pRegVal)
{
	S32 status, spdCtrl = 0x1;
	U32 nVal=0;
	
	for (;;)
	{
		DirRegRead(pModule, m_Fm401sTetrNum, 0, &nVal);
		if (nVal & 0x1)
			break;
	}
	if (spdType == SPDdev_FM401S_GEN) spdCtrl |= (0x55 << 4);
	if (spdType == SPDdev_FM401S_TCA62425) spdCtrl |= 0x220;
	IndRegWrite(pModule, m_Fm401sTetrNum, FM401S_SPD_DEVICE, spdType & 0xFF);
	IndRegWrite(pModule, m_Fm401sTetrNum, FM401S_SPD_ADDR, regAdr);
	IndRegWrite(pModule, m_Fm401sTetrNum, FM401S_SPD_CTRL, spdCtrl);
	for (;;)
	{
		DirRegRead(pModule, m_Fm401sTetrNum, 0, &nVal);
		if (nVal & 0x1)
			break;
	}
	status = IndRegRead(pModule, m_Fm401sTetrNum, FM401S_SPD_DATA, pRegVal);	

	return status;
}

int CFm401sSrv::SpdWrite(CModule* pModule, U32 spdType, U32 regAdr, U32 regVal)
{
	S32 status, spdCtrl = 0x2;
	U32 nVal = 0;

	for (;;)
	{
		DirRegRead(pModule, m_Fm401sTetrNum, 0, &nVal);
		if (nVal & 0x1)
			break;
	}
	if (spdType == SPDdev_FM401S_GEN) spdCtrl |= (0x55 << 4);
	if (spdType == SPDdev_FM401S_TCA62425) spdCtrl |= 0x220;
	IndRegWrite(pModule, m_Fm401sTetrNum, FM401S_SPD_DEVICE, spdType & 0xFF);
	IndRegWrite(pModule, m_Fm401sTetrNum, FM401S_SPD_ADDR, regAdr);
	IndRegWrite(pModule, m_Fm401sTetrNum, FM401S_SPD_DATA, regVal);
	IndRegWrite(pModule, m_Fm401sTetrNum, FM401S_SPD_CTRL, spdCtrl);
	for (;;)
	{
		status = DirRegRead(pModule, m_Fm401sTetrNum, 0, &nVal);
		if (nVal & 0x1)
			break;
	}

	return status;
}

int CFm401sSrv::Si571SetRate(CModule* pModule, double* pRate)
{
	FM401SSRV_CFG* pSrvCfg = (PFM401SSRV_CFG)m_pConfig;
	U32			regData[20];
	int			regAdr;
	int			status = BRDerr_OK;
	//
	// Скорректировать частоту, если необходимо
	//
	if (*pRate > (double)pSrvCfg->nGen1RefMax)
	{
		*pRate = (double)pSrvCfg->nGen1RefMax;
		status = BRDerr_WARN;
	}

	SI571_SetRate(pRate, pSrvCfg->dGenFxtal, regData);

	//
	// Запрограммировать генератор
	//
	SpdWrite(pModule, SPDdev_FM401S_GEN, 137, 0x10);		// Freeze DCO
	for (regAdr = 7; regAdr <= 18; regAdr++)
		SpdWrite(pModule, SPDdev_FM401S_GEN, regAdr, regData[regAdr]);
	SpdWrite(pModule, SPDdev_FM401S_GEN, 137, 0x0);		// Unfreeze DCO
	SpdWrite(pModule, SPDdev_FM401S_GEN, 135, 0x40);		// Assert the NewFreq bit

	return status;
}

int CFm401sSrv::Si571GetRate(CModule* pModule, double* pRate)
{
	FM401SSRV_CFG* pSrvCfg = (PFM401SSRV_CFG)m_pConfig;
	U32			regData[20];
	//ULONG		clkSrc;
	int			regAdr;

	*pRate = 0.0;

	//
	// Проверить источник частоты
	//
	//GetClkSource( pModule, clkSrc );
	//if( clkSrc != BRDclks_DAC_SUBGEN )
	//	BRDerr_ERROR;

	//
	// Считать регистры Si570/Si571
	//
	for (regAdr = 7; regAdr <= 12; regAdr++)
		SpdRead(pModule, SPDdev_FM401S_GEN, regAdr, &regData[regAdr]);

	SI571_GetRate(pRate, pSrvCfg->dGenFxtal, regData);
	//printf( "Si571GetRate: %x, %x, %x, %x, %x, %x\n", regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );

	return BRDerr_OK;
}

int CFm401sSrv::Si571GetFxTal(CModule* pModule, double* dGenFxTal)
{
	FM401SSRV_CFG* pSrvCfg = (PFM401SSRV_CFG)m_pConfig;
	U32 regAdr;
	pSrvCfg->dGenFxtal = 0.0;
	if ((pSrvCfg->bGen1Type == 1) && (pSrvCfg->nGen1Ref != 0))
	{
		U32			regData[20];		

		//
		// Подать питание на Si570/Si571
		//		
		SpdWrite(pModule, SPDdev_FM401S_GEN, 135, 0x1);		// Recall
		IPC_delay(10);

		//
		// Считать регистры Si570/Si571
		//
		for (regAdr = 7; regAdr <= 12; regAdr++)
		{
			SpdRead(pModule, SPDdev_FM401S_GEN, regAdr, &regData[regAdr]);
			printf("Si571: reg%02d = 0x%02X\n", regAdr, regData[regAdr]);
		}

		//
		// Рассчитать частоту кварца
		//
		SI571_CalcFxtal(&(pSrvCfg->dGenFxtal), (double)(pSrvCfg->nGen1Ref), regData);
		printf(">> XTAL = %f kHz\n", pSrvCfg->dGenFxtal / 1000.0);
		printf(">> GREF = %f kHz\n", ((double)(pSrvCfg->nGen1Ref)) / 1000.0);
	}
	return 0;
}
