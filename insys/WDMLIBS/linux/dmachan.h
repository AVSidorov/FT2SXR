//****************** File DmaChannel.h *********************************
//  class CDmaChannel definition
//
//	Copyright (c) 2007, Instrumental Systems,Corp.
//  Written by Dorokhin Andrey
//
//  History:
//  28-10-07 - builded
//
//*******************************************************************

#ifndef		_DMA_CHAN_H_
#define		_DMA_CHAN_H_

//#ifndef _PLX9x56_H_
//#include "plx9x56.h"
//#endif

#include "event.h"

#ifdef DZYTOOLS_2_4_X

struct device {
        struct pci_dev pdev;
};

static inline struct pci_dev *to_pci_dev(struct device *dev)
{
        return (struct pci_dev *) dev;
}

#define dma_alloc_coherent(dev,sz,dma,gfp) \
        pci_alloc_consistent(to_pci_dev(dev),(sz),(dma))
#define dma_free_coherent(dev,sz,addr,dma_addr) \
        pci_free_consistent(to_pci_dev(dev),(sz),(addr),(dma_addr))
#endif

#define PE_EXT_FIFO_ID	0x0018

// type of memory
enum {
	USER_MEMORY_TYPE	= 0,
	SYSTEM_MEMORY_TYPE	= 1,
	PHYS_MEMORY_TYPE	= 8
                      };

//  state of DMA channel
enum
{
    STATE_RUN = 1,
    STATE_STOP = 2,
    STATE_DESTROY = 3,
    STATE_BREAK = 4
              };

//  direction of DMA transfer
enum
{
    TRANSFER_DIR_TO_DEVICE = 0x0,	// From Host to Device
    TRANSFER_DIR_FROM_DEVICE = 0x1	// From Device to Host
                           };

// returns the number of pages spanned by the size.
#define PAGES_SPANNED(Size) \
(((Size) + (PAGE_SIZE - 1)) >> PAGE_SHIFT)

// Shared Memory between kernel and user mode description
typedef struct _SHARED_MEMORY_DESCRIPTION {
    void*           SystemAddress;  // адрес блока памяти выделенного в пространстве ядра
    dma_addr_t      LogicalAddress; // физический адрес блока памяти выделенного в пространстве ядра
    struct page**   Pages;          // указатели на страницы блока памяти выделенного в пространстве пользователя
    size_t          PageCount;      // количество страниц составляющи блок памяти выделенный в пространстве пользователя
    dma_addr_t*     PageAddresses;  // физические адреса страниц блока памяти выделенного в пространстве ядра
	u32				EotFlag;		// флаг End of Transfer (генерит прерывание)
	u32				BlockInc;		// инкремент блока между прерываниями
} SHARED_MEMORY_DESCRIPTION, *PSHARED_MEMORY_DESCRIPTION;

// Stub Structure
typedef struct _AMB_STUB {
    s32	lastBlock;		// Number of Block which was filled last Time
    u32	totalCounter;	// Total Counter of all filled Block
    u32	offset;			// First Unfilled Byte
    u32	state;			// CBUF local state
} AMB_STUB, *PAMB_STUB;

//
// DMA chaining descriptor for PLX
//
typedef struct _DMA_CHAINING_DESCRIPTOR {

    u32  PciAddrLo;				// PCI Address (low part)
    u32  PciAddrHi;             // PCI Address (hi part)
    u32  DmaLength;				// Length of DMA Transfer (Max DMA_SIZE bits)

    struct {
        u32 EndOfChain : 1;   // End of Chain
        u32 EndOfTrans : 1;   // End of Transfer
        u32 Res        : 30;  // reserved
    } Cmd;							// Descriptor Command

    u32  NextAddrLo;				// Next Descriptor Address (low part)
    u32  NextAddrHi;				// Next Descriptor Address (hi part)
    u32  Signature;					// 0xA5A64953
    u32  Crc;						// control code

} DMA_CHAINING_DESCRIPTOR, *PDMA_CHAINING_DESCRIPTOR;


