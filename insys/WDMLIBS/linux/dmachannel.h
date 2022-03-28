//****************** File DmaChannel.h *********************************
//  class CDmaChannel definition
//
//	Copyright (c) 2004, Instrumental Systems,Corp.
//  Written by Dorokhin Andrey
//
//  History:
//  10-10-04 - builded
//
//*******************************************************************

#ifndef		_DMA_CHANNEL_H_
#define		_DMA_CHANNEL_H_

#include <linux/interrupt.h>

#ifndef _EVENT_H_
#include "event.h"
#endif
#ifndef _PLX9x56_H_
#include "plx9x56.h"
#endif

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

// type of memory
enum {
	USER_MEMORY_TYPE = 0, SYSTEM_MEMORY_TYPE = 1
};

//  state of DMA channel
enum {
	STATE_RUN = 1, STATE_STOP = 2, STATE_DESTROY = 3, STATE_BREAK = 4
};

//  Scatter/gather Descriptor Location
enum {
	LOCATION_LOCAL_ADDRESS_SPACE = 0, LOCATION_PCI_ADDRESS_SPACE = 1
};

//  direction of DMA transfer
enum {
	TRANSFER_DIR_TO_DEVICE = 0x0, // From Host to Device
	TRANSFER_DIR_FROM_DEVICE = 0x1
// From Device to Host
};

// returns the number of pages spanned by the size.
#define PAGES_SPANNED(Size) \
    (((Size) + (PAGE_SIZE - 1)) >> PAGE_SHIFT)

#define THREAD_BOOST_PRIORITY 2

//#pragma pack(push,1)

// Shared Memory between kernel and user mode description
typedef struct _USER_ADDRESS {
	void** ppUserMapAddresses; // user blocks map memory addresses
	void* pMapStub; // user stub map memory addresses
	int pFileObject; //
	void* NextAddress; // next structure address
} USER_ADDRESS, *PUSER_ADDRESS;

// Shared Memory between kernel and user mode description
typedef struct _SHARED_MEMORY_DESCRIPTION {
    void*           SystemAddress;  // адрес блока памяти выделенного в пространстве ядра
    dma_addr_t      LogicalAddress; // физический адрес блока памяти выделенного в пространстве ядра
    struct page**   Pages;          // указатели на страницы блока памяти выделенного в пространстве пользователя
    size_t          PageCount;      // количество страниц составляющи блок памяти выделенный в пространстве пользователя
    dma_addr_t*     PageAddresses;  // физические адреса страниц блока памяти выделенного в пространстве ядра
} SHARED_MEMORY_DESCRIPTION, *PSHARED_MEMORY_DESCRIPTION;

// Stub Structure
typedef struct _AMB_STUB {
	u32 lastBlock; // Number of Block which was filled last Time
	u32 totalCounter; // Total Counter of all filled Block
	u32 offset; // First Unfilled Byte
	u32 state; // CBUF local state
} AMB_STUB, *PAMB_STUB;

typedef struct _DMA_CHAINING_DESCRIPTOR {
	u32 DmaPciAddress; // PCI Address
	u32 DmaLocalAddress; // Local Address
	u32 DmaLength; // Length of DMA Transfer (Max 22 bits)
	struct {
		u32 LocationPCI :1; // Descriptor Location: 1 = PCI
		u32 EndOfChain :1; // End of Chain
		u32 IntrTC :1; // Interupt after Terminal Count
		u32 DirFromDevice :1; // Direction of Transfer: 1 = From Local Bus to PCI Bus
		u32 Address :28; // Address of Next Descriptor (Max 28 bits)
	} Next;
} DMA_CHAINING_DESCRIPTOR, *PDMA_CHAINING_DESCRIPTOR;

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS 0
#endif

typedef void (*TASKLET_ROUTINE)(unsigned long);

