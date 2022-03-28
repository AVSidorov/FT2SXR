//=********************************************************
//
// SIDEDRV.H
//
// Definition of Classes: 
//   CServiceCmd, CGeneralService, CGeneralCollection,
//   CServiceCollection, CSubDriver.
//
// (C) Instr.Sys. by Ekkore Feb. 2004
//
//=********************************************************

#include	"SideDrv.h"

#define	CURRFILE		"SIDEDRV"


//
//
//=********************************************************
//=******************** class CGeneralService *************
//=********************************************************
//
//

//=************ CGeneralService::CGeneralService **********
//=********************************************************
	CGeneralService::CGeneralService()
{
	m_name[0] = '\0';
	m_attr = 0x0;               
	m_idxMain = 0x0;            
	m_paCmdList = NULL;         
	m_pGenDriver = NULL;        
}
//=************ CGeneralService::~CGeneralService *********
//=********************************************************
	CGeneralService::~CGeneralService()
{
	if( m_paCmdList )
		HFree( m_paCmdList );
}
	
//=**************** CGeneralService::Init *****************
//=********************************************************
S32		CGeneralService::Init( S32 idx, CGeneralDriver *pGenDriver )
{
	m_idxMain = idx;
	m_pGenDriver = pGenDriver;

	return 0;
}

//=**************** CGeneralService::InitCmdList **********
//=********************************************************
S32		CGeneralService::InitCmdList( CServiceCmd *paCmdList, S32 cmdListSize )
{
	int			curListSize = 0;
	int			ii;
	CServiceCmd	*paTmpCmdList;         // Command List

	//
	// Definr Current List Size
	//
	if( m_paCmdList != NULL )
	{
		for( ii=0; m_paCmdList[ii].cmd != 0; ii++ );
		curListSize = ii;
	}

	//
	// Allocate
	//
	paTmpCmdList = (CServiceCmd*)HAlloc( sizeof(CServiceCmd) *  (curListSize+cmdListSize));
	if( curListSize != 0 )
		memcpy( paTmpCmdList, m_paCmdList, sizeof(CServiceCmd) * curListSize );

	//
	// Copy 
	//
	memcpy( paTmpCmdList+curListSize, paCmdList, sizeof(CServiceCmd) * cmdListSize );
	m_paCmdList = paTmpCmdList;

	return 0;
}

//=**************** CGeneralService::Ctrl *****************
//=********************************************************
S32		CGeneralService::Ctrl( BRD_Handle handle, void *pContext, U32 cmd, void *pArgs )
{
	int			ii;
	int			mode = (handle >> 23) & 0x3;

	for( ii=0; m_paCmdList[ii].cmd != 0; ii++ )
	{
		if( m_paCmdList[ii].cmd == cmd )
		{
			if( BRDcapt_SPY == mode )
				if( m_paCmdList[ii].isSpy == 0 )
					return BRDerr_BAD_MODE;
			return (this->*(m_paCmdList[ii].pEntry))( handle, pContext, cmd, pArgs );
		}
	}

	return BRDerr_CMD_UNSUPPORTED;
}

//=**************** CGeneralService::CallDevsEntry ********
//=********************************************************
S32		CGeneralService::CallDevsEntry( void *pContext, U32 cmd, void *pArgs )
{
	return m_pGenDriver->m_pDevsEntry( m_pGenDriver->m_pDev, pContext, cmd, pArgs );
}


//
//
//=********************************************************
//=*************** class CGeneralCollection ***************
//=********************************************************
//
//

//=********* CGeneralCollection::CGeneralCollection *******
//=********************************************************
	CGeneralCollection::CGeneralCollection( S32 itemSize, S32 maxSize, S32 delta )
{
	m_itemSize = itemSize;
	m_size     = 0;
	m_maxSize  = maxSize;
	m_delta    = delta;

	if( m_maxSize < 2 )
		m_maxSize = 2;
	if( m_delta < 2 )
		m_delta = 2;
	
	m_ptr      = HAlloc( m_itemSize*m_maxSize );
}

//=********* CGeneralCollection::~CGeneralCollection ******
//=********************************************************
	CGeneralCollection::~CGeneralCollection(void)
{
	if( m_ptr )
		HFree( m_ptr );
}

