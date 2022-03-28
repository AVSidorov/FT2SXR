//=********************************************************
// StrmBox.h - Classes TStreamBox9056
//             Declaration
//
// (C) Copyright Ekkore 2002-2004
//
// Created:
//              08.06.2004 by Ekkore
// Modified:
//
//=********************************************************

#ifndef		_STRMBOX_H_
#define		_STRMBOX_H_

#ifndef _INTSTRM_H_
#include	"intstrm.h"
#endif

#ifndef _CAPTLIST_H_
#include	"captlist.h"
#endif

//
// Numbers of HOST_FIFO Block Registers
//
#    define		REG_BLOCK_ID		0x00
#    define		REG_BLOCK_VER		0x01
#    define		REG_FIFO0_ID		0x02
#    define		REG_FIFO1_ID		0x03
#    define		REG_FIFO2_ID		0x04
#    define		REG_FIFO3_ID		0x05
#    define		REG_FIFO4_ID		0x06
#    define		REG_FIFO5_ID		0x07
#    define		REG_FIFO0_CTRL		0x08
#    define		REG_FIFO1_CTRL		0x09
#    define		REG_FIFO2_CTRL		0x0A
#    define		REG_FIFO3_CTRL		0x0B
#    define		REG_FIFO4_CTRL		0x0C
#    define		REG_FIFO5_CTRL		0x0D

#    define		REG_FIFO0_DATA		0x10
#    define		REG_FIFO0_STATUS	0x11
#    define		REG_FIFO1_DATA		0x12
#    define		REG_FIFO1_STATUS	0x13
#    define		REG_FIFO2_DATA		0x14
#    define		REG_FIFO2_STATUS	0x15
#    define		REG_FIFO3_DATA		0x16
#    define		REG_FIFO3_STATUS	0x17
#    define		REG_FIFO4_DATA		0x18
#    define		REG_FIFO4_STATUS	0x19
#    define		REG_FIFO5_DATA		0x1A
#    define		REG_FIFO5_STATUS	0x1B

#    define		REG_FIFOn_ID(n)		(0x02+(n))
#    define		REG_FIFOn_CTRL(n)	(0x08+(n))
#    define		REG_FIFOn_DATA(n)	(0x10+2*(n))
#    define		REG_FIFOn_STATUS(n)	(0x11+2*(n))


#    define		STRMID0				0
#    define		STRMID1				1
#    define		STRMID2				2
#    define		STRMID3				3
#    define		STREAM_MAX			4

//=******************* class TStreamBox9056 ***************
//=********************************************************
typedef struct _TStreamBox9056
{
    void          *m_pciPlxAddress;              // PLX Memory Area mapped address
    void          *m_pciFifoBlockAddress;        // HOST_FIFO Block Memory Area mapped address
    u32            m_locFifoBlockAddress;        // HOST_FIFO Block Offset on Local Bus

    TInterStream9056 *m_stream[STREAM_MAX];      // m_stream[0,1]: DSP to HOST  Stream Support (DMA0 and DMA1)
						 // m_stream[2,3]: ADM express to HOST  Stream Support

    u32            m_dir[STREAM_MAX];            // Directions of m_stream[0-3]: 1-to HOST, 2-from HOST, 3-both

    TCaptureList  *m_pCaptList;                  // Capturer of DMA Resources
    int            m_captResource[STREAM_MAX];   // Captured Resources: 0 - "dma0", 1 - "dma1", -1 - none
    int            m_captPassword[STREAM_MAX];   // Passwords of Captured Resources

} TStreamBox9056;

TStreamBox9056 *TStreamBox9056Create ( struct device *pci,
                                       TCaptureList * pCaptList,
                                       void *pciPlxAddress,
                                       void *pciFifoBlockAddress,
                                       u32 locFifoBlockAddress );

void           TStreamBox9056Init ( TStreamBox9056 * box,
                                    TCaptureList * pCaptList,
                                    void *pciPlxAddress,
                                    void *pciFifoBlockAddress,
                                    u32 locFifoBlockAddress );

void 	       TStreamBox9056Delete ( TStreamBox9056 * box );

int            TStreamBox9056Alloc ( TStreamBox9056 * box, size_t strmId,
                                     size_t blkNum, size_t blkSize, size_t** pStub,
                                     size_t * pBlk[] );
int            TStreamBox9056Disalloc ( TStreamBox9056 * box, u32 strmId );

int            TStreamBox9056Start ( TStreamBox9056 * box, u32 strmId, u32 isCycle, u32 dir );  //dir=1 (to HOST), dir=2 (from HODST)
int            TStreamBox9056Stop ( TStreamBox9056 * box, u32 strmId );
int            TStreamBox9056Wait ( TStreamBox9056 * box, u32 strmId,
                                    TStreamState * pSS, u32 * pTimeElapsed );
int            TStreamBox9056Done ( TStreamBox9056 * box, u32 strmId,
                                    u32 blkNo );
int            TStreamBox9056Adjust ( TStreamBox9056 * box, u32 strmId,
                                      u32 isAdjust );


        //int   TStreamBox9056Isr( TStreamBox9056 *box, int StrmId );
int            TStreamBox9056Isr ( TStreamBox9056 * box );
void           TStreamBox9056Info ( TStreamBox9056 * box, u32 strmId,
                                    u32 * pAttr );
