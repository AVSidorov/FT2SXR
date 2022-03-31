#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "ddzynq.h"
#include "zynqdev.h"
#include "wambpmc.h"
#include "system.h"
#include "ioctlrw.h"

struct file_operations ambpex_fops = {

    owner: THIS_MODULE,         //  *owner
    read:  NULL,            //  *read
    write: NULL,           //  *write

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
    unlocked_ioctl:         adp_ioctl,           //  *ioctl
    compat_ioctl:           adp_ioctl,           //  *ioctl
#else
    ioctl:                  adp_ioctl,           //  *ioctl
#endif

    mmap:  adp_mmap,            //  *mmap
    open:  adp_open,            //  *open
    release: adp_close,         //  *release
    fasync: adp_fasync,         //  *fasync
    poll: adp_poll,
};

//--------------------------------------------------------------------

struct CWambpex *retrive_device( struct list_head *head, struct inode *inode )
{
    struct list_head *p;
    struct CWambpex *entry;

    unsigned int minor = MINOR(inode->i_rdev);

    list_for_each(p, &device_list) {
    	entry = list_entry(p, struct CWambpex, m_list);
    	if(entry->m_BoardIndex == minor)
            return entry;
    }

    return NULL;
}

//--------------------------------------------------------------------

struct CWambpex *retrive_device_file( struct file *file )
{
    return (struct CWambpex*)file->private_data;
}

//--------------------------------------------------------------------

int adp_fasync(int fd, struct file *file, int mode)
{
    int retval = -1;

    struct CWambpex *pDevice = file->private_data;

    if(!pDevice)
        return -ENODEV;

    retval = fasync_helper(fd, file, mode, &pDevice->m_Event.m_async);

    if (retval < 0)
        return retval;

    return 0;
}

//--------------------------------------------------------------------

unsigned int adp_poll(struct file *filp, poll_table *wait)
{
    unsigned int mask = 0;

    struct CWambpex *dev = filp->private_data;
    struct CDmaChannel *dma0 = dev->m_DmaChannel[0];
    struct CDmaChannel *dma1 = dev->m_DmaChannel[1];

    if( dma0->m_pStub->state == STATE_RUN ) {

        poll_wait(filp, &dma0->m_DmaWq, wait);

        if( CheckEventFlag(&dma0->m_BlockEndEvent) )
            mask |= POLLIN | POLLRDNORM;    /* readable */
    }

    if( dma1->m_pStub->state == STATE_RUN ) {

        poll_wait(filp, &dma1->m_DmaWq, wait);

        if( CheckEventFlag(&dma1->m_BlockEndEvent) )
            mask |= POLLOUT | POLLWRNORM;   /* writable */
    }

    poll_wait(filp, &dev->m_Event.m_wq, wait);

    if( CheckEventFlag(&dev->m_Event) )
        mask |= POLLOUT | POLLWRNORM | POLLIN | POLLRDNORM;   /* writable and readable */

    return mask;
}

//------------------------------------------------------------------------------

int adp_open( struct inode *inode, struct file *file )
{
    struct CWambpex *ambp = NULL;

    if(!(ambp = retrive_device( &device_list, inode )))
        return -ENODEV;

    file->private_data = (void*)ambp;

    return 0;
}


//------------------------------------------------------------------------------

