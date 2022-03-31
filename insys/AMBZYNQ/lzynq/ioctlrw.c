//****************** File Ioctlrw.cpp *******************************
//  class CWambpex - user interface functions
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
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "ddzynq.h"
#include "zynqdev.h"
#include "wambpmc.h"

//=============================================================================

static int get_ioctl_param(struct ioctl_param *param, size_t arg)
{
    if(!param || (arg == 0)) {
        err_msg(err_trace, "%s(): Error invalid argement in ioctl system call\n", __FUNCTION__);
        return -EFAULT;
    }

    memset(param, 0, sizeof(struct ioctl_param));

    if(copy_from_user(param, (void *)arg, sizeof(struct ioctl_param))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        return -EFAULT;
    }

    //dbg_msg(dbg_trace, "%s(): param->srcBuf = %p\n", __FUNCTION__, param->srcBuf);
    //dbg_msg(dbg_trace, "%s(): param->srcSize = 0x%x\n", __FUNCTION__, param->srcSize);
    //dbg_msg(dbg_trace, "%s(): param->dstBuf = %p\n", __FUNCTION__, param->dstBuf);
    //dbg_msg(dbg_trace, "%s(): param->dstSize = 0x%x\n", __FUNCTION__, param->dstSize);

    return 0;
}

//=============================================================================

int IoctlGetVersion(struct CWambpex *brd, size_t arg)
{
    char verInfo[128];
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    memset(verInfo, '\0', sizeof(verInfo));

    strcat( verInfo, ZynqDeviceName );
    strcat( verInfo, " ver. 2.36" );

    if(copy_to_user (( void *)param.dstBuf, (void *)verInfo, strlen(verInfo)+1)) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return 0;
}

//=============================================================================

int IoctlWriteRegData(struct CWambpex *brd, size_t arg)
{
    AMB_DATA_REG Register;
    int Status = 0;
    struct ioctl_param param;

    down(&brd->m_BoardRegSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        Status = -EINVAL;
        goto err;
    }

    if( copy_from_user((void *)&Register, (void *)param.srcBuf, sizeof(AMB_DATA_REG))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        Status = -EFAULT;
        goto err;
    }

    Status = WriteRegData(brd, Register.AdmNumber, Register.TetrNumber, Register.RegNumber, Register.Value);

err:
    up(&brd->m_BoardRegSem);

    return Status;
}

//=============================================================================

int IoctlWriteRegDataDir( struct CWambpex *brd, size_t arg )
{
    AMB_DATA_REG Register;
    u32 Address;
    u32 RegNumber;
    int Status = 0;
    struct ioctl_param param;

    down(&brd->m_BoardRegSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        Status = -EINVAL;
        goto err;
    }

    if( copy_from_user((void *)&Register, (void *)param.srcBuf, sizeof(AMB_DATA_REG))) {
        Status = -EFAULT;
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        goto err;
    }

    Address = Register.TetrNumber * TETRAD_SIZE;
    RegNumber = Register.RegNumber & 0x3;
    Address += RegNumber * REG_SIZE;

    WriteAmbReg(brd, Register.AdmNumber, Address, Register.Value);

    //dbg_msg(dbg_trace, "%s(): Adm = 0x%x, Tetr = 0x%x, Reg = 0x%x, Val = 0x%x\n", __FUNCTION__,
    //                Register.AdmNumber, Register.TetrNumber, Register.RegNumber, Register.Value);

err:
    up(&brd->m_BoardRegSem);

    return Status;
}

//=============================================================================

int IoctlReadRegData(struct CWambpex *brd, size_t arg)
{
    AMB_DATA_REG Register = {0};
    int Status = 0;
    struct ioctl_param param;

    down(&brd->m_BoardRegSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        Status = -EINVAL;
        goto err;
    }

    if( copy_from_user((void *)&Register, (void *)param.srcBuf, sizeof(AMB_DATA_REG))) {
        Status = -EFAULT;
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        goto err;
    }

    Status = ReadRegData(brd, Register.AdmNumber, Register.TetrNumber, Register.RegNumber, &Register.Value);

    //dbg_msg(dbg_trace, "%s(): Adm = 0x%x, Tetr = 0x%x, Reg = 0x%x, Val = 0x%x\n", __FUNCTION__,
    //                Register.AdmNumber, Register.TetrNumber, Register.RegNumber, Register.Value);

    if(copy_to_user (( void *)param.dstBuf, (void *)&Register, sizeof(AMB_DATA_REG))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        Status = -EFAULT;
    }

err:
    up(&brd->m_BoardRegSem);

    return Status;
}

//=============================================================================

int IoctlReadRegDataDir(struct CWambpex *brd, size_t arg)
{
    AMB_DATA_REG Register;
    u32 Address;
    u32 RegNumber;
    int Status = 0;
    struct ioctl_param param;

    down(&brd->m_BoardRegSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        Status = -EINVAL;
        goto err;
    }

    if( copy_from_user((void *)&Register, (void *)param.srcBuf, sizeof(AMB_DATA_REG))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        Status = -EFAULT;
        goto err;
    }

    Address = Register.TetrNumber * TETRAD_SIZE;
    RegNumber = Register.RegNumber & 0x3;
    Address += RegNumber * REG_SIZE;
    Register.Value = ReadAmbReg(brd, Register.AdmNumber, Address);

    //dbg_msg(dbg_trace, "%s(): Adm = 0x%x, Tetr = 0x%x, Reg = 0x%x, Val = 0x%x\n", __FUNCTION__,
    //                Register.AdmNumber, Register.TetrNumber, Register.RegNumber, Register.Value);

    if(copy_to_user (( void *)param.dstBuf, (void *)&Register, sizeof(AMB_DATA_REG))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        Status = -EFAULT;
    }

err:
    up(&brd->m_BoardRegSem);

    return Status;
}

//=============================================================================

