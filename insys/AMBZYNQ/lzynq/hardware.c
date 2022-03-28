//****************** File Hardware.cpp *******************************
//  class CWambp - functions to access hardware
//
//	Copyright (c) 2004, Instrumental Systems,Corp.
//  Written by Dorokhin Andrey
//
//  History:
//  05-10-04 - builded
//
//-----------------------------------------------------------------------------

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

int InitializeBoard(struct CWambpex *brd)
{
    u16 temp = 0;  // holds registers while we are modifying bits
    u16 blockId = 0;
    u16 blockVer = 0;
    int iChan = 0;
    int iBlock = 0;
    int i = 0;
    FIFO_ID FifoId;
    u32 brd_num;

    brd->m_VendorID = RegPeekInd(brd, 0, 0x108);
    if(brd->m_VendorID != INSYS_VENDOR_ID) {
        printk(KERN_ALERT"%s(): Invalid vendor ID value: 0x%X\n", __FUNCTION__, brd->m_VendorID);
        return -ENODEV;
    }

    brd->m_DeviceID = ReadOperationReg(brd, PEMAINadr_DEVICE_ID);
    brd->m_RevisionID = ReadOperationReg(brd, PEMAINadr_DEVICE_REV);

    dbg_msg(dbg_trace, "%s(): DeviceID = 0x%X, DeviceRev = 0x%X.\n", __FUNCTION__, brd->m_DeviceID, brd->m_RevisionID);

    if(!device_match(brd->m_DeviceID)) {
        printk(KERN_ALERT"%s(): Error in device_match() - DeviceID = 0x%X, DeviceRev = 0x%X.\n", __FUNCTION__, brd->m_DeviceID, brd->m_RevisionID);
        return -ENODEV;
    }

    device_name(brd, brd->m_DeviceID, brd->m_BoardIndex);

    if(!request_mem_region(brd->m_OperationRegisters.PhysicalAddress, brd->m_OperationRegisters.Length, brd->m_name)) {
        err_msg(err_trace, "%s(): request_mem_region(BAR0) was failed.\n", __FUNCTION__);
        return -EBUSY;
    }

    if(!request_mem_region(brd->m_AmbMainRegisters.PhysicalAddress, brd->m_AmbMainRegisters.Length, brd->m_name)) {
        err_msg(err_trace, "%s(): request_mem_region(BAR1) was failed.\n", __FUNCTION__);
        return -EBUSY;
    }

    blockId = ReadOperationReg(brd, PEMAINadr_BLOCK_ID);
    blockVer = ReadOperationReg(brd, PEMAINadr_BLOCK_VER);

    dbg_msg(dbg_trace, "%s(): BlockID = 0x%X, BlockVER = 0x%X.\n", __FUNCTION__, blockId, blockVer);

    temp = ReadOperationReg(brd, PEMAINadr_PLD_VER);

    dbg_msg(dbg_trace, "%s(): PldVER = 0x%X.\n", __FUNCTION__, temp);

    brd->m_BlockCnt = ReadOperationReg(brd, PEMAINadr_BLOCK_CNT);

    dbg_msg(dbg_trace, "%s(): Block count = %d.\n", __FUNCTION__, brd->m_BlockCnt);

	// начальное обнуление информации о каналах ПДП
	brd->m_DmaChanMask = 0;
	for(iChan = 0; iChan < MAX_NUMBER_OF_DMACHANNELS; iChan++)
	{
		brd->m_BlockFifoId[iChan] = 0;
		brd->m_DmaFifoSize[iChan] = 0;
		brd->m_DmaDir[iChan] = 0;
		brd->m_MaxDmaSize[iChan] = 0;
        brd->m_DmaChanEnbl[iChan] = 0;
	}

	// определим какие каналы ПДП присутствуют и их характеристики:
	// направление передачи данных, размер FIFO, максимальный размер блока ПДП
	for(iBlock = 1; iBlock < brd->m_BlockCnt; iBlock++)
	{
		u32 FifoAddr = 0;
		u16 block_id = 0;
        FifoAddr = iBlock * PE_BLK_SIZE;
        temp = ReadOperationReg(brd, PEFIFOadr_BLOCK_ID + FifoAddr);
		block_id = (temp & 0x0FFF);
		if(block_id == PE_FIFO_ID)
		{
			u64 one = 0;
			u64 maxdmasize = 0;
            u16 iChan = ReadOperationReg(brd, PEFIFOadr_FIFO_NUM + FifoAddr);
			brd->m_FifoAddr[iChan] = FifoAddr;
			brd->m_BlockFifoId[iChan] = block_id;
			brd->m_DmaChanMask |= (1 << iChan);
            FifoId.AsWhole = ReadOperationReg(brd, PEFIFOadr_FIFO_ID + FifoAddr);
			brd->m_DmaFifoSize[iChan] = FifoId.ByBits.Size;
			brd->m_DmaDir[iChan] = FifoId.ByBits.Dir;
            temp = ReadOperationReg(brd, PEFIFOadr_DMA_SIZE + FifoAddr);

			one = 1;
			maxdmasize = one << temp;
			// если макс. размер ПДП может быть больше или равен 4 Гбайт, то снижаем его до 1 Гбайта
			if(temp >= 32)
				brd->m_MaxDmaSize[iChan] = 0x40000000;
			else
				brd->m_MaxDmaSize[iChan] = (u32)maxdmasize;

            dbg_msg(dbg_trace, "%s(): Channel(ID) = %d(0x%x), FIFO size = %d Bytes, DMA Dir = %d,\n", __FUNCTION__,
								iChan, block_id, brd->m_DmaFifoSize[iChan] * 4, brd->m_DmaDir[iChan]);
            dbg_msg(dbg_trace, "%s(): Max DMA size (hard) = %d MBytes,  Max DMA size (soft) = %d MBytes.\n", __FUNCTION__,
								(u32)(maxdmasize / 1024 / 1024), brd->m_MaxDmaSize[iChan] / 1024 / 1024);
		}
		if(block_id == PE_EXT_FIFO_ID)
		{
			u32 resource_id = 0;
            u16 iChan = ReadOperationReg(brd, PEFIFOadr_FIFO_NUM + FifoAddr);
			brd->m_FifoAddr[iChan] = FifoAddr;
			brd->m_BlockFifoId[iChan] = block_id;
			brd->m_DmaChanMask |= (1 << iChan);
            FifoId.AsWhole = ReadOperationReg(brd, PEFIFOadr_FIFO_ID + FifoAddr);
			brd->m_DmaFifoSize[iChan] = FifoId.ByBits.Size;
			brd->m_DmaDir[iChan] = FifoId.ByBits.Dir;
			brd->m_MaxDmaSize[iChan] = 0x40000000; // макс. размер ПДП пусть будет 1 Гбайт
            resource_id = ReadOperationReg(brd, PEFIFOadr_DMA_SIZE + FifoAddr); // RESOURCE
            dbg_msg(dbg_trace, "%s(): Channel(ID) = %d(0x%x), FIFO size = %d Bytes, DMA Dir = %d, Max DMA size = %d MBytes, resource = 0x%x.\n", __FUNCTION__,
								iChan, block_id, brd->m_DmaFifoSize[iChan] * 4, brd->m_DmaDir[iChan], brd->m_MaxDmaSize[iChan] / 1024 / 1024, resource_id);
		}
	}

	// подготовим к работе ПЛИС ADM
    dbg_msg(dbg_trace, "%s(): Prepare ADM PLD.\n", __FUNCTION__);
    WriteOperationReg(brd,PEMAINadr_BRD_MODE, 0);
        ToPause(100);	// pause ~ 100 msec
	for(i = 0; i < 10; i++)
	{
        WriteOperationReg(brd, PEMAINadr_BRD_MODE, 1);
                ToPause(100);	// pause ~ 100 msec
        WriteOperationReg(brd, PEMAINadr_BRD_MODE, 3);
                ToPause(100);	// pause ~ 100 msec
        WriteOperationReg(brd, PEMAINadr_BRD_MODE, 7);
                ToPause(100);	// pause ~ 100 msec
        temp = ReadOperationReg(brd, PEMAINadr_BRD_STATUS) & 0x01;
		if(temp)
			break;
	}
    WriteOperationReg(brd, PEMAINadr_BRD_MODE, 0x0F);
        ToPause(100);	// pause ~ 100 msec

	if(temp)
	{
		u32 idx = 0;
		BRD_STATUS brd_status;
        dbg_msg(dbg_trace, "%s(): ADM PLD is captured.\n", __FUNCTION__);
        brd_status.AsWhole = ReadOperationReg(brd, PEMAINadr_BRD_STATUS);
        if( AMBPEX8_DEVID == brd->m_DeviceID)
            brd_status.ByBits.InFlags &= 0x80; // 1 - ADM PLD in test mode (AMBPEX8 only)
        else
            brd_status.ByBits.InFlags = 0x80;  // Set flag manually to disable errors
		if(brd_status.ByBits.InFlags)
		{
			BRD_MODE brd_mode;
            dbg_msg(dbg_trace, "%s(): ADM PLD in test mode.\n", __FUNCTION__);

			// проверка линий передачи флагов
            brd_mode.AsWhole = ReadOperationReg(brd, PEMAINadr_BRD_MODE);
			for(idx = 0; idx < 4; idx++)
			{
				brd_mode.ByBits.OutFlags = idx;
                WriteOperationReg(brd, PEMAINadr_BRD_MODE, brd_mode.AsWhole);
                                ToPause(10);
                brd_status.AsWhole = ReadOperationReg(brd, PEMAINadr_BRD_STATUS);
				brd_status.ByBits.InFlags &= 0x03;
				if(brd_mode.ByBits.OutFlags != brd_status.ByBits.InFlags)
				{
					temp = 0;
                    dbg_msg(dbg_trace, "%s(): FLG_IN (%d) NOT equ FLG_OUT (%d).\n", __FUNCTION__,
							brd_status.ByBits.InFlags, brd_mode.ByBits.OutFlags);
					break;
				}
			}
			if(temp)
                dbg_msg(dbg_trace, "%s(): FLG_IN equ FLG_OUT.\n", __FUNCTION__);
		}
		else
			temp = 0;
	}

	if(!temp)
	{
        WriteOperationReg(brd, PEMAINadr_BRD_MODE, 0);
        ToPause(100);	// pause ~ 100 msec
	}


    if(FMC130E_DEVID == brd->m_DeviceID || FMC138M_DEVID == brd->m_DeviceID)
        brd->m_PldStatus[0] = 1;
    else
        brd->m_PldStatus[0] = temp; // состояние ПЛИС ADM: 0 - не готова

    dbg_msg(dbg_trace, "%s(): ADM PLD[%d] status = 0x%X.\n", __FUNCTION__, i, temp);

	{
        int iBlock = 1;
		BRD_MODE brd_mode;
        brd_mode.AsWhole = ReadOperationReg(brd, PEMAINadr_BRD_MODE);
		brd_mode.ByBits.OutFlags = 0;
        WriteOperationReg(brd, PEMAINadr_BRD_MODE, brd_mode.AsWhole);
        dbg_msg(dbg_trace, "%s(): BRD_MODE = 0x%X.\n", __FUNCTION__, brd_mode.AsWhole);

        WriteOperationReg(brd, PEMAINadr_IRQ_MASK, 0x4000);

        brd->m_BlockFidAddr = 0;
        for(iBlock = 1; iBlock < brd->m_BlockCnt; iBlock++) {
            u16 block_id = 0;
            u32 FidAddr = iBlock * PE_BLK_SIZE;
            temp = ReadOperationReg(brd, PEFIDadr_BLOCK_ID + FidAddr);
            block_id = (temp & 0x0FFF);
            if(block_id == PE_FID_ID) {
                brd->m_BlockFidAddr = FidAddr;
                dbg_msg(dbg_trace, "%s(): BLOCK FID number = %d.\n", __FUNCTION__, iBlock);
            }
        }

        if(	(FMC105P_DEVID == brd->m_DeviceID ||
            FMC123E_DEVID == brd->m_DeviceID ||
            FMC121CP_DEVID== brd->m_DeviceID ||
            FMC122P_DEVID== brd->m_DeviceID ||
            FMC124P_DEVID== brd->m_DeviceID ||
            FMC125CP_DEVID== brd->m_DeviceID ||
            FMC106P_DEVID == brd->m_DeviceID ||
            FMC107P_DEVID == brd->m_DeviceID ||
            FMC103E_DEVID == brd->m_DeviceID ||
            FMC110P_DEVID == brd->m_DeviceID ||
            FMC114V_DEVID == brd->m_DeviceID ||
            FMC113E_DEVID == brd->m_DeviceID ||
            FMC108V_DEVID == brd->m_DeviceID ||
            FMC112CP_DEVID == brd->m_DeviceID ||
            FMC115CP_DEVID == brd->m_DeviceID ||
            FMC122PSL_DEVID == brd->m_DeviceID ||
            PANORAMA_DEVID == brd->m_DeviceID ||
            FMC117CP_DEVID == brd->m_DeviceID) && !brd->m_BlockFidAddr) {

            dbg_msg(dbg_trace, "%s(): FMC board have no FID block.\n", __FUNCTION__);
            return -ENODEV;
        }
    }

    brd_num = ReadOperationReg(brd, PEMAINadr_BOARD_NUM);
    dbg_msg(dbg_trace, "%s(): BRD_NUM = 0x%X.\n", __FUNCTION__, brd_num);
	if((brd_num >> 16) == 0x4912)
	{
		brd->m_GeographicAddr = (brd_num >> 8) & 0x7F;
		brd->m_FpgaOrderNum = brd_num & 0xFF;
	    dbg_msg(dbg_trace, "%s(): geographic addres = %d, Order number of PLD = %d.\n", __FUNCTION__, brd->m_GeographicAddr, brd->m_FpgaOrderNum);
	}
	else
	{
		brd->m_GeographicAddr = 0xFFFFFFFF;
		brd->m_FpgaOrderNum = 0xFFFFFFFF;
	}

    // создадим структуры описывающие каналы DMA
    for(i = 0; i < MAX_NUMBER_OF_DMACHANNELS; i++)
    {
            if(brd->m_DmaChanMask & (1 << i))
            {
                brd->m_DmaChannel[i] = CDmaChannelCreate(i, WambpexDpcForIsr, brd, brd->m_dev, brd->m_MaxDmaSize[i], brd->m_BlockFifoId[i], 1);
                if(!brd->m_DmaChannel[i])
                {
                    UninitializeBoard(brd);
                    return -EINVAL;
                }
            }

            atomic_set(&brd->m_DmaIRQ[i], 0);
    }

    // создадим структуры описывающие прервания от тетрад
    for(i = 0; i < NUM_TETR_IRQ; i++)
    {
        atomic_set(&brd->m_FlgIRQ[i], 0);
        memset(&brd->m_TetrIrq[i], 0, sizeof(TETRAD_IRQ));
        InitKevent(&brd->m_TetrIrq[i].Event);
    }

    // Разрешить прерывания PL -> PS [IRQ_F2P]
    WriteOperationReg(brd, PEMAINadr_IRQ_INV, 0x3);

	return 0;
}

