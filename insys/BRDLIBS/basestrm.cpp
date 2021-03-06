//=********************************************************
//
// BASESTRM.CPP
//
// BRD_BaseStream Base Class
//
// (C) Instrumental Systems
//
// Created: Ekkore Feb. 2003
// Modified:
//     Feb 2005 Ekkore - Support SelfCreated Blocks (isCont=2)
//
//=********************************************************

#include	<malloc.h>
#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<windows.h>

#include	"utypes.h"
#include	"brd.h"
#include	"brderr.h"
#include	"brdfunx.h"
#include	"basestrm.h"

//=************* BRD_BaseStrm::BRD_BaseStrm ***************
//=********************************************************
BRD_BaseStrm::BRD_BaseStrm( const char *devName, const char *rsrcName )
{
	char	tmpName[128];
	int		isAlreadyCreated = 0;

	//
	// Prepare Root Name
	//
	strcpy( m_rootName, devName );
	strcat( m_rootName, "_" );
	strcat( m_rootName, rsrcName );
	strcat( m_rootName, "ROOT" );

	//
	// Create Shareable (Global) Control Structure
	//
	strcpy( tmpName, m_rootName );
	strcat( tmpName, "MU" );
	m_hMutex = CreateMutex( NULL, FALSE, tmpName );

	strcpy( tmpName, m_rootName );
	strcat( tmpName, "FM" );
	m_hFileMap = CreateFileMappingA( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 
									 0, sizeof(BRDstrm_ShareData), tmpName );
	isAlreadyCreated = ( GetLastError() == ERROR_ALREADY_EXISTS ) ? 1 : 0;
	m_pShareData = ( m_hFileMap==NULL ) ? NULL : 
			(BRDstrm_ShareData*)MapViewOfFile( m_hFileMap, FILE_MAP_WRITE, 0, 0, 0 );

	//
	// Fill Shareable (Global) Control Structure
	//
	if( m_pShareData!=NULL )
	if( !isAlreadyCreated )
	{
		MutexWait();
		if( m_pShareData->rootCounter==0 )
		{
			m_pShareData->rootCounter = 1;
		}
		MutexFree();
	}

	//
	// Variables for ECLUSIVE Capture Mode
	//
	m_rootCounterExcl = 1;
	m_cntCaptModeExcl = 0;
	m_pBlockPointersExcl = NULL;
	m_stubPointerExcl.pStub = NULL;
	m_stubPointerExcl.hFileMap = NULL;
	m_blkNumExcl  = 0;
	m_blkSizeExcl = 0;

	//
	// Variables for SPY Capture Mode
	//
	m_rootCounterSpy = 0;
	m_cntCaptModeSpy = 0;
	m_pBlockPointersSpy = NULL;
	m_stubPointerSpy.pStub = NULL;
	m_stubPointerSpy.hFileMap = NULL;
	m_blkNumSpy  = 0;
	m_blkSizeSpy = 0;

}

//=************* BRD_BaseStrm::~BRD_BaseStrm **************
//=********************************************************
BRD_BaseStrm::~BRD_BaseStrm( )
{
	MutexWait();

	//
	// Free CBuf's
	//
	while( m_cntCaptModeSpy>1 )
		CBufFreeSpy();

	if( m_cntCaptModeExcl )
		CBufFreeExclusive();


	//
	// Free Shareable (Global) Control Structure
	//
	if( m_pShareData )
		UnmapViewOfFile( m_pShareData );
	if( m_hFileMap )
		CloseHandle( m_hFileMap );

	MutexFree();

	if( m_hMutex )
		CloseHandle( m_hMutex );
}

//=************* BRD_BaseStrm::IsValid ********************
//=********************************************************
S32	BRD_BaseStrm::IsValid( void )
{
	if( m_pShareData && m_hFileMap && m_hMutex )
		return 0;	//Valid
	return -1;		//Invalid
}

