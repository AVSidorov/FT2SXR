//****************** File PLX9x56.h *********************************
//  PLX9x56 hardware definitions
//
//	Copyright (c) 2003-2006, Instrumental Systems,Corp.
//  Written by Dorokhin Andrey
//
//  History:
//  19-12-03 - builded
//  21-06-06 - add MODEDMAARBITR & LOCALBUSREGDESCR definitions
//
//*******************************************************************

#ifndef _PLX9x56_H_
#define _PLX9x56_H_

#define PLX9656_PCIStatusRegister   0x06

#if !defined (WIN32) && !defined (_WIN32_)
	#define FIELD_OFFSET(type, field)    ((size_t)(size_t)&(((type *)0)->field))
#endif //

#ifdef _WIN32_
	typedef unsigned int   UINT;
#endif //

    // PCI Configuration Registers
typedef union _PVPDAD { // PCI Vital Product Data (VPD) Address register (0x4E)
    u16 AsWhole;	//				    (R-W-Rst)
	struct {//                          ------------- 
        u16 Address       :15; // 14:0 (1-1-0):Byte address of the VPD address to be accessed.
        u16 Flag          :1;  // 15   (1-1-0):Flag used to indicate when the transfer of data between PVPDATA and the storage component is complete.
	} ByBits;		//
} PVPDAD, *PPVPDAD;	

typedef struct _PLX9656_PCI_CONFIGURATION_REGISTERS {

    u16  VendorID;           // 0x00
    u16  DeviceID;           // 0x02
    u16  Command;            // 0x04
    u16  Status;				// 0x06
    u8   RevisionID;         // 0x08
    u8   ProgIf;             // 0x09
    u8   SubClass;           // 0x0A
    u8   BaseClass;          // 0x0B
    u8   CacheLineSize;      // 0x0C
    u8   LatencyTimer;       // 0x0D
    u8   HeaderType;         // 0x0E
    u8   BIST;               // 0x0F
    u32   BaseAddresses0;		// 0x10
    u32   BaseAddresses1;		// 0x14
    u32   BaseAddresses2;		// 0x18
    u32   BaseAddresses3;		// 0x1C
    u32   Reserved1[2];		// 0x20
    u32   CIS;				// 0x28
    u16  SubVendorID;		// 0x2C
    u16  SubSystemID;		// 0x2E
    u32   ROMBaseAddress;		// 0x30
    u32   Reserved2[2];		// 0x34
    u8   InterruptLine;      // 0x3C
    u8   InterruptPin;       // 0x3D
    u8   MinimumGrant;       // 0x3E
    u8   MaximumLatency;     // 0x3F
    u8   PowManagCapID;		// 0x40
    u8   PowManagNextCapPtr;	// 0x41
    u16  PowManagCapabilites;// 0x42
    u16  PowManagCtrlStat;	// 0x44
    u8   PMCSR_BrSupExt;		// 0x46
    u8   PowManagData;		// 0x47
    u8   HotSwapID;			// 0x48
    u8   HotSwapNextCapPtr;	// 0x49
    u16  HotSwapCtrlStat;	// 0x4A
    u8   VPD_ID;				// 0x4C
    u8   VPD_NextCapPtr;		// 0x4D
	PVPDAD  VPD_Address;		// 0x4E
    u32   VPD_Data;			// 0x50
} PLX9656_PCI_CONFIGURATION_REGISTERS, *PPLX9656_PCI_CONFIGURATION_REGISTERS;

#define PCI_CONFIG_REG_OFFSET(reg)   FIELD_OFFSET(PLX9656_PCI_CONFIGURATION_REGISTERS, reg)

