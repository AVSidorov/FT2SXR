//****************** File Wambpex.h *********************************
//  class CWambpex definition
//
//	Copyright (c) 2007, Instrumental Systems,Corp.
//  Written by Dorokhin Andrey
//
//  History:
//  20-03-07- builded
//
//*******************************************************************

#ifndef _ZYNQDEV_H_
#define _ZYNQDEV_H_

#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>

#include "zynqregs.h"
#include "streamll.h"
#include "event.h"
#include "dmachan.h"

// the increment applied to completed requests
#define MAX_NUMBER_OF_DMACHANNELS       4
#define NUMBER_OF_PLDS                  1
#define ZYNQ_FPGA_BARS                  2
#define ZYNQ_FPGA_IRQS                  4

#pragma pack(push,1)

// Shared Physical Memory between kernel and user mode description
typedef struct _SHARED_PHYSMEMORY_DESCRIPTION {
    void			*PhysAddress;	// OUT - user memory address
    void			*KernAddress;	// OUT - user memory address
    void			*UserAddress;	// OUT - user memory address
} SHARED_PHYSMEMORY_DESCRIPTION, *PSHARED_PHYSMEMORY_DESCRIPTION;

#define IRQ_NUMBER 1		// номер прерывания
#define NUM_TETR_IRQ 8		// число структур

// tetrad status interrupt request data structure
typedef struct _TETRAD_IRQ {
    KEVENT		Event;          // событие, возникающее по прерыванию от статуса тетрады
    size_t      EventId;
    u32			Address;
    u32			IrqMask;
    u32			IrqInv;
    struct tasklet_struct Dpc;
} TETRAD_IRQ, *PTETRAD_IRQ;

#pragma pack(pop)

struct address_t {

    size_t PhysicalAddress;
    void*  VirtualAddress;
    size_t Length;
    size_t Flags;

};

//-----------------------------------------------------------------------------

#define READ_CFG		0
#define WRITE_CFG		1

//-----------------------------------------------------------------------------

#define DRV_NAME "lzynq"

extern int dbg_trace;
extern int err_trace;

#ifdef PRINTK
#undef PRINTK
#endif
#ifndef PRINTK
#define PRINTK(S...) printk(S)
#endif

#ifdef dbg_msg
#undef dbg_msg
#endif
#ifndef dbg_msg
#define dbg_msg(flag, S...) do { if(flag) PRINTK(KERN_ALERT""DRV_NAME": " S); } while(0)
#endif
#ifdef err_msg
#undef err_msg
#endif
#ifndef err_msg
#define err_msg(flag, S...) do { if(flag) PRINTK(KERN_ERR""DRV_NAME": " S); } while(0)
#endif

//-----------------------------------------------------------------------------

extern struct list_head device_list;

//-----------------------------------------------------------------------------

typedef irqreturn_t (*zynq_isr_t)(int, void *);

//-----------------------------------------------------------------------------

struct CWambpex {

    struct list_head 	m_list;
    char            m_name[64];

    atomic_t 		m_TotalIRQ;
    atomic_t 		m_SpuriousIRQ;
    atomic_t 		m_DmaIRQ[MAX_NUMBER_OF_DMACHANNELS];
    atomic_t 		m_FlgIRQ[NUM_TETR_IRQ];

    struct device 	*m_dev;
    struct device 	*m_devobj;
    struct cdev     m_cdev;
    dev_t           m_devno;

    struct address_t    m_OperationRegisters;   // memory range information
    struct address_t    m_AmbMainRegisters;	// memory range information (ADM-interface)

    //u32         m_PID;
    u32 		m_DevFNumber;
    spinlock_t 		m_IsrLock[ZYNQ_FPGA_IRQS];
    struct semaphore 	m_BoardSem;
    struct semaphore 	m_BoardRegSem;
    u32 		m_BusNumber; 	// PCI bus number
    u32 		m_SlotNumber; 	// PCI slot number
    u16 		m_VendorID; 	// Device Identification
    u16 		m_DeviceID; 	// Device Identification
    u8 			m_RevisionID; 	// Revision Identification
    u32			m_BoardIndex;
    u32 		m_Interrupt[ZYNQ_FPGA_IRQS];
    zynq_isr_t  m_isr[ZYNQ_FPGA_IRQS];
    char        m_irq_name[ZYNQ_FPGA_IRQS][128];

    u32			m_GeographicAddr;		// Geographic Address - CompactPCI term (use for FMC115cP)
    u32			m_FpgaOrderNum;			// FPGA number - identification pins of FPGA (use for FMC115cP)

