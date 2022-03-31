//=********************************************************
// IntStrm.cpp - Classes TInterCBuf, TInterStream9056,
//               TGetPhysAdrByMdl Definition
//
// (C) Copyright Ekkore 2002
//
// Created:
//              04.12.2002 by Ekkore
// Modified:
//              06.04.2004 by DOR (STREAM STOP)
//
//=********************************************************

#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <linux/slab.h>

#include "pagehelp.h"
#include "intstrm.h"

//=********************************************************
//=********************************************************
//=********************************************************
//
// class TInterCBuf
//

//=**************** TInterCBuf::Create *******************
//=********************************************************
TInterCBuf    *TInterCBufCreate ( struct device *pci )
{
    TInterCBuf    *pcbuf = kmalloc ( sizeof ( TInterCBuf ), GFP_KERNEL );

    memset ( pcbuf, 0, sizeof ( TInterCBuf ) );

    if ( !pcbuf )
        return NULL;

    pcbuf->m_blkNum = 0;
    pcbuf->m_blkNum = 0;
    pcbuf->m_blkSize = 0;
    pcbuf->m_lastBlock = 0;
    pcbuf->m_arrBlock = NULL;
    pcbuf->m_stub.phyAdr = 0;
    pcbuf->m_stub.sysAdr = NULL;
    pcbuf->m_pci = pci;
    pcbuf->m_isAlloc = 0;
    pcbuf->m_IR = TInterRuptorCreate( );
    atomic_set ( &pcbuf->m_isOperationInUse, 0 );

    return pcbuf;
}

//=**************** TInterCBuf::Delete *******************
//=********************************************************
void TInterCBufDelete ( TInterCBuf * pcbuf )
{
    if(!pcbuf)
        return;

    TInterRuptorDelete(pcbuf->m_IR);

    kfree(pcbuf);
}

//=**************** TInterCBuf::Init *******************
//=********************************************************
void TInterCBufInit ( TInterCBuf * pcbuf, struct device *pci )
{
    pcbuf->m_blkNum = 0;
    pcbuf->m_blkNum = 0;
    pcbuf->m_blkSize = 0;
    pcbuf->m_lastBlock = 0;
    pcbuf->m_arrBlock = NULL;
    pcbuf->m_stub.phyAdr = 0;
    pcbuf->m_stub.sysAdr = NULL;
    pcbuf->m_pci = pci;
    pcbuf->m_isAlloc = 0;

    if( !pcbuf->m_IR )
        pcbuf->m_IR = TInterRuptorCreate( );

    atomic_set ( &pcbuf->m_isOperationInUse, 0 );
}