typedef union _MODEDMAARBITR {
    u32 AsWhole; //				        (R-W-Rst)
	struct {
        u32 LatencyTimer       :8; // 7:0   (1-1-0): Local Bus Latency Timer
        u32 PauseTimer         :8; // 15:8  (1-1-0): Local Bus Pause Timer
        u32 LatencyTimerEna    :1; // 16    (1-1-0):1 Pause Timer Latency Timer Enable
        u32 PauseTimerEna      :1; // 17    (1-1-0):1 Local Bus Pause Timer Enable
        u32 BREQiEna			:1; // 18    (1-1-0):1 Local Bus BREQ Enable
        u32 DmaChanPrior		:2; // 20:19 (1-1-0): DMA Channel Priority
        u32 DirSlaveRlsBusMode :1; // 21    (1-1-1): Local Bus Direct Slave Releasу Bus Mode
        u32 DirSlaveLOCKEna    :1; // 22    (1-1-0):1 Direct Slave LOCK# Enable
        u32 PCIReqMode			:1; // 23    (1-1-0): PCI Request Mode
        u32 DelayedRdMode		:1; // 24    (1-1-0): Delayed Read Mode
        u32 PCIRdNoWrMode		:1; // 25    (1-1-0): PCI Read No Write Mode
        u32 PCIRdWrFlushMode	:1; // 26    (1-1-0): PCI Read With Write Flush Mode
        u32 GateLatTimerBREQ	:1; // 27    (1-1-0): Gate Local Bus Latency Timer With BREQi
        u32 PCIRdNoFlushMode	:1; // 28    (1-1-0): PCI Read No Flush Mode
        u32 SubsystemRet		:1; // 29    (1-1-0):1 Subsystem ID and Verdor ID returns
        u32 FifoFullFlag		:1; // 30    (1-1-0): Fifo Full Status Flag
        u32 BigendWaitSel		:1; // 31    (1-1-0): BIGEND#/WAIT Input/Output Select (M mode only - our not use)
	} ByBits;		//
} MODEDMAARBITR, *PMODEDMAARBITR;	

typedef union _LOCALBUSREGDESCR {
    u32 AsWhole; //				        (R-W-Rst)
	struct {
        u32 BusWidth			:2; // 1:0   (1-1-3): Memory Space 0 Local Bus Width
        u32 WaitStates         :4; // 5:2   (1-1-0): Memory Space 0 Internal Wait States (0-15)
        u32 TAREADYEna		    :1; // 6     (1-1-1):1 Memory Space 0 TA#/READY# Input Enable
        u32 BTERMEna			:1; // 7     (1-1-0):1 Memory Space 0 BTERM# Input Enable
        u32 PrefetchDis		:1; // 8     (1-1-0):1 Memory Space 0 Prefetch Disable
        u32 EromPrefetchDis	:1; // 9     (1-1-0):1 Expansion ROM Space Prefetch Disable
        u32 PrefetchCntEna		:1; // 10    (1-1-0):1 Prefetch Counter Enable
        u32 PrefetchCounter	:4; // 14-11 (1-1-0):1 Prefetch Counter
        u32 Reserved           :1; // 15	 (1-0-0): reserved
        u32 EromBusWidth		:2; // 17:16 (1-1-3): Expansion ROM Space Local Bus Width
        u32 EromWaitStates     :4; // 21:18 (1-1-0): Expansion ROM Space Internal Wait States (0-15)
        u32 EromTAREADYEna		:1; // 22    (1-1-1):1 Expansion ROM Space TA#/READY# Input Enable
        u32 EromBTERMEna		:1; // 23    (1-1-0):1 Expansion ROM Space BTERM# Input Enable
        u32 BurstEna		    :1; // 24    (1-1-0):1 Memory Space 0 Burst Enable
        u32 ExtraLongEEPROM	:1; // 25    (1-1-0):1 Extra Long Load from Serial EEPROM
        u32 EromBurstEna		:1; // 26    (1-1-0):1 Expansion ROM Space Burst Enable
        u32 DirSlWrMode	    :1; // 27    (1-1-0): Direct Slave PCI Write Mode
        u32 DirSlDelayClk		:4; // 31-28 (1-1-4): Direct Slave Retry Delay Clocks
	} ByBits;		//
} LOCALBUSREGDESCR, *PLOCALBUSREGDESCR;

    // Local Configuration Registers
