
#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/pagemap.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#ifndef _EVENT_H_
#include "event.h"
#endif
#ifndef	 _DMA_CHAN_H_
#include "dmachan.h"
#endif

#include "pagehelp.h"
#include "memhelp.h"

///////////////////////////////////////////////////////////////////////////////

struct CDmaChannel* CDmaChannelCreate( 	u32 NumberOfChannel,
                                        TASKLET_ROUTINE dpc,
                                        void *brd,
                                        struct device	*dev,
                                        u32 cbMaxTransferLength,
                                        u16 idBlockFifo,
                                        int bScatterGather )
{
    struct CDmaChannel *dma = NULL;

    dma = kmalloc(sizeof(struct CDmaChannel), GFP_KERNEL);
    if(!dma) {
        err_msg(err_trace, "%s(): Error allocate memory for CDmaChannel object\n", __FUNCTION__);
        return NULL;
    }

    memset(dma, 0, sizeof(struct CDmaChannel));

    dma->m_NumberOfChannel = NumberOfChannel;
    dma->m_Board = brd;
    dma->m_dev = dev;
    dma->m_UseCount = 0;
    dma->m_DpcForIsr = dpc;
    dma->m_idBlockFifo = idBlockFifo;

    spin_lock_init( &dma->m_DmaLock );
    init_waitqueue_head( &dma->m_DmaWq );
    tasklet_init( &dma->m_Dpc, dma->m_DpcForIsr, (unsigned long)dma );
    InitKevent( &dma->m_BlockEndEvent );
    InitKevent( &dma->m_BufferEndEvent );
    INIT_LIST_HEAD(&dma->m_sg_blocks_list);

    dbg_msg(dbg_trace, "%s(%d): COMPLETE.\n", __FUNCTION__, dma->m_NumberOfChannel);

    return dma;
}

///////////////////////////////////////////////////////////////////////////////

