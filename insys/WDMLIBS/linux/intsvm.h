//=********************************************************
// IntSvm.h - Class TInterSveam
//
// (C) Copyright Ekkore 2002
//
// Created: 
//		21.10.2003 by Ekkore
// Modified:
//
//=********************************************************

#ifndef		_INTSVM_H_
#define		_INTSVM_H_

#include	"IntStrm.h"

#define		SVEAM_MAX	16


//=******************* class TInterSveam ******************
//=********************************************************

struct TInterSveam
{
	//
	// Variables
	//
	u32		m_id;				// Number of Sweam (0-15)

	//
	//  Locations of Descriptor and Buffer (in DSP IRAM/DPRAM Area)
	//
	u32		m_descAdr;			// Address/Offset of DSP Descriptor
	u32		m_isDescInDpram;		// Location of DSP Descriptor (1-DPRAM, 0-IRAM)
	u32		m_bufAdr;			// Address/Offset of DSP Buffer
	u32		m_isBufInDpram;			// Location of DSP Buffer (1-DPRAM, 0-IRAM)

	//
	//  DSP DPRAM Data
	//
	u32		m_dpramBlockNum;		// DPRAM Number of Blocks
	u32		m_dpramBlockSizeb;		// DPRAM Block Size (bytes)
	u32		m_dpramBufSizeb;		// DPRAM Complex Buffer Size (bytes)
	u32		m_dpramPointShift;		// DPRAM Pointer Shift to Get Pointer Fields
	u32		m_dpramOfsOfDspPoint;		// DPRAM Offset of DSP Pointer (bytes)
	u32		m_dpramOfsOfHostPoint;		// DPRAM Offset of HOST Pointer (bytes)
	u32		m_dpramOfsOfBuffer;		// DPRAM Offset of DSP Pointer (bytes)
	u32		m_dpramDesc1Code;		// Code of 2nd word of Descriptor


	u32		m_dpramHostOffset;		// Value of HOST Pointer Field "Offset"
	u32		m_dpramHostCount;		// Value of HOST Pointer Field "Buffer Counter"

	//
	// HOST CBUF Data
	//
	u32		m_cbufBlkOffset;		// Offset of Current CBUF Block

	//
	// Status
	//
	u32		m_flagOverflow;			// Overflow Flag
	u32		m_cntOverflow;			// Overflow Counter

	//
	// Other
	//
	u32		m_isCycle;			// 1-Cycle Mode
	u32		m_dir;				// Direction: 1-to HOST, 2-from HOST
	int		m_isStart;			// 1-Start, 0-Stop

	struct sveam_operations *m_op;
	
	//TInterSveam( void );
	//~TInterSveam();	

};

struct sveam_operations {

	void	(*Init)( struct TInterSveam *svm, u32 id );	
	int	(*Unlock)( struct TInterSveam *svm );  
	int	(*Disalloc)( struct TInterSveam *svm );  
	int	(*CloseHandle)( struct TInterSveam *svm );
	
		//dir=1 (to HOST), dir=2 (from HODST) 
	int	(*Start)( IN PFILE_OBJECT pFileObject, u32 isCycle, u32 dir, u32 descAdr, u32 isDescInDpram);
	int	(*Start)( IN PFILE_OBJECT pFileObject, u32 isCycle, u32 dir );
	
	int	(*Stop)( IN PFILE_OBJECT pFileObject ); 
	int	(*IsStart)( void ) { return m_isStart; }; 
	

	//
	// ISR, DPC for ISR (DISPATCH LEVEL)
	//
	int	(*Isr)( void )		{ if(m_isStart) QueueDPC(); return (m_isStart)?TRUE:FALSE; };
	void	(*QueueDPC)()		{ KeInsertQueueDpc( &m_dpc, NULL, NULL ); };
	int	(*CustomDPCAuxilary)( PVOID pArg1, PVOID pArg2 );

	int	(*DL_InputToHost)( u32 dpramDspOffset, u32 dpramDspCount, int *pIsBlockEdge );
	int	(*DL_OutputFromHost)( u32 dpramDspOffset, u32 dpramDspCount, int *pIsBlockEdge );


	//
	// Abstuct Virtual Funx (DISPATCH LEVEL)
	//
	int	(*PL_ReadDesc32)( u32 offset, u32 *pValue )			{return -1;};
	int	(*DL_ReadDesc32)( u32 offset, u32 *pValue )			{return -1;};
	int	(*DL_WriteDesc32)( u32 offset, u32 value )				{return -1;};
	int	(*DL_MoveDspToHost)( u32 offset, void *hostAdr, u32 bytes )	{return -1;};
	int	(*DL_MoveHostToDsp)( u32 offset, void *hostAdr, u32 bytes )	{return -1;};
	int	(*DL_SendInterrupt)( void )					{return -1;};
	int	(*DL_MoveDspToHost)( u32 cramOffset, TBlockAdrInfo *pBlockAdrInfo, u32 hostOffset, u32 sizeb )	{return -1;};
};



#endif		//_INTSVM_H_

//
// End of File
//