//=**************** TInterCBuf::Alloc *******************
//=********************************************************
int TInterCBufAlloc ( TInterCBuf * pcbuf, size_t blkNum, size_t blkSize, size_t **pStub,
                      size_t * pBlk[] )
{
    int            ii;
    //dma_addr_t     pa;

    printk ( "<0>%s()\n", __FUNCTION__ );

    //
    // Check if Block Operation in Use
    //
    if ( 0 != atomic_inc_and_test ( &pcbuf->m_isOperationInUse ) )
    {
        printk ( "<0>  Unlock() Block Operation in Use\n" );
        return -EBUSY;
    }

    //
    // Check if CBuf has already been Locked/Allocated
    //
    if ( pcbuf->m_blkNum != 0 )
    {
        printk ( "<0>  Lock() CBuf has already Allocated\n" );
        atomic_set ( &pcbuf->m_isOperationInUse, 0 );
        return -EINVAL;
    }

    //
    // Prepare arrBlock[]
    //
    pcbuf->m_arrBlock = (TBlockAdrInfo*)dma_alloc_coherent(pcbuf->m_pci, blkNum*sizeof(TBlockAdrInfo), &pcbuf->m_arrBlockPA, GFP_KERNEL);

    if ( pcbuf->m_arrBlock == NULL )
    {
        printk ( "<0> Alloc() dma_alloc_consistent for 'arrBlock[]' failed\n" );
        TInterCBufDisallocOnly ( pcbuf );
        atomic_set ( &pcbuf->m_isOperationInUse, 0 );
        return -ENOMEM;
    }

    for ( ii = 0; ii < ( int ) blkNum; ii++ )
    {
        pcbuf->m_arrBlock[ii].sysAdr = NULL;
        pcbuf->m_arrBlock[ii].phyAdr = 0;
    }

    //
    // Init Member Variables
    //
    pcbuf->m_blkNum = blkNum;
    pcbuf->m_blkSize = blkSize;
    pcbuf->m_lastBlock = 0;
    pcbuf->m_isAlloc = 1;

    //
    // Allocate All Blocks
    //
    for ( ii = 0; ii < ( int ) blkNum; ii++ )
    {
        pcbuf->m_arrBlock[ii].sysAdr = dma_alloc_coherent(  pcbuf->m_pci,
                                                            pcbuf->m_blkSize,
                                                           &pcbuf->m_arrBlock[ii].phyAdr, GFP_KERNEL );
        if ( pcbuf->m_arrBlock[ii].sysAdr == NULL )
        {
            printk ( "<0>  Alloc() failed to allocate block=%d\n", ii );
            TInterCBufDisallocOnly ( pcbuf );
            atomic_set ( &pcbuf->m_isOperationInUse, 0 );
            return -ENOMEM;
        }

        //printk ( "<0>%s(): %d - %p\n", __FUNCTION__, ii, (void*)pcbuf->m_arrBlock[ii].phyAdr );

        //try to lock all physical pages in the current block
        lock_pages ( pcbuf->m_arrBlock[ii].sysAdr, pcbuf->m_blkSize );
    }

    pcbuf->m_stub.sysAdr = dma_alloc_coherent( pcbuf->m_pci, sizeof(TStreamStub),&pcbuf->m_stub.phyAdr,GFP_KERNEL );
    if ( pcbuf->m_stub.sysAdr == NULL )
    {
        printk ( "<0>  Alloc() failed to create MDL for Stub\n" );
        TInterCBufDisallocOnly( pcbuf );
        atomic_set ( &pcbuf->m_isOperationInUse, 0 );
        return -ENOMEM;
    }

    //printk ( "<0>%s(): Stub - %p\n", __FUNCTION__, (void*)pcbuf->m_stub.phyAdr );

    //try to lock all physical pages in the stub
    lock_pages ( pcbuf->m_stub.sysAdr, PAGE_SIZE );

    //
    // Return
    //
    for(ii = 0; ii < (int)blkNum; ii++)
        pBlk[ii] = (size_t*)((size_t)pcbuf->m_arrBlock[ii].phyAdr);

    pStub[0] = (size_t*)((size_t)pcbuf->m_stub.phyAdr);

    atomic_set ( &pcbuf->m_isOperationInUse, 0 );

    return 0;
}

//=**************** TInterCBuf::Disalloc ******************
//=********************************************************
int TInterCBufDisalloc ( TInterCBuf * pcbuf )
{
    int            status;

    printk("<0>%s()\n", __FUNCTION__);

    //
    // Check if Block Operation in Use
    //
    if ( 0 != atomic_inc_and_test ( &pcbuf->m_isOperationInUse ) )
    {
        printk ( "<0>  Unlock() Block Operation in Use\n" );
        return -EBUSY;
    }

    //
    // Check if CBUS was alocated, not locked
    //
    if ( 0 == pcbuf->m_isAlloc )
    {
        printk ( "<0>  Disalloc() CBuf was locked, not allocated\n" );
        atomic_set ( &pcbuf->m_isOperationInUse, 0 );
        return -EINVAL;
    }

    status = TInterCBufDisallocOnly ( pcbuf );

    atomic_set ( &pcbuf->m_isOperationInUse, 0 );

    return status;
}

//=**************** TInterCBuf::DisallocOnly **************
//=********************************************************
int TInterCBufDisallocOnly ( TInterCBuf * pcbuf )
{
    u32            ii;

    printk("<0>%s()\n", __FUNCTION__);
    
    //
    // If CBUF has been allocated
    //
    if ( ( pcbuf->m_blkNum != 0 ) && ( pcbuf->m_arrBlock != NULL ) )
    {
        //
        // Free Blocks
        //
        for ( ii = 0; ii < ( int ) pcbuf->m_blkNum; ii++ )
        {
            //unlock all physical pages in the current block
            unlock_pages ( pcbuf->m_arrBlock[ii].sysAdr, pcbuf->m_blkSize );

            dma_free_coherent (   pcbuf->m_pci, pcbuf->m_blkSize,
                                  pcbuf->m_arrBlock[ii].sysAdr,
                                  pcbuf->m_arrBlock[ii].phyAdr );
        }

        //
        // Free Stub
        //
        unlock_pages ( pcbuf->m_stub.sysAdr, PAGE_SIZE );

        dma_free_coherent ( pcbuf->m_pci, sizeof ( TStreamStub ),
                              pcbuf->m_stub.sysAdr, pcbuf->m_stub.phyAdr );

    }

    if ( (pcbuf->m_arrBlock != NULL)  && (pcbuf->m_arrBlockPA != 0) )
        dma_free_coherent (   pcbuf->m_pci,
                              pcbuf->m_blkNum * sizeof ( TBlockAdrInfo ),
                              pcbuf->m_arrBlock,
                              pcbuf->m_arrBlockPA );

    //
    // Zero all Variables
    //
    pcbuf->m_blkNum = 0;
    pcbuf->m_blkSize = 0;
    pcbuf->m_lastBlock = 0;
    pcbuf->m_arrBlock = NULL;
    pcbuf->m_stub.sysAdr = NULL;

    //printk("<0>  DisallocOnly() End\n" );

    return 0;
}

