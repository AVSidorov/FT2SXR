//****************** File Wambpex.cpp *******************************
//  class CWambpex implementation
//
//	Copyright (c) 2007, Instrumental Systems,Corp.
//  Written by Dorokhin Andrey
//
//  History:
//  19-03-07 - builded
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

u32 ReadOperationReg(struct CWambpex *dev, u32 RelativePort)
{
    size_t addr = (size_t)dev->bar[0].VirtualAddress + RelativePort;
    return readl((u32*)addr);
}

//-----------------------------------------------------------------------------

void WriteOperationReg(struct CWambpex *dev, u32 RelativePort, u32 Value)
{
    size_t addr = (size_t)dev->bar[0].VirtualAddress + RelativePort;
    writel(Value, (u32*)addr);
}

//-----------------------------------------------------------------------------

u32 ReadAmbMainReg(struct CWambpex *dev, u32 RelativePort)
{
    size_t addr = (size_t)dev->bar[1].VirtualAddress + RelativePort;
    return readl((u32*)addr);
}

//-----------------------------------------------------------------------------

void WriteAmbMainReg(struct CWambpex *dev, u32 RelativePort, u32 Value)
{
    size_t addr = (size_t)dev->bar[1].VirtualAddress + RelativePort;
    writel(Value, (u32*)addr);
}

//-----------------------------------------------------------------------------

u32 ReadAmbReg(struct CWambpex *dev, u32 AdmNumber, u32 RelativePort)
{
    size_t addr = (size_t)dev->bar[1].VirtualAddress + AdmNumber*ADM_SIZE + RelativePort;
    return readl((u32*)addr);
}

//-----------------------------------------------------------------------------

void WriteAmbReg(struct CWambpex *dev, u32 AdmNumber, u32 RelativePort, u32 Value)
{
    size_t addr = (size_t)dev->bar[1].VirtualAddress + AdmNumber*ADM_SIZE + RelativePort;
    writel(Value, (u32*)addr);
}

//-----------------------------------------------------------------------------

void read_buffer(u32 *src, u32 *dst, u32 cnt)
{
    int i=0;
    for(i=0; i<cnt; i++) {
        dst[i] = readl(src);
    }
}

//-----------------------------------------------------------------------------

void write_buffer(u32 *dst, u32 *src, u32 cnt)
{
    int i=0;
    for(i=0; i<cnt; i++) {
        writel(src[i], dst);
    }
}

//-----------------------------------------------------------------------------

void ReadBufAmbReg(struct CWambpex *dev, u32 AdmNumber, u32 RelativePort, u32* VirtualAddress, u32 DwordsCount)
{
    size_t addr = (size_t)dev->bar[1].VirtualAddress + AdmNumber*ADM_SIZE + RelativePort;
    read_buffer((u32*)addr,VirtualAddress, DwordsCount);
}

//-----------------------------------------------------------------------------

void WriteBufAmbReg(struct CWambpex *dev, u32 AdmNumber, u32 RelativePort, u32* VirtualAddress, u32 DwordsCount)
{
    size_t addr = (size_t)dev->bar[1].VirtualAddress + AdmNumber*ADM_SIZE + RelativePort;
    write_buffer((u32*)addr, VirtualAddress, DwordsCount);
}

//-----------------------------------------------------------------------------

void WriteBufAmbMainReg(struct CWambpex *dev, u32 RelativePort, u32* VirtualAddress, u32 DwordsCount)
{
    size_t addr = (size_t)dev->bar[1].VirtualAddress + RelativePort;
    write_buffer((u32*)addr, VirtualAddress, DwordsCount);
}

//-----------------------------------------------------------------------------

