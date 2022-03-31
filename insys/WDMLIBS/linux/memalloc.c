//=********************************************************
// MemAlloc.cpp - Class TMemAlloc Definition
//
// (C) Copyright Ekkore 2003
//
// Created: 
//		10.04.2003 by Ekkore
// Modified:
//
//=********************************************************

#include	"MemAlloc.h" 

//=*************** TMemAlloc::TMemAlloc *******************
//=********************************************************
	TMemAlloc::TMemAlloc(void) 
{
	MEMALLOC_ExInitializeXxxLookasideList( &m_lalBlock, NULL, NULL, 0, sizeof(TMemBlock), 'ollA', 0 );
	MEMALLOC_ExInitializeXxxLookasideList( &m_lalVA, NULL, NULL, 0, sizeof(TMemVA), 'ollA', 0 );
	InitializeListHead( &m_mainList );
	KeInitializeSpinLock( &m_spin );
}


//=*************** TMemAlloc::~TMemAlloc ******************
//=********************************************************
	TMemAlloc::~TMemAlloc(void)
{
	MEMALLOC_ExDeleteXxxLookasideList( &m_lalBlock );
	MEMALLOC_ExDeleteXxxLookasideList( &m_lalVA );
}


//=*************** TMemAlloc::Alloc ***********************
//=********************************************************
NTSTATUS TMemAlloc::Alloc( PFILE_OBJECT pFileObject, ULONG bytes, ULONG *pAppAdr, ULONG *pPhyAdr, ULONG *pSysAdr, PMDL *ppMdl )
{
	NTSTATUS			status;
	PHYSICAL_ADDRESS	AddressToMap;
	ULONG				appAdr;

	TMemBlock			sMemBlock;
	TMemBlock			*pMemBlock;
	TMemVA				*pMemVA;

	//
	// Check DMA Adapter
	//
	if( m_pAdapter == NULL )
	{
		KdPrint((".....:  TMemAlloc::Alloc() Bad DMA Adapter\n"));
		return STATUS_UNSUCCESSFUL;
	}

	//
	// Alocate Buffer
	//
	sMemBlock.bytes = bytes;
	sMemBlock.sysAdr = (ULONG)HalAllocateCommonBuffer( m_pAdapter, bytes, &AddressToMap, FALSE );
	if(sMemBlock.sysAdr == NULL) 
	{
		status = STATUS_INSUFFICIENT_RESOURCES; 
		KdPrint((".....:  TMemAlloc::Alloc() HalAllocateCommonBuffer FAILED 0x%X\n", status));
		return status;
	}

	sMemBlock.phyAdr = AddressToMap.LowPart;

	//
	// allocate an MDL
	//
	//sMemBlock.pMdl = MmCreateMdl( NULL, (PVOID)sMemBlock.sysAdr, bytes );
	sMemBlock.pMdl = IoAllocateMdl( (PVOID)sMemBlock.sysAdr, bytes, FALSE, FALSE, NULL);
	if( NULL == sMemBlock.pMdl) 
	{
		HalFreeCommonBuffer( m_pAdapter, bytes, AddressToMap, (PVOID)sMemBlock.sysAdr, FALSE );
		status = STATUS_INSUFFICIENT_RESOURCES; 
		KdPrint((".....:  TMemAlloc::Alloc() IoAllocateMdl FAILED 0x%X\n", status));
		return status;
	}
	MmBuildMdlForNonPagedPool(sMemBlock.pMdl);

	//
	// Get VA
	//
	appAdr = (ULONG)MmMapLockedPages( sMemBlock.pMdl, UserMode );
	if( appAdr == 0 )
	{
		IoFreeMdl(sMemBlock.pMdl);
		HalFreeCommonBuffer( m_pAdapter, bytes, AddressToMap, (PVOID)sMemBlock.sysAdr, FALSE );
		status = STATUS_INSUFFICIENT_RESOURCES; 
		KdPrint((".....:  TMemAlloc::Alloc() MmMapLockedPages FAILED 0x%X\n", status));
		return status;
	}

	//
	// Keep Empty TMemBlock and TMemVA
	//
	if( 0>KeepBlockAndVA( pFileObject, appAdr, &sMemBlock, &pMemBlock, &pMemVA ) )
	{
		MmUnmapLockedPages( (PVOID)appAdr, sMemBlock.pMdl );
		IoFreeMdl(sMemBlock.pMdl);
		HalFreeCommonBuffer( m_pAdapter, bytes, AddressToMap, (PVOID)sMemBlock.sysAdr, FALSE );
		status = STATUS_INSUFFICIENT_RESOURCES; 
		KdPrint((".....:  TMemAlloc::Alloc() KeepEmptyBlock FAILED 0x%X\n", status));
		return status;
	}

	//
	// Return Results
	//
	*pAppAdr = (ULONG)appAdr;
	*pPhyAdr = (ULONG)sMemBlock.phyAdr;
	
	if( pSysAdr )
		*pSysAdr = (ULONG)sMemBlock.sysAdr;
	if( ppMdl )
		*ppMdl = sMemBlock.pMdl;

//	KdPrint((".....:  TMemAlloc::Alloc() appAdr=0x%X, phyAdr=0x%X, sysAdr=0x%X, size=0x%X, pMdl=0x%X, \n", 
//				appAdr, sMemBlock.phyAdr, sMemBlock.sysAdr, bytes, sMemBlock.pMdl ));

	return STATUS_SUCCESS;
}

