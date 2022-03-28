//=********************************************************
// IntSvm.cpp - Classes TInterSveam Definition
//
// (C) Copyright Ekkore 2002-2003
//
// Created: 
//		22.10.2003 by Ekkore
// Modified:
//
//=********************************************************
//
// Sveam Performance:
// HOST moves Data with MOV Operation, Device is in PCI bus #2
//  
//
//   Read Sveam (DSP->HOST): up to 3.2 MB/sec
//  Write Sveam (HOST->DSP): up to 28 MB/sec
//
//
//=********************************************************

#include	"IntSvm.h"

#define		min(a,b)	(((a)<(b))?(a):(b))

//=********************************************************
//=********************************************************
//=********************************************************
//
// class TInterSveam
//



//=*********** TInterSveam::TInterSveam *******************
//=********************************************************
TInterSveam::TInterSveam( void )
{
	m_isStart = 0;
}
	
//=*********** TInterSveam::~TInterSveam ******************
//=********************************************************
			TInterSveam::~TInterSveam()
{
}
	
//=**************** TInterSveam::Init *********************
//=********************************************************
void TInterSveam::Init( UINT32 id )
{
	m_id = id;
}
	
//=**************** TInterSveam::Unlock *******************
//=********************************************************
NTSTATUS	TInterSveam::Unlock( IN PFILE_OBJECT pFileObject )
{
	NTSTATUS	status;

	status = Stop( pFileObject );
	status = TInterCBuf::Unlock( pFileObject );

	return status;
}

//=**************** TInterSveam::Disalloc *****************
//=********************************************************
NTSTATUS	TInterSveam::Disalloc( IN TMemAlloc *pMemAlloc, IN PFILE_OBJECT pFileObject )
{
	NTSTATUS	status;

	status = Stop( pFileObject );
	status = TInterCBuf::Disalloc( pMemAlloc, pFileObject );

	return status;
}

//=**************** TInterSveam::CloseHandle **************
//=********************************************************
NTSTATUS	TInterSveam::CloseHandle( IN TMemAlloc *pMemAlloc, PFILE_OBJECT pFileObject )
{
	if( m_pFileObject != pFileObject )
	{
		return STATUS_SUCCESS;
	}
	return TInterCBuf::CloseHandle( pMemAlloc, pFileObject );
}

//=***************** TInterSveam::Start *******************
//=********************************************************
NTSTATUS	TInterSveam::Start( IN PFILE_OBJECT pFileObject, UINT32 isCycle, UINT32 dir, 
							UINT32 descAdr, UINT32 isDescInDpram )
{
	NTSTATUS		status;

	//
	// Define Some variables
	//
	m_descAdr       = descAdr;
	m_isDescInDpram = isDescInDpram;

	//
	// Call TInterSveam::Sart()
	//
	status = TInterSveam::Start( pFileObject, isCycle, dir );
	if(!NT_SUCCESS(status) ) 
	{
        return status;
    }

	//
	// Define the Others
	//
	if( m_dpramDesc1Code&0x80000000 )
	{
		m_isBufInDpram = 1;	
		m_bufAdr = m_dpramOfsOfBuffer;
	}
	else
	{
		m_isBufInDpram = m_isDescInDpram;
		m_bufAdr = m_descAdr + m_dpramOfsOfBuffer;
	}
		
	return STATUS_SUCCESS;
}


