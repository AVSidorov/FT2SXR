//****************** File DmaChannel.cpp *******************************
//  CDmaChannel Class Implementation
//
//	Copyright (c) 2004, Instrumental Systems,Corp.
//  Written by Dorokhin Andrey
//
//  History:
//  10-10-04 - builded
//
//*******************************************************************

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/pagemap.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include "pagehelp.h"
#include "dmachannel.h"

//*******************************************************************

u32 GetAdmNum(struct CDmaChannel *dma) {return dma->m_AdmNum;}
u32 GetTetrNum(struct CDmaChannel *dma) {return dma->m_TetrNum;}
u32 GetLocalBusWidth(struct CDmaChannel *dma) {return dma->m_LocalBusWidth;}

//*******************************************************************
int SetDmaDirection(CDmaChannel *dma, u32 DmaDirection)
{
    u32 iEntry;

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
    if(dma->m_UseCount) {
        for(iEntry = 0; iEntry < dma->m_ScatterGatherTableEntryCnt; iEntry++) {
            if(dma->m_pScatterGatherTableVA)
                dma->m_pScatterGatherTableVA[iEntry].Next.DirFromDevice = dma->m_DmaDirection;
        }
    }
    //printk("<0>%s(%d): %x\n", __FUNCTION__, dma->m_NumberOfChannel, dma->m_DmaDirection);
    return 0;
}

//*****************************************************************************
void SetDmaLocalAddress(CDmaChannel *dma, u32 Address)
{
    u32 iEntry;
    dma->m_DmaLocalAddress = Address;
    if(dma->m_UseCount) {
        for(iEntry = 0; iEntry < dma->m_ScatterGatherTableEntryCnt; iEntry++) {
            if(dma->m_pScatterGatherTableVA)
                dma->m_pScatterGatherTableVA[iEntry].DmaLocalAddress = dma->m_DmaLocalAddress;
        }
    }
    //printk("<0>%s(%d): %x\n", __FUNCTION__, dma->m_NumberOfChannel, dma->m_DmaLocalAddress);
}

//*******************************************************************
void SetLocalBusWidth(CDmaChannel *dma, u32 Param)
{
    u32 tmp = (Param >> 8) & 0xff;
    switch(tmp)
    {
    case 0:
        dma->m_LocalBusWidth = LBW_32bit;
        break;
    case 1:
        dma->m_LocalBusWidth = LBW_16bit;
        break;
    case 2:
        break;
    }
}

//*******************************************************************
void ReferenceBlockEndEvent(struct CDmaChannel *dma, void* hBlockEndEvent)
{
}

//*******************************************************************
void DereferenceBlockEndEvent(struct CDmaChannel *dma)
{
}

//********************************************************
struct CDmaChannel *CDmaChannelCreate(	void *ambp, struct device *dev,
        u32	NumberOfChannel,
        TASKLET_ROUTINE  DeferredRoutine,
        u32  cbMaxTransferLength,
        int    bScatterGather )
{
    struct CDmaChannel *dma = NULL;

    dma = kmalloc(sizeof(struct CDmaChannel), GFP_KERNEL);
    if(!dma) {
        printk("<0> %s: Error allocate memory for CDmaChannel object\n", __FUNCTION__);
        return NULL;
    }

    memset(dma, 0, sizeof(CDmaChannel));

    dma->m_Board = ambp;
    dma->m_NumberOfChannel = NumberOfChannel;
    dma->m_dev = dev;
    dma->m_UseCount = 0;
    dma->m_pUserAddress = NULL;
    dma->m_LocalBusWidth = LBW_32bit;
    dma->WambpDpcForIsr = DeferredRoutine;

    spin_lock_init( &dma->m_DmaLock );
    init_waitqueue_head( &dma->m_DmaWq );
    tasklet_init( &dma->m_Dpc, dma->WambpDpcForIsr, (unsigned long)dma );
    InitKevent( &dma->m_BlockEndEvent );
    InitKevent( &dma->m_BufferEndEvent );
    //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    return dma;
}

//********************************************************
void CDmaChannelDelete(struct CDmaChannel *dma)
{
    if (dma) {
        tasklet_kill( &dma->m_Dpc );
        //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);
        kfree(dma);
    }
}

//*****************************************************************************

