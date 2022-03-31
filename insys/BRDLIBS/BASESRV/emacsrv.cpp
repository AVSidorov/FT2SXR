/*
 ***************** File emacsrv.cpp ***********************
 *
 * BRD Driver for EMAC (Ethernet) service
 *
 * (C) InSys by Dorokhin A. Jan 2016
 *
 *********************************************************
*/

#include "module.h"
#include "emacsrv.h"

#define	CURRFILE _BRDC("EMACSRV")

static CMD_Info WRITEREG_CMD		= { BRDctrl_EMAC_WRITEREG,		0, 0, 0, NULL};
static CMD_Info READREG_CMD			= { BRDctrl_EMAC_READREG,		1, 0, 0, NULL};
static CMD_Info WRITEMDIO_CMD		= { BRDctrl_EMAC_WRITEMDIO,		0, 0, 0, NULL};
static CMD_Info READMDIO_CMD		= { BRDctrl_EMAC_READMDIO,		1, 0, 0, NULL};
static CMD_Info TRANSMIT_CMD		= { BRDctrl_EMAC_TRANSMIT,		0, 0, 0, NULL};
static CMD_Info RECEIVE_CMD			= { BRDctrl_EMAC_RECEIVE,		0, 0, 0, NULL};
static CMD_Info ISANEGCOMPLETE_CMD	= { BRDctrl_EMAC_ISANEGCOMPLETE,1, 0, 0, NULL};
									
//=***************************************************************************************
CEmacSrv::CEmacSrv(int idx, int srv_num, CModule* pModule, PEMACSRV_CFG pCfg) :
	CService(idx, _BRDC("EMACSRV"), srv_num, pModule, pCfg, sizeof(EMACSRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_DIRECTION_OUT|
			BRDserv_ATTR_EXCLUSIVE_ONLY;

	Init(&WRITEREG_CMD, (CmdEntry)&CEmacSrv::CtrlWriteReg);
	Init(&READREG_CMD, (CmdEntry)&CEmacSrv::CtrlReadReg);
	Init(&WRITEMDIO_CMD, (CmdEntry)&CEmacSrv::CtrlWriteMdio);
	Init(&READMDIO_CMD, (CmdEntry)&CEmacSrv::CtrlReadMdio);

	Init(&TRANSMIT_CMD, (CmdEntry)&CEmacSrv::CtrlTransmit);
	Init(&RECEIVE_CMD, (CmdEntry)&CEmacSrv::CtrlReceive);
	Init(&ISANEGCOMPLETE_CMD, (CmdEntry)&CEmacSrv::CtrlIsComplete);
}

//=**************************************************************************************
int CEmacSrv::CtrlIsAvailable(
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
	
	//ULONG TetrInstantNum = 1;
	//BRDCHAR* pBuf = m_name + (BRDC_strlen(m_name) - 2);
	//if(BRDC_strchr(pBuf, '1'))
	//	TetrInstantNum = 2;
	//GetEmacTetrNumEx(pModule, TetrInstantNum);
	
	m_EmacTetrNum = GetTetrNum(pModule, m_index, EMAC_TETR_ID);

	m_isAvailable = 0;

	if(m_MainTetrNum != -1 && m_EmacTetrNum != -1)
		m_isAvailable = 1;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//=***************************************************************************************
int CEmacSrv::CtrlCapture(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	if(m_isAvailable)
	{
		PEMACSRV_CFG pSrvCfg = (PEMACSRV_CFG)m_pConfig;
		if(!pSrvCfg->isAlreadyInit)
		{
			pSrvCfg->isAlreadyInit = 1;

			DEVS_CMD_Reg param;
			param.idxMain = m_index;
			param.tetr = m_EmacTetrNum;
			param.reg = 0x209;
			pModule->RegCtrl(DEVScmd_REGREADIND, &param);
			//printf( "\nEmac init sig=0x%.4X\n", param.val );

			param.reg = 0;
			param.val = 0;
			PADM2IF_MODE0 pMode0 = (PADM2IF_MODE0)&param.val;
			pMode0->ByBits.Reset = 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			for(int i = 1; i < 32; i++)
			{
				param.reg = i;
				param.val = 0;
				pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			}
			param.reg = 0;
			pMode0->ByBits.Reset = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			//printf( "Reset done\n");

			U32 val = 0;

			//readRegDev(pModule, 0x200, &val);
			//printf( "emac200 = 0x%08X\n", val );
			//readRegDev(pModule, 0x240, &val);
			//printf( "emac240 = 0x%08X\n", val );
			//readRegDev(pModule, 0x280, &val);
			//printf( "emac280 = 0x%08X\n", val );
			//readRegDev(pModule, 0x300, &val);
			//printf( "emac300 = 0x%08X\n", val );
			//readRegDev(pModule, 0x320, &val);
			//printf( "emac320 = 0x%08X\n", val );

			//readRegDev(pModule, 0x340, &val);
			//printf( "emac340 = 0x%08X\n", val );

			//readRegDev(pModule, 0x380, &val);
			//printf( "emac380 = 0x%08X\n", val );

			writeRegDev(pModule, 0x340, 0x7F);
			readRegDev(pModule, 0x340, &val); //printf( "emac340 = 0x%08X\n", val );

			writeMDIO(pModule, 0, 0 );
			val = readMDIO(pModule, 0 ); printf( "mdio0 = 0x%.4X\n", val );

			val = readMDIO(pModule, 2 ); printf( "mdio2 = 0x%.4X\n", val ); // PHY ID 1
			val = readMDIO(pModule, 3 ); printf( "mdio3 = 0x%.4X\n", val ); // PHY ID 2

			//val = readMDIO(pModule, 4 ); printf( "mdio4 = 0x%.4X\n", val );
			//val &=~0x100;
			writeMDIO(pModule, 4, 0x1E1  ); // 100/10 Mbps full-duplex/half-duplex capable
			val = readMDIO(pModule, 4 ); printf( "mdio4 = 0x%.4X\n", val );

			writeMDIO(pModule, 0, 0x1100 ); // Auto-Negotiation Enable, Full-duplex
			val = readMDIO(pModule, 0 ); printf( "mdio0 = 0x%.4X\n", val );

			// сброс узла передачи
			param.reg = EMACnr_TXCTRL; //0x10;
			param.val = 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			param.val = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

			// сброс узла приёма
			param.reg = EMACnr_RXCTRL; //0x11;
			param.val = 1;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
			param.val = 0;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

			// разрешение приёма пакетов
			param.val = 0x10;
			pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

			//val = readMDIO(pModule, 0 );printf( "mdio0 = 0x%.4X\n",  val);
			//val = readMDIO(pModule, 1 );printf( "mdio1 = 0x%.4X\n",  val);
			//val = readMDIO(pModule, 4 );printf( "mdio4 = 0x%.4X\n", val);

		}
	}
	return BRDerr_OK;
}

//***************************************************************************************
int CEmacSrv::CtrlIsComplete(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
						   )
{
	CModule* pModule = (CModule*)pDev;

	U32 status = readMDIO(pModule, 1);
	*(ULONG*)args = (status & 0x20) >> 5;

	return BRDerr_OK;
}

//=***************************************************************************************
int CEmacSrv::CtrlWriteReg(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
						   )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_RegVal pRegval = (PBRD_RegVal)args;
	
	U32 adr = pRegval->adr & 0x3FF;
	writeRegDev(pModule, adr, pRegval->val);

	return BRDerr_OK;
}

//=***************************************************************************************
int CEmacSrv::CtrlReadReg(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
						   )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_RegVal pRegval = (PBRD_RegVal)args;
	
	U32 data = 0;
	readRegDev(pModule, pRegval->adr & 0x3FF, &data);
	pRegval->val = data;

	return BRDerr_OK;
}