typedef struct _PLX9656_LOCAL_CONFIGURATION_REGISTERS {
    u32   AddressSpace0Range;                 // Range For PCI-to-Local Address Space 0
    u32   LocalBaseAddressPCItoLocalAddr0;    // Local Base Address (Remap) for PCI-to-Local Address Space 0
//    u32   ModeDMAArbitration;                 // Mode/DMA Arbitration
	MODEDMAARBITR ModeDMAArbitration;           // Mode/DMA Arbitration
    u8   BigLittleEndianDescriptor;          // Big/Little Endian Descriptor
    u8   LocalMiscellaneousControl1;         // Local Miscellaneous Control 1
    u8   SerialEEPROMAddressBoundry;         // Serial EEPROM WRite-Protected Address Boundary
    u8   LocalMiscellaneousControl2;         // Local Miscellaneous Control 2
    u32   RangeForPCITOLocalExpansionRom;     // Range for PCI-to-Local Expansion ROM
    u32   LocalBaseAddressForPCIToLocalRom;   // Local Base Address (Remap) for PCI-to-Local Epansion ROM
//    u32   LocalBusRegionDescriptor;           // Local Bus Region Descriptors (Space 0 and Expansion ROM) for PCI-to-Local Accesses
    LOCALBUSREGDESCR LocalBusRegionDescriptor; // Local Bus Region Descriptors (Space 0 and Expansion ROM) for PCI-to-Local Accesses
    u32   RangeForDirectMasterToPCI;          // Range for DirectMaster-to-PCI
    u32   LocalBaseAddrDMasterToPCIMemory;    // Local Base Address for Direct Master to PCI Memory
    u32   LocalBaseAddrDMasterToPCIIOConfig;  // Local Base Address Addresss for Direct Master-to-PCI I/O Configuration
    u32   PCIBaseAddressDMasterToPCI;         // PCI Base Address (Remap) for Direct Master-to-PCI
    u32   PCIConfigAddrDMasterToPCIIOConfig;  // PCI Configuration Address Register for Direct Master-to-PCI I/O Configuration
    u32   Unused[0x30];
    u32   RangeForPCIToLocalAddressSpace1;    // Range For PCI-to-Local Address Space 1
    u32   LocalBaseAddressPCItoLocalAddr1;    // Local Base Address (Remap) for PCI-to-Local Address Space 1
    u32   LocalBusRegionsDescPCIToLocal;      // Local Bus REgion Descriptor (Space 1) for PCI-to-Local Accesses
    u32   PCIBaseDualAddressCycle;            // PCI Base Dual Address Cycle (Remap) for Direct Master-to-PCI (Upper 32 Bits)
    u32   PCIArbiterControl;                  // PCI Arbiter Control (7:0)
    u32   PCIAbortAddress;                    // PCI Abort Address
} PLX9656_LOCAL_CONFIGURATION_REGISTERS, *PPLX9656_LOCAL_CONFIGURATION_REGISTERS;

#define LOCAL_CONFIG_REG_OFFSET(reg)   FIELD_OFFSET(PLX9656_LOCAL_CONFIGURATION_REGISTERS, reg)

    // Runtime Registers