//=************** CGeneralCollection::GetItem *************
//=********************************************************
	void*	CGeneralCollection::GetItem( S32 itemNo )
{
	char	*pChar;

	if( itemNo >= m_size )
		return NULL;
	pChar  = (char*)m_ptr;
	pChar += itemNo * m_itemSize;

	return (void*)pChar;
}

//=************* CGeneralCollection::Include **************
//=********************************************************
S32		CGeneralCollection::Include( void *pItem )
{
	if( m_size>=m_maxSize )
	{
		void	*ptr = HAlloc( m_itemSize*(m_maxSize+m_delta) );

		if( ptr==NULL )
			return -1;
		memcpy( ptr, m_ptr, m_itemSize*m_maxSize );
		HFree( m_ptr );
		m_ptr = ptr;
		m_maxSize = m_delta;
	}

	void *pNextItem = ((S08*)m_ptr) + (m_size*m_itemSize);

	memcpy( pNextItem, pItem, m_itemSize );
	m_size++;

	return 0;
}


//
//
//=********************************************************
//=**************** class CServiceCollection **************
//=********************************************************
//
//

//=******** CServiceCollection::CServiceCollection ********
//=********************************************************
	CServiceCollection::CServiceCollection( S32 maxSize, S32 delta )
	:
	CGeneralCollection( (S32)sizeof(CGeneralCollection*), maxSize, delta )
{
}

//=******** CServiceCollection::~CServiceCollection *******
//=********************************************************
	CServiceCollection::~CServiceCollection(void)
{
	CGeneralService		*pServ;
	S32					itemNo;

	for( itemNo=0; itemNo<GetSize(); itemNo++ )
	{
		pServ = GetService(itemNo);
		if( pServ )
			delete pServ;
	}
}

//=******** CServiceCollection::IncludeService ************
//=********************************************************
S32		CServiceCollection::IncludeService( CGeneralService *pServ )
{
	return Include( &pServ );
}

//=******** CServiceCollection::GetServiceList ************
//=********************************************************
S32		CServiceCollection::GetServiceList( SIDE_CMD_GetServList *pGSL )
{
	CGeneralService	*pServ;
	int				itemNo;

	pGSL->itemReal = GetSize();
	if( (S32)pGSL->item < GetSize() )
		return BRDerr_BUFFER_TOO_SMALL;

	//
	// Fill Array
	//
	for( itemNo=0; itemNo<GetSize(); itemNo++ )
	{
		pServ = GetService(itemNo);
		strncpy( pGSL->pSLI[itemNo].name, pServ->m_name, SERVNAME_SIZE );
		pGSL->pSLI[itemNo].attr    = pServ->m_attr;
		pGSL->pSLI[itemNo].idxMain = pServ->m_idxMain;
	}

	return BRDerr_OK;
}

//=********** CServiceCollection::SetServiceList **********
//=********************************************************
S32		CServiceCollection::SetServiceList( SIDE_CMD_SetServList *pSSL )
{
	CGeneralService	*pServ;
	int				itemNo;

	for( itemNo=0; itemNo<GetSize() && itemNo<(S32)pSSL->item; itemNo++ )
	{
		pServ = GetService(itemNo);
		strncpy( pServ->m_name, pSSL->pSLI[itemNo].name, SERVNAME_SIZE );
		pServ->m_attr    = pSSL->pSLI[itemNo].attr;
		pServ->m_idxMain = pSSL->pSLI[itemNo].idxMain;
	}

	return BRDerr_OK;
}

//=************* CServiceCollection::Ctrl *****************
//=********************************************************
S32		CServiceCollection::Ctrl( BRD_Handle handle, void *pContext, U32 cmd, void *pArgs )
{
	int				idxMain  = (handle >> 16) & 0x3F;
	CGeneralService	*pServ;
	int				itemNo;

	for( itemNo=0; itemNo<GetSize(); itemNo++ )
	{
		pServ = GetService(itemNo);
		if( pServ->m_idxMain == idxMain )
			return pServ->Ctrl( handle, pContext, cmd, pArgs );
	}

	return BRDerr_ERROR;
}




//
//
//=********************************************************
//=******************** class CGeneralDriver **************
//=********************************************************
//
//

//=**************** CGeneralDriver::CGeneralDriver ********
//=********************************************************
	CGeneralDriver::CGeneralDriver()
	:
	m_collServ()
{
	m_boardName[0] = '\0';
	m_pDevsEntry = NULL;
	m_pDev = NULL;
	m_hMainMutex = NULL;

	m_errcode = 0;
	m_errcnt = 0;
}