//=***************************************************************************************
int CEmacSrv::CtrlWriteMdio(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
						   )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_RegVal pRegval = (PBRD_RegVal)args;
	
	writeMDIO(pModule, pRegval->adr, pRegval->val);

	return BRDerr_OK;
}

//=***************************************************************************************
int CEmacSrv::CtrlReadMdio(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
						   )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_RegVal pRegval = (PBRD_RegVal)args;

	U32 data = readMDIO(pModule, pRegval->adr);
	pRegval->val = data;

	return BRDerr_OK;
}

//=***************************************************************************************
int CEmacSrv::CtrlTransmit(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
						   )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_EmacTetrNum;

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
    if(param.val & 0x10) // TX_EMPTY = 1 - FIFO передачи пакетов пустое (можно передавать)
    {
		U32 wsize = pBuf->size >> 2; // размер был в байтах, стал в 32-битных словах
		if( pBuf->size & 0x3 )
			wsize++;
		printf( "\nWR size (in words) = (%d)\n", wsize);
		param.reg = EMACnr_TXCTRL; // 0x10
		param.val = 0;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

		param.reg = ADM2IFnr_DATA;
//		param.pBuf = pBuf->pData;
//		param.bytes = pBuf->size;
//		pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);
	
		PULONG buf = (PULONG)pBuf->pData;
		for(ULONG i = 0; i < wsize; i++)
		{
			param.val = buf[i];
			pModule->RegCtrl(DEVScmd_REGWRITEDIR, &param);
			printf( "TX_buf[%d] = 0x%08X\n",  i, param.val);
		}

		U32 mask=0xF10;
		switch( pBuf->size & 0x3)
		{
			case 1: mask=0x110; break;
			case 2: mask=0x310; break;
			case 3: mask=0x710; break;
		}

		param.reg = EMACnr_TXCTRL; // 0x10
		param.val = mask;
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		//printf( "write mask= %d wsize=%d\n", mask, wsize );  // key=getchar();
	}
	else
		return BRDerr_NOT_READY;

	return BRDerr_OK;
}

