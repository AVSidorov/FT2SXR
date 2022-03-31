
////////////////////////////////////////////////////////////////
// File - P9X5X.CPP
//
// Library for 'PLX 9X5X' API.
//
////////////////////////////////////////////////////////////////

#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/pci.h>

#include "p9x5x.h"

/*
  DMA Read to memory from local bus
*/
u32   P9X5X_dmaReadRequest( size_t MEM_BASE,
                            u32 chan,
                            u32 isIrq,
                            u32 pcAddr,
                            u32 dspAddr,
                            u32 pciCount,
                            u32 mode)
{
	u32	x;

        //printk("<0> %s(): MEM_BASE = 0x%X\n", __FUNCTION__, MEM_BASE);
        //printk("<0> %s(): pcAddr = 0x%X\n", __FUNCTION__, pcAddr);
        //printk("<0> %s(): dspAddr = 0x%X\n", __FUNCTION__, dspAddr);
        //printk("<0> %s(): pciCount = 0x%X\n", __FUNCTION__, pciCount);

	asm("nop");

	// set mode
        if( isIrq==0)
	{
		writel(mode, (u32*)(MEM_BASE+P9X5X_DMAMODE+chan*P9X5X_DMA_CHANNEL_SHIFT));
	}
        else
	{
		writel(mode|0x20400, (u32*)(MEM_BASE+P9X5X_DMAMODE+chan*P9X5X_DMA_CHANNEL_SHIFT));

                //asm("cli");
		x = readl( (u32*)(MEM_BASE+P9X5X_INTCSR) );
		if( chan==0)
			 x |= 0x40100;
		else x |= 0x80100;
		writel(x, (u32*)(MEM_BASE+P9X5X_INTCSR));
                //asm("sti");
	}

        // set PC address
	writel(pcAddr, (u32*)(MEM_BASE+P9X5X_DMAPADR+chan*P9X5X_DMA_CHANNEL_SHIFT));
        // set local address
        writel(dspAddr<<2, (u32*)(MEM_BASE+P9X5X_DMALADR+chan*P9X5X_DMA_CHANNEL_SHIFT));
	// set size of bytes
	writel(pciCount, (u32*)(MEM_BASE+P9X5X_DMASIZ+chan*P9X5X_DMA_CHANNEL_SHIFT));

	// set descriptor
	writel(0x9, (u32*)(MEM_BASE+P9X5X_DMADPR+chan*P9X5X_DMA_CHANNEL_SHIFT));

	// start
	if( chan==0 )
		writeb(0x3, (u8*)(MEM_BASE+P9X5X_DMACSR));
	else
		writeb(0x3, (u8*)(MEM_BASE+P9X5X_DMACSR+1));
        return 0;
}

/*
  DMA Write from memory to local bus
*/
u32   P9X5X_dmaWriteRequest( size_t MEM_BASE,
                             u32 chan,
                             u32 isIrq,
                             u32 pcAddr,
                             u32 dspAddr,
                             u32 pciCount,
                             u32 mode)
{
	u32 x;

        //printk("<0> %s(): MEM_BASE = 0x%X\n", __FUNCTION__, MEM_BASE);
        //printk("<0> %s(): pcAddr = 0x%X\n", __FUNCTION__, pcAddr);
        //printk("<0> %s(): dspAddr = 0x%X\n", __FUNCTION__, dspAddr);
        //printk("<0> %s(): pciCount = 0x%X\n", __FUNCTION__, pciCount);

        // set mode
        if( isIrq==0)
	{
		writel(mode, (u32*)(MEM_BASE+P9X5X_DMAMODE+chan*P9X5X_DMA_CHANNEL_SHIFT));
	}
	else
	{
		writel(mode|0x20400, (u32*)(MEM_BASE+P9X5X_DMAMODE+chan*P9X5X_DMA_CHANNEL_SHIFT));
                //asm("cli");
		x = readl( (u32*)(MEM_BASE+P9X5X_INTCSR) );
		if( chan==0)
			 x |= 0x40100;
		else x |= 0x80100;
		writel(x, (u32*)(MEM_BASE+P9X5X_INTCSR));
                //asm("sti");
	}
        // set PC address
	writel(pcAddr, (u32*)(MEM_BASE+P9X5X_DMAPADR+chan*P9X5X_DMA_CHANNEL_SHIFT));
        // set local address
	writel(dspAddr<<2, (u32*)(MEM_BASE+P9X5X_DMALADR+chan*P9X5X_DMA_CHANNEL_SHIFT));
        // set size of bytes
	writel(pciCount, (u32*)(MEM_BASE+P9X5X_DMASIZ+chan*P9X5X_DMA_CHANNEL_SHIFT));

        // set descriptor
	writel(1, (u32*)(MEM_BASE+P9X5X_DMADPR+chan*P9X5X_DMA_CHANNEL_SHIFT));

       // start
	if( chan==0)
		writeb(0x3, (u8*)(MEM_BASE+P9X5X_DMACSR));
	else
		writeb(0x3, (u8*)(MEM_BASE+P9X5X_DMACSR+1));

        return 0;
}

int P9X5X_checkDMARequest(size_t MEM_BASE, u32 chan)
{
    u16   x = 0;

    //printk("<0>%s()\n", __FUNCTION__);

    x = readw((u16*)(MEM_BASE + P9X5X_DMACSR));

    if( chan==0 ) {
        x &= 0x10;
    } else {
    	x &= 0x1000;
    }

    //printk("<0>%s(): MASK = %X\n", __FUNCTION__, x);

    if( x != 0 ) {
		//okay DMA done
		return 1;
    }

	//DMA not done
    return 0;
}

void	P9X5X_DMAReset(size_t MEM_BASE, u32 dmaChan)
{
	int i = 0;
	int	cnt = 0;

        //printk("<0>%s()\n", __FUNCTION__);

	writeb(0x5, (u8*)(MEM_BASE+P9X5X_DMACSR+dmaChan));

	for(i=0;i<1000;i++)
  		asm("nop");

	// wait for end DMA
	while( (readb((u8*)(MEM_BASE+P9X5X_DMACSR+dmaChan))&0x10) == 0)
	{
		writeb(0x5, (u8*)(MEM_BASE+P9X5X_DMACSR+dmaChan));	// ABORT DMA
		asm("nop");
		if(cnt++ == 100000) return;
        }
	writeb(0x0, (u8*)(MEM_BASE+P9X5X_DMACSR+dmaChan));
}