void CDmaChannelDelete(struct CDmaChannel *dma)
{
    dbg_msg(dbg_trace, "%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);
    if (dma) {
        tasklet_kill( &dma->m_Dpc );
        kfree(dma);
    }
}

///////////////////////////////////////////////////////////////////////////////


int RequestMemory(struct CDmaChannel *dma, void** ppVirtAddr, u32 size, u32 *pCount, void** pStub, u32 bMemType)
{
    int Status = -ENOMEM;

    dbg_msg(dbg_trace, "%s(): Channel = %d. Type %d\n", __FUNCTION__, dma->m_NumberOfChannel, bMemType);

    // при первом обращении действительно выделяем память,
    // а при повторных только отображаем выделенную память на пользовательское пространство
    if(!dma->m_UseCount)
    {
        dma->m_MemType = bMemType;
        dma->m_BlockCount = *pCount;
        dma->m_BlockSize = size;

        // выделяем память под описатели блоков (системный, и логический адрес для каждого блока)
        dma->m_pBufDscr.SystemAddress = (void*)dma_alloc_coherent( dma->m_dev,
                                                                   dma->m_BlockCount * sizeof(SHARED_MEMORY_DESCRIPTION),
                                                                   &dma->m_pBufDscr.LogicalAddress, GFP_KERNEL);
        if(!dma->m_pBufDscr.SystemAddress)
        {
            err_msg(err_trace, "%s(): Not memory for buffer descriptions\n", __FUNCTION__);
            return -ENOMEM;
        }

        dma->m_ScatterGatherTableEntryCnt = 0;
    }

    Status = RequestStub(dma, pStub);
    if(Status == 0)
    {
        if(dma->m_MemType == SYSTEM_MEMORY_TYPE) {
            Status = RequestSysBuf(dma, ppVirtAddr);
        } else
            if(dma->m_MemType == PHYS_MEMORY_TYPE) {
                Status = RequestPhysAddrBuf(dma, ppVirtAddr);
            } else if(dma->m_MemType == USER_MEMORY_TYPE) {
                Status = RequestUserAddrBuf(dma, ppVirtAddr);
            }
            else
            {
                ReleaseStub(dma);
                dma_free_coherent(dma->m_dev,
                                  dma->m_BlockCount * sizeof(SHARED_MEMORY_DESCRIPTION),
                                  dma->m_pBufDscr.SystemAddress,
                                  dma->m_pBufDscr.LogicalAddress);
                dma->m_pBufDscr.SystemAddress = NULL;
                dma->m_pBufDscr.LogicalAddress = 0;
                err_msg(err_trace, "%s(): Invalid memory type for DMA data blocks\n", __FUNCTION__);
                return -EINVAL;
            }

        if(Status == 0)
        {
            if(!dma->m_UseCount)
            {
                dbg_msg(dbg_trace, "%s(): Scatter/Gather Table Entry is %d\n", __FUNCTION__, dma->m_ScatterGatherTableEntryCnt);
                if(dma->m_idBlockFifo == PE_EXT_FIFO_ID) {
                    Status = SetScatterGatherListExt(dma);
                    dbg_msg(dbg_trace, "%s(): Scatter/Gather Table Entry is OK\n", __FUNCTION__);
                }
                else {
                    dbg_msg(dbg_trace, "%s(): Scatter/Gather Table Entry not created\n", __FUNCTION__);
                    //SetScatterGatherList(dma);
                }
                if(Status == 0)
                {
                    *pCount = dma->m_BlockCount;
                    dma->m_pStub = (PAMB_STUB)dma->m_StubDscr.SystemAddress;
                    //				dma->m_pStub->lastBlock = -1;
                    //				dma->m_pStub->totalCounter = 0;
                    dma->m_pStub->lastBlock = (u32)dma->m_StubDscr.LogicalAddress;
                    dma->m_pStub->totalCounter = ((sizeof(dma_addr_t) == 8) ? (u32)(dma->m_StubDscr.LogicalAddress >> 32) : 0);
                    dma->m_pStub->offset = dma->m_IrqCount;
                    dma->m_pStub->state = STATE_STOP;
                    dbg_msg(dbg_trace, "%s(): Stub Phys addr: %08X:%08X, IRQ_CNT = %d\n", __FUNCTION__, dma->m_pStub->totalCounter, dma->m_pStub->lastBlock, dma->m_pStub->offset);

                    // передаю информацию о таблице дескрипторных блоков в приложение в отладочных целях
                    //				SHARED_MEMORY_DESCRIPTION *pMemDscr = (SHARED_MEMORY_DESCRIPTION*)dma->m_pBufDscr.SystemAddress;
                    //				u32* psgtable = (u32*)pMemDscr[0].SystemAddress;
                    //				dbg_msg(dbg_trace, "%s(): Virt Addr = %p\n", __FUNCTION__, psgtable);

                    //u32* psgtable = (u32*)ppVirtAddr[0];
                    //				psgtable[0] = (u32)dma->m_SGTableDscr.SystemAddress;
                    //				psgtable[0] = dma->m_ScatterGatherTableEntryCnt;
                    //				psgtable[2] = dma->m_ScatterGatherBlockCnt;
                    //				dbg_msg(dbg_trace, "%s(): SGT_adr = %08X SGT_entry = %d SGblkCnt = %d\n", __FUNCTION__, psgtable[0], psgtable[1], psgtable[2]);
                    // и физический адрес блочка (фактически, адрес счетчика последнего блока)
                    //				psgtable[3] = (u32)dma->m_StubDscr.LogicalAddress;
                    //				psgtable[4] = (u32)(dma->m_StubDscr.LogicalAddress >> 32);
                    //				dbg_msg(dbg_trace, "%s(): Stub Phys addr :  %08X:%08X\n", __FUNCTION__, psgtable[4], psgtable[3]);
                }
                else
                {
                    ReleaseStub( dma );
                    if(dma->m_MemType == SYSTEM_MEMORY_TYPE) {
                        ReleaseSysBuf( dma );
                    } else
                        if(dma->m_MemType == PHYS_MEMORY_TYPE) {
                            ReleasePhysAddrBuf( dma );
                        }
                        else if(dma->m_MemType == USER_MEMORY_TYPE) {
                            ReleaseUserAddrBuf( dma );
                        }
                    dma_free_coherent(dma->m_dev,
                                      dma->m_BlockCount * sizeof(SHARED_MEMORY_DESCRIPTION),
                                      dma->m_pBufDscr.SystemAddress, dma->m_pBufDscr.LogicalAddress);
                    dma->m_pBufDscr.SystemAddress = NULL;
                    dma->m_pBufDscr.LogicalAddress = 0;
                    return Status;
                }
            }
            dma->m_UseCount++;
        }
        else
        {
            ReleaseStub(dma);
            if(dma->m_MemType == SYSTEM_MEMORY_TYPE) {
                ReleaseSysBuf( dma );
            } else
                if(dma->m_MemType == PHYS_MEMORY_TYPE) {
                    ReleasePhysAddrBuf( dma );
                }
            dma_free_coherent(dma->m_dev, dma->m_BlockCount * sizeof(SHARED_MEMORY_DESCRIPTION),
                              dma->m_pBufDscr.SystemAddress, dma->m_pBufDscr.LogicalAddress);
            dma->m_pBufDscr.SystemAddress = NULL;
            dma->m_pBufDscr.LogicalAddress = 0;
            err_msg(err_trace, "%s(): Error allocate memory\n", __FUNCTION__);
            return -EINVAL;
        }
    }
    else
    {
        if(!dma->m_UseCount)	{
            dma_free_coherent(dma->m_dev,
                              dma->m_BlockCount * sizeof(SHARED_MEMORY_DESCRIPTION),
                              dma->m_pBufDscr.SystemAddress, dma->m_pBufDscr.LogicalAddress);
            dma->m_pBufDscr.SystemAddress = NULL;
            dma->m_pBufDscr.LogicalAddress = 0;
        }
    }
    return Status;
}

///////////////////////////////////////////////////////////////////////////////

void ReleaseMemory(struct CDmaChannel *dma)
{
    if(dma->m_UseCount) {

        dbg_msg(dbg_trace, "%s(): Entered. Channel = %d\n", __FUNCTION__, dma->m_NumberOfChannel);

        ReleaseStub( dma );
        ReleaseSGList( dma );
        if(dma->m_MemType == SYSTEM_MEMORY_TYPE) {
            ReleaseSysBuf( dma );
        } else
            if(dma->m_MemType == PHYS_MEMORY_TYPE) {
                ReleasePhysAddrBuf( dma );
            }
            else if(dma->m_MemType == USER_MEMORY_TYPE) {
                ReleaseUserAddrBuf( dma );
            }
        dma_free_coherent(dma->m_dev,
                          dma->m_BlockCount * sizeof(SHARED_MEMORY_DESCRIPTION),
                          dma->m_pBufDscr.SystemAddress, dma->m_pBufDscr.LogicalAddress);
        dma->m_pBufDscr.SystemAddress = NULL;
        dma->m_pBufDscr.LogicalAddress = 0;
        dma->m_UseCount--;
    } else {
        err_msg(err_trace, "%s(): Memory not allocated.\n", __FUNCTION__);
    }
}

///////////////////////////////////////////////////////////////////////////////
/*
void FreeUserAddress(struct CDmaChannel *dma)
{
    if(dma->m_UseCount)
    {
        if(dma->m_MemType == SYSTEM_MEMORY_TYPE) {
            ReleaseSysBuf(dma);
        } else {
            err_msg(err_trace, "%s(): Unsupported memory allocation.\n",__FUNCTION__);
        }

        ReleaseStub(dma);
    }
}
*/
///////////////////////////////////////////////////////////////////////////////

int SetScatterGatherListExt(struct CDmaChannel *dma)
{
    int Status = 0;
    u32 iBlock = 0, ii = 0;
    u32 iEntry = 0;
    u32 iBlkEntry = 0;
    u64 *pDscrBuf = NULL;
    u16* pNextDscr = NULL;
    u32 DscrSize = 0;
    struct list_head *p;
    struct DMA_SG_CONTAINER *pSGContainer;
    SHARED_MEMORY_DESCRIPTION *pMemDscr = (SHARED_MEMORY_DESCRIPTION*)dma->m_pBufDscr.SystemAddress;
    DMA_CHAINING_DESCR_EXT	*pSGTEx = 0;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    Status = RequestSGList(dma);
    if(Status < 0)
        return Status;

    //получим адрес таблицы для хранения цепочек DMA
    dma->m_pScatterGatherTableExt = (DMA_CHAINING_DESCR_EXT*)dma->m_SGTableDscr.SystemAddress;

    DscrSize = DSCR_BLOCK_SIZE*sizeof(DMA_CHAINING_DESCR_EXT);

    //обнулим таблицу дескрипторов DMA
    list_for_each(p, &dma->m_sg_blocks_list)
    {
        pSGContainer = list_entry(p, struct DMA_SG_CONTAINER, m_list);
        memset(pSGContainer->m_va, 0, SG_CONTAINER_SIZE);
    }

    dbg_msg(dbg_trace, "%s(): m_SGTableDscr.SystemAddress = %p\n", __FUNCTION__, dma->m_SGTableDscr.SystemAddress );
    dbg_msg(dbg_trace, "%s(): m_SGTableDscr.LogicalAddress = %llx\n", __FUNCTION__, (u64)dma->m_SGTableDscr.LogicalAddress );

    //заполним значениями таблицу цепочек DMA
    for(iBlock=0, iEntry=0; iBlock < dma->m_BlockCount; iBlock++) {
        u64	address = 0ULL;
        u64	DmaSize = 0ULL;
        u32 iPage = 0;

        do
        {
            if(dma->m_MemType == USER_MEMORY_TYPE)
            {
                address = pMemDscr[iBlock].PageAddresses[iPage];
                DmaSize = PAGE_SIZE;
            }
            else
            {
                //адрес и размер DMA блока
                address = pMemDscr[iBlock].LogicalAddress;
                DmaSize = dma->m_BlockSize;
            }

            DmaSize -= 0x1000;

            dbg_msg(dbg_trace, "%s(): address[%d] = 0x%llx, DmaSize = 0x%llx\n", __FUNCTION__, iPage, address, DmaSize);

            pSGTEx = GetSGTExEntry(dma, iEntry, 0);

            //заполним поля элментов таблицы дескрипторов
            pSGTEx->AddrByte1  = (u8)((address >> 8) & 0xFF);
            pSGTEx->AddrByte2  = (u8)((address >> 16) & 0xFF);
            pSGTEx->AddrByte3  = (u8)((address >> 24) & 0xFF);
            pSGTEx->AddrByte4  = (u8)((address >> 32) & 0xFF);
            pSGTEx->SizeByte1  = (u8)((DmaSize >> 8) & 0xFF);
            pSGTEx->SizeByte2  = (u8)((DmaSize >> 16) & 0xFF);
            pSGTEx->SizeByte3  = (u8)((DmaSize >> 24) & 0xFF);
            pSGTEx->Cmd.JumpNextDescr = 1; //перейти к следующему дескриптору
            pSGTEx->Cmd.JumpNextBlock = 0; //перейти к следующему блоку дескрипторов
            pSGTEx->Cmd.JumpDescr0 = 0;
            pSGTEx->Cmd.Res0 = 0;
            pSGTEx->Cmd.EndOfTrans = 0;
            pSGTEx->Cmd.Res = 0;
            pSGTEx->SizeByte1 |= dma->m_DmaDirection;

            {
                u32 *ptr=(u32*)pSGTEx;
                dbg_msg(dbg_trace, "%s(): %d: Entry Addr: %p, Data Addr: %llx  %.8X %.8X\n",
                        __FUNCTION__, iEntry, pSGTEx, address, ptr[1], ptr[0]);
            }

            if(((iEntry+2)%DSCR_BLOCK_SIZE) == 0)
            {
                DMA_CHAINING_DESCR_EXT *pdscr = 0;
                u32 NextDscrBlockAddr = 0;
                dma_addr_t NextDscrBlockAddrPA = 0;
                DMA_NEXT_BLOCK *pNextBlock = NULL;

                pSGTEx->Cmd.JumpNextBlock = 1;
                pSGTEx->Cmd.JumpNextDescr = 0;

                if(((iEntry + 2)/DSCR_BLOCK_SIZE) == dma->m_ScatterGatherBlockCnt)
                    pdscr = GetSGTExEntry(dma, 0, &NextDscrBlockAddrPA);
                else
                    pdscr = GetSGTExEntry(dma, iEntry+2, &NextDscrBlockAddrPA);

                NextDscrBlockAddr = (u32)NextDscrBlockAddrPA;

                dbg_msg(dbg_trace, "%s(): NextDscrBlock [PA]: %x\n", __FUNCTION__, NextDscrBlockAddr);
                dbg_msg(dbg_trace, "%s(): NextDscrBlock [VA]: %p\n", __FUNCTION__, GetSGTExEntry(dma, iEntry+2, 0));

                pNextBlock = (DMA_NEXT_BLOCK*)GetSGTExEntry(dma, iEntry+1, 0);

                dbg_msg(dbg_trace, "%s(): pNextBlock: %p\n", __FUNCTION__, pNextBlock);

                pNextBlock->NextBlkAddr = (NextDscrBlockAddr >> 8) & 0xFFFFFF;
                pNextBlock->Signature = 0x4953;
                pNextBlock->Crc = 0;
                iEntry++;
            }

            iEntry++;
            iPage++;
        } while(iPage < pMemDscr[iBlock].PageCount);

        if(((iEntry % DSCR_BLOCK_SIZE)) != 0)
        {
            pSGTEx = GetSGTExEntry(dma, iEntry-1, 0);
            pSGTEx->Cmd.EndOfTrans = pMemDscr[iBlock].EotFlag; //1;
        }
        else
        {
            pSGTEx = GetSGTExEntry(dma, iEntry-2, 0);
            pSGTEx->Cmd.EndOfTrans = pMemDscr[iBlock].EotFlag; //1;
        }
    }

    dbg_msg(dbg_trace, "%s(): iEntry = %d\n", __FUNCTION__, iEntry);

    if(((iEntry % DSCR_BLOCK_SIZE)) != 0)
    {
        DMA_NEXT_BLOCK *pNextBlock = NULL;
        u32 i = 0;

        pSGTEx = GetSGTExEntry(dma, iEntry-1, 0);
        pSGTEx->Cmd.JumpNextDescr = 0;

        pNextBlock = (DMA_NEXT_BLOCK*)GetSGTExEntry(dma, iEntry, 0);
        pNextBlock->NextBlkAddr = (dma->m_SGTableDscr.LogicalAddress >> 8);

        i = (DSCR_BLOCK_SIZE * dma->m_ScatterGatherBlockCnt) - 1;
        pNextBlock = (DMA_NEXT_BLOCK*)GetSGTExEntry(dma, i, 0);

        dbg_msg(dbg_trace, "%s(): %d: pNextBlock: %p\n", __FUNCTION__, i, pNextBlock );

        pNextBlock->NextBlkAddr = 0;
        pNextBlock->Signature = 0x4953;
        pNextBlock->Crc = 0;
    }

    dbg_msg(dbg_trace, "%s(): DmaDirection = %d, DmaLocalAddress = 0x%X\n", __FUNCTION__, dma->m_DmaDirection, dma->m_DmaLocalAddress);

    for( ii=0; ii<dma->m_ScatterGatherBlockCnt*DSCR_BLOCK_SIZE; ii++ )
    {
        u32 *ptr = (u32*)GetSGTExEntry(dma, ii, 0);
        dbg_msg(dbg_trace, "%s(): %d: %.8X %.8X\n", __FUNCTION__, ii, ptr[1], ptr[0]);
    }

    pDscrBuf = (u64*)dma->m_pScatterGatherTableExt;

    for(iBlkEntry = 0; iBlkEntry < dma->m_ScatterGatherBlockCnt; iBlkEntry++)
    {
        u32 ctrl_code = 0xFFFFFFFF;

        pDscrBuf = (u64*)GetSGTExEntry(dma, iBlkEntry * DSCR_BLOCK_SIZE, 0);

        for(iBlock = 0; iBlock < DSCR_BLOCK_SIZE; iBlock++)
        {
            u16 data0 = (u16)(pDscrBuf[iBlock] & 0xFFFF);
            u16 data1 = (u16)((pDscrBuf[iBlock] >> 16) & 0xFFFF);
            u16 data2 = (u16)((pDscrBuf[iBlock] >> 32) & 0xFFFF);
            u16 data3 = (u16)((pDscrBuf[iBlock] >> 48) & 0xFFFF);
            if(iBlock == DSCR_BLOCK_SIZE-1)
            {
                ctrl_code = ctrl_code ^ data0 ^ data1 ^ data2 ^ data3;

                dbg_msg(dbg_trace, "%s(): DSCR_BLCK[%d] - NextBlkAddr = 0x%8X, Signature = 0x%4X, Crc = 0x%4X\n", __FUNCTION__,
                        iBlkEntry,
                        (u32)(pDscrBuf[iBlock] << 8),
                        (u16)((pDscrBuf[iBlock] >> 32) & 0xFFFF),
                        (u16)ctrl_code);
            }
            else
            {
                u32 ctrl_tmp = 0;
                ctrl_code = ctrl_code ^ data0 ^ data1 ^ data2 ^ data3;
                ctrl_tmp = ctrl_code << 1;
                ctrl_tmp |= (ctrl_code & 0x8000) ? 0: 1;
                ctrl_code = ctrl_tmp;

                //dbg_msg(dbg_trace, "%s(): %d(%d) - PciAddr = 0x%8X, Cmd = 0x%2X, DmaLength = %d(%2X %2X %2X)\n",  __FUNCTION__,
                //									iBlock, iBlkEntry,
                //									(u32)(pDscrBuf[iBlock] << 8),
                //									(u8)(pDscrBuf[iBlock] >> 32),
                //									(u32)((pDscrBuf[iBlock] >> 41) << 9),
                //									(u8)(pDscrBuf[iBlock] >> 56),
                //									(u8)(pDscrBuf[iBlock] >> 48),
                //									(u8)(pDscrBuf[iBlock] >> 40));
                //dbg_msg(dbg_trace, "%s(): JumpNextDescr = %d, JumpNextBlock = %d, JumpDescr0 = %d, EndOfTrans = %d, Signature = 0x%08X, Crc = 0x%08X\n",
                //									__FUNCTION__, m_pScatterGatherTable[iEntry].Cmd.EndOfChain,
                //									m_pScatterGatherTable[iEntry].Cmd.EndOfTrans,
                //									m_pScatterGatherTable[iEntry].Signature,
                //									m_pScatterGatherTable[iEntry].Crc
                //									);
            }
        }
        pNextDscr = (u16*)pDscrBuf;
        pNextDscr[255] |= (u16)ctrl_code;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

u32 NexDscrAddress(void *pVirtualAddress)
{
    return (u32)(virt_to_phys(pVirtualAddress)>>4);
}

///////////////////////////////////////////////////////////////////////////////
// получение из пользовательского пространства физических адресов памяти, доступной для операций ПДП
///////////////////////////////////////////////////////////////////////////////

int RequestPhysAddrBuf(struct CDmaChannel *dma, void **pMemPhysAddr)
{
    u32 iBlock = 0;
    SHARED_MEMORY_DESCRIPTION *pMemDscr = (SHARED_MEMORY_DESCRIPTION*)dma->m_pBufDscr.SystemAddress;
    u32 BlockIncSum = 0;
    dma->m_IrqCount = 0;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++)
    {
        dma_addr_t 	 LogicalAddress = (size_t)pMemPhysAddr[iBlock];
        u32 EotFlag = (u32)(LogicalAddress & 0x1ULL); // получим флаг прервания из младшего бита адреса
        LogicalAddress &= ~0x1ULL;                    // удаляем флаг прервания из младшего бита адреса

        if(pMemPhysAddr[iBlock] == NULL)
        {
            dbg_msg(dbg_trace, "%s(): Not enough memory for %d block location. m_BlockSize = 0x%X\n",
                    __FUNCTION__, (int)iBlock, dma->m_BlockSize);
            return -ENOMEM;
        }
        pMemDscr[iBlock].SystemAddress = phys_to_virt(LogicalAddress);
        pMemDscr[iBlock].LogicalAddress = LogicalAddress;
        pMemDscr[iBlock].PageCount = 0;
        pMemDscr[iBlock].Pages = 0;
        pMemDscr[iBlock].PageAddresses = 0;
        pMemDscr[iBlock].EotFlag = EotFlag;
        if(pMemDscr[iBlock].EotFlag)
        {
            pMemDscr[dma->m_IrqCount].BlockInc = (iBlock+1) - BlockIncSum;
            dbg_msg(dbg_trace, "%s(): BlockInc = %d\n", __FUNCTION__, pMemDscr[dma->m_IrqCount].BlockInc);
            BlockIncSum += pMemDscr[dma->m_IrqCount].BlockInc;
            dma->m_IrqCount++;
        }

        //lock_pages( pMemDscr[iBlock].SystemAddress, dma->m_BlockSize );

        pMemPhysAddr[iBlock] = (void*)((size_t)pMemDscr[iBlock].LogicalAddress);

        dbg_msg(dbg_trace, "%s(): Block[%i]: Phys Addr = 0x%llX, Virt Addr = %p\n", __FUNCTION__, iBlock, (long long)pMemDscr[iBlock].LogicalAddress, pMemDscr[iBlock].SystemAddress);
    }

    dma->m_BlockCount = iBlock;
    dma->m_ScatterGatherTableEntryCnt = iBlock;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// получение из пользовательского пространства виртуальных адресов памяти,
// которую необходимо сделать доступной для операций ПДП
///////////////////////////////////////////////////////////////////////////////

int RequestUserAddrBuf(struct CDmaChannel *dma, void **pMemUserAddr)
{
    u32 iBlock = 0;
    u32 totalLockedPage = 0;
    int Status = -EINVAL;
    SHARED_MEMORY_DESCRIPTION *pDscr = 0;

    if(!dma) {
        dbg_msg(dbg_trace, "%s(): Invalid dma channel pointer\n", __FUNCTION__);
        return -EINVAL;
    }

    dbg_msg(dbg_trace, "%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if(dma->m_UseCount) {
        dbg_msg(err_trace, "%s(): Memory already allocated.\n", __FUNCTION__);
        return -EINVAL;
    }

    pDscr = (SHARED_MEMORY_DESCRIPTION*)dma->m_pBufDscr.SystemAddress;
    if(!pDscr) {
        dbg_msg(err_trace, "%s(): Memory fro descriptors not allocated.\n", __FUNCTION__);
        return -EINVAL;
    }

    // проверим размер у буферов пользователя
    if(check_size( dma->m_BlockSize )) {
        dbg_msg(err_trace, "%s(): Ivalid user space block size: %x\n", __FUNCTION__, (int)dma->m_BlockSize);
        return -EINVAL;
    }

    // проверим выравнивание адресов у буферов пользователя
    for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++) {
        if(check_address(pMemUserAddr[iBlock])) {
            dbg_msg(err_trace, "%s(): Ivalid user space address! Block[%d] = %p\n", __FUNCTION__, (int)iBlock, pMemUserAddr[iBlock]);
            return -EINVAL;
        }
    }

    dma->m_IrqCount = dma->m_BlockCount;

    for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++) {

        int res = lock_user_memory(&pDscr[iBlock], pMemUserAddr[iBlock], dma->m_BlockSize);
        if(res < 0) {
            dbg_msg(err_trace, "%s(): Not enough memory for %i block location. m_BlockSize = %X\n", __FUNCTION__, (int)iBlock, (int)dma->m_BlockSize );
            break;
        }

        totalLockedPage += pDscr[iBlock].PageCount;

        pDscr[iBlock].SystemAddress = NULL;
        pDscr[iBlock].LogicalAddress = 0;
        pDscr[iBlock].EotFlag = 1;

        dbg_msg(dbg_trace, "%s(): Userspace buffer %p locked\n", __FUNCTION__, pMemUserAddr[iBlock]);
    }

    dbg_msg(dbg_trace, "%s(): Total page number %d\n", __FUNCTION__, totalLockedPage);

    if(totalLockedPage)
        Status = 0;

    dma->m_BlockCount = iBlock;
    dma->m_ScatterGatherTableEntryCnt = totalLockedPage;
    dma->m_IrqCount = dma->m_BlockCount;

    return Status;
}

///////////////////////////////////////////////////////////////////////////////////

int ReleaseUserAddrBuf(struct CDmaChannel *dma)
{
    u32 iBlock = 0;
    //u32 i = 0;
    SHARED_MEMORY_DESCRIPTION *pMemDscr = (SHARED_MEMORY_DESCRIPTION*)dma->m_pBufDscr.SystemAddress;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    if(!pMemDscr) {
        err_msg(err_trace, "%s(): MemDscr is NULL. Possible memory already released.\n", __FUNCTION__);
        return 0;
    }

    for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++)
    {
        unlock_user_memory(&pMemDscr[iBlock]);
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////

void ReleasePhysAddrBuf(struct CDmaChannel *dma)
{
    u32 iBlock = 0;
    //u32 order = 0;
    SHARED_MEMORY_DESCRIPTION *pMemDscr = (SHARED_MEMORY_DESCRIPTION*)dma->m_pBufDscr.SystemAddress;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    if(dma->m_UseCount) {

        for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++)
        {
            //iounmap(pMemDscr[iBlock].SystemAddress);
            pMemDscr[iBlock].SystemAddress = NULL;
            pMemDscr[iBlock].LogicalAddress = 0;
        }
    } else {
        err_msg(err_trace, "%s(): Memory not allocated.\n", __FUNCTION__);
    }
}

///////////////////////////////////////////////////////////////////////////////
// размещение в системном адресном пространстве памяти,
// доступной для операций ПДП и отображаемой в пользовательское пространство
///////////////////////////////////////////////////////////////////////////////

void fillMemoryBlock(void *blockAddr, int blockSize)
{
    int i=0;
    u32* data = (u32*)blockAddr;
    for(i=0; i<blockSize/4; i++)
        data[i] = i+1;
}

int RequestSysBuf(struct CDmaChannel *dma, void **pMemPhysAddr)
{
    u32 iBlock = 0;
    u32 order = 0;
    SHARED_MEMORY_DESCRIPTION *pMemDscr = (SHARED_MEMORY_DESCRIPTION*)dma->m_pBufDscr.SystemAddress;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    order = get_order(dma->m_BlockSize);
    dma->m_IrqCount = dma->m_BlockCount;

    for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++)
    {
        dma_addr_t 	 LogicalAddress;
        void 		*pSystemAddress = NULL;

        pSystemAddress = dma_alloc_coherent(  dma->m_dev, dma->m_BlockSize, &LogicalAddress, GFP_KERNEL );
        //pSystemAddress = (void*)__get_free_pages(GFP_KERNEL, order);
        if(!pSystemAddress) {
            err_msg(err_trace, "%s(): Not enought memory for %i block location. m_BlockSize = %X, BlockOrder = %d\n",
                    __FUNCTION__, (int)iBlock, (int)dma->m_BlockSize, (int)order );

            if(iBlock == 0)
                return -ENOMEM;

            break;
        }

        pMemDscr[iBlock].SystemAddress = pSystemAddress;
        pMemDscr[iBlock].LogicalAddress = LogicalAddress;
        pMemDscr[iBlock].PageCount = 0;
        pMemDscr[iBlock].Pages = 0;
        pMemDscr[iBlock].PageAddresses = 0;
        //pMemDscr[iBlock].LogicalAddress = virt_to_bus(pSystemAddress);
        pMemDscr[iBlock].EotFlag = 1;

        lock_pages( pMemDscr[iBlock].SystemAddress, dma->m_BlockSize );

        pMemPhysAddr[iBlock] = (void*)((size_t)pMemDscr[iBlock].LogicalAddress);

        dbg_msg(dbg_trace, "%s(): %i: %p\n", __FUNCTION__, iBlock, pMemPhysAddr[iBlock]);

        //fillMemoryBlock(pSystemAddress, dma->m_BlockSize);
    }

    dma->m_BlockCount = iBlock;
    dma->m_ScatterGatherTableEntryCnt = iBlock;
    dma->m_IrqCount = dma->m_BlockCount;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////

void ReleaseSysBuf(struct CDmaChannel *dma)
{
    u32 iBlock = 0;
    //u32 order = 0;
    SHARED_MEMORY_DESCRIPTION *pMemDscr = (SHARED_MEMORY_DESCRIPTION*)dma->m_pBufDscr.SystemAddress;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    //order = get_order(dma->m_BlockSize);

    if(dma->m_UseCount) {

        if(!pMemDscr) {
            err_msg(err_trace, "%s(): MemDscr is NULL. Possible memory already released.\n", __FUNCTION__);
            return;
        }

        for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++)
        {
            unlock_pages( pMemDscr[iBlock].SystemAddress, dma->m_BlockSize );

            dma_free_coherent( dma->m_dev, dma->m_BlockSize, pMemDscr[iBlock].SystemAddress, pMemDscr[iBlock].LogicalAddress );
            //free_pages((size_t)pMemDscr[iBlock].SystemAddress, order);

            pMemDscr[iBlock].SystemAddress = NULL;
            pMemDscr[iBlock].LogicalAddress = 0;
        }
    } else {
        err_msg(err_trace, "%s(): Memory not allocated.\n", __FUNCTION__);
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////////

int RequestStub(struct CDmaChannel *dma, void **pStubPhysAddr)
{
    dma_addr_t LogicalAddress = 0;
    u32	StubSize = 0;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__ );

    if(!dma)
        return -EINVAL;

    StubSize = sizeof(AMB_STUB) > PAGE_SIZE ? sizeof(AMB_STUB) : PAGE_SIZE;

    //dbg_msg(dbg_trace, "%s() 0\n", __FUNCTION__ );

    if(!dma->m_UseCount)
    {
        void *pStub = dma_alloc_coherent( dma->m_dev, StubSize, &LogicalAddress, GFP_KERNEL );
        if(!pStub)
        {
            err_msg(err_trace, "%s(): Not enought memory for stub\n", __FUNCTION__);
            return -ENOMEM;
        }

        memset(pStub, 0, StubSize);

        lock_pages( pStub, StubSize );

        //dbg_msg(dbg_trace, "%s() 1\n", __FUNCTION__ );

        dma->m_StubDscr.SystemAddress = pStub;
        dma->m_StubDscr.LogicalAddress = LogicalAddress;
        dma->m_pStub = (AMB_STUB*)pStub; 	  //может быть в этом нет необходимости,
        //но в дальнейшем в модуле используется dma->m_pStub
    }

    //dbg_msg(dbg_trace, "%s() 2\n", __FUNCTION__ );

    pStubPhysAddr[0] = (void*)((size_t)dma->m_StubDscr.LogicalAddress);

    dbg_msg(dbg_trace, "%s(): Stub physical address: %llx\n", __FUNCTION__, (u64)dma->m_StubDscr.LogicalAddress);
    dbg_msg(dbg_trace, "%s(): Stub virtual address: %p\n", __FUNCTION__, dma->m_StubDscr.SystemAddress);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

void ReleaseStub(struct CDmaChannel *dma)
{
    u32 StubSize = sizeof(AMB_STUB) > PAGE_SIZE ? sizeof(AMB_STUB) : PAGE_SIZE;

    if(dma->m_UseCount)
    {
        dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

        unlock_pages(dma->m_StubDscr.SystemAddress, StubSize);

        dma_free_coherent(dma->m_dev, StubSize,
                          dma->m_StubDscr.SystemAddress,
                          dma->m_StubDscr.LogicalAddress);
        dma->m_pStub = NULL;
        dma->m_StubDscr.SystemAddress = NULL;
        dma->m_StubDscr.LogicalAddress = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// вызываем только при первичном размещении буфера
///////////////////////////////////////////////////////////////////////////////

int RequestSGList(struct CDmaChannel *dma)
{
    u32 SGListSize = 0;
    u32 i;
    u32 SGContainerCnt = 0;
    struct DMA_SG_CONTAINER *pSGContainer;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    SGListSize = sizeof(DMA_CHAINING_DESCRIPTOR) * dma->m_ScatterGatherTableEntryCnt;

    if(dma->m_idBlockFifo == PE_EXT_FIFO_ID)
    {
        dma->m_ScatterGatherBlockCnt = dma->m_ScatterGatherTableEntryCnt / (DSCR_BLOCK_SIZE-1);
        dma->m_ScatterGatherBlockCnt = (dma->m_ScatterGatherTableEntryCnt % (DSCR_BLOCK_SIZE-1)) ? (dma->m_ScatterGatherBlockCnt+1) : dma->m_ScatterGatherBlockCnt;
        SGListSize = sizeof(DMA_CHAINING_DESCR_EXT) * DSCR_BLOCK_SIZE * dma->m_ScatterGatherBlockCnt;
        dbg_msg(dbg_trace, "%s(): SGBlockCnt = %d, SGListSize = %d.\n", __FUNCTION__, dma->m_ScatterGatherBlockCnt, SGListSize);
    }

    SGContainerCnt = (SGListSize / SG_CONTAINER_SIZE);
    SGContainerCnt =  (SGListSize % SG_CONTAINER_SIZE) ? SGContainerCnt+1 : SGContainerCnt;

    for(i = 0; i < SGContainerCnt; i++)
    {
        dbg_msg(dbg_trace, "%s(): i = %d\n", __FUNCTION__, i);

        pSGContainer = (struct DMA_SG_CONTAINER*)kmalloc( sizeof(struct DMA_SG_CONTAINER), GFP_KERNEL );

        if(!pSGContainer)
        {
            err_msg(err_trace, "%s(): Not enought memory for scatter/gather list\n", __FUNCTION__);
            return -ENOMEM;
        }

        INIT_LIST_HEAD(&pSGContainer->m_list);

        //	выделяем память под список
        pSGContainer->m_va = dma_alloc_coherent( dma->m_dev, SG_CONTAINER_SIZE, &pSGContainer->m_pa, GFP_KERNEL);

        if(!pSGContainer->m_va)
        {
            err_msg(err_trace, "%s(): Not enought memory for scatter/gather list\n", __FUNCTION__);
            return -ENOMEM;
        }

        // закрепляем список в физической памяти
        lock_pages(pSGContainer->m_va, SG_CONTAINER_SIZE);

        dbg_msg(dbg_trace, "%s(): pSGContainer = %p.\n", __FUNCTION__, pSGContainer);
        dbg_msg(dbg_trace, "%s(): m_va = %p, m_pa = 0x%lld.\n", __FUNCTION__, pSGContainer->m_va, (u64)pSGContainer->m_pa);

        list_add_tail(&pSGContainer->m_list, &dma->m_sg_blocks_list);

        if(i == 0)
        {
            dma->m_SGTableDscr.LogicalAddress = pSGContainer->m_pa;
            dma->m_SGTableDscr.SystemAddress  = pSGContainer->m_va;
        }
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

void ReleaseSGList(struct CDmaChannel *dma)
{
    u32 SGListMemSize = 0;
    u32 SGListSize = 0;
    struct DMA_SG_CONTAINER *pSGContainer;
    struct list_head *p, *n;
    u32 i = 0;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    if(dma->m_idBlockFifo == PE_EXT_FIFO_ID)
    {
        SGListSize = sizeof(DMA_CHAINING_DESCR_EXT) * DSCR_BLOCK_SIZE * dma->m_ScatterGatherBlockCnt;
    } else {
        SGListSize = sizeof(DMA_CHAINING_DESCRIPTOR) * dma->m_ScatterGatherTableEntryCnt;
    }

    SGListMemSize = (SGListSize >= PAGE_SIZE) ? SGListSize : PAGE_SIZE;

    dma->m_SGTableDscr.SystemAddress = NULL;
    dma->m_SGTableDscr.LogicalAddress = 0;

    list_for_each_safe(p, n, &dma->m_sg_blocks_list)
    {
        pSGContainer = list_entry(p, struct DMA_SG_CONTAINER, m_list);

        dbg_msg(dbg_trace, "%s(): i = %d.\n", __FUNCTION__, i++);
        dbg_msg(dbg_trace, "%s(): pSGContainer = %p.\n", __FUNCTION__, pSGContainer);

        if(!pSGContainer) {
            continue;
        }

        dbg_msg(dbg_trace, "%s(): m_va = %p, m_pa = 0x%lld.\n", __FUNCTION__, pSGContainer->m_va, (u64)pSGContainer->m_pa);

        unlock_pages(pSGContainer->m_va, SG_CONTAINER_SIZE);

        dma_free_coherent( dma->m_dev, SG_CONTAINER_SIZE,
                           pSGContainer->m_va,
                           pSGContainer->m_pa );

        list_del(p);

        kfree( (void*)pSGContainer );
    }

    INIT_LIST_HEAD(&dma->m_sg_blocks_list);
}

///////////////////////////////////////////////////////////////////////////////

int StartDmaTransfer(struct CDmaChannel *dma, u32 IsCycling)
{

    u32 iLastEntry;
    u64 *pDscrBuf = NULL;
    DMA_CHAINING_DESCR_EXT	*pSGTEx = NULL;

    //dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    ResetEvent(&dma->m_BlockEndEvent);
    ResetEvent(&dma->m_BufferEndEvent);

    dma->m_DmaCycling = IsCycling;
    dma->m_DoneBlock = -1;
    dma->m_DoneFlag = 1;
    dma->m_CycleNum	= 0;
    dma->m_BlocksRemaining	= dma->m_BlockCount;
    dma->m_CurBlockNum = 0;
    dma->m_CurIrqNum = 0;
    dma->m_preBlockCount1 = 1;
    dma->m_preBlockCount2 = 2;
    dma->m_preBlockCount3 = 3;

    if(dma->m_idBlockFifo == PE_EXT_FIFO_ID)
    {
        u32 ctrl_code = ~0;
        u16* pDscr = NULL;
        int iEntry = 0;
        iLastEntry = dma->m_ScatterGatherTableEntryCnt + dma->m_ScatterGatherBlockCnt - 1;
        dbg_msg(dbg_trace, "%s(): dma->m_ScatterGatherTableEntryCnt = %d\n", __FUNCTION__, dma->m_ScatterGatherTableEntryCnt);
        dbg_msg(dbg_trace, "%s(): dma->m_ScatterGatherBlockCnt = %d\n", __FUNCTION__, dma->m_ScatterGatherBlockCnt);
        dbg_msg(dbg_trace, "%s(): iLastEntry = %d\n", __FUNCTION__, iLastEntry);
        
        pSGTEx = GetSGTExEntry(dma, iLastEntry-1, 0);

        if(dma->m_ScatterGatherBlockCnt == 1)
        {
            pSGTEx->Cmd.JumpDescr0 = dma->m_DmaCycling;
            pSGTEx->Cmd.JumpNextBlock = 0;
        }
        else
        {
            pSGTEx->Cmd.JumpNextBlock = dma->m_DmaCycling;
            pSGTEx->Cmd.JumpDescr0 = 0;
        }

        //dbg_msg(dbg_trace, "%s(): m_DmaCycling = %d\n", __FUNCTION__, dma->m_DmaCycling);

        pDscrBuf = (u64*)GetSGTExEntry(dma, DSCR_BLOCK_SIZE * (dma->m_ScatterGatherBlockCnt-1), 0);

        dbg_msg(dbg_trace, "%s(): dma->m_pScatterGatherTableExt = %p\n", __FUNCTION__, (u64*)dma->m_pScatterGatherTableExt);
        dbg_msg(dbg_trace, "%s(): pDscrBuf = %p\n", __FUNCTION__, pDscrBuf);

        ctrl_code = 0xFFFFFFFF;
        pDscr = (u16*)pDscrBuf;
        pDscr[255] = 0;
        for(iEntry = 0; iEntry < DSCR_BLOCK_SIZE; iEntry++)
        {
            u16 data0 = (u16)(pDscrBuf[iEntry] & 0xFFFF);
            u16 data1 = (u16)((pDscrBuf[iEntry] >> 16) & 0xFFFF);
            u16 data2 = (u16)((pDscrBuf[iEntry] >> 32) & 0xFFFF);
            u16 data3 = (u16)((pDscrBuf[iEntry] >> 48) & 0xFFFF);
            if(iEntry == DSCR_BLOCK_SIZE-1)
            {
                ctrl_code = ctrl_code ^ data0 ^ data1 ^ data2 ^ data3;

                dbg_msg(dbg_trace, "%s(): DSCR_BLK[%d] - NextBlkAddr = 0x%08X, Signature = 0x%04X, Crc = 0x%04X\n",
                        __FUNCTION__,
                        dma->m_ScatterGatherBlockCnt-1,
                        (u32)(pDscrBuf[iEntry] << 8),
                        (u16)((pDscrBuf[iEntry] >> 32) & 0xFFFF),
                        (u16)ctrl_code);
            }
            else
            {
                u32 ctrl_tmp = 0;
                ctrl_code = ctrl_code ^ data0 ^ data1 ^ data2 ^ data3;
                ctrl_tmp = ctrl_code << 1;
                ctrl_tmp |= (ctrl_code & 0x8000) ? 0: 1;
                ctrl_code = ctrl_tmp;

                // 3 способа циклического сдвига влево 32-разрядного значения
                // (у нас оказалось не совсем то: значение берется 16-разрядное и старший бит при переносе в младший инвертируется)
                //_rotl(ctrl_code, 1);
                //ctrl_code = (ctrl_code << 1) | (ctrl_code >> 31);
                //__asm {
                //	rol ctrl_code,1
                //}

                //dbg_msg(dbg_trace, "%s(): %d(%d) - PciAddr = 0x%08X, Cmd = 0x%02X, DmaLength = %d(%02X %02X %02X)\n",
                //                                    __FUNCTION__,
                //                                    iEntry, dma->m_ScatterGatherBlockCnt-1,
                //                                    (u32)(pDscrBuf[iEntry] << 8),
                //                                    (u16)(pDscrBuf[iEntry] >> 32),
                //                                    (u32)((pDscrBuf[iEntry] >> 41) << 9),
                //                                    (u8)(pDscrBuf[iEntry] >> 56), (u8)(pDscrBuf[iEntry] >> 48), (u8)(pDscrBuf[iEntry] >> 40));
            }
            if(iEntry < DSCR_BLOCK_SIZE-1) {
                dbg_msg(dbg_trace, "%s(): %d - 0x%04X%04X%04X%04X\n",__FUNCTION__,iEntry,data3,data2,data1,data0);
            }
        }

        pDscr[255] |= (u16)ctrl_code;

        {
            u16 data0 = (u16)(pDscrBuf[DSCR_BLOCK_SIZE-1] & 0xFFFF);
            u16 data1 = (u16)((pDscrBuf[DSCR_BLOCK_SIZE-1] >> 16) & 0xFFFF);
            u16 data2 = (u16)((pDscrBuf[DSCR_BLOCK_SIZE-1] >> 32) & 0xFFFF);
            u16 data3 = (u16)((pDscrBuf[DSCR_BLOCK_SIZE-1] >> 48) & 0xFFFF);
            dbg_msg(dbg_trace, "%s(): %d - 0x%04X%04X%04X%04X\n",__FUNCTION__,DSCR_BLOCK_SIZE-1,data3,data2,data1,data0);
        }

    }
    else
    {
        u32 *pElemSG = NULL;
        u32 ctrl_code = 0;
        int idx = 0;

        dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].Cmd.EndOfChain = !dma->m_DmaCycling;
        dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].Crc = 0;

        pElemSG = (u32*)&(dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1]);
        ctrl_code = 0;

        for(idx = 0; idx < 8; idx++)
        {
            u32 val = pElemSG[idx];
            ctrl_code = ctrl_code ^ val;
            ctrl_code = ctrl_code ^ (~(val >> 16));
        }
        ctrl_code &= 0xFFFF;
        dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].Crc = ctrl_code;

        dbg_msg(dbg_trace, "%s(): %d - PciAddrLo = 0x%08X, PciAddrHi = 0x%08X, DmaLength = %d, NextAddrLo = 0x%08X\n",
                __FUNCTION__,
                dma->m_ScatterGatherTableEntryCnt - 1,
                dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].PciAddrLo,
                dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].PciAddrHi,
                dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].DmaLength,
                dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].NextAddrLo );
        dbg_msg(dbg_trace, "%s(): EndOfChain = %d, EndOfTrans = %d, Signature = 0x%08X, Crc = 0x%08X\n", __FUNCTION__,
                dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].Cmd.EndOfChain,
                dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].Cmd.EndOfTrans,
                dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].Signature,
                dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].Crc );
    }
    dma->m_pStub->lastBlock = -1;
    dma->m_pStub->totalCounter = 0;
    dma->m_pStub->state = STATE_RUN;

    wmb();

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

