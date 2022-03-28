//=********************************************************
//
// SIDEDRV.H
//
// Declaration of Classes: 
//   CServiceCmd, CGeneralService, CGeneralCollection,
//   CServiceCollection, CSubDriver.
//
// (C) Instr.Sys. by Ekkore Feb. 2004
//
//=********************************************************

#ifndef	__SIDEDRV_H_
#define	__SIDEDRV_H_


#include	"brdapi.h"
#include	"brdfunx.h"
#include	"bserv.h"


//
//==== Constants
//


//
//======= Macros
//

//
//==== Types
//

class CGeneralService;
class CGeneralDriver;

//=******************** CServiceCmd ***********************
//=********************************************************

typedef S32 (CGeneralService::*CCmdEntry)( 
			BRD_Handle	handle,		// Service Index into Main Service List and Capture Mode
			void		*pContext,	// Specific Data to send to TECH Driver (for SIDE driver only)
			U32			cmd,		// Command Code (from BRD_ctrl())
			void		*pArgs 		// Command Arguments (from BRD_ctrl())
		);

typedef struct
{
	U32			cmd;			// Command Code
	U32			isSpy:1,		// 0-Control Cmd, 1-Monitoring Cmd
				isCapture:1,	// 0-don't capture the Service, 1-capture the Service if need
				isRelease:1;	// 0-don't release the Service, 1-release the Service if need
	CCmdEntry	pEntry;			// Command Support Entry Point
} CServiceCmd, *PCServiceCmd;


//=******************** class CGeneralService ****************
//=********************************************************
class CGeneralService
{
public:
	char		m_name[SERVNAME_SIZE];// Service name with Number
	U32			m_attr;               // Attributes of Service
	S32			m_idxMain;            // Index of this Resource into TECH/DUPLEX Main Resource List
	CServiceCmd	*m_paCmdList;         // Command List
	CGeneralDriver	*m_pGenDriver;      // Pointer of SIDE Driver

	CGeneralService();
	virtual ~CGeneralService();
	
	PVOID operator new(size_t Size)   {return HAlloc(Size);};
	VOID operator delete(PVOID pVoid) {if(pVoid) HFree(pVoid); };
	
	S32		Init( S32 idx, CGeneralDriver *pGenDriver );
	S32		InitCmdList( CServiceCmd *paCmdList, S32 cmdListSize );
	S32		Ctrl( BRD_Handle handle, void *pContext, U32 cmd, void *pArgs );
	S32		CallDevsEntry( void *pContext, U32 cmd, void *pArgs );
};
typedef CGeneralService *PCGeneralService;

//=******************** class CGeneralCollection *************
//=********************************************************
class CGeneralCollection
{
	S32		m_itemSize;		// Size of Item (bytes)
	S32		m_size;			// Number of Filled Items
	S32		m_maxSize;		// Total Number of Items
	S32		m_delta;		// Delta
	void	*m_ptr;			// Array Pointer

public:

	CGeneralCollection( S32 itemSize, S32 maxSize=25, S32 delta=10 );
	~CGeneralCollection(void);

	PVOID operator new(size_t Size)   {return HAlloc(Size);};
	VOID operator delete(PVOID pVoid) {if(pVoid) HFree(pVoid); };
	
	
	S32		IsValid( void ) { return (m_ptr) ? 0 : -1; };
	void*	GetArray(void)  { return m_ptr; };
	S32		GetSize(void)   { return m_size; };
	void*	GetItem( S32 itemNo );
	S32		Include( void *pItem );
};
typedef CGeneralCollection *PCGeneralCollection;


//=**************** class CServiceCollection **************
//=********************************************************
class CServiceCollection : public CGeneralCollection
{
public:
	CServiceCollection(S32 maxSize=10, S32 delta=5);
	~CServiceCollection(void);

	PVOID operator new(size_t Size)   {return HAlloc(Size);};
	VOID operator delete(PVOID pVoid) {if(pVoid) HFree(pVoid); };
	
	        S32		IncludeService( CGeneralService *pServ );
	        S32		GetServiceList( SIDE_CMD_GetServList *pGSL );
	virtual S32		SetServiceList( SIDE_CMD_SetServList *pSSL );
	virtual S32		Ctrl( BRD_Handle handle, void *pContext, U32 cmd, void *pArgs );

