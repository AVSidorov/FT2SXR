//=********************************************************
// StrmBox.cpp - Classes TStreamBox9056 Definition
//
// (C) Copyright Ekkore 2002-2004
//
// Created:
//              08.06.2004 by Ekkore
// Modified:
//
//=********************************************************

#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <linux/slab.h>

#ifndef _STRMBOX_H_
        #include "strmbox.h"
#endif

//=********************************************************
//=********************************************************
//=********************************************************
//
// class TStreamBox9056
//

//=********** TStreamBox9056::Create **************
//=********************************************************
TStreamBox9056 *TStreamBox9056Create ( struct device *pci,
                                       TCaptureList * pCaptList,
                                       void *pciPlxAddress,
                                       void *pciFifoBlockAddress,
                                       u32 locFifoBlockAddress )
{
    int            ii;
    TStreamBox9056 *box = kmalloc ( sizeof ( TStreamBox9056 ), GFP_KERNEL );

    if ( !box )
        return NULL;

    //box->m_pCaptList = NULL;
    for ( ii = 0; ii < STREAM_MAX; ii++ )
    {
        box->m_captResource[ii] = -1;
        box->m_captPassword[ii] = 0;
        box->m_stream[ii] = TInterStream9056Create ( pci, pciPlxAddress, locFifoBlockAddress, ii );
    }

    TStreamBox9056Init ( box, pCaptList, pciPlxAddress,
                         pciFifoBlockAddress, locFifoBlockAddress );

    return box;
}

//=********** TStreamBox9056::Delete ************************
//=********************************************************

void TStreamBox9056Delete ( TStreamBox9056 * box )
{
    int	strmId;

    if ( !box )
        return;

    for ( strmId = 0; strmId < STREAM_MAX; strmId++ )
    {
        TStreamBox9056DisableDmaRequest ( box, strmId );
        TStreamBox9056ResetFifo ( box, strmId );

	TInterStream9056Delete( box->m_stream[strmId] );
    }

    kfree(box);
}

//=********** TStreamBox9056::Init ************************
//=********************************************************
void TStreamBox9056Init ( TStreamBox9056 * box,
                          TCaptureList * pCaptList,
                          void *pciPlxAddress,
                          void *pciFifoBlockAddress, u32 locFifoBlockAddress )
{
    int            strmId;

    box->m_pciPlxAddress = pciPlxAddress;
    box->m_pciFifoBlockAddress = pciFifoBlockAddress;
    box->m_locFifoBlockAddress = locFifoBlockAddress;
    box->m_pCaptList = pCaptList;

    for ( strmId = 0; strmId < STREAM_MAX; strmId++ )
    {
        TStreamBox9056DisableDmaRequest ( box, strmId );
        TStreamBox9056ResetFifo ( box, strmId );
        //m_stream[strmId].Init ( pInterruptObject, m_pciPlxAddress, 0, 0 );
    }
}

/*
//=********** TStreamBox9056::CloseHandle *****************
//=********************************************************
int TStreamBox9056CloseHandle ( TStreamBox9056 * box )
{
    int            isStarted;
    int            strmId;

    for ( strmId = 0; strmId < STREAM_MAX; strmId++ )
    {
        isStarted = TInterStream9056IsStarted ( box->m_stream[strmId] );
        //m_stream[strmId].CloseHandle( pMemAlloc, pFileObject );
        TStreamBox9056ReleaseStartedDma ( box, isStarted, strmId, 0x7700 );
    }

    return 0;
}

//=********** TStreamBox9056::RemoveDevice ****************
//=********************************************************
int TStreamBox9056RemoveDevice ( TStreamBox9056 * box )
{
    int            isStarted;
    int            strmId;

    for ( strmId = 0; strmId < STREAM_MAX; strmId++ )
    {
        isStarted = TInterStream9056IsStarted ( box->m_stream[strmId] );
        //m_stream[strmId].RemoveDevice();
        TStreamBox9056ReleaseStartedDma ( box, isStarted, strmId, 0x7701 );
    }

    return 0;
}
*/

//=********** TStreamBox9056::Alloc ***********************
//=********************************************************
int TStreamBox9056Alloc ( TStreamBox9056 * box, size_t strmId, size_t blkNum,
                          size_t blkSize, size_t **ppStub, size_t *pBlk[] )
{
    if ( strmId >= STREAM_MAX )
        return -EINVAL;
    if ( box == NULL )
        return -EINVAL;

    return TInterCBufAlloc( box->m_stream[strmId]->m_Buffer, blkNum, blkSize, ppStub, pBlk);
}