//=**************** TInterCBuf::Wait ********************
//=********************************************************
int TInterCBufWait ( TInterCBuf * pcbuf,
                     TStreamState * pSS, u32 * pTimeElapsed )
{
    int            status;

    status = WaitInterrupt ( pcbuf->m_IR, pSS->timeout, pTimeElapsed );
    if ( status == 0 )
    {
        pSS->lastBlock = pcbuf->m_lastBlock;
        pSS->totalCounter = atomic_read ( &pcbuf->m_IR->m_totalCnt );
        pSS->offset = 0;
        pSS->state = -1;
    }

    return status;
}

//=**************** TInterCBuf::Grab ********************
//=********************************************************
int TInterCBufGrab ( TInterCBuf * pcbuf, TStreamState * pSS,
                     u32 * pTimeElapsed )
{
    int            status;

    status = GrabInterrupt ( pcbuf->m_IR, pSS->timeout, pTimeElapsed );
    if ( status == 0 )
    {
        pSS->lastBlock = pcbuf->m_lastBlock;
        pSS->totalCounter = atomic_read ( &pcbuf->m_IR->m_totalCnt );
        pSS->offset = 0;
        pSS->state = -1;
    }

    return status;
}

//=**************** TInterCBuf::QueueDPC ****************
//=********************************************************
void TInterCBufQueueDPC ( TInterCBuf * pcbuf )
{
    //printk("<0>%s()\n", __FUNCTION__);

    TInterRuptorQueueDPC ( pcbuf->m_IR );

    if ( pcbuf->m_blkNum )
    {
        //
        // Increment Current Block Number
        //
        pcbuf->m_lastBlock++;
        if ( pcbuf->m_lastBlock >= pcbuf->m_blkNum )
            pcbuf->m_lastBlock = 0;

        //
        // Fill Stub
        //
        ((TStreamStub*)pcbuf->m_stub.sysAdr)->lastBlock = pcbuf->m_lastBlock;
        ((TStreamStub*)pcbuf->m_stub.sysAdr)->totalCounter = atomic_read(&pcbuf->m_IR->m_totalCnt);

        //printk("<0>%s(): Stub->lastBlok = %d\n", __FUNCTION__, ((TStreamStub*)pcbuf->m_stub.sysAdr)->lastBlock);
        //printk("<0>%s(): Stub->totalCounter = %d\n", __FUNCTION__, ((TStreamStub*)pcbuf->m_stub.sysAdr)->totalCounter);
    }
}

//=********************************************************
//=********************************************************
//=********************************************************
//
// class TInterStream9056
//

//=*********** TInterStream9056::TInterStream9056 *********
//=********************************************************
TInterStream9056 *TInterStream9056Create ( struct device *pci, //void *pDevice,
                                           void *pciPlxAddress,
                                           u32 locFifoBlockAddress,
                                           u32 dmaChan )
{
    TInterStream9056 *strm = kmalloc(sizeof(TInterStream9056), GFP_KERNEL);

    if ( !strm )
        return NULL;

    printk("<0>%s()\n", __FUNCTION__);

    memset((void*)strm, 0, sizeof(TInterStream9056));

    strm->m_op = &s_ops;
    strm->m_waitStates = 0;
    strm->m_dmaCodeStart = 0x21B03;
    strm->m_dmaCodeStop = 0x103;
    strm->m_Buffer = TInterCBufCreate (pci);
    strm->m_suspender = TStreamSuspenderCreate(0, 0);

    TStreamSuspenderStop(strm->m_suspender);
    TInterStream9056Init(strm, pciPlxAddress, locFifoBlockAddress, dmaChan);

    return strm;
}

//=*********** TInterStream9056::~TInterStream9056 ********
//=********************************************************
void TInterStream9056Delete ( TInterStream9056 * strm )
{
    if ( !strm )
        return;

    TInterCBufDelete( strm->m_Buffer );
    TStreamSuspenderDelete( strm->m_suspender );

    kfree ( strm );
}

//=*********** TInterStream9056::Init *********************
//=********************************************************
void TInterStream9056Init ( TInterStream9056 * strm,
                            void *pciPlxAddress, u32 locFifoBlockAddress,
                            u32 dmaChan )
{
    printk("<0>%s()\n", __FUNCTION__);

    strm->m_op = &s_ops;
    strm->m_pAddress = pciPlxAddress;
    strm->m_locAdr = locFifoBlockAddress;
    strm->m_dmaChan = dmaChan;
    strm->m_isCycle = 0;
    strm->m_arrChain = NULL;
    strm->m_chainSize = 0;
}

