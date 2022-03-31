//=********************************************************
//
// TKubMod.CPP
//
// TECH Driver for ADP64Z2PCI Board (theme KubMod).
//
// (C) Instr.Sys. by Ekkore Sep. 2004
//
//=********************************************************

#include	<malloc.h>
#include	<stdio.h>
#include	<conio.h>
#include	<string.h>
#include	<ctype.h>
#include	<dos.h>
#include	<time.h>
#include	<math.h>
#include	<windows.h>
#include	<winioctl.h>

#include	"utypes.h"
#include	"brderr.h"
#include	"brdapi.h"
#include	"brdfunx.h"
#include	"extn_ek.h"
#include	"dbprintf.h"
#include	"DSPSrv.h"
#include	"DSPTechDrv.h"

//
//
//=********************************************************
//=**************** class CDSPService *******************
//=********************************************************
//
//

//=******* CDSPService::Ctrl_SysIsAvaliable *************
//=********************************************************
S32		CDSPService::Ctrl_SysIsAvaliable( BRD_Handle handle, void *pContext, U32 cmd, void *pArgs )
{
	SERV_CMD_IsAvailable		*ptr = (SERV_CMD_IsAvailable*)pArgs;

	ptr->attr = m_attr;
	ptr->isAvailable = 1;

	return BRDerr_OK;
}

//=*********** CDSPService::Ctrl_SysCapture *************
//=********************************************************
S32		CDSPService::Ctrl_SysCapture( BRD_Handle handle, void *pContext, U32 cmd, void *pArgs )
{
	return BRDerr_OK;
}

//=*********** CDSPService::Ctrl_SysRelease *************
//=********************************************************
S32		CDSPService::Ctrl_SysRelease( BRD_Handle handle, void *pContext, U32 cmd, void *pArgs )
{
	return BRDerr_OK;
}

//=************** CDSPService::CallBaseEntry *************
//=********************************************************
S32		CDSPService::CallBaseEntry( S32 cmd, void *pArgs )
{
	TECH_Driver		*pTechDriver = (TECH_Driver*)m_pGenDriver;

	return pTechDriver->CallBaseEntry( cmd, pArgs );
}

//=************** CDSPService::PutMsg *************
//=********************************************************
S32		CDSPService::PutMsg( void *hostAdr, U32 size, U32 timeout )
{
	TECH_DSPDriver		*pTechDriver = (TECH_DSPDriver*)m_pGenDriver;

	return pTechDriver->Entry_PutMsg( NODE0, hostAdr, size, timeout );
}

//=************** CDSPService::ReadRAM *************
//=********************************************************
S32		CDSPService::ReadRAM( U32 brdAdr, void *hostAdr, U32 itemNum, U32 itemSize )
{
	TECH_DSPDriver		*pTechDriver = (TECH_DSPDriver*)m_pGenDriver;

	return pTechDriver->Entry_ReadRAM( NODE0, brdAdr, hostAdr, itemNum, itemSize ); 
}

//=************** CDSPService::GetMsg *************
//=********************************************************
S32		CDSPService::GetMsg( void *hostAdr, U32 size, U32 timeout )
{
	TECH_DSPDriver		*pTechDriver = (TECH_DSPDriver*)m_pGenDriver;

	return pTechDriver->Entry_GetMsg( NODE0, hostAdr, size, timeout );
}

//=************** CDSPService::CallTechEntry *************
//=********************************************************
S32		CDSPService::CallTechEntry( void *pContext, S32 cmd, void *pArgs )
{
	TECH_DSPDriver		*pTechDriver = (TECH_DSPDriver*)m_pGenDriver;

	return pTechDriver->Dev_Entry( pContext, cmd, pArgs );
}

//=************** CDSPService::Reset *************
//=***********************************************
S32		CDSPService::Reset( U32 nodeId )
{
	TECH_DSPDriver		*pTechDriver = (TECH_DSPDriver*)m_pGenDriver;

	return pTechDriver->Entry_Reset( nodeId );
}

//=************** CDSPService::Start *************
//=***********************************************
S32		CDSPService::Start( U32 nodeId )
{
	TECH_DSPDriver		*pTechDriver = (TECH_DSPDriver*)m_pGenDriver;

	return pTechDriver->Entry_Reset( nodeId );
}

//=************** CDSPService::Load **************
//=***********************************************
S32		CDSPService::Load( U32 nodeId, const char *fileName, int argc, void *argv )
{
	TECH_DSPDriver		*pTechDriver = (TECH_DSPDriver*)m_pGenDriver;

	return pTechDriver->Entry_Load( nodeId, fileName, argc, argv );
}

//=************** CDSPService::Symbol ************
//=***********************************************
S32		CDSPService::Symbol( char *fileName, char *symbName, U32 *val )
{
	TECH_DSPDriver		*pTechDriver = (TECH_DSPDriver*)m_pGenDriver;

	return pTechDriver->Entry_Symbol( fileName, symbName, val );
}

//=************** CDSPService::GetBusType************
//=**************************************************
S32		CDSPService::GetBusType( S32 *pBusType )
{
	TECH_DSPDriver		*pTechDriver = (TECH_DSPDriver*)m_pGenDriver;

	return pTechDriver->Entry_GetBusType( pBusType );
}

//
// End of File
//