//-----------------------------------------------------------------------------

void UninitializeBoard(struct CWambpex *brd)
{
    int i=0;
    for(i = 0; i < MAX_NUMBER_OF_DMACHANNELS; i++) {
            if(brd->m_DmaChannel[i]) {
                 CDmaChannelDelete(brd->m_DmaChannel[i]);
            }
    }

    return;
}

//-----------------------------------------------------------------------------

int SetDmaMode(struct CWambpex *brd, u32 NumberOfChannel, u32 AdmNumber, u32 TetrNumber)
{
	int Status = -EINVAL;
	MAIN_SELX sel_reg;
	Status = ReadRegData(brd, AdmNumber, 0, 16 + NumberOfChannel, &sel_reg.AsWhole);
	sel_reg.ByBits.DmaTetr = TetrNumber;
	sel_reg.ByBits.DrqEnbl = 1;
	Status = WriteRegData(brd, AdmNumber, 0, 16 + NumberOfChannel, sel_reg.AsWhole);
    //dbg_msg(dbg_trace, "%s(): channel = %d, SELX = 0x%X.\n", __FUNCTION__,NumberOfChannel, sel_reg.AsWhole);
	return Status;
}

//-----------------------------------------------------------------------------
int SetDrqFlag(struct CWambpex *brd, u32 AdmNumber, u32 TetrNumber, u32 DrqFlag)
{
	int Status = 0;
	u32 Value;
	Status = ReadRegData(brd, AdmNumber, TetrNumber, 0, &Value);
	if(Status != 0) return Status;
	Value |= (DrqFlag << 12);
	Status = WriteRegData(brd, AdmNumber, TetrNumber, 0, Value);
    //dbg_msg(dbg_trace, "%s: SetDrqFlag: MODE0 = 0x%X.\n", __FUNCTION__, Value);
	return Status;
}