//=*********** TInterStream9056::Alloc *****************
//=********************************************************
int TInterStream9056Alloc ( TInterStream9056 * strm, size_t blkNum, size_t blkSize, size_t** pStub, size_t* pBlk[] )
{
    int status = 0;

    status = TInterCBufAlloc ( strm->m_Buffer, blkNum, blkSize, pStub, pBlk );

	return status;
}

//=*********** TInterStream9056::Disalloc *****************
//=********************************************************
int TInterStream9056Disalloc ( TInterStream9056 * strm )
{
    int            status;

    status = TInterStream9056Stop ( strm );
    status = TInterCBufDisalloc ( strm->m_Buffer );

    return status;
}

//=*********** TInterStream9056::Start ********************
//=********************************************************
int TInterStream9056Start ( TInterStream9056 * strm, u32 isCycle, u32 dir )
{
    int            status;
    u32            intcsr;
    TInterCBuf    *pBuf = strm->m_Buffer;
    TInterRuptor  *pIR = strm->m_Buffer->m_IR;

    printk("<0>%s()\n", __FUNCTION__);

    //
    // Create PLX Chain
    //
    strm->m_isCycle = isCycle;
    strm->m_dir = dir;
    status = TInterStream9056CreateChain( strm );
    if ( 0 < status )
    {
        return status;
    }

    //
    // Stop DMA and Clear Interrupt
    //
    strm->m_op->WritePlxMem08(strm->m_pAddress,(strm->m_dmaChan == 0) ? plxDMACSR0 : plxDMACSR1, 8);
    strm->m_op->WritePlxMem08(strm->m_pAddress,(strm->m_dmaChan == 0) ? plxDMACSR0 : plxDMACSR1, 0);

    //
    // Prepare Stub Data
    //
    pBuf->m_lastBlock = -1;
    atomic_set( &pIR->m_totalCnt, 0 );
    if( pBuf->m_stub.sysAdr )
    {
        ((TStreamStub*)pBuf->m_stub.sysAdr)->lastBlock = pBuf->m_lastBlock;
        ((TStreamStub*)pBuf->m_stub.sysAdr)->totalCounter = atomic_read ( &pIR->m_totalCnt );
        ((TStreamStub*)pBuf->m_stub.sysAdr)->offset = 0;
    }

    //
    // Init sincro event
    //
    TInterRuptorClearFlag ( pIR );

    TStreamSuspenderInit ( strm->m_suspender, pBuf->m_blkNum, pBuf->m_blkSize );
    TStreamSuspenderStart ( strm->m_suspender );

    //
    // Enable DMA Channel 0/1 Interrupt
    //
    intcsr = strm->m_op->ReadPlxMem32(strm->m_pAddress, plxINTCSR);
    intcsr |= ( strm->m_dmaChan == 0 ) ? 0x40100 : 0x80100;
    strm->m_op->WritePlxMem32(strm->m_pAddress, plxINTCSR, intcsr);


    //
    // Prepare DMA
    //
    strm->m_op->WritePlxMem08(strm->m_pAddress,(strm->m_dmaChan==0) ? plxDMACSR0 : plxDMACSR1, 0);

    strm->m_op->WritePlxMem32(strm->m_pAddress,(strm->m_dmaChan == 0) ? plxDMAMODE0 : plxDMAMODE1,
							  strm->m_dmaCodeStart | ((strm->m_waitStates & 0xF) << 2));

    printk("<0>%s(): m_dmaCodeStart = 0x%X\n", __FUNCTION__, strm->m_dmaCodeStart);
    printk("<0>%s(): DMAPADR = 0x%X\n", __FUNCTION__, strm->m_arrChain[strm->m_chainSize-1].padr);
    printk("<0>%s(): DMALADR = 0x%X\n", __FUNCTION__, strm->m_arrChain[strm->m_chainSize-1].ladr);
    printk("<0>%s(): DMASIZE = 0x%X\n", __FUNCTION__, 0);
    printk("<0>%s(): DMADPR = 0x%X\n", __FUNCTION__, strm->m_arrChain[strm->m_chainSize-1].dpr);

    strm->m_op->WritePlxMem32(strm->m_pAddress,(strm->m_dmaChan == 0) ? plxDMAPADR0 : plxDMAPADR1,
							  strm->m_arrChain[strm->m_chainSize-1].padr );

    strm->m_op->WritePlxMem32(strm->m_pAddress,(strm->m_dmaChan == 0) ? plxDMALADR0 : plxDMALADR1,
							  strm->m_arrChain[strm->m_chainSize-1].ladr );

    strm->m_op->WritePlxMem32(strm->m_pAddress,(strm->m_dmaChan == 0) ? plxDMASIZ0 : plxDMASIZ1, 0);

    //
    //Записываем в регистр DMADPR значение последнего элемента в цепочке, чтобы предача началась с первого блока.
    //
    strm->m_op->WritePlxMem32(strm->m_pAddress,(strm->m_dmaChan == 0 ) ? plxDMADPR0 : plxDMADPR1,
							  strm->m_arrChain[strm->m_chainSize-1].dpr);

    //
    // If Single Buf
    //
    if ( !strm->m_isCycle )
        strm->m_arrChain[strm->m_chainSize-1].dpr |= 0x2;

    //
    // Start DMA
    //
    strm->m_op->WritePlxMem08(strm->m_pAddress,(strm->m_dmaChan == 0) ? plxDMACSR0 : plxDMACSR1, 3 );

    //#warning !!!!!!!!!!!!!!!!!!!!!!!
    //
    // Write Stub.state
    //
    //( ( TStreamStub * ) pBuf->m_stub.sysAdr )->state = 1;

    asm ( "nop" );

    return 0;
}