int RequestMemory(struct CDmaChannel *dma,void **ppMemPhysAddr,u32 size,u32*pCount,void **pStubPhysAddr, u32 bMemType)
{
    int 		Status = 0;

    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if(dma->m_UseCount) {
        printk("<0>%s(): Memory already allocated.\n", __FUNCTION__);
        return -EINVAL;
    }

    dma->m_MemType = bMemType;
    dma->m_BlockCount = *pCount;
    dma->m_BlockSize = size;

    dma->m_BufDscrVA = (PSHARED_MEMORY_DESCRIPTION)dma_alloc_coherent(dma->m_dev,
                                                                      dma->m_BlockCount * sizeof(SHARED_MEMORY_DESCRIPTION),
                                                                      &dma->m_BufDscrPA, GFP_KERNEL );

    if(!dma->m_BufDscrVA)
    {
        printk("<0>%s(): Not enought memory for buffer descriptions\n", __FUNCTION__);
        return ENOMEM;
    }

    memset(dma->m_BufDscrVA, 0, dma->m_BlockCount * sizeof(SHARED_MEMORY_DESCRIPTION));

    printk("<0>%s(): dma->m_BufDscrVA = %p\n", __FUNCTION__, dma->m_BufDscrVA);

    dma->m_ScatterGatherTableEntryCnt = 0;

    Status = RequestStub(dma, pStubPhysAddr);
    if(Status == STATUS_SUCCESS)
    {
        if(dma->m_MemType == SYSTEM_MEMORY_TYPE) {
            Status = RequestSysBuf(dma, ppMemPhysAddr);
        }
        else {
            Status = RequestUsrBuf(dma, ppMemPhysAddr);
        }
        if(Status == STATUS_SUCCESS)
        {
            printk("<0>%s(): Scatter/Gather Table Entry is %d\n", __FUNCTION__, dma->m_ScatterGatherTableEntryCnt);
            Status = SetScatterGatherList(dma);
            if(Status != STATUS_SUCCESS)
            {
                FreeUserAddress(dma);
                ReleaseStub(dma);
                dma_free_coherent(dma->m_dev,dma->m_BlockCount * sizeof(SHARED_MEMORY_DESCRIPTION),dma->m_BufDscrVA,dma->m_BufDscrPA);
                dma->m_BufDscrVA = NULL;

                return Status;
            }

            *pCount = dma->m_BlockCount;

            dma->m_pStub = (PAMB_STUB)dma->m_StubDscr.SystemAddress;
            dma->m_pStub->lastBlock = -1;
            dma->m_pStub->totalCounter = 0;
            dma->m_pStub->offset = 0;
            dma->m_pStub->state = STATE_STOP;
            dma->m_UseCount++;
        }
        else
        {
            ReleaseStub(dma);
            dma_free_coherent(dma->m_dev,dma->m_BlockCount * sizeof(SHARED_MEMORY_DESCRIPTION),dma->m_BufDscrVA,dma->m_BufDscrPA);
            dma->m_BufDscrVA = NULL;
        }
    }
    else
{
    dma_free_coherent(dma->m_dev,dma->m_BlockCount * sizeof(SHARED_MEMORY_DESCRIPTION),dma->m_BufDscrVA,dma->m_BufDscrPA);
    dma->m_BufDscrVA = NULL;
}
return Status;
}

//*****************************************************************************

void ReleaseMemory(struct CDmaChannel *dma)
{
    if(!dma->m_UseCount) {
        printk("<0> %s: Memory not allocated.\n", __FUNCTION__);
        return;
    }

    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    FreeUserAddress( dma );
    ReleaseSGList( dma );

    dma_free_coherent(dma->m_dev,dma->m_BlockCount*sizeof(SHARED_MEMORY_DESCRIPTION),dma->m_BufDscrVA, dma->m_BufDscrPA);

    dma->m_BufDscrVA = NULL;
    dma->m_UseCount--;
}

//*****************************************************************************

void FreeUserAddress(struct CDmaChannel *dma)
{
    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if(dma->m_UseCount)
    {
        if(dma->m_MemType == SYSTEM_MEMORY_TYPE) {
            ReleaseSysBuf(dma);
        }
        if(dma->m_MemType == USER_MEMORY_TYPE) {
            ReleaseUsrBuf(dma);
        }
        ReleaseStub(dma);
    }
}

