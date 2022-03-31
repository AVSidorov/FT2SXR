//=********************************************************
//
// TECHDRV.H
//
// Class TECH_Driver Definition.
//
// (C) Instr.Sys. by Ekkore Feb. 2004
//
//=********************************************************

#include	"DSPTechDrv.h"
#include	"dspcmd.h"

//=************* TECH_DSPDriver::Devs_Entry *************
//=*******************************************************
S32		TECH_DSPDriver::Devs_Entry( void *pContext, S32 cmd, void *pParam )
{
	DEVS_CMD_Reg			*pCmdParam = (DEVS_CMD_Reg*)pParam;
	DSP_CMD_AccessReg		*rAccessReg;
	U32						nCmd;

	U32 aTmpBuf[32];
	U32 nBytes;

	//
	// Check cmd
	//
	switch( cmd )
	{
		case DEVScmd_REGREADDIR:		
			nCmd = DSPcmd_REGREADDIR; 
			pCmdParam->val = -1;	
			break;

		case DEVScmd_REGREADSDIR:		
			nCmd = DSPcmd_REGREADSDIR; 
			break;
		
		case DEVScmd_REGREADIND:		
			nCmd = DSPcmd_REGREADIND; 
			pCmdParam->val = -1;	
			break;

		case DEVScmd_REGREADSIND:		
			nCmd = DSPcmd_REGREADSDIR; 
			break;

		case DEVScmd_REGWRITEDIR:		
			nCmd = DSPcmd_REGWRITEDIR;	
			break;

		case DEVScmd_REGWRITESDIR:		
			nCmd = DSPcmd_REGWRITEDIR;	
			break;
		
		case DEVScmd_REGWRITEIND:		
			nCmd = DSPcmd_REGWRITEIND;	
			break;

		case DEVScmd_REGWRITESIND:		
			nCmd = DSPcmd_REGWRITEIND;	
			break;
		
		default:						
			return BRDerr_ERROR;
	}

	//
	// Write Data to DPRAM
	//
	nBytes=sizeof(aTmpBuf);
	aTmpBuf[0]=nCmd;

	rAccessReg=(DSP_CMD_AccessReg*)&aTmpBuf[1];
	rAccessReg->tetr  = pCmdParam->tetr;
	rAccessReg->reg  = pCmdParam->reg;
	rAccessReg->val  = pCmdParam->val;
	rAccessReg->pBuf = pCmdParam->pBuf;
	rAccessReg->bytes = pCmdParam->bytes;

	Entry_PutMsg( NODE0, aTmpBuf, nBytes, 0 );

	Entry_GetMsg( NODE0, aTmpBuf, nBytes, -1 );//FIXME: timeout

	pCmdParam->val  = rAccessReg->val;
	
	return BRDerr_OK;
}

//=************** TECH_DSPDriver::Entry_Symbol **************
//=********************************************************
S32		TECH_DSPDriver::Entry_Symbol( char *fileName, char *symbName, U32 *val )
{
	DEV_CMD_Symbol	args;

	args.fileName = fileName;
	args.symbName = symbName;
	args.val = val;

	return CallBaseEntry(  DEVcmd_SYMBOL, &args );
}

//=************** TECH_DSPDriver::Entry_GetBusType *********
//=********************************************************
S32		TECH_DSPDriver::Entry_GetBusType( S32 *pBusType )

{
	BRD_Info		rInfo;
	DEV_CMD_Getinfo	args;
	S32				ret;

	args.pInfo = &rInfo;

	ret = CallBaseEntry(  DEVcmd_GETINFO, &args );

	*pBusType = rInfo.busType;

	return ret;
}

//=************** TECH_DSPDriver::Entry_Load ************
//=********************************************************
S32		TECH_DSPDriver::Entry_Load( U32 nodeId, const char *fileName, int argc, void *argv )
{
	DEV_CMD_Load	args;

	args.nodeId = nodeId;
	args.fileName = fileName;
	args.argc = argc;
	args.argv = argv;

	return CallBaseEntry(  DEVcmd_LOAD, &args );
}

//=************** TECH_DSPDriver::Entry_Reset ***************
//=********************************************************
S32		TECH_DSPDriver::Entry_Reset( U32 nodeId )
{
	DEV_CMD_Reset	args;

	args.nodeId = nodeId;

	return CallBaseEntry(  DEVcmd_RESET, &args );
}

//=************** TECH_DSPDriver::Entry_Start ***************
//=********************************************************
S32		TECH_DSPDriver::Entry_Start( U32 nodeId )
{
	DEV_CMD_Start	args;

	args.nodeId = nodeId;

	return CallBaseEntry(  DEVcmd_START, &args );
}

//=************* TECH_DSPDriver::GetMsg *************
//=*******************************************************
S32		TECH_DSPDriver::Entry_GetMsg( U32 nodeId, void *hostAdr, U32 size, U32 timeout )
{
	DEV_CMD_GetMsg	args;

	args.hostAdr = hostAdr;
	args.nodeId = nodeId;
	args.size = size;
	args.timeout = timeout;

	return CallBaseEntry(  DEVcmd_GETMSG, &args );
}

//=************* TECH_DSPDriver::GetMsg ****************
//=*****************************************************
S32		TECH_DSPDriver::Entry_PutMsg( U32 nodeId, void *hostAdr, U32 size, U32 timeout )
{
	DEV_CMD_PutMsg	args;

	args.hostAdr = hostAdr;
	args.nodeId = nodeId;
	args.size = size;
	args.timeout = timeout;

	return CallBaseEntry(  DEVcmd_PUTMSG, &args );
}


//=************* TECH_DSPDriver::ReadRAM ***************
//=*****************************************************
S32		TECH_DSPDriver::Entry_ReadRAM( U32 nodeId, U32 brdAdr, void *hostAdr, U32 itemNum, U32 itemSize )
{
	//typedef struct { U32 nodeId; U32 brdAdr; void *hostAdr; U32 itemNum; U32 itemSize; } DEV_CMD_ReadRAM, *PDEV_CMD_ReadRAM;
	DEV_CMD_ReadRAM	args;

	args.brdAdr = brdAdr;
	args.hostAdr = hostAdr;
	args.nodeId = nodeId;
	args.itemSize = itemSize;
	args.itemNum = itemNum;

	return CallBaseEntry(  DEVcmd_READRAM, &args );
}


//
// End of File
//