//=*************** TMemAlloc::AllocVA *********************
//=********************************************************
NTSTATUS TMemAlloc::AllocVA( PFILE_OBJECT pFileObject, ULONG phyAdr, ULONG *pAppAdr, PMDL *ppMdl, ULONG *pIsBlockFree )
{
	TMemBlock			sMemBlock;
	TMemBlock			*pMemBlock;
	TMemVA				*pMemVA;
	PLIST_ENTRY			pListBlock;
	KIRQL				kIrql;


	if( pIsBlockFree ) 
		*pIsBlockFree = 0;

	//
	// Check DMA Adapter
	//
	if( m_pAdapter == NULL )
	{
		KdPrint((".....:  TMemAlloc::AllocVA() Bad DMA Adapter\n"));
		return STATUS_UNSUCCESSFUL;
	}

	//
	// Allocate TMemVA
	//
	pMemVA = (TMemVA*)MEMALLOC_ExAllocateFromXxxLookasideList( &m_lalVA );
	if( pMemVA == NULL )
		return STATUS_NO_MEMORY;

	pMemVA->appAdr = 0xFFFFFFFF;
	pMemVA->pFileObject = NULL;

	//
	// Search TMemBlock
	//
	KeAcquireSpinLock( &m_spin, &kIrql );
	pListBlock = m_mainList.Flink;
	while( pListBlock != &m_mainList )
	{
		pMemBlock = (TMemBlock*)pListBlock;
		if( pMemBlock->phyAdr == phyAdr )
			break;
		pListBlock  = pMemBlock->dlList.Flink;
		pMemBlock = NULL;
	}

	if( pMemBlock == NULL )
	{
		KeReleaseSpinLock( &m_spin, kIrql );
		MEMALLOC_ExFreeToXxxLookasideList( &m_lalVA, pMemVA );
		return STATUS_UNSUCCESSFUL;
	}

	//
	// Insert TMemVA into TMemBlock List
	//
	InsertTailList( &(pMemBlock->vaList), &(pMemVA->dlList) );

	KeReleaseSpinLock( &m_spin, kIrql );

	//
	// Get VA
	//
	*pAppAdr = (ULONG)MmMapLockedPages( pMemBlock->pMdl, UserMode );
	if( *pAppAdr == 0 )
	{
		sMemBlock = *pMemBlock;

		//
		// Unkeep TMemVA
		//
		UnkeepVA( pMemBlock, pMemVA );
		
		//
		// Unkeep and Free TMemBlock if Need
		//
		if( 0<=UnkeepBlock( pMemBlock ) )
		{

			PHYSICAL_ADDRESS	AddressToMap = {0};

			//
			// Free Block
			//
			IoFreeMdl(sMemBlock.pMdl);

			AddressToMap.LowPart = sMemBlock.phyAdr;
			HalFreeCommonBuffer( m_pAdapter, sMemBlock.bytes, AddressToMap, (PVOID)sMemBlock.sysAdr, FALSE );

			if( pIsBlockFree ) 
				*pIsBlockFree = 1;
		}
		return STATUS_INSUFFICIENT_RESOURCES; 
	}

	//
	// Fill TMemVA
	//
	KeAcquireSpinLock( &m_spin, &kIrql );

	pMemVA->appAdr = *pAppAdr;
	pMemVA->pFileObject = pFileObject;

	KeReleaseSpinLock( &m_spin, kIrql );

	//
	// Return
	//
	if( ppMdl )
		*ppMdl = pMemBlock->pMdl;

	return STATUS_SUCCESS;
}

