/*
 ***************** File Dsp201Srv.cpp ***********************
 *
 * CTRL-command for BRD Driver for DSP201 service
 *
 * (C) InSys by Dorokhin S. Nov 2010
 *
 ******************************************************
*/

#include "module.h"
#include "dsp201srv.h"
#include "trdx1.h"
#include "extn.h"

static	HANDLE tHandle;			// ISR thread handle
static	HANDLE hProcEvent;		// processor interrupt event handle
//thread_value isrThread(void *arg);
typedef struct _THREAD_PARAM {
	CDsp201Srv* pDspSrv;
	int idx;
} THREAD_PARAM, *PTHREAD_PARAM;
THREAD_PARAM thread_param;

#define	CURRFILE "DSP201SRV"

static CMD_Info READDIR_CMD			= { BRDctrl_REG_READDIR,		1, 0, 0, NULL};
static CMD_Info READSDIR_CMD		= { BRDctrl_REG_READSDIR,		1, 0, 0, NULL};
static CMD_Info READIND_CMD			= { BRDctrl_REG_READIND,		1, 0, 0, NULL};
static CMD_Info READSIND_CMD		= { BRDctrl_REG_READSIND,		1, 0, 0, NULL};
static CMD_Info WRITEDIR_CMD		= { BRDctrl_REG_WRITEDIR,		0, 0, 0, NULL};
static CMD_Info WRITESDIR_CMD		= { BRDctrl_REG_WRITESDIR,		0, 0, 0, NULL};
static CMD_Info WRITEIND_CMD		= { BRDctrl_REG_WRITEIND,		0, 0, 0, NULL};
static CMD_Info WRITESIND_CMD		= { BRDctrl_REG_WRITESIND,		0, 0, 0, NULL};
static CMD_Info SETSTATIRQ_CMD		= { BRDctrl_REG_SETSTATIRQ,		0, 0, 0, NULL};
static CMD_Info CLEARSTATIRQ_CMD	= { BRDctrl_REG_CLEARSTATIRQ,	0, 0, 0, NULL};

static CMD_Info PROCRESET_CMD		= { BRDctrl_DSP_PROCRESET,		1, 0, 0, NULL};
static CMD_Info PROCSTART_CMD		= { BRDctrl_DSP_PROCSTART,		1, 0, 0, NULL};
static CMD_Info PROCLOAD_CMD		= { BRDctrl_DSP_PROCLOAD,		1, 0, 0, NULL};
static CMD_Info GETMSG_CMD			= { BRDctrl_DSP_GETMSG,			1, 0, 0, NULL};
static CMD_Info PUTMSG_CMD			= { BRDctrl_DSP_PUTMSG,			1, 0, 0, NULL};
static CMD_Info PEEKDPRAM_CMD		= { BRDctrl_DSP_PEEKDPRAM,		1, 0, 0, NULL};
static CMD_Info POKEDPRAM_CMD		= { BRDctrl_DSP_POKEDPRAM,		1, 0, 0, NULL};
static CMD_Info READDPRAM_CMD		= { BRDctrl_DSP_READDPRAM,		1, 0, 0, NULL};
static CMD_Info WRITEDPRAM_CMD		= { BRDctrl_DSP_WRITEDPRAM,		1, 0, 0, NULL};
static CMD_Info EXTENSION_CMD		= { BRDctrl_DSP_EXTENSION,		1, 0, 0, NULL};
static CMD_Info SIGNALWAIT_CMD		= { BRDctrl_DSP_SIGNALWAIT,		1, 0, 0, NULL};
static CMD_Info SIGNALSEND_CMD		= { BRDctrl_DSP_SIGNALSEND,		1, 0, 0, NULL};