//=************* BRD_BaseStrm::CBufAllocExclusive *********
//=********************************************************
S32		BRD_BaseStrm::CBufAllocExclusive( U32 blkNum, U32 blkSize, U32 dir, U32 isCont, 
										  U08 **ppBlk, BRDstrm_Stub **ppStub )
{
	S32					err;

	MutexWait();
	if( m_cntCaptModeExcl )
	{
		MutexFree();
		return DRVERR(BRDerr_STREAM_ALREADY_ALLOCATED);
	}

	if( blkNum==0 || blkSize==0 )
	{
		MutexFree();
		return DRVERR(BRDerr_BAD_PARAMETER);
	}

	if( (dir&3)!=BRDstrm_DIR_IN && (dir&3)!=BRDstrm_DIR_OUT )
	{
		MutexFree();
		return DRVERR(BRDerr_BAD_PARAMETER);
	}


	//
	// Make Allocation
	//
	m_isCont = isCont;
	switch( isCont )
	{
		case 0:
			err = CBufAllocDiscontinuous( blkNum, blkSize, dir, ppBlk, ppStub );
			break;
		case 2:
			err = CBufAllocSelfcreated( blkNum, blkSize, dir, ppBlk, ppStub );
			break;
		default:
			err = CBufAllocContinuous( blkNum, blkSize, dir, ppBlk, ppStub );
			break;
	}

	MutexFree();
	return err;
}

//=************* BRD_BaseStrm::CBufAllocSpy ***************
//=********************************************************
S32		BRD_BaseStrm::CBufAllocSpy( U32 blkNum, U32 *pBlkNumReal, U32 *pBlkSize, 
								    U32 *pDir, U08 **ppBlk, BRDstrm_Stub **ppStub )
{
	int					ii;
	char				tmpName[128];
	char				tmpLin[32];

	MutexWait();
	*pBlkNumReal = m_pShareData->blkNum;

	//
	// Check if Stream has already been Allocated
	//
	if( m_pShareData->blkNum==0 )
	{
		MutexFree();
		return DRVERR(BRDerr_STREAM_NOT_ALLOCATED_YET);
	}

	//
	// Check if Stream has already been Allocated as Continuous
	//
	if( m_pShareData->isCont!=0 )
	{
		MutexFree();
		return DRVERR(BRDerr_STREAM_CONTINUOUS);
	}

	//
	// Check Size of Array "ppBlk[]"
	//
	if( blkNum < m_pShareData->blkNum )
	{
		MutexFree();
		return DRVERR(BRDerr_STREAM_WARNING);
	}


	//
	// If Stream has already been Allocated with SPY mode
	//
	if( m_cntCaptModeSpy )
	{
		//
		// If CBuf has already been destroied
		//
		if( m_pShareData->rootCounter != m_rootCounterSpy )
		{
			MutexFree();
			return DRVERR(BRDerr_STREAM_ALREADY_DESTROYED);
		}

		//
		// Fill Pointers
		//
		for( ii=0; ii < (int)m_pShareData->blkNum; ii++ )
		{
			ppBlk[ii] = m_pBlockPointersExcl[ii].pBlock;
		}
		*ppStub   = m_stubPointerExcl.pStub;

		m_cntCaptModeSpy++;
		MutexFree();
		return DRVERR(BRDerr_OK);
	}

	//
	// Create Array of Blocks
	//
	m_pBlockPointersSpy = (BRDstrm_BlockPointer*)HAlloc( m_pShareData->blkNum*sizeof(BRDstrm_BlockPointer));
	if( m_pBlockPointersSpy==NULL )
	{
		MutexFree();
		return DRVERR(BRDerr_INSUFFICIENT_RESOURCES);
	}

	//
	// Fill Array of Blocks
	//
	for( ii=0; ii < (int)m_pShareData->blkNum; ii++ )
	{
		//
		// Form FileMapping Name for current Block
		//
		strcpy( tmpName, m_rootName );
		_itoa( m_pShareData->rootCounter, tmpLin, 10 );
		strcat( tmpName, tmpLin );
		strcat( tmpName, "_" );
		_itoa( ii, tmpLin, 10 );
		strcat( tmpName, tmpLin );

		//
		// Create FileMapping and Get Pointer
		//
		m_pBlockPointersSpy[ii].hFileMap = 
				CreateFileMappingA( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 
									0, m_pShareData->blkSize, tmpName );
		m_pBlockPointersSpy[ii].pBlock = 
				( m_pBlockPointersSpy[ii].hFileMap==NULL ) ? 
				NULL :
				(U08*)MapViewOfFile( m_pBlockPointersSpy[ii].hFileMap, 
									 FILE_MAP_WRITE, 0, 0, 0 );
		if( m_pBlockPointersSpy[ii].pBlock == NULL )
		{
			ClearBlocksSpy();
			MutexFree();
			return DRVERR(BRDerr_INSUFFICIENT_RESOURCES);			
		}
	}

	//
	// Form FileMapping Name for Stub
	//
	strcpy( tmpName, m_rootName );
	_itoa( m_pShareData->rootCounter, tmpLin, 10 );
	strcat( tmpName, tmpLin );
	strcat( tmpName, "Stub" );

	//
	// Fill Stub
	//
	m_stubPointerSpy.hFileMap = 
			CreateFileMappingA( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 
								0, 2*sizeof(BRDstrm_Stub), tmpName );
	m_stubPointerSpy.pStub = 
			( m_stubPointerSpy.hFileMap==NULL ) ? 
			NULL :
			(BRDstrm_Stub*)MapViewOfFile( m_stubPointerSpy.hFileMap, 
										  FILE_MAP_WRITE, 0, 0, 0 );
	if( m_stubPointerSpy.pStub == NULL )
	{
		ClearBlocksSpy();
		MutexFree();
		return DRVERR(BRDerr_INSUFFICIENT_RESOURCES);			
	}

	//
	// Fill Class Data
	//
	m_rootCounterSpy = m_pShareData->rootCounter;
	m_cntCaptModeSpy++;	
	m_blkNumSpy  = m_pShareData->blkNum;		
	m_blkSizeSpy = m_pShareData->blkSize;	

	//
	// Fill Return Data
	//
	for( ii=0; ii < (int)blkNum; ii++ )
	{
		ppBlk[ii] = m_pBlockPointersSpy[ii].pBlock;
	}
	*ppStub   = m_stubPointerSpy.pStub;
	*pBlkSize = m_pShareData->blkSize;
	*pDir     = m_pShareData->dir;

	MutexFree();
	return DRVERR(BRDerr_OK);
}