//=********** TStreamBox9056::Disalloc ********************
//=********************************************************
int TStreamBox9056Disalloc ( TStreamBox9056 * box, u32 strmId )
{
    int            ntstatus;
    int            isStarted;

    if ( strmId >= STREAM_MAX )
        return -EINVAL;
    if ( box == NULL )
        return -EINVAL;

    isStarted = TInterStream9056IsStarted ( box->m_stream[strmId] );
    ntstatus = TInterStream9056Disalloc ( box->m_stream[strmId] );
    TStreamBox9056ReleaseStartedDma ( box, isStarted, strmId, 0x7703 );

    return ntstatus;
}

//=********** TStreamBox9056::Start ***********************
//=********************************************************
int TStreamBox9056Start ( TStreamBox9056 * box, u32 strmId, u32 isCycle,
                          u32 dir )
{
    int            ntstatus;
    u32            regFifoData;

/*
    printk ( "<0> %s: FIFO_STATUS: 0x%X, 0x%X, 0x%X, 0x%X\n", __FUNCTION__,
             ReadFifoBlock32 ( box, REG_FIFO0_STATUS ),
             ReadFifoBlock32 ( box, REG_FIFO1_STATUS ),
             ReadFifoBlock32 ( box, REG_FIFO2_STATUS ),
             ReadFifoBlock32 ( box, REG_FIFO3_STATUS ) );
*/
    if ( strmId >= STREAM_MAX )
        return -EINVAL;

    TStreamBox9056GetDirections ( box );
    if ( dir != 1 && dir != 2 )
        return -EINVAL;
    if ( ( dir & box->m_dir[strmId] ) == 0 )
        return -EINVAL;

    if ( 0 > TStreamBox9056CaptureDma ( box, strmId ) )
        return -EINVAL;
    if ( ( box->m_captResource[strmId] != 0 )
         && ( box->m_captResource[strmId] != 1 ) )
        return -EINVAL;

    TStreamBox9056EnableDmaRequest ( box, strmId, dir );

    if ( strmId <= STRMID1 )
    {
        regFifoData = ( strmId == STRMID0 ) ?
            ( ( dir == 2 ) ? REG_FIFO0_DATA : REG_FIFO3_DATA ) :
            ( ( dir == 2 ) ? REG_FIFO2_DATA : REG_FIFO1_DATA );
    }
    else
    {
        regFifoData = ( strmId == STRMID2 ) ? REG_FIFO4_DATA : REG_FIFO5_DATA;
    }

    //m_stream[strmId].Init( NULL, m_pciPlxAddress, m_locFifoBlockAddress+regFifoData*4, m_captResource[strmId] );
    box->m_stream[strmId]->m_locAdr =
        box->m_locFifoBlockAddress + regFifoData * 4;
    box->m_stream[strmId]->m_dmaChan = box->m_captResource[strmId];

//    printk ( "<0> %s: strm=%d, fifo=%0x, rsrc=%d\n", __FUNCTION__, strmId,
//             regFifoData, box->m_captResource[strmId] );

    ntstatus = TInterStream9056Start ( box->m_stream[strmId], isCycle, dir );

    return ntstatus;
}

//=********** TStreamBox9056::Stop ************************
//=********************************************************
int TStreamBox9056Stop ( TStreamBox9056 * box, u32 strmId )
{
    int            ntstatus;
    int            isStarted;

    if ( strmId >= STREAM_MAX )
        return -EINVAL;

    isStarted = TInterStream9056IsStarted ( box->m_stream[strmId] );
    ntstatus = TInterStream9056Stop ( box->m_stream[strmId] );
    TStreamBox9056ReleaseStartedDma ( box, isStarted, strmId, 0x7704 );

    return ntstatus;
}

//=********** TStreamBox9056::Wait ************************
//=********************************************************
int TStreamBox9056Wait ( TStreamBox9056 * box, u32 strmId,
                         TStreamState * pSS, u32 * pTimeElapsed )
{
    int            ntstatus;

    if ( strmId >= STREAM_MAX )
        return -EINVAL;


    ntstatus =
        TInterStream9056Wait ( box->m_stream[strmId], pSS, pTimeElapsed );

    return ntstatus;
}