typedef union _INTCSR {
    u32 AsWhole; //								  (R-W-Rst)
	struct {
        u32 EnaLocalBus                    :1; // 0  (1-1  -0)
        u32 EnaLocalBusParErr              :1; // 1  (1-1  -0)
        u32 GenerPCIBus                    :1; // 2  (1-1  -0)
        u32 MailboxIntrEna                 :1; // 3  (1-1  -0)
        u32 PowerManageIntrEna             :1; // 4  (1-1  -0)
        u32 PowerManageIntr                :1; // 5  (1-1/0-0)
        u32 enaWrDirSlaveRdLocDataParChErr :1; // 6  (1-1  -0)
        u32 statWrDirSlaveRdLocDataParChErr:1; // 7  (1-1/0-0)
        u32 PciIntrEna                     :1; // 8  (1-1  -1):1 enables PCI interrupts.
        u32 PciDoorIntrEna                 :1; // 9  (1-1  -0)
        u32 PciAbortIntrEna                :1; // 10 (1-1  -0)
        u32 LocalIntrInpEna                :1; // 11 (1-1  -0)
        u32 RetryAbortEna                  :1; // 12 (1-1  -0)
        u32 PciDoorIntrActive              :1; // 13 (1-0  -0)
        u32 PciAbortIntrActive             :1; // 14 (1-0  -0)
        u32 LocalInpIntrActive             :1; // 15 (1-0  -0)
        u32 LocalIntrOutEna                :1; // 16 (1-1  -1)
        u32 LocalDoorIntrEna               :1; // 17 (1-1  -0)
        u32 LocalDmaCh0IntrEna             :1; // 18 (1-1  -0)
        u32 LocalDmaCh1IntrEna             :1; // 19 (1-1  -0)
        u32 LocalDoorIntrActive            :1; // 20 (1-0  -0)
        u32 DmaCh0IntrActive               :1; // 21 (1-0  -0)
        u32 DmaCh1IntrActive               :1; // 22 (1-0  -0)
        u32 BISTIntrActive                 :1; // 23 (1-0  -0)
        u32 DirMasterWasBusMaster          :1; // 24 (1-0  -1)
        u32 DmaCh0wasBusMaster             :1; // 25 (1-0  -1)
        u32 DmaCh1wasBusMaster             :1; // 26 (1-0  -1)
        u32 TargetAbortWasAsserted         :1; // 27 (1-0  -1)
        u32 PciBusWroteDataMBOX0           :1; // 28 (1-0  -0)
        u32 PciBusWroteDataMBOX1           :1; // 29 (1-0  -0)
        u32 PciBusWroteDataMBOX2           :1; // 30 (1-0  -0)
        u32 PciBusWroteDataMBOX3           :1; // 31 (1-0  -0)
	} ByBits;		//
} INTCSR, *PINTCSR;	


typedef union _CNTRL {
    u32 AsWhole; //								 (R-W  -Rst)
	struct {
        u32 PciReadCmdCodeDma         :4; //  3: 0  (1-1  -1110)
        u32 PciWriteCmdCodeDma        :4; //  7: 4  (1-1  -0111)
        u32 PciMemReadCmdCoreDirMstr  :4; // 11: 8  (1-1  -0110)
        u32 PciMemWriteCmdCoreDirMstr :4; // 15:12  (1-1  -0111)
        u32 GeneralPurposeOutput      :1; // 16     (1-1  -1)
        u32 GeneralPurposeInput       :1; // 17     (1-0  --)
        u32 USERi                     :1; // 18     (1-1  -1)
        u32 USERo                     :1; // 19     (1-1  -1)
        u32 LINToIntrStatus           :1; // 20     (1-1/0-0)
        u32 TEA_LSERR_IntrStatus      :1; // 21     (1-1/0-0)
        u32 Reserved                  :2; // 23:22  (1-0  -00)
        u32 AssertsEEPROMclk          :1; // 24     (1-1  -0)
        u32 EEPROMchipSelect          :1; // 25     (1-1  -0)
        u32 WriteBitToEEPROM          :1; // 26     (1-1  -0)
        u32 ReadBitFromEEPROM         :1; // 27     (1-0  --)
        u32 ProgEEPROMpresent         :1; // 28     (1-0  -0)
        u32 ReloadConfigRegister      :1; // 29     (1-1  -0)
        u32 PciSoftReset              :1; // 30     (1-1  -0)
        u32 EEDOInputEna              :1; // 31     (1-0  -0)
	} ByBits;		//
} CNTRL, *PCNTRL;	