//=**************** CGeneralDriver::~CGeneralDriver *******
//=********************************************************
	CGeneralDriver::~CGeneralDriver()
{
}

//=**************** CGeneralDriver::ErrorPrintf ***********
//=********************************************************
S32	 CGeneralDriver::ErrorPrintf( S32 errorCode, void *ptr, char *title, char *format, ... )
{
	va_list		arglist;

	if( ptr && errorCode<0 )
	{
		m_errcnt++;
		m_errcode = errorCode;
	}

	va_start( arglist, format );		 
	BRDI_printf( 1<<EXTERRLVL(errorCode), title, format, arglist );
	va_end( arglist );

	return errorCode;
}

//=***************** CGeneralDriver::Printf ***************
//=********************************************************
void	CGeneralDriver::Printf( S32 errLevel, char *title, char *format, ... )
{
	va_list		arglist;

	va_start( arglist, format );		 
	BRDI_printf( errLevel, title, format, arglist );
	va_end( arglist );
}

//=********** CGeneralDriver::InitData_FillParams *********
//=********************************************************
S32		CGeneralDriver::InitData_FillParams( TBRD_InitData *paInitData, S32 initDataSize, CFillParam *paFillParam )
{
	S32		isIntoBlock = 0;// Where Driver is Described: 0 - LID-Section, 1 - #begin/#end Block
	int		ii, jj;


	//
	// If Driver is Described in the #begin/#end Block
	//
	ii = 0;
	if( !stricmp( paInitData[ii].key, "#begin" ) )
	{
		ii++;
		isIntoBlock = 1;
	}

	//
	// Search "type" Key word
	//
	for( ; ii<initDataSize; ii++ )
	{
		//
		// Go through #begin/#end block 
		//		
		if( !stricmp( paInitData[ii].key, "#begin" ) )	// Found keyword "#begin"
		{
			int beginCnt = 1;
			for(;;)
			{
				if( ++ii >= initDataSize )
					break;
				if( !stricmp( paInitData[ii].key, "#begin" ) )		// Begin of nested SubSection
					beginCnt++;
				if( !stricmp( paInitData[ii].key, "#end" ) )			// End of this or nested SubSection
					if( --beginCnt == 0 )
						break;
			}
			continue;
		}

		//
		// Keyword "#end" - end of Driver SubSection
		// (Driver was Described in the #begin/#end Block, not in the [LID:##] Section)
		//
		if( isIntoBlock )
		if( !stricmp( paInitData[ii].key, "#end" ) )	 
		{
			break;
		}

		//
		// Compare Key Words
		//
		for( jj=0; paFillParam[jj].type != SIDEfpt_NONE; jj++ )
		{
			if( !stricmp( paInitData[ii].key, paFillParam[jj].key ) )
			{
				switch(paFillParam[jj].type)
				{
					case SIDEfpt_INT:
					{
						long		*ptr = (long*)paFillParam[jj].ptr;
						char		*endptr;

						*ptr = strtol( paInitData[ii].val, &endptr, 0 );

						break;
					}
					case SIDEfpt_DOUBLE:
					{
						double		*ptr = (double*)paFillParam[jj].ptr;
						char		*endptr;

						*ptr = strtod( paInitData[ii].val, &endptr );

						break;
					}
					case SIDEfpt_STRING:
					{
						char		*ptr = (char*)paFillParam[jj].ptr;

						strcpy( ptr, paInitData[ii].val );

						break;
					}
				}
				break;
			} 
		}
	}

	return 0;
}

//=************* CGeneralDriver::IncludeService **********
//=*******************************************************
S32		CGeneralDriver::IncludeService( CGeneralService *pServ )
{
	return m_collServ.IncludeService( pServ );
}

//
//
//=********************************************************
//=******************** class CSideDriver *****************
//=********************************************************
//
//

//=**************** CSideDriver::CSideDriver ****************
//=********************************************************
	CSideDriver::CSideDriver()
{
}

//=**************** CSideDriver::~CSideDriver ***************
//=********************************************************
	CSideDriver::~CSideDriver()
{
}