u32 NextDmaTransfer(struct CDmaChannel *dma)
{
    SHARED_MEMORY_DESCRIPTION *pMemDscr = (SHARED_MEMORY_DESCRIPTION*)dma->m_pBufDscr.SystemAddress;
    dbg_msg(dbg_trace, "%s(): last block = %d, cycle counter = %d\n", __FUNCTION__, dma->m_pStub->lastBlock, dma->m_CycleNum );

    if(dma->m_MemType == PHYS_MEMORY_TYPE)
    {
        dbg_msg(dbg_trace, "%s(): current IRQ number = %d\n", __FUNCTION__, dma->m_CurIrqNum );
        if(dma->m_pStub->lastBlock + pMemDscr[dma->m_CurIrqNum].BlockInc >= (long)dma->m_BlockCount)
            dma->m_pStub->lastBlock = dma->m_pStub->lastBlock + pMemDscr[dma->m_CurIrqNum].BlockInc - dma->m_BlockCount;
        else
            dma->m_pStub->lastBlock += pMemDscr[dma->m_CurIrqNum].BlockInc;

        dma->m_CurBlockNum += pMemDscr[dma->m_CurIrqNum].BlockInc;
        dma->m_pStub->totalCounter += pMemDscr[dma->m_CurIrqNum].BlockInc;
        dma->m_BlocksRemaining -= pMemDscr[dma->m_CurIrqNum].BlockInc;
        dbg_msg(dbg_trace, "%s(): block increment = %d, block remain = %d\n", __FUNCTION__, pMemDscr[dma->m_CurIrqNum].BlockInc, dma->m_BlocksRemaining );
        if((dma->m_CurIrqNum + 1 >= dma->m_IrqCount) && (dma->m_BlocksRemaining > 0) && dma->m_DmaCycling)
        {
            pMemDscr[0].BlockInc += dma->m_BlocksRemaining;
            dma->m_BlocksRemaining = 0;
        }
    }
    else
    {
        if(dma->m_pStub->lastBlock + 1 >= (long)dma->m_BlockCount)
            dma->m_pStub->lastBlock = 0;
        else
            dma->m_pStub->lastBlock++;

        dma->m_CurBlockNum++;
        dma->m_pStub->totalCounter++;
        dma->m_BlocksRemaining--;
    }

    dma->m_CurIrqNum++;

    if(dma->m_BlocksRemaining <= 0 && dma->m_DmaCycling)
    {
        dma->m_BlocksRemaining = dma->m_BlockCount;
        dma->m_CurBlockNum = 0;
        dma->m_CycleNum++;
        dma->m_CurIrqNum = 0;
    }

    if(dma->m_preBlockCount1+1 == dma->m_BlockCount)
    {
        dma->m_preBlockCount1 = 0;
    } else {
        dma->m_preBlockCount1++;
    }

    if(dma->m_preBlockCount2+1 == dma->m_BlockCount)
    {
        dma->m_preBlockCount2 = 0;
    } else {
        dma->m_preBlockCount2++;
    }

    if(dma->m_preBlockCount3+1 == dma->m_BlockCount)
    {
        dma->m_preBlockCount3 = 0;
    } else {
        dma->m_preBlockCount3++;
    }

    if(dma->m_AdjustMode && dma->m_DmaCycling)
    {
        if(((dma->m_preBlockCount3 == 0)&&(dma->m_DoneBlock == -1)) || (dma->m_preBlockCount3 == dma->m_DoneBlock)) {
            dma->m_DoneFlag = 0;
            dbg_msg(dbg_trace, "%s(): m_preBlockCount = %d, m_DoneBlock = %d\n", __FUNCTION__, dma->m_preBlockCount3, dma->m_DoneBlock);
        }
    }

    tasklet_hi_schedule(&dma->m_Dpc);

    return dma->m_DoneFlag;
}