// Descriptor Structure (PE_EXT_FIFO)
// PCI Address (byte 0)	= 0 always
// DMA Length (byte 0)	= 0 always
typedef struct _DMA_CHAINING_DESCR_EXT {

    u8	AddrByte1;				// PCI Address (byte 1)
    u8	AddrByte2;				// PCI Address (byte 2)
    u8	AddrByte3;				// PCI Address (byte 3)
    u8	AddrByte4;				// PCI Address (byte 4)

    struct {
        u8 JumpNextDescr : 1;	// Jump to Next Descriptor
        u8 JumpNextBlock : 1;	// Jump to Next Block
        u8 JumpDescr0	: 1;	// Jump to Descriptor 0
        u8 Res0		: 1;	// reserved
        u8 EndOfTrans	: 1;	// End of Transfer
        u8 Res			: 3;	// reserved
    } Cmd;							// Descriptor Command

    u8	SizeByte1;				// DMA Length (byte 1), bit 0: 0 - TRANSFER_DIR_TO_DEVICE, 1 - TRANSFER_DIR_FROM_DEVICE
    u8	SizeByte2;				// DMA Length (byte 2)
    u8	SizeByte3;				// DMA Length (byte 3)

} DMA_CHAINING_DESCR_EXT, *PDMA_CHAINING_DESCR_EXT;

typedef struct _DMA_NEXT_BLOCK {

    u32	NextBlkAddr;
    u16 Signature;				// 0x4953
    u16 Crc;					// control code

} DMA_NEXT_BLOCK, *PDMA_NEXT_BLOCK;

#define DSCR_BLOCK_SIZE 64

typedef void (*TASKLET_ROUTINE)(unsigned long);

struct DMA_SG_CONTAINER {

    struct list_head m_list;
    dma_addr_t m_pa;
    void*      m_va;

};

#define SG_CONTAINER_SIZE 0x100000

struct CDmaChannel
{
    u32							m_NumberOfChannel;
    u16							m_idBlockFifo;

    void 						*m_Board;				// PCI device object
    struct device                                       *m_dev;					// PCI device object
    struct  tasklet_struct                              m_Dpc;              	// the DPC object
    wait_queue_head_t                                   m_DmaWq;
    spinlock_t                                          m_DmaLock;				// spinlock

    KEVENT						m_BlockEndEvent;
    KEVENT						m_BufferEndEvent;

    AMB_STUB                                            m_State;

    u32							m_AdmNum;
    u32							m_TetrNum;
    u32							m_DmaDirection;
    u32							m_DmaLocalAddress;
    u32							m_DmaCycling;

    u32							m_CycleNum;
    u32							m_BlocksRemaining;
    u32							m_CurBlockNum;

    u32							m_UseCount;

    u32							m_AdjustMode;
    u32							m_DoneBlock;
    u32							m_DoneFlag;

	u32							m_CurIrqNum;
	u32							m_IrqCount;

	u32							m_MemType;
    AMB_STUB                                            *m_pStub;

    SHARED_MEMORY_DESCRIPTION	m_StubDscr;		//Содержит описатель управляющего блока

    u32							m_BlockCount;	//Количество блоков для DMA
    u32							m_BlockSize;	//Размер одного блока DMA
    u32							m_preBlockCount1;
    u32							m_preBlockCount2;
    u32							m_preBlockCount3;

    SHARED_MEMORY_DESCRIPTION                           m_pBufDscr;	//Описатель для адресов блоков DMA каждый элемент содержит

    DMA_CHAINING_DESCRIPTOR                             *m_pScatterGatherTable;    		//Содержит массив для организации цепочек DMA (v1)
    DMA_CHAINING_DESCR_EXT                              *m_pScatterGatherTableExt; 		//Содержит массив для организации цепочек DMA (v2)
    u32							m_ScatterGatherBlockCnt;  		//Количество элементов-дескрипторов цепочек DMA
    SHARED_MEMORY_DESCRIPTION                           m_SGTableDscr;			   		//Описатель массива цепочек DMA
    u32							m_ScatterGatherTableEntryCnt; 	//Количество блоков для DMA

