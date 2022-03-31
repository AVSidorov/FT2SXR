
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/pagemap.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

#include <asm/io.h>

#include "memhelp.h"
#include "pagehelp.h"

//--------------------------------------------------------------------

int lock_user_memory( SHARED_MEMORY_DESCRIPTION *MemDscr, void __user *userSpaceAddress, size_t userSpaceSize )
{
    int i = 0;
    int requested_page_count = 0;
    int allocated_page_count = 0;
    int errcode = 0;

    dbg_msg(0, "%s()\n", __FUNCTION__);

    if(!MemDscr) {
        dbg_msg(err_trace, "%s(): Invalid memory descriptor pointer.\n", __FUNCTION__);
        errcode = -EINVAL;
        goto err_exit;
    }

    // calculate total amounts of pages
    requested_page_count = (userSpaceSize >> PAGE_SHIFT);

    dbg_msg(0, "%s(): userSpaceSize = %lld.\n", __FUNCTION__, (unsigned long long)userSpaceSize);
    dbg_msg(0, "%s(): requested_page_count = %d.\n", __FUNCTION__, requested_page_count);

    MemDscr->Pages = kzalloc(sizeof(struct page*)*requested_page_count, GFP_KERNEL);
    if(!MemDscr->Pages) {
        dbg_msg(err_trace, "%s(): Cant allocate memory for locked pages pointers.\n", __FUNCTION__);
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
        dbg_msg(err_trace, "%s(): Error in get_user_pages()!\n", __FUNCTION__);
        errcode = -ENOMEM;
        goto do_free_mem1;
    }

    MemDscr->PageCount = allocated_page_count;

    dbg_msg(0, "%s(): MemDscr->PageCount = %lld\n", __FUNCTION__, (unsigned long long)MemDscr->PageCount);
    dbg_msg(0, "%s(): MemDscr->Pages = %p\n", __FUNCTION__, MemDscr->Pages);

    MemDscr->PageAddresses = kzalloc(MemDscr->PageCount*sizeof(dma_addr_t), GFP_KERNEL);
    if(!MemDscr->PageAddresses) {
        dbg_msg(err_trace, "%s(): Cant allocate memory for PageAddresses.\n", __FUNCTION__);
        errcode = -ENOMEM;
        goto do_free_mem1;
    }

    for(i=0; i<MemDscr->PageCount; i++) {

        struct page* page = MemDscr->Pages[i];

        MemDscr->PageAddresses[i] = page_to_phys(page);

        dbg_msg(0, "%s(): %d - PageAddress %p, PhysicalAddress = 0x%llX\n", __FUNCTION__, i, page, (unsigned long long)MemDscr->PageAddresses[i]);

        if(PageReserved(page)) {
            dbg_msg(err_trace, "%s(): Pages[%d] = %p is reserverd!!!\n", __FUNCTION__, i, page);
        } else {
            //SetPageDirty(page);
            SetPageReserved(page);
            //page_cache_release(page);
            //dbg_msg(dbg_trace, "%s(): Lock page %p\n", __FUNCTION__, page);
        }
    }

    dbg_msg(0, "%s(): Lock %lld memory pages\n", __FUNCTION__, (unsigned long long)MemDscr->PageCount);

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

//--------------------------------------------------------------------

int unlock_user_memory( SHARED_MEMORY_DESCRIPTION *MemDscr )
{
    int i = 0;

    if(!MemDscr) {
        dbg_msg(err_trace, "%s(): Invalid parameter MemDscr = %p\n", __FUNCTION__, MemDscr);
        return -EINVAL;
    }

    //dbg_msg(dbg_trace, "%s(): MemDscr = %p\n", __FUNCTION__, MemDscr);

    if(!MemDscr->Pages) {
        dbg_msg(err_trace, "%s(): MemDscr->LockedPages = %p\n", __FUNCTION__, MemDscr->Pages);
        return -EINVAL;
    }

    for(i=0; i<MemDscr->PageCount; i++) {
        struct page* page = MemDscr->Pages[i];
        ClearPageReserved(page);
        //page_cache_release(page);
        //SetPageDirty(page);
        //put_page(page);
        dbg_msg(0, "%s(): %d - PageAddress %p, PhysicalAddress = 0x%llX\n", __FUNCTION__, i, page, (unsigned long long)MemDscr->PageAddresses[i]);
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

//--------------------------------------------------------------------