//=*********** TInterStream9056::Stop *********************
//=********************************************************
int TInterStream9056Stop ( TInterStream9056 * strm )
{
    u32            intcsr;
    TInterCBuf    *pBuf = strm->m_Buffer;

    printk("<0>%s()\n", __FUNCTION__);

    //
    // If STREAM hasn't been started yet
    //
    if ( strm->m_arrChain == NULL )
    {
        TStreamSuspenderStop ( strm->m_suspender );
        return -EINVAL;
    }

    //
    // Disable DMA Interrupt
    //
    intcsr = strm->m_op->ReadPlxMem32 ( strm->m_pAddress, plxINTCSR );
    intcsr &= ( strm->m_dmaChan == 0 ) ? ~0x40000 : ~0x80000;
    strm->m_op->WritePlxMem32 ( strm->m_pAddress, plxINTCSR, intcsr );

    //
    // Abort DMA
    //
    {
        int            nNopCnt;
        int            nAbortCnt;

        strm->m_op->WritePlxMem08 ( strm->m_pAddress, ( strm->m_dmaChan == 0 ) ? plxDMACSR0 : plxDMACSR1, 0x5 );    // ABORT DMA

        for ( nNopCnt = 0; nNopCnt < 1000; nNopCnt++ )
            asm ( "nop" );

        //
        // Wait for end DMA
        //
        nAbortCnt = 0;
        while ( ( strm->m_op->ReadPlxMem08( strm->m_pAddress, ( strm->m_dmaChan == 0 ) ? plxDMACSR0 : plxDMACSR1 ) & 0x10 ) == 0 )
        {
            strm->m_op->WritePlxMem08 ( strm->m_pAddress, ( strm->m_dmaChan == 0 ) ? plxDMACSR0 : plxDMACSR1, 0x5 );        // ABORT DMA Again
            for ( nNopCnt = 0; nNopCnt < 1000; nNopCnt++ )
            {
                asm ( "nop" );
                asm ( "nop" );
                asm ( "nop" );
                asm ( "nop" );
                asm ( "nop" );
            }

            if ( nAbortCnt++ == 1000000 )
            {
                ( ( TStreamStub * ) pBuf->m_stub.sysAdr )->state = -10;
						 printk ( "<0> %s ABORT COUNTER TOO LARGE chan=%d, csr=0x%X\n",
                         __FUNCTION__,
                         strm->m_dmaChan,
                         strm->m_op->ReadPlxMem08 ( strm->m_pAddress, ( strm->m_dmaChan == 0 ) ? plxDMACSR0 : plxDMACSR1 ) );
                break;
            }
        }
        //printk ( "<0> %s: ABORT COUNTER = %d\n", __FUNCTION__, nAbortCnt );
    }

    //
    // Stop DMA and Clear Interrupt
    //
    strm->m_op->WritePlxMem08 ( strm->m_pAddress,
                    ( strm->m_dmaChan == 0 ) ? plxDMACSR0 : plxDMACSR1, 8 );
    strm->m_op->WritePlxMem08 ( strm->m_pAddress,
                    ( strm->m_dmaChan == 0 ) ? plxDMACSR0 : plxDMACSR1, 0 );


    //
    // DisAllocate Chain
    //
    TInterStream9056FreeChain ( strm );

    //
    // Write Stub.state
    //
    ( ( TStreamStub * ) pBuf->m_stub.sysAdr )->state = 2;

    TStreamSuspenderStop ( strm->m_suspender );

    return 0;
}

