//=********************************************************
// DpramBox.h - Classes TDpramBox Declaration
//
// (C) Copyright Ekkore 2002-2004
//
// Created: 
//              16.06.2004 by Ekkore
// Modified:
//
//=********************************************************

#ifndef		_DPRAMBOX_H_
#define		_DPRAMBOX_H_

#include	"captlist.h"

//
// Numbers of HOST_MAIN Block Registers
//
#    define		HREG_BLOCK_ID		0x00
#    define		HREG_BLOCK_VER		0x01
#    define		HREG_DEVICE_ID		0x02
#    define		HREG_REV_MOD		0x03
#    define		HREG_PLD_VER		0x04
#    define		HREG_BLOCK_CNT		0x05
#    define		HREG_DPRAM_OFFSET	0x06
#    define		HREG_DPRAM_SIZE		0x07
#    define		HREG_BRD_MODE		0x08

#    define		HREG_SEM0			0x13

#    define		SEMID0				0
#    define		SEMOWN				1
#    define		SEMFREE				0

//=******************* class TDpramBox ********************
//=********************************************************
typedef struct _TDpramBox
{
    //
    // Variables
    //
    void          *m_pciPlxAddress;              // PLX Port Area mapped address
    void          *m_pciMainBlockAddress;        // HOST_MAIN Block Memory Area mapped address
    u32            m_locMainBlockAddress;        // HOST_MAIN Block Offset on Local Bus

    TCaptureList  *m_pCaptList;                  // Capturer of DMA Resources

    void          *m_pDpram;                     // Beginnig of DPARAM mapped address
    u32            m_nDpramSizeb;                // Size of DPRAM (bytes)

} TDpramBox;

//
// Methods
//
TDpramBox     *TDpramBoxCreate ( TCaptureList * pCaptList,
                                 void *pciPlxAddress,
                                 void *pciMainBlockAddress,
                                 u32 locMainBlockAddress );

void           TDpramBoxDelete ( TDpramBox * dpr );

void           TDpramBoxInit ( TDpramBox * dpr, TCaptureList * pCaptList,
                               void *pciPlxAddress,
                               void *pciMainBlockAddress,
                               u32 locMainBlockAddress );

int            TDpramBoxCloseHandle ( TDpramBox * box );
int            TDpramBoxRemoveDevice ( TDpramBox * box );
void           DefineDpramArea ( TDpramBox * box );
int            ReadWriteDpram ( TDpramBox * box, u32 nDpramOffset, void *pvBuf,
                                u32 nSizeb, int isRead );

void           SemSet ( TDpramBox * dpr, u32 nSemId, int isOwn );
int            SemCheck ( TDpramBox * dpr, u32 nSemId );
void           SemWait ( TDpramBox * dpr, u32 nSemId );

int            TDpramBoxCaptureDma ( TDpramBox * dpr, u32 * pnDmaId,
                                     int *pPassword );
void           TDpramBoxReleaseDma ( TDpramBox * dpr, u32 nDmaId,
                                     int password );

//
// HOST_MAIN Block Acesses
//
u32            ReadMainBlock32 ( TDpramBox * box, u32 regNum );
void           WriteMainBlock32 ( TDpramBox * box, u32 regNum, u32 value );


#endif //_DPRAMBOX_H_

//
// End of File
//