/*
u32 NextDmaTransfer(struct CDmaChannel *dma)
{
    //dbg_msg(dbg_trace, "%s(): last block = %d, cycle counter = %d\n", __FUNCTION__, dma->m_pStub->lastBlock, dma->m_CycleNum );

    if(dma->m_pStub->lastBlock + 1 >= (long)dma->m_BlockCount) {
        dma->m_pStub->lastBlock = 0;
    }  else {
        dma->m_pStub->lastBlock++;
    }

    dma->m_CurBlockNum++;
    dma->m_pStub->totalCounter++;
    dma->m_BlocksRemaining--;

    tasklet_hi_schedule(&dma->m_Dpc);

    //dbg_msg(dbg_trace, "%s(): tasklet_hi_schedule()\n", __FUNCTION__);

    if(dma->m_AdjustMode && dma->m_DmaCycling)
    {
        u32 next_done_blk = (dma->m_DoneBlock == dma->m_BlockCount-1) ? 0 : (dma->m_DoneBlock + 1);
        u32 next_cur_blk = ((dma->m_CurBlockNum + 1) >= dma->m_BlockCount) ? ((dma->m_CurBlockNum + 1) - dma->m_BlockCount) : (dma->m_CurBlockNum + 1);

        long difBlock = next_done_blk - next_cur_blk;

        if(!difBlock)
            dma->m_DoneFlag = 0;

        //dbg_msg(dbg_trace, "%s(): DoneBlock = %d, Nextdb = %d, CurBlock = %d, Nextcb = %d, difBlock = %d, DoneFlag = %d\n",
        //		__FUNCTION__, dma->m_DoneBlock, next_done_blk, dma->m_CurBlockNum,
        //		next_cur_blk, (int)difBlock, dma->m_DoneFlag);
    }

    if(dma->m_BlocksRemaining <= 0 && dma->m_DmaCycling)
    {
        dma->m_BlocksRemaining = dma->m_BlockCount;
        dma->m_CurBlockNum = 0;
        dma->m_CycleNum++;
    }

    //dbg_msg(dbg_trace, "%s(): complete\n", __FUNCTION__);

    return dma->m_DoneFlag;

}
*/