    TASKLET_ROUTINE                                     m_DpcForIsr;

    struct list_head            m_sg_blocks_list;
};

struct CDmaChannel* CDmaChannelCreate( 	u32 NumberOfChannel,
                                        TASKLET_ROUTINE dpc,
                                        void *m_Board,
                                        struct device	*dev,
                                        u32 cbMaxTransferLength,
                                        u16 idBlockFifo,
                                        int bScatterGather );
void CDmaChannelDelete(struct CDmaChannel *dma);
int RequestMemory(struct CDmaChannel *dma, void **ppMemPhysAddr, u32 size, u32 *pCount, void **pStubPhysAddr, u32 bMemType);
void ReleaseMemory(struct CDmaChannel *dma);
int RequestPhysAddrBuf(struct CDmaChannel *dma, void **pMemPhysAddr);
void ReleasePhysAddrBuf(struct CDmaChannel *dma);
int RequestSysBuf(struct CDmaChannel *dma, void **pMemPhysAddr);
void ReleaseSysBuf(struct CDmaChannel *dma);
int RequestUserAddrBuf(struct CDmaChannel *dma, void **pMemUserAddr);
int ReleaseUserAddrBuf(struct CDmaChannel *dma);
int RequestSGList(struct CDmaChannel *dma);
void ReleaseSGList(struct CDmaChannel *dma);
int RequestStub(struct CDmaChannel *dma, void **pStubPhysAddr);
void ReleaseStub(struct CDmaChannel *dma);
int SetScatterGatherListExt(struct CDmaChannel *dma);
void FreeUserAddress(struct CDmaChannel *dma);
u32 NextDmaTransfer(struct CDmaChannel *dma);
int SetScatterGatherList(struct CDmaChannel *dma);
int SetDmaDirection(struct CDmaChannel *dma, u32 DmaDirection);
void SetDmaLocalAddress(struct CDmaChannel *dma, u32 Address);
void SetLocalBusWidth(struct CDmaChannel *dma, u32 Param);
void ReferenceBlockEndEvent(struct CDmaChannel *dma, void* hBlockEndEvent);
void DereferenceBlockEndEvent(struct CDmaChannel *dma);
void SetAdmTetr(struct CDmaChannel *dma, u32 AdmNum, u32 TetrNum);
void Adjust(struct CDmaChannel *dma, u32 mode);
void GetState(struct CDmaChannel *dma, u32 *BlockNum, u32 *BlockNumTotal, u32 *OffsetInBlock, u32 *DmaChanState);
int CompleteDmaTransfer(struct CDmaChannel *dma);
int WaitBlockEndEvent(struct CDmaChannel *dma, u32 msTimeout);
int WaitBufferEndEvent(struct CDmaChannel *dma, u32 msTimeout);
u32 GetAdmNum(struct CDmaChannel *dma);
u32 GetTetrNum(struct CDmaChannel *dma);
void GetSGStartParams(struct CDmaChannel *dma, u64* SGTableAddress, u32* LocalAddress, u32* DmaDirection);
int StartDmaTransfer(struct CDmaChannel *dma, u32 IsCycling);
u32 SetDoneBlock(struct CDmaChannel *dma, long numBlk);
int unlock_pages( void *va, u32 size );
int lock_pages( void *va, u32 size );
DMA_CHAINING_DESCR_EXT *GetSGTExEntry(struct CDmaChannel *dma, u64 iEntry, dma_addr_t *physAddr);

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
#define dbg_msg(flag, S...) do { if(flag) PRINTK(KERN_DEBUG"DMACHAN: " S); } while(0)
#endif
#ifdef err_msg
#undef err_msg
#endif
#ifndef err_msg
#define err_msg(flag, S...) do { if(flag) PRINTK(KERN_ERR"DMACHAN: " S); } while(0)
#endif

#endif //_DMA_CHANNEL_H_

//
// End of File
//