//-----------------------------------------------------------------------------
int DmaEnable(struct CWambpex *brd, u32 AdmNumber, u32 TetrNumber)
{
	int Status = 0;
	u32 Value;
	Status = ReadRegData(brd, AdmNumber, TetrNumber, 0, &Value);
	if(Status != 0) return Status;
	Value |= 0x8; // DRQ enable
	Status = WriteRegData(brd, AdmNumber, TetrNumber, 0, Value);
    //dbg_msg(dbg_trace, "%s: MODE0 = 0x%X.\n", __FUNCTION__, Value);
	return Status;
}

//-----------------------------------------------------------------------------
int DmaDisable(struct CWambpex *brd, u32 AdmNumber, u32 TetrNumber)
{
	int Status = 0;
	u32 Value;
	Status = ReadRegData(brd, AdmNumber, TetrNumber, 0, &Value);
	if(Status != 0) return Status;
	Value &= 0xfff7; // DRQ disable
	Status = WriteRegData(brd, AdmNumber, TetrNumber, 0, Value);
    //dbg_msg(dbg_trace, "%s: DmaDisable: MODE0 = 0x%X.\n", __FUNCTION__, Value);
	return Status;
}

//-----------------------------------------------------------------------------
int ResetFifo(struct CWambpex *brd, u32 NumberOfChannel)
{
	int Status = 0;
	u32 FifoAddr = brd->m_FifoAddr[NumberOfChannel];

	if(brd->m_BlockFifoId[NumberOfChannel] == PE_FIFO_ID)
	{
		FIFO_CTRL FifoCtrl;
        FifoCtrl.AsWhole = ReadOperationReg(brd,PEFIFOadr_FIFO_CTRL + FifoAddr);
        dbg_msg(dbg_trace, "%s(): channel = %d, FIFO_CTRL = 0x%X.\n", __FUNCTION__, NumberOfChannel, FifoCtrl.AsWhole);
		FifoCtrl.ByBits.Reset = 1;
        WriteOperationReg(brd,PEFIFOadr_FIFO_CTRL + FifoAddr, FifoCtrl.AsWhole);
                ToTimeOut(1000);
        dbg_msg(dbg_trace, "%s(): channel = %d, FIFO_CTRL = 0x%X.\n", __FUNCTION__, NumberOfChannel, FifoCtrl.AsWhole);
		FifoCtrl.ByBits.Reset = 0;
        WriteOperationReg(brd,PEFIFOadr_FIFO_CTRL + FifoAddr, FifoCtrl.AsWhole);
                ToTimeOut(1000);
        dbg_msg(dbg_trace, "%s(): channel = %d, FIFO_CTRL = 0x%X.\n", __FUNCTION__, NumberOfChannel, FifoCtrl.AsWhole);
	}
	if(brd->m_BlockFifoId[NumberOfChannel] == PE_EXT_FIFO_ID)
	{
		DMA_CTRL_EXT CtrlExt;
        CtrlExt.AsWhole = 0;//ReadOperationReg(PEFIFOadr_DMA_CTRL + FifoAddr);
        WriteOperationReg(brd,PEFIFOadr_DMA_CTRL + FifoAddr, CtrlExt.AsWhole);
                ToTimeOut(1);
        //dbg_msg(dbg_trace, "%s(): channel = %d, DMA_CTRL_EXT = 0x%X.\n", NumberOfChannel, CtrlExt.AsWhole);
		CtrlExt.ByBits.ResetFIFO = 1;
        WriteOperationReg(brd,PEFIFOadr_DMA_CTRL + FifoAddr, CtrlExt.AsWhole);
                ToTimeOut(1);
        //dbg_msg(dbg_trace, "%s(): channel = %d, DMA_CTRL_EXT = 0x%X.\n", __FUNCTION__, NumberOfChannel, CtrlExt.AsWhole);
		CtrlExt.ByBits.ResetFIFO = 0;
        WriteOperationReg(brd,PEFIFOadr_DMA_CTRL + FifoAddr, CtrlExt.AsWhole);
		//ToPause(200);
                ToTimeOut(1);
        //dbg_msg(dbg_trace, "%s(): channel = %d, DMA_CTRL_EXT = 0x%X.\n", __FUNCTION__, NumberOfChannel, CtrlExt.AsWhole);
	}
	return Status;
}