//=***************************************************************************************
int CEmacSrv::CtrlReceive(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
						   )
{
	CModule* pModule = (CModule*)pDev;
	PBRD_DataBuf pBuf = (PBRD_DataBuf)args;
	
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_EmacTetrNum;

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
    if(param.val & 0x08) // RX_READY = 1 - пакет в FIFO приема готов к чтению (можно принимать)
    {
		param.reg = EMACnr_RXCNT; //0x208;
		pModule->RegCtrl(DEVScmd_REGREADIND, &param);
		printf( "\nRX CNT =0x%4X (%d)\n", param.val, param.val & 0xfff);

		pBuf->size = param.val & 0xfff;
		U32 wsize = pBuf->size >> 2; // размер был в байтах, стал в 32-битных словах
		if( pBuf->size & 0x3 )
			wsize++;

		param.reg = ADM2IFnr_DATA;
		param.pBuf = pBuf->pData;
		param.bytes = pBuf->size;
//		pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);
	
		PULONG buf = (PULONG)pBuf->pData;
		for(ULONG i = 0; i < wsize; i++)
		{
			pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
			buf[i] = param.val;
			printf( "RX_buf[%d] = 0x%08X\n",  i, param.val);
		}

		param.reg = EMACnr_RXCTRL; // 0x11
		param.val = 0x30; // разрешение приема пакетов и переход к следующему
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
		param.val = 0x10; // разрешение приема пакетов
		pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	}
	else
		return BRDerr_NOT_READY;

	return BRDerr_OK;
}

//=***************************************************************************************
void CEmacSrv::writeMDIO(CModule* pModule, U32 adr, U32 data)
{
	U32 reg = adr & 0x1F;
    reg |=0xA020;
	writeRegDev(pModule, reg, data);
}

//=***************************************************************************************
U32 CEmacSrv::readMDIO(CModule* pModule, U32 adr)
{
	U32 phAdr = 1;
	
    U32 reg = adr & 0x1F;
    reg |= 0xC000;
    reg |= phAdr<<5;

	U32 val = 0;
	readRegDev(pModule, reg, &val);
	return val;
}

//=***************************************************************************************
int CEmacSrv::writeRegDev(CModule* pModule, U32 reg, U32 val)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_EmacTetrNum;

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// выбираем устройство - не используется
	//param.reg = MAIN17nr_SPDDEVICE; // 0x203
	//param.val = dev;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем адрес
	param.reg = EMACnr_SPDADDR; // 0x205
	param.val = reg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем данные
	param.reg = EMACnr_SPDDATAL; // 0x206  
	param.val = val & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	param.reg = EMACnr_SPDDATAH; // 0x207  
	param.val = (val >> 16) & 0xFFFF;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// посылаем команду записи
	//param.val = (synchr << 12) | (num << 4) | 0x2; // write
	param.val = 0x2; // write
	param.reg = EMACnr_SPDCTRL; // 0x204
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
//	param.val = 0;
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	return BRDerr_OK;
}

//=***************************************************************************************
int CEmacSrv::readRegDev(CModule* pModule, U32 reg, U32* pVal)
{
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_EmacTetrNum;

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	PADM2IF_STATUS pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// выбираем устройство
	//param.reg = EMACnr_SPDDEVICE; // 0x203
	//param.val = dev;
	//pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// записываем адрес
	param.reg = EMACnr_SPDADDR; // 0x205
	param.val = reg;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// посылаем команду чтения
	//param.val = (synchr << 12) | (num << 4) | 0x1; // read
	param.val = 0x1; // read
	param.reg = EMACnr_SPDCTRL; // 0x204
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	param.val = 0;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	// ожидаем готовности тетрады
	param.reg = ADM2IFnr_STATUS;
	pStatus = (PADM2IF_STATUS)&param.val;
	do	{
		pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	} while(!pStatus->ByBits.CmdRdy);

	// считываем данные
	param.reg = EMACnr_SPDDATAL; // 0x206;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	U32 val_low = param.val & 0xFFFF;

	param.reg = EMACnr_SPDDATAH; // 0x207;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	U32 val_hi = (param.val & 0xFFFF) << 16;

//	param.val = 0;
//	param.reg = EMACnr_SPDCTRL; // 0x204
//	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);

	*pVal = val_low + val_hi;

	return BRDerr_OK;
}

// ***************** End of file emacsrv.cpp ***********************