//=************** CSideDriver::InitData ********************
//=********************************************************
S32		CSideDriver::InitData( void *pSubunitICR, void *pCfg, TBRD_InitData *paInitData, S32 initDataSize )
{
	return 1;
}

//=************** CSideDriver::InitAuto *******************
//=*******************************************************
S32		CSideDriver::InitAuto( void *pSubunitICR, void *pCfg, char *pLin, S32 linSize )
{
	return 1;
}

//=***************** CSideDriver::Entry *******************
//=*******************************************************
S32		CSideDriver::Entry( void *pContext, S32 cmd, void *pArgs )
{

	//
	// Support SIDEcmd_INIT
	//
	if( cmd==SIDEcmd_INIT )
	{
		SIDE_CMD_Init *pInit = (SIDE_CMD_Init*)pArgs;
		strncpy( m_boardName, pInit->boardName, BOARDNAME_SIZE );
		m_pDevsEntry = pInit->pEntry;				
		m_pDev = pInit->pDev;						
		m_hMainMutex = pInit->hMutex;

		return BRDerr_OK;

	}

	//
	// Support SIDEcmd_CLEANUP
	//
	if( cmd==SIDEcmd_CLEANUP )
	{
		//
		// PlaceHolder
		//
		return BRDerr_OK;

	}

	//
	// Support SIDEcmd_GETSERVLIST
	//
	else if( cmd==SIDEcmd_GETSERVLIST )
	{
		SIDE_CMD_GetServList *pGSL = (SIDE_CMD_GetServList*)pArgs;

		return m_collServ.GetServiceList( pGSL );

	}

	//
	// Support SIDEcmd_SETSERVLIST
	//
	else if( cmd==SIDEcmd_SETSERVLIST )
	{
		SIDE_CMD_SetServList *pSSL = (SIDE_CMD_SetServList*)pArgs;

		return m_collServ.SetServiceList( pSSL );

	}

	//
	// Support SIDEcmd_CTRL
	//
	else if( cmd==SIDEcmd_CTRL )
	{
		SIDE_CMD_Ctrl *pCtrl = (SIDE_CMD_Ctrl*)pArgs;

		return m_collServ.Ctrl( pCtrl->handle, pContext, pCtrl->cmd, pCtrl->arg );

	}
	return BRDerr_ERROR;
}

//
//
//=********************************************************
//=******************** class CSubDriver ******************
//=********************************************************
//
//

//=**************** CSubDriver::CSubDriver ****************
//=********************************************************
	CSubDriver::CSubDriver()
	:
	m_collServ()
{
	m_errcode = 0;
	m_errcnt = 0;
	m_boardName[0] = '\0';
	m_pDevsEntry = NULL;
	m_pDev = NULL;
}

//=**************** CSubDriver::~CSubDriver ***************
//=********************************************************
	CSubDriver::~CSubDriver()
{
}

//=**************** CSubDriver::ErrorPrintf ***************
//=********************************************************
S32	 CSubDriver::ErrorPrintf( S32 errorCode, void *ptr, char *title, char *format, ... )
{
	va_list		arglist;

	if( ptr && errorCode<0 )
	{
		m_errcnt++;
		m_errcode = errorCode;
	}

	va_start( arglist, format );		 
	BRDI_printf( 1<<EXTERRLVL(errorCode), title, format, arglist );
	va_end( arglist );

	return errorCode;
}

//=***************** CSubDriver::Printf *******************
//=********************************************************
void	CSubDriver::Printf( S32 errLevel, char *title, char *format, ... )
{
	va_list		arglist;

	va_start( arglist, format );		 
	BRDI_printf( errLevel, title, format, arglist );
	va_end( arglist );
}