#define		WAIT_ABANDONED		22
#define		STREAM_SUSPENDED	23

//=**************** TInterStream9056::Wait ****************
//=********************************************************
int TInterStream9056Wait ( TInterStream9056 * strm, TStreamState * pSS,
                           u32 * pTimeElapsed )
{
    int            status = 0;
    TInterCBuf    *pBuf = strm->m_Buffer;
    TInterRuptor  *pIR = strm->m_Buffer->m_IR;

    //printk ( "<0> %s: Wait() m_isCycle=%d, m_totalCnt=0x%X\n", __FUNCTION__,
    //         strm->m_isCycle, atomic_read ( &pIR->m_totalCnt ) );

    if ( ( strm->m_isCycle == 0 )
         && ( atomic_read ( &pIR->m_totalCnt ) >= pBuf->m_blkNum ) )
    {
        // End of One Cycle Mode
        // Return immediately
        pSS->timeout = WAIT_ABANDONED;
        //printk ( "<0> %s: WAIT_ABANDONED\n", __FUNCTION__ );
    }
    else if ( strm->m_arrChain == NULL )
    {
        // Stream wasn't Started
        // Return immediately
        pSS->timeout = WAIT_ABANDONED;
        //printk ( "<0> %s: WAIT_ABANDONED\n", __FUNCTION__ );
    }
    else if ( TStreamSuspenderIsSuspend ( strm->m_suspender )
              && pSS->timeout == 0xFFFFFFFF )
    {
        // Stream is Supended
        // Return immediately
        pSS->timeout = STREAM_SUSPENDED;
        //printk ( "<0> %s: STREAM_SUSPENDED\n", __FUNCTION__ );
    }
    else if ( pSS->timeout == 0x0 )
    {
        // Don't wait for the End of Block
        // Return immediately
        //printk ( "<0> %s: Return immediately\n", __FUNCTION__ );
    }
    else
    {
        status = WaitInterrupt ( pIR, pSS->timeout, pTimeElapsed );
    }

    if ( ( 0 > status ) && TStreamSuspenderIsSuspend ( strm->m_suspender ) )
    {
        pSS->timeout = STREAM_SUSPENDED;
        status = 0;
        //printk ( "<0> %s: STREAM_SUSPENDED\n", __FUNCTION__ );
    }

    if ( status == 0 )
    {
        pSS->lastBlock = pBuf->m_lastBlock;
        pSS->totalCounter = GetCounter ( pIR );
        pSS->offset = 0;
        pSS->state = -1;
        //
        // If Stream has been stoped
        //
        if ( strm->m_arrChain == NULL )
        {
            //printk ( "<0> %s: WAIT_ABANDONED\n", __FUNCTION__ );
            pSS->timeout = WAIT_ABANDONED;
        }
    }

    return status;
}

//=******************** DoneSyncronize ********************
//=********************************************************
int DoneSyncronize ( void *pSynchronizeContext )
{
    TInterStream9056 *pStream = ( TInterStream9056 * ) pSynchronizeContext;
    int            suspState;

    suspState =
        TStreamSuspenderDone ( pStream->m_suspender, pStream->m_keeBlkDone );

    //printk ( "<0> %s: blkNo=%2d, mark=%d\n", __FUNCTION__,
    //         pStream->m_keeBlkDone, pStream->m_suspender->m_blkMark );
    if ( suspState == SUSPENDR_SUSPEND )
    {
        TInterStream9056Suspend ( pStream );
        //printk ( " ----- SUSPEND\n" );
    }

    if ( suspState == SUSPENDR_RESUME )
    {
        TInterStream9056Resume ( pStream );
        //printk ( ( " +++++ RESUME\n" ) );
    }

    //printk ( "\n" );

    return 1;
}

//=**************** TInterStream9056::Done **TStreamSuspenderDone****************
//=********************************************************
int TInterStream9056Done ( TInterStream9056 * strm, int blkNo )
{
    strm->m_keeBlkDone = blkNo;

    DoneSyncronize ( ( void * ) strm );

    return 0;
}

//=******************** AdjustSyncronize ********************
//=********************************************************
int AdjustSyncronize ( void *pSynchronizeContext )
{
    TInterStream9056 *pStream = ( TInterStream9056 * ) pSynchronizeContext;
    int            suspState;

    suspState =
        TStreamSuspenderAdjust ( pStream->m_suspender, pStream->m_keeIsAdjust );
    //printk ( "<0> on=%2d, mark=%d\n", pStream->m_suspender->m_on,
    //         pStream->m_suspender->m_blkMark );
    if ( suspState == SUSPENDR_RESUME )
    {
        TInterStream9056Resume ( pStream );
        //printk ( " +++++ RESUME\n" );
    }

    //printk ( "\n" );
    return 1;
}