//*****************************************************************************

int SetScatterGatherList(struct CDmaChannel *dma)
{
    int Status = STATUS_SUCCESS;
    u32 iBlock = 0;
    u32 iPage = 0;
    u32 iEntry = 0;
    SHARED_MEMORY_DESCRIPTION *pBuffer = NULL;
    DMA_CHAINING_DESCRIPTOR *pTable = NULL;
    dma_addr_t tablePhysAddr = 0;

    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    Status = RequestSGList(dma);
    if(Status < 0)
        return Status;

    dma->m_pScatterGatherTableVA = dma->m_SGTableDscrVA;
    pTable = dma->m_SGTableDscrVA;
    pBuffer = dma->m_BufDscrVA;
    tablePhysAddr = dma->m_SGTableDscrPA;
    iEntry = 0;

    printk("<0> %s: m_SGTableDscrVA = %p\n", __FUNCTION__, pTable);
    printk("<0> %s: m_BufDscrVA = %p\n", __FUNCTION__, pBuffer);
    printk("<0> %s: tablePhysAddr = 0x%llx\n", __FUNCTION__, tablePhysAddr);

    for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++)
    {
        if(dma->m_MemType == USER_MEMORY_TYPE) {

            printk("<0>%s(): Block %i. Page in block %ld\n", __FUNCTION__, iBlock, pBuffer[iBlock].PageCount);

            for(iPage = 0; iPage < pBuffer[iBlock].PageCount; iPage++) {

                dma_addr_t address = page_to_phys(pBuffer[iBlock].Pages[iPage]);

                printk("<0>%s(): Page physical address %x\n", __FUNCTION__, (int)address);

                pTable[iEntry].DmaPciAddress   = address;
                pTable[iEntry].DmaLocalAddress = dma->m_DmaLocalAddress;
                pTable[iEntry].DmaLength      = PAGE_SIZE;

                pTable[iEntry].Next.LocationPCI	= 1;
                pTable[iEntry].Next.EndOfChain	= 0;
                pTable[iEntry].Next.IntrTC	= 0;
                pTable[iEntry].Next.DirFromDevice = dma->m_DmaDirection;

                if((iPage + 1) == pBuffer[iBlock].PageCount) {
                    pTable[iEntry].Next.IntrTC = 1;
                    printk("<0>%s(): iEntry = %d, iPage = %d: IntrTC = 1\n", __FUNCTION__, iEntry, iPage);
                }

                if((iEntry + 1) == dma->m_ScatterGatherTableEntryCnt)
                    tablePhysAddr = dma->m_SGTableDscrPA;
                else
                    tablePhysAddr += sizeof(DMA_CHAINING_DESCRIPTOR);

                pTable[iEntry].Next.Address = NexDscrAddress(tablePhysAddr);

                printk("<0> %s: tablePhysAddr[%d] = 0x%llx\n", __FUNCTION__, iEntry, tablePhysAddr);
                printk("<0> %s: NexDscrAddress[%d] = %x\n", __FUNCTION__, iEntry, (int)NexDscrAddress(tablePhysAddr));
                printk("<0>%s(): iEntry = %d, iPage = %d, Addr %p\n", __FUNCTION__, iEntry, iPage, pBuffer[iBlock].Pages[iPage]);

                iEntry++;
            }
        }

        if(dma->m_MemType == SYSTEM_MEMORY_TYPE) {

            dma_addr_t address = pBuffer[iBlock].LogicalAddress;

            pTable[iBlock].DmaPciAddress   = address;
            pTable[iBlock].DmaLocalAddress = dma->m_DmaLocalAddress;
            pTable[iBlock].DmaLength      = dma->m_BlockSize;

            pTable[iBlock].Next.LocationPCI	= 1;
            pTable[iBlock].Next.EndOfChain	= 0;
            pTable[iBlock].Next.IntrTC	= 1;
            pTable[iBlock].Next.DirFromDevice = dma->m_DmaDirection;

            if((iBlock + 1) == dma->m_ScatterGatherTableEntryCnt)
                tablePhysAddr = dma->m_SGTableDscrPA;
            else
                tablePhysAddr += sizeof(DMA_CHAINING_DESCRIPTOR);

            //printk("<0> %s: pTablePhysAddr[%d] = %p\n", __FUNCTION__, iEntry, pTablePhysAddr);

            dma->m_pScatterGatherTableVA[iBlock].Next.Address = NexDscrAddress(tablePhysAddr);

            //printk("<0> %s: NexDscrAddress[%d] = %x\n", __FUNCTION__, iEntry, (int)NexDscrAddress(pTablePhysAddr));
        }
    }

    dma->m_pScatterGatherTableVA[dma->m_ScatterGatherTableEntryCnt-1].Next.EndOfChain = 1;

    printk("<0> %s(): DmaDirection = %d, DmaLocalAddress = 0x%X\n", __FUNCTION__, dma->m_DmaDirection, dma->m_DmaLocalAddress);

    for(iEntry = 0; iEntry < dma->m_ScatterGatherTableEntryCnt; iEntry++) {
        printk("<0> %d - DmaPciAddress = 0x%X, DmaLength = %d, Next.Address = 0x%X\n",
               iEntry,
               dma->m_pScatterGatherTableVA[iEntry].DmaPciAddress,
               dma->m_pScatterGatherTableVA[iEntry].DmaLength,
               dma->m_pScatterGatherTableVA[iEntry].Next.Address);
    }

    return Status;
}