void           TStreamBox9056SetWaitStates ( TStreamBox9056 * box, u32 strmId,
                                             u32 waitStates );
void           TStreamBox9056SetDmaCodeStart ( TStreamBox9056 * box, u32 strmId,
                                               u32 dmaCodeStart );
void           TStreamBox9056SetDmaCodeStop ( TStreamBox9056 * box, u32 strmId,
                                              u32 dmaCodeStop );
void           TStreamBox9056ResetFifo ( TStreamBox9056 * box, u32 strmId );
void           TStreamBox9056GetDirections ( TStreamBox9056 * box );
void           TStreamBox9056BoxCaptureDma ( TStreamBox9056 * box, u32 strmId );

        //void          TStreamBox9056BoxReleaseDma(TStreamBox9056 *box, u32 strmId);
void           TStreamBox9056EnableDmaRequest ( TStreamBox9056 * box,
                                                u32 strmId, u32 dir );
void           TStreamBox9056DisableDmaRequest ( TStreamBox9056 * box,
                                                 u32 strmId );

u32            ReadFifoBlock32 ( TStreamBox9056 * box, u32 regNum );
void           WriteFifoBlock32 ( TStreamBox9056 * box, u32 regNum, u32 value );

        //void          TStreamBox9056GetDirections(TStreamBox9056 *box);
int            TStreamBox9056CaptureDma ( TStreamBox9056 * box, u32 strmId );
void           TStreamBox9056ReleaseDma ( TStreamBox9056 * box, u32 strmId );
void           TStreamBox9056ReleaseStartedDma ( TStreamBox9056 * box,
                                                 int wasStarted, u32 strmId,
                                                 u32 nFuncID );

/*
	void		Init( PKINTERRUPT pInterruptObject, TCaptureList *pCaptList, PVOID pciPlxAddress, PVOID pciFifoBlockAddress, UINT32 locFifoBlockAddress );
	NTSTATUS	CloseHandle( IN TMemAlloc *pMemAlloc, PFILE_OBJECT pFileObject );
	NTSTATUS	RemoveDevice( void );

	NTSTATUS	Lock( IN PFILE_OBJECT pFileObject, UINT32 strmId, UINT32 blkNum, UINT32 blkSize, UINT32 *ppStub, UINT32 *pBlk[] );
	NTSTATUS	Unlock( IN PFILE_OBJECT pFileObject, UINT32 strmId );
	NTSTATUS	Alloc( IN TMemAlloc *pMemAlloc, IN PFILE_OBJECT pFileObject, UINT32 strmId, UINT32 blkNum, UINT32 blkSize, UINT32 *pStub, UINT32 *pBlk[] );
	NTSTATUS	Disalloc( IN TMemAlloc *pMemAlloc, IN PFILE_OBJECT pFileObject, UINT32 strmId );

	NTSTATUS	Start(IN PFILE_OBJECT pFileObject, UINT32 strmId, UINT32 isCycle, UINT32 dir ); //dir=1 (to HOST), dir=2 (from HODST)
	NTSTATUS	Stop( IN PFILE_OBJECT pFileObject, UINT32 strmId );
	NTSTATUS	Wait( IN PFILE_OBJECT pFileObject, UINT32 strmId, IN OUT TStreamState *pSS, OUT UINT32 *pTimeElapsed );
	NTSTATUS	Done( IN PFILE_OBJECT pFileObject, UINT32 strmId, UINT32 blkNo );
	NTSTATUS	Adjust( IN PFILE_OBJECT pFileObject, UINT32 strmId, UINT32 isAdjust );

	BOOLEAN		Isr( void );

	void		Info(  UINT32 strmId, UINT32 *pAttr )   { GetDirections(); *pAttr = m_dir[strmId]; };

	void		SetWaitStates(  UINT32 strmId, UINT32 waitStates)   { m_stream[strmId].SetWaitStates(waitStates); };
	void		SetDmaCodeStart(UINT32 strmId, UINT32 dmaCodeStart) { m_stream[strmId].SetDmaCodeStart(dmaCodeStart); };
	void		SetDmaCodeStop( UINT32 strmId, UINT32 dmaCodeStop)  { m_stream[strmId].SetDmaCodeStop(dmaCodeStop); };
	void		ResetFifo(UINT32 strmId);

private:
	void		GetDirections(void);
	int			CaptureDma(UINT32 strmId);
	void		ReleaseDma(UINT32 strmId);
	void		ReleaseStartedDma(int wasStarted, UINT32 strmId, UINT32 nFuncID );
	void		EnableDmaRequest(UINT32 strmId, UINT32 dir);
	void		DisableDmaRequest(UINT32 strmId);

	//
	// HOST_FIFO Block Acesses
	//
	ULONG ReadFifoBlock32(ULONG regNum)
	{ return READ_REGISTER_ULONG( (PULONG)((PUCHAR)m_pciFifoBlockAddress + regNum*4)); }

	VOID WriteFifoBlock32(ULONG regNum, ULONG value)
	{ WRITE_REGISTER_ULONG( (PULONG)((PUCHAR)m_pciFifoBlockAddress + regNum*4), value); }
*/

#endif //_STRMBOX_H_

//
// End of File
//