int IoctlWriteRegBuf(	struct CWambpex *brd, size_t arg)
{
    u32 CmdAddress;
    u32 StatusAddress;
    u32 DataAddress;
    AMB_BUF_REG Register;
    u32	*KernelData = NULL;
    int Status = 0;
    struct ioctl_param param;

    down(&brd->m_BoardRegSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        Status = -EINVAL;
        goto err;
    }

    if( copy_from_user((void *)&Register, (void *)param.srcBuf, sizeof(AMB_BUF_REG))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        Status = -EFAULT;
        goto err;
    }

    if(Register.BufferSize > 0x20000) {
        err_msg(err_trace, "%s(): Too large buffer size\n", __FUNCTION__);
        Status = -EFAULT;
        goto err;
    }

    KernelData = kmalloc( Register.BufferSize, GFP_KERNEL );
    if(!KernelData) {
        err_msg(err_trace, "%s(): Error allocate buffer\n", __FUNCTION__);
        Status = -ENOMEM;
        goto err;
    }

    // get the user buffer
    if( copy_from_user((void *)KernelData, (void *)Register.pBuffer, Register.BufferSize)) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        Status = -EFAULT;
        goto err1;
    }

    CmdAddress = Register.TetrNumber * TETRAD_SIZE + TRDadr_CMD_ADR * REG_SIZE;
    WriteAmbReg(brd, Register.AdmNumber, CmdAddress, Register.RegNumber);
    StatusAddress = Register.TetrNumber * TETRAD_SIZE + TRDadr_STATUS * REG_SIZE;
    Status = WaitCmdReady(brd, Register.AdmNumber, StatusAddress); // wait CMD_RDY

    if(Status == 0) {
        DataAddress = Register.TetrNumber * TETRAD_SIZE + TRDadr_CMD_DATA * REG_SIZE;
        WriteBufAmbReg(brd, Register.AdmNumber, DataAddress, (u32*)KernelData, Register.BufferSize / 4);
    }

err1:
    kfree(KernelData);
err:
    up(&brd->m_BoardRegSem);

    return Status;
}

//=============================================================================

int IoctlWriteRegBufDir(struct CWambpex *brd, size_t arg)
{
    int Status = 0;
    AMB_BUF_REG Register;
    u32 Value = 0;
    u32 DataAddress;
    u32	*KernelData = NULL;
    struct ioctl_param param;

    down(&brd->m_BoardRegSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        Status = -EINVAL;
        goto err;
    }

    if( copy_from_user((void *)&Register, (void *)param.srcBuf, sizeof(AMB_BUF_REG))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        Status = -EFAULT;
        goto err;
    }

    if(Register.BufferSize > 0x20000) {
        err_msg(err_trace, "%s(): Too large buffer size\n", __FUNCTION__);
        Status = -EFAULT;
        goto err;
    }

    KernelData = kmalloc( Register.BufferSize, GFP_KERNEL );
    if(!KernelData) {
        err_msg(err_trace, "%s(): Error allocate buffer\n", __FUNCTION__);
        Status = -ENOMEM;
        goto err;
    }

    // get the user buffer
    if( copy_from_user((void *)KernelData, (void *)Register.pBuffer, Register.BufferSize)) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        Status = -EFAULT;
        goto err1;
    }

    Status = ReadRegData(brd, Register.AdmNumber, 0, 10, &Value);
    Value |= 1 << Register.TetrNumber;
    Status = WriteRegData(brd, Register.AdmNumber, 0, 10, Value);

    //dbg_msg(dbg_trace, "%s(): MODE2 = 0x%X.\n", __FUNCTION__, (int)Value);

    DataAddress = Register.TetrNumber * TETRAD_SIZE + TRDadr_DATA * REG_SIZE;
    WriteBufAmbReg(brd, Register.AdmNumber, DataAddress, (u32*)KernelData, Register.BufferSize / 4);
    Value &= ~(1 << Register.TetrNumber);
    Status = WriteRegData(brd, Register.AdmNumber, 0, 10, Value);

    //dbg_msg(dbg_trace, "%s(): MODE2 = 0x%X.\n", __FUNCTION__, (int)Value);

err1:
    kfree(KernelData);
err:
    up(&brd->m_BoardRegSem);

    return Status;
}

//=============================================================================

int IoctlReadRegBuf(struct CWambpex *brd, size_t arg)
{
    AMB_BUF_REG Register;
    u32 CmdAddress;
    u32 StatusAddress;
    int Status = 0;
    u32	*KernelData = NULL;
    struct ioctl_param param;

    down(&brd->m_BoardRegSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        Status = -EINVAL;
        goto err;
    }

    if( copy_from_user((void *)&Register, (void *)param.srcBuf, sizeof(AMB_BUF_REG))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        Status = -EFAULT;
        goto err;
    }

    if(Register.BufferSize > 0x20000) {
        err_msg(err_trace, "%s(): Too large buffer size\n", __FUNCTION__);
        Status = -EFAULT;
        goto err;
    }

    KernelData = kmalloc( Register.BufferSize, GFP_KERNEL );
    if(!KernelData) {
        err_msg(err_trace, "%s(): Error allocate buffer\n", __FUNCTION__);
        Status = -ENOMEM;
        goto err;
    }

    CmdAddress = Register.TetrNumber * TETRAD_SIZE + TRDadr_CMD_ADR * REG_SIZE;
    WriteAmbReg(brd,Register.AdmNumber, CmdAddress, Register.RegNumber);
    StatusAddress = Register.TetrNumber * TETRAD_SIZE + TRDadr_STATUS * REG_SIZE;
    Status = WaitCmdReady(brd,Register.AdmNumber, StatusAddress); // wait CMD_RDY

    if(Status == 0) {
        u32 DataAddress = Register.TetrNumber * TETRAD_SIZE + TRDadr_CMD_DATA * REG_SIZE;
        ReadBufAmbReg(brd,Register.AdmNumber, DataAddress, (u32*)KernelData, Register.BufferSize / 4);
    }

    // copy buffer to user
    if( copy_to_user((void*)Register.pBuffer, (void *)KernelData, Register.BufferSize)) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        Status = -EFAULT;
    }

    kfree(KernelData);

err:
    up(&brd->m_BoardRegSem);

    return Status;
}

//=============================================================================

