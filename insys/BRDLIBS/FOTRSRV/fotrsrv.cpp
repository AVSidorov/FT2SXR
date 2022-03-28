/****************** File FotrSrv.cpp **********************************
 *
 * CTRL-command for BRD Driver for FOTR service on ADM-FOTR submodule
 *
 * (C) InSys by Dorokhin A. Jun 2006
 *
 **********************************************************************/

#include "module.h"
#include "fotrsrv.h"

#define	CURRFILE "FOTRSRV"

static CMD_Info SETMODE_CMD			= { BRDctrl_FOTR_SETMODE, 0, 0, 0, NULL};
static CMD_Info GETMODE_CMD			= { BRDctrl_FOTR_GETMODE, 1, 0, 0, NULL};
static CMD_Info GETCFG_CMD			= { BRDctrl_FOTR_GETCFG, 1, 0, 0, NULL};
static CMD_Info MSGFIFORESET_CMD	= { BRDctrl_FOTR_MSGFIFORESET, 0, 0, 0, NULL};
static CMD_Info SETTESTMODE_CMD		= { BRDctrl_FOTR_SETTESTMODE, 0, 0, 0, NULL};
static CMD_Info GETTESTMODE_CMD		= { BRDctrl_FOTR_GETTESTMODE, 1, 0, 0, NULL};

static CMD_Info RESET_CMD		= { BRDctrl_FOTR_RESET,		 0, 0, 0, NULL};
static CMD_Info SYNC_CMD		= { BRDctrl_FOTR_SYNC,		 0, 0, 0, NULL};
static CMD_Info DISCON_CMD		= { BRDctrl_FOTR_DISCONNECT, 0, 0, 0, NULL};
static CMD_Info CONNECT_CMD		= { BRDctrl_FOTR_CONNECT,	 0, 0, 0, NULL};
static CMD_Info CHCKCON_CMD		= { BRDctrl_FOTR_CHECKCONNECT, 0, 0, 0, NULL};
static CMD_Info SENDMSG_CMD		= { BRDctrl_FOTR_SENDMSG,	 0, 0, 0, NULL};
static CMD_Info RECEIVEMSG_CMD	= { BRDctrl_FOTR_RECEIVEMSG, 0, 0, 0, NULL};
static CMD_Info WRITERMREG_CMD	= { BRDctrl_FOTR_WRITERMREG, 0, 0, 0, NULL};
static CMD_Info READRMREG_CMD	= { BRDctrl_FOTR_READRMREG,	 0, 0, 0, NULL};
static CMD_Info WRITERMREGS_CMD	= { BRDctrl_FOTR_WRITERMREGS,0, 0, 0, NULL};
static CMD_Info READRMREGS_CMD	= { BRDctrl_FOTR_READRMREGS, 0, 0, 0, NULL};
static CMD_Info SETFIFOADDR_CMD	= { BRDctrl_FOTR_SETFIFOADDR, 0, 0, 0, NULL};
static CMD_Info GETFIFOADDR_CMD	= { BRDctrl_FOTR_GETFIFOADDR, 0, 0, 0, NULL};

static CMD_Info INFIFORESET_CMD		= { BRDctrl_FOTR_INFIFORESET,	  0, 0, 0, NULL};
static CMD_Info INENABLE_CMD		= { BRDctrl_FOTR_INENABLE,		  0, 0, 0, NULL};
static CMD_Info INFIFOSTATUS_CMD	= { BRDctrl_FOTR_INFIFOSTATUS,	  1, 0, 0, NULL};
static CMD_Info GETDATA_CMD			= { BRDctrl_FOTR_GETDATA,		  0, 0, 0, NULL};
static CMD_Info GETINSRCSTREAM_CMD	= { BRDctrl_FOTR_GETINSRCSTREAM,  1, 0, 0, NULL};

static CMD_Info OUTFIFORESET_CMD	= { BRDctrl_FOTR_OUTFIFORESET,	  0, 0, 0, NULL};
static CMD_Info OUTENABLE_CMD		= { BRDctrl_FOTR_OUTENABLE,		  0, 0, 0, NULL};
static CMD_Info OUTFIFOSTATUS_CMD	= { BRDctrl_FOTR_OUTFIFOSTATUS,	  1, 0, 0, NULL};
static CMD_Info PUTDATA_CMD			= { BRDctrl_FOTR_PUTDATA,		  0, 0, 0, NULL};
static CMD_Info GETOUTSRCSTREAM_CMD	= { BRDctrl_FOTR_GETOUTSRCSTREAM, 1, 0, 0, NULL};