//-----------------------------------------------------------------------------
int SetIrqTable(struct CWambpex *brd, u32 NumberOfChannel, u32 Mode, u32 TableNum, u32* AddrTable)
{
	int Status = 0;
	u32 FifoAddr = brd->m_FifoAddr[NumberOfChannel];

	IRQ_EXT_MODE IrqMode;
	IrqMode.AsWhole = 0;
	IrqMode.AsWhole = ReadOperationReg(brd,PEFIFOadr_IRQ_MODE + FifoAddr);
    dbg_msg(dbg_trace, "%s(): channel = %d, IRQ_MODE = 0x%08X.\n", __FUNCTION__, NumberOfChannel, IrqMode.AsWhole);
	if(IrqMode.ByBits.Sign == 0xA4)
	{
		brd->m_IrqMode[NumberOfChannel] = Mode;
		brd->m_IrqTableNum = 4;
		if(Mode)
		{
			u32 iTnum = 0;
			brd->m_IrqTableNum = TableNum;
			for(iTnum = 0; iTnum < brd->m_IrqTableNum; iTnum++)
			{
                WriteOperationReg(brd,PEFIFOadr_IRQ_TBLADR + FifoAddr, (u16)iTnum);
				brd->m_IrqTable[iTnum] = AddrTable[iTnum];
				WriteOperationReg(brd,PEFIFOadr_IRQ_TBLDATA + FifoAddr, brd->m_IrqTable[iTnum]);
                dbg_msg(dbg_trace, "%s(): IrqAdr[%d] = 0x%08X.\n", __FUNCTION__, iTnum, brd->m_IrqTable[iTnum]);
			}
		}
		IrqMode.ByBits.Mode = Mode; 
		IrqMode.ByBits.Cnt = (brd->m_IrqTableNum >> 2) - 1;
		WriteOperationReg(brd,PEFIFOadr_IRQ_MODE + FifoAddr, IrqMode.AsWhole);
		IrqMode.AsWhole = ReadOperationReg(brd,PEFIFOadr_IRQ_MODE + FifoAddr);
        dbg_msg(dbg_trace, "%s(): channel = %d, IRQ_MODE = 0x%08X.\n", __FUNCTION__, NumberOfChannel, IrqMode.AsWhole);
	}
	else
		Status = -ENOSYS;

	return Status;
}

//-----------------------------------------------------------------------------
int Done(struct CWambpex *brd, u32 NumberOfChannel)
{
	DMA_CTRL_EXT CtrlExt;
	int Status = 0;
	u32 FifoAddr = brd->m_FifoAddr[NumberOfChannel];
    //dbg_msg(dbg_trace, "%s(): Pause Cancel\n", __FUNCTION__);
    CtrlExt.AsWhole = ReadOperationReg(brd, PEFIFOadr_DMA_CTRL + FifoAddr);
	CtrlExt.ByBits.Pause = 0;
    WriteOperationReg(brd, PEFIFOadr_DMA_CTRL + FifoAddr, CtrlExt.AsWhole);

	return Status;
}