int IoctlReadRegBufDir(struct CWambpex *brd, size_t arg)
{
    AMB_BUF_REG Register;
    u32 DataAddress;
    u32	*KernelData = NULL;
    int Status = 0;
    struct ioctl_param param;

    down(&brd->m_BoardRegSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        Status = -EINVAL;
        goto err;
    }

    if( copy_from_user((void *)&Register, (void *)param.srcBuf, sizeof(Register))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        Status = -EFAULT;
        goto err;
    }

    if(Register.BufferSize > 0x20000) {
        err_msg(err_trace, "%s(): Too large buffer size: 0x%x\n", __FUNCTION__, Register.BufferSize);
        Status = -EFAULT;
        goto err;
    }

    KernelData = kmalloc( Register.BufferSize, GFP_KERNEL );
    if(!KernelData) {
        err_msg(err_trace, "%s(): Error allocate buffer\n", __FUNCTION__);
        Status = -ENOMEM;
        goto err;
    }


    DataAddress = Register.TetrNumber * TETRAD_SIZE + TRDadr_DATA * REG_SIZE;
    ReadBufAmbReg(brd,Register.AdmNumber, DataAddress, KernelData, Register.BufferSize / 4);

    // copy buffer to user
    if( copy_to_user((void*)Register.pBuffer, (void *)KernelData, Register.BufferSize)) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        Status = -EFAULT;
    }

    kfree(KernelData);
err:
    up(&brd->m_BoardRegSem);

    return Status;
}