//=***************** TInterSveam::Start *******************
//=********************************************************
NTSTATUS	TInterSveam::Start( IN PFILE_OBJECT pFileObject, UINT32 isCycle, UINT32 dir )
{
	//
	// If CBUF wasn't locked by this FileObject
	//
	if( pFileObject != m_pFileObject )
	{
		KdPrint((KDHEAD": ::Start() CBuf wasn't locked by this FileObject\n" ));
		return STATUS_UNSUCCESSFUL;
	}

	//
	// Explore the Head of Descriptor
	//
	UINT32		head0;
	UINT32		head1;

	PL_ReadDesc32( 0, &head0 );
	PL_ReadDesc32( 4, &head1 );
	if( (head0^head1) != 0xFFFFFFFF )
		return STATUS_UNSUCCESSFUL;
	if( (head0&0xFF) <= m_id )
		return STATUS_UNSUCCESSFUL;

	//
	// Explore the Sveam Descriptor
	//
	UINT32		desc0;
	UINT32		desc1;

	PL_ReadDesc32( 8+m_id*8+0, &desc0 );
	PL_ReadDesc32( 8+m_id*8+4, &desc1 );

	//
	// Check direction
	//
	if( (desc1&0x40000000)==0 )				// Direction: from DSP to HOST
	{	if( (dir&1)==0 )
			return STATUS_UNSUCCESSFUL;
	}
	else									// Direction: from HOST to DSP
	{	if( (dir&2)==0 )
			return STATUS_UNSUCCESSFUL;
	}

	//
	// Fill TInterRuptor member variables
	//
	m_totalCnt = 0;

	//
	// Fill TInterStream member variables
	//
	m_lastBlock = -1;
	if( m_stub.sysAdr )
	{
		((TStreamStub*)m_stub.sysAdr)->lastBlock    = m_lastBlock;
		((TStreamStub*)m_stub.sysAdr)->totalCounter = m_totalCnt;
		((TStreamStub*)m_stub.sysAdr)->offset = 0;
	}


	//
	// Fill TInterSveam member variables
	//
	m_dpramBlockNum   = ((desc1>>20)&0xF)+1;
	m_dpramBlockSizeb = (desc1&0xFFFFF)+1;
	m_dpramBufSizeb = m_dpramBlockNum*m_dpramBlockSizeb;

	for( m_dpramPointShift=0; m_dpramPointShift<32; m_dpramPointShift++ )
		if( (UINT32)(1<<m_dpramPointShift) > (m_dpramBufSizeb-1) )
			break;

	m_dpramOfsOfDspPoint  = 8 + (head0&0xFF)*8 + m_id*4;
	m_dpramOfsOfHostPoint = 8 + (head0&0xFF)*12+ m_id*4;
	m_dpramOfsOfBuffer = desc0;
	m_dpramDesc1Code   = desc1;

	UINT32		dpramDspPoint;

	PL_ReadDesc32( m_dpramOfsOfDspPoint, &dpramDspPoint );

	m_dpramHostOffset  = dpramDspPoint & ((1<<m_dpramPointShift)-1);
	m_dpramHostOffset &= ~0x3L;
	m_dpramHostCount   = dpramDspPoint >> m_dpramPointShift;

	m_cbufBlkOffset = 0;

	m_flagOverflow = 0;
	m_cntOverflow = 10;

	m_isCycle = isCycle;
	m_dir = dir;
	m_isStart = 1;

	return STATUS_SUCCESS;
}

//=**************** TInterSveam::Stop *********************
//=********************************************************
NTSTATUS	TInterSveam::Stop( IN PFILE_OBJECT pFileObject )
{
	//
	// If CBUF wasn't locked by this FileObject
	//
	if( pFileObject != m_pFileObject )
	{
		KdPrint((KDHEAD": ::Stop() CBuf wasn't locked by this FileObject\n" ));
		return STATUS_UNSUCCESSFUL;
	}

	m_isStart = 0;

	//
	// Write Stub.state
	//
	if( m_stub.sysAdr )
	{
		((TStreamStub*)m_stub.sysAdr)->state &= ~0xF;
		((TStreamStub*)m_stub.sysAdr)->state |= 2;
	}


	return STATUS_SUCCESS;
}

//=*********** TInterSveam::CustomDPCAuxilary *************
//=********************************************************
int		TInterSveam::CustomDPCAuxilary( PVOID pArg1, PVOID pArg2 )
{
	int			isBlockEdge = 0;
	UINT32		dpramHostPoint;
	UINT32		dpramDspPoint;
	UINT32		dpramDspOffset;
	UINT32		dpramDspCount;

	//
	// Compare HOST Pointer and DSP Pointer
	//
	DL_ReadDesc32( m_dpramOfsOfDspPoint, &dpramDspPoint );
	dpramDspOffset  = dpramDspPoint & ((1<<m_dpramPointShift)-1);
	dpramDspOffset &= ~0x3L;
	dpramDspCount   = dpramDspPoint >> m_dpramPointShift;
	if( m_dpramHostOffset==dpramDspOffset && m_dpramHostCount==dpramDspCount )
		return -1;

	//
	// Check Value of DSP Pointer
	//
	if( dpramDspOffset >= m_dpramBufSizeb )
	{
		m_isStart = 0;
		if( m_stub.sysAdr )
		{
			((TStreamStub*)m_stub.sysAdr)->state &= ~0xF;
			((TStreamStub*)m_stub.sysAdr)->state |= 0x24;
		}
	}

	//
	// Input/Output Data
	//
	if( m_dir==1 )
		DL_InputToHost( dpramDspOffset, dpramDspCount, &isBlockEdge );
	else
		DL_OutputFromHost( dpramDspOffset, dpramDspCount, &isBlockEdge );

	//
	// Store New Pointer
	//
	dpramHostPoint  = m_dpramHostOffset & ((1<<m_dpramPointShift)-1);
	dpramHostPoint |= m_dpramHostCount << m_dpramPointShift;

//	KdPrint((KDHEAD": ::CustomDPC() dspPnt=0x%X, hostPnt=0x%X, totCnt=%d, blk=%d\n", 
//		dpramDspPoint, dpramHostPoint, m_totalCnt, m_lastBlock ));

	DL_WriteDesc32( m_dpramOfsOfHostPoint, dpramHostPoint&(~0x3L) );

	//
	// Stub
	//
	if( m_stub.sysAdr )
	{
		((TStreamStub*)m_stub.sysAdr)->lastBlock    = m_lastBlock;
		((TStreamStub*)m_stub.sysAdr)->totalCounter = m_totalCnt;
		((TStreamStub*)m_stub.sysAdr)->offset = m_cbufBlkOffset;
	}
	
	//
	// Set Events
	//
	if( isBlockEdge )
	{
		SetAndRemoveAllEvents();
		KeSetEvent( &m_event, 0, FALSE );
	}
	return -1;
}

