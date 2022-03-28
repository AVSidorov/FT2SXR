//=********************************************************
// DpramBox.cpp - Classes TDpramBox Definition
//
// (C) Copyright Ekkore 2002-2004
//
// Created:
//		16.06.2004 by Ekkore
// Modified:
//
//=********************************************************

#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/pagemap.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <asm/io.h>

#include "plx9080.h"
#include "dprambox.h"


//=********************************************************
//=********************************************************
//=********************************************************
//
// class TDpramBox
//
//
// HOST_MAIN Block Acesses
//

u32 ReadMainBlock32(TDpramBox *box, u32 regNum)
{
	return readl((u32*)((u8*)box->m_pciMainBlockAddress + regNum*4 ));
}

void WriteMainBlock32(TDpramBox *box, u32 regNum, u32 value)
{
	writel(value, (u32*)((u8*)box->m_pciMainBlockAddress + regNum*4 ));
}

TDpramBox *TDpramBoxCreate( TCaptureList *pCaptList,
			  void *pciPlxAddress,
			  void *pciMainBlockAddress,
			  u32 locMainBlockAddress )
{
	TDpramBox *dpr = kmalloc( sizeof(TDpramBox), GFP_KERNEL );
	if(!dpr)
		return NULL;

        printk ( "<0>%s()\n", __FUNCTION__ );

	memset((void*)dpr, 0, sizeof(TDpramBox));

	TDpramBoxInit(dpr, pCaptList, pciPlxAddress, pciMainBlockAddress, locMainBlockAddress);

	return dpr;
}

void TDpramBoxDelete( TDpramBox *dpr )
{
	if(dpr)
	    kfree(dpr);
}

//=*************** TDpramBox::Init ************************
//=********************************************************
void	TDpramBoxInit(  TDpramBox *dpr,
			TCaptureList *pCaptList,
			void *pciPlxAddress,
			void *pciMainBlockAddress,
			u32 locMainBlockAddress )
{
	dpr->m_pCaptList = pCaptList;
	dpr->m_pDpram = 0;
	dpr->m_nDpramSizeb = 0;

	dpr->m_pciPlxAddress       = pciPlxAddress;
	dpr->m_pciMainBlockAddress = pciMainBlockAddress;
	dpr->m_locMainBlockAddress = locMainBlockAddress;

	//
	// Define DPRAM Area
	//
	DefineDpramArea( dpr );
}

//=*************** TDpramBox::CloseHandle *****************
//=********************************************************
int	TDpramBoxCloseHandle( TDpramBox *dpr )
{
	return 0;
}

//=*************** TDpramBox::RemoveDevice ****************
//=********************************************************
int	TDpramBoxRemoveDevice( TDpramBox *dpr )
{
	return 0;
}

//=*************** TDpramBox::DefineDpramArea ****************
//=********************************************************
void	DefineDpramArea( TDpramBox *dpr )
{
    u32          nBlockId;
    u32          nDpramOffset;
    u32          nDpramSizeb;

    nBlockId     = 0xFFFF & ReadMainBlock32( dpr, HREG_BLOCK_ID );
    nDpramSizeb = 0xFFFF & ReadMainBlock32( dpr, HREG_DPRAM_SIZE );
    nDpramOffset = 0xFFFF & ReadMainBlock32( dpr, HREG_DPRAM_OFFSET );

    printk("<0>%s(): blockId=0x%X, nDpramSizeb=0x%X, nDpramOffset=0x%X\n", __FUNCTION__, nBlockId, nDpramSizeb, nDpramOffset );

    nBlockId     = 0xFFFF & ReadMainBlock32( dpr, HREG_BLOCK_ID );
    if( (0xFFF & nBlockId)!=0x001 ) {
            printk("<0> %s: blockId=0x%X\n", __FUNCTION__, nBlockId );
            return;
    }

    dpr->m_nDpramSizeb = 0xFFFF & ReadMainBlock32( dpr, HREG_DPRAM_SIZE );
    dpr->m_nDpramSizeb *= sizeof(u32);

    nDpramOffset = 0xFFFF & ReadMainBlock32( dpr, HREG_DPRAM_OFFSET );
    dpr->m_pDpram = (void*)((u8*)dpr->m_pciMainBlockAddress + (nDpramOffset*sizeof(u32)));
}

//=*************** TDpramBox::ReadWriteDpram **************
//=********************************************************
int ReadWriteDpram( TDpramBox *dpr, u32 nDpramOffset, void *pvBuf, u32 nSizeb, int isRead )
{
	if( dpr->m_nDpramSizeb==0 )
		DefineDpramArea( dpr );

	if( dpr->m_nDpramSizeb==0 )
		return -ENOMEM;

	if( dpr->m_nDpramSizeb < nDpramOffset+nSizeb )
		return -EINVAL;

	//
	// Read Dump
	//
	if( isRead )
	{
	    memcpy_fromio( pvBuf, (void*)(((u8*)dpr->m_pDpram) + nDpramOffset),  nSizeb );
	}

	//
	// Write Dump
	//
	else
	{
	    memcpy_toio( (void*)(((u8*)dpr->m_pDpram) + nDpramOffset), pvBuf, nSizeb );
	}

	return 0;
}

//=*************** TDpramBox::SemSet **********************
//=********************************************************
void SemSet( TDpramBox *dpr, u32 nSemId, int isOwn )
{
	u32		val;

	if( nSemId > SEMID0+7 )
		return;

	val = 1 << (nSemId+8);
	if( isOwn )
		val |= 1 << nSemId;

	WriteMainBlock32( dpr, HREG_SEM0, val );
}

//=*************** TDpramBox::SemCheck ********************
//=********************************************************
int	SemCheck( TDpramBox *dpr, u32 nSemId )
{
	u32		val;

	if( nSemId > SEMID0+7 )
		return -1;


	val = 0xFFFF & ReadMainBlock32( dpr, HREG_SEM0 );
	val >>= nSemId;
	val &= 0x1;

	return (int)val;
}

//=*************** TDpramBox::SemWait *********************
//=********************************************************
void SemWait( TDpramBox *dpr, u32 nSemId )
{
	if( nSemId > SEMID0+7 )
		return;

	for(;;)
	{
		SemSet( dpr, nSemId, SEMOWN );
		if( SEMOWN == SemCheck( dpr, SEMOWN ) )
			break;
	}
}

//=*************** TDpramBox::CaptureDma ******************
//=********************************************************
int	TDpramBoxCaptureDma( TDpramBox *dpr, u32 *pnDmaId, int *pPassword)
{
	u32		nDmaId;
	int		password;

	//
	// Try to Capture DMA0 or DMA1
	//
	nDmaId = 0;
	if( 0 == (password = Capture( dpr->m_pCaptList, "dma0", 0 ) ) )
	{
		nDmaId = 1;
		password = Capture( dpr->m_pCaptList, "dma1", 0 );
	}

	//
	// Return result
	//
	if( password==0 )
		return -1;

	*pnDmaId   = nDmaId;
	*pPassword = password;

	return 0;
}

//=*************** TDpramBox::ReleaseDma ******************
//=********************************************************
void TDpramBoxReleaseDma( TDpramBox *dpr, u32 nDmaId, int password)
{
	Release( dpr->m_pCaptList, (nDmaId==0) ? "dma0" : "dma1", password );
}

//
// End of File
//