int WaitCmdReady(struct CWambpex *dev, u32 AdmNumber, u32 StatusAddress)
{
    u32 cmd_rdy = 0;
    //u32 timeout = 1;
    u32 pass = 0;

    ndelay(10);

    while(1) {

        ndelay(10);

        cmd_rdy = ReadAmbReg(dev, AdmNumber, StatusAddress);
        cmd_rdy &= AMB_statCMDRDY;

        if(cmd_rdy)
            break;

        //wait_event_interruptible_timeout(dev->m_WaitQueue, (cmd_rdy != 0), msecs_to_jiffies(10));
        pass++;

        if(pass > 10000) {
            //err_msg(err_trace, "%s(): TIMEOUT!\n", __FUNCTION__);
            return -1;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------

int WriteRegData(struct CWambpex *dev, u32 AdmNumber, u32 TetrNumber, u32 RegNumber, u32 Value)
{
    int Status = 0;
    u32 TrdAddress    = TetrNumber * TETRAD_SIZE;
    u32 CmdAddress    = TrdAddress + TRDadr_CMD_ADR * REG_SIZE;
    u32 DataAddress   = TrdAddress + TRDadr_CMD_DATA * REG_SIZE;
    u32 StatusAddress = TrdAddress + TRDadr_STATUS * REG_SIZE;

    WriteAmbReg(dev, AdmNumber, CmdAddress, RegNumber);

    Status = WaitCmdReady(dev, AdmNumber, StatusAddress); // wait CMD_RDY
    if(Status != 0) {
        err_msg(err_trace, "%s(): ERROR wait cmd ready.\n", __FUNCTION__);
        return Status;
    }

    WriteAmbReg(dev, AdmNumber, DataAddress, Value);

    return Status;
}

//--------------------------------------------------------------------

int ReadRegData(struct CWambpex *dev, u32 AdmNumber, u32 TetrNumber, u32 RegNumber, u32 *Value)
{
    int Status = 0;
    u32 TrdAddress    = TetrNumber * TETRAD_SIZE;
    u32 CmdAddress    = TrdAddress + TRDadr_CMD_ADR * REG_SIZE;
    u32 DataAddress   = TrdAddress + TRDadr_CMD_DATA * REG_SIZE;
    u32 StatusAddress = TrdAddress + TRDadr_STATUS * REG_SIZE;

    WriteAmbReg(dev, AdmNumber, CmdAddress, RegNumber);

    Status = WaitCmdReady(dev, AdmNumber, StatusAddress); // wait CMD_RDY
    if(Status != 0) {
        err_msg(err_trace, "%s(): ERROR wait cmd ready. trd 0x%x, reg 0x%x\n", __FUNCTION__, TetrNumber, RegNumber);
        return Status;
    }

    *Value = ReadAmbReg(dev, AdmNumber, DataAddress) & 0xffff;

    return Status;
}

//--------------------------------------------------------------------

u32 RegPeekInd(struct CWambpex *dev, u32 trdNo, u32 rgnum)
{
    u32 Value = 0;

    ReadRegData(dev, 0, trdNo, rgnum, &Value);

    return Value;
}

//--------------------------------------------------------------------

int RegPokeInd(struct CWambpex *dev, u32 trdNo, u32 rgnum, u32 Value)
{
    return WriteRegData(dev, 0, trdNo, rgnum, Value);
}

//--------------------------------------------------------------------

int RegPokeDir(struct CWambpex *dev, u32 TetrNumber, u32 RegNumber, u32 Value)
{
    u32 Address;

    Address = TetrNumber * TETRAD_SIZE;
    RegNumber = RegNumber & 0x3;
    Address += RegNumber * REG_SIZE;

    WriteAmbReg(dev, 0, Address, Value);

    return 0;
}

//--------------------------------------------------------------------

int RegPeekDir(struct CWambpex *dev, u32 TetrNumber, u32 RegNumber)
{
    u32 Address;

    Address = TetrNumber * TETRAD_SIZE;
    RegNumber = RegNumber & 0x3;

    Address += RegNumber * REG_SIZE;

    return ReadAmbReg(dev, 0, Address);
}

//--------------------------------------------------------------------

void device_name(struct CWambpex *brd, int id, int bn)
{
    switch(id) {

    case FMC130E_DEVID:
        snprintf(brd->m_name, sizeof(brd->m_name), "%s%d", "FMC130E", bn);
        break;
    case FMC133V_DEVID:
        snprintf(brd->m_name, sizeof(brd->m_name), "%s%d", "FMC133V", bn);
        break;
    case FMC138M_DEVID:
        snprintf(brd->m_name, sizeof(brd->m_name), "%s%d", "FMC138M", bn);
        break;
    default:
        snprintf(brd->m_name, sizeof(brd->m_name), "%s", "UFO_BOARD");
    }
}

//-----------------------------------------------------------------------------

int device_match(u16 device_id)
{
    switch(device_id) {

    case FMC130E_DEVID:
    case FMC133V_DEVID:
    case FMC138M_DEVID:
        return 1;
    default:
        return 0;
    }
}

//-----------------------------------------------------------------------------

int ReadConfigSpace(struct CWambpex *brd,
                    u8	 *pBuffer,
                    u32	 Offset,
                    u32	 Length )
{
    dbg_msg(dbg_trace, "%s(%d, %d)\n", __FUNCTION__, Offset, Length);

    if((Offset == 2) && (Length == 2)) {
        *((u16*)pBuffer) = brd->m_DeviceID;
        return 0;
    }

    if((Offset == 8) && (Length == 1)) {
        *((u8*)pBuffer) = brd->m_RevisionID;
        return 0;
    }

    return -ENOSYS;
}

//-----------------------------------------------------------------------------

int WriteConfigSpace(struct CWambpex *brd, u8 *pBuffer, u32 Offset, u32 Length)
{
    return -ENOSYS;
}

//-----------------------------------------------------------------------------

int SetDmaDelay(struct CWambpex *brd)
{
    brd->m_mcTimeout = 1;
    return 0;
}

//-----------------------------------------------------------------------------

void ToPause(int time_ms)
{
    //msleep(time_ms);
}

//-----------------------------------------------------------------------------

void ToTimeOut(int time_us)
{
    //ndelay(time_us);
}

//-----------------------------------------------------------------------------