    KEVENT              m_Event;
    //struct timer_list	m_TimeoutTimer;			// Timeout Timer (have to callback function)
    //atomic_t		m_IsTimeout;			// Timeout flag for Timeout Timer callback function
    wait_queue_head_t   m_WaitQueue;
    u32			m_mcTimeout;

    struct CDmaChannel	*m_DmaChannel[MAX_NUMBER_OF_DMACHANNELS];	//

    u32			m_PldStatus[NUMBER_OF_PLDS];			//

    u32			m_MemOpUseCount;	// счетчик использования диапазона памяти (PE_MAIN on AMBPEX8)

    u16			m_BlockCnt;
    u32			m_DmaIrqEnbl;
    u32			m_FlgIrqEnbl;
    u32			m_DmaChanMask;
    u32			m_DmaChanEnbl[MAX_NUMBER_OF_DMACHANNELS];
    u32			m_DmaFifoSize[MAX_NUMBER_OF_DMACHANNELS];
    u32			m_DmaDir[MAX_NUMBER_OF_DMACHANNELS];
    u32			m_MaxDmaSize[MAX_NUMBER_OF_DMACHANNELS];
    u16			m_BlockFifoId[MAX_NUMBER_OF_DMACHANNELS];
    u32			m_FifoAddr[MAX_NUMBER_OF_DMACHANNELS];
    u32			m_BlockFidAddr;
    u32         m_primChan;

    TETRAD_IRQ 		m_TetrIrq[NUM_TETR_IRQ];

    u32         m_IrqMode[MAX_NUMBER_OF_DMACHANNELS];
    u32         m_IrqTable[512];
    u32         m_IrqTableNum;

    struct address_t    bar[ZYNQ_FPGA_BARS];
};

irqreturn_t    WambpexIsr( int irq, void *dev_id );
irqreturn_t    dma0_isr( int irq, void *dev_id );
irqreturn_t    dma1_isr( int irq, void *dev_id );

void WambpexDpcForIsr( unsigned long Context );
void TetrDpcForIsr(unsigned long Context);
int InitializeBoard(struct CWambpex *brd);
void UninitializeBoard(struct CWambpex *brd);

#define ReadDmaReg(brd, RegName)            ReadOperationReg (brd, DMA_REG_OFFSET(RegName))
#define WriteDmaReg(brd, RegName, val)      WriteOperationReg(brd, DMA_REG_OFFSET(RegName), val)

//
// Hardware access functions for BAR0 and BAR1 space
//
u32 ReadOperationReg(struct CWambpex *dev, u32 RelativePort);
void WriteOperationReg(struct CWambpex *dev, u32 RelativePort, u32 Value);
u32 ReadAmbMainReg(struct CWambpex *dev, u32 RelativePort);
void WriteAmbMainReg(struct CWambpex *dev, u32 RelativePort, u32 Value);
u32 ReadAmbReg(struct CWambpex *dev, u32 AdmNumber, u32 RelativePort);
void WriteAmbReg(struct CWambpex *dev, u32 AdmNumber, u32 RelativePort, u32 Value);
void ReadBufAmbReg(struct CWambpex *dev, u32 AdmNumber, u32 RelativePort, u32* VirtualAddress, u32 DwordsCount);
void WriteBufAmbReg(struct CWambpex *dev, u32 AdmNumber, u32 RelativePort, u32* VirtualAddress, u32 DwordsCount);
int WaitCmdReady(struct CWambpex *dev, u32 AdmNumber, u32 StatusAddress);
int WriteRegData(struct CWambpex *dev, u32 AdmNumber, u32 TetrNumber, u32 RegNumber, u32 Value);
int ReadRegData(struct CWambpex *dev, u32 AdmNumber, u32 TetrNumber, u32 RegNumber, u32 *Value);
u32 RegPeekInd(struct CWambpex *dev, u32 trdNo, u32 rgnum);
int RegPokeInd(struct CWambpex *dev, u32 trdNo, u32 rgnum, u32 Value);
int RegPokeDir(struct CWambpex *dev, u32 TetrNumber, u32 RegNumber, u32 Value);
int RegPeekDir(struct CWambpex *dev, u32 TetrNumber, u32 RegNumber);

/////////-------------------------------------------------
struct CWambpex *CWambpexCreate(void);
void CWambpexDelete( struct CWambpex *brd );
int InitializeBoard( struct CWambpex *brd );
void UninitializeBoard(struct CWambpex *brd);