//***************************************************************************************
dma_addr_t NexDscrAddress(dma_addr_t physAddr)
{
    return (physAddr>>4);
}

//*******************************************************************

static int CheckUserAddress( void *pMemUserAddr )
{
    size_t addr = (size_t)pMemUserAddr;
    size_t mask = (size_t)~PAGE_MASK;

    //printk("<0>%s()\n", __FUNCTION__);

    // адрес пользовательского буфера должен быть выровнен на страницу
    if(addr & mask) {
        printk("<0>%s(): %p - Error! Address must be aling at PAGE_SIZE border\n", __FUNCTION__, pMemUserAddr );
    	return 1;
    }

    return 0;
}

//*******************************************************************

static int CheckUserSize( size_t userSize )
{
    //printk("<0>%s()\n", __FUNCTION__);

    // размер пользовательского буфера должен быть кратен размеру страницы
    if((userSize % PAGE_SIZE) != 0) {
        printk("<0>%s(): Invalid user memory block size - 0x%lX.\n", __FUNCTION__, userSize);
        return 1;
    }

    return 0;
}

//*******************************************************************

static int UserMemoryLock( SHARED_MEMORY_DESCRIPTION *MemDscr, void* userSpaceAddress, size_t userSpaceSize )
{
    int i = 0;
    int requested_page_count = 0;
    int allocated_page_count = 0;
    int errcode = 0;

    printk("<0>%s()\n", __FUNCTION__);

    if(!MemDscr) {
        printk("<0>%s(): Invalid memory descriptor pointer.\n", __FUNCTION__);
        errcode = -EINVAL;
        goto err_exit;
    }

    // calculate total amounts of pages
    requested_page_count = (userSpaceSize >> PAGE_SHIFT);

    printk("<0>%s(): userSpaceSize = %lld.\n", __FUNCTION__, (unsigned long long)userSpaceSize);
    printk("<0>%s(): requested_page_count = %d.\n", __FUNCTION__, requested_page_count);

    MemDscr->Pages = kzalloc(sizeof(struct page*)*requested_page_count, GFP_KERNEL);
    if(!MemDscr->Pages) {
        printk("<0>%s(): Cant allocate memory for locked pages pointers.\n", __FUNCTION__);
        errcode = -ENOMEM;
        goto err_exit;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
    down_read(&current->mm->mmap_lock);
#else
    down_read(&current->mm->mmap_sem);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,5,0)
    allocated_page_count = get_user_pages(current,
                                          current->mm,
                                          (size_t)userSpaceAddress,
                                          requested_page_count,
                                          1,
                                          0,
                                          MemDscr->Pages,
                                          0);
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0))
    allocated_page_count = get_user_pages((size_t)userSpaceAddress,
                                          requested_page_count,
                                          1,
                                          0,
                                          MemDscr->Pages,
                                          0);
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
    allocated_page_count = get_user_pages((size_t)userSpaceAddress,
                                          requested_page_count,
                                          1,
                                          MemDscr->Pages,
                                          0);