//=**************** TInterStream9056::Adjust ******************
//=********************************************************
int TInterStream9056Adjust ( TInterStream9056 * strm, int isAdjust )
{
    strm->m_keeIsAdjust = isAdjust;

    AdjustSyncronize ( ( void * ) strm );

    return 0;
}

//=*********** TInterStream9056::Isr **********************
//=********************************************************
int TInterStream9056Isr ( TInterStream9056 * strm )
{
    u32            intcsr = 0;
    u32            intmask = ( strm->m_dmaChan == 0 ) ? 0x200000 : 0x400000;
    u32            dmacsr = 0;
    TInterCBuf    *pBuf = strm->m_Buffer;

    //printk("<0>%s(): STRM %d\n", __FUNCTION__, strm->m_dmaChan );

    //
    // Check if it is my DMA Interrupt
    //
    intcsr = strm->m_op->ReadPlxMem32 ( strm->m_pAddress, plxINTCSR );
    if ( !(intcsr & intmask) )
    {
    	//printk("<0>%s(): INVALID INTERRUPT\n", __FUNCTION__ );
        return 0;               // No, it isn't mine
    }

    //
    // Turn off DMA Interrupt
    //
    dmacsr = strm->m_op->ReadPlxMem08 ( strm->m_pAddress,
                       ( strm->m_dmaChan == 0 ) ? plxDMACSR0 : plxDMACSR1 );
    strm->m_op->WritePlxMem08 ( strm->m_pAddress,
                    ( strm->m_dmaChan == 0 ) ? plxDMACSR0 : plxDMACSR1,
                    ( dmacsr | 0x8 ) );

    if ( SUSPENDR_SUSPEND ==
         TStreamSuspenderEOT ( strm->m_suspender, pBuf->m_lastBlock ) )
    {
        TInterStream9056Suspend ( strm );
    }

    //
    // Stack DPC
    //
    TInterCBufQueueDPC ( pBuf );

    return 1;
}

//--------------------------------------------------------------------

//=*********** TInterStream9056::CreateChain **************
//=********************************************************
int TInterStream9056CreateChain ( TInterStream9056 * strm )
{
    u32            iiBlk;
    u32            sizeofChain;
    int            i = 0;
    TInterCBuf    *pcbuf = strm->m_Buffer;

    printk("<0>%s()\n", __FUNCTION__);
    //
    // Calculate Size of Chain
    //
    sizeofChain = sizeof ( TPlxChainLink ) * pcbuf->m_blkNum;

    //
    // Allocate Chain
    //
    strm->m_arrChain = (TPlxChainLink*)dma_alloc_coherent( pcbuf->m_pci, sizeofChain, &strm->m_arrChainPA, GFP_KERNEL );
    if ( strm->m_arrChain == NULL )
    {
        printk( "<0>%s(): dma_alloc_consistent for 'm_arrChain[%ld]' failed\n", __FUNCTION__, sizeofChain / sizeof ( TPlxChainLink ) );
        return -ENOMEM;
    }

    //
    // Initialize Chain
    //
    for ( iiBlk = 0; iiBlk < pcbuf->m_blkNum; iiBlk++ )
    {
        strm->m_arrChain[iiBlk].padr = pcbuf->m_arrBlock[iiBlk].phyAdr;
        strm->m_arrChain[iiBlk].ladr = strm->m_locAdr;
        strm->m_arrChain[iiBlk].siz = pcbuf->m_blkSize;
    }

    //
    // Initialize DPR pointer in the Chain
    //
    for( i = 0; i < pcbuf->m_blkNum-1; i++ )
    {
        //printk("<0>%s(): %d: PA = %p VTB = %p", __FUNCTION__, i,
        //	((u8*)strm->m_arrChainPA + (i+1)*sizeof(TPlxChainLink)),
        //	(void*)virt_to_bus(&strm->m_arrChain[i+1]) );

        strm->m_arrChain[i].dpr = (u32)(strm->m_arrChainPA + (i+1)*sizeof(TPlxChainLink));
        strm->m_arrChain[i].dpr |= 0x5;
        strm->m_arrChain[i].dpr |= (strm->m_dir == 1) ? 0x8 : 0;      //1-to HOST, 2-from HOST
    }

    //
    // Point Last Chain to First Chain (real)
    //
    //printk("<0>%s(): %d: PA = %p VTB = %p", __FUNCTION__, i,
    //		((u8*)strm->m_arrChainPA + i*sizeof(TPlxChainLink)),
    //		(void*)virt_to_bus(&strm->m_arrChain[0]) );

    strm->m_arrChain[i].dpr = (u32)(strm->m_arrChainPA);
    strm->m_arrChain[i].dpr |= 0x5;
    strm->m_arrChain[i].dpr |= (strm->m_dir == 1) ? 0x8 : 0;      //1-to HOST, 2-from HOST

    strm->m_chainSize = pcbuf->m_blkNum;

	//
	// Display Chain
	//
        /*
	for( iiBlk=0; iiBlk<strm->m_chainSize; iiBlk++ )
	{
                printk("<0>Chain[%d]: [ padr=0x%X, ladr=0x%X, siz=0x%06X, dpr=0x%X ] : [%p] \n",
					 iiBlk,
					 strm->m_arrChain[iiBlk].padr,
					 strm->m_arrChain[iiBlk].ladr,
					 strm->m_arrChain[iiBlk].siz,
					 strm->m_arrChain[iiBlk].dpr,
                                         (void*)virt_to_bus(&strm->m_arrChain[iiBlk]) );
	}

        printk("<0>9056: Assign pChain = %p, size = 0x%X, sizeofChain=0x%X\n",
                                         strm->m_arrChain,
                                         strm->m_chainSize,
                                         sizeofChain/sizeof(TPlxChainLink) );
        */


    return 0;
}