//=************* BRD_BaseStrm::CBufFreeExclusive **********
//=********************************************************
S32		BRD_BaseStrm::CBufFreeExclusive( void )
{
	S32				err;


	MutexWait();

	//
	// Make Free Operation
	//
	switch( m_pShareData->isCont )
	{
		case 0:
			err = CBufFreeDiscontinuous( );
			break;
		case 2:
			err = CBufFreeSelfcreated( );
			break;
		default:
			err = CBufFreeContinuous( );
			break;
	}

	MutexFree();
	return err;
}

//=************* BRD_BaseStrm::CBufFreeSpy ****************
//=********************************************************
S32		BRD_BaseStrm::CBufFreeSpy( void )
{
	MutexWait();

	//
	// If There is no Allocated SPY CBuf
	//
	if( m_cntCaptModeSpy<=0 )
	{
		m_cntCaptModeSpy = 0;
		MutexFree();
		return -1;		// -1 - Stream is not Allocated
	}

	//
	// If There is once more SPY CBuf
	//
	if( m_cntCaptModeSpy > 1 )
	{
		m_cntCaptModeSpy--;
		MutexFree();
		return 0;
	}

	//
	// Clear Blocks and Stub
	//
	ClearBlocksSpy();

	//
	// Finish
	//
	m_rootCounterSpy = 0;
	m_cntCaptModeSpy = 0;	
	MutexFree();
	return 0;
}