typedef struct _PLX9656_RUNTIME_REGISTERS {
    u32   Unused0[0x10];
    u32   MailboxRegister0InQueue;            // 0x40: Mailbox Register 0 / Inbound Queue
    u32   MailboxRegister1OutQueue;           // Mailbox Register 1 / Outbound Queue
    u32   MailboxRegister2;                   // Mailbox Register 2
    u32   MailboxRegister3;                   // Mailbox Register 3
    u32   MailboxRegister4;                   // Mailbox Register 4
    u32   MailboxRegister5;                   // Mailbox Register 5
    u32   MailboxRegister6;                   // Mailbox Register 6
    u32   MailboxRegister7;                   // Mailbox Register 7
    u32   PCIToLocalDoorbellRegister;         // PCI-to-Local Doorbell Register
    u32   LocalToPCIDoorbellRegister;         // Local-to-PCI Doorbell Register
    INTCSR  IntrControlStatus;                  // Interrupt Control Status
    CNTRL   CommandControl;                     // Serial EEPROM Control, PCI Command Codes, User I/O Control, and Init Control
    u16  VendorID;                           // Vendor ID
    u16  DeviceID;                           // Device ID
    u16  RevisionID;                         // Revision ID
    u16  Unused1;
    u32   MailboxRegister0;                   // Mailbox Register 0
    u32   MailboxRegister1;                   // Mailbox Register 1
} PLX9656_RUNTIME_REGISTERS, *PPLX9656_RUNTIME_REGISTERS;

#define RUNTIME_REG_OFFSET(reg)   FIELD_OFFSET(PLX9656_RUNTIME_REGISTERS, reg)


    // DMA Registers

typedef union _DMAMODE {
    u32 AsWhole; //				        (R-W-Rst)
	struct {
        u32 LocalBusWidth      :2; // 1:0  (1-1-11):bus width 00 - 8, 01 - 16, 10/11 - 32 bits
        u32 InternalWaitState  :4; // 5:2  (1-1-0000)
        u32 TA_READYinputEna   :1; // 6    (1-1-0):
        u32 BTERMinputEna      :1; // 7    (1-1-1):1 enables BTERM# input.
        u32 LocalBurstEna      :1; // 8    (1-1-0):1 enables Local bursting.
        u32 ScatterGatherMode  :1; // 9    (1-0-0):1 enables Scatter/Gather mode.
        u32 DoneIntrEna        :1; // 10   (1-1-0):1 enables an interrupt when done.
        u32 LocalAddrConst     :1; // 11   (1-1-0):1 holds the Local Address bus constant.
        u32 DemandMode         :1; // 12   (1-1-0):1 causes the DMA controller to operate in Demand mode.
        u32 MemWriteInvMode    :1; // 13   (1-1-0):
        u32 DMA_EOTenable      :1; // 14   (1-1-0):
        u32 FastSlowTermMode   :1; // 15   (1-1-0):
        u32 DmaClearCountMode  :1; // 16   (1-1-0):
        u32 DmaChanIntrSelect  :1; // 17   (1-1-0):1 routes the DMA Channel interrupt to the PCI Bus interrupt.
        u32 DACChainLoad       :1; // 18   (1-1-0):
        u32 EOTEndLink         :1; // 19   (1-1-0):
        u32 ValidModeEna       :1; // 20   (1-1-0):
        u32 ValidStopCtrl      :1; // 21   (1-1-0):
        u32 Reserved          :10; // 31:22(1-0-0):
	} ByBits;		//
} DMAMODE, *PDMAMODE;	

enum LOCAL_BUS_WIDTH {
	LBW_8bit,
	LBW_16bit,
	LBW_32bitToo,
	LBW_32bit,
};




typedef union _DMADPR {
    u32 AsWhole; //					     (R-W-Rst)
	struct {//                               ---------
        u32 DptrLocationPCI    :1;  // 0    (1-1-0) Descriptor Location: 1 = PCI
        u32 EndOfChain         :1;  // 1    (1-1-0)
        u32 IntrAfterTC        :1;  // 2    (1-1-0)
        u32 DirFromLocalBus    :1;  // 3    (1-1-0)
//#if !defined (WIN32) && !defined (_WIN32_)
//		u32 NextAddr0          :12; // 15:4 (1-1-0)
//		u32 NextAddr1          :16; // 31:15(1-1-0)
//#else
        u32 NextAddr           :28; // 31:4 (1-1-0)
//#endif
	} ByBits;		//
} DMADPR, *PDMADPR;	

