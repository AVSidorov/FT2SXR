
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

//--------------------------------------------------------------------

int lock_pages( void *va, u32 size )
{
    struct page *start_page_addr = virt_to_page(va);
    int i = 0;

    for (i=0; i < (size >> PAGE_SHIFT); i++) {
        SetPageReserved(start_page_addr+i);
    }

    return i;
}

//--------------------------------------------------------------------

int unlock_pages( void *va, u32 size )
{
    struct page *start_page_addr = virt_to_page(va);
    int i = 0;

    for (i=0; i < (size >> PAGE_SHIFT); i++) {
        ClearPageReserved(start_page_addr+i);
    }

    return i;
}

//--------------------------------------------------------------------

int check_address( void __user *pMemUserAddr )
{
    size_t addr = (size_t)pMemUserAddr;
    size_t mask = (size_t)~PAGE_MASK;

    // адрес пользовательского буфера должен быть выровнен на страницу
    if(addr & mask) {
        return 1;
    }

    return 0;
}

//--------------------------------------------------------------------

int check_size( size_t userSize )
{
    // размер пользовательского буфера должен быть кратен размеру страницы
    if((userSize % PAGE_SIZE) != 0) {
        return 1;
    }

    return 0;
}

//--------------------------------------------------------------------