//=*********** TInterSveam::DL_InputToHost ****************
//=********************************************************
int		TInterSveam::DL_InputToHost( UINT32 dpramDspOffset, UINT32 dpramDspCount, int *pIsBlockEdge )
{
	UINT32		sizebOfTransfer;
	UINT32		restOfDspBuf;
	UINT32		restOfHostBlock;
	UINT32		sizeb;
	UINT32		isSendInterrupt = 0;
	
	sizebOfTransfer  = dpramDspCount-m_dpramHostCount;
	sizebOfTransfer &= (1 << (32-m_dpramPointShift)) - 1;
	sizebOfTransfer *= m_dpramBufSizeb;
	sizebOfTransfer += dpramDspOffset;
	sizebOfTransfer -= m_dpramHostOffset;

	//
	// Check if Block Edge will be crossed
	//
	{
		UINT32	restOfDspBlock;
		
		restOfDspBlock  = m_dpramBufSizeb - m_dpramHostOffset;
		restOfDspBlock %= m_dpramBlockSizeb;
		if( restOfDspBlock == 0 )
			restOfDspBlock = m_dpramBlockSizeb;

		if( sizebOfTransfer >= restOfDspBlock )
			isSendInterrupt = 1;
	}


	//
	// Check Lost of Data
	//
	if( sizebOfTransfer > m_dpramBufSizeb )
	{

		//
		// Process Overflow Event
		//
		m_flagOverflow = 1;
		if( m_stub.sysAdr )
		{
			((TStreamStub*)m_stub.sysAdr)->state |= 0x10;
		}
		if( m_cntOverflow != 0xFFFFFFFF )
		{
			m_cntOverflow--;
			if( m_cntOverflow==0 )
			{
				m_isStart = 0;
				if( m_stub.sysAdr )
				{
					((TStreamStub*)m_stub.sysAdr)->state &= ~0xF;
					((TStreamStub*)m_stub.sysAdr)->state |= 4;
				}
			}
		}

		//
		// Process Lost Data
		//

		sizebOfTransfer -= m_dpramBufSizeb;
		
		//
		// Calculate DSP DPRAM Offset
		//

		// dpramDspOffset isn't modified

		//
		// Calculate HOST CBUF Pointer
		//
		while(1)
		{
			//
			// Go to the End of the CBUF Block
			//
			restOfHostBlock = m_blkSize - m_cbufBlkOffset;
			sizeb = min( sizebOfTransfer, restOfHostBlock );
			sizebOfTransfer   -= sizeb;
			m_cbufBlkOffset   += sizeb;
			m_dpramHostOffset += sizeb;
			m_dpramHostCount  += m_dpramHostOffset/m_dpramBufSizeb;
			m_dpramHostOffset %= m_dpramBufSizeb;


			//
			// Check if HOST CBUF Pointer Crossed the end of Block
			//
			if( m_cbufBlkOffset >= m_blkSize )
			{
				m_cbufBlkOffset = 0;
				*pIsBlockEdge  = 1;
				m_totalCnt++;
				m_lastBlock++;
				if( m_lastBlock >= m_blkNum )
					m_lastBlock = 0;
				
				//
				// Stop if SingleCycle Mode
				//
				if( m_lastBlock == m_blkNum-1 )
				{
					if( m_isCycle==0 )
					{
						m_isStart = 0;
						break;
					}
				}
			}

			//
			// If End of Transfer
			//
			if( sizebOfTransfer == 0 )
				break;

			//
			// Go to the End of Transfer
			//
			{
				UINT32		tmpBlkCnt = sizebOfTransfer/m_blkSize;

				if( m_isCycle==0 )
				{
					if( tmpBlkCnt >= m_blkNum - m_lastBlock - 1 )
					{
						tmpBlkCnt = m_blkNum - m_lastBlock - 1;
						sizebOfTransfer = m_blkSize*tmpBlkCnt;
						m_isStart = 0;
					}
				}
				m_totalCnt += tmpBlkCnt;
				m_lastBlock+= tmpBlkCnt;
				m_lastBlock%= m_blkNum;
				m_cbufBlkOffset = sizebOfTransfer - m_blkSize*tmpBlkCnt;

				m_dpramHostOffset += sizebOfTransfer;
				m_dpramHostCount  += m_dpramHostOffset/m_dpramBufSizeb;
				m_dpramHostOffset %= m_dpramBufSizeb;
			}

			break;
		}

		//
		// Set sizebOfTransfer
		//
		sizebOfTransfer = m_dpramBufSizeb;
	}

	//
	// Move Data From DSP to HOST
	//
	if( m_isStart )
	while( sizebOfTransfer!=0 )
	{
		restOfDspBuf    = m_dpramBufSizeb - m_dpramHostOffset;
		restOfHostBlock = m_blkSize - m_cbufBlkOffset;

		//
		// Calculate Size of Portion
		//
		sizeb = min( sizebOfTransfer, restOfDspBuf );
		sizeb = min( sizeb, restOfHostBlock );

		//
		// DEBUG Info
		//
		//{
		//	UINT32		arrTmp[4];
		//
		//	DL_MoveDspToHost( m_bufAdr+m_dpramHostOffset, arrTmp, sizeof(arrTmp) );
		//	KdPrint((KDHEAD": :: [0x%X, 0x%X, 0x%X, 0x%X]\n", arrTmp[0], arrTmp[1], arrTmp[2], arrTmp[3] ));
		//}

		//
		// Move Portion of Data
		//
		KdPrint((KDHEAD": Move  %4X %4X %4X %4X %4X | ", 
						m_bufAdr, m_dpramHostOffset, (m_lastBlock+1)%m_blkNum, m_cbufBlkOffset, sizeb ));
		DL_MoveDspToHost( m_bufAdr+m_dpramHostOffset, &m_arrBlock[(m_lastBlock+1)%m_blkNum],
						  m_cbufBlkOffset, sizeb );
//		DL_MoveDspToHost( m_bufAdr+m_dpramHostOffset, 
//						  ((CHAR*)(m_arrBlock[(m_lastBlock+1)%m_blkNum].sysAdr))+m_cbufBlkOffset, 
//						  sizeb );
		//
		// DEBUG Info
		//
		{
			UINT32		*pTmp = (UINT32*)(((CHAR*)(m_arrBlock[(m_lastBlock+1)%m_blkNum].sysAdr))+m_cbufBlkOffset);
	
			KdPrint(("  0x%X\n", pTmp[0] ));
//			KdPrint((KDHEAD":     0x%X, 0x%X, 0x%X, 0x%X \n", pTmp[0], pTmp[1], pTmp[2], pTmp[3] ));
		}

		sizebOfTransfer   -= sizeb;
		m_dpramHostOffset += sizeb;
		m_cbufBlkOffset   += sizeb;

		//
		// Check if DSP DPRAM Pointer Crossed the end of Buffer
		//
		if( m_dpramHostOffset >= m_dpramBufSizeb )
		{
			m_dpramHostOffset = 0;
			m_dpramHostCount++;
		}

		//
		// Check if HOST CBUF Pointer Crossed the end of Block
		//
		if( m_cbufBlkOffset >= m_blkSize )
		{
			m_cbufBlkOffset = 0;
			*pIsBlockEdge  = 1;
			m_totalCnt++;
			m_lastBlock++;
			if( m_lastBlock >= m_blkNum )
				m_lastBlock = 0;
				
			//
			// Stop if SingleCycle Mode
			//
			if( m_lastBlock == m_blkNum-1 )
			{
				if( m_isCycle==0 )
				{
					m_isStart = 0;
					break;
				}
			}
		}
	}
	m_dpramHostOffset &= (1 << m_dpramPointShift) - 1;
	m_dpramHostCount  &= (1 << (32-m_dpramPointShift)) - 1;

	//
	// Send Interrupt if Need
	//
	{
		UINT32	deviceEvent = (m_dpramDesc1Code>>24) & 3;

		if( deviceEvent==1 && isSendInterrupt==1 )
			DL_SendInterrupt();
		if( deviceEvent==2 )
			DL_SendInterrupt();
	}

	return 0;
}

//=*********** TInterSveam::DL_OutputFromHost *************
//=********************************************************
int		TInterSveam::DL_OutputFromHost( UINT32 dpramDspOffset, UINT32 dpramDspCount, int *pIsBlockEdge )
{
	return 0;
}


//
// End of File
//