//=*************** TMemAlloc::Free ************************
//=********************************************************
NTSTATUS TMemAlloc::Free( PFILE_OBJECT pFileObject, ULONG appAdr, ULONG *pIsBlockFree )
{
	TMemBlock			sMemBlock;
	TMemBlock			*pMemBlock;
	TMemVA				*pMemVA;
	TMemVA				emptyMemVA;
	KIRQL				kIrql;

	if( pIsBlockFree ) 
		*pIsBlockFree = 0;

	//
	// Check DMA Adapter
	//
	if( m_pAdapter == NULL )
	{
		KdPrint((".....:  TMemAlloc::Free() Bad DMA Adapter\n"));
		return STATUS_UNSUCCESSFUL;
	}

	//
	// Search TMemBlock
	//
	KeAcquireSpinLock( &m_spin, &kIrql );
	if( 0 > FindBlock( pFileObject, appAdr, &pMemBlock, &pMemVA ) )
	{
		KeReleaseSpinLock( &m_spin, kIrql );
		return STATUS_UNSUCCESSFUL;
	}
	
	if( pMemVA->appAdr == 0xFFFFFFFF )
	{
		// It could be if the Function RemoveDevice() was called
		KeReleaseSpinLock( &m_spin, kIrql );
		return STATUS_WAIT_1;
	}
	
	//
	// Lock pMemBlock with emptyMemVA
	//
	emptyMemVA.appAdr = 0xFFFFFFFF;
	emptyMemVA.pFileObject = NULL;
	InsertTailList( &(pMemBlock->vaList), &(emptyMemVA.dlList) );

	//
	// Unkeep TMemVA
	//
	RemoveEntryList( &(pMemVA->dlList) );
	
	KeReleaseSpinLock( &m_spin, kIrql );


//	KdPrint((".....:  TMemAlloc::Free()  Free VA\n"
//			 ".....:         appAdr=0x%X, phyAdr=0x%X, sysAdr=0x%X, size=0x%X, pMdl=0x%X, \n", 
//		appAdr, pMemBlock->phyAdr, pMemBlock->sysAdr, pMemBlock->bytes, pMemBlock->pMdl ));

	//
	// Free VA, if the Function RemoveDevice() wasn't called
	//
	if( pFileObject != NULL )
		MmUnmapLockedPages( (PVOID)pMemVA->appAdr, pMemBlock->pMdl );

	MEMALLOC_ExFreeToXxxLookasideList( &m_lalVA, pMemVA );
	

	//
	// Unkeep TMemVA
	//
	sMemBlock = *pMemBlock;

	KeAcquireSpinLock( &m_spin, &kIrql );
	RemoveEntryList( &(emptyMemVA.dlList) );
	KeReleaseSpinLock( &m_spin, kIrql );

	//
	// Unkeep and Free TMemBlock if Need
	//
	if( 0<=UnkeepBlock( pMemBlock ) )
	{

		PHYSICAL_ADDRESS	AddressToMap = {0};

		//
		// Free Block
		//
		IoFreeMdl(sMemBlock.pMdl);

		AddressToMap.LowPart = sMemBlock.phyAdr;
		HalFreeCommonBuffer( m_pAdapter, sMemBlock.bytes, AddressToMap, (PVOID)sMemBlock.sysAdr, FALSE );
		KdPrint((".....:  TMemAlloc::Free()  Free MemBlock\n"));
		
		if( pIsBlockFree ) 
			*pIsBlockFree = 1;
	}

	return STATUS_SUCCESS;
}