//=********** BRD_BaseStrm::CBufAllocDiscontinuous ********
//=********************************************************
S32		BRD_BaseStrm::CBufAllocDiscontinuous( U32 blkNum, U32 blkSize, U32 dir, 
										  U08 **ppBlk, BRDstrm_Stub **ppStub )
{
	int					ii;
	char				tmpName[128];
	char				tmpLin[32];


	//
	// Create Array of Blocks
	//
	m_pBlockPointersExcl = (BRDstrm_BlockPointer*)HAlloc( blkNum*sizeof(BRDstrm_BlockPointer) );
	if( m_pBlockPointersExcl==NULL )
	{
		return DRVERR(BRDerr_INSUFFICIENT_RESOURCES);
	}

	//
	// Fill Array of Blocks
	//
	for( ii=0; ii < (int)blkNum; ii++ )
	{
		//
		// Form FileMapping Name for current Block
		//
		strcpy( tmpName, m_rootName );
		_itoa( m_pShareData->rootCounter, tmpLin, 10 );
		strcat( tmpName, tmpLin );
		strcat( tmpName, "_" );
		_itoa( ii, tmpLin, 10 );
		strcat( tmpName, tmpLin );

		//
		// Create FileMapping and Get Pointer
		//
		m_pBlockPointersExcl[ii].hFileMap = 
				CreateFileMappingA( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 
									0, blkSize, tmpName );
		m_pBlockPointersExcl[ii].pBlock = 
				( m_pBlockPointersExcl[ii].hFileMap==NULL ) ? 
				NULL :
				(U08*)MapViewOfFile( m_pBlockPointersExcl[ii].hFileMap, 
									 FILE_MAP_WRITE, 0, 0, 0 );
		if( m_pBlockPointersExcl[ii].pBlock == NULL )
		{
			ClearBlocksExcl();
			return DRVERR(BRDerr_INSUFFICIENT_RESOURCES);			
		}
	}

	//
	// Form FileMapping Name for Stub
	//
	strcpy( tmpName, m_rootName );
	_itoa( m_pShareData->rootCounter, tmpLin, 10 );
	strcat( tmpName, tmpLin );
	strcat( tmpName, "Stub" );

	//
	// Fill Stub
	//
	m_stubPointerExcl.hFileMap = 
			CreateFileMappingA( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 
								0, 2*sizeof(BRDstrm_Stub), tmpName );
	m_stubPointerExcl.pStub = 
			( m_stubPointerExcl.hFileMap==NULL ) ? 
			NULL :
			(BRDstrm_Stub*)MapViewOfFile( m_stubPointerExcl.hFileMap, 
										  FILE_MAP_WRITE, 0, 0, 0 );
	if( m_stubPointerExcl.pStub == NULL )
	{
		ClearBlocksExcl();
		return DRVERR(BRDerr_INSUFFICIENT_RESOURCES);			
	}
	m_stubPointerExcl.pStub->lastBlock = -1;
	m_stubPointerExcl.pStub->totalCounter = 0;
	m_stubPointerExcl.pStub->offset = 0;
	m_stubPointerExcl.pStub->state = BRDstrm_STAT_STOP;

	//
	// Fill Class Data
	//
	m_rootCounterExcl = m_pShareData->rootCounter;
	m_cntCaptModeExcl++;	
	m_blkNumExcl  = blkNum;		
	m_blkSizeExcl = blkSize;	

	//
	// Lock Blocks with WDM-Driver
	//
	if( 0>LockBlocks() )
	{
		m_cntCaptModeExcl--;	
		ClearBlocksExcl();
		return BRDerr_STREAM_UNLOCKED;
	}

	//
	// Fill Shareable (Global) Control Structure
	//
	m_pShareData->blkNum  = blkNum;	
	m_pShareData->blkSize = blkSize;
	m_pShareData->state   = BRDstrm_STAT_STOP;
	m_pShareData->dir     = dir;
	m_pShareData->isCont  = 0;

	//
	// Fill Return Data
	//
	for( ii=0; ii < (int)blkNum; ii++ )
	{
		ppBlk[ii] = m_pBlockPointersExcl[ii].pBlock;
	}
	*ppStub   = m_stubPointerExcl.pStub;

	return DRVERR(BRDerr_OK);
}