int WaitCmdReady(struct CWambpex *brd, u32 AdmNumber, u32 StatusAddress);
int WriteRegData(struct CWambpex *brd, u32 AdmNumber, u32 TetrNumber, u32 RegNumber, u32 Value);
int ReadRegData(struct CWambpex *brd, u32 AdmNumber, u32 TetrNumber, u32 RegNumber, u32* Value);

int SetDmaMode(struct CWambpex *brd, u32 NumberOfChannel, u32 AdmNumber, u32 TetrNumber);
int SetDrqFlag(struct CWambpex *brd, u32 AdmNumber, u32 TetrNumber, u32 DrqFlag);
int DmaEnable(struct CWambpex *brd, u32 AdmNumber, u32 TetrNumber);
int DmaDisable(struct CWambpex *brd, u32 AdmNumber, u32 TetrNumber);

int ReadConfigSpace(struct CWambpex *brd, u8 *pBuffer, u32 Offset, u32 Length);
int WriteConfigSpace(struct CWambpex *brd, u8 *pBuffer, u32 Offset, u32 Length);

int SetPciBridge(struct CWambpex *brd);
int GetDeviceLocation(struct CWambpex *brd);

int SetDmaDelay(struct CWambpex *brd);

void ToPause(int time_out);			// timeout by PASSIVE_LEVEL IRQL (msec)
void ToTimeOut(int mctime_out);	// timeout by any IRQL (mcsec) - recommented minimum
int WaitVPDBusy(struct CWambpex *brd, int ReadOrWrite);
int WaitAdmIDROMBusy(struct CWambpex *brd);

int ReadEEPROM(struct CWambpex *brd, int idRom, u16 *buf, u16 Offset, u32 Length);
int WriteEEPROM(struct CWambpex *brd, int idRom, u16 *buf, u16 Offset, u32 Length);

int ReadFmcEeprom(struct CWambpex *brd, int devid, int devadr, u8 *buf, u16 Offset, long Length);
int WriteFmcEeprom(struct CWambpex *brd, int devid, int devadr, u8 *buf, u16 Offset, long Length);

int ReadAdmIDROM(struct CWambpex *brd, u16 *buf, long Offset, long Length);
int WriteAdmIDROM(struct CWambpex *brd, u16 *buf, long Offset, long Length);

//u8 PldGetHexRecord(u8 **pPldBuf, PLD_RECORD *rec);
//int PldLoadBitStream(struct CWambpex *brd, u32 pldNum, PLD_RECORD *record);
int SetPldMode(struct CWambpex *brd, u32 pldNum, u32 mode);
u32 CheckPld(struct CWambpex *brd, u32 pldNum);
u32 GetPldMode(struct CWambpex *brd, u32 pldNum);
int PutPldData(struct CWambpex *brd, u32 pldNum, u8 Data);
int PutPldBuf(struct CWambpex *brd, u32 pldNum, u32 *pAddress, u32 Count);

int SetAuxRegs(struct CWambpex *brd, u32 mode);

//void AcquireSpinLockAtDpc ( spinlock_t *lock );
//void ReleaseSpinLockFromDpc ( spinlock_t *lock );

void *GetMemUserAddr(struct CWambpex *brd);
void FreeMemUserAddr(struct CWambpex *brd);
void *GetAmbUserAddr(struct CWambpex *brd);
void FreeAmbUserAddr(struct CWambpex *brd);

int SetDmaDelay(struct CWambpex *brd);

//irqreturn_t WambpexIsr ( int irq, void *dev_id );
//void WambpDpcForIsr( unsigned long Context );
//void TetrDpcForIsr(unsigned long Context);

int HwStartDmaTransfer(struct CWambpex *brd, u32 NumberOfChannel);
int HwCompleteDmaTransfer(struct CWambpex *brd, u32 NumberOfChannel);

int Done(struct CWambpex *brd, u32 NumberOfChannel);
int ResetFifo(struct CWambpex *brd, u32 NumberOfChannel);
int SetIrqTable(struct CWambpex *brd, u32 NumberOfChannel, u32 Mode, u32 TableNum, u32* AddrTable);

#define ReadRuntimeReg(brd, RegName)        ReadOperationReg (brd, RUNTIME_REG_OFFSET(RegName))
#define WriteRuntimeReg(brd, RegName, val)  WriteOperationReg(brd, RUNTIME_REG_OFFSET(RegName), val)

int device_match(u16 device_id);
void device_name( struct CWambpex *brd, int id, int bn);

u32 GetPID(struct CWambpex *brd);
u32 GetTAG(struct CWambpex *brd);

#endif //_ZYNQDEV_H_