///////////////////////////////////////////////////////////////////////////////

u32 SetDoneBlock(struct CDmaChannel *dma, long numBlk)
{
    if(numBlk != dma->m_DoneBlock)
    {
        dma->m_DoneBlock = numBlk;

        if((dma->m_preBlockCount1 != dma->m_DoneBlock) && (dma->m_preBlockCount2 != dma->m_DoneBlock) && (dma->m_preBlockCount3 != dma->m_DoneBlock)) {

            if(dma->m_AdjustMode && dma->m_DmaCycling && !dma->m_DoneFlag)
            {
                dma->m_DoneFlag = 1;
            }
        }
    }

    //dbg_msg(dbg_trace, "%s(): DoneBlock = %d, DoneFlag = %d\n", __FUNCTION__, dma->m_DoneBlock, dma->m_DoneFlag);

    return dma->m_DoneFlag;

}

///////////////////////////////////////////////////////////////////////////////

void GetState(struct CDmaChannel *dma, u32 *BlockNum, u32 *BlockNumTotal, u32 *OffsetInBlock, u32 *DmaChanState)
{
    //dbg_msg(dbg_trace, "%s(): - last block = %d, cycle counter = %d\n", __FUNCTION__, dma->m_pStub->lastBlock, dma->m_CycleNum);
    if(dma->m_pStub) {
        *BlockNum = dma->m_pStub->lastBlock;
        *BlockNumTotal = dma->m_pStub->totalCounter;
        *OffsetInBlock = dma->m_pStub->offset;
        *DmaChanState = dma->m_pStub->state;
    } else {
        *BlockNum = 0;
        *BlockNumTotal = 0;
        *OffsetInBlock = 0;
        *DmaChanState = STATE_DESTROY;
    }
}