//=********** BRD_BaseStrm::CBufFreeDiscontinuous *********
//=********************************************************
S32		BRD_BaseStrm::CBufFreeDiscontinuous( void )
{
	//
	// Unlock Blocks with WDM-Driver
	//
	UnlockBlocks();


	//
	// If There is no Allocated EXCLUSIVE CBuf
	//
	if( m_cntCaptModeExcl<=0 )
	{
		m_cntCaptModeExcl = 0;
		return -1;	// Stream is not Allocated
	}

	//
	// Say: CBuf is Destroied
	//
	if( m_stubPointerExcl.pStub )
		m_stubPointerExcl.pStub->state = BRDstrm_STAT_DESTROY;

	//
	// Clear Blocks and Stub
	//
	ClearBlocksExcl();

	//
	// Fill Shareable (Global) Control Structure
	//
	m_pShareData->rootCounter++;
	m_pShareData->blkNum  = 0;	
	m_pShareData->blkSize = 0;
	m_pShareData->state   = BRDstrm_STAT_DESTROY;
	m_pShareData->dir     = 0;
	m_pShareData->isCont  = 0;

	//
	// Finish
	//
	m_rootCounterExcl = 0;
	m_cntCaptModeExcl = 0;	

	return 0;

}

//=************* BRD_BaseStrm::CBufAllocContinuous ********
//=********************************************************
S32		BRD_BaseStrm::CBufAllocContinuous( U32 blkNum, U32 blkSize, U32 dir, 
										  U08 **ppBlk, BRDstrm_Stub **ppStub )
{
	int					ii;


	//
	// Create Array of Blocks
	//
	m_pBlockPointersExcl = (BRDstrm_BlockPointer*)HAlloc( blkNum*sizeof(BRDstrm_BlockPointer) );
	if( m_pBlockPointersExcl==NULL )
	{
		return DRVERR(BRDerr_INSUFFICIENT_RESOURCES);
	}

	//
	// Fill Class Data
	//
	m_rootCounterExcl = m_pShareData->rootCounter;
	m_cntCaptModeExcl++;	
	m_blkNumExcl  = blkNum;		
	m_blkSizeExcl = blkSize;	

	//
	// Alloc Blocks with WDM-Driver
	//
	if( 0>AllocBlocks() )
	{
		m_rootCounterExcl = 0;
		m_cntCaptModeExcl--;	
		m_blkNumExcl  = 0;		
		m_blkSizeExcl = 0;	
		HFree(m_pBlockPointersExcl);
		return BRDerr_INSUFFICIENT_RESOURCES;
	}

	//
	// Fill Stub
	//
	if( m_stubPointerExcl.pStub )
	{
		m_stubPointerExcl.pStub->lastBlock = -1;
		m_stubPointerExcl.pStub->totalCounter = 0;
		m_stubPointerExcl.pStub->offset = 0;
		m_stubPointerExcl.pStub->state = BRDstrm_STAT_STOP;
	}

	//
	// Fill Shareable (Global) Control Structure
	//
	m_pShareData->blkNum  = blkNum;	
	m_pShareData->blkSize = blkSize;
	m_pShareData->state   = BRDstrm_STAT_STOP;
	m_pShareData->dir     = dir;
	m_pShareData->isCont  = 1;

	//
	// Fill Return Data
	//
	for( ii=0; ii < (int)blkNum; ii++ )
	{
		ppBlk[ii] = m_pBlockPointersExcl[ii].pBlock;
	}
	*ppStub   = m_stubPointerExcl.pStub;

	return DRVERR(BRDerr_OK);
}

