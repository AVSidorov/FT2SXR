//=********************************************************
// IntStrm.h - Classes TInterStream, TInterStream9056
//             Declaration
//
// (C) Copyright Ekkore 2002
//
// Created:
//              04.12.2002 by Ekkore
// Modified:
//
//=********************************************************

#ifndef		_INTSTRM_H_
#define		_INTSTRM_H_

#ifndef 	_INTRUPT_H_
        #include "intrupt.h"
#endif
#ifndef 	_SUSPENDR_H_
        #include "suspendr.h"
#endif

#ifdef DZYTOOLS_2_4_X

struct device {
    struct pci_dev dev;
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


//=******************* struct TStreamStub *****************
//=********************************************************
typedef struct
{
    u32            lastBlock;                    // Number of Block which was filled last Time
    u32            totalCounter;                 // Total Counter of all filled Block
    u32            offset;                       // First Unfilled Byte
    u32            state;                        // CBUF local state
} TStreamStub;

//=******************* struct TStreamAlloc ****************
//=********************************************************
typedef struct
{
    u32            blkNum;
    u32            blkSize;
    u32           *pStub;
    u32           *pBlk[1];
} TStreamLock;

//=******************* struct TIStreamState ***************
//=********************************************************
typedef struct
{
    u32            lastBlock;                    // Number of Block which was filled last Time
    u32            totalCounter;                 // Total Counter of all filled Block
    u32            offset;                       // First Unfilled Byte
    u32            state;                        // CBUF local state
    u32            timeout;
} TStreamState;

//=******************* struct TBlockAdrInfo ***************
//=********************************************************
typedef struct
{
    void          *sysAdr;                       // System Virtual Address of Block
    dma_addr_t     phyAdr;                       // Physical Address of Block (if Allocated)
} TBlockAdrInfo;

//=******************* class TInterCBuf *****************
//=********************************************************
typedef struct _TInterCBuf
{
    struct device  *m_pci;                       // Pointer of the PCI device structure

    u32            m_blkNum;                     // Number of Blocks
    u32            m_blkSize;                    // Size of every Block (bytes)
    u32            m_lastBlock;                  // Number of Block which was filled last Time

    TBlockAdrInfo  m_stub;                       // Stub Pointers
    TBlockAdrInfo *m_arrBlock;                   // Array of Block Pointers
    dma_addr_t	   m_arrBlockPA;                   // Array of Block Pointers
    TInterRuptor  *m_IR;

    atomic_t       m_isOperationInUse;           // It is Used like a SpinLock
    u32            m_isAlloc;                    // !0 if Blocks were allocated, 0 - locked

} TInterCBuf;

//
// Circular buffer functions
//

TInterCBuf    *TInterCBufCreate ( struct device *pci );
void           TInterCBufInit ( TInterCBuf * pcbuf, struct device *pci );
void           TInterCBufDelete ( TInterCBuf * pcbuf );
int            TInterCBufAlloc ( TInterCBuf * pcbuf, size_t blkNum, size_t blkSize, size_t** pStub, size_t* pBlk[] );
int            TInterCBufDisalloc ( TInterCBuf * pcbuf );
int            TInterCBufDisallocOnly ( TInterCBuf * pcbuf );
void           TInterCBufQueueDPC ( TInterCBuf * pcbuf );

int  lock_pages( void *va, u32 size );
int  unlock_pages( void *va, u32 size );

//=******************* class TInterStream9056 *************
//=********************************************************

//
// Runtime Registers
//
enum
{
    plxMBOX0 = 0x78,
    plxMBOX1 = 0x7C,
    plxMBOX2 = 0x48,
    plxMBOX3 = 0x4C,
    plxMBOX4 = 0x50,
    plxMBOX5 = 0x54,
    plxMBOX6 = 0x58,
    plxMBOX7 = 0x5C,
    plxP2LDBELL = 0x60,
    plxL2PDBELL = 0x64,
    plxINTCSR = 0x68,
    plxCNTRL = 0x6C,
    plxPCIHIDR = 0x70,
    plxPCIHREV = 0x74,
};

//
// PLX9056 DMA Registers
//
enum
{
    plxDMAMODE0 = 0x80,
    plxDMAPADR0 = 0x84,
    plxDMALADR0 = 0x88,
    plxDMASIZ0 = 0x8C,
    plxDMADPR0 = 0x90,
    plxDMAMODE1 = 0x94,
    plxDMAPADR1 = 0x98,
    plxDMALADR1 = 0x9C,
    plxDMASIZ1 = 0xA0,
    plxDMADPR1 = 0xA4,
    plxDMACSR0 = 0xA8,
    plxDMACSR1 = 0xA9,
    plxDMAARB = 0xAC,
    plxDMATHR = 0xB0,
};

typedef struct
{
    u32            padr,
                   ladr,
                   siz,
                   dpr;
} TPlxChainLink;

typedef struct _TInterStream9056
{
    void          *m_pAddress;                   // PLX Memory Area mapped address
    u32            m_locAdr;                     // Local Address of FIFO
    u32            m_dmaChan;                    // DMA Channel Number: 0, 1
    u32            m_isCycle;                    // 1-Cycle Mode
    u32            m_dir;                        // Direction: 1-to HOST, 2-from HOST
    TPlxChainLink *m_arrChain;                   // PLX Chain
    dma_addr_t	   m_arrChainPA;
    TInterCBuf    *m_Buffer;
    TStreamSuspender *m_suspender;
    u32            m_chainSize;                  // Number of Links in the Chain
    int            m_keeBlkDone;
    int            m_keeIsAdjust;

    u32            m_waitStates;                 // Wait States on Local Bus
    u32            m_dmaCodeStart;               // Code to Write to PLX DMAMODE# Register if Start
    u32            m_dmaCodeStop;                // Code to Write to PLX DMAMODE# Register if Stop

    struct stream_operations *m_op;		 // Stream low-level operations

} TInterStream9056;

TInterStream9056 *TInterStream9056Create ( struct device *pci,
                                           void *pciPlxAddress,
                                           u32 locFifoBlockAddress,
                                           u32 dmaChan );
void           TInterStream9056Init ( TInterStream9056 * strm,
                                      void *pciPlxAddress,
                                      u32 locFifoBlockAddress, u32 dmaChan );
void 	       TInterStream9056Delete ( TInterStream9056 * strm );

int            TInterStream9056Alloc ( TInterStream9056 * strm, size_t blkNum, size_t blkSize, size_t** pStub, size_t* pBlk[] );
int            TInterStream9056Disalloc ( TInterStream9056 * strm );

int            TInterStream9056Start ( TInterStream9056 * strm, u32 isCycle, u32 dir );
int            TInterStream9056Stop ( TInterStream9056 * strm );
int            TInterStream9056Wait ( TInterStream9056 * strm, TStreamState * pSS, u32 * pTimeElapsed );

int            DoneSyncronize ( void *pSynchronizeContext );
int            AdjustSyncronize ( void *pSynchronizeContext );

int            TInterStream9056Done ( TInterStream9056 * strm, int blkNo );
int            TInterStream9056Adjust ( TInterStream9056 * strm, int isAdjust );
int            TInterStream9056Isr ( TInterStream9056 * strm );
int            TInterStream9056CreateChain ( TInterStream9056 * strm );
int            TInterStream9056FreeChain ( TInterStream9056 * strm );
int            TInterStream9056Suspend ( TInterStream9056 * strm );
int            TInterStream9056Resume ( TInterStream9056 * strm );
int            TInterStream9056IsStarted ( TInterStream9056 * strm );
void           TInterStream9056SetWaitStates ( TInterStream9056 * strm, u32 waitStates );
void           TInterStream9056SetDmaCodeStart ( TInterStream9056 * strm, u32 dmaCodeStart );
void           TInterStream9056SetDmaCodeStop ( TInterStream9056 * strm, u32 dmaCodeStop );

struct stream_operations {

    u32  (*ReadPlxMem32)( void *m_pAddress, u32 offset );
    void (*WritePlxMem32)( void *m_pAddress, u32 offset, u32 value );
    u8   (*ReadPlxMem08)( void *m_pAddress, u32 offset );
    void (*WritePlxMem08)( void *m_pAddress, u32 offset, u8 value );

};

extern struct stream_operations s_ops;

#endif //_INTSTRM_H_

//
// End of File
//