//***************************************************************************************
CDsp201Srv::CDsp201Srv(int idx, int srv_num, CModule* pModule, PDSP201SRV_CFG pCfg) :
	CService(idx, _BRDC("DSP201"), srv_num, pModule, pCfg, sizeof(DSP201SRV_CFG))
{
	m_attribute = 
			BRDserv_ATTR_DIRECTION_IN |
			BRDserv_ATTR_STREAMABLE_IN |
			BRDserv_ATTR_DIRECTION_OUT |
			BRDserv_ATTR_STREAMABLE_OUT;// |
			//BRDserv_ATTR_EXCLUSIVE_ONLY;

	Init(&READDIR_CMD, (CmdEntry)&CDsp201Srv::CtrlReadDir);
	Init(&READSDIR_CMD, (CmdEntry)&CDsp201Srv::CtrlReadsDir);
	Init(&READIND_CMD, (CmdEntry)&CDsp201Srv::CtrlReadInd);
	Init(&READSIND_CMD, (CmdEntry)&CDsp201Srv::CtrlReadsInd);
	Init(&WRITEDIR_CMD, (CmdEntry)&CDsp201Srv::CtrlWriteDir);
	Init(&WRITESDIR_CMD, (CmdEntry)&CDsp201Srv::CtrlWritesDir);
	Init(&WRITEIND_CMD, (CmdEntry)&CDsp201Srv::CtrlWriteInd);
	Init(&WRITESIND_CMD, (CmdEntry)&CDsp201Srv::CtrlWritesInd);
	Init(&SETSTATIRQ_CMD, (CmdEntry)&CDsp201Srv::CtrlSetStatIrq);
	Init(&CLEARSTATIRQ_CMD, (CmdEntry)&CDsp201Srv::CtrlClearStatIrq);
	
	Init(&PROCRESET_CMD, (CmdEntry)&CDsp201Srv::CtrlProcReset);
	Init(&PROCSTART_CMD, (CmdEntry)&CDsp201Srv::CtrlProcStart);
	Init(&PROCLOAD_CMD, (CmdEntry)&CDsp201Srv::CtrlProcLoad);

	Init(&GETMSG_CMD, (CmdEntry)&CDsp201Srv::CtrlGetMsg);
	Init(&PUTMSG_CMD, (CmdEntry)&CDsp201Srv::CtrlPutMsg);

	Init(&PEEKDPRAM_CMD, (CmdEntry)&CDsp201Srv::CtrlPeekDpram);
	Init(&POKEDPRAM_CMD, (CmdEntry)&CDsp201Srv::CtrlPokeDpram);
	Init(&READDPRAM_CMD, (CmdEntry)&CDsp201Srv::CtrlReadDpram);
	Init(&WRITEDPRAM_CMD, (CmdEntry)&CDsp201Srv::CtrlWriteDpram);
	Init(&EXTENSION_CMD, (CmdEntry)&CDsp201Srv::CtrlExtension);

	Init(&SIGNALWAIT_CMD, (CmdEntry)&CDsp201Srv::CtrlSignalWait);
	Init(&SIGNALSEND_CMD, (CmdEntry)&CDsp201Srv::CtrlSignalSend);

	m_isMsgInit = 0;

}

//***************************************************************************************
void	CDsp201Srv::SetAdr(PULONG adrOper, PULONG adrTetr)
{
	m_BaseAdrOper = adrOper;
	m_BaseAdrTetr = adrTetr;	
}