//=********** TStreamBox9056::Done ************************
//=********************************************************
int TStreamBox9056Done ( TStreamBox9056 * box, u32 strmId, u32 blkNo )
{
    int            ntstatus;

    if ( strmId >= STREAM_MAX )
        return -EINVAL;

    ntstatus = TInterStream9056Done ( box->m_stream[strmId], blkNo );

    return ntstatus;
}

//=********** TStreamBox9056::Adjust **********************
//=********************************************************
int TStreamBox9056Adjust ( TStreamBox9056 * box, u32 strmId, u32 isAdjust )
{
    int            ntstatus;

    if ( strmId >= STREAM_MAX )
        return -EINVAL;

    ntstatus = TInterStream9056Adjust ( box->m_stream[strmId], isAdjust );

    return ntstatus;
}

//=********** TStreamBox9056::Isr *************************
//=********************************************************
int TStreamBox9056Isr ( TStreamBox9056 * box )
{
    int            res = 0;
    int            strmId;

    for ( strmId = 0; strmId < STREAM_MAX; strmId++ )
    {
        if ( TInterStream9056IsStarted ( box->m_stream[strmId] ) )
        {
            if ( TInterStream9056Isr ( box->m_stream[strmId] ) )
            {
                res = 1;
            }
        }
    }

    return res;
}

//=********************************************************
// HOST_FIFO Block Acesses
//=********************************************************
u32 ReadFifoBlock32 ( TStreamBox9056 * box, u32 regNum )
{
    //return ioread32 ( ( u32 * ) ( ( u8 * ) box->m_pciFifoBlockAddress + regNum * 4 ) );
    return readl((u32*)((u8*)box->m_pciFifoBlockAddress + regNum*4 ));
}

void WriteFifoBlock32 ( TStreamBox9056 * box, u32 regNum, u32 value )
{
    //iowrite32 ( value, ( u32 * ) ( ( u8 * ) box->m_pciFifoBlockAddress + regNum * 4 ) );
    writel(value, (u32*)((u8*)box->m_pciFifoBlockAddress + regNum*4 ));
}

//=*********** TStreamBox9056::ResetFifo ******************
//=********************************************************
void TStreamBox9056ResetFifo ( TStreamBox9056 * box, u32 strmId )
{
    u32            keeVal;

    if ( strmId == STRMID0 )
    {
        keeVal = 0xFFFE & ReadFifoBlock32 ( box, REG_FIFO0_CTRL );
        WriteFifoBlock32 ( box, REG_FIFO0_CTRL, keeVal | 1 );
        WriteFifoBlock32 ( box, REG_FIFO0_CTRL, keeVal );

        keeVal = 0xFFFE & ReadFifoBlock32 ( box, REG_FIFO3_CTRL );
        WriteFifoBlock32 ( box, REG_FIFO3_CTRL, keeVal | 1 );
        WriteFifoBlock32 ( box, REG_FIFO3_CTRL, keeVal );
    }
    else if ( strmId == STRMID1 )
    {
        keeVal = 0xFFFE & ReadFifoBlock32 ( box, REG_FIFO2_CTRL );
        WriteFifoBlock32 ( box, REG_FIFO2_CTRL, keeVal | 1 );
        WriteFifoBlock32 ( box, REG_FIFO2_CTRL, keeVal );

        keeVal = 0xFFFE & ReadFifoBlock32 ( box, REG_FIFO1_CTRL );
        WriteFifoBlock32 ( box, REG_FIFO1_CTRL, keeVal | 1 );
        WriteFifoBlock32 ( box, REG_FIFO1_CTRL, keeVal );
    }
    else if ( strmId == STRMID2 )
    {
        keeVal = 0xFFFE & ReadFifoBlock32 ( box, REG_FIFO4_CTRL );
        WriteFifoBlock32 ( box, REG_FIFO4_CTRL, keeVal | 1 );
        WriteFifoBlock32 ( box, REG_FIFO4_CTRL, keeVal );
    }
    else if ( strmId == STRMID3 )
    {
        keeVal = 0xFFFE & ReadFifoBlock32 ( box, REG_FIFO5_CTRL );
        WriteFifoBlock32 ( box, REG_FIFO5_CTRL, keeVal | 1 );
        WriteFifoBlock32 ( box, REG_FIFO5_CTRL, keeVal );
    }
}

