//=********************************************************
//
// TKubMod.H
//
// TECH Driver Declaration for ADP6203PCI Board (theme KubMod).
//
// (C) Instr.Sys. by Ekkore Feb. 2004
//
//=********************************************************

#ifndef	__DSPSRV_H_
#define	__DSPSRV_H_


#include	"brdapi.h"
#include	"brdfunx.h"
#include	"SideDRV.h"
#include	"TechDrv.h"
#include	"Ctrl.h"

//
//======= Macros
//

//
//==== Types
//

//=******************** CDSPService **********************
//=********************************************************
class CDSPService : public CGeneralService
{
public:

	PVOID operator new(size_t Size)   {return HAlloc(Size);};
	VOID operator delete(PVOID pVoid) {if(pVoid) HFree(pVoid); };

	S32		Ctrl_SysIsAvaliable( BRD_Handle handle, void *pContext, U32 cmd, void *pArgs );
	S32		Ctrl_SysCapture( BRD_Handle handle, void *pContext, U32 cmd, void *pArgs );
	S32		Ctrl_SysRelease( BRD_Handle handle, void *pContext, U32 cmd, void *pArgs );

	S32		CallBaseEntry( S32 cmd, void *pArgs );
	S32		CallTechEntry( void *pContext, S32 cmd, void *pArgs );
	
	S32		Reset( U32 nodeId ); 
	S32		Start( U32 nodeId ); 
	S32		Load( U32 nodeId, const char *fileName, int argc, void *argv ); 
	S32		Symbol( char *fileName, char *symbName, U32 *val );
	S32		GetBusType( S32 *pBusType );

	S32		PutMsg( void *hostAdr, U32 size, U32 timeout );
	S32		GetMsg( void *hostAdr, U32 size, U32 timeout );
	S32		ReadRAM( U32 brdAdr, void *hostAdr, U32 itemNum, U32 itemSize );

};
typedef CDSPService *PCDSPService;

#endif	// __TKUBMODSRV_H_

//
// End of File
//