typedef struct CDmaChannel {

	u32 m_NumberOfChannel;
	spinlock_t m_DmaLock; // DMA Descriptor
        void *m_Board; // Store struct CWambp* pointer
        struct device *m_dev; // Adapter Object
	struct tasklet_struct m_Dpc; // the DPC object
	wait_queue_head_t m_DmaWq;

	KEVENT m_BlockEndEvent;
	KEVENT m_BufferEndEvent;

	AMB_STUB m_State; // for GetState

	u32 m_AdmNum;
	u32 m_TetrNum;
	u32 m_DmaDirection;
	u32 m_DmaLocalAddress;
	u32 m_LocalBusWidth;
	u32 m_DmaCycling;

	u32 m_CycleNum;
	u32 m_BlocksRemaining;
	u32 m_CurBlockNum;

	u32 m_UseCount;

	u32 m_MemType;
	PAMB_STUB m_pStub;

	u32 m_BlockCount;
	u32 m_BlockSize;

	SHARED_MEMORY_DESCRIPTION m_StubDscr;

        SHARED_MEMORY_DESCRIPTION *m_BufDscrVA;
        dma_addr_t m_BufDscrPA;

        DMA_CHAINING_DESCRIPTOR *m_pScatterGatherTableVA;
        dma_addr_t m_pScatterGatherTablePA;

        DMA_CHAINING_DESCRIPTOR *m_SGTableDscrVA;
        dma_addr_t  m_SGTableDscrPA;

	u32 m_ScatterGatherTableEntryCnt;

	PUSER_ADDRESS m_pUserAddress;

	TASKLET_ROUTINE WambpDpcForIsr;

} CDmaChannel;

int unlock_pages(void *va, u32 size);
int lock_pages(void *va, u32 size);

u32 GetAdmNum(struct CDmaChannel *dma);
u32 GetTetrNum(struct CDmaChannel *dma);
u32 GetLocalBusWidth(struct CDmaChannel *dma);

void SetDmaLocalAddress(struct CDmaChannel *dma, u32 Address);
void SetLocalBusWidth(struct CDmaChannel *dma, u32 Param);
void ReferenceBlockEndEvent(struct CDmaChannel *dma, void* hBlockEndEvent);
void DereferenceBlockEndEvent(struct CDmaChannel *dma);

struct CDmaChannel *CDmaChannelCreate(void *ambp, struct device *dev, u32 NumberOfChannel,
		TASKLET_ROUTINE DeferredRoutine, u32 cbMaxTransferLength,
		int bScatterGather);

void CDmaChannelDelete(struct CDmaChannel *dma);

int RequestMemory(struct CDmaChannel *dma, void **ppMemPhysAddr, u32 size,
		u32 *pCount, void **pStubPhysAddr, u32 bMemType);

void ReleaseMemory(struct CDmaChannel *dma);
void FreeUserAddress(struct CDmaChannel *dma);
int SetScatterGatherList(struct CDmaChannel *dma);
dma_addr_t NexDscrAddress(dma_addr_t physicalAddress);
int RequestSysBuf(struct CDmaChannel *dma, void **pMemPhysAddr);
void ReleaseSysBuf(struct CDmaChannel *dma);

int RequestUsrBuf(struct CDmaChannel *dma, void **pMemUserAddr);
void ReleaseUsrBuf(struct CDmaChannel *dma);

int RequestStub(struct CDmaChannel *dma, void **pStubPhysAddr);
void ReleaseStub(struct CDmaChannel *dma);
int RequestSGList(struct CDmaChannel *dma);
void ReleaseSGList(struct CDmaChannel *dma);
void QueuedStartMem(struct CDmaChannel *dma, u32 IsCycling);
int StartDmaTransfer(struct CDmaChannel *dma, u32 IsCycling);
void NextDmaTransfer(struct CDmaChannel *dma);
void GetState(struct CDmaChannel *dma, u32 *BlockNum, u32 *BlockNumTotal,
		u32 *OffsetInBlock, u32 *DmaChanState);
int WaitBlockEndEvent(struct CDmaChannel *dma, u32 msTimeout);
int WaitBufferEndEvent(struct CDmaChannel *dma, u32 msTimeout);
int CompleteDmaTransfer(struct CDmaChannel *dma);
void GetSGStartParams(struct CDmaChannel *dma, u32* SGTableAddress,
		u32* LocalAddress, u32* DmaDirection);
void GetStartParams(struct CDmaChannel *dma, u32* PciAddress,
		u32* LocalAddress, u32* DmaLength);
int SetDmaDirection(struct CDmaChannel *dma, u32 DmaDirection);
void SetAdmTetr(struct CDmaChannel *dma, u32 AdmNum, u32 TetrNum);
void FreezeState(struct CDmaChannel *dma);

#endif		//_DMA_CHANNEL_H_
//
// End of File
//