typedef union _DMACSR {
    u8 AsWhole;	//				  (R-W  -Rst)
	struct {//                       ------------- 
        u8 ChanEna       :1; // 0   (1-1  -0):1 enables channel to transfer data.
        u8 ChanStart     :1; // 1   (0-1/1-0):1 causes the channel to start transferring data if the channel is enabled.
        u8 ChanAbort     :1; // 2   (0-1/1-0):1 causes the channel to abort current transfer.
        u8 ChanClrIntr   :1; // 3   (0-1/0-0):1 clears Channel interrupts.
        u8 ChanDone      :1; // 4   (1-0  -1):1 indicates a channel transfer is complete.
        u8 Reserved      :3; // 7:5 (1-0  -000)
	} ByBits;		//
} DMACSR, *PDMACSR;	


typedef struct _PLX9656_DMA_REGISTERS {
    u32   Unused0[0x20];
    DMAMODE DMACh0Mode;                         // 0x80: DMA Channel 0 Mode
    u32   DMACh0PCIAddress;                   // DMA Channel 0 PCI Address
    u32   DMACh0LocalAddress;                 // DMA Channel 0 Local Address
    u32   DMACh0TransferByteCount;            // DMA Channel 0 Transfer Byte Count
    DMADPR  DMACh0DescriptorPointer;            // DMA Channel 0 Descpritor Pointer
    DMAMODE DMACh1Mode;                         // DMA Channel 1 Mode
    u32   DMACh1PCIAddress;                   // DMA Channel 1 PCI Address
    u32   DMACh1LocalAddress;                 // DMA Channel 1 Local Address
    u32   DMACh1TransferByteCount;            // DMA Channel 1 Transfer Byte Count
    DMADPR  DMACh1DescriptorPointer;            // DMA Channel 1 Descpritor Pointer
    DMACSR  DMAch0CommandStatus;                // DMA Channel 0 Command Status
    DMACSR  DMAch1CommandStatus;                // DMA Channel 1 Command Status
    u16  Reserved;
    u32   ModeDMAArbitration;                 // Mode/DMA Arbitration
    u32   DMAThreshold;                       // DMA Threshold
    u32   DMACh0PCIDualAddressCycle;          // DMA Channel 0 Dual Address Cycle (Upper 32 Bits)
    u32   DMACh1PCIDualAddressCycle;          // DMA Channel 1 Dual Address Cycle (Upper 32 Bits)
} PLX9656_DMA_REGISTERS, *PPLX9656_DMA_REGISTERS;

#define PLX_MAX_TRANSFER_LENGTH   0x400000 // 0 - 22 bits = 0x7FFFFF rounded to 0x400000

#define DMA_REG_OFFSET(reg)   FIELD_OFFSET(PLX9656_DMA_REGISTERS, reg)

    // Messaging Queue Registers
typedef struct _PLX9656_MESSAGING_QUEUE_REGISTERS {
    u32   Unused0[0xC];
    u32   OutboundPostQueueInterruptStatus;   // 0x30: Outbound Post Queue Interrupt Status
    u32   OutboundPostQueueInterruptMask;     // Outbount Post Queue Interrupt Mask
    u32   Unused1[0x2];                       //
    u32   InboundQueuePort;                   // 0x40: Inbound Queue Port
    u32   OutboundQueuePort;                  // Outbound Queue Port
    u32   Unused2[0x1F];                      //
    u32   MessagingUnitConfiguration;         // 0xC0: Messaging Unit Configuration
    u32   QueueBaseAddress;                   // Queue Base Address
    u32   InboundFreeHeadPointer;             // Inbound Free Head Pointer
    u32   InboundFreeTailPointer;             // Inbound Free Tail Pointer
    u32   InboundPostHeadPointer;             // Inbound Post Head Pointer
    u32   InboundPostTailPointer;             // Inbound Post Tail Pointer
    u32   OutboundFreeHeadPointer;            // Outbound Free Head Pointer
    u32   OutboundFreeTailPointer;            // Outbound Free Tail Pointer
    u32   OutboundPostHeadPointer;            // Outbound Post Head Pointer
    u32   OutboundPostTailPointer;            // Outbound Post Tail Pointer
    u32   QueueStatusControl;                 // Queue Status Control
} PLX9656_MESSAGING_QUEUE_REGISTERS, *PPLX9656_MESSAGING_QUEUE_REGISTERS;

    // command codes