#endif

    if(allocated_page_count != requested_page_count) {
        printk("<0>%s(): Error in get_user_pages()!\n", __FUNCTION__);
        errcode = -ENOMEM;
        goto do_free_mem1;
    }

    MemDscr->PageCount = allocated_page_count;

    printk("<0>%s(): MemDscr->PageCount = %lld\n", __FUNCTION__, (unsigned long long)MemDscr->PageCount);
    printk("<0>%s(): MemDscr->Pages = %p\n", __FUNCTION__, MemDscr->Pages);

    MemDscr->PageAddresses = kzalloc(MemDscr->PageCount*sizeof(dma_addr_t), GFP_KERNEL);
    if(!MemDscr->PageAddresses) {
        printk("<0>%s(): Cant allocate memory for PageAddresses.\n", __FUNCTION__);
        errcode = -ENOMEM;
        goto do_free_mem1;
    }

    for(i=0; i<MemDscr->PageCount; i++) {

        struct page* page = MemDscr->Pages[i];

        MemDscr->PageAddresses[i] = page_to_phys(page);

        printk("<0>%s(): %d - PageAddress %p, PhysicalAddress = 0x%llX\n", __FUNCTION__, i, page, (unsigned long long)MemDscr->PageAddresses[i]);

        if(PageReserved(page)) {
            printk("<0>%s(): Pages[%d] = %p is reserverd!!!\n", __FUNCTION__, i, page);
        } else {
            //SetPageDirty(page);
            SetPageReserved(page);
            //page_cache_release(page);
            //dbg_msg(dbg_trace, "%s(): Lock page %p\n", __FUNCTION__, page);
        }
    }

    printk("<0>%s(): Lock %lld memory pages\n", __FUNCTION__, (unsigned long long)MemDscr->PageCount);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
    up_read(&current->mm->mmap_lock);
#else
    up_read(&current->mm->mmap_sem);
#endif

    return 0;

do_free_mem1:
    kfree(MemDscr->Pages);
    MemDscr->Pages = NULL;
    MemDscr->PageCount = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
    up_read(&current->mm->mmap_lock);
#else
    up_read(&current->mm->mmap_sem);
#endif

err_exit:
    return errcode;
}

//*******************************************************************

static int UserMemoryUnlock( SHARED_MEMORY_DESCRIPTION *MemDscr )
{
    int i = 0;

    if(!MemDscr) {
        printk("<0>%s(): Invalid parameter MemDscr = %p\n", __FUNCTION__, MemDscr);
        return -EINVAL;
    }

    if(!MemDscr->Pages) {
        printk("<0>%s(): MemDscr->LockedPages = %p\n", __FUNCTION__, MemDscr->Pages);
        return -EINVAL;
    }

    for(i=0; i<MemDscr->PageCount; i++) {
        struct page* page = MemDscr->Pages[i];
        ClearPageReserved(page);
        printk("<0>%s(): %d - PageAddress %p, PhysicalAddress = 0x%llX\n", __FUNCTION__, i, page, (unsigned long long)MemDscr->PageAddresses[i]);
    }

    if(MemDscr->Pages) {
        kfree(MemDscr->Pages);
        kfree(MemDscr->PageAddresses);
        MemDscr->Pages = 0;
        MemDscr->PageAddresses = 0;
        MemDscr->PageCount = 0;
    }

    return 0;
}

//*******************************************************************

int RequestUsrBuf(struct CDmaChannel *dma, void **pMemUserAddr)
{
    u32 iBlock = 0;
    u32 totalLockedPage = 0;
    int Status = STATUS_SUCCESS;

    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if(dma->m_UseCount) {
        printk("<0>%s(): Memory already allocated.\n", __FUNCTION__);
        return -EINVAL;
    }

    // проверим размер у буферов пользователя
    if(CheckUserSize( dma->m_BlockSize )) {
        return -EINVAL;
    }

    // проверим выравнивание адресов у буферов пользователя
    for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++) {
        if(CheckUserAddress(pMemUserAddr[iBlock])) {
            return -EINVAL;
        }
    }

    for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++)
    {
        int res = UserMemoryLock(&dma->m_BufDscrVA[iBlock], pMemUserAddr[iBlock], dma->m_BlockSize);
        if(res < 0) {
            printk("<0>%s(): Not enought memory for %i block location. m_BlockSize = %X\n", __FUNCTION__, (int)iBlock, (int)dma->m_BlockSize );
            break;
        }

        totalLockedPage += dma->m_BufDscrVA[iBlock].PageCount;

        dma->m_BufDscrVA[iBlock].SystemAddress = NULL;
        dma->m_BufDscrVA[iBlock].LogicalAddress = 0;

        printk("<0>%s(): Userspace buffer %p locked\n", __FUNCTION__, pMemUserAddr[iBlock]);
    }

    printk("<0>%s(): Total page number %d\n", __FUNCTION__, totalLockedPage);

    dma->m_BlockCount = iBlock;
    dma->m_ScatterGatherTableEntryCnt = totalLockedPage;

    return Status;
}

