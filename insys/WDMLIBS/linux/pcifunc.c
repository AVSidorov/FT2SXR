#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/config.h>
#include <linux/proc_fs.h>
#include <asm/system.h>
#include <asm/io.h>

#include "PciFunc.h"

#ifdef MAP_NR
#define virt_to_page(page) (mam_map + MAP_NR(page))
#endif

//
// Used in 2.4 kernel
//
inline int get_order(unsigned long size)
{
	int order;

#ifndef XCHAL_HAVE_NSU
	unsigned long x1, x2, x4, x8, x16;

	size = (size + PAGE_SIZE - 1) >> PAGE_SHIFT;
	x1  = size & 0xAAAAAAAA;
	x2  = size & 0xCCCCCCCC;
	x4  = size & 0xF0F0F0F0;
	x8  = size & 0xFF00FF00;
	x16 = size & 0xFFFF0000;
	order = x2 ? 2 : 0;
	order += (x16 != 0) * 16;
	order += (x8 != 0) * 8;
	order += (x4 != 0) * 4;
	order += (x1 != 0);

	return order;
#else
	size = (size - 1) >> PAGE_SHIFT;
	asm ("nsau %0, %1" : "=r" (order) : "r" (size));
	return 32 - order;
#endif
}

//
// Used in 2.4 kernel
//
void *pci_alloc_consistent( struct pci_dev *dev,
			    size_t size,
			    dma_addr_t *bus_addr )
{
    u32 mem = 0;
    int order = get_order(size);

    if(!dev)
	return NULL;

    mem = __get_free_pages(GFP_KERNEL, order);
    if(!mem) {
	printk("%s - Error in __get_free_pages().\n", __FUNCTION__);
	return NULL;
    }

    *bus_addr = virt_to_bus( (void*)mem );

    return (void*)mem;
}

//
// Used in 2.4 kernel
//
void pci_free_consistent( struct pci_dev *dev,
			    size_t size,
			    void *cpuaddr,
			    dma_handle_t bus_addr )
{
    int 	 order = 0;
    dma_handle_t phys_addr = 0;

    if(!dev)
	return;

    phys_addr = (dma_handle_t)virt_to_bus(cpuaddr);

    if( phys_addr != bus_addr ) {
	printk("<0> %s: Logical address 0x%x not\
	 correspondent physical address 0x%x\n",
	 __FUNCTION__, (int)cpuaddr, (int)bus_addr );
	 return;
    }

    order = get_order(size);

    free_pages(cpuaddr, order);

    return;
}

u32 pci_resource_start( struct pci_dev *dev, int bar )
{
    u32 curr = 0;
    u32 addr = PCI_BASE_ADDRESS_0 + 4*bar;

    pci_read_config_dword(dev, addr, &curr);

    //printk("<0> %s: resourse start %x\n", __FUNCTION__, curr);

    return (curr & (~0xf));
}

u32 pci_resource_end( struct pci_dev *dev, int bar )
{
    u32 curr = 0, mask = 0, end = 0;
    u32 addr = PCI_BASE_ADDRESS_0 + 4*bar;

    pci_read_config_dword(dev, addr, &curr);
    cli();
    pci_write_config_dword(dev, addr, ~0);
    pci_read_config_dword(dev, addr, &mask);
    pci_write_config_dword(dev, addr, curr);
    sti();

    if(!mask)
	return 0;

    end = ~mask + curr;

    //printk("<0> %s: resourse end %x, mask = %x\n", __FUNCTION__, end, mask);

    return end;
}

u32 pci_resource_flags( struct pci_dev *dev, int bar )
{
    u32 curr = 0;
    u32 addr = PCI_BASE_ADDRESS_0 + 4*bar;

    pci_read_config_dword(dev, addr, &curr);

    return (curr & 0xf);
}

int pci_request_regions( struct pci_dev *dev, char *devname )
{
    int i=0, err=0, count=0;
    u32 io_start, io_end, size, flags;

    count = 0;

    for(i=0; i<6; i++) {

        io_start = pci_resource_start ( dev, i );
        io_end = pci_resource_end ( dev, i );
        flags = pci_resource_flags ( dev, i );

	size = io_end-io_start+1;

        if ( io_start && (flags & PCI_BASE_ADDRESS_SPACE_IO) ) {
	//if ( io_start ) {
	    err = check_region( io_start, size );
	    if(err < 0) {
		printk("<0> %s: Error in check_region().\n", __FUNCTION__);
		continue;
	    }

	    request_region( io_start, size, devname );
	    count++;
        }
    }

    if(count)
	return 0;

    return -1;
}

int pci_release_regions( struct pci_dev *dev )
{
    int i=0, count=0;
    u32 io_start, io_end, size, flags;

    count = 0;

    for(i=0; i<6; i++) {

        io_start = pci_resource_start ( dev, i );
        io_end = pci_resource_end ( dev, i );
        flags = pci_resource_flags ( dev, i );

	size = io_end-io_start+1;

        if ( io_start && (flags & PCI_BASE_ADDRESS_SPACE_IO) ) {

	    release_region( io_start, size );
	    count++;
        }
    }

    if(count)
	return 0;

    return -1;
}