static int g_flProgGen = 0;
//***************************************************************************************
CFotrSrv::CFotrSrv(int idx, int srv_num, CModule* pModule, PFOTRSRV_CFG pCfg) :
	CService(idx, _BRDC("FOTR"), srv_num, pModule, pCfg, sizeof(FOTRSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_DIRECTION_OUT |
			BRDserv_ATTR_STREAMABLE_OUT |
			BRDserv_ATTR_EXCLUSIVE_ONLY;

	Init(&SETMODE_CMD, (CmdEntry)&CFotrSrv::CtrlMode);
	Init(&GETMODE_CMD, (CmdEntry)&CFotrSrv::CtrlMode);
	Init(&GETCFG_CMD, (CmdEntry)&CFotrSrv::CtrlCfg);
	Init(&MSGFIFORESET_CMD, (CmdEntry)&CFotrSrv::CtrlMsgFifoRst);
	Init(&SETTESTMODE_CMD, (CmdEntry)&CFotrSrv::CtrlTestMode);
	Init(&GETTESTMODE_CMD, (CmdEntry)&CFotrSrv::CtrlTestMode);

	Init(&RESET_CMD, (CmdEntry)&CFotrSrv::CtrlReset);
	Init(&SYNC_CMD, (CmdEntry)&CFotrSrv::CtrlSync);
	Init(&DISCON_CMD, (CmdEntry)&CFotrSrv::CtrlDisconnect);
	Init(&CONNECT_CMD, (CmdEntry)&CFotrSrv::CtrlConnect);
	Init(&CHCKCON_CMD, (CmdEntry)&CFotrSrv::CtrlCheckConnect);
	Init(&SENDMSG_CMD, (CmdEntry)&CFotrSrv::CtrlSendMsg);
	Init(&RECEIVEMSG_CMD, (CmdEntry)&CFotrSrv::CtrlReceiveMsg);
	Init(&WRITERMREG_CMD, (CmdEntry)&CFotrSrv::CtrlWriteRmReg);
	Init(&READRMREG_CMD, (CmdEntry)&CFotrSrv::CtrlReadRmReg);
	Init(&WRITERMREGS_CMD, (CmdEntry)&CFotrSrv::CtrlWriteRmRegs);
	Init(&READRMREGS_CMD, (CmdEntry)&CFotrSrv::CtrlReadRmRegs);

	Init(&SETFIFOADDR_CMD, (CmdEntry)&CFotrSrv::CtrlFifoAddr);
	Init(&GETFIFOADDR_CMD, (CmdEntry)&CFotrSrv::CtrlFifoAddr);

	Init(&INFIFORESET_CMD, (CmdEntry)&CFotrSrv::CtrlFifoReset);
	Init(&INENABLE_CMD, (CmdEntry)&CFotrSrv::CtrlEnable);
	Init(&INFIFOSTATUS_CMD, (CmdEntry)&CFotrSrv::CtrlFifoStatus);
	Init(&GETDATA_CMD, (CmdEntry)&CFotrSrv::CtrlGetData);
	Init(&GETINSRCSTREAM_CMD, (CmdEntry)&CFotrSrv::CtrlGetAddrData);

	Init(&OUTFIFORESET_CMD, (CmdEntry)&CFotrSrv::CtrlFifoReset);
	Init(&OUTENABLE_CMD, (CmdEntry)&CFotrSrv::CtrlEnable);
	Init(&OUTFIFOSTATUS_CMD, (CmdEntry)&CFotrSrv::CtrlFifoStatus);
	Init(&PUTDATA_CMD, (CmdEntry)&CFotrSrv::CtrlPutData);
	Init(&GETOUTSRCSTREAM_CMD, (CmdEntry)&CFotrSrv::CtrlGetAddrData);
}

//***************************************************************************************
int CFotrSrv::CtrlIsAvailable(
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

	//DEVS_CMD_Reg Param;
	//Param.idxMain = m_index;
	//Param.tetr = ADM2IFnt_MAIN;
	//Param.reg = 0x108;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &Param);

	//	2 канала FOTR - это 2 экземпляра тетрад
	// канал 0 - служба FOTR0, канал 1 - служба FOTR1
	ULONG TetrInstantNum = 1;
	BRDCHAR* pBuf = m_name + (BRDC_strlen(m_name) - 2);
	if(BRDC_strchr(pBuf, '1'))
		TetrInstantNum = 2;
	
	m_ModuleType = 0; // 2G
	m_FotrTetrNum = GetTetrNumEx(pModule, m_index, FOTR_TETR_ID, TetrInstantNum);
	if(m_FotrTetrNum == -1)
	{
		m_FotrTetrNum = GetTetrNumEx(pModule, m_index, FOTR3G_TETR_ID, TetrInstantNum);
		if(m_FotrTetrNum != -1)
		{
			m_ModuleType = 1; // 3G

			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_FotrTetrNum;
			RegParam.reg = ADM2IFnr_STATUS;
			pModule->RegCtrl(DEVScmd_REGREADDIR, &RegParam);
			if(!(RegParam.val & 0x100))
				m_FotrTetrNum = -1;
		}
		else
		{ // RIO
			m_FotrTetrNum = GetTetrNum(pModule, m_index, RIO_MSG_TETR_ID);
			m_RioCtrlTetrNum = GetTetrNum(pModule, m_index, RIO_CTRL_TETR_ID);
			if(m_FotrTetrNum != -1 && m_RioCtrlTetrNum != -1)
				m_ModuleType = 2; // RIO
		}
	}
	if(m_ModuleType == 2) // RIO
	{
		m_DataInTetrNum = GetTetrNum(pModule, m_index, RIO_DATA64IN_TETR_ID);
		m_DataOutTetrNum = GetTetrNum(pModule, m_index, RIO_DATA64OUT_TETR_ID);
	}
	else
	{
		m_DataInTetrNum = GetTetrNumEx(pModule, m_index, DATA32IN_TETR_ID, TetrInstantNum);
		if(m_DataInTetrNum == -1)
			m_DataInTetrNum = GetTetrNumEx(pModule, m_index, DATA64IN_TETR_ID, TetrInstantNum);
		m_DataOutTetrNum = GetTetrNumEx(pModule, m_index, DATA32OUT_TETR_ID, TetrInstantNum);
		if(m_DataOutTetrNum == -1)
			m_DataOutTetrNum = GetTetrNumEx(pModule, m_index, DATA64OUT_TETR_ID, TetrInstantNum);
	}
	if(m_MainTetrNum != -1 && m_FotrTetrNum != -1 && m_DataInTetrNum != -1)
	{
		if(m_ModuleType == 0)
		{
			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_FotrTetrNum;
			RegParam.reg = 0x108;
			pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
			if(RegParam.val)
			{
				m_InitTetrNum = RegParam.val;
				RegParam.tetr = m_InitTetrNum;
				RegParam.reg = ADM2IFnr_ID;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				if(RegParam.val == FM401S_TETR_ID)
					m_ModuleType = 3;
				if(RegParam.val == FM402S_TETR_ID)
					m_ModuleType = 4;
			}
		}
		m_isAvailable = 1;
		PFOTRSRV_CFG pSrvCfg = (PFOTRSRV_CFG)m_pConfig;
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;
			DEVS_CMD_Reg RegParam;
			RegParam.idxMain = m_index;
			RegParam.tetr = m_DataInTetrNum;
			if(!pSrvCfg->DataInFifoSize)
			{
				RegParam.reg = ADM2IFnr_FTYPE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				int widthFifo = RegParam.val >> 3;
				RegParam.reg = ADM2IFnr_FSIZE;
				pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
				pSrvCfg->DataInFifoSize = RegParam.val * widthFifo;
			}
			RegParam.tetr = m_FotrTetrNum;
			RegParam.reg = ADM2IFnr_MODE0;
			RegParam.val = 0;
			PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&RegParam.val;
			pMode0->ByBits.Reset = 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			for(int i = 1; i < 32; i++)
			{
				RegParam.reg = i;
				RegParam.val = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			}
			RegParam.reg = ADM2IFnr_MODE0;
			pMode0->ByBits.Reset = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

			RegParam.tetr = m_DataInTetrNum;
			RegParam.reg = ADM2IFnr_MODE0;
			RegParam.val = 0;
			pMode0 = (PADM2IF_MODE0)&RegParam.val;
			pMode0->ByBits.Reset = 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			for(int i = 1; i < 32; i++)
			{
				RegParam.reg = i;
				RegParam.val = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			}
			RegParam.reg = ADM2IFnr_MODE0;
			pMode0->ByBits.Reset = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

			if(m_DataOutTetrNum != -1)
			{
				RegParam.tetr = m_DataOutTetrNum;
				if(!pSrvCfg->DataOutFifoSize)
				{
					RegParam.reg = ADM2IFnr_FTYPE;
					pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
					int widthFifo = RegParam.val >> 3;
					RegParam.reg = ADM2IFnr_FSIZE;
					pModule->RegCtrl(DEVScmd_REGREADIND, &RegParam);
					pSrvCfg->DataOutFifoSize = RegParam.val * widthFifo;
				}
				RegParam.reg = ADM2IFnr_MODE0;
				RegParam.val = 0;
				pMode0 = (PADM2IF_MODE0)&RegParam.val;
				pMode0->ByBits.Reset = 1;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
				for(int i = 1; i < 32; i++)
				{
					RegParam.reg = i;
					RegParam.val = 0;
					pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
				}
				RegParam.reg = ADM2IFnr_MODE0;
				pMode0->ByBits.Reset = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
			}

			if(m_ModuleType == 3 || m_ModuleType == 4)
			{
				if(m_ModuleType == 3)
				{
					writeSpdDev(pModule, REGISTER_DEVID, REGISTER_NUMB, 0, 0x0C, 0x00); // TCA62425
					writeSpdDev(pModule, REGISTER_DEVID, REGISTER_NUMB, 0, 0x0D, 0xF0); // TCA62425
					writeSpdDev(pModule, REGISTER_DEVID, REGISTER_NUMB, 0, 0x0E, 0xFF); // TCA62425
					writeSpdDev(pModule, REGISTER_DEVID, REGISTER_NUMB, 0, 0x85, 0x07); // TCA62425

					writeSpdDev(pModule, REGISTER_DEVID, REGISTER_NUMB, 0, 0x84, 0x55); // TCA62425 - flash
				}
				pSrvCfg->dGenFxtal = 0.0;
				if(pSrvCfg->GenType)
				{	
					writeSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen, 0, 135, 0x80); // Reset
					//SpdWrite( pModule, SPDdev_GEN, 135, 0x80 );		
					Sleep(100);
					ULONG	regAdr, regData[20];

					// Считать регистры Si570/Si571
					for( regAdr=7; regAdr<=12; regAdr++ )
						readSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen, 0, regAdr, &regData[regAdr]);
						//SpdRead( pModule, SPDdev_GEN, regAdr, &regData[regAdr] );

					// Рассчитать частоту кварца
					SI571_CalcFxtal( &(pSrvCfg->dGenFxtal), (double)(pSrvCfg->RefGen), (U32*)regData );
					//printf( "After reset Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n", regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );
					//printf( ">> XTAL = %f kHz\n", pSrvCfg->dGenFxtal/1000.0 );
					//printf( ">> GREF = %f kHz\n", ((double)(pSrvCfg->RefGen0))/1000.0 );
				}
			}
			if(m_ModuleType == 1)
			{
				UCHAR sfp_dev;
				ReadSfpEeprom(pModule, 0, 0, sfp_dev, 1000);
				//UCHAR vendor_name[17];
				//for(int i = 0; i < 17; i++)
				//	ReadSfpEeprom(pModule, 0, i+20, vendor_name[i], 1000);
				//UCHAR vendor_part_number[21];
				//for(int i = 0; i < 20; i++)
				//	ReadSfpEeprom(pModule, 0, i+40, vendor_part_number[i], 1000);
				//vendor_part_number[20] = 0;

				WriteSfpEeprom(pModule, 1, 110, 8, 1000);
			}

		}
	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlReset(
						void			*pDev,		// InfoSM or InfoBM
						void			*pServData,	// Specific Service Data
						ULONG			cmd,		// Command Code (from BRD_ctrl())
						void			*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg RegParam;
	RegParam.tetr = m_FotrTetrNum;
	RegParam.reg = ADM2IFnr_MODE0;
	RegParam.val = 0;
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&RegParam.val;
	pMode0->ByBits.Reset = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
	for(int i = 1; i < 32; i++)
	{
		RegParam.reg = i;
		RegParam.val = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
	}
	RegParam.reg = ADM2IFnr_MODE0;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

	RegParam.tetr = m_DataInTetrNum;
	RegParam.reg = ADM2IFnr_MODE0;
	RegParam.val = 0;
	pMode0 = (PADM2IF_MODE0)&RegParam.val;
	pMode0->ByBits.Reset = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
	for(int i = 1; i < 32; i++)
	{
		RegParam.reg = i;
		RegParam.val = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
	}
	RegParam.reg = ADM2IFnr_MODE0;
	pMode0->ByBits.Reset = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);

	if(m_DataOutTetrNum != -1)
	{
		RegParam.tetr = m_DataOutTetrNum;
		RegParam.reg = ADM2IFnr_MODE0;
		RegParam.val = 0;
		pMode0 = (PADM2IF_MODE0)&RegParam.val;
		pMode0->ByBits.Reset = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
		for(int i = 1; i < 32; i++)
		{
			RegParam.reg = i;
			RegParam.val = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
		}
		RegParam.reg = ADM2IFnr_MODE0;
		pMode0->ByBits.Reset = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &RegParam);
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlGetAddrData(
							void			*pDev,		// InfoSM or InfoBM
							void			*pServData,	// Specific Service Data
							ULONG			cmd,		// Command Code (from BRD_ctrl())
							void			*args 		// Command Arguments (from BRD_ctrl())
							)
{
	ULONG AdmNum;
	//TCHAR buf[SERVNAME_SIZE];
	//_tcscpy_s(buf, m_name);
	//PTCHAR pBuf = buf + (strlen(buf) - 2);
	//if(_tcschr(pBuf, '1'))
	//	AdmNum = 1;
	//else
		AdmNum = 0;
//	if(BRDctrl_FOTR_GETINSRCSTREAM == cmd)
		*(ULONG*)args = (AdmNum << 16) | m_DataInTetrNum;
	if(BRDctrl_FOTR_GETOUTSRCSTREAM == cmd)
		if(m_DataOutTetrNum !=  -1)
			*(ULONG*)args = (AdmNum << 16) | m_DataOutTetrNum;
		else
		{
			*(ULONG*)args = 0;
			return BRDerr_INSUFFICIENT_RESOURCES;
		}
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlCfg(
					void		*pDev,		// InfoSM or InfoBM
					void		*pServData,	// Specific Service Data
					ULONG		cmd,		// Command Code (from BRD_ctrl())
					void		*args 		// Command Arguments (from BRD_ctrl())
					)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	PBRD_FotrCfg pCfg = (PBRD_FotrCfg)args;
	PFOTRSRV_CFG pSrvCfg = (PFOTRSRV_CFG)m_pConfig;
	pCfg->RefGen = pSrvCfg->RefGen;
	pCfg->Chan = pSrvCfg->MaxChan;
	pCfg->IoType = pSrvCfg->TrancieverType;
	pCfg->InFifoSize = pSrvCfg->DataInFifoSize;
	pCfg->OutFifoSize = pSrvCfg->DataOutFifoSize;
	return Status;
}