//=============================================================================
// TODO: Функция нигде не используется. Возможно ее лучше удалить.
int IoctlSetBusConfig(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    u8 *pBuffer = NULL;
    u32 BufferSize = 0;
    u32 Offset = 0;
    u32 Length = 0;
    AMB_DATA_BUF PciConfig;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if( copy_from_user((void *)&PciConfig, (void *)param.srcBuf, sizeof(AMB_DATA_BUF))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    pBuffer = (u8*)PciConfig.pBuffer;
    BufferSize = PciConfig.BufferSize;
    Offset = PciConfig.Offset;
    Length = BufferSize;

    Status = ReadConfigSpace(brd, pBuffer, Offset, Length);

    Status = WriteConfigSpace(brd, pBuffer, Offset, Length);

    if(copy_to_user (( void *)param.dstBuf, (void *)pBuffer, Length)) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlGetBusConfig(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    u32 Offset;
    u32 Length;
    AMB_DATA_BUF PciConfig;
    struct ioctl_param param;
//#error out of range possible !!!
    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if( copy_from_user((void *)&PciConfig, (void *)param.srcBuf, sizeof(AMB_DATA_BUF))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    Offset = PciConfig.Offset;
    Length = PciConfig.BufferSize;

    Status = ReadConfigSpace(brd, (u8*)&PciConfig, Offset, Length);

    if(copy_to_user(( void *)param.dstBuf, (void *)&PciConfig, Length)) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlGetLocation(struct CWambpex *brd, size_t arg)
{
    AMB_LOCATION AmbLocation;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    AmbLocation.BusNumber = brd->m_BusNumber;
    AmbLocation.DeviceNumber = brd->m_DevFNumber;
    AmbLocation.SlotNumber = brd->m_SlotNumber;

    dbg_msg(dbg_trace, "%s(): DevID = %X, G.add = %d\n", __FUNCTION__, brd->m_DeviceID, brd->m_GeographicAddr);
	if((FMC115CP_DEVID == brd->m_DeviceID ||
		FMC117CP_DEVID == brd->m_DeviceID) &&
		brd->m_GeographicAddr != 0xFFFFFFFF)
	{
		AmbLocation.SlotNumber = brd->m_GeographicAddr + (brd->m_FpgaOrderNum << 16);
	    dbg_msg(dbg_trace, "%s(): G.add = %d, PLD Order = %d - Slot = %d\n", __FUNCTION__, brd->m_GeographicAddr, brd->m_FpgaOrderNum, brd->m_SlotNumber);
	}

	if(copy_to_user (( void *)param.dstBuf, (void *)&AmbLocation, sizeof(AMB_LOCATION))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return 0;
}

//=============================================================================

int IoctlWriteLocalBus(struct CWambpex *brd, size_t arg)
{
    u8 ByteVal = 0;
    u16 WordVal = 0;
    u32 DwordVal = 0;
    u32 Val = 0;
    u32 BufferSize = 0;
    u32 Offset = 0;
    AMB_DATA_BUF LocalBus;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if( copy_from_user((void *)&LocalBus, (void *)param.srcBuf, sizeof(AMB_DATA_BUF))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
    	return -EFAULT;
    }

    BufferSize = LocalBus.BufferSize;
    Offset = LocalBus.Offset;
    switch(BufferSize)
    {
    case 1:
        {
            ByteVal = *(u8*)LocalBus.pBuffer;
            WriteOperationReg(brd, Offset, ByteVal);
            ByteVal = ReadOperationReg(brd, Offset);
            Val = ByteVal;
            break;
        }
    case 2:
        {
            WordVal = *(u16*)LocalBus.pBuffer;
            WriteOperationReg(brd,Offset, WordVal);
            WordVal = ReadOperationReg(brd,Offset);
            Val = WordVal;
            break;
        }
    case 4:
        {
            DwordVal = *(u32*)LocalBus.pBuffer;
            WriteOperationReg(brd,Offset, DwordVal);
            DwordVal = ReadOperationReg(brd,Offset);
            Val = DwordVal;
            break;
        }
    default: {
        up(&brd->m_BoardSem);
        return -EINVAL;
    }
    }

    dbg_msg(dbg_trace, "%s(): Address = 0x%X, Value = 0x%X\n", __FUNCTION__, Offset, Val);

    if(copy_to_user ((void *)param.dstBuf, (void *)&Val, sizeof(Val))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return 0;
}

//=============================================================================

int IoctlReadLocalBus(struct CWambpex *brd, size_t arg)
{
    u8 ByteVal = 0;
    u16 WordVal = 0;
    u32 DwordVal = 0;
    u32 Val = 0;
    u32 BufferSize = 0;
    u32 Offset = 0;
    AMB_DATA_BUF LocalBus;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if( copy_from_user((void *)&LocalBus, (void *)param.srcBuf, sizeof(AMB_DATA_BUF))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
    	return -EFAULT;
    }

    BufferSize = LocalBus.BufferSize;
    Offset = LocalBus.Offset;
    switch(BufferSize)
    {
    case 1:
        {
            ByteVal = ReadOperationReg(brd, Offset);
            Val = ByteVal;
            break;
        }
    case 2:
        {
            WordVal = ReadOperationReg(brd,Offset);
            Val = WordVal;
            break;
        }
    case 4:
        {
            DwordVal = ReadOperationReg(brd,Offset);
            Val = DwordVal;
            break;
        }
    default: {
        up(&brd->m_BoardSem);
        return -EINVAL;
    }
    }

    dbg_msg(dbg_trace, "%s(): Address = 0x%X, Value = 0x%X\n", __FUNCTION__, Offset, Val);

    if(copy_to_user ((void *)param.dstBuf, (void *)&Val, sizeof(Val))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return 0;
}

//=============================================================================

int IoctlWriteNvRAM(struct CWambpex *brd, size_t arg)
{
    AMB_DATA_BUF DataRom;
    u16 *pBufUser = NULL;
    u16 *pBufKern = NULL;
    u16 Offset;
    int Status;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if( copy_from_user((void *)&DataRom, (void *)param.srcBuf, sizeof(AMB_DATA_BUF))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    pBufUser = (u16*)DataRom.pBuffer;
    Offset = (u16)DataRom.Offset;

    pBufKern = kmalloc(DataRom.BufferSize, GFP_KERNEL);
    if(!pBufKern) {
        err_msg(err_trace, "%s: Cant allocate memory.\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -ENOMEM;
    }

    if(copy_from_user(pBufKern, pBufUser, DataRom.BufferSize)) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        kfree(pBufKern);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    Status = WriteEEPROM(brd, 1, pBufKern, Offset/2, DataRom.BufferSize/2); // 2-byte words

    kfree(pBufKern);

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlReadNvRAM(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    AMB_DATA_BUF DataRom;
    u16 	Offset;
    void  	*pUserBuf;
    void  	*pKernBuf;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if( copy_from_user((void *)&DataRom, (void *)param.srcBuf, sizeof(AMB_DATA_BUF))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
    	return -EFAULT;
    }

    pUserBuf = param.dstBuf;
    dbg_msg(err_trace, "%s(): User buffer %p\n", __FUNCTION__, pUserBuf);

    if(DataRom.BufferSize > PCI_EXPROM_SIZE) {
        dbg_msg(err_trace, "%s(): Large user buffer size!\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    pKernBuf = kmalloc(DataRom.BufferSize, GFP_KERNEL);
    if(!pKernBuf) {
        err_msg(err_trace, "%s: Cant allocate memory.\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -ENOMEM;
    }

    Offset = (u16)DataRom.Offset;
    Status = ReadEEPROM(brd, 1, pKernBuf, Offset / 2, DataRom.BufferSize / 2); // 2-byte words
    if(Status < 0) {
        memset(pKernBuf, 0xff, DataRom.BufferSize);
    }

    if(copy_to_user (( void *)pUserBuf, (void *)pKernBuf, DataRom.BufferSize)) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        kfree(pKernBuf);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    kfree(pKernBuf);

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlWriteAdmIdROM(struct CWambpex *brd, size_t arg)
{
    AMB_DATA_BUF DataRom;
    u16 *pBuffer = NULL;
    u32 BufferSize;
    u16 Offset;
    int Status;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if( copy_from_user((void *)&DataRom, (void *)param.srcBuf, sizeof(AMB_DATA_BUF))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
    	return -EFAULT;
    }

    pBuffer = DataRom.pBuffer;
    BufferSize = DataRom.BufferSize; // bytes
    Offset = (u16)DataRom.Offset;

    if(brd->m_BlockFidAddr) {

        Status = WriteFmcEeprom(brd, 0, 0x50, (u8*)pBuffer, Offset, BufferSize); // by byte

    } else {

        Status = WriteEEPROM(brd, 0, pBuffer, Offset / 2, BufferSize / 2); // 2-byte words
    }

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlReadAdmIdROM(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    AMB_DATA_BUF DataRom;
    u16 	Offset;
    void  	*pUserBuf;
    void  	*pKernBuf;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if( copy_from_user((void *)&DataRom, (void *)param.srcBuf, sizeof(AMB_DATA_BUF))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
    	return -EFAULT;
    }

    //pUserBuf = DataRom.pBuffer;
    pUserBuf = param.dstBuf;

    if(DataRom.BufferSize > PCI_EXPROM_SIZE) {
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    pKernBuf = kmalloc(DataRom.BufferSize, GFP_KERNEL);
    if(!pKernBuf) {
        err_msg(err_trace, "%s: Cant allocate memory.\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -ENOMEM;
    }

    Offset = (u16)DataRom.Offset;

    if(brd->m_BlockFidAddr) {

        Status = ReadFmcEeprom(brd, 0, 0x50, pKernBuf, Offset,  DataRom.BufferSize); // by byte

    } else {

        Status = ReadEEPROM(brd, 0, pKernBuf, Offset / 2, DataRom.BufferSize / 2); // 2-byte words
    }

    if(copy_to_user (( void *)pUserBuf, (void *)pKernBuf, DataRom.BufferSize)) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        kfree(pKernBuf);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    kfree(pKernBuf);

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlGetConfiguration(struct CWambpex *brd, size_t arg)
{
    AMB_CONFIGURATION	AmbConfiguration;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    AmbConfiguration.PhysAddress[0] = brd->m_OperationRegisters.PhysicalAddress;
    AmbConfiguration.VirtAddress[0] = (void*)brd->m_OperationRegisters.VirtualAddress;
    AmbConfiguration.Size[0] = brd->m_OperationRegisters.Length;
    AmbConfiguration.PhysAddress[1] = brd->m_AmbMainRegisters.PhysicalAddress;
    AmbConfiguration.VirtAddress[1] = (void*)brd->m_AmbMainRegisters.VirtualAddress;
    AmbConfiguration.Size[1] = brd->m_AmbMainRegisters.Length;
    AmbConfiguration.InterruptLevel = brd->m_Interrupt[0];
    AmbConfiguration.InterruptVector = brd->m_Interrupt[1];

    if(copy_to_user (( void *)param.dstBuf, (void *)&AmbConfiguration, sizeof(AMB_CONFIGURATION))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return 0;
}

//=============================================================================

int IoctlGetPldStatus(struct CWambpex *brd, size_t arg)
{
    u32	PldNum = 0;
    u32 PldState = 0;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if( copy_from_user((void *)&PldNum, (void *)param.srcBuf, sizeof(u32))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(PldNum >= NUMBER_OF_PLDS) {
        err_msg(err_trace, "%s(): Invalid PLD number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    PldState = brd->m_PldStatus[PldNum]; //in user space we wait 0 if PLD is ready

    //dbg_msg(dbg_trace, "%s(): ADM PLD[%d] status = 0x%X.\n", __FUNCTION__, PldNum, PldState);

    if(copy_to_user (( void *)param.dstBuf, (void *)&PldState, sizeof(u32))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return 0;
}

//=============================================================================
/*
static int createMemDscr(AMB_MEM_DMA_CHANNEL **Descr, unsigned long arg)
{
    AMB_MEM_DMA_CHANNEL MemDescr;
    AMB_MEM_DMA_CHANNEL *KernDescr = NULL;

    size_t tmpSize = 0;
    size_t tmpOrder = 0;

    dbg_msg(dbg_trace, "%s()\n",  __FUNCTION__);

    //копируем заголовок
    if (copy_from_user((void *)&MemDescr, (void*) arg, sizeof(AMB_MEM_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error get memory descriptor\n", __FUNCTION__);
        return -EFAULT;
    }

    tmpSize = sizeof(AMB_MEM_DMA_CHANNEL) + (MemDescr.BlockCnt - 1) * sizeof(size_t);

    //строим дескриптор в ядре
    tmpOrder = get_order(tmpSize);

    KernDescr = (AMB_MEM_DMA_CHANNEL*) __get_free_pages(GFP_KERNEL, tmpOrder);
    if (!KernDescr) {
        err_msg(err_trace, "%s(): No memory\n",  __FUNCTION__);
        return -ENOMEM;
    }

    //копируем весь пользовательский дескриптор в ядро
    if (copy_from_user((void *)KernDescr, (void*) arg, tmpSize)) {
        err_msg(err_trace, "%s(): Error copy user descriptor\n",  __FUNCTION__);
        free_pages((size_t)KernDescr, tmpOrder);
        return -EFAULT;
    }

    dbg_msg(dbg_trace, "%s(): blkNum = %d\n", __FUNCTION__, (int)KernDescr->BlockCnt );
    dbg_msg(dbg_trace, "%s(): blkSize = %x\n", __FUNCTION__, (int)KernDescr->BlockSize );
    dbg_msg(dbg_trace, "%s(): DmaChanNum = %x\n", __FUNCTION__, (int)KernDescr->DmaChanNum );
    dbg_msg(dbg_trace, "%s(): MemType = %x\n", __FUNCTION__, (int)KernDescr->MemType );
    dbg_msg(dbg_trace, "%s(): blkOrder = %d\n", __FUNCTION__, (int)get_order(KernDescr->BlockSize) );
    dbg_msg(dbg_trace, "%s(): dscOrder = %d\n", __FUNCTION__, (int)get_order(tmpSize) );

    *Descr = KernDescr;

    return 0;
}

//=============================================================================

static int freeMemDscr(AMB_MEM_DMA_CHANNEL *MemDescr)
{
    dbg_msg(dbg_trace, "%s()\n", __FUNCTION__);

    if (!MemDescr)
        return -EINVAL;

    dbg_msg(dbg_trace, "%s(): BlockCnt = %ld\n", __FUNCTION__, MemDescr->BlockCnt);

    //освобождение дескриптора
    free_pages((size_t)MemDescr,  get_order(sizeof(AMB_MEM_DMA_CHANNEL)+(MemDescr->BlockCnt-1)*sizeof(size_t)));

    return 0;
}
*/

//=============================================================================

int IoctlSetMem(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    u32 AdmNumber = 0;
    u32 TetrNumber = 0;
    u32 Address = 0;
    u32 dscrSize = 0;
    struct CDmaChannel *dma = NULL;
    //AMB_MEM_DMA_CHANNEL MemDscr = {0};
    PAMB_MEM_DMA_CHANNEL pKernMemDscr = NULL;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    dscrSize = param.srcSize;

    pKernMemDscr = kmalloc(  dscrSize, GFP_KERNEL );
    if ( !pKernMemDscr ) {
        err_msg(err_trace, "%s(): Can't allocate memory for DMA blocks\n", __FUNCTION__ );
        up(&brd->m_BoardSem);
        return -ENOMEM;
    }

    // get the user buffer
    if( copy_from_user((void *)pKernMemDscr, (void *)param.srcBuf, dscrSize)) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    //dscrSize = sizeof ( AMB_MEM_DMA_CHANNEL ) + ( MemDscr.BlockCnt - 1 ) * sizeof ( void * );
    //memset ( pKernMemDscr, 0, dscrSize );
    //memcpy ( pKernMemDscr, &MemDscr, dscrSize );

    //{
    //  int iBlk = 0;
    //  for(iBlk = 0; iBlk < pKernMemDscr->BlockCnt; iBlk++)
    //    dbg_msg(dbg_trace, "%s(): USER BLOCK ADDRESS %p\n", __FUNCTION__, pKernMemDscr->pBlock[iBlk] );
    //}

    if(pKernMemDscr->DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        kfree(pKernMemDscr);
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }
    if(!(brd->m_DmaChanMask & (1 << pKernMemDscr->DmaChanNum))) {
        err_msg(err_trace, "%s(): invalid stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    dma = brd->m_DmaChannel[pKernMemDscr->DmaChanNum];

    if(brd->m_BlockFifoId[pKernMemDscr->DmaChanNum] == PE_FIFO_ID) {
        if(brd->m_DmaDir[pKernMemDscr->DmaChanNum] != pKernMemDscr->Direction)	{
            kfree(pKernMemDscr);
            up(&brd->m_BoardSem);
            err_msg(err_trace, "%s(): invalid DMA direction\n", __FUNCTION__);
            return -EINVAL;
        }
    }

    Status = SetDmaDirection(dma, pKernMemDscr->Direction);
    if(Status != 0)	{
        kfree(pKernMemDscr);
        up(&brd->m_BoardSem);
        err_msg(err_trace, "IoctlSetMem: not supported transfer direction\n");
        return Status;
    }

    Adjust(dma,0);

    pKernMemDscr->BlockSize = (pKernMemDscr->BlockSize >> 12) << 12;
    if(!pKernMemDscr->BlockSize) {
        err_msg(err_trace, "%s(): block size is zero\n", __FUNCTION__);
/*
        if(copy_to_user (( void *)param.dstBuf, (void *)pKernMemDscr, tmpSize)) {
            kfree(pKernMemDscr);
            up(&brd->m_BoardSem);
            return -EFAULT;
    	}
*/
        kfree(pKernMemDscr);
        up(&brd->m_BoardSem);

        return 0;
    }

    AdmNumber = pKernMemDscr->LocalAddr >> 16;
    TetrNumber = pKernMemDscr->LocalAddr & 0xff;
    Address = AdmNumber*ADM_SIZE + TetrNumber*TETRAD_SIZE + TRDadr_DATA*REG_SIZE;

    SetDmaLocalAddress(dma, Address);
    SetAdmTetr(dma, AdmNumber, TetrNumber);
    Status = SetDmaMode(brd, pKernMemDscr->DmaChanNum, AdmNumber, TetrNumber);
    Status = RequestMemory( dma, pKernMemDscr->pBlock, pKernMemDscr->BlockSize,
                            (u32*)&pKernMemDscr->BlockCnt, &pKernMemDscr->pStub, pKernMemDscr->MemType );
    if(Status < 0) {
        err_msg(err_trace, "%s(): Error in RequestMemory()\n", __FUNCTION__);
        kfree(pKernMemDscr);
        up(&brd->m_BoardSem);
        return -ENOMEM;
    }

    SetIrqTable(brd, pKernMemDscr->DmaChanNum, 0, 4, 0);

    if(copy_to_user (( void *)param.dstBuf, (void *)pKernMemDscr, dscrSize)) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        kfree(pKernMemDscr);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    kfree(pKernMemDscr);
    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlFreeMem(struct CWambpex *brd, size_t arg)
{
    AMB_MEM_DMA_CHANNEL MemDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&MemDscr, (void *)param.srcBuf, sizeof(AMB_MEM_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(MemDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): Invalid stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    ReleaseMemory(brd->m_DmaChannel[MemDscr.DmaChanNum]);

    up(&brd->m_BoardSem);

    return 0;
}

//=============================================================================

int IoctlStartMem(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    AMB_START_DMA_CHANNEL StartDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&StartDscr, (void *)param.srcBuf, sizeof(AMB_START_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
    	return -EFAULT;
    }

    if(StartDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }
    if(!(brd->m_DmaChanMask & (1 << StartDscr.DmaChanNum))) {
        err_msg(err_trace, "%s(): invalid stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    Status = StartDmaTransfer(brd->m_DmaChannel[StartDscr.DmaChanNum], StartDscr.IsCycling);

    ToTimeOut(100);

    Status = HwStartDmaTransfer(brd, StartDscr.DmaChanNum);

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlStopMem(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    AMB_STATE_DMA_CHANNEL StopDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&StopDscr, (void *)param.srcBuf, sizeof(AMB_STATE_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(StopDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if(StopDscr.Timeout)
        Status = WaitBlockEndEvent(brd->m_DmaChannel[StopDscr.DmaChanNum], StopDscr.Timeout);
    else
        Status = 0;

    HwCompleteDmaTransfer(brd, StopDscr.DmaChanNum);

    GetState( brd->m_DmaChannel[StopDscr.DmaChanNum], (u32*)&StopDscr.BlockNum,
              (u32*)&StopDscr.BlockCntTotal, (u32*)&StopDscr.OffsetInBlock,
              (u32*)&StopDscr.DmaChanState);

    if(copy_to_user (( void *)param.dstBuf, (void *)&StopDscr, sizeof(AMB_STATE_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlStateMem(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    AMB_STATE_DMA_CHANNEL StateDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&StateDscr, (void *)param.srcBuf, sizeof(AMB_STATE_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(StateDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if(StateDscr.Timeout)
        Status = WaitBlockEndEvent(brd->m_DmaChannel[StateDscr.DmaChanNum], StateDscr.Timeout);
    else
        Status = 0;

    //FreezeState(brd->m_DmaChannel[i]);
    GetState(brd->m_DmaChannel[StateDscr.DmaChanNum], 	(u32*)&StateDscr.BlockNum,
             (u32*)&StateDscr.BlockCntTotal,
             (u32*)&StateDscr.OffsetInBlock,
             (u32*)&StateDscr.DmaChanState);

    if(copy_to_user (( void *)param.dstBuf, (void *)&StateDscr, sizeof(AMB_STATE_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlSetDirMem(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    AMB_SET_DMA_CHANNEL MemDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&MemDscr, (void *)param.srcBuf, sizeof(AMB_SET_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(MemDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    Status = SetDmaDirection(brd->m_DmaChannel[MemDscr.DmaChanNum], MemDscr.Param);

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlSetSrcMem(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    u32 AdmNumber = 0;
    u32 TetrNumber = 0;
    u32 Address = 0;
    AMB_SET_DMA_CHANNEL MemDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&MemDscr, (void *)param.srcBuf, sizeof(AMB_SET_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
    	return -EFAULT;
    }

    if(MemDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    AdmNumber = MemDscr.Param >> 16;
    TetrNumber = MemDscr.Param & 0xff;
    Address = AdmNumber * ADM_SIZE + TetrNumber * TETRAD_SIZE + TRDadr_DATA * REG_SIZE;
    SetDmaLocalAddress(brd->m_DmaChannel[MemDscr.DmaChanNum], Address);
    SetAdmTetr(brd->m_DmaChannel[MemDscr.DmaChanNum], AdmNumber, TetrNumber);
    Status = SetDmaMode(brd, MemDscr.DmaChanNum, AdmNumber, TetrNumber);

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlSetDrqMem(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    u32 AdmNum = 0;
    u32 TetrNum = 0;
    AMB_SET_DMA_CHANNEL MemDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&MemDscr, (void *)param.srcBuf, sizeof(AMB_SET_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(MemDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    AdmNum = GetAdmNum( brd->m_DmaChannel[MemDscr.DmaChanNum] );
    TetrNum = GetTetrNum( brd->m_DmaChannel[MemDscr.DmaChanNum] );
    Status = SetDrqFlag( brd, AdmNum, TetrNum, MemDscr.Param );

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlAdjust(struct CWambpex *brd, size_t arg)
{
    AMB_SET_DMA_CHANNEL MemDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&MemDscr, (void *)param.srcBuf, sizeof(AMB_SET_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(MemDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    if(!(brd->m_DmaChanMask & (1 << MemDscr.DmaChanNum))) {
        err_msg(err_trace, "%s(): invalid stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    Adjust(brd->m_DmaChannel[MemDscr.DmaChanNum], MemDscr.Param);

    up(&brd->m_BoardSem);

    return 0;
}

//=============================================================================

int IoctlDone(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    u32 done_flag;
    AMB_SET_DMA_CHANNEL MemDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&MemDscr, (void *)param.srcBuf, sizeof(AMB_SET_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(MemDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }
    if(!(brd->m_DmaChanMask & (1 << MemDscr.DmaChanNum))) {
        err_msg(err_trace, "%s(): invalid stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    done_flag = SetDoneBlock(brd->m_DmaChannel[MemDscr.DmaChanNum], MemDscr.Param);
    if(done_flag)
        Status = Done(brd, MemDscr.DmaChanNum);

    up(&brd->m_BoardSem);

    return 0;
}

//=============================================================================

int IoctlResetFifo(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    AMB_SET_DMA_CHANNEL MemDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&MemDscr, (void *)param.srcBuf, sizeof(AMB_SET_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(MemDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }
    if(!(brd->m_DmaChanMask & (1 << MemDscr.DmaChanNum))) {
        err_msg(err_trace, "%s(): invalid stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    Status = ResetFifo(brd, MemDscr.DmaChanNum);

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlSetIrqTable(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
	AMB_SET_IRQ_TABLE IrqDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&IrqDscr, (void *)param.srcBuf, sizeof(AMB_SET_IRQ_TABLE))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(IrqDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }
    if(!(brd->m_DmaChanMask & (1 << IrqDscr.DmaChanNum))) {
        err_msg(err_trace, "%s(): invalid stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    Status = SetIrqTable(brd, IrqDscr.DmaChanNum, IrqDscr.Mode, IrqDscr.TableNum, IrqDscr.AddrTable);

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlGetDmaChannelInfo(struct CWambpex *brd, size_t arg)
{
    AMB_GET_DMA_INFO DmaInfo = {0};
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&DmaInfo, (void *)param.srcBuf, sizeof(AMB_GET_DMA_INFO))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(DmaInfo.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }
    if(!(brd->m_DmaChanMask & (1 << DmaInfo.DmaChanNum))) {
        err_msg(0, "%s(): invalid stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    DmaInfo.Direction = brd->m_DmaDir[DmaInfo.DmaChanNum];
    DmaInfo.FifoSize = brd->m_DmaFifoSize[DmaInfo.DmaChanNum];
    DmaInfo.MaxDmaSize = brd->m_MaxDmaSize[DmaInfo.DmaChanNum];

    if(copy_to_user (( void *)param.dstBuf, (void *)&DmaInfo, sizeof(AMB_GET_DMA_INFO))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return 0;
}

//=============================================================================

int IoctlWaitDmaBuffer(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    AMB_STATE_DMA_CHANNEL StateDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&StateDscr, (void *)param.srcBuf, sizeof(AMB_STATE_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(StateDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    Status = WaitBufferEndEvent(brd->m_DmaChannel[StateDscr.DmaChanNum], StateDscr.Timeout);

    GetState(brd->m_DmaChannel[StateDscr.DmaChanNum], 	(u32*)&StateDscr.BlockNum,
             (u32*)&StateDscr.BlockCntTotal,
             (u32*)&StateDscr.OffsetInBlock,
             (u32*)&StateDscr.DmaChanState);

    if(copy_to_user (( void *)param.dstBuf, (void *)&StateDscr, sizeof(AMB_STATE_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlWaitDmaBlock(struct CWambpex *brd, size_t arg)
{
    int Status = -EINVAL;
    AMB_STATE_DMA_CHANNEL StateDscr;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&StateDscr, (void *)param.srcBuf, sizeof(AMB_STATE_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    if(StateDscr.DmaChanNum >= MAX_NUMBER_OF_DMACHANNELS) {
        err_msg(err_trace, "%s(): too large stream number\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    Status = WaitBlockEndEvent(brd->m_DmaChannel[StateDscr.DmaChanNum], StateDscr.Timeout);

    GetState(brd->m_DmaChannel[StateDscr.DmaChanNum], 	(u32*)&StateDscr.BlockNum,
             (u32*)&StateDscr.BlockCntTotal,
             (u32*)&StateDscr.OffsetInBlock,
             (u32*)&StateDscr.DmaChanState);

    if(copy_to_user (( void *)param.dstBuf, (void *)&StateDscr, sizeof(AMB_STATE_DMA_CHANNEL))) {
        err_msg(err_trace, "%s(): Error in copy_to_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    up(&brd->m_BoardSem);

    return Status;
}

//=============================================================================

int IoctlSetTetradIrq(struct CWambpex *brd, size_t arg)
{
    int i = 0;
    int res = 0;
    u32 irq_en;
    AMB_TETR_IRQ TetrIrq;
    TETRAD_IRQ* pTetrIrq = 0;
    MAIN_SELX sel_reg;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&TetrIrq, (void *)param.srcBuf, sizeof(AMB_TETR_IRQ))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }
    // found first free event
    for(i = 0; i < NUM_TETR_IRQ; i++) {
        if(brd->m_TetrIrq[i].EventId == 0) {
            break;
        }
    }
    if(i == NUM_TETR_IRQ) {
        err_msg(err_trace, "%s(): No free tetrade IRQ available\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    pTetrIrq = &brd->m_TetrIrq[i];

    brd->m_TetrIrq[i].Address = TetrIrq.AdmNumber * ADM_SIZE + TetrIrq.TetrNumber * TETRAD_SIZE + TRDadr_STATUS * REG_SIZE;
    brd->m_TetrIrq[i].IrqMask = TetrIrq.IrqMask;
    brd->m_TetrIrq[i].IrqInv = TetrIrq.IrqInv;
    brd->m_TetrIrq[i].EventId = (size_t)(&brd->m_TetrIrq[i].EventId); // get address of same struct field to mark event busy

    tasklet_init(&brd->m_TetrIrq[i].Dpc, TetrDpcForIsr, (unsigned long)&brd->m_TetrIrq[i]);
    //void tasklet_enable(struct tasklet_struct *t);

    irq_en = 0x1 << IRQ_NUMBER;
    res = WriteRegData(brd, TetrIrq.AdmNumber, 0, MAINnr_IRQENST, irq_en);
    res = ReadRegData(brd, TetrIrq.AdmNumber, 0, 16 + TetrIrq.TetrNumber, &sel_reg.AsWhole);
    sel_reg.ByBits.IrqNum = IRQ_NUMBER;
    res = WriteRegData(brd, TetrIrq.AdmNumber, 0, 16 + TetrIrq.TetrNumber, sel_reg.AsWhole);

    brd->m_FlgIrqEnbl = 1;

    dbg_msg(dbg_trace, "%s(): Event = %d, Address = 0x%x, TetrNumber = %d, IrqMask = 0x%X\n", __func__, i, pTetrIrq->Address, TetrIrq.TetrNumber, pTetrIrq->IrqMask);

    up(&brd->m_BoardSem);

    return res;
}

//=============================================================================

int IoctlClearTetradIrq(struct CWambpex *brd, size_t arg)
{
    int i = 0;
    int res = 0;
    int registered_events_count = 0;
    u32 Address;
    AMB_TETR_IRQ TetrIrq;
    TETRAD_IRQ* pTetrIrq = 0;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EINVAL;
    }

    // get the user buffer
    if( copy_from_user((void *)&TetrIrq, (void *)param.srcBuf, sizeof(AMB_TETR_IRQ))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        up(&brd->m_BoardSem);
        return -EFAULT;
    }

    Address = TetrIrq.AdmNumber * ADM_SIZE + TetrIrq.TetrNumber * TETRAD_SIZE + TRDadr_STATUS * REG_SIZE;
    for(i = 0; i < NUM_TETR_IRQ; i++) {
        pTetrIrq = &brd->m_TetrIrq[i];
        if(brd->m_TetrIrq[i].Address == Address)
        {
            MAIN_SELX sel_reg;
            res = ReadRegData(brd, TetrIrq.AdmNumber, 0, 16 + TetrIrq.TetrNumber, &sel_reg.AsWhole);
            sel_reg.ByBits.IrqNum = 0;
            res = WriteRegData(brd, TetrIrq.AdmNumber, 0, 16 + TetrIrq.TetrNumber, sel_reg.AsWhole);

            if(brd->m_TetrIrq[i].EventId) {
                brd->m_TetrIrq[i].EventId = 0;
                tasklet_disable(&brd->m_TetrIrq[i].Dpc);
            }

            dbg_msg(dbg_trace, "%s(): Event = %d, Address = 0x%x, TetrNumber = %d, IrqMask = 0x%X\n", __func__, i, pTetrIrq->Address, TetrIrq.TetrNumber, pTetrIrq->IrqMask);
            break;
        }
        if(brd->m_TetrIrq[i].EventId) {
            ++registered_events_count;
        }
    }

    if(registered_events_count) {
        brd->m_FlgIrqEnbl = 1;
    } else {
        brd->m_FlgIrqEnbl = 0;
    }

    up(&brd->m_BoardSem);

    return 0;
}

//=============================================================================

int IoctlWaitTetradIrq(struct CWambpex *brd, size_t arg)
{
    int i = 0;
    int status = 0;
    u32 Address;
    u32 timeout = 1000;
    AMB_TETR_IRQ TetrIrq;
    TETRAD_IRQ* pTetrIrq = 0;
    struct ioctl_param param;

    down(&brd->m_BoardSem);

    if(get_ioctl_param(&param, arg) < 0) {
        err_msg(err_trace, "%s(): Error in get_ioctl_param()\n", __FUNCTION__);
        status = -EINVAL;
        goto err_exit;
    }

    // get the user buffer
    if( copy_from_user((void *)&TetrIrq, (void *)param.srcBuf, sizeof(AMB_TETR_IRQ))) {
        err_msg(err_trace, "%s(): Error in copy_from_user()\n", __FUNCTION__);
        status = -EFAULT;
        goto err_exit;
    }

    Address = TetrIrq.AdmNumber * ADM_SIZE + TetrIrq.TetrNumber * TETRAD_SIZE + TRDadr_STATUS * REG_SIZE;
    timeout = TetrIrq.IrqInv;
    for(i = 0; i < NUM_TETR_IRQ; i++) {
        pTetrIrq = &brd->m_TetrIrq[i];
        if(brd->m_TetrIrq[i].Address == Address)
        {
            if(brd->m_TetrIrq[i].EventId) {
                dbg_msg(dbg_trace, "%s(): Wait event = %d, Address = 0x%x, TetrNumber = %d, IrqMask = 0x%X\n", __func__, i, pTetrIrq->Address, TetrIrq.TetrNumber, pTetrIrq->IrqMask);
                status = WaitEvent(&brd->m_TetrIrq[i].Event, timeout);
                if(status == 0) {
                    ResetEvent(&brd->m_TetrIrq[i].Event);
                    dbg_msg(dbg_trace, "%s(): Wait event = %d, Address = 0x%x, TetrNumber = %d, IrqMask = 0x%X - SUCCESS\n", __func__, i, pTetrIrq->Address, TetrIrq.TetrNumber, pTetrIrq->IrqMask);
                } else {
                    dbg_msg(dbg_trace, "%s(): Wait event = %d, Address = 0x%x, TetrNumber = %d, IrqMask = 0x%X - TIMEOUT\n", __func__, i, pTetrIrq->Address, TetrIrq.TetrNumber, pTetrIrq->IrqMask);
                }
            } else {
                dbg_msg(err_trace,"%s(): ERROR - Event was not initialized, TetrNumber = %d\n", __func__, TetrIrq.TetrNumber);
                status = -EINVAL;
                goto err_exit;
            }
            break;
        }
    }
    if(i == NUM_TETR_IRQ)
    {
        dbg_msg(err_trace,"%s(): ERROR - No Event was not captured, TetrNumber = %d\n", __func__, TetrIrq.TetrNumber);
        status = -EINVAL;
    }

err_exit:
    up(&brd->m_BoardSem);

    return status;
}

//=============================================================================

//////////////////////// end of file ioctlrw.c ////////////////////////////
