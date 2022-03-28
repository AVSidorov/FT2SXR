//=********************************************************
//
// TECHDRV.H
//
// Class TECH_Driver Definition.
//
// (C) Instr.Sys. by Ekkore Feb. 2004
//
//=********************************************************


#include	"TechDrv.h"


//=**************** TECH_Driver::TECH_Driver **************
//=********************************************************
	TECH_Driver::TECH_Driver()
	:
	m_mainServColl( (S32)sizeof(TECH_MainServiceList) )
{
	int			ii;

	//
	// CSubDriver
	//
	m_pDevsEntry = DEVS_entry;


	//
	// Clear Library
	//
	for( ii=0; ii<SIDELIB_MAX; ii++ )
	{
		m_mainSubLib[ii].type = 0;
		m_mainSubLib[ii].pEntry = NULL;
		m_mainSubLib[ii].pSub = NULL;
		m_mainSubLib[ii].hLib = NULL;
	}

	m_errcode = 0;
	m_errcnt = 0;
	m_boardName[0] = '\0';
	m_pBaseEntry = NULL;
	m_baseLibName[0] = '\0';
}

//=**************** TECH_Driver::~TECH_Driver *************
//=********************************************************
	TECH_Driver::~TECH_Driver()
{
}

//=************ TECH_Driver::InitData_SearchType **********
//=********************************************************
S32		TECH_Driver::InitData_SearchType( TBRD_InitData *paInitData, S32 initDataSize, char *libName )
{
	S32		isIntoBlock = 0;// Where Driver is Described: 0 - LID-Section, 1 - #begin/#end Block
	int		ii;


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
		if( !stricmp( paInitData[ii].key, "type" ) )
		{
			strcpy( libName, paInitData[ii].val );
			return 0;
		} 
	}

	return -1;
}

