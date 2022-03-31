//=********************************************************
//
// TECHDRV.H
//
// Class TECH_DSPDriver Declaration.
//
// (C) Instr.Sys. by Ekkore Feb. 2004
//
//=********************************************************

#ifndef	__TECHDSPDRV_H_
#define	__TECHDSPDRV_H_


#include	"brdapi.h"
#include	"brdfunx.h"
#include	"TechDrv.h"
#include	"SideDrv.h"


//
//==== Constants
//

//
//======= Macros
//

//
//==== Types
//

typedef struct
{
	S32 tetr; S32 reg; U32 val;
	void *pBuf; S32 bytes;
}
DSP_CMD_AccessReg;

//=******************** TECH_DSPDriver ***********************
//=********************************************************
class TECH_DSPDriver : public TECH_Driver
{
public:
	PVOID operator new(size_t Size)   {return HAlloc(Size);};
	VOID operator delete(PVOID pVoid) {if(pVoid) HFree(pVoid); };

	S32		Devs_Entry( void *pContext, S32 cmd, void *pParam );
	
	S32		Entry_Reset( U32 nodeId ); 
	S32		Entry_Start( U32 nodeId ); 
	S32		Entry_Load( U32 nodeId, const char *fileName, int argc, void *argv ); 
	S32		Entry_Symbol( char *fileName, char *symbName, U32 *val );
	S32		Entry_GetBusType( S32 *pBusType );
	
	S32		Entry_PutMsg( U32 nodeId, void *hostAdr, U32 size, U32 timeout );
	S32		Entry_GetMsg( U32 nodeId, void *hostAdr, U32 size, U32 timeout );
	S32		Entry_ReadRAM( U32 nodeId, U32 brdAdr, void *hostAdr, U32 itemSize, U32 itemNum );


};


#endif	// __TECHDSPDRV_H_

//
// End of File
//