//*******************************************************************

void ReleaseUsrBuf(struct CDmaChannel *dma)
{
    u32 iBlock = 0;

    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if(!dma) {
        printk("<0>%s(): Invalid dma channel\n", __FUNCTION__);
        return;
    } else {
        printk("<0>%s(): dma->m_BufDscrVA = %p\n", __FUNCTION__, dma->m_BufDscrVA);
    }

    for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++) {
        if(dma->m_BufDscrVA) {
            UserMemoryUnlock(&dma->m_BufDscrVA[iBlock]);
        }
    }
}

//*******************************************************************

int RequestSysBuf(struct CDmaChannel *dma, void **pMemPhysAddr)
{
    u32 iBlock = 0;
    int Status = STATUS_SUCCESS;
    SHARED_MEMORY_DESCRIPTION *pBuffer = NULL;

    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if(dma->m_UseCount) {
        printk("<0> RequestSysBuf(): Memory already allocated.\n");
        return -EINVAL;
    }

    pBuffer = dma->m_BufDscrVA;

    for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++)
    {
        dma_addr_t 	 LogicalAddress;
        void 		*pSystemAddress;

        pSystemAddress = dma_alloc_coherent( dma->m_dev, dma->m_BlockSize, &LogicalAddress, GFP_KERNEL );
        if(pSystemAddress == NULL) {
            printk("<0> CDmaChannel::RequestSysBuf: Not enought memory for %i block location. m_BlockSize = %X\n", (int)iBlock, (int)dma->m_BlockSize );
            return -ENOMEM;
        }

        lock_pages( pSystemAddress, dma->m_BlockSize );

        pBuffer[iBlock].SystemAddress = pSystemAddress;
        pBuffer[iBlock].LogicalAddress = LogicalAddress;

        pMemPhysAddr[iBlock] = (void*)((size_t)pBuffer[iBlock].LogicalAddress);

        printk("<0>%s(): Address %p\n", __FUNCTION__, pMemPhysAddr[iBlock]);
    }

    dma->m_BlockCount = iBlock;
    dma->m_ScatterGatherTableEntryCnt = iBlock;

    return Status;
}

//*******************************************************************

void ReleaseSysBuf(struct CDmaChannel *dma)
{
    u32 iBlock = 0;
    SHARED_MEMORY_DESCRIPTION *pBuffer = NULL;

    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if(!dma->m_UseCount) {
        printk("<0> ReleaseSysBuf(): Memory not allocated.\n");
        return;
    }

    if(dma->m_MemType == USER_MEMORY_TYPE) {
        printk("<0>%s(): Invalid memory type\n", __FUNCTION__);
        return;
    }

    pBuffer = dma->m_BufDscrVA;

    for(iBlock = 0; iBlock < dma->m_BlockCount; iBlock++)
    {
        unlock_pages( pBuffer[iBlock].SystemAddress, dma->m_BlockSize );

        dma_free_coherent( dma->m_dev, dma->m_BlockSize, pBuffer[iBlock].SystemAddress, pBuffer[iBlock].LogicalAddress );
        pBuffer[iBlock].SystemAddress = NULL;
        pBuffer[iBlock].LogicalAddress = 0;
    }
}