//-----------------------------------------------------------------------------
int HwStartDmaTransfer(struct CWambpex *brd, u32 NumberOfChannel)
{
	int Status = 0;
	DMA_CTRL DmaCtrl = {0};
	u32 adm_num = 0;
	u32 tetr_num = 0;
    u64 SGTableAddress = 0;
    u32 LocalAddress = 0, DmaDirection = 0;
	u32 FifoAddr = brd->m_FifoAddr[NumberOfChannel];

    dbg_msg(dbg_trace, "%s(): channel = %d, FifoAddr = 0x%04X.\n", __FUNCTION__, NumberOfChannel, FifoAddr);

	DmaCtrl.AsWhole = 0;
    WriteOperationReg(brd, PEFIFOadr_DMA_CTRL + FifoAddr, DmaCtrl.AsWhole);
    WriteOperationReg(brd, PEFIFOadr_FLAG_CLR + FifoAddr, 0x10);

	GetSGStartParams(brd->m_DmaChannel[NumberOfChannel], &SGTableAddress, &LocalAddress, &DmaDirection); // SG

    dbg_msg(dbg_trace, "%s(): SGTableAddress = %llx\n", __FUNCTION__, SGTableAddress);

    WriteOperationReg(brd, PEFIFOadr_PCI_ADDRL + FifoAddr, (SGTableAddress)); // SG
    WriteOperationReg(brd, PEFIFOadr_PCI_ADDRH + FifoAddr, (SGTableAddress>>32));
	WriteOperationReg(brd, PEFIFOadr_PCI_SIZE + FifoAddr, 0); // SG
    wmb();

    dbg_msg(dbg_trace, "%s(): SG Table Address = 0x%llx, Local Address = 0x%X.\n", __FUNCTION__, SGTableAddress, LocalAddress);

	WriteOperationReg(brd, PEFIFOadr_LOCAL_ADR + FifoAddr, LocalAddress);

    brd->m_DmaChanEnbl[NumberOfChannel] = 1;
    brd->m_DmaIrqEnbl = 1;

    dbg_msg(dbg_trace, "%s(): Enable interrupt for device %p. Channel %d\n", __FUNCTION__, brd, NumberOfChannel);

	if(brd->m_BlockFifoId[NumberOfChannel] == PE_FIFO_ID)
	{
		FIFO_CTRL FifoCtrl;

		DmaCtrl.ByBits.SGEnbl = 1; // 1 - ���������� ������������� scatter/gather
		DmaCtrl.ByBits.DemandMode = 1;
		DmaCtrl.ByBits.IntEnbl = 1; // 1 - ���������� ������������ ���������� �� ���������� DMA
		DmaCtrl.ByBits.Start = 1;
        WriteOperationReg(brd, PEFIFOadr_DMA_CTRL + FifoAddr, DmaCtrl.AsWhole);
        dbg_msg(dbg_trace, "%s(): DMA_CTRL = 0x%04X.\n", __FUNCTION__, DmaCtrl.AsWhole);

        FifoCtrl.AsWhole = ReadOperationReg(brd, PEFIFOadr_FIFO_CTRL + FifoAddr);
		FifoCtrl.ByBits.DrqEn = 1;
        WriteOperationReg(brd, PEFIFOadr_FIFO_CTRL + FifoAddr, FifoCtrl.AsWhole);
        dbg_msg(dbg_trace, "%s(): FIFO_CTRL = 0x%04X.\n", __FUNCTION__, FifoCtrl.AsWhole);

        wmb();
	}
	if(brd->m_BlockFifoId[NumberOfChannel] == PE_EXT_FIFO_ID)
	{
		DMA_MODE_EXT ModeExt = {0};
		DMA_CTRL_EXT CtrlExt = {0};

        //ModeExt.AsWhole = ReadOperationReg(brd, PEFIFOadr_FIFO_CTRL + FifoAddr);
		ModeExt.ByBits.SGModeEnbl = 1;
		ModeExt.ByBits.DemandMode = 1;
		ModeExt.ByBits.IntEnbl = 1;
		ModeExt.ByBits.Dir = DmaDirection;
/*
<<<<<<< Updated upstream
=======
#warning !!!!! bla bla Enable IRQ_TEST MODE
        //ModeExt.AsWhole |= 0x80;

        WriteOperationReg(brd, (0x18*8 + brd->m_FifoAddr[0]), 0x44444);
        WriteOperationReg(brd, (0x18*8 + brd->m_FifoAddr[1]), 0x66666);
>>>>>>> Stashed changes
*/
        WriteOperationReg(brd, PEFIFOadr_FIFO_CTRL + FifoAddr, ModeExt.AsWhole);
        wmb();

        dbg_msg(dbg_trace, "%s(): channel = %d, DMA_MODE_EXT = 0x%X.\n", __FUNCTION__, NumberOfChannel, ModeExt.AsWhole);

		CtrlExt.AsWhole = 0;
		CtrlExt.ByBits.Start = 1;
/*
<<<<<<< Updated upstream
=======
        //CtrlExt.AsWhole |= 0x80;
>>>>>>> Stashed changes
*/
        WriteOperationReg(brd, PEFIFOadr_DMA_CTRL + FifoAddr, CtrlExt.AsWhole);

        dbg_msg(dbg_trace, "%s(): channel = %d, DMA_CTRL_EXT = 0x%04X.\n", __FUNCTION__, NumberOfChannel, CtrlExt.AsWhole);

        wmb();
	}

	adm_num = GetAdmNum(brd->m_DmaChannel[NumberOfChannel]);
	tetr_num = GetTetrNum(brd->m_DmaChannel[NumberOfChannel]);
	Status = DmaEnable(brd, adm_num, tetr_num);

    return Status;
}