//***************************************************************************************
int CDsp201Srv::CtrlIsAvailable(
								void		*pDev,		// InfoSM or InfoBM
								void		*pServData,	// Specific Service Data
								ULONG		cmd,		// Command Code (from BRD_ctrl())
								void		*args 		// Command Arguments (from BRD_ctrl())
								)
{
	CModule* pModule = (CModule*)pDev;
	PSERV_CMD_IsAvailable pServAvailable = (PSERV_CMD_IsAvailable)args;
	pServAvailable->attr = m_attribute;

	// check for tetrade MAIN
	m_MainTetrNum = GetTetrNum(pModule, m_index, MAIN_TETR_ID);
	m_MsgTetrNum = GetTetrNum(pModule, m_index, MSG_TETR_ID);
	m_DspTetrNum = GetTetrNum(pModule, m_index, TS201_TETR_ID);

	if( (m_MainTetrNum != -1) || 
		(m_MsgTetrNum != -1)  ||
		(m_DspTetrNum != -1) )
	{
		m_isAvailable = 1;
		PDSP201SRV_CFG pSrvCfg = (PDSP201SRV_CFG)m_pConfig;
		if(!pSrvCfg->isAlreadyInit)
			pSrvCfg->isAlreadyInit = 1;
	}
	else
		m_isAvailable = 0;

	pServAvailable->isAvailable = m_isAvailable;
	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlCapture(
						void			*pDev,		// InfoSM or InfoBM
						void			*pServData,	// Specific Service Data
						ULONG			cmd,		// Command Code (from BRD_ctrl())
						void			*args 		// Command Arguments (from BRD_ctrl())
						)
{
	// read constants of tetrades
	// TS201
	U32 ID		= trd_peekInd(m_BaseAdrTetr, m_DspTetrNum, 0x100);
	U32 ID_MOD	= trd_peekInd(m_BaseAdrTetr, m_DspTetrNum, 0x101);
	U32 VER		= trd_peekInd(m_BaseAdrTetr, m_DspTetrNum, 0x102);
	// MSG
	U32 ID_MSG		= trd_peekInd(m_BaseAdrTetr, m_MsgTetrNum, 0x100);
	U32 ID_MOD_MSG	= trd_peekInd(m_BaseAdrTetr, m_MsgTetrNum, 0x101);
	U32 VER_MSG		= trd_peekInd(m_BaseAdrTetr, m_MsgTetrNum, 0x102);

	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlReadDir(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	pModule->RegCtrl(DEVScmd_REGREADDIR, &param);
	pReg->val = param.val;
	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlReadsDir(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.pBuf = pReg->pBuf;
	param.bytes = pReg->bytes;
	pModule->RegCtrl(DEVScmd_REGREADSDIR, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlReadInd(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	pModule->RegCtrl(DEVScmd_REGREADIND, &param);
	pReg->val = param.val;
	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlReadsInd(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.pBuf = pReg->pBuf;
	param.bytes = pReg->bytes;
	pModule->RegCtrl(DEVScmd_REGREADSIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlWriteDir(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.val = pReg->val;
	pModule->RegCtrl(DEVScmd_REGWRITEDIR, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlWritesDir(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.pBuf = pReg->pBuf;
	param.bytes = pReg->bytes;
	pModule->RegCtrl(DEVScmd_REGWRITESDIR, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlWriteInd(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.val = pReg->val;
	pModule->RegCtrl(DEVScmd_REGWRITEIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlWritesInd(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Reg pReg = (PBRD_Reg)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pReg->tetr;
	param.reg =	pReg->reg;
	param.pBuf = pReg->pBuf;
	param.bytes = pReg->bytes;
	pModule->RegCtrl(DEVScmd_REGWRITESIND, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlSetStatIrq(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	PBRD_Irq pIrq = (PBRD_Irq)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = pIrq->tetr;
	param.reg =	pIrq->irqMask;
	param.val =	pIrq->irqInv;
	param.pBuf = pIrq->hEvent;
	pModule->RegCtrl(DEVScmd_SETSTATIRQ, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlClearStatIrq(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	CModule* pModule = (CModule*)pDev;
	ULONG pIrq = *(PULONG)args;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = *(PULONG)args;
	param.reg =	0;
	param.val =	0;
	param.pBuf = NULL;
	pModule->RegCtrl(DEVScmd_CLEARSTATIRQ, &param);
	return BRDerr_OK;
}

//***************************************************************************************
int CDsp201Srv::CtrlProcReset(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	U32	node = *(U32*)args;
	m_isMsgInit = 0;
	return ProcReset(node);
}

//***************************************************************************************
int CDsp201Srv::CtrlProcStart(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{

/*
	// создаем поток для обработки прерываниний от DSP
	thread_param.pDspSrv = this;
	thread_param.idx = 0;
	unsigned threadID;

	tHandle = (HANDLE)_beginthreadex( NULL, 0, &isrThread, &(thread_param), 0, &threadID );

	//tHandle=(HANDLE)_beginthreadex(NULL,0,isrThread,&(thread_par),0,&threadID); 

	hProcEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	// разрешение прерывания от процессора
	CModule* pModule = (CModule*)pDev;
	DEVS_CMD_Reg param;
	param.idxMain = m_index;
	param.tetr = m_MsgTetrNum;
	param.reg =	0x400;
	//param.reg =	0x1;
	param.val =	0;
	param.pBuf = hProcEvent;
	pModule->RegCtrl(DEVScmd_SETSTATIRQ, &param);

	// разрешаем прерывания от тетрады
	U32 x = trd_peekInd(m_BaseAdrTetr, m_MsgTetrNum, 0);
	trd_pokeInd(m_BaseAdrTetr, m_MsgTetrNum, 0, x|4);

	// устанавливаем маску прерываний от тетрады
	x = trd_peekInd(m_BaseAdrTetr, m_MsgTetrNum, 1);
	trd_pokeInd(m_BaseAdrTetr, m_MsgTetrNum, 1, x|0x400);
	//trd_pokeInd(m_BaseAdrTetr, m_MsgTetrNum, 1, 1);
*/
	U32	node = *(U32*)args;
	return ProcStart(node);
}


//***************************************************************************************

//=********************** GetPathToFile ****************
//=*****************************************************
static	int	GetPathToFile(const char	*fileName, char *path)
{
	int i;
	
	for(i=strlen(fileName);i>0;i--)
	{
		if(fileName[i]=='\\') break;
	}
	if( i==0) return 1;

	for(int j=0;j<i+1;j++)
		path[j] = fileName[j];

	return 0;
}

char	loadPath[256] = ".\\";

int CDsp201Srv::CtrlProcLoad(
							void		*pDev,		// InfoSM or InfoBM
							void		*pServData,	// Specific Service Data
							ULONG		cmd,		// Command Code (from BRD_ctrl())
							void		*args 		// Command Arguments (from BRD_ctrl())
							)
{
	PROC_LOAD_PARAM	*ptr = (PROC_LOAD_PARAM	*)args;
    FILE        *fin;               // Load File handle
    //S32         err;                // Error Code
    //char        symb;

    //
    //=== Open File
    //
    fin = fopen( ptr->filename, "rb");
    if( fin == NULL )
        return DRVERR(BRDerr_BAD_FILE);

	// get PATH to file 
	strcpy(&loadPath[0],".\\");
	GetPathToFile(ptr->filename, &loadPath[0]); 
    fclose( fin );

    //
    //=== Reset Board
    //
    ProcReset(ptr->node);

	return ProcLoad(ptr->node, ptr->filename, ptr->argc, ptr->argv);
	
}

typedef struct MSG_HDR {
	U32	sig;
	U32	type;
	U32	src;
	U32	dst;
	U32	size;
	U32	data[3];
} MSG_HDR;

int	CDsp201Srv::CtrlGetMsg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   )
{

	MSG_PARAM	*msg_param = (MSG_PARAM	*)args;
	U32		node = msg_param->node;
	void	*msg = msg_param->msg;
	U32		size = msg_param->size;
	U32		timeout = msg_param->timeout;

	if(m_isMsgInit==0) {
		MsgInit(node);		
		m_isMsgInit=1;
	}

	// get header
	MSG_HDR	msg_hdr;
	U32	    _size = sizeof(MSG_HDR)/4;
	int ret = GetMsg(node,&msg_hdr,&_size,timeout);
	U32 *ptr = (U32*)msg;
	if(msg_hdr.size<4) {
		for(int i=0;i<(int)msg_hdr.size;i++)
			ptr[i] = msg_hdr.data[i];
		msg_param->size = msg_hdr.size*4;
		return 0;
	}

	U32 tail = msg_hdr.size%4;
	U32 currSize = msg_hdr.size-tail;
    while( currSize>0 ) {
		U32 count = currSize;
		GetMsg(node,ptr,&count,timeout);	
		ptr+=count;
        currSize-=count;
    }
	if(tail!=0) {
		U32 x[4];
		U32 count; 
		GetMsg(node,&x[0],&count,timeout);	
		for(int i=0;i<(int)tail;i++)
			ptr[i] = x[i];
	}

	msg_param->size = msg_hdr.size*4;

	return 0;
}


int	CDsp201Srv::CtrlPutMsg(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   )
{
	MSG_PARAM	*msg_param = (MSG_PARAM	*)args;
	U32		node = msg_param->node;
	void	*msg = msg_param->msg;
	U32		size = msg_param->size;
	U32		timeout = msg_param->timeout;

	if(m_isMsgInit==0) {
		MsgInit(node);		
		m_isMsgInit=1;
	}
	
	// send MSG_HDR
    U32 *ptr = (U32*)msg;
	MSG_HDR	msg_hdr;
	U32	tail  = (size/4)%4;
	U32	_size = (size/4) - tail;
	msg_hdr.sig = 0;
	msg_hdr.type= 0;
	msg_hdr.src = 0;
	msg_hdr.dst = 0;
	msg_hdr.size= (size/4);
	msg_hdr.data[0]= ptr[0];
	msg_hdr.data[1]= ptr[1];
	msg_hdr.data[2]= ptr[2];
	PutMsg(node,&msg_hdr,sizeof(MSG_HDR)/4,timeout);
	if((size/4)<4) return size;

	U32 currSize = _size;
    U32 count=min(120,(int)currSize);
	while( currSize>0 ) {
		PutMsg(node,ptr,count,timeout);
        ptr+=count;
        currSize-=count;
        count=min(120,(int)currSize);
	}
	if( tail!=0 ) PutMsg(node,ptr,4,timeout);

	return size;
}

int CDsp201Srv::CtrlPeekDpram(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   )
{
	PEEK_DPRAM_PARAM	*dpram_param = (PEEK_DPRAM_PARAM	*)args;
	U32		node = dpram_param->node;
	U32		offset = dpram_param->offset;
	U32		*data = dpram_param->data;

	if(m_isMsgInit==0) {
		MsgInit(node);		
		m_isMsgInit=1;
	}

	return PeekDPRAM(node,offset, data);
}

int	CDsp201Srv::CtrlPokeDpram(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   )
{
	POKE_DPRAM_PARAM	*dpram_param = (POKE_DPRAM_PARAM	*)args;
	U32		node = dpram_param->node;
	U32		offset = dpram_param->offset;
	U32		data = dpram_param->data;

	if(m_isMsgInit==0) {
		MsgInit(node);		
		m_isMsgInit=1;
	}

	return PokeDPRAM(node,offset,data);
}

int CDsp201Srv::CtrlReadDpram(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   )
{
	RW_DPRAM_PARAM	*dpram_param = (RW_DPRAM_PARAM	*)args;
	U32		node = dpram_param->node;
	U32		offset = dpram_param->offset;
	U32		*data = dpram_param->data;
	U32		size = dpram_param->size;

	if(m_isMsgInit==0) {
		MsgInit(node);		
		m_isMsgInit=1;
	}

	return ReadDPRAM(node,offset,data,size);
}

int CDsp201Srv::CtrlWriteDpram(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   )
{
	RW_DPRAM_PARAM	*dpram_param = (RW_DPRAM_PARAM	*)args;
	U32		node = dpram_param->node;
	U32		offset = dpram_param->offset;
	U32		*data = dpram_param->data;
	U32		size = dpram_param->size;

	if(m_isMsgInit==0) {
		MsgInit(node);		
		m_isMsgInit=1;
	}

	return WriteDPRAM(node,offset,data,size);
}

int CDsp201Srv::CtrlExtension(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   )
{
    DEV_CMD_Extension   *ptr = (DEV_CMD_Extension*)args;

    switch(ptr->cmd)
    {
	    //=== Allocate nonpageble block of system memory
		case BRDextn_GET_MINPERBYTE:    
        {
            BRDextn_GetMinperbyte  *pGM = (BRDextn_GetMinperbyte*)ptr->arg;

            pGM->val = 4;
            break;
        }
	    default:
            return DRVERR(BRDerr_CMD_UNSUPPORTED);
    }

    return DRVERR(BRDerr_OK);
}

int CDsp201Srv::CtrlSignalWait(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   )
{
    return DRVERR(BRDerr_OK);
}

int CDsp201Srv::CtrlSignalSend(
				void		*pDev,		// InfoSM or InfoBM
				void		*pServData,	// Specific Service Data
				ULONG		cmd,		// Command Code (from BRD_ctrl())
				void		*args 		// Command Arguments (from BRD_ctrl())
			   )
{
    return DRVERR(BRDerr_OK);
}

//thread_value isrThread(void *arg)
//{
//	PTHREAD_PARAM pThreadParam = (PTHREAD_PARAM)arg;
//	CDsp201Srv* pDspSrv = pThreadParam->pDspSrv;


//	if(pDspSrv->m_isMsgInit==0) {
//		pDspSrv->MsgInit(0);
//		pDspSrv->m_isMsgInit=1;
//	}


//	for(;;)
//	{
		
//		U32 x = trd_peekDir(pDspSrv->m_BaseAdrTetr, pDspSrv->m_MsgTetrNum, 0);
//		if( (x&0x400) == 0) continue;
////		if( (x&1) == 0) continue;
		
//		// ожидание события прерывания
//		WaitForSingleObject(hProcEvent,INFINITE);

//		x = 123;
//		U32 y = 321;

//		x = trd_peekDir(pDspSrv->m_BaseAdrTetr, pDspSrv->m_MsgTetrNum, 0);

//		// подтверждение прерывания
//		trd_pokeInd(pDspSrv->m_BaseAdrTetr, pDspSrv->m_MsgTetrNum, 0x21A,0x100);

//		x = trd_peekDir(pDspSrv->m_BaseAdrTetr, pDspSrv->m_MsgTetrNum, 0);

//		//разрешение следующего прерывания
//		U32 z = trd_peekInd(pDspSrv->m_BaseAdrTetr, pDspSrv->m_MsgTetrNum, 0);
//		trd_pokeInd(pDspSrv->m_BaseAdrTetr, pDspSrv->m_MsgTetrNum, 0, z|4);

//	}

//	return 0;
//}

// ***************** End of file Dsp201Srv.cpp ***********************