///////////////////////////////////////////////////////////////////////////////

void FreezeState(struct CDmaChannel *dma)
{
    //dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);
}

///////////////////////////////////////////////////////////////////////////////

int WaitBlockEndEvent(struct CDmaChannel *dma, u32 msTimeout)
{
    int status = -ETIMEDOUT;

    //dbg_msg(dbg_trace, "%s(): DMA%d\n", __FUNCTION__, dma->m_NumberOfChannel);

    if( msTimeout < 0 ) {
        status = GrabEvent( &dma->m_BlockEndEvent, -1 );
    } else {
        status = GrabEvent( &dma->m_BlockEndEvent, msTimeout );
    }
    return status;
}

///////////////////////////////////////////////////////////////////////////////

int WaitBufferEndEvent(struct CDmaChannel *dma, u32 msTimeout)
{
    int status = -ETIMEDOUT;

    //dbg_msg(dbg_trace, "%s(): DMA%d\n", __FUNCTION__, dma->m_NumberOfChannel);

    if( msTimeout < 0 ) {
        status = GrabEvent( &dma->m_BufferEndEvent, -1 );
    } else {
        status = GrabEvent( &dma->m_BufferEndEvent, msTimeout );
    }

    return status;
}

///////////////////////////////////////////////////////////////////////////////

