//=********************************************************
//
// TECHDRV.H
//
// Class TECH_Driver Declaration.
//
// (C) Instr.Sys. by Ekkore Feb. 2004
//
//=********************************************************

#ifndef	__TECHDRV_H_
#define	__TECHDRV_H_


#include	"brdapi.h"
#include	"brdfunx.h"
#include	"SideDrv.h"


//
//==== Constants
//

#define	SIDELIB_MAX		16

//
//======= Macros
//

//
//==== Types
//

//=**************** TECH_MainSubunitList ******************
//=********************************************************
typedef struct 
{
    S32             type;           // Subunit Type ID
    HMODULE         hLib;           // Dll Handle
    SIDE_API_entry   *pEntry;        // SIDE API Entry Point
    void            *pSub;          // SIDE Driver Extension (InfoSM)
} TECH_MainSubunitList;

//=**************** TECH_MainServiceList ******************
//=********************************************************
typedef enum
{
	TECHsl_BASE,
	TECHsl_TECH,
	TECHsl_DUPLEX,
	TECHsl_SIDE
} TECH_ServiceLocation;

typedef struct 
{
	char			name[SERVNAME_SIZE];	// Service Name with Number
	U32				attr;			// Attributes of Service (Look BRDserv_ATTR_XXX constants)
	TECH_ServiceLocation	servLocation;	// Where Service is kept
    SIDE_API_entry			*pEntry;        // SIDE API Entry Point
    void					*pSub;          // SIDE Driver Extension (InfoSM)
} TECH_MainServiceList;

//=******************** TECH_Driver ***********************
//=********************************************************
class TECH_Driver : public CGeneralDriver
{

public:
	TECH_MainSubunitList	m_mainSubLib[SIDELIB_MAX];	// SIDE Driver Params
	HMODULE					m_hBaseLib;					// Library Handle of BASE Driver 
	DEV_API_entry			*m_pBaseEntry;				// Entry Point of BASE Driver
    void					*m_pBaseDev;				// BASE Driver Extension (InfoBM)
	char					m_baseLibName[MAX_PATH];
	CGeneralCollection		m_mainServColl;				// Keeps TECH_MainServiceList objects
	
	S32			m_errcode; 		// Driver Last Error Number 
	S32			m_errcnt;	 		// Driver Error Counter 

public:
	TECH_Driver();
	virtual ~TECH_Driver();

	PVOID operator new(size_t Size)   {return HAlloc(Size);};
	VOID operator delete(PVOID pVoid) {if(pVoid) HFree(pVoid); };
	
private:
	S32		InitData_SearchType( TBRD_InitData *paInitData, S32 initDataSize, char *libName );
	S32		InitData_SearchBaseunit( TBRD_InitData *paInitData, S32 initDataSize, S32 *pInitDataIdx, char *libName );
	S32		InitData_FillSubLib( TBRD_InitData *paInitData, S32 initDataSize, void *pSubunitICR, void *pCfg );

private:
	S32		InitMainServCollection( void );

public:
	S32		CallBaseEntry( S32 cmd, void *pArgs );

public:
	S32		Dev_InitData( S32 idxThisBrd, char *pBoardName, void *pSubunitICR, void *pCfg, 
							TBRD_InitData *paInitData, S32 initDataSize );
	S32		Dev_InitAuto( S32 idxThisBrd, char *pBoardName, void *pSubunitICR, void *pCfg, 
							char *pLin, S32 linSize, TBRD_SubEnum *pSubEnum, U32 sizeSubEnum );
	S32		Dev_Entry( void *pContext, S32 cmd, void *pArgs );

};


#endif	// __TECHDRV_H_

//
// End of File
//