//=*************** TMemAlloc::CloseHandle *********************
//=********************************************************
NTSTATUS TMemAlloc::CloseHandle( PFILE_OBJECT pFileObject )
{
	NTSTATUS			status;

	//
	// Check DMA Adapter
	//
	if( m_pAdapter == NULL )
	{
		KdPrint((".....:  TMemAlloc::FreeAll() Bad DMA Adapter\n"));
		return STATUS_UNSUCCESSFUL;
	}

	//
	// Free All MemBlocks with pFileObject
	//
	for( ;; )
	{
		status = Free( pFileObject, 0 );
		if( !NT_SUCCESS(status) ) 
		{
			break;
		}
	}
	return STATUS_SUCCESS;
}

//=*************** TMemAlloc::RemoveDevice *******************
//=********************************************************
NTSTATUS TMemAlloc::RemoveDevice( void )
{
	//
	// Check DMA Adapter
	//
	if( m_pAdapter == NULL )
	{
		KdPrint((".....:  TMemAlloc::FreeTotal() Bad DMA Adapter\n"));
		return STATUS_UNSUCCESSFUL;
	}

	//
	// Free All MemBlocks with pFileObject==NULL
	//
	CloseHandle( NULL );

	TDmaAdapter::RemoveDevice();

	return STATUS_SUCCESS;
}

//=*************** TMemAlloc::KeepBlockAndVA *******************
//=********************************************************
int TMemAlloc::KeepBlockAndVA( PFILE_OBJECT pFileObject, ULONG appAdr, TMemBlock *aMemBlock, 
					TMemBlock **ppMemBlock, TMemVA **ppMemVA )
{
	TMemBlock			*pMemBlock;
	TMemVA				*pMemVA;
	KIRQL				kIrql;

	//
	// Allocate TMemBlock
	//
	pMemBlock = (TMemBlock*)MEMALLOC_ExAllocateFromXxxLookasideList( &m_lalBlock );
	if( pMemBlock == NULL )
		return -1;

	InitializeListHead( &(pMemBlock->vaList) );
	pMemBlock->bytes  = aMemBlock->bytes;
	pMemBlock->phyAdr = aMemBlock->phyAdr;
	pMemBlock->sysAdr = aMemBlock->sysAdr;
	pMemBlock->pMdl   = aMemBlock->pMdl;

	//
	// Allocate TMemVA
	//
	pMemVA = (TMemVA*)MEMALLOC_ExAllocateFromXxxLookasideList( &m_lalVA );
	if( pMemVA == NULL )
	{
		MEMALLOC_ExFreeToXxxLookasideList( &m_lalBlock, pMemBlock );
		return -1;
	}

	pMemVA->appAdr = appAdr;
	pMemVA->pFileObject = pFileObject;

	//
	// Put TMemVA into TMemBlock's List
	//
	InsertTailList( &(pMemBlock->vaList), &(pMemVA->dlList) );

	//
	// Put TMemBlock into Main List
	//
	KeAcquireSpinLock( &m_spin, &kIrql );
	InsertTailList( &m_mainList, &(pMemBlock->dlList) );
	KeReleaseSpinLock( &m_spin, kIrql );

	//
	// Return Pointers
	//
	*ppMemBlock = pMemBlock;
	*ppMemVA    = pMemVA;

	return 0;
}