int CompleteDmaTransfer(struct CDmaChannel *dma)
{
    u32 ii=0;

    dbg_msg(dbg_trace, " %s(): DMA%d\n", __FUNCTION__, dma->m_NumberOfChannel);

    for( ii=0; ii<dma->m_ScatterGatherBlockCnt*DSCR_BLOCK_SIZE; ii++ )
    {
        u32 *ptr = (u32*)GetSGTExEntry(dma, ii, 0);
        if(ptr) {
            dbg_msg(dbg_trace, "%s(): %d: %.8X %.8X\n", __FUNCTION__, ii, ptr[1], ptr[0]);
        }
    }
    dma->m_pStub->state = STATE_STOP;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

void GetSGStartParams(struct CDmaChannel *dma, u64 *SGTableAddress, u32 *LocalAddress, u32 *DmaDirection)
{
    if(dma->m_idBlockFifo == PE_EXT_FIFO_ID)
    {
        *SGTableAddress = dma->m_SGTableDscr.LogicalAddress;
    }
    else
        *SGTableAddress = dma->m_pScatterGatherTable[dma->m_ScatterGatherTableEntryCnt - 1].NextAddrLo; // адрес таблицы

    *LocalAddress = dma->m_DmaLocalAddress;
    *DmaDirection = dma->m_DmaDirection;
}

///////////////////////////////////////////////////////////////////////////////

void GetStartParams(struct CDmaChannel *dma, u32 *PciAddress, u32 *LocalAddress, u32 *DmaLength)
{
    // возвращает адрес и размер DMA из первого элемента таблицы
    if(dma->m_idBlockFifo == PE_EXT_FIFO_ID)
    {
        u64 *pDscrBuf = (u64*)dma->m_pScatterGatherTableExt;
        *PciAddress = (u32)(pDscrBuf[0] << 16);
        *DmaLength = (u32)((pDscrBuf[0] >> 40) << 8);
    }
    else
    {
        *PciAddress = dma->m_pScatterGatherTable[0].PciAddrLo; // & 0xfffffffe;
        *DmaLength = dma->m_pScatterGatherTable[0].DmaLength; // & 0xfffffffe;
    }
    *LocalAddress = dma->m_DmaLocalAddress;
}

///////////////////////////////////////////////////////////////////////////////

void Adjust(struct CDmaChannel *dma, u32 mode)
{
    //dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);
    dma->m_AdjustMode = mode;
}

///////////////////////////////////////////////////////////////////////////////

void SetAdmTetr(struct CDmaChannel *dma, u32 AdmNum, u32 TetrNum)
{
    //dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);
    dma->m_AdmNum = AdmNum;
    dma->m_TetrNum = TetrNum;
}

///////////////////////////////////////////////////////////////////////////////

void SetDmaLocalAddress(struct CDmaChannel *dma, u32 Address)
{
    //dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);
    dma->m_DmaLocalAddress = Address;
}

///////////////////////////////////////////////////////////////////////////////

int SetDmaDirection(struct CDmaChannel *dma, u32 DmaDirection)
{
    //dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    switch(DmaDirection)
    {
    case 1:
        dma->m_DmaDirection = TRANSFER_DIR_FROM_DEVICE;
        break;
    case 2:
        dma->m_DmaDirection = TRANSFER_DIR_TO_DEVICE;
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

u32 GetAdmNum(struct CDmaChannel *dma)
{
    return dma->m_AdmNum;
}

///////////////////////////////////////////////////////////////////////////////

u32 GetTetrNum(struct CDmaChannel *dma)
{
    return dma->m_TetrNum;
}

///////////////////////////////////////////////////////////////////////////////

DMA_CHAINING_DESCR_EXT *GetSGTExEntry(struct CDmaChannel *dma, u64 iEntry, dma_addr_t *physAddr)
{
    struct list_head *p;
    struct DMA_SG_CONTAINER *pSGContainer;
    DMA_CHAINING_DESCR_EXT *pEntry;
    u64 i = 0, j = 0, cnt = 0;

    j = iEntry % (SG_CONTAINER_SIZE / sizeof(DMA_CHAINING_DESCR_EXT));
    i = (iEntry - j) / (SG_CONTAINER_SIZE / sizeof(DMA_CHAINING_DESCR_EXT));

    list_for_each(p, &dma->m_sg_blocks_list) {
        if(cnt == i)
            break;
        cnt++;
    }

    pSGContainer = list_entry(p, struct DMA_SG_CONTAINER, m_list);
    pEntry = pSGContainer->m_va;
    if(physAddr) {
        *physAddr = (pSGContainer->m_pa + j*sizeof(DMA_CHAINING_DESCR_EXT));
    }

    if(physAddr) {
        dbg_msg(dbg_trace, "%s(): iEntry = %lld, physAddr = 0x%llx, i = %lld, j = %lld.\n", __FUNCTION__, iEntry, (unsigned long long)*physAddr, i, j);
    }


    return pEntry + j;
}

///////////////////////////////////////////////////////////////////////////////

//
// End of file
//