//-----------------------------------------------------------------------------
int HwCompleteDmaTransfer(struct CWambpex *brd, u32 NumberOfChannel)
{
	int Status = 0;
	DMA_CTRL DmaCtrl = {0};
	u32 tetr_num = 0;
	int i = 0;
    int enbl = 0;
	u32 FifoAddr = brd->m_FifoAddr[NumberOfChannel];//(NumberOfChannel+1) * PE_FIFO_ADDR;

    dbg_msg(dbg_trace, "%s(): channel = %d\n", __FUNCTION__, NumberOfChannel);

	if(brd->m_BlockFifoId[NumberOfChannel] == PE_EXT_FIFO_ID)
	{
		DMA_CTRL_EXT CtrlExt;
		DMA_MODE_EXT ModeExt;

		CtrlExt.AsWhole = 0;
        WriteOperationReg(brd, PEFIFOadr_DMA_CTRL + FifoAddr, CtrlExt.AsWhole);
        //dbg_msg(dbg_trace, "%s(): DMA_CTRL_EXT = 0x%04X.\n", __FUNCTION__,CtrlExt.AsWhole);
		ModeExt.AsWhole = 0;
        WriteOperationReg(brd, PEFIFOadr_FIFO_CTRL + FifoAddr, ModeExt.AsWhole);
        //dbg_msg(dbg_trace, "%s(): channel = %d, DMA_MODE_EXT = 0x%X.\n", __FUNCTION__,NumberOfChannel, ModeExt.AsWhole);
	}
	if(brd->m_BlockFifoId[NumberOfChannel] == PE_FIFO_ID)
	{
		FIFO_STATUS FifoStat;

        DmaCtrl.AsWhole = ReadOperationReg(brd, PEFIFOadr_DMA_CTRL + FifoAddr);
        dbg_msg(dbg_trace, "%s(): channel = %d, DMA_CTRL = 0x%04X.\n", __FUNCTION__,NumberOfChannel, DmaCtrl.AsWhole);
		DmaCtrl.ByBits.Stop = 1;
        WriteOperationReg(brd, PEFIFOadr_DMA_CTRL + FifoAddr, DmaCtrl.AsWhole);
        dbg_msg(dbg_trace, "%s(): DMA_CTRL = 0x%04X.\n", __FUNCTION__,DmaCtrl.AsWhole);

		// ��������� ����� ���������� �������� DMA
		for(i = 0; i < 10; i++)
		{
            FifoStat.AsWhole = ReadOperationReg(brd, PEFIFOadr_FIFO_STATUS + FifoAddr);
            dbg_msg(dbg_trace, "%s(): FIFO_STATUS = 0x%04X.\n", __FUNCTION__, FifoStat.AsWhole);
			if(FifoStat.ByBits.SGEot)
				break;
                        ToTimeOut( 10);
		}

		// ����� ���� 4 �������� ��������� DMA_EOT � ����������
        WriteOperationReg(brd, PEFIFOadr_FLAG_CLR + FifoAddr, 0x10);
        WriteOperationReg(brd, PEFIFOadr_FLAG_CLR + FifoAddr, 0x00);
        dbg_msg(dbg_trace, "%s(): FLAG_CLR is clear.\n", __FUNCTION__);
	}

    brd->m_DmaChanEnbl[NumberOfChannel] = 0;
    for(i = 0; i < MAX_NUMBER_OF_DMACHANNELS; i++)
        if(brd->m_DmaChanEnbl[i])
            enbl = 1;
    brd->m_DmaIrqEnbl = enbl;

    dbg_msg(dbg_trace, "%s(): Disable interrupt for device %p. Channel %d\n", __FUNCTION__, brd, NumberOfChannel);

	tetr_num = GetTetrNum(brd->m_DmaChannel[NumberOfChannel]);
	Status = DmaDisable(brd, 0, tetr_num);
	CompleteDmaTransfer(brd->m_DmaChannel[NumberOfChannel]);

    return Status;
}
/*
void TimeoutTimerCallback(unsigned long arg )
{
    struct CWambpex *pDevice = (struct CWambpex*) arg;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    atomic_set(&pDevice->m_IsTimeout, 1);
}
*/
//-----------------------------------------------------------------------------
//  WaitSemaReady - wait semaphor flag or data ready flag (1)
//  Input:	SemaOrReady = 1 sema, 0 ready
//  Output:
//  Notes: при SemaOrReady = 1 ожидаем срабатывание флага семафора
//		   при SemaOrReady = 0 ожидаем готовности данных
//-----------------------------------------------------------------------------
int WaitSemaReady(struct CWambpex *brd, int SemaOrReady)
{
	SPD_CTRL SpdControl;
	int stat;
    //u32 timeout = 5;
    u32 pass = 0;

    SpdControl.AsWhole = ReadOperationReg(brd, PEMAINadr_SPD_CTRL);
    //dbg_msg(err_trace, "%s(): 0 -- SPD_CTRL = 0x%X\n", __FUNCTION__, SpdControl.AsWhole);

    while(1) {

        ndelay(10);

        SpdControl.AsWhole = ReadOperationReg(brd, PEMAINadr_SPD_CTRL);

        //dbg_msg(err_trace, "%s(): SPD_CTRL = 0x%X\n", __FUNCTION__, SpdControl.AsWhole);

        if(SemaOrReady)
            stat = SpdControl.ByBits.Sema;
        else
            stat = SpdControl.ByBits.Ready;

        if(stat)
            break;

        //deprecated
        //interruptible_sleep_on_timeout(&brd->m_WaitQueue, msecs_to_jiffies(timeout));
        //wait_event_interruptible_timeout(brd->m_WaitQueue, (stat != 0), msecs_to_jiffies(timeout));
        //interruptible_sleep_on_timeout(brd->m_WaitQueue, msecs_to_jiffies(timeout));

        pass++;

        if(pass > 10000) {
            dbg_msg(err_trace, "%s(): TIMEOUT\n", __FUNCTION__);
            return -ETIMEDOUT;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
//  WriteEEPROM - writing data to serial EEPROM
//  Input:  idRom - identificator of EEPROM (0 - ADM, 1 - base ICR)
//			buf - write data buffer
//			offset - offset within EEPROM
//			size - size of data buffer
//  Output:
//  Notes:
//-----------------------------------------------------------------------------
int WriteEEPROM(struct CWambpex *brd, int idRom, u16 *buf, u16 Offset, u32 Length)
{
	int Status = 0;
	SPD_CTRL SpdControl;
	u32 i=0;

    //dbg_msg(err_trace, "%s() ================================== \n", __FUNCTION__);

	SpdControl.AsWhole = 0;
	SpdControl.ByBits.Sema = 1;
	SpdControl.ByBits.SpdId = idRom; // 0 - submodule EEPROM, 1 - base EEPROM

    WriteOperationReg(brd, PEMAINadr_SPD_CTRL, SpdControl.AsWhole);
    udelay(500);
    Status = WaitSemaReady(brd,1); // waiting SEMA
    if(Status < 0) {
        dbg_msg(err_trace, "%s() Error in WaitSemaReady(SEMA)\n", __FUNCTION__);
        return 0;
    }

	SpdControl.ByBits.WriteEn = 1; // write enable
    WriteOperationReg(brd,PEMAINadr_SPD_CTRL, SpdControl.AsWhole);
    udelay(500);
    Status = WaitSemaReady(brd,0); // waiting READY
    if(Status < 0) {
        dbg_msg(err_trace, "%s() Error in WaitSemaReady(READY)\n", __FUNCTION__);
        return 0;
    }

	for(i = 0; i < Length; i++)
	{
        WriteOperationReg(brd,PEMAINadr_SPD_ADDR, Offset); // write address
        udelay(500);
        WriteOperationReg(brd,PEMAINadr_SPD_DATAL, buf[i]); // write data
        udelay(500);
		SpdControl.ByBits.WriteOp = 1;	// type operation - write
        dbg_msg(err_trace, "%s(): SPD_ADDR: 0x%x, SPD_DATAL: 0x%x, SPD_CTRL: 0x%x\n", __func__, Offset, buf[i], SpdControl.AsWhole);
        WriteOperationReg(brd,PEMAINadr_SPD_CTRL, SpdControl.AsWhole);
        udelay(500);
        Status = WaitSemaReady(brd,0); // waiting READY
        if(Status < 0) {
            dbg_msg(err_trace, "%s() Error in WaitSemaReady(READY)\n", __FUNCTION__);
            return 0;
        }
		Offset++;
	}

	SpdControl.ByBits.WriteEn = 0;
	SpdControl.ByBits.WriteOp = 0;
	SpdControl.ByBits.WriteDis = 1;  // write disable
    WriteOperationReg(brd,PEMAINadr_SPD_CTRL, SpdControl.AsWhole);
    udelay(500);
	SpdControl.AsWhole = 0;
    WriteOperationReg(brd,PEMAINadr_SPD_CTRL, SpdControl.AsWhole);
    udelay(500);

	return Status;
}

//-----------------------------------------------------------------------------
//  ReadEEPROM - reading data from serial EEPROM
//  Input:  idRom - identificator of EEPROM (0 - ADM, 1 - base ICR)
//			buf - reading data buffer
//			offset - offset within EEPROM
//			size - size of data buffer
//  Output: buf - read data buffer
//  Notes:
//-----------------------------------------------------------------------------
int ReadEEPROM(struct CWambpex *brd, int idRom, u16 *buf, u16 Offset, u32 Length)
{
	int Status = 0;
	SPD_CTRL SpdControl;
	u32 i=0;

    //dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

	SpdControl.AsWhole = 0;
	SpdControl.ByBits.Sema = 1;
	SpdControl.ByBits.SpdId = idRom; // 0 - submodule EEPROM, 1 - base EEPROM

    dbg_msg(err_trace, "%s() SpdControl.ByBits.SpdId = %d\n", __FUNCTION__, SpdControl.ByBits.SpdId);

    WriteOperationReg(brd,PEMAINadr_SPD_CTRL, SpdControl.AsWhole);
    udelay(500);
    Status = WaitSemaReady(brd,1); // waiting SEMA
    if(Status < 0) {
        dbg_msg(err_trace, "%s() Error in WaitSemaReady(SEMA)\n", __FUNCTION__);
        return 0;
    }

	for(i = 0; i < Length; i++)
	{
        WriteOperationReg(brd,PEMAINadr_SPD_ADDR, Offset); // write address
        udelay(500);
		SpdControl.ByBits.ReadOp = 1;	// type operation - read
        WriteOperationReg(brd,PEMAINadr_SPD_CTRL, SpdControl.AsWhole);
        udelay(500);
        Status = WaitSemaReady(brd,0); // waiting READY
        if(Status < 0) {
            dbg_msg(err_trace, "%s() Error in WaitSemaReady(READY)\n", __FUNCTION__);
            return 0;
        }
        buf[i] = ReadOperationReg(brd,PEMAINadr_SPD_DATAL); // read data
        dbg_msg(err_trace, "SPD_ADDR: 0x%x, SPD_DATAL: 0x%x, SPD_CTRL: 0x%x\n", Offset, buf[i], SpdControl.AsWhole);
		Offset++;
        udelay(500);
	}

	SpdControl.AsWhole = 0;
    WriteOperationReg(brd,PEMAINadr_SPD_CTRL, SpdControl.AsWhole);
    udelay(500);

	return Status;
}

//-----------------------------------------------------------------------------

int WaitReady(struct CWambpex *brd)
{
    u32 SpdControl = 0;
    int stat = 0;
    int i = 0;

    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    for(i = 0; i < 10000; i++) // wait 10 msec
    {
        SpdControl = ReadOperationReg(brd, PEFIDadr_SPD_CTRL + brd->m_BlockFidAddr);
        stat = SpdControl & 0x8000;
        dbg_msg(dbg_trace, "%s(): FMC EEPROM - stat = 0x%X\n", __FUNCTION__, stat);
        if(stat) break;
        udelay(10);
    }
    if(!stat)
        return -1;
    return 0;
}

//-----------------------------------------------------------------------------
//  WriteFmcEeprom - writing data to I2C device
//  Input:  devid - device identificator (0 - I2C FMC, 1 - source of power)
//			devadr - device address (0x50 - I2C FMC, 0 - source of power)
//			buf - write data buffer
//			offset - offset within EEPROM
//			size - size of data buffer
//  Output:
//  Notes:
//-----------------------------------------------------------------------------
int WriteFmcEeprom(struct CWambpex *brd, int devid, int devadr, u8 *buf, u16 Offset, long Length)
{
    int Status = 0;
    int i = 0;
    FMC_SPD_CTRL SpdControl;

    dbg_msg(dbg_trace, "%s(): FMC EEPROM - devid = 0x%X, devadr = 0x%X, offset = %d, length = %ld\n", __FUNCTION__, devid, devadr, Offset, Length);

    WriteOperationReg(brd, PEFIDadr_SPD_DEVICE + brd->m_BlockFidAddr, devid);
    SpdControl.AsWhole = 0;
    WriteOperationReg(brd, PEFIDadr_SPD_CTRL + brd->m_BlockFidAddr, SpdControl.AsWhole);
    ToTimeOut(1000);
    //ToPauseEx(1);
    Status = WaitReady(brd); // waiting READY
    if(Status < 0)
        return Status;

    for(i = 0; i < Length; i++)
    {
        int sector = Offset / 256;
        WriteOperationReg(brd, PEFIDadr_SPD_ADDR + brd->m_BlockFidAddr, Offset); // write address
        WriteOperationReg(brd, PEFIDadr_SPD_DATAL + brd->m_BlockFidAddr, buf[i]); // write data
        SpdControl.ByBits.Addr = devadr + sector;
        SpdControl.ByBits.WriteOp = 1;	// type operation - write
        WriteOperationReg(brd, PEFIDadr_SPD_CTRL + brd->m_BlockFidAddr, SpdControl.AsWhole);
        ToTimeOut(10000);
        Status = WaitReady(brd); // waiting READY
        dbg_msg(dbg_trace, "%s(): FMC EEPROM - sector = %d, address = 0x%X, offset = %d, data = %x, Status = %x\n",
                __FUNCTION__,sector, SpdControl.ByBits.Addr, Offset, buf[i], Status);
        if(Status < 0)
            break;
        Offset++;
    }
    SpdControl.AsWhole = 0;
    WriteOperationReg(brd, PEFIDadr_SPD_CTRL + brd->m_BlockFidAddr, SpdControl.AsWhole);
    return Status;
}

//-----------------------------------------------------------------------------
//  ReadFmcEeprom - reading data from I2C device
//  Input:  devid - device identificator (0 - I2C FMC, 1 - source of power)
//			devadr - device address (0x50 - I2C FMC, 0 - source of power)
//			buf - reading data buffer
//			offset - offset within EEPROM
//			Length - size of data buffer
//  Output: buf - read data buffer
//  Notes:
//-----------------------------------------------------------------------------
int ReadFmcEeprom(struct CWambpex *brd, int devid, int devadr, u8 *buf, u16 Offset, long Length)
{
    int Status = 0;
    int i = 0;
    FMC_SPD_CTRL SpdControl;

    dbg_msg(dbg_trace, "%s(): FMC EEPROM - devid = 0x%X, devadr = 0x%X, offset = %d, length = %ld\n", __FUNCTION__, devid, devadr, Offset, Length);

    WriteOperationReg(brd, PEFIDadr_SPD_DEVICE + brd->m_BlockFidAddr, devid);

    SpdControl.AsWhole = 0;
    WriteOperationReg(brd, PEFIDadr_SPD_CTRL + brd->m_BlockFidAddr, SpdControl.AsWhole);
    ToTimeOut(1000);
    //ToPauseEx(1);
    Status = WaitReady(brd); // waiting READY
    if(Status < 0)
        return Status;

    for(i = 0; i < Length; i++)
    {
        int sector = Offset / 256;
        WriteOperationReg(brd, PEFIDadr_SPD_ADDR + brd->m_BlockFidAddr, Offset); // write address
        SpdControl.ByBits.Addr = devadr + sector;
        SpdControl.ByBits.ReadOp = 1;	// type operation - read
//	    KdPrint(("CWambpex::Reading MFC EEPROM - sector = %d, address = 0x%X, offset = %d\n", sector, SpdControl.ByBits.Addr, Offset));
        WriteOperationReg(brd,PEFIDadr_SPD_CTRL + brd->m_BlockFidAddr, SpdControl.AsWhole);
//		ToPauseEx(1);
        Status = WaitReady(brd); // waiting READY
        //USHORT ubuf = ReadOperationReg(PEFIDadr_SPD_DATAL + m_BlockFidAddr); // read data
        //KdPrint(("CWambpex::Reading FMC EEPROM - ubuf = 0x%X, offset = %d\n", ubuf, Offset));
        buf[i] = ReadOperationReg(brd,PEFIDadr_SPD_DATAL + brd->m_BlockFidAddr) & 0xFF; // read data
        dbg_msg(dbg_trace, "%s(): FMC EEPROM - sector = %d, address = 0x%X, offset = %d, data = %x, Status = %x\n",
                __FUNCTION__,sector, SpdControl.ByBits.Addr, Offset, buf[i], Status);
        if(Status < 0)
            break;
        Offset++;
    }
    SpdControl.AsWhole = 0;
    WriteOperationReg(brd,PEFIDadr_SPD_CTRL + brd->m_BlockFidAddr, SpdControl.AsWhole);
    return Status;
}

//-----------------------------------------------------------------------------
// Base module identification
//
typedef struct _ICR_Id4953 {
    u16 wTag;           // tag of structure (BASE_ID_TAG)
    u16 wSize;          // size of all following fields of structure (without wTag + wSize) = sizeof(ICR_IdBase) - 4
    u16 wSizeAll;       // size of all data, writing into base module EPROM
    u32 dSerialNum;     // serial number ( () )
    u16 wDeviceId;      // base module type (  )
    u8  bVersion;       // base module version (  )
    u8  bDay;           // day of Data (      )
    u8  bMon;           // montag of Data (      )
    u16 wYear;          // year of Data (      )
} __attribute__((packed)) ICR_Id4953, *PICR_Id4953, ICR_IdBase, *PICR_IdBase;

//-----------------------------------------------------------------------------

u32 GetPID(struct CWambpex *brd)
{
    ICR_IdBase Info;
    int offset = (0x80/2);
    int size = (sizeof(Info)/2);
    int res = ReadEEPROM(brd, 1, (u16*)&Info, offset, size);
    if(res < 0) {
        err_msg(err_trace, "%s(): Error in ReadEEPROM()\n", __FUNCTION__);
        return ~0x0;
    }

    return Info.dSerialNum;
}

//-----------------------------------------------------------------------------

u32 GetTAG(struct CWambpex *brd)
{
    ICR_IdBase Info;
    int offset = (0x80/2);
    int size = (sizeof(Info)/2);
    int res = ReadEEPROM(brd, 1, (u16*)&Info, offset, size);
    if(res < 0) {
        err_msg(err_trace, "%s(): Error in ReadEEPROM()\n", __FUNCTION__);
        return ~0x0;
    }

    return Info.wTag;
}

//-----------------------------------------------------------------------------