	CGeneralService	*GetService( S32 itemNo )
	{ return *(CGeneralService**)GetItem( itemNo ); };

};
typedef CServiceCollection *PCServiceCollection;


//=**************** SIDE_FillParam *************************
//=********************************************************
typedef enum
{
	SIDEfpt_NONE,	
	SIDEfpt_INT,	
	SIDEfpt_DOUBLE,	
	SIDEfpt_STRING	
} CFillParamType;

typedef struct 
{
    CFillParamType		 type;      // Parameter Type
	void                 *ptr;		// Parameter Pointer
	char                 key[32];	// Parameter Keyword
} CFillParam;


//=******************** class CGeneralDriver **************
//=********************************************************
class CGeneralDriver
{

public:
	char			m_boardName[BOARDNAME_SIZE];	// Unique Board Name
	DEVS_API_entry	*m_pDevsEntry;					// Entry Point of BASE Driver
	void			*m_pDev;						// InfoBM of TECH Driver
	HANDLE			m_hMainMutex;
	
	S32				m_errcode; 		// Driver Last Error Number 
	S32				m_errcnt;	 	// Driver Error Counter 

	CServiceCollection	m_collServ;				// Base Service List

	//
	// Functions
	//
public:
	CGeneralDriver();
	virtual ~CGeneralDriver();

	PVOID operator new(size_t Size)   {return HAlloc(Size);};
	VOID operator delete(PVOID pVoid) {if(pVoid) HFree(pVoid); };
	
	S32		ErrorPrintf( S32 errorCode, void *ptr, char *title, char *format, ... );
	void	Printf( S32 errLevel, char *title, char *format, ... );

protected:
	S32		InitData_FillParams( TBRD_InitData *paInitData, S32 initDataSize, CFillParam *paFillParam );
	S32		IncludeService( CGeneralService *pServ );
};


//=******************** class CSideDriver *****************
//=********************************************************
class CSideDriver : public CGeneralDriver
{
	//
	// Functions
	//
public:
	CSideDriver();
	virtual ~CSideDriver();

	PVOID operator new(size_t Size)   {return HAlloc(Size);};
	VOID operator delete(PVOID pVoid) {if(pVoid) HFree(pVoid); };
	
public:
	virtual S32		InitData( void *pSubunitICR, void *pCfg, TBRD_InitData *paInitData, S32 initDataSize );						
	virtual S32		InitAuto( void *pSubunitICR, void *pCfg, char *pLin, S32 linSize );						
	        S32		Entry( void *pContext, S32 cmd, void *pArgs );
};


//=******************** class CSubDriver ******************
//=********************************************************
class CSubDriver
{

public:
	char			m_boardName[BOARDNAME_SIZE];	// Unique Board Name
	DEVS_API_entry	*m_pDevsEntry;					// Entry Point of BASE Driver
	void			*m_pDev;						// InfoBM of TECH Driver
	HANDLE			m_hMainMutex;
	
	S32			m_errcode; 		// Driver Last Error Number 
	S32			m_errcnt;	 		// Driver Error Counter 

	CServiceCollection	m_collServ;				// Base Service List

	//
	// Functions
	//
public:
	CSubDriver();
	virtual ~CSubDriver();

	PVOID operator new(size_t Size)   {return HAlloc(Size);};
	VOID operator delete(PVOID pVoid) {if(pVoid) HFree(pVoid); };
	
	S32		ErrorPrintf( S32 errorCode, void *ptr, char *title, char *format, ... );
	void	Printf( S32 errLevel, char *title, char *format, ... );

protected:
	S32		InitData_FillParams( TBRD_InitData *paInitData, S32 initDataSize, CFillParam *paFillParam );
public:
	virtual S32		InitData( void *pSubunitICR, void *pCfg, TBRD_InitData *paInitData, S32 initDataSize );						
	virtual S32		InitAuto( void *pSubunitICR, void *pCfg, char *pLin, S32 linSize );						
	        S32		Entry( void *pContext, S32 cmd, void *pArgs );

	S32		IncludeService( CGeneralService *pServ );
};


#endif	// __SIDEDRV_H_

//
// End of File
//