//=********** TStreamBox9056::GetDirections ***************
//=********************************************************
void TStreamBox9056GetDirections ( TStreamBox9056 * box )
{
    u32            fifoId[6],
                   blockId;
    int            ii;

    for ( ii = 0; ii < 6; ii++ )
        fifoId[ii] = 0xFFFF & ReadFifoBlock32 ( box, REG_FIFOn_ID ( ii ) );
    blockId = 0x0FFF & ReadFifoBlock32 ( box, REG_BLOCK_ID );

    box->m_dir[STRMID0] = 0;
    box->m_dir[STRMID1] = 0;
    box->m_dir[STRMID2] = 0;
    box->m_dir[STRMID3] = 0;
/*
    printk( "<0>%s: FIFO_ID: 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X. BLOCK_ID = 0x%X\n", __FUNCTION__,
          fifoId[0], fifoId[1], fifoId[2], fifoId[3], fifoId[4], fifoId[5],
          blockId );
    printk( "<0>%s: adr = 0x%p, lock=0x%X\n", __FUNCTION__,
             box->m_pciFifoBlockAddress, ( int ) box->m_locFifoBlockAddress );
*/
    //
    // Check if HOST_FIFO Block is missing
    //
    if ( 0x002 != blockId )
    {
        printk ( "<0>%s: ERROR: BLOCK_ID = 0x%X\n", __FUNCTION__, blockId );
        return;
    }

    //
    // Read FIFO_ID Regs
    //
    if ( 0 != fifoId[0] )
        box->m_dir[0] |= 0x2;
    if ( 0 != fifoId[3] )
        box->m_dir[0] |= 0x1;


    if ( 0 != fifoId[2] )
        box->m_dir[1] |= 0x2;
    if ( 0 != fifoId[1] )
        box->m_dir[1] |= 0x1;

    if ( 0 != fifoId[4] )
        box->m_dir[2] = ( ( fifoId[4] >> 12 ) == 1 ) ? 0x1 : 0x2;

    if ( 0 != fifoId[5] )
        box->m_dir[3] = ( ( fifoId[5] >> 12 ) == 1 ) ? 0x1 : 0x2;

//    printk ( "<0> GetDirections()  dir = 0x%X, 0x%X, 0x%X, 0x%X\n",
//            box->m_dir[0], box->m_dir[1], box->m_dir[2], box->m_dir[3] );
}

//=********** TStreamBox9056::CaptureDma ******************
//=********************************************************
int TStreamBox9056CaptureDma ( TStreamBox9056 * box, u32 strmId )
{
    if ( 0 == box->m_pCaptList )
        return -1;
    if ( strmId <= STRMID1 )
    {
        box->m_captPassword[strmId] =
            Capture ( box->m_pCaptList, ( strmId == STRMID0 ) ? "dma0" : "dma1",
                      0 );
        if ( 0 != box->m_captPassword[strmId] )
            box->m_captResource[strmId] = strmId;
    }
    else
    {
        char           dmaName[2][10] = {
            "dma0", "dma1"
        };
        int            dmaId;

        for ( dmaId = 0; dmaId <= 1; dmaId++ )
        {
            box->m_captPassword[strmId] =
                Capture ( box->m_pCaptList, dmaName[dmaId], 0 );
            if ( 0 != box->m_captPassword[strmId] )
            {
                box->m_captResource[strmId] = dmaId;
                break;
            }
        }
    }

    return ( 0 != box->m_captPassword[strmId] ) ? 0 : -1;
}

//=********** TStreamBox9056::ReleaseDma ******************
//=********************************************************
void TStreamBox9056ReleaseDma ( TStreamBox9056 * box, u32 strmId )
{
    //KdPrint((KDHEAD": ::-------ReCapture() strm=%d, pass=0x%X\n", strmId, m_captPassword[strmId] ));

    char           dmaName[2][10] = {
        "dma0", "dma1"
    };

    if ( box->m_pCaptList )
        if ( box->m_captResource[strmId] >= 0 )
        {
            if ( 0 <=
                 Release ( box->m_pCaptList,
                           dmaName[box->m_captResource[strmId]],
                           box->m_captPassword[strmId] ) )
            {
                box->m_captResource[strmId] = -1;
                box->m_captPassword[strmId] = 0;
                //KdPrint((KDHEAD": ::-------ReCapture() OK\n" ));
            }
        }
}

//=********** TStreamBox9056::ReleaseStartedDma ***********
//=********************************************************
void TStreamBox9056ReleaseStartedDma ( TStreamBox9056 * box,
                                       int wasStarted, u32 strmId, u32 nFuncID )
{
    //KdPrint((KDHEAD": ::-------ReCapture() strm=%d, func=0x%X, was=%d, is=%d\n", strmId, nFuncID, wasStarted, m_stream[strmId].IsStarted() ));
    if ( wasStarted
         && 0 == TInterStream9056IsStarted ( box->m_stream[strmId] ) )
    {
        TStreamBox9056ReleaseDma ( box, strmId );
        TStreamBox9056DisableDmaRequest ( box, strmId );
    }
}