//=********** TECH_Driver::InitData_SearchBaseunit ********
//=********************************************************
S32		TECH_Driver::InitData_SearchBaseunit( TBRD_InitData *paInitData, S32 initDataSize, S32 *pInitDataIdx, char *libName )
{
	S32		isIntoBlock = 0;// Where Driver is Described: 0 - LID-Section, 1 - #begin/#end Block
	int		ii;


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
	// Search "BASEDRIVER" Key word
	//
	for( ; ii<initDataSize; ii++ )
	{
		//
		// Go through #begin/#end block 
		//		
		if( !stricmp( paInitData[ii].key, "#begin" ) )	// Found keyword "#begin"
		{
			if( !stricmp( paInitData[ii].val, "BASEDRIVER" ) )	// Found keyword "BASEDRIVER"
			{
				*pInitDataIdx = ii;
				return InitData_SearchType( paInitData+ii, initDataSize-ii, libName );

			}
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
	}
	return -1;
}

//=********** TECH_Driver::InitData_FillSubLib ************
//=********************************************************
S32		TECH_Driver::InitData_FillSubLib( TBRD_InitData *paInitData, S32 initDataSize, void *pSubunitICR, void *pCfg )
{
	S32		isIntoBlock = 0;// Where Driver is Described: 0 - LID-Section, 1 - #begin/#end Block
	int		ii;
	char	libName[MAX_PATH];

	HMODULE				hLib;
	SIDE_API_initData	*pSIDE_initData;
	SIDE_API_entry		*pEntry;
	void				*pSub;
	S32					err;
	int					idxSubLib = 0;



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
	// Search "SIDEDRIVER" Key word
	//
	for( ; ii<initDataSize; ii++ )
	{
		//
		// Go through #begin/#end block 
		//		
		if( !stricmp( paInitData[ii].key, "#begin" ) )	// Found keyword "#begin"
		{
			if( idxSubLib<SIDELIB_MAX )
			if( !stricmp( paInitData[ii].val, "SIDEDRIVER" ) )	// Found keyword "SIDEDRIVER"
			{
				for(;;)
				{
					//
					// Get Name of Library
					//
					if( 0>InitData_SearchType( paInitData+ii, initDataSize-ii, libName ) )
						break;

					//
					// Open Library
					//
					hLib = LoadLibrary( libName );
					if( hLib <= (HINSTANCE)HINSTANCE_ERROR )
					{
						DWORD	err = GetLastError();
						break;
					}

					//
					// Get Entry Point
					//
//					pSIDE_initData  = (SIDE_API_initData*)GetProcAddress( hLib, "_SIDE_initData@20" );
//					pEntry         = (SIDE_API_entry*)GetProcAddress( hLib, "_SIDE_entry@16" );
					pSIDE_initData  = (SIDE_API_initData*)GetProcAddress( hLib, "SIDE_initData" );
					pEntry         = (SIDE_API_entry*)GetProcAddress( hLib, "SIDE_entry" );
					if( pSIDE_initData == 0 || pEntry == 0 )
					{
						FreeLibrary( hLib );
						break;
					}

					//
					// Parse the rest of the record
					//
					pSub = NULL;
					err = (pSIDE_initData)( &pSub, pSubunitICR, paInitData+ii, initDataSize-ii );
					if( (err<0) || (pSub==NULL) )
					{
						FreeLibrary( hLib );
						break;
					}

					//
					// Fill Device Extension
					//
					m_mainSubLib[idxSubLib].type = err;	
					m_mainSubLib[idxSubLib].pEntry = pEntry;
					m_mainSubLib[idxSubLib].pSub = pSub;	
					m_mainSubLib[idxSubLib].hLib = hLib;
					idxSubLib++;

					break;
				}
			}
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
	}
	return 0;
}

//=********** TECH_Driver::InitMainServCollection *********
//=********************************************************
S32		TECH_Driver::InitMainServCollection( void )
{
	TECH_MainServiceList	sMSL;
	SIDE_CMD_GetServList		sGSL;
	SIDE_CMD_SetServList		sSSL;
	DEV_CMD_ServListItem	*paSLI;
	U32						ii;
	int						iiLib;

	//
	// Get Service List from BASE Driver
	//
	sGSL.item = 0;
	sGSL.pSLI = NULL;
	m_pBaseEntry( m_pBaseDev, DEVcmd_GETSERVLIST, &sGSL );

	sGSL.item = (sGSL.itemReal>0) ? sGSL.itemReal : 1;
	paSLI = (DEV_CMD_ServListItem*)HAlloc( sizeof(TECH_MainServiceList) * sGSL.item );
	sGSL.pSLI = paSLI;
	m_pBaseEntry( m_pBaseDev, DEVcmd_GETSERVLIST, &sGSL );

	//
	// Put BASE Driver Services into Main Service Collection
	//
	for( ii=0; ii<sGSL.itemReal; ii++ )
	{
		strncpy( sMSL.name, paSLI[ii].name, SERVNAME_SIZE );
		sMSL.servLocation = TECHsl_BASE;
		sMSL.attr = paSLI[ii].attr;
		sMSL.pEntry = NULL;
		sMSL.pSub = NULL;
		m_mainServColl.Include( &sMSL );
	}

	//
	// Put TECH Driver Services into Main Service Collection
	//

	/* REFACTORING


	if( (S32)sGSL.item < m_collServ.GetSize() )
	{
		sGSL.item = m_collServ.GetSize();
		HFree( paSLI );
		paSLI = (DEV_CMD_ServListItem*)HAlloc( sizeof(TECH_MainServiceList) * sGSL.item );
		sGSL.pSLI = paSLI;
	}

	*/

	for( ii=0; ii<(U32)m_collServ.GetSize(); ii++ )
	{
		CGeneralService		*pServ = m_collServ.GetService( ii );
		if( pServ==NULL )
			continue;

		strncpy( sMSL.name, pServ->m_name, SERVNAME_SIZE );
		sMSL.servLocation = TECHsl_TECH;
		sMSL.attr = pServ->m_attr;
		sMSL.pEntry = DEVS_entry;
		sMSL.pSub = NULL;
		m_mainServColl.Include( &sMSL );
	}

	//
	// Go through all SIDE Drivers to Get Service Lists
	//
	for( iiLib=0; iiLib<SIDELIB_MAX; iiLib++ )
	{
		if( m_mainSubLib[iiLib].hLib == NULL )
			continue;

		//
		// Get Service List from SIDE Driver
		//
		m_mainSubLib[iiLib].pEntry( m_mainSubLib[iiLib].pSub, NULL, SIDEcmd_GETSERVLIST, &sGSL );
		if( sGSL.item < sGSL.itemReal )
		{
			sGSL.item = sGSL.itemReal;
			HFree( paSLI );
			paSLI = (DEV_CMD_ServListItem*)HAlloc( sizeof(TECH_MainServiceList) * sGSL.item );
			sGSL.pSLI = paSLI;
			m_mainSubLib[iiLib].pEntry( m_mainSubLib[iiLib].pSub, NULL, SIDEcmd_GETSERVLIST, &sGSL );
		}

		//
		// Put SIDE Driver Services into Main Service Collection
		//
		for( ii=0; ii<sGSL.itemReal; ii++ )
		{
			strncpy( sMSL.name, paSLI[ii].name, SERVNAME_SIZE );
			sMSL.servLocation = TECHsl_SIDE;
			sMSL.attr = paSLI[ii].attr;
			sMSL.pEntry = m_mainSubLib[iiLib].pEntry;
			sMSL.pSub = m_mainSubLib[iiLib].pSub;
			m_mainServColl.Include( &sMSL );
		}
	}

	//
	// Service Enumeration
	//

	//
	// Go through all SIDE Drivers to Set Enumerated Service Lists
	//
	TECH_MainServiceList	*pMSL;
	TECH_MainServiceList	sPrevMSL;
	S32						itemNo;

	//
	// Go through BASE Driver Services
	//
	for( itemNo=0; itemNo<m_mainServColl.GetSize(); itemNo++ )
	{
		pMSL = (TECH_MainServiceList*)m_mainServColl.GetItem(itemNo);
		if( pMSL->servLocation != TECHsl_BASE )
			break;
	}

	//
	// Go through TECH Driver Services
	//
	sSSL.item = 0;
	sSSL.pSLI = paSLI;
	for( ; itemNo<m_mainServColl.GetSize(); itemNo++ )
	{
		pMSL = (TECH_MainServiceList*)m_mainServColl.GetItem(itemNo);
		if( pMSL->servLocation != TECHsl_TECH )
			break;
		strncpy( paSLI[sSSL.item].name, pMSL->name, SERVNAME_SIZE );
		paSLI[sSSL.item].attr    = pMSL->attr;
		paSLI[sSSL.item].idxMain = itemNo;
		sSSL.item++;
	}
	if( 0!=sSSL.item )
		m_collServ.SetServiceList( &sSSL );

	//
	// Go through SIDE Driver Services
	//
	sSSL.item = 0;
	sPrevMSL.pSub = NULL;
	for( ; itemNo<m_mainServColl.GetSize(); itemNo++ )
	{
		pMSL = (TECH_MainServiceList*)m_mainServColl.GetItem(itemNo);
		if( sPrevMSL.pSub != pMSL->pSub )
		{
			if( sPrevMSL.pSub != NULL )
			{
				//
				// Send  Enumerated Service Lists to SIDE Deiver
				//
				sSSL.pSLI = paSLI;
				sPrevMSL.pEntry( sPrevMSL.pSub, NULL, SIDEcmd_SETSERVLIST, &sSSL );

			}
			sPrevMSL = *pMSL;
			sSSL.item = 0;
		}

		//
		// Copy Service Params to List
		//
		strncpy( paSLI[sSSL.item].name, pMSL->name, SERVNAME_SIZE );
		paSLI[sSSL.item].attr    = pMSL->attr;
		paSLI[sSSL.item].idxMain = itemNo;
		sSSL.item++;
	}
	if( sPrevMSL.pSub != NULL )
	{
		//
		// Send  Enumerated Service Lists to SIDE Deiver
		//
		sSSL.pSLI = paSLI;
		sPrevMSL.pEntry( sPrevMSL.pSub, NULL, SIDEcmd_SETSERVLIST, &sSSL );

	}

	HFree( paSLI );

	return 0;
}

//=************** TECH_Driver::CallBaseEntry **************
//=********************************************************
S32		TECH_Driver::CallBaseEntry( S32 cmd, void *pArgs )
{
	S32		err;

	err = m_pBaseEntry( m_pBaseDev, cmd, pArgs );
	return err;
}

//=************** TECH_Driver::Dev_InitData ***************
//=********************************************************
S32		TECH_Driver::Dev_InitData( S32 idxThisBrd, char *pBoardName, void *pSubunitICR, void *pCfg, 
									TBRD_InitData *paInitData, S32 initDataSize )
{
	void					*pBaseDev;
	DEV_API_initData		*pDEV_initData;
	DEV_API_entry			*pEntry;
	HMODULE					hBaseLib;				// DLL header
	S32						boardType;
	
	S32						initDataIdx;
	char					libName[128];

	initDataIdx = 0;
	strcpy( libName, m_baseLibName );
	InitData_SearchBaseunit( paInitData, initDataSize, &initDataIdx, libName );

	//
	// Open Library
	//
	hBaseLib = LoadLibrary( libName );
	if( hBaseLib <= (HINSTANCE)HINSTANCE_ERROR )
	{
		return TECHERR(BRDerr_ERROR);
	}

	//
	// Get Entry Point
	//
	pDEV_initData  = (DEV_API_initData*)GetProcAddress( hBaseLib, "_DEV_initData@20" );
	pEntry         = (DEV_API_entry*)GetProcAddress( hBaseLib, "_DEV_entry@12" );
	if( pDEV_initData == 0 || pEntry == 0 )
	{
		FreeLibrary( hBaseLib );
		return TECHERR(BRDerr_ERROR);
	}

	//
	// Parse the rest of the record
	//
	boardType =	(pDEV_initData)( idxThisBrd, &pBaseDev, pBoardName, paInitData+initDataIdx, initDataSize-initDataIdx );
	if( boardType<=0 )
	{
		FreeLibrary( hBaseLib );
		return boardType;
	}
	if( pBaseDev == NULL )
	if( boardType<=0 )
	{
		FreeLibrary( hBaseLib );
		return TECHERR(BRDerr_ERROR);
	}

	//
	// Fill Device Extension
	//
	strcpy( m_boardName, pBoardName );
	m_pBaseEntry = pEntry;
	m_pBaseDev = pBaseDev;	
	m_hBaseLib = hBaseLib;

	//
	// Fill Main Subunit Library Array
	//
	InitData_FillSubLib( paInitData, initDataSize, pSubunitICR, pCfg );

	//
	// Fill Main Service Collection
	//
	InitMainServCollection();

	//
	// Return 
	//
	return boardType;
}

//=************* TECH_Driver::Dev_InitAuto ****************
//=********************************************************
S32		TECH_Driver::Dev_InitAuto( S32 idxThisBrd, char *pBoardName, void *pSubunitICR, void *pCfg,
									 char *pLin, S32 linSize, TBRD_SubEnum *pSubEnum, U32 sizeSubEnum )
{
	return 0;
}

//=***************** TECH_Driver::Dev_Entry ***************
//=********************************************************
S32		TECH_Driver::Dev_Entry( void *pContext, S32 cmd, void *pArgs )
{
	//
	// Support DEVcmd_INIT
	//
	if( cmd==DEVcmd_INIT )
	{
		S32				iiLib;
		SIDE_CMD_Init	sInit;


		sInit.pDev = this;
		sInit.pEntry = DEVS_entry;
		strncpy( sInit.boardName, m_boardName, BOARDNAME_SIZE );
		sInit.hMutex = NULL;

		for( iiLib=0; iiLib<SIDELIB_MAX; iiLib++ )
		{
			if( m_mainSubLib[iiLib].hLib == NULL )
				continue;

			m_mainSubLib[iiLib].pEntry( m_mainSubLib[iiLib].pSub, NULL, SIDEcmd_INIT, &sInit );
		}		
		return BRDerr_OK;
	}

	//
	// Support DEVcmd_GETSERVLIST
	//
	if( cmd==DEVcmd_GETSERVLIST )
	{
		DEV_CMD_GetServList		*pGSL = (DEV_CMD_GetServList*)pArgs;
		TECH_MainServiceList	*pMSL;
		S32						itemNo;
		
		pGSL->itemReal = m_mainServColl.GetSize();
		if( pGSL->item < (U32)m_mainServColl.GetSize() )
			return BRDerr_BUFFER_TOO_SMALL;

		for( itemNo=0; itemNo<m_mainServColl.GetSize(); itemNo++ )
		{
			pMSL = (TECH_MainServiceList*)m_mainServColl.GetItem(itemNo);
			strncpy( pGSL->pSLI[itemNo].name, pMSL->name, SERVNAME_SIZE );
			pGSL->pSLI[itemNo].attr = pMSL->attr;
			pGSL->pSLI[itemNo].idxMain = itemNo;
		}
		return BRDerr_OK;
	}

	//
	// Support DEVcmd_CTRL
	//
	if( cmd==DEVcmd_CTRL )
	{
		DEV_CMD_Ctrl			*pCtrl = (DEV_CMD_Ctrl*)pArgs;
		SIDE_CMD_Ctrl			sCtrl;
		TECH_MainServiceList	*pMSL;
		int						idxMain  = (pCtrl->handle >> 16) & 0x3F;
		S32						err;

		pMSL = (TECH_MainServiceList*)m_mainServColl.GetItem(idxMain);
		if( pMSL->servLocation == TECHsl_TECH )
		{
			sCtrl.handle = pCtrl->handle;
			sCtrl.cmd = pCtrl->cmd;
			sCtrl.arg = pCtrl->arg;

			err = m_collServ.Ctrl( pCtrl->handle, pContext, pCtrl->cmd, pCtrl->arg );
			if( err!=BRDerr_CMD_UNSUPPORTED )
				return err;
		}
		if( pMSL->servLocation == TECHsl_SIDE )
		{
			sCtrl.handle = pCtrl->handle;
			sCtrl.cmd = pCtrl->cmd;
			sCtrl.arg = pCtrl->arg;

			err = pMSL->pEntry( pMSL->pSub, pContext, SIDEcmd_CTRL, &sCtrl );
			if( err!=BRDerr_CMD_UNSUPPORTED )
				return err;
		}
	}

	//
	// Default
	//
	return m_pBaseEntry( m_pBaseDev, cmd, pArgs );
}

//
// End of File
//

