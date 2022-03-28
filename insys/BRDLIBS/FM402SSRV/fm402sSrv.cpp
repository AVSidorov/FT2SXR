#include "module.h"
#include "fm402sSrv.h"

#define CURRFILE "FM402SSRV"

static CMD_Info SETCHANMASK_CMD = { BRDctrl_FM402S_SETCHANMASK, 0, 0, 0, NULL };
static CMD_Info GETCHANMASK_CMD = { BRDctrl_FM402S_GETCHANMASK, 1, 0, 0, NULL };
static CMD_Info SETCLKMODE_CMD = { BRDctrl_FM402S_SETCLKMODE, 0, 0, 0, NULL };
static CMD_Info GETCLKMODE_CMD = { BRDctrl_FM402S_GETCLKMODE, 1, 0, 0, NULL };
static CMD_Info GETSTATUS_CMD = { BRDctrl_FM402S_GETSTATUS, 1, 0, 0, NULL };
static CMD_Info SETTESTMODE_CMD = { BRDctrl_FM402S_SETTESTMODE, 0, 0, 0, NULL };
static CMD_Info GETTESTMODE_CMD = { BRDctrl_FM402S_GETTESTMODE, 1, 0, 0, NULL };
static CMD_Info START_CMD = { BRDctrl_FM402S_START, 0, 0, 0, NULL };
static CMD_Info STOP_CMD = { BRDctrl_FM402S_STOP, 0, 0, 0, NULL };
static CMD_Info GETTESTRESULT_CMD = { BRDctrl_FM402S_GETTESTRESULT, 1, 0, 0, NULL };
static CMD_Info SETDIR_CMD = { BRDctrl_FM402S_SETDIR, 0, 0, 0, NULL };
static CMD_Info GETDIR_CMD = { BRDctrl_FM402S_GETDIR, 1, 0, 0, NULL };
static CMD_Info GETRXTX_CMD = { BRDctrl_FM402S_GETRXTXSTATUS, 1, 0, 0, NULL };
static CMD_Info PREPARE_CMD = { BRDctrl_FM402S_PREPARE, 1, 0, 0, NULL };

int CFm402sSrv::ReadErrData16(void* pDev, U08 nChan, U32 nReg, U32& nVal)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg rParam = { 0 };
	
	rParam.idxMain = m_index;
	rParam.tetr = m_Fm402sTetrNum;

	rParam.reg = FM402S_REG_ERR_ADR;
	rParam.val = (nChan * 0x1000) + nReg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &rParam);
	rParam.reg = FM402S_SPD_REG_ERR_DATA;
	pModule->RegCtrl(DEVScmd_REGREADIND, &rParam);
	nVal = rParam.val;	

	return 0;
}

int CFm402sSrv::ReadErrData32(void* pDev, U08 nChan, U32 nReg, U32& nVal)
{
	U32 tmpVal=0;
	ReadErrData16(pDev, nChan, nReg, tmpVal);
	nVal = tmpVal;
	ReadErrData16(pDev, nChan, nReg+1, tmpVal);
	nVal += (tmpVal << 16);
	return 0;
}

int CFm402sSrv::ReadErrData64(void* pDev, U08 nChan, U32 nReg, unsigned long long& nVal)
{
	U32 tmpVal = 0;
	ReadErrData32(pDev, nChan, nReg, tmpVal);
	nVal = tmpVal;
	ReadErrData32(pDev, nChan, nReg + 2, tmpVal);
	nVal += ((unsigned long long)tmpVal << 32);
	return 0;
}