//=************* BRD_BaseStrm::CBufFreeContinuous **********
//=********************************************************
S32		BRD_BaseStrm::CBufFreeContinuous( void )
{

	//
	// Disalloc Blocks with WDM-Driver
	//
	DisallocBlocks();


	//
	// If There is no Allocated EXCLUSIVE CBuf
	//
	if( m_cntCaptModeExcl<=0 )
	{
		m_cntCaptModeExcl = 0;
		return -1;	// Stream is not Allocated
	}

	//
	// Free Array of Block Pointers
	//
	if( m_pBlockPointersExcl!=NULL )
		HFree(m_pBlockPointersExcl);

	//
	// Fill Shareable (Global) Control Structure
	//
	m_pShareData->rootCounter++;
	m_pShareData->blkNum  = 0;	
	m_pShareData->blkSize = 0;
	m_pShareData->state   = BRDstrm_STAT_DESTROY;
	m_pShareData->dir     = 0;
	m_pShareData->isCont  = 0;

	//
	// Finish
	//
	m_rootCounterExcl = 0;
	m_cntCaptModeExcl = 0;	

	return 0;
}

//=********** BRD_BaseStrm::CBufAllocSelfcreated **********
//=********************************************************
S32		BRD_BaseStrm::CBufAllocSelfcreated( U32 blkNum, U32 blkSize, U32 dir, 
										  U08 **ppBlk, BRDstrm_Stub **ppStub )
{
	int					ii;

	//
	// Check Input Data
	//
	for( ii=0; ii < (int)blkNum; ii++ )
	{
		if( NULL == ppBlk[ii] )
			return DRVERR(BRDerr_BAD_PARAMETER);

	}

	//
	// Create Array of Blocks
	//
	m_pBlockPointersExcl = (BRDstrm_BlockPointer*)HAlloc( blkNum*sizeof(BRDstrm_BlockPointer) );
	if( m_pBlockPointersExcl==NULL )
	{
		return DRVERR(BRDerr_INSUFFICIENT_RESOURCES);
	}

	//
	// Fill Array of Blocks
	//
	for( ii=0; ii < (int)blkNum; ii++ )
	{
		m_pBlockPointersExcl[ii].pBlock = ppBlk[ii];
	}

	//
	// Fill Stub
	//
	m_stubPointerExcl.pStub = (BRDstrm_Stub*)HAlloc( 2*sizeof(BRDstrm_Stub) );
	if( m_stubPointerExcl.pStub == NULL )
	{
		HFree( m_pBlockPointersExcl );
		m_pBlockPointersExcl = NULL;
		return DRVERR(BRDerr_INSUFFICIENT_RESOURCES);			
	}

	m_stubPointerExcl.pStub->lastBlock = -1;
	m_stubPointerExcl.pStub->totalCounter = 0;
	m_stubPointerExcl.pStub->offset = 0;
	m_stubPointerExcl.pStub->state = BRDstrm_STAT_STOP;

	//
	// Fill Class Data
	//
	m_rootCounterExcl = m_pShareData->rootCounter;
	m_cntCaptModeExcl++;	
	m_blkNumExcl  = blkNum;		
	m_blkSizeExcl = blkSize;	

	//
	// Lock Blocks with WDM-Driver
	//
	if( 0>LockBlocks() )
	{
		m_cntCaptModeExcl--;	

		HFree( m_pBlockPointersExcl );
		m_pBlockPointersExcl = NULL;

		HFree( m_stubPointerExcl.pStub );
		m_stubPointerExcl.pStub = NULL;

		return BRDerr_STREAM_UNLOCKED;
	}

	//
	// Fill Shareable (Global) Control Structure
	//
	m_pShareData->blkNum  = blkNum;	
	m_pShareData->blkSize = blkSize;
	m_pShareData->state   = BRDstrm_STAT_STOP;
	m_pShareData->dir     = dir;
	m_pShareData->isCont  = 2;

	//
	// Fill Return Data
	//
	*ppStub   = m_stubPointerExcl.pStub;

	return DRVERR(BRDerr_OK);
}

