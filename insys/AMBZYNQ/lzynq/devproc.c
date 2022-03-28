
#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/pagemap.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>


#include "ddzynq.h"
#include "zynqdev.h"
#include "wambpmc.h"
#include "devproc.h"
#include "dmachan.h"

#define print_info(p, S...) seq_printf(p, S)

//--------------------------------------------------------------------

static int board_fpga_info(struct CWambpex *brd, void *buf)
{
    u32 d = 0;
    u32 d1 = 0;
    u32 d2 = 0;
    u32 d3 = 0;
    u32 d4 = 0;
    u32 d5 = 0;
    u32 status = 0;
    int ii = 0;
    void *p = buf;

    if(!p || !brd)
        goto err_exit;

    print_info(p, "\n" );
    print_info(p, "  Прошивка ПЛИС ADM\n" );

    RegPokeDir( brd, 0, 1, 1 );

    d=RegPeekInd( brd, 0, 0x108 );
    if( d==0x4953 ) {
        print_info(p, "  SIG= 0x%04X - Ok\n", d );
    } else {
        print_info(p, "  SIG= 0x%04X - Ошибка, ожидается 0x4953\n", d );
        goto err_exit;
    }

    d=RegPeekInd( brd,  0, 0x109 );  print_info(p, "  Версия интерфейса ADM:  %d.%d\n", d>>8, d&0xFF );
    d=RegPeekInd( brd,  0, 0x110 ); d1=RegPeekInd( brd,  0, 0x111 );
    print_info(p, "  Базовый модуль: 0x%04X  v%d.%d\n", d, d1>>8, d1&0xFF );

    d=RegPeekInd( brd,  0, 0x112 ); d1=RegPeekInd( brd,  0, 0x113 );
    print_info(p, "  Субмодуль:      0x%04X  v%d.%d\n", d, d1>>8, d1&0xFF );

    d=RegPeekInd( brd,  0, 0x10B );  print_info(p, "  Модификация прошивки ПЛИС:  %d \n", d );
    d=RegPeekInd( brd,  0, 0x10A );  print_info(p, "  Версия прошивки ПЛИС:       %d.%d\n", d>>8, d&0xFF );
    d=RegPeekInd( brd,  0, 0x114 );  print_info(p, "  Номер сборки прошивки ПЛИС: 0x%04X\n", d );

    print_info(p, "\n  Информация о тетрадах:\n\n" );
    for( ii=0; ii<16; ii++ ) {

        char *str;

        d=RegPeekInd( brd,  ii, 0x100 );
        d1=RegPeekInd( brd,  ii, 0x101 );
        d2=RegPeekInd( brd,  ii, 0x102 );
        d3=RegPeekInd( brd,  ii, 0x103 );
        d4=RegPeekInd( brd,  ii, 0x104 );
        d5=RegPeekInd( brd,  ii, 0x105 );
        status = RegPeekDir( brd,  ii, 0x0 );

        switch( d ) {
        case 1: str="TRD_MAIN      "; break;
        case 2: str="TRD_BASE_DAC  "; break;
        case 3: str="TRD_PIO_STD   "; break;
        case 0:    str=" -            "; break;
        case 0x47: str="SBSRAM_IN     "; break;
        case 0x48: str="SBSRAM_OUT    "; break;
        case 0x12: str="DIO64_OUT     "; break;
        case 0x13: str="DIO64_IN      "; break;
        case 0x14: str="ADM212x200M   "; break;
        case 0x5D: str="ADM212x500M   "; break;
        case 0x41: str="DDS9956       "; break;
        case 0x4F: str="TEST_CTRL     "; break;
        case 0x3F: str="ADM214x200M   "; break;
        case 0x40: str="ADM216x100    "; break;
        case 0x2F: str="ADM28x1G      "; break;
        case 0x2D: str="TRD128_OUT    "; break;
        case 0x4C: str="TRD128_IN     "; break;
        case 0x30: str="ADMDDC5016    "; break;
        case 0x2E: str="ADMFOTR2G     "; break;
        case 0x49: str="ADMFOTR3G     "; break;
        case 0x67: str="DDS9912       "; break;
        case 0x70: str="AMBPEX5_SDRAM "; break;
        case 0x71: str="TRD_MSG       "; break;
        case 0x72: str="TRD_TS201     "; break;
        case 0x73: str="TRD_STREAM_IN "; break;
        case 0x74: str="TRD_STREAM_OUT"; break;


        default: str="UNKNOWN       "; break;
        }
        print_info(p, "  %02d  0x%04X 0x%04X %s ", ii, d, status, str );
        if( d>0 ) {
            print_info(p, " MOD: %-2d VER: %d.%d ", d1, d2>>8, d2&0xFF );
            if( d3 & 0x10 ) {
                print_info(p, "FIFO IN   %dx%d\n", d4, d5 );
            } else if( d3 & 0x20 ) {
                print_info(p, "FIFO OUT  %dx%d\n", d4, d5 );
            } else {
                print_info(p,   "\n" );
            }
        } else {
            print_info(p,   "\n" );
        }
    }

    return 0;

err_exit:
    return -EINVAL;
}