CFm402sSrv::CFm402sSrv(int idx, int srv_num, CModule* pModule, PFM402SSRV_CFG pCfg) :
	CService(idx, _BRDC("FM402S"), -1, pModule, pCfg, sizeof(PFM402SSRV_CFG))
{
	m_attribute =
		BRDserv_ATTR_DIRECTION_IN |
		BRDserv_ATTR_DIRECTION_OUT |
		BRDserv_ATTR_STREAMABLE_IN |
		BRDserv_ATTR_STREAMABLE_OUT |
		BRDserv_ATTR_SDRAMABLE;

	m_SrvNum = srv_num;

	Init(&SETCHANMASK_CMD, (CmdEntry)&CFm402sSrv::CtrlChanMask);
	Init(&GETCHANMASK_CMD, (CmdEntry)&CFm402sSrv::CtrlChanMask);
	Init(&SETCLKMODE_CMD, (CmdEntry)&CFm402sSrv::CtrlClkMode);
	Init(&GETCLKMODE_CMD, (CmdEntry)&CFm402sSrv::CtrlClkMode);
	Init(&GETSTATUS_CMD, (CmdEntry)&CFm402sSrv::CtrlStatus);
	Init(&SETTESTMODE_CMD, (CmdEntry)&CFm402sSrv::CtrlTestMode);
	Init(&GETTESTMODE_CMD, (CmdEntry)&CFm402sSrv::CtrlTestMode);
	Init(&START_CMD, (CmdEntry)&CFm402sSrv::CtrlStart);
	Init(&STOP_CMD, (CmdEntry)&CFm402sSrv::CtrlStart);
	Init(&GETTESTRESULT_CMD, (CmdEntry)&CFm402sSrv::CtrlTestResult);
	Init(&SETDIR_CMD, (CmdEntry)&CFm402sSrv::CtrlDir);
	Init(&GETDIR_CMD, (CmdEntry)&CFm402sSrv::CtrlDir);
	Init(&GETRXTX_CMD, (CmdEntry)&CFm402sSrv::CtrlRxTx);
	Init(&PREPARE_CMD, (CmdEntry)&CFm402sSrv::CtrlPrepare);
}

int CFm402sSrv::CtrlIsAvailable(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	PSERV_CMD_IsAvailable pServAvailable = (PSERV_CMD_IsAvailable)args;
	U32 nVal = 0xFF, nTetr = 0;
	pServAvailable->attr = m_attribute;
	m_MainTetrNum = GetTetrNum(pModule, m_index, MAIN_TETR_ID);
	//m_Fm402sTetrNum = GetTetrNumEx(pModule, m_index, FM402S_TETR_ID, m_SrvNum);
	m_Fm402sTetrNum = -1;
	for (int i = 0; i < 16; i++)
	{
		IndRegRead(pModule, i, ADM2IFnr_ID, &nVal);
		if (nVal == FM402S_TETR_ID)
		{
			if (nTetr == m_SrvNum)
			{
				m_Fm402sTetrNum = i;
				break;
			}
			nTetr++;
		}
	}

	m_isAvailable = 0;
	if ((m_MainTetrNum != -1) && (m_Fm402sTetrNum != -1))
	{
		m_isAvailable = 1;
		PFM402SSRV_CFG pSrvCfg = (PFM402SSRV_CFG)m_pConfig;
		if (!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;
			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_Fm402sTetrNum;
			RegParam.reg = ADM2IFnr_MODE0;
			RegParam.val = 0;
			PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&RegParam.val;
			pMode0->ByBits.Reset = 1;
			pMode0->ByBits.FifoRes = 1;	
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			for (int i = 1; i < 32; i++)
			{
				RegParam.reg = i;
				RegParam.val = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			}			
			RegParam.reg = ADM2IFnr_MODE0;
			pMode0->ByBits.Reset = 0;
			pMode0->ByBits.FifoRes = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			RegParam.reg = FM402S_REG_SFP_CONTROL;
			RegParam.val = 0;
			PFM402S_SFP_CONTROL pSFPControl = (PFM402S_SFP_CONTROL)&RegParam.val;
			pSFPControl->ByBits.SFP_Reset = 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			pSFPControl->ByBits.LowPowerMode = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			pSFPControl->ByBits.SFP_Reset = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
		}
	}
	else
		m_isAvailable = 0;
	pServAvailable->isAvailable = m_isAvailable;
	return 0;
}

int CFm402sSrv::CtrlChanMask(void* pDev, void* pServData, ULONG cmd, void* args)
{
	return 0;
}

int CFm402sSrv::CtrlClkMode(void* pDev, void* pServData, ULONG cmd, void* args)
{
	return 0;
}