//=********** BRD_BaseStrm::CBufFreeSelfcreated ***********
//=********************************************************
S32		BRD_BaseStrm::CBufFreeSelfcreated( void )
{
	//
	// Unlock Blocks with WDM-Driver
	//
	UnlockBlocks();


	//
	// If There is no Allocated EXCLUSIVE CBuf
	//
	if( m_cntCaptModeExcl<=0 )
	{
		m_cntCaptModeExcl = 0;
		return -1;		// Stream is not Allocated
	}

	//
	// Say: CBuf is Destroied
	//
	if( m_stubPointerExcl.pStub )
		m_stubPointerExcl.pStub->state = BRDstrm_STAT_DESTROY;

	//
	// Clear Blocks and Stub
	//
	if( m_pBlockPointersExcl != NULL )
	{
		HFree( m_pBlockPointersExcl );
		m_pBlockPointersExcl = NULL;
	}

	if( m_stubPointerExcl.pStub != NULL )
	{
		HFree( m_stubPointerExcl.pStub );
		m_stubPointerExcl.pStub = NULL;
	}

	//
	// Fill Shareable (Global) Control Structure
	//
	m_pShareData->rootCounter++;
	m_pShareData->blkNum  = 0;	
	m_pShareData->blkSize = 0;
	m_pShareData->state   = BRDstrm_STAT_DESTROY;
	m_pShareData->dir     = 0;
	m_pShareData->isCont  = 0;

	//
	// Finish
	//
	m_rootCounterExcl = 0;
	m_cntCaptModeExcl = 0;
		
	return 0;
}

//=************* BRD_BaseStrm::ClearBlocksExcl ************
//=********************************************************
void		BRD_BaseStrm::ClearBlocksExcl(void)
{
	int			ii;

	//
	// Clear Blocks
	//
	if( m_pBlockPointersExcl )
	{
		for( ii=0; ii < (int)m_blkNumExcl; ii++ )
		{
			if( m_pBlockPointersExcl[ii].pBlock )
				UnmapViewOfFile( m_pBlockPointersExcl[ii].pBlock );
			if( m_pBlockPointersExcl[ii].hFileMap )
				CloseHandle( m_pBlockPointersExcl[ii].hFileMap );
		}
		HFree( m_pBlockPointersExcl );
		m_pBlockPointersExcl = NULL;
	}

	//
	// Clear Stub
	//
	if( m_stubPointerExcl.pStub )
	{
		UnmapViewOfFile( m_stubPointerExcl.pStub );
		m_stubPointerExcl.pStub = NULL;
	}
	if( m_stubPointerExcl.hFileMap )
	{
		CloseHandle( m_stubPointerExcl.hFileMap );
		m_stubPointerExcl.hFileMap = 0;
	}

	//
	// Zero Some Variables
	//
	m_blkNumExcl = 0;
	m_blkSizeExcl = 0;
}

//=************* BRD_BaseStrm::ClearBlocksSpy *************
//=********************************************************
void		BRD_BaseStrm::ClearBlocksSpy(void)
{
	int			ii;

	//
	// Clear Blocks
	//
	if( m_pBlockPointersSpy )
	{
		for( ii=0; ii < (int)m_blkNumSpy; ii++ )
		{
			if( m_pBlockPointersSpy->pBlock )
				UnmapViewOfFile( m_pBlockPointersSpy->pBlock );
			if( m_pBlockPointersSpy->hFileMap )
				CloseHandle( m_pBlockPointersSpy->hFileMap );
		}
		HFree( m_pBlockPointersSpy );
		m_pBlockPointersSpy = NULL;
	}

	//
	// Clear Stub
	//
	if( m_stubPointerSpy.pStub )
		UnmapViewOfFile( m_stubPointerSpy.pStub );
	if( m_stubPointerSpy.hFileMap )
		CloseHandle( m_stubPointerSpy.hFileMap );

	//
	// Zero Some Variables
	//
	m_blkNumSpy = 0;
	m_blkSizeSpy = 0;
}

//
// End of File
//