//=********** TStreamBox9056::EnableDmaRequest ************
//=********************************************************
void TStreamBox9056EnableDmaRequest ( TStreamBox9056 * box,
                                      u32 strmId, u32 dir )
{
    u32            regFifoCtrl;

    TStreamBox9056DisableDmaRequest ( box, strmId );

    if ( strmId <= STRMID1 )
    {
        regFifoCtrl = ( strmId == STRMID0 ) ?
            ( ( dir == 2 ) ? REG_FIFO0_CTRL : REG_FIFO3_CTRL ) :
            ( ( dir == 2 ) ? REG_FIFO2_CTRL : REG_FIFO1_CTRL );

        WriteFifoBlock32 ( box, regFifoCtrl, 2 );
    }
    else
    {
        regFifoCtrl = ( strmId == STRMID2 ) ? REG_FIFO4_CTRL : REG_FIFO5_CTRL;
        if ( 0 <= box->m_captResource[strmId] )
        {
            WriteFifoBlock32 ( box, regFifoCtrl,
                               2 | ( box->m_captResource[strmId] << 3 ) );

            //
            // Suppres DREQ from FIFOs 0-3
            //
            if ( 0 == box->m_captResource[strmId] )
            {
                WriteFifoBlock32 ( box, REG_FIFO0_CTRL, 0 );
                WriteFifoBlock32 ( box, REG_FIFO3_CTRL, 0 );
            }
            if ( 1 == box->m_captResource[strmId] )
            {
                WriteFifoBlock32 ( box, REG_FIFO1_CTRL, 0 );
                WriteFifoBlock32 ( box, REG_FIFO2_CTRL, 0 );
            }
        }
    }
    //printk ( "<0> %s: strmId=%d, dir=%d, reg=0x%X\n", __FUNCTION__, strmId, dir,
    //         regFifoCtrl );
}

//=********** TStreamBox9056::DisableDmaRequest ***********
//=********************************************************
void TStreamBox9056DisableDmaRequest ( TStreamBox9056 * box, u32 strmId )
{
    if ( strmId == STRMID0 )
    {
        WriteFifoBlock32 ( box, REG_FIFO0_CTRL, 0 );
        WriteFifoBlock32 ( box, REG_FIFO3_CTRL, 0 );
    }
    else if ( strmId == STRMID1 )
    {
        WriteFifoBlock32 ( box, REG_FIFO2_CTRL, 0 );
        WriteFifoBlock32 ( box, REG_FIFO1_CTRL, 0 );
    }
    else if ( strmId == STRMID2 )
    {
        WriteFifoBlock32 ( box, REG_FIFO4_CTRL, 0 );
    }
    else if ( strmId == STRMID3 )
    {
        WriteFifoBlock32 ( box, REG_FIFO5_CTRL, 0 );
    }
//      KdPrint((KDHEAD": ::DisableDmaRequest() strmId=%d\n", strmId ));
}

void TStreamBox9056Info ( TStreamBox9056 * box, u32 strmId, u32 * pAttr )
{
    TStreamBox9056GetDirections ( box );

    *pAttr = box->m_dir[strmId];

    //printk("<0> &box->m_dir[%d] = %x\n", strmId, (int)&box->m_dir[strmId] );
    //printk("<0> box->m_dir[%d] = %x\n", strmId, (int)box->m_dir[strmId]);
}

void TStreamBox9056SetWaitStates ( TStreamBox9056 * box, u32 strmId,
                                   u32 waitStates )
{
    TInterStream9056SetWaitStates ( box->m_stream[strmId], waitStates );
}

void TStreamBox9056SetDmaCodeStart ( TStreamBox9056 * box, u32 strmId,
                                     u32 dmaCodeStart )
{
    TInterStream9056SetDmaCodeStart ( box->m_stream[strmId], dmaCodeStart );
}

void TStreamBox9056SetDmaCodeStop ( TStreamBox9056 * box, u32 strmId,
                                    u32 dmaCodeStop )
{
    TInterStream9056SetDmaCodeStop ( box->m_stream[strmId], dmaCodeStop );
}


//
// End of File
//