// ******************************************************************
int RequestStub(struct CDmaChannel *dma, void **pStubPhysAddr)
{
    int Status = STATUS_SUCCESS;

    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if(!dma)
        return -EINVAL;

    if(!dma->m_UseCount)
    {
        dma->m_StubDscr.SystemAddress = dma_alloc_coherent( dma->m_dev, PAGE_SIZE, &dma->m_StubDscr.LogicalAddress, GFP_KERNEL );
        if(!dma->m_StubDscr.SystemAddress)
        {
            printk("%s(): Not enought memory for stub\n", __FUNCTION__);
            return -ENOMEM;
        }

        lock_pages( dma->m_StubDscr.SystemAddress, PAGE_SIZE );

        dma->m_pStub = (AMB_STUB*)dma->m_StubDscr.SystemAddress;            //может быть в этом нет необходимости,
                                                                            //но в дальнейшем в модуле используется dma->m_pStub
    }

    pStubPhysAddr[0] = (void*)((size_t)dma->m_StubDscr.LogicalAddress);

    printk("<0>%s(): Stub physical address: %p\n", __FUNCTION__, pStubPhysAddr[0]);

    return Status;
}

// ******************************************************************
void ReleaseStub(struct CDmaChannel *dma)
{
    /*
 if(!dma->m_UseCount) {
  printk("<0> ReleaseStub(): Stub memory not allocated.\n");
  return;
 }
 */
    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    unlock_pages(dma->m_StubDscr.SystemAddress, PAGE_SIZE);

    dma_free_coherent(dma->m_dev, PAGE_SIZE, dma->m_StubDscr.SystemAddress, dma->m_StubDscr.LogicalAddress);

    dma->m_pStub = NULL;
    dma->m_StubDscr.SystemAddress = NULL;
    dma->m_StubDscr.LogicalAddress = 0;
}

// ******************************************************************
int RequestSGList(struct CDmaChannel *dma)
{
    u32 SGListSize = sizeof(DMA_CHAINING_DESCRIPTOR) * dma->m_ScatterGatherTableEntryCnt;

    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if(SGListSize < PAGE_SIZE) SGListSize = PAGE_SIZE;

    //	выделим память для s/g списка
    dma->m_SGTableDscrVA = dma_alloc_coherent( dma->m_dev, SGListSize, &dma->m_SGTableDscrPA, GFP_KERNEL );
    if(!dma->m_SGTableDscrVA)
    {
        printk("<0>%s(): Not enought memory for scatter/gather list\n", __FUNCTION__);
        return -ENOMEM;
    }

    lock_pages( dma->m_SGTableDscrVA, SGListSize );

    printk("<0>%s(): m_SGTableDscrVA = %p, [%x]\n", __FUNCTION__, dma->m_SGTableDscrVA, (int)dma->m_SGTableDscrPA);

    return STATUS_SUCCESS;
}

// ******************************************************************
void ReleaseSGList(struct CDmaChannel *dma)
{
    u32 SGListSize = sizeof(DMA_CHAINING_DESCRIPTOR) * dma->m_ScatterGatherTableEntryCnt;

    printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if(SGListSize < PAGE_SIZE) SGListSize = PAGE_SIZE;

    unlock_pages(dma->m_SGTableDscrVA, SGListSize);

    dma_free_coherent(dma->m_dev,SGListSize,dma->m_SGTableDscrVA,dma->m_SGTableDscrPA);

    dma->m_SGTableDscrVA = NULL;
    dma->m_SGTableDscrPA = 0;
}

//*******************************************************************
// call from WambpIoctlDispatch (IRQL = PASSIVE_LEVEL)
//*******************************************************************
void QueuedStartMem(struct CDmaChannel *dma, u32 IsCycling)
{
    //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);
    dma->m_DmaCycling = IsCycling;
}

//*******************************************************************
// call from WambpStartReadWrite (IRQL = DISPATCH_LEVEL)
//*******************************************************************
int StartDmaTransfer(struct CDmaChannel *dma, u32 IsCycling)
{
    //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    dma->m_DmaCycling = IsCycling;

    dma->m_CycleNum	= 0;
    dma->m_BlocksRemaining	= dma->m_BlockCount;
    dma->m_CurBlockNum = 0;

    dma->m_pScatterGatherTableVA[dma->m_ScatterGatherTableEntryCnt - 1].Next.EndOfChain = !dma->m_DmaCycling;

    dma->m_pStub->lastBlock = -1;
    dma->m_pStub->totalCounter = 0;
    dma->m_pStub->state = STATE_RUN;

    return STATUS_SUCCESS;
}