#define PLX9656_PCI_CR_MASTER   0x00000004      // Enable PCI Bus Master

#define PLX9656_PCI_SR_SIG_TAR  0x00000800      // 9656 Signaled Target Abort
#define PLX9656_PCI_SR_REC_TAR  0x00001000      // Recieved Target Abort
#define PLX9656_PCI_SR_REC_MSTR 0x00002000      // Recieved Master Abort
#define PLX9656_PCI_SR_SYS_ERR  0x00004000      // Signaled System Error

#define PLX9656_END_BE_DSLV0    0x00000004      // Direct Slave Address Space 0 Big Endian
#define PLX9656_END_BE_DDLV1    0x00000020      // Direct Slave Address Space 1 Big Endian
#define PLX9656_END_BE_BDMA1    0x00000040      // DMA Channel 1 Big Endian
#define PLX9656_END_BE_BDMA0    0x00000080      // DMA Channel 0 Big Endian


#define PLX9656_INT_CSR_ENABLE  0x00000100      // Enable PCI Interupt
#define PLX9656_INT_CSR_MABORT  0x00000400
#define PLX9656_INT_CSR_DMA0    0x00040000      // Enable DMA Channel 0 Interupt
#define PLX9656_INT_CSR_DMA1    0x00080000      // Enable DMA Channel 1 Interupt
#define PLX9656_INT_CSR_ACTV0   0x00200000      // DMA Channel 0 Active Interrupt
#define PLX9656_INT_CSR_ACTV1   0x00400000      // DMA Channel 1 Active Interrupt

#define PLX9656_DMA_CSR_ENABLE  0x00000001      // Enables Transfer
#define PLX9656_DMA_CSR_START   0x00000002      // Starts Transfer
#define PLX9656_DMA_CSR_ABORT   0x00000004      // Abort Transfer
#define PLX9656_DMA_CSR_CLEAR   0x00000008      // Clear Interupt
#define PLX9656_DMA_CSR_DONE    0x00000010      // Transfer Complete

#define PLX9656_DMA_DPR_LOC     0x00000001      // Descriptor Location: 1 = PCI 
#define PLX9656_DMA_DPR_EOC     0x00000002      // End of Chain
#define PLX9656_DMA_DPR_INT     0x00000004      // Interupt after Terminal Count
#define PLX9656_DMA_DPR_DIR     0x00000008      // Direction of Transfer: 1 = From Local Bus to PCI Bus

#define PLX9656_DMA_MODE_32     0x00000003      // 32-bit local bus width
#define PLX9656_DMA_MODE_ESG    0x00000200      // Enable Scatter/Gather
#define PLX9656_DMA_MODE_EINT   0x00000400      // Enable Interupt when Done
#define PLX9656_DMA_MODE_BUSC   0x00000800      // Hold local bus constant
#define PLX9656_DMA_MODE_CLR    0x00010000      // Clear count in descriptor
#define PLX9656_DMA_MODE_INTS   0x00020000      // Interupt Select: 1 = Interumpt to PCI Bus

    // masks
#define PLX9656_DMA_DPR_LOW     0x0000000F      // & to check low bits
#define PLX9656_DMA_SIZE_LOW    0x007FFFFF      // & to check low bits
#define PLX9656_DMA_SIZE_HIGH   0xFF800000      // & to check high bits

#endif // _PLX9x56_H_