int CFm402sSrv::CtrlStatus(void* pDev, void* pServData, ULONG cmd, void* args)
{
	IPC_delay(50);
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg rParam = { 0 };
	PFM402S_GETSTATUS rStatus = (PFM402S_GETSTATUS)args;
	rParam.idxMain = m_index;
	rParam.tetr = m_Fm402sTetrNum;
	rParam.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &rParam);	
	PFM402S_STATUS pStatusReg = (PFM402S_STATUS)&rParam.val;
	if (BRDctrl_FM402S_GETSTATUS == cmd)
	{
		//*(PULONG)args = rParam.val;
		if (pStatusReg->ByBits.ChanUp == 0)
			rStatus->isChanUp = 1;
		else
			rStatus->isChanUp = 0;
		if (pStatusReg->ByBits.PLL_Lock == 1)
			rStatus->isPLL_Lock = 1;
		else
			rStatus->isPLL_Lock = 0;
		if (pStatusReg->ByBits.QSFP_LINE_UP_0 == 1)
			rStatus->isQSFPLineUp[0] = 1;
		else
			rStatus->isQSFPLineUp[0] = 0;
		if (pStatusReg->ByBits.QSFP_LINE_UP_1 == 1)
			rStatus->isQSFPLineUp[1] = 1;
		else
			rStatus->isQSFPLineUp[1] = 0;
		if (pStatusReg->ByBits.QSFP_LINE_UP_2 == 1)
			rStatus->isQSFPLineUp[2] = 1;
		else
			rStatus->isQSFPLineUp[2] = 0;
		if (pStatusReg->ByBits.QSFP_LINE_UP_3 == 1)
			rStatus->isQSFPLineUp[3] = 1;
		else
			rStatus->isQSFPLineUp[3] = 0;
	}
	return BRDerr_OK;
}

int CFm402sSrv::CtrlTestMode(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg rParam = {0};
	PFM402S_TESTMODE pTestMode = (PFM402S_TESTMODE)args;
	rParam.idxMain = m_index;
	rParam.tetr = m_Fm402sTetrNum;
	rParam.reg = FM402S_REG_TEST_SEQUENCE;
	pModule->RegCtrl(DEVScmd_REGREADIND, &rParam);
	PFM402S_TEST_SEQUENCE pTestReg = (PFM402S_TEST_SEQUENCE)&rParam.val;
	if (BRDctrl_FM402S_SETTESTMODE == cmd)
	{
		if (pTestMode->isCntEnable)
			pTestReg->ByBits.Cnt_Enable = 1;
		if (pTestMode->isGenEnable)
			pTestReg->ByBits.Gen_Enable = 1;
		//rParam.val = *(PULONG)args;
		//BRDC_printf(_BRDC("set test mode tetr=%d mode=%d\n"), m_Fm402sTetrNum, rParam.val);
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &rParam);
	}
	else
		if (BRDctrl_FM402S_GETTESTMODE == cmd)
		{
			//*(PULONG)args = rParam.val;
			if (pTestReg->ByBits.Cnt_Enable == 1)
				pTestMode->isCntEnable = 1;
			else
				pTestMode->isCntEnable = 0;
			if (pTestReg->ByBits.Gen_Enable == 1)
				pTestMode->isGenEnable = 1;
			else
				pTestMode->isGenEnable = 0;
		}
	return BRDerr_OK;
}

int CFm402sSrv::CtrlStart(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg rParam1 = { 0 };
	rParam1.idxMain = m_index;
	rParam1.tetr = m_Fm402sTetrNum;
	rParam1.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &rParam1);
	PFM402S_MODE1 pMode1 = (PFM402S_MODE1)&rParam1.val;
	if (BRDctrl_FM402S_START == cmd)
	{
		pMode1->ByBits.StartTx = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &rParam1);
	}
	else
	{
		pMode1->ByBits.StartTx = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &rParam1);
		ADM2IF_MODE0 pMode0 = { 0 };
		IndRegRead(pModule, m_Fm402sTetrNum, ADM2IFnr_MODE0, &(pMode0.AsWhole));
		pMode0.ByBits.Start = 0;
		IndRegWrite(pModule, m_Fm402sTetrNum, ADM2IFnr_MODE0, pMode0.AsWhole);
	}
	return BRDerr_OK;
}