//--------------------------------------------------------------------

static int board_info(struct CWambpex *brd,  void *buf)
{
    int iBlock=0;
    int i=0;
    void *p = buf;

    if(!brd) {
        print_info(p, "  Invalid device pointer\n" );
        return -ENODEV;
    }

    print_info(p, "\n" );
    print_info(p, "  m_name = %s\n", brd->m_name );
    print_info(p, "  TAG = 0x%x, PID = %d\n", GetTAG(brd), GetPID(brd));
    print_info(p, "  m_BusNumber = 0x%X\n", (int)brd->m_BusNumber );
    print_info(p, "  m_DeviceNumber = 0x%X\n", (int)brd->m_DevFNumber);
    print_info(p, "  m_SlotNumber = 0x%X\n", (int)brd->m_SlotNumber);
    print_info(p, "  m_DevFNumber = 0x%X\n", brd->m_DevFNumber);
    print_info(p, "  m_DeviceID = 0x%X\n", brd->m_DeviceID);
    print_info(p, "  m_RevisionID = 0x%X\n", brd->m_RevisionID);
    for(i=0; i<ZYNQ_FPGA_IRQS; i++) {
        print_info(p, "  m_Interrupt[%d] = %d\n", i, brd->m_Interrupt[i]);
    }
    print_info(p, "  m_DmaIrqEnbl = %i\n", brd->m_DmaIrqEnbl);
    print_info(p, "  m_FlgIrqEnbl = %i\n", brd->m_FlgIrqEnbl);
    print_info(p, "  m_DmaChanMask = 0x%x\n", brd->m_DmaChanMask);
    print_info(p, "\n" );
    print_info(p, "  m_TotalIRQ = %d\n", atomic_read(&brd->m_TotalIRQ));
    print_info(p, "  m_SpuriousIRQ = %d\n", atomic_read(&brd->m_SpuriousIRQ));
    print_info(p, "\n" );
    for(i = 0; i < MAX_NUMBER_OF_DMACHANNELS; i++) {
        print_info(p, "  m_DmaIRQ[%d] = %d\n", i, atomic_read(&brd->m_DmaIRQ[i]));
    }
    /*
    print_info(p, "\n" );
    for(i = 0; i < NUM_TETR_IRQ; i++) {
        print_info(p, "  m_FlgIRQ[%d] = %d\n", i, atomic_read(&brd->m_FlgIRQ[i]));
    }
    */
    print_info(p, "\n" );
    print_info(p, "  m_OperationRegisters.PhysicalAddress = %p\n", (void*)brd->m_OperationRegisters.PhysicalAddress);
    print_info(p, "  m_AmbMainRegisters.PhysicalAddress = %p\n", (void*)brd->m_AmbMainRegisters.PhysicalAddress);
    print_info(p, "\n" );
    print_info(p, "  m_OperationRegisters.VirtualAddress = %p\n", (void*)brd->m_OperationRegisters.VirtualAddress);
    print_info(p, "  m_AmbMainRegisters.VirtualAddress = %p\n", (void*)brd->m_AmbMainRegisters.VirtualAddress);

    print_info(p, "\n" );
    print_info(p, "  PE_MAIN_INFO\n" );
    print_info(p, "\n" );
    print_info(p, "  BLOCK_ID = 0x%04x\n", (ReadOperationReg(brd, PEMAINadr_BLOCK_ID) & 0x0FFF));
    print_info(p, "  BLOCK_VER = 0x%04x\n", (ReadOperationReg(brd, PEMAINadr_BLOCK_VER) & 0xFFFF));
    print_info(p, "  DEVICE_ID = 0x%04x\n", (ReadOperationReg(brd, PEMAINadr_DEVICE_ID) & 0xFFFF));
    print_info(p, "  DEVICE_REV = 0x%04x\n", (ReadOperationReg(brd, PEMAINadr_DEVICE_REV) & 0xFFFF));
    print_info(p, "  PLD_VER = 0x%04x\n", (ReadOperationReg(brd, PEMAINadr_PLD_VER) & 0xFFFF));
    print_info(p, "  BLOCK_CNT = 0x%04x\n", (ReadOperationReg(brd, PEMAINadr_BLOCK_CNT) & 0xFFFF));
    print_info(p, "  CORE_ID_MOD = 0x%04x\n", (ReadOperationReg(brd, PEMAINadr_DPRAM_SIZE) & 0xFFFF));
    print_info(p, "  CORE_ID = 0x%04x\n", (ReadOperationReg(brd, PEMAINadr_DPRAM_SIZE) & 0xFFFF) );
    print_info(p, "  BOARD_NUMBER = 0x%08x\n", ReadOperationReg(brd, 0xF8) );
    print_info(p, "  DPRAM_OFFEST = %x\n", (ReadOperationReg(brd, PEMAINadr_DPRAM_OFFSET) & 0xFFFF) );
    print_info(p, "  DPRAM_SIZE = %x\n", (ReadOperationReg(brd, PEMAINadr_DPRAM_SIZE) & 0xFFFF) );
    print_info(p, "\n" );

    for(iBlock = 1; iBlock < brd->m_BlockCnt; iBlock++)
    {
        u32 FifoAddr = 0;
        u16 val = 0;

        FifoAddr = iBlock * PE_BLK_SIZE;
        val = ReadOperationReg(brd, PEFIFOadr_BLOCK_ID + FifoAddr);

        if((val & 0x0FFF) != PE_EXT_FIFO_ID)
            continue;

        print_info(p, "\n" );
        print_info(p, "  PE_EXT_FIFO %d\n", iBlock+1 );
        print_info(p, "\n" );

        print_info(p, "  BLOCK_ID = %x\n", (val & 0x0FFF) );
        val = ReadOperationReg(brd, PEFIFOadr_BLOCK_VER + FifoAddr);
        print_info(p, "  BLOCK_VER = %x\n", (val & 0xFFFF) );
        val = ReadOperationReg(brd, PEFIFOadr_FIFO_ID + FifoAddr);
        print_info(p, "  FIFO_ID = %x\n", (val & 0xFFFF) );
        val = ReadOperationReg(brd, PEFIFOadr_FIFO_NUM + FifoAddr);
        print_info(p, "  FIFO_NUMBER = %x\n", (val & 0xFFFF) );
        val = ReadOperationReg(brd, PEFIFOadr_DMA_SIZE + FifoAddr);
        print_info(p, "  RESOURCE = %x\n", (val & 0xFFFF) );
        val = ReadOperationReg(brd, PEFIFOadr_FIFO_CTRL + FifoAddr);
        print_info(p, "  DMA_MODE = %x\n", (val & 0xFFFF) );
        val = ReadOperationReg(brd, PEFIFOadr_DMA_CTRL + FifoAddr);
        print_info(p, "  DMA_CTRL = %x\n", (val & 0xFFFF) );
        val = ReadOperationReg(brd, PEFIFOadr_FIFO_STATUS + FifoAddr);
        print_info(p, "  FIFO_STATUS = %x\n", (val & 0xFFFF) );
        val = ReadOperationReg(brd, PEFIFOadr_FLAG_CLR + FifoAddr);
        print_info(p, "  FLAG_CLR = %x\n", (val & 0xFFFF) );
        val = ReadOperationReg(brd, PEFIFOadr_PCI_ADDRL + FifoAddr);
        print_info(p, "  PCI_ADRL = %x\n", (val & 0xFFFF) );
        val = ReadOperationReg(brd, PEFIFOadr_PCI_ADDRH + FifoAddr);
        print_info(p, "  PCI_ADRH = %x\n", (val & 0xFFFF) );
        val = ReadOperationReg(brd, PEFIFOadr_LOCAL_ADR + FifoAddr);
        print_info(p, "  LOCAL_ADR = %x\n", (val & 0xFFFF) );
    }

    print_info(p, "\n" );

    return 0;
}