int adp_close( struct inode *inode, struct file *file )
{
    struct CWambpex *ambp = NULL;

    if(!(ambp = retrive_device( &device_list, inode )))
        return -ENODEV;

    if( ambp->m_Event.m_async )
        adp_fasync(-1, file, 0);

    file->private_data = NULL;

    return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
long adp_ioctl( struct file *file, unsigned int cmd, unsigned long arg )
#else
int adp_ioctl( struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg )
#endif
{
    int status = -ENOSYS;
    struct CWambpex *pDevice = NULL;

    if(!(pDevice = retrive_device_file( file )))
        return -ENODEV;

    switch (cmd)
    {
    case IOCTL_AMB_GET_VERSION:
        status = IoctlGetVersion(pDevice, arg);
        break;
    case IOCTL_AMB_WRITE_REG_DATA:
        status = IoctlWriteRegData(pDevice, arg);
        break;
    case IOCTL_AMB_WRITE_REG_DATA_DIR:
        status = IoctlWriteRegDataDir(pDevice, arg);
        break;
    case IOCTL_AMB_READ_REG_DATA:
        status = IoctlReadRegData(pDevice, arg);
        break;
    case IOCTL_AMB_READ_REG_DATA_DIR:
        status = IoctlReadRegDataDir(pDevice, arg);
        break;
    case IOCTL_AMB_WRITE_REG_BUF:
        status = IoctlWriteRegBuf(pDevice, arg);
        break;
    case IOCTL_AMB_WRITE_REG_BUF_DIR:
        status = IoctlWriteRegBufDir(pDevice, arg);
        break;
    case IOCTL_AMB_READ_REG_BUF:
        status = IoctlReadRegBuf(pDevice, arg);
        break;
    case IOCTL_AMB_READ_REG_BUF_DIR:
        status = IoctlReadRegBufDir(pDevice, arg);
        break;
    case IOCTL_AMB_SET_BUS_CONFIG:
        status = IoctlSetBusConfig(pDevice, arg);
        break;
    case IOCTL_AMB_GET_BUS_CONFIG:
        status = IoctlGetBusConfig(pDevice, arg);
        break;
    case IOCTL_AMB_GET_LOCATION:
        status = IoctlGetLocation(pDevice, arg);
        break;
    case IOCTL_AMB_GET_CONFIGURATION:
        status = IoctlGetConfiguration(pDevice, arg);
        break;
    case IOCTL_AMB_WRITE_NVRAM:
        status = IoctlWriteNvRAM(pDevice, arg);
        break;
    case IOCTL_AMB_READ_NVRAM:
        status = IoctlReadNvRAM(pDevice, arg);
        break;
    case IOCTL_AMB_WRITE_ADMIDROM:
        status = IoctlWriteAdmIdROM(pDevice, arg);
        break;
    case IOCTL_AMB_READ_ADMIDROM:
        status = IoctlReadAdmIdROM(pDevice, arg);
        break;
    case IOCTL_AMB_GET_PLD_STATUS:
        status = IoctlGetPldStatus(pDevice, arg);
        break;
    case IOCTL_AMB_READ_LOCALBUS:
        status = IoctlReadLocalBus(pDevice, arg);
        break;
    case IOCTL_AMB_WRITE_LOCALBUS:
        status = IoctlWriteLocalBus(pDevice, arg);
        break;
    case IOCTL_AMB_SET_MEMIO:
        status = IoctlSetMem(pDevice, arg);
        break;
    case IOCTL_AMB_FREE_MEMIO:
        status = IoctlFreeMem(pDevice, arg);
        break;
    case IOCTL_AMB_START_MEMIO:
        status = IoctlStartMem(pDevice, arg);
        break;
    case IOCTL_AMB_STOP_MEMIO:
        status = IoctlStopMem(pDevice, arg);
        break;
    case IOCTL_AMB_STATE_MEMIO:
        status = IoctlStateMem(pDevice, arg);
        break;
    case IOCTL_AMB_SET_DIR_MEMIO:
        status = IoctlSetDirMem(pDevice, arg);
        break;
    case IOCTL_AMB_SET_SRC_MEMIO:
        status = IoctlSetSrcMem(pDevice, arg);
        break;
    case IOCTL_AMB_SET_DRQ_MEMIO:
        status = IoctlSetDrqMem(pDevice, arg);
        break;
    case IOCTL_AMB_RESET_FIFO:
        status = IoctlResetFifo(pDevice, arg);
        break;
    case IOCTL_AMB_SET_IRQ_TABLE:
        status = IoctlSetIrqTable(pDevice, arg);
        break;
    case IOCTL_AMB_GET_INFOIO:
        status = IoctlGetDmaChannelInfo(pDevice, arg);
        break;
    case IOCTL_AMB_ADJUST:
        status = IoctlAdjust(pDevice, arg);
        break;
    case IOCTL_AMB_DONE:
        status = IoctlDone(pDevice, arg);
        break;
    case IOCTL_AMB_WAIT_DMA_BUFFER:
        status = IoctlWaitDmaBuffer(pDevice, arg);
    break;
    case IOCTL_AMB_WAIT_DMA_BLOCK:
        status = IoctlWaitDmaBlock(pDevice, arg);
    break;
    case IOCTL_AMB_SET_TETRIRQ:
        status = IoctlSetTetradIrq(pDevice, arg);
    break;
    case IOCTL_AMB_CLEAR_TETRIRQ:
        status = IoctlClearTetradIrq(pDevice, arg);
    break;
    case IOCTL_AMB_WAIT_TETRIRQ:
        status = IoctlWaitTetradIrq(pDevice, arg);
    break;
    default:
        dbg_msg(dbg_trace, "%s(): Unsopported ioctl command = 0x%x\n", __FUNCTION__, cmd);
    break;
    }

    return status;
}

//-----------------------------------------------------------------------------

static inline int private_mapping_ok(struct vm_area_struct *vma)
{
    return vma->vm_flags & VM_MAYSHARE;
}

//-----------------------------------------------------------------------------

int adp_mmap(struct file *file, struct vm_area_struct *vma)
{
    size_t size = vma->vm_end - vma->vm_start;

#ifdef DZYTOOLS_2_4_X

    vma->vm_flags |= VM_RESERVED;

    if( remap_page_range(vma->vm_start, VMA_OFFSET(vma),
                         vma->vm_end-vma->vm_start,
                         vma->vm_page_prot)) {
        err_msg(err_trace, "%s(): error in remap_page_range.\n", __FUNCTION__ );
        return -EAGAIN;
    }
#else
    if (!private_mapping_ok(vma))
        return -EINVAL;

    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    if (remap_pfn_range(vma,
                        vma->vm_start,
                        vma->vm_pgoff,
                        size,
                        vma->vm_page_prot)) {
        err_msg(err_trace, "%s(): error in remap_page_range.\n", __FUNCTION__ );
        return -EAGAIN;
    }
#endif

    return 0;
}

