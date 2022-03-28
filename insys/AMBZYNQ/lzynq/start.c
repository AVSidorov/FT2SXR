#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>

#include "ddzynq.h"
#include "zynqdev.h"
#include "wambpmc.h"
#include "devproc.h"

MODULE_AUTHOR("Karakozov Vladimir E-mail: karakozov@gmail.com");
MODULE_LICENSE("GPL");

struct list_head device_list;
static int boards_count = 0;
static dev_t devno = MKDEV(0, 0);
static struct class *zynq_class = 0;
extern struct file_operations 	ambpex_fops;

int dbg_trace = 0;
int err_trace = 1;
int max_device = 1;

module_param( dbg_trace, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
module_param( err_trace, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
module_param( max_device, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );

//-----------------------------------------------------------------------------

static int zynqdev_probe(struct platform_device *pdev)
{
    int error = 0;
    int i = 0;
    struct resource *r_irq[ZYNQ_FPGA_IRQS];
    struct resource *r_mem[ZYNQ_FPGA_BARS];
    const char *irq_names[ZYNQ_FPGA_IRQS] = {"DMA0", "DMA1", "TRD", "ERROR"};
    struct device *dev = &pdev->dev;
    struct CWambpex *entry = 0;

    entry = (struct CWambpex*)kzalloc( sizeof(struct CWambpex), GFP_KERNEL );
    if(!entry) {
        err_msg(err_trace, "%s(): Error allocate memory for device\n", __FUNCTION__);
        error = -ENOMEM;
        goto err_exit;
    }

    dev_info(dev, "Device Tree Probing\n");
    for(i=0; i<ZYNQ_FPGA_BARS; i++) {

        r_mem[i] = platform_get_resource(pdev, IORESOURCE_MEM, i);
        if (!r_mem[i]) {
            dev_err(dev, "invalid address for BAR%d\n", i);
            error = -ENODEV;
            goto err_free_resources;
        }

        if(i==0) {
            entry->m_OperationRegisters.PhysicalAddress = r_mem[i]->start;
            entry->m_OperationRegisters.Length = resource_size(r_mem[i]);
        }
        if(i==1) {
            entry->m_AmbMainRegisters.PhysicalAddress = r_mem[i]->start;
            entry->m_AmbMainRegisters.Length = resource_size(r_mem[i]);
        }
    }

    // map BAR0 resources
    entry->m_OperationRegisters.VirtualAddress = ioremap(entry->m_OperationRegisters.PhysicalAddress, entry->m_OperationRegisters.Length);
    if(!entry->m_OperationRegisters.VirtualAddress) {
        err_msg(err_trace, "%s(): pci_iomap() for BAR0 was failed.\n", __FUNCTION__);
        error = -EBUSY;
        goto err_free_resources;
    }

    // map BAR1 resources
    entry->m_AmbMainRegisters.VirtualAddress = ioremap(entry->m_AmbMainRegisters.PhysicalAddress, entry->m_AmbMainRegisters.Length);
    if(!entry->m_AmbMainRegisters.VirtualAddress) {
        err_msg(err_trace, "%s(): pci_iomap() for BAR1 was failed.\n", __FUNCTION__);
        goto err_free_resources;
    }

    entry->bar[0] = entry->m_OperationRegisters;
    entry->bar[1] = entry->m_AmbMainRegisters;

    INIT_LIST_HEAD(&entry->m_list);

    entry->m_dev = dev;
    entry->m_DmaIrqEnbl = 0;
    entry->m_FlgIrqEnbl = 0;
    entry->m_primChan = 0;
    entry->m_BusNumber  = 0;
    entry->m_DevFNumber = 0;
    entry->m_SlotNumber = 0;
    for(i=0; i<ZYNQ_FPGA_IRQS; i++) {
        entry->m_Interrupt[i] = -1;
        spin_lock_init(&entry->m_IsrLock[i]);
    }
    entry->m_BoardIndex = boards_count;
    sema_init(&entry->m_BoardSem, 1);
    sema_init(&entry->m_BoardRegSem, 1);
    init_waitqueue_head(&entry->m_WaitQueue);
    atomic_set(&entry->m_TotalIRQ, 0);
    atomic_set(&entry->m_SpuriousIRQ, 0);
    entry->m_isr[0] = dma0_isr;
    entry->m_isr[1] = dma1_isr;
    entry->m_isr[2] = WambpexIsr;
    entry->m_isr[3] = WambpexIsr;

    dbg_msg(dbg_trace, "%s(): board %d. busn = 0x%X, slotn = 0x%X\n", __FUNCTION__, entry->m_BoardIndex, entry->m_BusNumber, entry->m_SlotNumber );

    error = InitializeBoard(entry);
    if(error < 0) {
        err_msg(err_trace, "%s(): InitializeBoard() was failed.\n", __FUNCTION__);
        goto err_free_resources;
    }

    for(i=0; i<ZYNQ_FPGA_IRQS; i++) {

        r_irq[i] = platform_get_resource(pdev, IORESOURCE_IRQ, i);
        if (!r_irq[i]) {
            err_msg(err_trace, "%s(): No IRQ resource defined.\n", __FUNCTION__);
            goto err_free_resources;
        }

        snprintf(entry->m_irq_name[i], 128, "%s-%s", entry->m_name, irq_names[i]);

        dbg_msg(err_trace, "%s(%d): IRQ %lld [from DTB] - %s\n", __FUNCTION__, i, (long long)r_irq[i]->start, entry->m_irq_name[i]);

        error = request_irq(r_irq[i]->start, entry->m_isr[i], IRQF_SHARED, entry->m_irq_name[i], (void*)entry);
        if(error < 0) {
            err_msg(err_trace, "%s(): request_irq() was failed.\n", __FUNCTION__);
            error = -EBUSY;
            goto err_free_resources;
        }

        dbg_msg(dbg_trace, "%s(): %s - request irq %lld for device %p\n", __FUNCTION__, entry->m_irq_name[i], (long long)r_irq[i]->start, entry);

        entry->m_Interrupt[i] = r_irq[i]->start;
    }

    cdev_init(&entry->m_cdev, &ambpex_fops);
    entry->m_cdev.owner = THIS_MODULE;
    entry->m_cdev.ops = &ambpex_fops;
    entry->m_devno = MKDEV(MAJOR(devno), boards_count);

    error = cdev_add(&entry->m_cdev, entry->m_devno, 1);
    if(error) {
        err_msg(err_trace, "%s(): Error add char device %d\n", __FUNCTION__, boards_count);
        goto err_free_irq;
    }

    dbg_msg(dbg_trace, "%s(): Add cdev %d\n", __FUNCTION__, boards_count);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
    entry->m_devobj = device_create(zynq_class, 0, entry->m_devno, "%s%d", DRV_NAME, boards_count);
#else
    entry->m_devobj = device_create(zynq_class, 0, entry->m_devno, NULL, "%s%d", DRV_NAME, boards_count);
#endif
    if(!entry->m_devobj) {
        err_msg(err_trace, "%s(): Error create device for board: %s\n", __FUNCTION__, entry->m_name);
        error = -EINVAL;
        goto err_free_cdev;
    }

    dbg_msg(dbg_trace, "%s(): Create device file for board: %s\n", __FUNCTION__, entry->m_name);

    list_add_tail(&entry->m_list, &device_list);

    board_register_proc(entry->m_name, entry);

    dbg_msg(dbg_trace, "%s(): Create board entry: /proc/%s\n", __FUNCTION__, entry->m_name);

    boards_count++;

    return error;

err_free_cdev:
    cdev_del(&entry->m_cdev);
err_free_irq:
    for(i=0; i<ZYNQ_FPGA_IRQS; i++) {
        free_irq(entry->m_Interrupt[i], (void*)entry);
    }
    UninitializeBoard(entry);
err_free_resources:
    if(entry->m_AmbMainRegisters.VirtualAddress)
        iounmap(entry->m_AmbMainRegisters.VirtualAddress);
    if(entry->m_OperationRegisters.VirtualAddress)
        iounmap(entry->m_OperationRegisters.VirtualAddress);
    release_mem_region(entry->m_AmbMainRegisters.PhysicalAddress, entry->m_AmbMainRegisters.Length);
    release_mem_region(entry->m_OperationRegisters.PhysicalAddress, entry->m_OperationRegisters.Length);
    kfree(entry);
err_exit:
    return error;

}

//-----------------------------------------------------------------------------

static int zynqdev_remove(struct platform_device *pdev)
{
    struct list_head *p, *n;
    struct device *dev = &pdev->dev;
    struct CWambpex *entry = dev_get_drvdata(dev);

    //dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    list_for_each_safe(p, n, &device_list) {

        entry = list_entry(p, struct CWambpex, m_list);

        if(entry->m_dev == dev) {

            int i = 0;

            dbg_msg(dbg_trace, "%s(): Remove device class.\n", __FUNCTION__);
            device_destroy(zynq_class, entry->m_devno);

            dbg_msg(dbg_trace, "%s(): Remove /dev entry.\n", __FUNCTION__);
            cdev_del(&entry->m_cdev);

            dbg_msg(dbg_trace, "%s(): Remove /proc entry.\n", __FUNCTION__);
            board_remove_proc( entry->m_name );

            dbg_msg(dbg_trace, "%s(): Delete IRQ.\n", __FUNCTION__);
            for(i=0; i<ZYNQ_FPGA_IRQS; i++) {
                free_irq(entry->m_Interrupt[i], (void*)entry);
            }

            dbg_msg(dbg_trace, "%s(): Delete DMA.\n", __FUNCTION__);
            UninitializeBoard(entry);

            dbg_msg(dbg_trace, "%s(): Unamp BAR0\n", __FUNCTION__);
            if(entry->m_OperationRegisters.VirtualAddress)
                iounmap(entry->m_OperationRegisters.VirtualAddress);

            dbg_msg(dbg_trace, "%s(): Unamp BAR1\n", __FUNCTION__);
            if(entry->m_AmbMainRegisters.VirtualAddress)
                iounmap(entry->m_AmbMainRegisters.VirtualAddress);

            release_mem_region(entry->m_AmbMainRegisters.PhysicalAddress, entry->m_AmbMainRegisters.Length);
            release_mem_region(entry->m_OperationRegisters.PhysicalAddress, entry->m_OperationRegisters.Length);

            dbg_msg(dbg_trace, "%s(): Remove device from list of devices\n", __FUNCTION__);
            list_del(p);

            kfree(entry);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------

#ifdef CONFIG_OF
static struct of_device_id zynqdev_of_match[] = {
    { .compatible = "insys,lzynq", },
    { /* end of list */ },
};
MODULE_DEVICE_TABLE(of, zynqdev_of_match);
#else
# define zynqdev_of_match 0
#endif

//-----------------------------------------------------------------------------

static struct platform_driver zynqdev_driver = {
    .driver = {
        .name = DRV_NAME,
        .owner = THIS_MODULE,
        .of_match_table	= zynqdev_of_match,
    },
    .probe		= zynqdev_probe,
    .remove		= zynqdev_remove,
};

//-----------------------------------------------------------------------------

static int zynq_init( void )
{
    int error = 0;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    INIT_LIST_HEAD(&device_list);

    error = alloc_chrdev_region(&devno, 0, max_device, DRV_NAME);
    if(error < 0) {
        err_msg(err_trace, "%s(): Erorr allocate char device regions\n", __FUNCTION__);
        goto do_out;
    }

    zynq_class = class_create(THIS_MODULE, DRV_NAME);
    if(!zynq_class) {
           err_msg(err_trace, "%s(): Erorr create device class.\n", __FUNCTION__);
           error = -EINVAL;
           goto do_free_chrdev;
    }

    error = platform_driver_register(&zynqdev_driver);
    if(error) {
        printk(KERN_DEBUG"%s(): Erorr register platform device driver.\n", __FUNCTION__);
        goto do_delete_class;
    }

    dbg_msg(dbg_trace, "%s(): Find %d board.\n", __FUNCTION__, boards_count);

    if(boards_count == 0) {
        error = -ENODEV;
        goto do_unregister_pcidrv;
    }

    return 0;

do_unregister_pcidrv:
    platform_driver_unregister(&zynqdev_driver);
do_delete_class:
    class_destroy(zynq_class);
do_free_chrdev:
    unregister_chrdev_region(devno, max_device);
do_out:
    return error;
}

//-----------------------------------------------------------------------------

static void zynq_cleanup(void)
{
    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);
    platform_driver_unregister(&zynqdev_driver);
    class_destroy(zynq_class);
    unregister_chrdev_region(devno, max_device);
}

//-----------------------------------------------------------------------------

module_init(zynq_init);
module_exit(zynq_cleanup);

//-----------------------------------------------------------------------------