//=*************** TMemAlloc::KeepEmptyBlock *******************
//=********************************************************
/*
int TMemAlloc::KeepEmptyBlock( TMemBlock **ppMemBlock, TMemVA **ppMemVA )
{
	TMemBlock			*pMemBlock;
	TMemVA				*pMemVA;
	KIRQL				kIrql;

	//
	// Allocate TMemBlock
	//
	pMemBlock = (TMemBlock*)MEMALLOC_ExAllocateFromXxxLookasideList( &m_lalBlock );
	if( pMemBlock == NULL )
		return -1;

	InitializeListHead( &(pMemBlock->vaList) );
	pMemBlock->bytes = 0;
	pMemBlock->phyAdr = 0;
	pMemBlock->sysAdr = 0;
	pMemBlock->pMdl = NULL;

	//
	// Allocate TMemVA
	//
	pMemVA = (TMemVA*)MEMALLOC_ExAllocateFromXxxLookasideList( &m_lalVA );
	if( pMemVA == NULL )
	{
		MEMALLOC_ExFreeToXxxLookasideList( &m_lalBlock, pMemBlock );
		return -1;
	}

	pMemVA->appAdr = 0xFFFFFFFF;
	pMemVA->pFileObject = NULL;

	//
	// Put TMemVA into TMemBlock's List
	//
	InsertTailList( &(pMemBlock->vaList), &(pMemVA->dlList) );

	//
	// Put TMemBlock into Main List
	//
	KeAcquireSpinLock( &m_spin, &kIrql );
	InsertTailList( &m_mainList, &(pMemBlock->dlList) );
	KeReleaseSpinLock( &m_spin, kIrql );

	//
	// Return Pointers
	//
	*ppMemBlock = pMemBlock;
	*ppMemVA    = pMemVA;

	return 0;
}
*/
//=*************** TMemAlloc::KeepEmptyVA **********************
//=********************************************************
/*
int TMemAlloc::KeepEmptyVA( PFILE_OBJECT pFileObject, TMemBlock *pMemBlock, TMemVA **ppMemVA )
{
	TMemVA				*pMemVA;
	KIRQL				kIrql;
	int					ret = -1;

	//
	// Allocate TMemVA
	//
	pMemVA = (TMemVA*)MEMALLOC_ExAllocateFromXxxLookasideList( &m_lalVA );
	if( pMemVA == NULL )
		return -1;

	pMemVA->appAdr = 0xFFFFFFFF;
	pMemVA->pFileObject = NULL;

	//
	// Put pMemVA into List
	//
	KeAcquireSpinLock( &m_spin, &kIrql );

	if( IsValidBlock( pMemBlock ) )
	{
		InsertTailList( &(pMemBlock->vaList), &(pMemVA->dlList) );
		ret = 0;
	}
	KeReleaseSpinLock( &m_spin, kIrql );

	if( ret!=0 )
		MEMALLOC_ExFreeToXxxLookasideList( &m_lalVA, pMemVA );

	*ppMemVA = pMemVA;

	return ret;
}
*/

//=*************** TMemAlloc::KeepVA **********************
//=********************************************************
/*
int TMemAlloc::KeepVA( PFILE_OBJECT pFileObject, TMemBlock *pMemBlock, ULONG appAdr )
{
	TMemVA				*pMemVA;
	KIRQL				kIrql;
	int					ret = -1;

	//
	// Allocate TMemVA
	//
	pMemVA = (TMemVA*)MEMALLOC_ExAllocateFromXxxLookasideList( &m_lalVA );
	if( pMemVA == NULL )
		return -1;

	pMemVA->appAdr = appAdr;
	pMemVA->pFileObject = pFileObject;

	//
	// Put pMemVA into List
	//
	KeAcquireSpinLock( &m_spin, &kIrql );

	if( IsValidBlock( pMemBlock ) )
	{
		InsertTailList( &(pMemBlock->vaList), &(pMemVA->dlList) );
		ret = 0;
	}
	KeReleaseSpinLock( &m_spin, kIrql );

	if( ret!=0 )
		MEMALLOC_ExFreeToXxxLookasideList( &m_lalVA, pMemVA );

	return ret;
}
*/

//=*************** TMemAlloc::UnkeepBlock *****************
//=********************************************************
int TMemAlloc::UnkeepBlock( TMemBlock *pMemBlock )
{
	KIRQL				kIrql;

	KeAcquireSpinLock( &m_spin, &kIrql );
	if( !IsValidBlock( pMemBlock ) )
	{
		KeReleaseSpinLock( &m_spin, kIrql );
		return -1;
	}

	if( !IsListEmpty(&(pMemBlock->vaList)) )
	{
		KeReleaseSpinLock( &m_spin, kIrql );
		return -1;
	}

	RemoveEntryList( &(pMemBlock->dlList) );

	KeReleaseSpinLock( &m_spin, kIrql );
	
	MEMALLOC_ExFreeToXxxLookasideList( &m_lalBlock, pMemBlock );

	return 0;
}