//*******************************************************************
// call from WambpIsr (IRQL = DIRQL)
//*******************************************************************
void NextDmaTransfer(struct CDmaChannel *dma)
{
    //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if(dma->m_pStub->lastBlock+1 >= (u32)dma->m_BlockCount) {
        dma->m_pStub->lastBlock = 0;
    } else {
        dma->m_pStub->lastBlock++;
    }
    dma->m_CurBlockNum++;
    dma->m_pStub->totalCounter++;
    dma->m_BlocksRemaining--;

    tasklet_hi_schedule(&dma->m_Dpc);

    if((dma->m_BlocksRemaining <= 0) && dma->m_DmaCycling)
    {
        dma->m_BlocksRemaining = dma->m_BlockCount;
        dma->m_CurBlockNum = 0;
        dma->m_CycleNum++;
    }
    //printk("<0>%s(%d): dma->m_CurBlockNum = %d, dma->m_BlockCount = %d\n", __FUNCTION__, dma->m_NumberOfChannel, dma->m_CurBlockNum, dma->m_BlockCount);
}

//*******************************************************************
// call from WambpIoctlDispatch (IRQL = PASSIVE_LEVEL)
//*******************************************************************
void GetState(struct CDmaChannel *dma, u32 *BlockNum, u32 *BlockNumTotal, u32 *OffsetInBlock, u32 *DmaChanState)
{
    //printk("<0>%s(%d):  last block = %d, cycle counter = %d\n", __FUNCTION__, dma->m_NumberOfChannel, dma->m_pStub->lastBlock, dma->m_CycleNum);
    //spin_lock_bh(&dma->m_DmaLock);
    *BlockNum = dma->m_State.lastBlock;
    *BlockNumTotal = dma->m_State.totalCounter;
    *OffsetInBlock = dma->m_State.offset;
    //spin_unlock_bh(&dma->m_DmaLock);
}

//*******************************************************************
int WaitBlockEndEvent(struct CDmaChannel *dma, u32 msTimeout)
{
    int status = -ETIMEDOUT;

    //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if( msTimeout < 0 ) {
        status = GrabEvent( &dma->m_BlockEndEvent, -1 );
    } else {
        status = GrabEvent( &dma->m_BlockEndEvent, msTimeout );
    }
    return status;
}

//*******************************************************************
int WaitBufferEndEvent(struct CDmaChannel *dma, u32 msTimeout)
{
    int status = -ETIMEDOUT;

    //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);

    if( msTimeout < 0 ) {
        status = GrabEvent( &dma->m_BufferEndEvent, -1 );
    } else {
        status = GrabEvent( &dma->m_BufferEndEvent, msTimeout );
    }
    return status;
}

//*******************************************************************
int CompleteDmaTransfer(struct CDmaChannel *dma)
{
    //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);
    if(dma->m_pStub) {
        dma->m_pStub->state = STATE_STOP;
    }
    //SetEvent( &dma->m_BufferEndEvent );
    return STATUS_SUCCESS;
}

//*******************************************************************
void GetSGStartParams(struct CDmaChannel *dma, u32* SGTableAddress, u32* LocalAddress, u32* DmaDirection)
{
    //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);
    *SGTableAddress = dma->m_pScatterGatherTableVA[dma->m_ScatterGatherTableEntryCnt - 1].Next.Address;
    *LocalAddress = dma->m_pScatterGatherTableVA[0].DmaLocalAddress;
    *DmaDirection = dma->m_pScatterGatherTableVA[0].Next.DirFromDevice;
}

//*******************************************************************
void GetStartParams(struct CDmaChannel *dma, u32* PciAddress, u32* LocalAddress, u32* DmaLength)
{
    //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);
    *PciAddress = dma->m_pScatterGatherTableVA[0].DmaPciAddress;
    *LocalAddress = dma->m_pScatterGatherTableVA[0].DmaLocalAddress;
    *DmaLength = dma->m_pScatterGatherTableVA[0].DmaLength;
}

//*******************************************************************
void SetAdmTetr(struct CDmaChannel *dma, u32 AdmNum, u32 TetrNum)
{
    //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);
    dma->m_AdmNum = AdmNum;
    dma->m_TetrNum = TetrNum;
}

//*******************************************************************
void FreezeState(struct CDmaChannel *dma)
{
    //printk("<0>%s(%d)\n", __FUNCTION__, dma->m_NumberOfChannel);
}

// ******************************************************************