//=********** CSubDriver::InitData_FillParams *************
//=********************************************************
S32		CSubDriver::InitData_FillParams( TBRD_InitData *paInitData, S32 initDataSize, CFillParam *paFillParam )
{
	S32		isIntoBlock = 0;// Where Driver is Described: 0 - LID-Section, 1 - #begin/#end Block
	int		ii, jj;


	//
	// If Driver is Described in the #begin/#end Block
	//
	ii = 0;
	if( !stricmp( paInitData[ii].key, "#begin" ) )
	{
		ii++;
		isIntoBlock = 1;
	}

	//
	// Search "type" Key word
	//
	for( ; ii<initDataSize; ii++ )
	{
		//
		// Go through #begin/#end block 
		//		
		if( !stricmp( paInitData[ii].key, "#begin" ) )	// Found keyword "#begin"
		{
			int beginCnt = 1;
			for(;;)
			{
				if( ++ii >= initDataSize )
					break;
				if( !stricmp( paInitData[ii].key, "#begin" ) )		// Begin of nested SubSection
					beginCnt++;
				if( !stricmp( paInitData[ii].key, "#end" ) )			// End of this or nested SubSection
					if( --beginCnt == 0 )
						break;
			}
			continue;
		}

		//
		// Keyword "#end" - end of Driver SubSection
		// (Driver was Described in the #begin/#end Block, not in the [LID:##] Section)
		//
		if( isIntoBlock )
		if( !stricmp( paInitData[ii].key, "#end" ) )	 
		{
			break;
		}

		//
		// Compare Key Words
		//
		for( jj=0; paFillParam[jj].type != SIDEfpt_NONE; jj++ )
		{
			if( !stricmp( paInitData[ii].key, paFillParam[jj].key ) )
			{
				switch(paFillParam[jj].type)
				{
					case SIDEfpt_INT:
					{
						long		*ptr = (long*)paFillParam[jj].ptr;
						char		*endptr;

						*ptr = strtol( paInitData[ii].val, &endptr, 0 );

						break;
					}
					case SIDEfpt_DOUBLE:
					{
						double		*ptr = (double*)paFillParam[jj].ptr;
						char		*endptr;

						*ptr = strtod( paInitData[ii].val, &endptr );

						break;
					}
					case SIDEfpt_STRING:
					{
						char		*ptr = (char*)paFillParam[jj].ptr;

						strcpy( ptr, paInitData[ii].val );

						break;
					}
				}
				break;
			} 
		}
	}

	return 0;
}

//=************** CSubDriver::InitData ********************
//=********************************************************
S32		CSubDriver::InitData( void *pSubunitICR, void *pCfg, TBRD_InitData *paInitData, S32 initDataSize )
{
	return 1;
}

//=************** CSubDriver::InitAuto *******************
//=*******************************************************
S32		CSubDriver::InitAuto( void *pSubunitICR, void *pCfg, char *pLin, S32 linSize )
{
	return 1;
}

//=***************** CSubDriver::Entry *******************
//=*******************************************************
S32		CSubDriver::Entry( void *pContext, S32 cmd, void *pArgs )
{

	//
	// Support SIDEcmd_INIT
	//
	if( cmd==SIDEcmd_INIT )
	{
		SIDE_CMD_Init *pInit = (SIDE_CMD_Init*)pArgs;
		strncpy( m_boardName, pInit->boardName, BOARDNAME_SIZE );
		m_pDevsEntry = pInit->pEntry;				
		m_pDev = pInit->pDev;						
		m_hMainMutex = pInit->hMutex;

		return BRDerr_OK;

	}

	//
	// Support SIDEcmd_CLEANUP
	//
	if( cmd==SIDEcmd_CLEANUP )
	{
		//
		// PlaceHolder
		//
		return BRDerr_OK;

	}

	//
	// Support SIDEcmd_GETSERVLIST
	//
	else if( cmd==SIDEcmd_GETSERVLIST )
	{
		SIDE_CMD_GetServList *pGSL = (SIDE_CMD_GetServList*)pArgs;

		return m_collServ.GetServiceList( pGSL );

	}

	//
	// Support SIDEcmd_SETSERVLIST
	//
	else if( cmd==SIDEcmd_SETSERVLIST )
	{
		SIDE_CMD_SetServList *pSSL = (SIDE_CMD_SetServList*)pArgs;

		return m_collServ.SetServiceList( pSSL );

	}

	//
	// Support SIDEcmd_CTRL
	//
	else if( cmd==SIDEcmd_CTRL )
	{
		SIDE_CMD_Ctrl *pCtrl = (SIDE_CMD_Ctrl*)pArgs;

		return m_collServ.Ctrl( pCtrl->handle, pContext, pCtrl->cmd, pCtrl->arg );

	}
	return BRDerr_ERROR;
}

//=************* CSubDriver::IncludeService **************
//=*******************************************************
S32		CSubDriver::IncludeService( CGeneralService *pServ )
{
	return m_collServ.IncludeService( pServ );
}

//
// End of File
//