//--------------------------------------------------------------------

int board_proc_info( char *buf, char **start, off_t off,
                   int count, int *eof, void *data )
{
    return 0;
}

static int board_proc_show(struct seq_file *m, void *v)
{
    struct CWambpex* brd = m->private;

    board_fpga_info(brd, m);
    board_info(brd, m);

    return 0;
}

//--------------------------------------------------------------------

static int board_proc_open(struct inode *inode, struct file *file)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
    struct CWambpex* brd = PDE_DATA(inode);
#else
    struct CWambpex* brd = PDE(inode)->data;
#endif

    return single_open(file, board_proc_show, brd);
}

//--------------------------------------------------------------------

static int board_proc_release(struct inode *inode, struct file *file)
{
    return single_release(inode, file);
}

//--------------------------------------------------------------------

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops board_proc_fops = {
    .proc_open           = board_proc_open,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = board_proc_release,
};
#else
static const struct file_operations board_proc_fops = {
    .owner          = THIS_MODULE,
    .open           = board_proc_open,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = board_proc_release,
};
#endif

//--------------------------------------------------------------------

void board_register_proc(char *name, void *data)
{
    struct CWambpex *brd = (struct CWambpex *)data;

    if(!brd) {
        dbg_msg( dbg_trace, "%s(): Invalid driver pointer\n", __FUNCTION__ );
        return;
    }

    if(!proc_create_data(name, S_IRUGO, NULL, &board_proc_fops, brd)) {
        dbg_msg(1, "%s(): Error register proc entry: /proc/%s\n", __FUNCTION__, name);
    }
}

//--------------------------------------------------------------------

void board_remove_proc( char *name )
{
    remove_proc_entry( name, NULL );
}

//--------------------------------------------------------------------
