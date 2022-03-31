
#ifndef _IOCTLRW_H_
#define _IOCTLRW_H_

#include "zynqdev.h"

int IoctlGetVersion(struct CWambpex *brd, size_t arg);
int IoctlWriteRegData( struct CWambpex *brd, size_t arg);
int IoctlWriteRegDataDir( struct CWambpex *brd, size_t arg);
int IoctlReadRegData(struct CWambpex *brd, size_t arg);
int IoctlReadRegDataDir(struct CWambpex *brd, size_t arg);
int IoctlWriteRegBuf(	struct CWambpex *brd, size_t arg);
int IoctlWriteRegBufDir( struct CWambpex *brd, size_t arg );
int IoctlReadRegBuf(struct CWambpex *brd, size_t arg);
int IoctlReadRegBufDir(struct CWambpex *brd, size_t arg);
int IoctlSetBusConfig(struct CWambpex *brd, size_t arg);
int IoctlGetBusConfig(struct CWambpex *brd, size_t arg);
int IoctlGetLocation(struct CWambpex *brd, size_t arg);
int IoctlGetConfiguration(struct CWambpex *brd, size_t arg);
int IoctlWriteNvRAM(struct CWambpex *brd, size_t arg);
int IoctlReadNvRAM(struct CWambpex *brd, size_t arg);
int IoctlWriteAdmIdROM(struct CWambpex *brd, size_t arg);
int IoctlReadAdmIdROM(struct CWambpex *brd, size_t arg);
int IoctlLoadPld(struct CWambpex *brd, size_t arg);
int IoctlGetPldStatus(struct CWambpex *brd, size_t arg);
int SetMemCheckParameters(struct CWambpex *brd, size_t arg);
int IoctlSetMem(struct CWambpex *brd, size_t arg);
int FreeMemCheckParameters(struct CWambpex *brd, size_t arg);
int IoctlFreeMem(struct CWambpex *brd, size_t arg);
int StartMemCheckParameters(struct CWambpex *brd, size_t arg);
int IoctlStartMem(struct CWambpex *brd, size_t arg);
int StopMemCheckParameters(struct CWambpex *brd, size_t arg);
int IoctlStopMem(struct CWambpex *brd, size_t arg);
int StateMemCheckParameters(struct CWambpex *brd, size_t arg);
int IoctlStateMem(struct CWambpex *brd, size_t arg);
int IoctlSetDirMem(struct CWambpex *brd, size_t arg);
int IoctlSetSrcMem(struct CWambpex *brd, size_t arg);
int IoctlSetDrqMem(struct CWambpex *brd, size_t arg);
int IoctlWriteLocalBus(struct CWambpex *brd, size_t arg);
int IoctlReadLocalBus(struct CWambpex *brd, size_t arg);
int IoctlSetTetradIrq(struct CWambpex *brd, size_t arg);
int IoctlClearTetradIrq(struct CWambpex *brd, size_t arg);
int IoctlWaitTetradIrq(struct CWambpex *brd, size_t arg);
int IoctlAdjust(struct CWambpex *brd, size_t arg);
int IoctlDone(struct CWambpex *brd, size_t arg);
int IoctlGetDmaChannelInfo(struct CWambpex *brd, size_t arg);
int IoctlResetFifo(struct CWambpex *brd, size_t arg);
int IoctlSetIrqTable(struct CWambpex *brd, size_t arg);
int IoctlWaitDmaBuffer(struct CWambpex *brd, size_t arg);
int IoctlWaitDmaBlock(struct CWambpex *brd, size_t arg);

#endif
