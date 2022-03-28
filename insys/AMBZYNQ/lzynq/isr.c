//****************** File isr.cpp ***********************************
//  interrupt service routine and DPC
//
//	Copyright (c) 2007, Instrumental Systems,Corp.
//  Written by Dorokhin Andrey
//
//  History:
//  20-03-07 - builded
//
//*******************************************************************

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/wait.h>
#include <linux/interrupt.h>

#include "ddzynq.h"
#include "zynqdev.h"
#include "wambpmc.h"

//-----------------------------------------------------------------------------

static irqreturn_t dma_isr(int channel, void *pContext)
{
    unsigned long flags = 0;
    struct CDmaChannel *dma = 0;
    struct CWambpex* pDevice = (struct CWambpex*)pContext;            // our device
    if(!pDevice) {
        err_msg(err_trace, "%s(): Invalid device pointer\n", __FUNCTION__ );
        return IRQ_NONE;
    }

    dbg_msg(dbg_trace, "%s(%d)\n", __func__, channel);

    dma = pDevice->m_DmaChannel[channel];
    if(!dma) {
        err_msg(err_trace, "%s(): Invalid device pointer\n", __FUNCTION__ );
        return IRQ_NONE;
    }

    spin_lock_irqsave(&dma->m_DmaLock, flags);

    if(!pDevice->m_DmaIrqEnbl) {
        spin_unlock_irqrestore(&dma->m_DmaLock, flags);
        return IRQ_NONE;
    }

    atomic_inc(&pDevice->m_TotalIRQ);

    if(pDevice->m_DmaChanEnbl[channel])
    {
        FIFO_STATUS FifoStatus;
        u32 flag = 0;
        u32 FifoAddr = pDevice->m_FifoAddr[channel];

//<<<<<<< Updated upstream
        FifoStatus.AsWhole = ReadOperationReg(pDevice, PEFIFOadr_FIFO_STATUS + FifoAddr);
//=======
//        FifoStatus.AsWhole = ReadOperationReg(pDevice, PEFIFOadr_FIFO_STATUS + FifoAddr);
//        dbg_msg(dbg_trace, "%s(%d): A - Channel = %d, Fifo Status = 0x%X\n", __FUNCTION__, atomic_read(&pDevice->m_TotalIRQ), channel, FifoStatus.AsWhole);
//        FifoStatus.ByBits.IntRql = 1;
//>>>>>>> Stashed changes
        if(FifoStatus.ByBits.IntRql)
        {
            dbg_msg(dbg_trace, "%s(%d): Channel = %d, Fifo Status = 0x%X\n", __FUNCTION__, atomic_read(&pDevice->m_TotalIRQ), channel, FifoStatus.AsWhole);

            flag = NextDmaTransfer(dma);
            if(!flag)
            {
                DMA_CTRL_EXT CtrlExt;

                dbg_msg(dbg_trace, "%s(%d): Pause\n", __FUNCTION__, atomic_read(&pDevice->m_TotalIRQ));

                CtrlExt.AsWhole = 0;
                CtrlExt.ByBits.Pause = 1;
                CtrlExt.ByBits.Start = 1;

                WriteOperationReg(pDevice, PEFIFOadr_DMA_CTRL + FifoAddr, CtrlExt.AsWhole);
            }

            dbg_msg(dbg_trace, "%s(%d): Flag Clear\n", __FUNCTION__, atomic_read(&pDevice->m_TotalIRQ));

            WriteOperationReg(pDevice, PEFIFOadr_FLAG_CLR + FifoAddr, 0x10);
            WriteOperationReg(pDevice, PEFIFOadr_FLAG_CLR + FifoAddr, 0x00);

            atomic_inc(&pDevice->m_DmaIRQ[channel]);

            spin_unlock_irqrestore(&dma->m_DmaLock, flags);

            return IRQ_HANDLED;
        }
    }

    dbg_msg(dbg_trace, "%s(%d): Something else...\n", __FUNCTION__, atomic_read(&pDevice->m_TotalIRQ));

    atomic_inc(&pDevice->m_SpuriousIRQ);

    spin_unlock_irqrestore(&dma->m_DmaLock, flags);

    return IRQ_NONE;	// we did not interrupt
}

//-----------------------------------------------------------------------------

irqreturn_t dma0_isr( int irq, void *pContext)
{
    return dma_isr(0, pContext);
}

//-----------------------------------------------------------------------------

irqreturn_t dma1_isr( int irq, void *pContext)
{
    return dma_isr(1, pContext);
}

//-----------------------------------------------------------------------------