int CFm402sSrv::CtrlTestResult(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	PFM402S_GETTESTRESULT pTest = (PFM402S_GETTESTRESULT)args;
	U32 nErrors = 0;

	ReadErrData32(pDev, pTest->nChan, FM402S_ERR_DATA_TOTAL_ERR_L, nErrors);

	if (nErrors > 32)
		nErrors = 32;
	for (int i = 0; i < nErrors; i++)
	{
		ReadErrData64(pDev, pTest->nChan, FM402S_ERR_DATA_READ_D0 + (i * 0x10), pTest->lReadWord[i]);
		ReadErrData64(pDev, pTest->nChan, FM402S_ERR_DATA_EXPECT_D0 + (i * 0x10), pTest->lExpectWord[i]);
		ReadErrData16(pDev, pTest->nChan, FM402S_ERR_DATA_INDEX + (i * 0x10), pTest->nIndex[i]);
		ReadErrData32(pDev, pTest->nChan, FM402S_ERR_DATA_BLOCK_D0 + (i * 0x10), pTest->nBlock[i]);
	}
	ReadErrData32(pDev, pTest->nChan, FM402S_ERR_DATA_TOTAL_ERR_L, pTest->nTotalError);
	
	return BRDerr_OK;
}

int CFm402sSrv::CtrlDir(void* pDev, void* pServData, ULONG cmd, void* args)
{
	return 0;
}

int CFm402sSrv::CtrlRxTx(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	PFM402S_GETRXTXSTATUS pTest = (PFM402S_GETRXTXSTATUS)args;	

	ReadErrData32(pDev, 0, FM402S_ERR_ADDR_BLOCK_RD_L, pTest->nBlockRead);
	ReadErrData32(pDev, 0, FM402S_ERR_ADDR_BLOCK_WR_L, pTest->nBlockWrite);
	ReadErrData32(pDev, 0, FM402S_ERR_ADDR_BLOCK_ERR_L, pTest->nBlockErr);
	ReadErrData32(pDev, 0, FM402S_ERR_ADDR_BLOCK_OK_L, pTest->nBlockOk);

	return 0;
}

int CFm402sSrv::CtrlPrepare(void* pDev, void* pServData, ULONG cmd, void* args)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg rParam = { 0 };
	rParam.idxMain = m_index;
	rParam.tetr = m_Fm402sTetrNum;
	rParam.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &rParam);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&rParam.val;
	if (BRDctrl_FM402S_PREPARE == cmd)
	{		
		pMode0->ByBits.Start = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &rParam);
	}
	return BRDerr_OK;

	return 0;
}

int CFm402sSrv::CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args)
{
	PFM402SSRV_CFG pSrvCfg = (PFM402SSRV_CFG)m_pConfig;
	pSrvCfg->isAlreadyInit = 0;
	return 0;
}

void CFm402sSrv::SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName)
{
	FM402SSRV_CFG* pSrvCfg = (PFM402SSRV_CFG)m_pConfig;
	BRDCHAR Buffer[256] = { 0 }, ParamName[128] = {0};
	BRDCHAR* endptr=0;
	S32	err=0;

	double rate=0.0;
	IPC_getPrivateProfileString(SectionName, _BRDC("SamplingRate"), _BRDC("100000000.0"), Buffer, sizeof(Buffer), iniFilePath);
	rate = BRDC_atof(Buffer);
	SetRate(pDev, rate, 0.0);
}

int CFm402sSrv::SetRate(CModule* pModule, double& Rate, double ClkValue)
{
	return 0;
}

int CFm402sSrv::SpdRead(CModule* pModule, U32 spdType, U32 regAdr, U32* pRegVal)
{
	return 0;
}

int CFm402sSrv::SpdWrite(CModule* pModule, U32 spdType, U32 regAdr, U32 regVal)
{
	return 0;
}

int CFm402sSrv::Si571SetRate(CModule* pModule, double* pRate)
{
	FM402SSRV_CFG* pSrvCfg = (PFM402SSRV_CFG)m_pConfig;
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
	SpdWrite(pModule, SPDdev_FM402S_GEN, 137, 0x10);		// Freeze DCO
	for (regAdr = 7; regAdr <= 18; regAdr++)
		SpdWrite(pModule, SPDdev_FM402S_GEN, regAdr, regData[regAdr]);
	SpdWrite(pModule, SPDdev_FM402S_GEN, 137, 0x0);		// Unfreeze DCO
	SpdWrite(pModule, SPDdev_FM402S_GEN, 135, 0x40);		// Assert the NewFreq bit

	return status;
}

int CFm402sSrv::Si571GetRate(CModule* pModule, double* pRate)
{
	return 0;
}

int CFm402sSrv::Si571GetFxTal(CModule* pModule, double* dGenFxTal)
{
	return 0;
}