//***************************************************************************************
int CFotrSrv::CtrlMode(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PULONG pMode = (PULONG)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PFOTR_MODE2 pMode2 = (PFOTR_MODE2)&param.val;

	if(BRDctrl_FOTR_SETMODE == cmd)
	{
		pMode2->ByBits.SfpEn = *pMode;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
		*pMode = pMode2->ByBits.SfpEn;

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlTestMode(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PULONG pMode = (PULONG)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PFOTR_MODE1 pMode1 = (PFOTR_MODE1)&param.val;

	if(BRDctrl_FOTR_SETTESTMODE == cmd)
	{
		pMode1->ByBits.TestMode = *pMode;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	else
		*pMode = pMode1->ByBits.TestMode;

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlMsgFifoRst (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	ULONG dir = *(PULONG)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PFOTR_MODE1 pMode1 = (PFOTR_MODE1)&param.val;
	if(BRDfotr_SENDFIFO == dir)
		pMode1->ByBits.FifoOutRst = 0;
	if(BRDfotr_RECEIVEFIFO == dir)
		pMode1->ByBits.FifoInRst = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(100);
	if(BRDfotr_SENDFIFO == dir)
		pMode1->ByBits.FifoOutRst = 1;
	if(BRDfotr_RECEIVEFIFO == dir)
		pMode1->ByBits.FifoInRst = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::ReadSfpEeprom(CModule* pModule, ULONG idrom, ULONG addr, UCHAR& data, ULONG timeout)
{
	int i = 0;
	int timeoutcnt = timeout / 100;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;
	param.reg = FOTRnr_SFPCTRL;
	PFOTR_SFPROMCTRL pSfpRomCtrl = (PFOTR_SFPROMCTRL)&param.val;
	for(i = 0; i < timeoutcnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pSfpRomCtrl->ByBits.Ready)
			break;
		Sleep(100);
	}
	//if(i == timeoutcnt)
	//	return BRDerr_FOTR_WAIT_SFPEEPROM_READY;

	param.reg = FOTRnr_SFPADDR;
	param.val = addr;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = FOTRnr_SFPCTRL;
	param.val = 0;
	pSfpRomCtrl->ByBits.Addr = idrom;
	pSfpRomCtrl->ByBits.WrCmd = 0;
	pSfpRomCtrl->ByBits.RdCmd = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	for(i = 0; i < timeoutcnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pSfpRomCtrl->ByBits.Ready)
			break;
		Sleep(100);
	}
//	if(i == timeoutcnt)
//		return BRDerr_FOTR_WAIT_SFPEEPROM_READY;

	param.reg = FOTRnr_SFPDATA;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	data = param.val;

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::WriteSfpEeprom(CModule* pModule, ULONG idrom, ULONG addr, UCHAR data, ULONG timeout)
{
	int i = 0;
	int timeoutcnt = timeout / 100;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;
	param.reg = FOTRnr_SFPCTRL;
	PFOTR_SFPROMCTRL pSfpRomCtrl = (PFOTR_SFPROMCTRL)&param.val;
	for(i = 0; i < timeoutcnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(pSfpRomCtrl->ByBits.Ready)
			break;
		Sleep(100);
	}
	//if(i == timeoutcnt)
	//	return BRDerr_FOTR_WAIT_SFPEEPROM_READY;

	param.reg = FOTRnr_SFPADDR;
	param.val = addr;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = FOTRnr_SFPDATA;
	param.val = data;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = FOTRnr_SFPCTRL;
	param.val = 0;
	pSfpRomCtrl->ByBits.Addr = idrom;
	pSfpRomCtrl->ByBits.WrCmd = 1;
	pSfpRomCtrl->ByBits.RdCmd = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::writeSpdDev(CModule* pModule, ULONG dev, ULONG num, ULONG synchr, ULONG reg, ULONG val)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_InitTetrNum;

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// выбираем устройство
	param.reg = ADM2IFnr_SPDDEVICE; // 0x203
	param.val = dev;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем адрес
	param.reg = ADM2IFnr_SPDADDR; // 0x205
	param.val = reg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем данные
	param.reg = ADM2IFnr_SPDDATAL; // 0x206  
	param.val = val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// посылаем команду записи
	param.val = (synchr << 12) | (num << 4) | 0x2; // write
	param.reg = ADM2IFnr_SPDCTRL; // 0x204
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
int CFotrSrv::readSpdDev(CModule* pModule, ULONG dev, ULONG num, ULONG synchr, ULONG reg, ULONG* pVal)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_InitTetrNum;

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// выбираем устройство
	param.reg = ADM2IFnr_SPDDEVICE; // 0x203
	param.val = dev;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем адрес
	param.reg = ADM2IFnr_SPDADDR; // 0x205
	param.val = reg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// посылаем команду чтения
	param.val = (synchr << 12) | (num << 4) | 0x1; // read
	param.reg = ADM2IFnr_SPDCTRL; // 0x204
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// считываем данные
	param.reg = ADM2IFnr_SPDDATAL; // 0x206;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	*pVal = param.val;

	return BRDerr_OK;
}

//***************************************************************************************
//int	CFotrSrv::Si571SetClkVal( CModule* pModule, double *pRate )
int	CFotrSrv::Si571SetClkVal( CModule* pModule)
{
	PFOTRSRV_CFG pSrvCfg = (PFOTRSRV_CFG)m_pConfig;
	ULONG			regData[20];
	int			regAdr;
	int			status = BRDerr_OK;

	double Rate = pSrvCfg->FreqGen;
	// Скорректировать частоту, если необходимо
	if( Rate > (double)pSrvCfg->RefMaxGen )
	{
		Rate = (double)pSrvCfg->RefMaxGen;
		status = BRDerr_WARN;
	}

    SI571_SetRate( &Rate, pSrvCfg->dGenFxtal, (U32*)regData );
	//if(ret < 0)
	//	printf( "Error SI571_SetRate\n");

	if(g_flProgGen)
		return status;	

	g_flProgGen = 1;
	// Запрограммировать генератор
	writeSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen, 0, 137, 0x10); // Freeze DCO
	for( regAdr=7; regAdr<=18; regAdr++ )
		writeSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen, 0, regAdr, regData[regAdr]);
	writeSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen, 0, 137, 0x0); // Unfreeze DCO
	writeSpdDev(pModule, GENERATOR_DEVID, pSrvCfg->AdrGen, 0, 135, 0x40 ); // Assert the NewFreq bit
	//printf( "Write Si571 regs 7-12: %x, %x, %x, %x, %x, %x\n", regData[7], regData[8], regData[9], regData[10], regData[11], regData[12] );
	//printf( ">> XTAL = %f kHz\n", pSrvCfg->dGenFxtal/1000.0 );
	//printf( ">> Rate = %f kHz\n", *pRate/1000.0 );

	return status;	
}

//***************************************************************************************
int CFotrSrv::Sync401S(CModule* pModule, ULONG timeoutcnt)
{
	ULONG i = 0;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_InitTetrNum;
	param.reg = 0x17; //CONTROL
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = 0x16; //LC_FLAG
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

//	double Rate = 62500000.;
//	Si571SetClkVal( pModule,  &Rate);
	Si571SetClkVal( pModule);

	CtrlReset(pModule, 0,0,0);
//	i = 4;
//	CtrlFifoAddr(pModule, 0,0,&i);

	param.reg = 0x17; //CONTROL
	param.val = 3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	
	param.tetr = m_FotrTetrNum;
	param.reg = ADM2IFnr_STATUS;
	PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
	for(i = 0; i < timeoutcnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.RmClkDone)
			break;
		Sleep(100);
	}
	if(i == timeoutcnt)
	{
		g_flProgGen = 0;	
		return BRDerr_FOTR_WAIT_REMOTE_CLK_DONE;
	}
	param.reg = ADM2IFnr_MODE2;
	param.val = 0x3F1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_STATUS;
//	PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
	for(i = 0; i < timeoutcnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.RmConnect)
			break;
		Sleep(100);
	}
	if(i == timeoutcnt)
	{
		g_flProgGen = 0;
		return BRDerr_FOTR_WAIT_REMOTE_CONNECT;
	}

	param.reg = ADM2IFnr_MODE1;
	param.val = 0x3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_MODE3;
	param.val = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// ждем появления флага готовности удаленного устройства
	param.reg = FOTRnr_RMFLAGL;
	for(i = 0; i < timeoutcnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(param.val & 1)
			break;
		Sleep(100);
	}
	if(i == timeoutcnt)
	{
		g_flProgGen = 0;
		return BRDerr_FOTR_WAIT_REMOTE_READY;
	}

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::Sync3G(CModule* pModule, ULONG timeoutcnt)
{
	ULONG i = 0;

	FOTR_MODE2 Mode2;
	Mode2.AsWhole = 0;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;

	// выключить передатчик
	param.val = 0;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADM2IFnr_MODE2;
	param.val = Mode2.AsWhole;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_MODE2;
	// включить передатчик 
	Mode2.ByBits.SfpEn = 1;
	// снять сбросы на Physical Layer
	Mode2.ByBits.PhysInRst = 1;
	Mode2.ByBits.PhysOutRst = 1;
	// снять сбросы на Link Layer
	Mode2.ByBits.LinkInRst = 1;
	Mode2.ByBits.LinkOutRst = 1;
	// снять сбросы на Transport Layer
	Mode2.ByBits.TransInRst = 1;
	Mode2.ByBits.TransOutRst = 1;
	param.val = Mode2.AsWhole;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// установить флаг готовности
	param.reg = ADM2IFnr_MODE3;
	param.val = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// ждем появления флага готовности удаленного устройства
	param.reg = FOTRnr_RMFLAGL;
	for(i = 0; i < timeoutcnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(param.val & 1)
			break;
		Sleep(100);
	}

	// снять сбросы FIFO
	param.reg = ADM2IFnr_MODE1;
	param.val = 3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	if(i == timeoutcnt)
		return BRDerr_FOTR_WAIT_REMOTE_READY;

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::InitRio(CModule* pModule, ULONG timeoutcnt)
{
	ULONG i = 0;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_RioCtrlTetrNum;

	param.reg = ADM2IFnr_MODE1;
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = 2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_STATUS;
	for(i = 0; i < timeoutcnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(param.val && 0x200)
			break;
		Sleep(100);
	}
	if(i == timeoutcnt)
		return BRDerr_FOTR_WAIT_REMOTE_READY;

	param.reg = ADM2IFnr_MODE1;
	param.val = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_STATUS;
	for(i = 0; i < timeoutcnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(param.val && 0x3000)
			break;
		Sleep(100);
	}
	if(i == timeoutcnt)
		return BRDerr_FOTR_WAIT_LOCAL_DCM_LOCKED;

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlConnect(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
//	ULONG i = 0;

	FOTR_MODE2 Mode2;
	Mode2.AsWhole = 0;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;

	// выключить передатчик
	param.val = 0;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADM2IFnr_MODE2;
	param.val = Mode2.AsWhole;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_MODE2;
	// включить передатчик 
	Mode2.ByBits.SfpEn = 1;
	// снять сбросы на Physical Layer
	Mode2.ByBits.PhysInRst = 1;
	Mode2.ByBits.PhysOutRst = 1;
	// снять сбросы на Link Layer
	Mode2.ByBits.LinkInRst = 1;
	Mode2.ByBits.LinkOutRst = 1;
	// снять сбросы на Transport Layer
	Mode2.ByBits.TransInRst = 1;
	Mode2.ByBits.TransOutRst = 1;
	param.val = Mode2.AsWhole;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// установить флаг готовности
	param.reg = ADM2IFnr_MODE3;
	param.val = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	//// ждем появления флага готовности удаленного устройства
	//param.reg = FOTRnr_RMFLAGL;
	//for(i = 0; i < timeoutcnt; i++)
	//{
	//	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//	if(param.val & 1)
	//		break;
	//	Sleep(100);
	//}

	// снять сбросы FIFO
	//param.reg = ADM2IFnr_MODE1;
	//param.val = 3;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

//	if(i == timeoutcnt)
//		return BRDerr_FOTR_WAIT_REMOTE_READY;

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlCheckConnect(
								void		*pDev,		// InfoSM or InfoBM
								void		*pServData,	// Specific Service Data
								ULONG		cmd,		// Command Code (from BRD_ctrl())
								void		*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Sync pSync = (PBRD_Sync)args;

	Sleep(pSync->timeout);

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;

	// проверяем флаг готовности удаленного устройства
	param.reg = FOTRnr_RMFLAGL;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);

	pSync->mode = param.val;

	if(param.val)
	{
		// снять сбросы FIFO
		param.reg = ADM2IFnr_MODE1;
		param.val = 3;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlSync(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Sync pSync = (PBRD_Sync)args;
	ULONG cnt = pSync->timeout / 100;

	if(m_ModuleType == 1)
		return Sync3G(pModule, cnt);

	if(m_ModuleType == 2)
		return InitRio(pModule, cnt);

	if((m_ModuleType == 3) || (m_ModuleType == 4))
		return Sync401S(pModule, cnt);

//	int Status = BRDerr_CMD_UNSUPPORTED;
	ULONG i = 0;
	FOTR_MODE2 Mode2;
	Mode2.AsWhole = 0;

	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;

	//if(pSync->mode == 0)
	//{ // Slave mode
	//	// ждем ВЫКЛЮЧЕНИЯ передатчика на удаленном устройстве
	//	param.reg = ADM2IFnr_STATUS;
	//	PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
	//	for(i = 0; i < cnt; i++)
	//	{
	//		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//		if(!pStatus->ByBits.SpfActive)
	//			break;
	//		Sleep(100);
	//	}
	//	if(i == cnt)
	//		return BRDerr_FOTR_WAIT_REMOTE_TRANSMITTER_OFF;
	//}

	// выключить передатчик
	param.val = 0;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADM2IFnr_MODE2;
	param.val = Mode2.AsWhole;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(500);

//	if(pSync->mode)
//	{ // Master mode
//		// ждем ВЫКЛЮЧЕНИЯ передатчика на удаленном устройстве
//		param.reg = ADM2IFnr_STATUS;
//		PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
////		ULONG cnt = pSync->timeout / 100; // 
//		for(i = 0; i < cnt; i++)
//		{
//			pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
//			if(!pStatus->ByBits.SpfActive)
//				break;
//			Sleep(100);
//		}
//		if(i == cnt)
//			return BRDerr_FOTR_WAIT_REMOTE_TRANSMITTER_OFF;
//	}

	//if(pSync->mode == 0)
	//{ // Slave mode
	//	// ждем ВКЛЮЧЕНИЯ передатчика на удаленном устройстве
	//	param.reg = ADM2IFnr_STATUS;
	//	PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
	//	for(i = 0; i < cnt; i++)
	//	{
	//		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//		if(pStatus->ByBits.SpfActive)
	//			break;
	//		Sleep(100);
	//	}
	//	if(i == cnt)
	//		return BRDerr_FOTR_WAIT_REMOTE_TRANSMITTER_ON;
	//}

	// включить передатчик 
	param.reg = ADM2IFnr_MODE2;
	Mode2.ByBits.SfpEn = 1;
	param.val = Mode2.AsWhole;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(500);

	cnt = pSync->timeout / (100+100); // 
	for(i = 0; i < cnt; i++)
	{
		// сбросить Physical Layer
		param.reg = ADM2IFnr_MODE2;
		Mode2.ByBits.PhysOutRst = 0;
		Mode2.ByBits.PhysInRst = 0;
		param.val = Mode2.AsWhole;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		Sleep(100);
		// снять сбросы на Physical Layer
		Mode2.ByBits.PhysOutRst = 1;
		Mode2.ByBits.PhysInRst = 1;
		param.val = Mode2.AsWhole;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		Sleep(100);
		// проверить появление несущей частоты от удаленного устройства
		param.reg = ADM2IFnr_STATUS;
		PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.RmClkDone)
			break;
	}
	if(i == cnt)
		return BRDerr_FOTR_WAIT_REMOTE_CLK_DONE;

	// снять сбросы на Link Layer
	param.reg = ADM2IFnr_MODE2;
	Mode2.ByBits.LinkOutRst = 1;
	Mode2.ByBits.LinkInRst = 1;
	param.val = Mode2.AsWhole;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(100);

	// ждем появления пустых данных от удаленного устройства
	param.reg = ADM2IFnr_STATUS;
	PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
//	cnt = pSync->timeout / 100; // 
	for(i = 0; i < cnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.RmConnect)
			break;
		Sleep(100);
	}
	if(i == cnt)
		return BRDerr_FOTR_WAIT_REMOTE_CONNECT;

	// снять сбросы на Transport Layer
	param.reg = ADM2IFnr_MODE2;
	Mode2.ByBits.TransOutRst = 1;
	Mode2.ByBits.TransInRst = 1;
	param.val = Mode2.AsWhole;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(1);

//	if(pSync->mode)
//	{ // Master mode
		// установить флаг готовности
		param.reg = ADM2IFnr_MODE3;
		param.val = 1;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		Sleep(100);
//	}
	// ждем появления флага готовности удаленного устройства
	param.reg = FOTRnr_RMFLAGL;
	cnt = pSync->timeout / 100; // 
	for(i = 0; i < cnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		if(param.val & 1)
			break;
		Sleep(100);
	}
	if(i == cnt)
		return BRDerr_FOTR_WAIT_REMOTE_READY;

	// снять сбросы FIFO
	param.reg = ADM2IFnr_MODE1;
	param.val = 3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	Sleep(100);

	//if(pSync->mode == 0)
	//{ // Slave mode
	//	// установить флаг готовности
	//	param.reg = ADM2IFnr_MODE3;
	//	param.val = 1;
	//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//	Sleep(100);
	//}
	// debug
	param.reg = ADM2IFnr_STATUS;
	pStatus = (PFOTR_STATUS)&param.val;
	cnt = 0;
	for(i = 0; i < 10; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.FifoInRdy)
			cnt++;
	}
	if(i == cnt)
	{
		param.reg = FOTRnr_FICNT;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		param.val &= 0xFFFF;
		if(param.val > 128)
			return BRDerr_FOTR_SIZE_RECEIVE_TOOBIG;

		ULONG pData[128];
		param.reg = ADM2IFnr_DATA;
		param.pBuf = pData;
		param.bytes = param.val << 2; // в FICNT размер в 32-разрядных словах, а надо в байтах
		pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlDisconnect(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
//	ULONG data = *(PULONG)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;
	// выключить передатчик
	param.val = 0;
	param.reg = ADM2IFnr_MODE1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADM2IFnr_MODE2;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = ADM2IFnr_MODE3;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlSendMsg(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_MsgDataBuf pMsg = (PBRD_MsgDataBuf)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;

	//param.reg = ADM2IFnr_MODE1;
	//param.val = 7;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	//param.reg = FOTRnr_EXTADRL;
	//param.val = 1;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	// ждем готовности выходного FIFO к записи пакетов
	param.reg = ADM2IFnr_STATUS;
	PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
	ULONG cnt = pMsg->timeout / 100; // 
	ULONG i;
	for(i = 0; i < cnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.FifoOutRdy)
			break;
		Sleep(100);
	}
	if(i == cnt)
		return BRDerr_FOTR_WAIT_SEND_READY;

	param.reg = FOTRnr_FOADRL;
	param.val = pMsg->addr & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = FOTRnr_FOADRH;
	param.val = pMsg->addr >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = FOTRnr_FOCNT;
	param.val = pMsg->size;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_DATA;
	param.pBuf = pMsg->pData;
	param.bytes = pMsg->size << 2; // в FOCNT размер в 32-разрядных словах, а надо в байтах
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);

	//param.reg = ADM2IFnr_STATUS;
	//pStatus = (PFOTR_STATUS)&param.val;
	//pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//
	//
	//param.reg = ADM2IFnr_DATA;
	//param.pBuf = pMsg->pData;
	//param.bytes = pMsg->size << 1; // в FOCNT размер в 32-разрядных словах, а надо в байтах
	//pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);

	//param.reg = ADM2IFnr_STATUS;
	//pStatus = (PFOTR_STATUS)&param.val;
	//pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	
	//PUCHAR pdata = (PUCHAR)pMsg->pData;
	//for(i = 0; i < pMsg->size; i++)
	//{
	//	param.val = pdata[i];
	//	pModule->RegCtrl(DEVScmd_REGWRITEDIR, &param);
	//}
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlReceiveMsg(
								void		*pDev,		// InfoSM or InfoBM
								void		*pServData,	// Specific Service Data
								ULONG		cmd,		// Command Code (from BRD_ctrl())
								void		*args 		// Command Arguments (from BRD_ctrl())
								)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_MsgDataBuf pMsg = (PBRD_MsgDataBuf)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;

	//param.reg = ADM2IFnr_MODE1;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//param.reg = ADM2IFnr_MODE2;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	//param.reg = ADM2IFnr_MODE3;
	//pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	// ждем флага принятия пакета
	param.reg = ADM2IFnr_STATUS;
	PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
	ULONG cnt = pMsg->timeout / 100; // 
	ULONG i;
	for(i = 0; i < cnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.FifoInRdy)
			break;
		Sleep(100);
	}
	if(i == cnt)
		return BRDerr_FOTR_WAIT_RECEIVE_READY;

	param.reg = FOTRnr_FICNT;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	param.val &= 0xFFFF;
	pMsg->size = param.val;
    if(param.val > 125)
		return BRDerr_FOTR_SIZE_RECEIVE_TOOBIG;

	param.reg = ADM2IFnr_DATA;
	param.pBuf = pMsg->pData;
	param.bytes = pMsg->size << 2; // в FICNT размер в 32-разрядных словах, а надо в байтах
	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);

	//PULONG pdata = (PULONG)pMsg->pData;
	//for(i = 0; i < pMsg->size; i++)
	//{
	//	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//	pdata[i] = param.val;
	//}
	//param.bytes = 4;
	//for(i = 0; i < pMsg->size; i++)
	//{
	//	param.pBuf = pdata;
	//	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);
	//	pdata++;
	//}
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlWriteRmReg(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_RemoteReg pReg = (PBRD_RemoteReg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;
	param.reg = ADM2IFnr_STATUS;
	PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
	ULONG cnt = pReg->timeout / 100; // 
	ULONG i;
	for(i = 0; i < cnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.FifoOutRdy)
			break;
		Sleep(100);
	}
	if(i == cnt)
		return BRDerr_FOTR_WAIT_SEND_READY;

	FOTR_CMD cmd_code;
	cmd_code.AsWhole = 0;
	cmd_code.ByBits.WriteCmd = 1; // команда записи
	cmd_code.ByBits.BusNum = pReg->bus; // шина 0
	cmd_code.ByBits.TypeCmd = 3; // по заданному адресу

	ULONG data_out[6];
	data_out[0] = TRANS_TAG; // сигнатура пакета транзакции
	data_out[1] = cmd_code.AsWhole; // команда
	data_out[2] = 0x01;		// адрес ответа (0x01 - FI, 0x04 - FDI)
	data_out[3] = 0;		// для выравнивания
	data_out[4] = pReg->addr; // адрес 
	data_out[5] = pReg->val; // данные

	param.reg = FOTRnr_FOADRL;
	param.val = pReg->node & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = FOTRnr_FOADRH;
	param.val = pReg->node >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = FOTRnr_FOCNT;
	param.val = 6;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_DATA;
	param.pBuf = data_out;
	param.bytes = 6 * sizeof(ULONG); // в FOCNT размер в 32-разрядных словах, а надо в байтах
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlReadRmReg(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_RemoteReg pReg = (PBRD_RemoteReg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;
	param.reg = ADM2IFnr_STATUS;
	PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
	ULONG cnt = pReg->timeout / 100; // 
	ULONG i;
	for(i = 0; i < cnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.FifoOutRdy)
			break;
		Sleep(100);
	}
	if(i == cnt)
		return BRDerr_FOTR_WAIT_SEND_READY;

	FOTR_CMD cmd_code;
	cmd_code.AsWhole = 0;
	cmd_code.ByBits.ReadCmd = 1; // команда чтения
	cmd_code.ByBits.RetSig = 1; // с возвращением сигнатуры
	cmd_code.ByBits.BusNum = pReg->bus; // шина 0
	cmd_code.ByBits.TypeCmd = 3; // по заданному адресу

	ULONG data_out[5];
	data_out[0] = TRANS_TAG; // сигнатура пакета транзакции
	data_out[1] = cmd_code.AsWhole; // команда
	data_out[2] = 0x01;		// адрес ответа (0x01 - FI, 0x04 - FDI)
	data_out[3] = 0;		// для выравнивания
	data_out[4] = pReg->addr; // адрес

	param.reg = FOTRnr_FOADRL;
	param.val = pReg->node & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = FOTRnr_FOADRH;
	param.val = pReg->node >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = FOTRnr_FOCNT;
	param.val = 5;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_DATA;
//	param.pBuf = data_out;
//	param.bytes = 5 * sizeof(ULONG); // в FOCNT размер в 32-разрядных словах, а надо в байтах
//	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);

	for(i = 0; i < 5; i++)
	{
		param.val = data_out[i];
		pModule->RegCtrl(DEVScmd_REGWRITEDIR, &param);
	}

	param.reg = ADM2IFnr_STATUS;
	pStatus = (PFOTR_STATUS)&param.val;
//	ULONG cnt = pReg->timeout / 100; // 
//	ULONG i;
	for(i = 0; i < 5; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.FifoInRdy)
			break;
	}
	if(i == 5)
	{
		for(i = 0; i < cnt; i++)
		{
			pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
			if(pStatus->ByBits.FifoInRdy)
				break;
			Sleep(1);
		}
		if(i == cnt)
			return BRDerr_FOTR_WAIT_RECEIVE_READY;
	}
	param.reg = FOTRnr_FICNT;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	param.val &= 0xFFFF;
    if(param.val > 2)
		return BRDerr_FOTR_SIZE_RECEIVE_TOOBIG;
//    if(param.val < 2)
//		return BRDerr_FOTR_SIZE_RECEIVE_TOOBIG;
	ULONG size = param.val;

	ULONG data_in[2];

	param.reg = ADM2IFnr_DATA;
//	param.pBuf = data_in;
//	param.bytes = size << 2; // в FICNT размер в 32-разрядных словах, а надо в байтах
//	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);
	for(i = 0; i < size; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		data_in[i] = param.val;
	}

	pReg->val = data_in[1];

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlWriteRmRegs(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_RemoteReg pReg = (PBRD_RemoteReg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;
	param.reg = ADM2IFnr_STATUS;
	PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
	ULONG cnt = pReg->timeout / 100; // 
	ULONG i;
	for(i = 0; i < cnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.FifoOutRdy)
			break;
		Sleep(100);
	}
	if(i == cnt)
		return BRDerr_FOTR_WAIT_SEND_READY;

    if(pReg->size > 60)
		return BRDerr_FOTR_SIZE_SEND_TOOBIG;

	FOTR_CMD cmd_code;
	cmd_code.AsWhole = 0;
	cmd_code.ByBits.WriteCmd = 1; // команда записи
	cmd_code.ByBits.BusNum = pReg->bus; // шина 0
	cmd_code.ByBits.TypeCmd = 3; // по заданному адресу

	ULONG data_out[125];
	data_out[0] = TRANS_TAG; // сигнатура пакета транзакции
	data_out[1] = cmd_code.AsWhole; // команда
	data_out[2] = 0x01;		// адрес ответа (0x01 - FI, 0x04 - FDI)
	data_out[3] = 0;		// для выравнивания
	for( i = 0; i < pReg->size; i++)
	{
		data_out[4+2*i] = pReg->incFlag ? pReg->addr+i : pReg->addr;
		data_out[5+2*i] = pReg->pVal[i];
	}
	ULONG size = (pReg->size << 1) + 4;

	param.reg = FOTRnr_FOADRL;
	param.val = pReg->node & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = FOTRnr_FOADRH;
	param.val = pReg->node >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = FOTRnr_FOCNT;
	param.val = size;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_DATA;
	param.pBuf = data_out;
	param.bytes = size * sizeof(ULONG); // в FOCNT размер в 32-разрядных словах, а надо в байтах
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlReadRmRegs(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	PBRD_RemoteReg pReg = (PBRD_RemoteReg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;
	param.reg = ADM2IFnr_STATUS;
	PFOTR_STATUS pStatus = (PFOTR_STATUS)&param.val;
	ULONG cnt = pReg->timeout / 100; // 
	ULONG i;
	for(i = 0; i < cnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.FifoOutRdy)
			break;
		Sleep(100);
	}
	if(i == cnt)
		return BRDerr_FOTR_WAIT_SEND_READY;

    if(pReg->size > 60)
		return BRDerr_FOTR_SIZE_SEND_TOOBIG;

	FOTR_CMD cmd_code;
	cmd_code.AsWhole = 0;
	cmd_code.ByBits.ReadCmd = 1; // команда чтения
	cmd_code.ByBits.RetSig = 1; // с возвращением сигнатуры
	cmd_code.ByBits.BusNum = pReg->bus; // шина 0
	cmd_code.ByBits.TypeCmd = 3; // по заданному адресу

	ULONG data_out[125];
	data_out[0] = TRANS_TAG; // сигнатура пакета транзакции
	data_out[1] = cmd_code.AsWhole; // команда
	data_out[2] = 0x01;		// адрес ответа (0x01 - FI, 0x04 - FDI)
	data_out[3] = 0;		// для выравнивания
	for(i = 0; i < pReg->size; i++)
	{
		data_out[4+2*i] = pReg->incFlag ? pReg->addr+i : pReg->addr;
		data_out[5+2*i] = 0;//pReg->pVal[i];
	}
	ULONG size = (pReg->size << 1) + 4;

	param.reg = FOTRnr_FOADRL;
	param.val = pReg->node & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = FOTRnr_FOADRH;
	param.val = pReg->node >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = FOTRnr_FOCNT;
	param.val = size;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_DATA;
	param.pBuf = data_out;
	param.bytes = size * sizeof(ULONG); // в FOCNT размер в 32-разрядных словах, а надо в байтах
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);

	param.reg = ADM2IFnr_STATUS;
	pStatus = (PFOTR_STATUS)&param.val;
//	ULONG cnt = pReg->timeout / 100; // 
//	ULONG i;
	for(i = 0; i < cnt; i++)
	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
		if(pStatus->ByBits.FifoInRdy)
			break;
		Sleep(100);
	}
	if(i == cnt)
		return BRDerr_FOTR_WAIT_RECEIVE_READY;

	param.reg = FOTRnr_FICNT;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	param.val &= 0xFFFF;
    if(param.val > 125)
		return BRDerr_FOTR_SIZE_RECEIVE_TOOBIG;
	size = param.val;

	ULONG data_in[125];

	param.reg = ADM2IFnr_DATA;
	param.pBuf = data_in;
	param.bytes = size << 2; // в FICNT размер в 32-разрядных словах, а надо в байтах
	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);
	
	for(i = 1; i < size; i++)
		pReg->pVal[i-1] = data_in[i];

	pReg->size = size - 1;

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlFifoAddr(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	int Status = BRDerr_CMD_UNSUPPORTED;
	CModule* pModule = (CModule*)pDev;
	ULONG addr = *(PULONG)args;
	//PBRD_RemoteReg pReg = (PBRD_RemoteReg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_FotrTetrNum;
	param.reg = FOTRnr_FDOADRL;
	param.val = addr & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.reg = FOTRnr_FDOADRH;
	param.val = addr >> 16;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}	

//***************************************************************************************
int CFotrSrv::CtrlFifoReset (
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DataInTetrNum;
	if(BRDctrl_FOTR_OUTFIFORESET == cmd)
		if(m_DataOutTetrNum != -1)
			param.tetr = m_DataOutTetrNum;
		else
			return BRDerr_INSUFFICIENT_RESOURCES;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pMode0->ByBits.FifoRes = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlEnable (
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DataInTetrNum;
	if(BRDctrl_FOTR_OUTENABLE == cmd)
		if(m_DataOutTetrNum != -1)
			param.tetr = m_DataOutTetrNum;
		else
			return BRDerr_INSUFFICIENT_RESOURCES;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.Start = *(PULONG)args;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlFifoStatus(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DataInTetrNum;
	if(BRDctrl_FOTR_OUTFIFOSTATUS == cmd)
		if(m_DataOutTetrNum != -1)
			param.tetr = m_DataOutTetrNum;
		else
			return BRDerr_INSUFFICIENT_RESOURCES;
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	ULONG data = pStatus->AsWhole;
	*(PULONG)args = data;
	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlGetData(
						void		*pDev,		// InfoSM or InfoBM
						void		*pServData,	// Specific Service Data
						ULONG		cmd,		// Command Code (from BRD_ctrl())
						void		*args 		// Command Arguments (from BRD_ctrl())
						)
{
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DataInTetrNum;
/*	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pMode0->ByBits.FifoRes = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
*/
	//PADM2IF_STATUS pStatus;
	//do
	//{
	//	param.reg = ADM2IFnr_STATUS;
	//	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//	pStatus = (PADM2IF_STATUS)&param.val;
	//} while(pStatus->ByBits.HalfFull);

	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);

	//PULONG buf = (PULONG)pBuf->pData;
	//for(ULONG i = 0; i < pBuf->size / sizeof(ULONG); i++)
	//{
	//	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//	buf[i] = param.val;
	//}
	
	//param.reg = ADM2IFnr_STATUS;
	//pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//pStatus = (PADM2IF_STATUS)&param.val;

	return BRDerr_OK;
}

//***************************************************************************************
int CFotrSrv::CtrlPutData(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	if(m_DataOutTetrNum == -1)
		return BRDerr_INSUFFICIENT_RESOURCES;

	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_DataOutTetrNum;
/*
	param.reg = ADM2IFnr_MODE0;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
	pMode0->ByBits.FifoRes = 1;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	pMode0->ByBits.FifoRes = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
*/
	param.reg = ADM2IFnr_DATA;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	param.pBuf = pBuf->pData;
	param.bytes = pBuf->size;
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);

	//PULONG buf = (PULONG)pBuf->pData;
	//for(ULONG i = 0; i < pBuf->size / sizeof(ULONG); i++)
	//{
	//	param.val = buf[i];
	//	pModule->RegCtrl(DEVScmd_REGWRITEDIR, &param);
	//}
	//
	//param.reg = ADM2IFnr_STATUS;
	//pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	//pStatus = (PADM2IF_STATUS)&param.val;

	return BRDerr_OK;
}

// ***************** End of file FotrSrv.cpp ***********************