irqreturn_t WambpexIsr( int irq, void *pContext )
{
    //unsigned long flags = 0;
    struct CWambpex* pDevice = (struct CWambpex*) pContext;            // our device
    if(!pDevice) {
        err_msg(err_trace, "%s(): Invalid device pointer\n", __FUNCTION__ );
        return IRQ_NONE;
    }

    //spin_lock_irqsave(&pDevice->m_IsrLock[3], flags);

    if(!pDevice->m_FlgIrqEnbl) {
        //spin_unlock_irqrestore(&pDevice->m_IsrLock[3], flags);
        return IRQ_NONE;
    }

    atomic_inc(&pDevice->m_TotalIRQ);

    if(pDevice->m_FlgIrqEnbl)
    {
        int i = 0;
        u32 status = ReadOperationReg(pDevice, PEMAINadr_BRD_STATUS);

        dbg_msg(dbg_trace, "%s(): BRD_STATUS = 0x%X.\n", __FUNCTION__, status);

        if(status & 0x4000)
        {
            for(i = 0; i < NUM_TETR_IRQ; i++) {

                if(pDevice->m_TetrIrq[i].EventId)
                {
                    u32 status = ReadAmbMainReg(pDevice, pDevice->m_TetrIrq[i].Address);
                    dbg_msg(dbg_trace, "%s(): TetrIrq = %d, Address = 0x%X, IrqInv = 0x%X, IrqMask = 0x%X, Status = 0x%X.\n",
                                __FUNCTION__, i, pDevice->m_TetrIrq[i].Address, pDevice->m_TetrIrq[i].IrqInv, pDevice->m_TetrIrq[i].IrqMask, status);
                    status ^= pDevice->m_TetrIrq[i].IrqInv;
                    status &= pDevice->m_TetrIrq[i].IrqMask;
                    dbg_msg(dbg_trace, "%s(): TetrIrq = %d, Address = 0x%X, IrqInv = 0x%X, IrqMask = 0x%X, Status = 0x%X.\n",
                                __FUNCTION__, i, pDevice->m_TetrIrq[i].Address, pDevice->m_TetrIrq[i].IrqInv, pDevice->m_TetrIrq[i].IrqMask, status);
                    if(status)
                    {
                        u32 CmdAddress = 0;
                        u32 DataAddress = 0;
                        u32 Mode0Value = 0;

                        tasklet_hi_schedule(&pDevice->m_TetrIrq[i].Dpc);

                        dbg_msg(dbg_trace, "%s(): Tetrad IRQ address = 0x%x\n", __FUNCTION__, pDevice->m_TetrIrq[i].Address);

                        CmdAddress = pDevice->m_TetrIrq[i].Address + TRDadr_CMD_ADR * REG_SIZE;
                        WriteAmbMainReg(pDevice, CmdAddress, 0);
                        DataAddress = pDevice->m_TetrIrq[i].Address + TRDadr_CMD_DATA * REG_SIZE;
                        Mode0Value = ReadAmbMainReg(pDevice, DataAddress);
                        Mode0Value &= 0xFFFB;

                        WriteAmbMainReg(pDevice, DataAddress, Mode0Value);

                        atomic_inc(&pDevice->m_FlgIRQ[i]);

                        break;
                    }
                }
            }

            //spin_unlock_irqrestore(&pDevice->m_IsrLock[3], flags);

            return IRQ_HANDLED;

        } else {

            dbg_msg(dbg_trace, "%s(): Something in the way...\n", __FUNCTION__ );
            atomic_inc(&pDevice->m_SpuriousIRQ);
            //spin_unlock_irqrestore(&pDevice->m_IsrLock[3], flags);

            return IRQ_NONE;	// we did not interrupt
        }
    }

    dbg_msg(dbg_trace, "%s(%d): Something else...\n", __FUNCTION__, atomic_read(&pDevice->m_TotalIRQ));
    atomic_inc(&pDevice->m_SpuriousIRQ);

    //spin_unlock_irqrestore(&pDevice->m_IsrLock[3], flags);

    return IRQ_NONE;	// we did not interrupt
}

//-----------------------------------------------------------------------------

void WambpexDpcForIsr( unsigned long Context )
{
    struct CDmaChannel *DmaChannel = (struct CDmaChannel *)Context;

    dbg_msg(dbg_trace, "%s(): [DMA%d] m_CurBlockNum = %d, m_BlockCount = %d\n",
		__FUNCTION__, DmaChannel->m_NumberOfChannel, DmaChannel->m_CurBlockNum, DmaChannel->m_BlockCount );

    if(DmaChannel->m_CurBlockNum >= DmaChannel->m_BlockCount)
    {
        HwCompleteDmaTransfer(DmaChannel->m_Board,DmaChannel->m_NumberOfChannel);
		SetEvent( &DmaChannel->m_BufferEndEvent );
    }

    DmaChannel->m_State = *DmaChannel->m_pStub;

    SetEvent( &DmaChannel->m_BlockEndEvent );
}

//-----------------------------------------------------------------------------

void TetrDpcForIsr(unsigned long Context)
{
    TETRAD_IRQ *pTrdIrq = (TETRAD_IRQ*)Context;
    if(!pTrdIrq) {
        err_msg(err_trace, "%s(): Invalid context pointer\n", __FUNCTION__ );
        return;
    }

    if(pTrdIrq->EventId) {

        SetEvent(&pTrdIrq->Event);
        dbg_msg(dbg_trace, "%s(): Set event for address = 0x%x\n", __func__, pTrdIrq->Address);
    }
}

//-----------------------------------------------------------------------------