//--------------------------------------------------------------------

int TInterStream9056FreeChain ( TInterStream9056 * strm )
{
    TInterCBuf    *pcbuf = ( TInterCBuf * ) strm->m_Buffer;
    int            chain_size = sizeof ( TPlxChainLink ) * pcbuf->m_blkNum;

    if ( strm->m_arrChain )
    {
        dma_free_coherent ( pcbuf->m_pci, chain_size, strm->m_arrChain, strm->m_arrChainPA );
        strm->m_arrChain = NULL;
    }

    strm->m_chainSize = 0;
    return 0;
}

//=*********** TInterStream9056::Suspend ******************
//=********************************************************
int TInterStream9056Suspend ( TInterStream9056 * strm )
{
    strm->m_op->WritePlxMem08 ( strm->m_pAddress,
                    ( strm->m_dmaChan == 0 ) ? plxDMACSR0 : plxDMACSR1, 0 );
    return 0;
}

//=*********** TInterStream9056::Resume *******************
//=********************************************************
int TInterStream9056Resume ( TInterStream9056 * strm )
{
    strm->m_op->WritePlxMem08 ( strm->m_pAddress,
                    ( strm->m_dmaChan == 0 ) ? plxDMACSR0 : plxDMACSR1, 1 );
    strm->m_op->WritePlxMem08 ( strm->m_pAddress,
                    ( strm->m_dmaChan == 0 ) ? plxDMACSR0 : plxDMACSR1, 1 );
    return 0;
}

//--------------------------------------------------------------------

int TInterStream9056IsStarted ( TInterStream9056 * strm )
{
    return ( strm->m_arrChain == NULL ) ? 0 : 1;
}

//--------------------------------------------------------------------

void TInterStream9056SetWaitStates ( TInterStream9056 * strm, u32 waitStates )
{
    strm->m_waitStates = waitStates;
}

//--------------------------------------------------------------------

void TInterStream9056SetDmaCodeStart ( TInterStream9056 * strm,
                                       u32 dmaCodeStart )
{
    strm->m_dmaCodeStart = dmaCodeStart;
}

//--------------------------------------------------------------------

void TInterStream9056SetDmaCodeStop ( TInterStream9056 * strm, u32 dmaCodeStop )
{
    strm->m_dmaCodeStop = dmaCodeStop;
}

//--------------------------------------------------------------------
//
// PLX Memory Acesses
//

static u32 ReadPlxMem32 ( void *m_pAddress, u32 offset )
{
    return readl ( ( u32 * ) ( ( u8 * ) m_pAddress + offset ) );
}

//--------------------------------------------------------------------

static void WritePlxMem32 ( void *m_pAddress, u32 offset, u32 value )
{
    writel ( value, ( u32 * ) ( ( u8 * ) m_pAddress + offset ) );
}

//--------------------------------------------------------------------

static u8 ReadPlxMem08 ( void *m_pAddress, u32 offset )
{
    return readb ( ( u32 * ) ( ( u8 * ) m_pAddress + offset ) );
}

//--------------------------------------------------------------------

static void WritePlxMem08 ( void *m_pAddress, u32 offset, u8 value )
{
    writeb ( value, ( u32 * ) ( ( u8 * ) m_pAddress + offset ) );
}

//--------------------------------------------------------------------

struct stream_operations s_ops = {

    ReadPlxMem32: ReadPlxMem32,
    WritePlxMem32: WritePlxMem32,
    ReadPlxMem08: ReadPlxMem08,
    WritePlxMem08: WritePlxMem08,
};

//
// End of File
//