//=*************** TMemAlloc::UnkeepVA *********************
//=********************************************************
/*
int TMemAlloc::UnkeepVA( PFILE_OBJECT pFileObject, ULONG appAdr, TMemBlock **ppMemBlock, ULONG *pAppAdr )
{
	TMemBlock			*pMemBlock;
	TMemVA				*pMemVA;
	KIRQL				kIrql;

	//
	// Find TMemVA into TMemBlock List
	//
	KeAcquireSpinLock( &m_spin, &kIrql );
	if( 0>FindBlock( pFileObject, appAdr, &pMemBlock, &pMemVA ) )
	{
		KeReleaseSpinLock( &m_spin, kIrql );
		return -1;
	}

	//
	// Remove TMemVA
	//
	RemoveEntryList( &(pMemVA->dlList) );
	KeReleaseSpinLock( &m_spin, kIrql );

	//
	// Return
	//
	*ppMemBlock = pMemBlock;
	*pAppAdr = pMemVA->appAdr;

	MEMALLOC_ExFreeToXxxLookasideList( &m_lalVA, pMemVA );

	return 0;
}
*/

//=*************** TMemAlloc::UnkeepVA *********************
//=********************************************************
int TMemAlloc::UnkeepVA( TMemBlock *pMemBlock, TMemVA *pMemVA )
{
	KIRQL				kIrql;

	KeAcquireSpinLock( &m_spin, &kIrql );
	if( !IsValidBlock( pMemBlock ) )
	{
		KeReleaseSpinLock( &m_spin, kIrql );
		return -1;
	}
	RemoveEntryList( &(pMemVA->dlList) );
	KeReleaseSpinLock( &m_spin, kIrql );

	MEMALLOC_ExFreeToXxxLookasideList( &m_lalVA, pMemVA );

	return 0;
}

//=*************** TMemAlloc::FindBlock *******************
//=********************************************************
int		TMemAlloc::FindBlock( PFILE_OBJECT pFileObject, ULONG appAdr, TMemBlock **ppMemBlock, TMemVA **ppMemVA )
{
	TMemBlock			*pMemBlock;
	TMemVA				*pMemVA;
	PLIST_ENTRY			pListBlock;
	PLIST_ENTRY			pListVA;

	//
	// Go through main List
	//
	pListBlock = m_mainList.Flink;
	while( pListBlock != &m_mainList )
	{
		pMemBlock = (TMemBlock*)pListBlock;
		//
		// Go through MemBlock List
		//

		pListVA = pMemBlock->vaList.Flink;
		while( pListVA != &(pMemBlock->vaList) )
		{
			pMemVA = (TMemVA*)pListVA;

			//
			// Compare pFileObject and appAdr
			//
			if( 
				( pFileObject==NULL ) ||
				( appAdr==0 && pMemVA->pFileObject==pFileObject ) ||
				( pMemVA->appAdr==appAdr && pMemVA->pFileObject==pFileObject )
			  )
			{
				*ppMemBlock = pMemBlock;
				if( ppMemVA )
					*ppMemVA = pMemVA;
				return 0;
			}
			pListVA  = pMemVA->dlList.Flink;
		}
		pListBlock  = pMemBlock->dlList.Flink;
	}

	*ppMemBlock = NULL;
	return -1;
}

//=*************** TMemAlloc::IsValidBlock *******************
//=********************************************************
int TMemAlloc::IsValidBlock( TMemBlock *pMemBlock )
{
	TMemBlock			*ptrMemBlock;
	PLIST_ENTRY			pListBlock;

	//
	// Go through main List
	//
	pListBlock = m_mainList.Flink;
	while( pListBlock != &m_mainList )
	{
		ptrMemBlock = (TMemBlock*)pListBlock;
		if( pMemBlock == ptrMemBlock )
			return 1;
		pListBlock  = ptrMemBlock->dlList.Flink;
	}

	return 0;
}

//
// End of File
//






