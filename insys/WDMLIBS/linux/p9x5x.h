/*******************************************************
 *      Copyright (c) 1999 InSys Corp.
 *
 *      ===== P9X5X.h =====
 *      PLX PCI 9X5X Definitions
 */

#ifndef __P9X5X_H_
#define __P9X5X_H_

#define BZERO(buf) memset(&(buf),0,sizeof(buf))

enum {DMA_PAGES=256};

typedef struct
{
    u32     hDma;             // handle of dma buffer
    void*     pUserAddr;        // begining of buffer
    u32     dwBytes;          // size of buffer
    u32     dwOptions;        // allocation options: mast be 0
    u32     dwPages;          // number of pages in buffer

    struct {
        void* pPhysicalAddr;    // physical address of page
        u32 dwBytes;          // size of page
    } Page[DMA_PAGES];
} DMA;

// PLX register definitions
enum {
    P9X5X_LAS0RR     = 0x00,
    P9X5X_LAS0BA     = 0x04,
    P9X5X_EROMRR     = 0x10,
    P9X5X_EROMBA     = 0x14,
    P9X5X_LBRD0      = 0x18,
    P9X5X_MBOX0      = 0x40,
    P9X5X_MBOX1      = 0x44,
    P9X5X_MBOX2      = 0x48,
    P9X5X_MBOX3      = 0x4c,
    P9X5X_MBOX4      = 0x50,
    P9X5X_MBOX5      = 0x54,
    P9X5X_MBOX6      = 0x58,
    P9X5X_MBOX7      = 0x5c,
    P9X5X_P2LDBELL   = 0x60,
    P9X5X_L2PDBELL   = 0x64,
    P9X5X_INTCSR     = 0x68,
    P9X5X_CNTRL      = 0x6c,

    P9X5X_LAS1RR     = 0xF0,
    P9X5X_LAS1BA     = 0xF4,
    P9X5X_LBRD1      = 0xF8

};

// P9X5X local registers
enum {
    P9X5X_DMAMODE    = 0x80,
    P9X5X_DMAPADR    = 0x84,
    P9X5X_DMALADR    = 0x88,
    P9X5X_DMASIZ     = 0x8c,
    P9X5X_DMADPR     = 0x90,
    P9X5X_DMACSR     = 0xA8
};

enum { P9X5X_DMA_CHANNEL_SHIFT = 0x14 }; // shift in address between channels 0 and 1 of DMA

typedef enum
{
    P9X5X_DMA_CHANNEL_0 = 0,
    P9X5X_DMA_CHANNEL_1 = 1
} P9X5X_DMA_CHANNEL;

typedef struct {
    DMA dma;
    DMA dmaList;
    P9X5X_DMA_CHANNEL dmaChannel;
} P9X5X_DMA_STRUCT, *P9X5X_DMA_HANDLE;


#define P9X5X_MODE_DESC        0xF9000140
#define P9X5X_MODE_DESC_BYTE   0x00000000
#define P9X5X_MODE_DESC_WORD   0x00010001
#define P9X5X_MODE_DESC_u32  0x00030003

typedef enum
{
    P9X5X_MODE_BYTE   = 0,
    P9X5X_MODE_WORD   = 1,
    P9X5X_MODE_u32  = 2
} P9X5X_MODE;

typedef enum
{
    P9X5X_ADDR_REG0    = 0,
    P9X5X_ADDR_REG1    = 1,
    P9X5X_ADDR_SPACE0  = 2,
    P9X5X_ADDR_SPACE1  = 3,
    P9X5X_ADDR_SPACE2  = 4,
    P9X5X_ADDR_SPACE3  = 5,
    P9X5X_ADDR_EPROM   = 6
} P9X5X_ADDR;

enum { P9X5X_RANGE_REG = 0x00000080 };

typedef struct P9X5X_STRUCT *P9X5X_HANDLE;
/*
typedef struct
{
    u32 dwCounter;   // number of interrupts received
    u32 dwLost;      // number of interrupts not yet dealt with
    BOOL fStopped;     // was interrupt disabled during wait
    u32 dwStatusReg; // value of status register when interrupt occured
} P9X5X_INT_RESULT;
*/
//typedef void (WINAPI *P9X5X_INT_HANDLER)( P9X5X_HANDLE hPlx, P9X5X_INT_RESULT *intResult);
/*
typedef struct
{
    WD_INTERRUPT Int;
//    HANDLE hThread;
    WD_TRANSFER Trans[2];
//    P9X5X_INT_HANDLER funcIntHandler;
} P9X5X_INTERRUPT;
*/
typedef struct
{
    u32 dwLocalBase;
    u32 dwMask;
    u32 dwBytes;
    u32 dwAddr;
    u32 dwAddrDirect;
    int  fIsMemory;
} P9X5X_ADDR_DESC;

typedef struct P9X5X_STRUCT
{
//    WD_CARD cardLock;
//    WD_PCI_SLOT pciSlot;
//    WD_CARD_REGISTER cardReg;
    P9X5X_ADDR_DESC addrDesc[6];
    u32  addrSpace;
//    int   fUseInt;
//    P9X5X_INTERRUPT Int;
    u32  modeDesc[3];
} P9X5X_STRUCT;

// options for PLX_Open
enum { P9X5X_OPEN_USE_INT =   0x1 };
enum { P9X5X_OPEN_USE_DMA =   0x2 }; // not yet implemented

// this string is set to an error message, if one occurs
extern char P9X5X_ErrorString[];

typedef struct _PCI_9X5X_CONFIG {
    unsigned short  VendorID;                   // (ro)
    unsigned short  DeviceID;                   // (ro)
    unsigned short  Command;                    // Device control
    unsigned short  Status;
    unsigned char   RevisionID;                 // (ro)
    unsigned char   ProgIf;                     // (ro)
    unsigned char   SubClass;                   // (ro)
    unsigned char   BaseClass;                  // (ro)
    unsigned char   CacheLineSize;              // (ro+)
    unsigned char   LatencyTimer;               // (ro+)
    unsigned char   HeaderType;                 // (ro)
    unsigned char   BIST;                       // Built in self test

    unsigned long   BaseAddresses[6];
    unsigned long   CIS;
    unsigned short  SubVendorID;
    unsigned short  SubSystemID;
    unsigned long   ROMBaseAddress;
    unsigned long   Reserved2[2];
    unsigned char   InterruptLine;      //
    unsigned char   InterruptPin;       // (ro)
    unsigned char   MinimumGrant;       // (ro)
    unsigned char   MaximumLatency;     // (ro)

    unsigned char   PM_CapID;
    unsigned char   PM_NextCapPointer;
    unsigned short  PM_Cap;
    unsigned short  PM_CS;
    unsigned char   PM_BridgeExt;
    unsigned char   PM_Data;

    unsigned char   HS_Ctrl;
    unsigned char   HS_NextCapPointer;
    unsigned char   HS_CS;
    unsigned char   HS_Res;

    unsigned char   VPD_Ctrl;
    unsigned char   VPD_NextCapPointer;
    unsigned short  VPD_Addr;
    unsigned long   VPD_Data;

    unsigned char   DeviceSpecific[172];

} PCI_9X5X_CONFIG, *PPCI_9X5X_CONFIG;

    // number of mailboxes
#define MAX_MAILBOX_REGISTERS   8

    // Shared Runtime Registers
typedef struct _PLX_SHARED_RUNTIME_REGISTERS
{
    u32   Mailbox[MAX_MAILBOX_REGISTERS]; // 0x40 - mailbox registers
    u32   PciToLocalDoorbell;             // 0x60 - PCI to Local Doorbell
    u32   LocalToPciDoorbell;             // 0x64 - Local to PCI doorbell
    u32   InterruptControl;               // 0x68 - Interrupt control/status
    u32   Control;                        // 0x6C - EEPROM control, PCI command
                                            //  Codes, User I/O control,
                                            //  Init control
    u16  VendorID;                       // 0x70
    u16  DeviceID;                       // 0x72
    u16  RevisionID;                     // 0x74
    u16  Unused;                         // 0x76
    u32   MailboxI2O[2];                  // 0x78 - mailbox registers 0, 1
                                            // for I2O enable
} PLX_SHARED_RUNTIME_REGISTERS, *PPLX_SHARED_RUNTIME_REGISTERS;

    // PLX Messaging Queue Registers
typedef struct _PLX_MESSAGING_REGISTERS1
{
    u32   InterruptStatus;                // 0x30 - Outbound Post Queue
                                            // Interrupt Status
    u32   InterruptMask;                  // 0x34 - Outbound Post Queue
                                            // Interrupt Mask
    u32   Reserved[2];                    // 0x38..0x3C
} PLX_MESSAGING_REGISTERS1, *PPLX_MESSAGING_REGISTERS1;

typedef struct _PLX_MESSAGING_REGISTERS2
{
    u32   InboundPort;                    // 0x40 - Inbound Queue Port
    u32   OutboundPort;                   // 0x44 - Outbound Queue Port
} PLX_MESSAGING_REGISTERS2, *PPLX_MESSAGING_REGISTERS2;

typedef struct _PLX_MESSAGING_REGISTERS3
{
    u32   UnitConfiguration;              // 0xC0 - Messaging Unit
                                            // Configuration
    u32   BaseAddress;                    // 0xC4 - Queue Base Address
    u32   InFreeHeadPtr;                  // 0xC8 - Inbound Free Header
                                            // Pointer
    u32   InFreeTailPtr;                  // 0xCC - Inbound Free Tail
                                            // Pointer
    u32   InPostHeadPtr;                  // 0xD0 - Inbound Post Header
                                            // Pointer
    u32   InPostTailPtr;                  // 0xD4 - Inbound Post Tail
                                            // Pointer
    u32   OutFreeHeadPtr;                 // 0xD8 - Outbound Free Header
                                            // Pointer
    u32   OutFreeTailPtr;                 // 0xDC - Outbound Free Tail
                                            // Pointer
    u32   OutPostHeadPtr;                 // 0xE0 - Outbound Post Header
                                            // Pointer
    u32   OutPostTailPtr;                 // 0xE4 - Outbound Post Tail
                                            // Pointer
    u32   Reserved[2];                    // 0xE8..0xEC

} PLX_MESSAGING_REGISTERS3, *PPLX_MESSAGING_REGISTERS3;

    // Local DMA Registers
typedef struct _PLX_DMA_REGISTERS
{
    u32   Channel0Mode;                   // 0x80 - channel 0 mode
    u32   Channel0PciAddress;             // 0x84 - channel 0 PCI address
    u32   Channel0LocalAddress;           // 0x88 - channel 0 Local address
    u32   Channel0TransferCount;          // 0x8C - channel 0 Transfer byte count
    u32   Channel0DescriptorPointer;      // 0x90 - channel 0 Descriptor pointer
    u32   Channel1Mode;                   // 0x94 - channel 1 mode
    u32   Channel1PciAddress;             // 0x98 - channel 1 PCI address
    u32   Channel1LocalAddress;           // 0x9C - channel 1 Local address
    u32   Channel1TransferCount;          // 0xA0 - channel 1 Transfer byte count
    u32   Channel1DescriptorPointer;      // 0xA4 - channel 1 Descriptor pointer
    u32   CommandStatus;                  // 0xA8 - command/status register
    u32   Arbitration0;                   // 0xAC - mODE/dma Arbitration
    u32   Threshold;                      // 0xB0 - DMA Threshold
    u32   Channel0DualAddress;            // 0xB4 - DMA Ch 0 PCI Dual
                                            // Address Cycle (Upper 32 bits)
    u32   Channel1DualAddress;            // 0xB8 - DMA Ch 1 PCI Dual
                                            // Address Cycle (Upper 32 bits)
    u32   Reserved;

} PLX_DMA_REGISTERS, *PPLX_DMA_REGISTERS;

//
//  TO DO: add the cards registers which exist in the boards local address space
//  the local bus register definitions below must be changed to match
//
//typedef struct _USER_READ_REGISTERS
//{
//    u32   InterruptEnable;
//    .....
//} USER_READ_REGISTERS, *PUSER_READ_REGISTERS;
typedef struct _USER_WRITE_REGISTERS
{
    u32   InterruptClear;                // imaginary interrupt clear regsiter
//    .....
} USER_WRITE_REGISTERS, *PUSER_WRITE_REGISTERS;



    // Local address ranges
    //   PLX DMA registers are located at offset 0x100 from here
#define LOCAL_PLX_BASE_REGISTERS    0x20000000
    //   User and DMA registers local address
#define LOCAL_USER_BASE_REGISTERS   0x80000000
    // PLX local bus address
#define LOCAL_BUS_ADDRESS           0xFFFFFE00

    // PLX Registers
    //      BusRegionDescriptor bit defines
#define BRD_MEMORY_32BITS   0x00000003
#define BRD_MEMORY_16BITS   0x00000001
#define BRD_MEMORY_8BITS    0x00000000
#define BRD_MEMORY_WAIT0    0x00000000
#define BRD_MEMORY_WAIT1    0x00000004
#define BRD_MEMORY_WAIT2    0x00000008
#define BRD_MEMORY_READY_INPUT_ENABLE 0x00000040
#define BRD_MEMORY_PREFETCH_DISABLE 0x00000100
#define BRD_ROM_PREFETCH_DISABLE 0x00000200
#define BRD_ROM_32BITS      0x00030000
#define BRD_ROM_WAIT2       0x00080000
#define BRD_ROM_WAIT3       0x000C0000
#define BRD_CLOCK32         0x40000000
    //      AddressSpace0LocalBaseAddress
#define LBA0_SPACE_ENABLE   0x00000001
    //      InterruptControl
#define INT_PCI_INT_ENABLE                  0x00000100
#define INT_PCI_DOORBELL_ENABLE             0x00000200
#define INT_PCI_LOCAL_INTERRUPT_ENABLE      0x00000800
#define INT_LOCAL_INTERRUPT_ACTIVE          0x00008000
#define INT_LOCAL_INTERRUPT_ENABLE          0x00010000
#define INT_DOORBELL_INTERRUPT_ENABLE       0x00020000
#define INT_CHANNEL0_DMA_INTERRUPT_ENABLE   0x00040000
#define INT_CHANNEL1_DMA_INTERRUPT_ENABLE   0x00080000
#define INT_DOORBELL_INTERRUPT_ACTIVE       0x00100000
#define INT_CHANNEL0_DMA_INTERRUPT_ACTIVE   0x00200000
#define INT_CHANNEL1_DMA_INTERRUPT_ACTIVE   0x00400000
#define INT_BIST_INTERRUPT_ACTIVE           0x00800000
#define INT_ANY_INTERRUPT    (INT_LOCAL_INTERRUPT_ACTIVE        |\
                              INT_DOORBELL_INTERRUPT_ACTIVE     |\
                              INT_CHANNEL0_DMA_INTERRUPT_ACTIVE |\
                              INT_CHANNEL1_DMA_INTERRUPT_ACTIVE |\
                              INT_BIST_INTERRUPT_ACTIVE)

    // DMA Registers
    //   ChannelnMode
#define CM_8BITS                  0x00000000
#define CM_WAIT0                  0x00000000
#define CM_WAIT1                  0x00000004
#define CM_READY_ENABLE           0x00000040
#define CM_BURST_ENABLE           0x00000100
#define CM_CHAINING_ENABLE        0x00000200
#define CM_DONE_INTERRUPT_ENABLE  0x00000400
#define CM_LOCAL_ADDRESS_CONSTANT 0x00000800
#define CM_DEMAND_MODE            0x00001000

    //   CommandStatus
#define CS_CHANNEL0_ENABLE  0x00000001
#define CS_CHANNEL0_START   0x00000002
#define CS_CHANNEL0_ABORT   0x00000004
#define CS_CHANNEL0_CLEAR_INTERRUPTS 0x00000008
#define CS_CHANNEL0_DONE    0x00000010
#define CS_CHANNEL1_ENABLE  0x00000100
#define CS_CHANNEL1_START   0x00000200
#define CS_CHANNEL1_ABORT   0x00000400
#define CS_CHANNEL1_CLEAR_INTERRUPTS 0x00000800
#define CS_CHANNEL1_DONE    0x00001000
#define CS_CHANNEL0_READ_MASK CS_CHANNEL0_ENABLE
#define CS_CHANNEL1_READ_MASK CS_CHANNEL1_ENABLE


    //  ChannelnDescriptorPointer
#define CH_DESCRIP_CARD_TO_PCI  0x00000008
#define CH_DESCRIP_PCI_TO_CARD  0x00000000


    // User Registers
    //
    //  TO DO: add user register defines
    // INTERRUPTCLEAR_ALL is an example of clearing the
    // interrupts on the local bus side of the PLX. This value
    // and address needs to be modified for the actual hardware in
    // use
#define INTERRUPTCLEAR_ALL  (0xFF)



    // Configuration Registers 1
typedef struct PLX_LCR1
{
    u32   AS0_Range;             // 0x00 - Address Space 0 Range
    u32   AS0_LBA;               // 0x04 - Address Space 0 Local Base
    u32   Arbitration;           // 0x08 - Mode/DMA Arbitration
    u8   BLED;                  // 0x0C - Big/Little Endian Descriptor
    u8   LMC;                   // 0x0D - Local Miscellaneous Control
    u8   WP_EEPROM;             // 0x0E - Serial EEPROM Write-Protected
                                   // Address Boundary
    u8   Res1;                  // 0x0F
    u32   EXPROM_Range;          // 0x10 - Expansion ROM Range
    u32   EXPROM_LBA;            // 0x14 - Expansion ROM Local Base Address
    u32   BRD0;                  // 0x18 - Bus Region Descriptor
                                   // for PCI to Local Accesses
    u32   RR_DM;                 // 0x1C - Range Register
                                   // for Direct Master to PCI
    u32   BBA;                   // 0x20 - Bus Base Address Register
                                   // for Direct Master to PCI Memory
    u32   BA_DM;                 // 0x24 - Base Address
                                   // for Direct Master-to-PCI IO/CFG
    u32   PCI_BA_DM;             // 0x28 - PCI Base Address register
                                   // for Direct Master to PCI
    u32   PCI_CA_DM;             // 0x2C - PCI Configuration Address
                                   //  register for Direct Master to
                                   //  PCI IO/CFG
} PLX_LCR1;

    // Configuration Registers 2
typedef struct PLX_LCR2
{
    u32   AS1_Range;             // 0xF0 - Address Space 1 Range
    u32   AS1_LBA;               // 0xF4 - Address Space 1 Local Base
    u32   BRD1;                  // 0xF8 - Bus Region Descriptor for PCI
                                   // to Local Accesses Space 1
    u32   BDAC;                  // 0xFC - PCI Base Dual Address Cycle (Remap)
                                   // for Direct Master-to-PCI
                                   // (Upper 32 bits)
}PLX_LCR2;

    // Shared Runtime Registers
typedef struct PLX_RTR
{
    u32   Mailbox[MAX_MAILBOX_REGISTERS]; // 0x40 - mailbox registers
    u32   PciToLocalDoorbell;             // 0x60 - PCI to Local Doorbell
    u32   LocalToPciDoorbell;             // 0x64 - Local to PCI doorbell
    u32   InterruptControl;               // 0x68 - Interrupt control/status
    u32   Control;                        // 0x6C - EEPROM control, PCI command
                                            //  Codes, User I/O control,
                                            //  Init control
    u16  VendorID;                       // 0x70
    u16  DeviceID;                       // 0x72
    u16  RevisionID;                     // 0x74
    u16  Unused;                         // 0x76
    u32   MailboxI2O[2];                  // 0x78 - mailbox registers 0, 1
                                            // for I2O enable
} PLX_RTR;



    // PLX control Registers
typedef struct _PLX_CTL_REGS
{
    PLX_LCR1 Config1;           // configuration regsiters 1
                                               // (PCI offset 0x00 - 0x2C)
    PLX_MESSAGING_REGISTERS1           Msg1;   // messaging registers 1
                                               // (PCI offset 0x30 - 0x3C)
    PLX_SHARED_RUNTIME_REGISTERS       Runtime;// runtime registers
                                               // (PCI offset 0x40 - 0x6C)
    PLX_DMA_REGISTERS                  DMA;    // DMA Registers
                                               // (PCI offset 0x80 - 0xBC)
    PLX_MESSAGING_REGISTERS3           Msg3;   // messaging registers 1
                                               // (PCI offset 0xC0 - 0xEC)
    PLX_LCR2 Config2;                          // configuration regsiters 2
                                               // (PCI offset 0xF0 - 0xFC)

} PLX_CTL_REGS, *PPLX_CTL_REGS;


#define P9X5X_LOCAL_RESET        0x40000000
#define P9X5X_RELOAD_EEPROM      0x20000000

//
//      PLX PCI 9X5X Functions
//

u32   P9X5X_CountCards (u32 dwVendorID, u32 dwDeviceID);
int    P9X5X_Open (P9X5X_HANDLE *phPlx, u32 dwVendorID, u32 dwDeviceID, u32 nCardNum, u32 dwOptions);
void    P9X5X_Close (P9X5X_HANDLE hPlx);
int    P9X5X_IsAddrSpaceActive(P9X5X_HANDLE hPlx, P9X5X_ADDR addrSpace);
void    P9X5X_ReadWriteBlockLocal (P9X5X_HANDLE hPlx, u32 dwLocalAddr, void* buf,
                    u32 dwBytes, int fIsRead, P9X5X_MODE mode);
void    P9X5X_ReadBlockLocal (P9X5X_HANDLE hPlx, u32 dwLocalAddr, void* buf, u32 dwBytes, P9X5X_MODE mode);
void    P9X5X_WriteBlockLocal (P9X5X_HANDLE hPlx, u32 dwLocalAddr, void* buf, u32 dwBytes, P9X5X_MODE mode);
u8    P9X5X_ReadByteLocal (P9X5X_HANDLE hPlx, u32 dwLocalAddr);
void    P9X5X_WriteByteLocal (P9X5X_HANDLE hPlx, u32 dwLocalAddr, u8 data);
u16    P9X5X_Reau32Local (P9X5X_HANDLE hPlx, u32 dwLocalAddr);
void    P9X5X_WriteWordLocal (P9X5X_HANDLE hPlx, u32 dwLocalAddr, u16 data);
u32   P9X5X_Readu32Local (P9X5X_HANDLE hPlx, u32 dwLocalAddr);
void    P9X5X_Writeu32Local (P9X5X_HANDLE hPlx, u32 dwLocalAddr, u32 data);

u8    P9X5X_ReadByte (P9X5X_HANDLE hPlx, P9X5X_ADDR addrSpace, u32 dwOffset);
void    P9X5X_WriteByte (P9X5X_HANDLE hPlx, P9X5X_ADDR addrSpace, u32 dwOffset, u8 data);
u16    P9X5X_ReadWord (P9X5X_HANDLE hPlx, P9X5X_ADDR addrSpace, u32 dwOffset);
void    P9X5X_WriteWord (P9X5X_HANDLE hPlx, P9X5X_ADDR addrSpace, u32 dwOffset, u16 data);
u32   P9X5X_Readu32 (P9X5X_HANDLE hPlx, P9X5X_ADDR addrSpace, u32 dwOffset);
void    P9X5X_Writeu32 (P9X5X_HANDLE hPlx, P9X5X_ADDR addrSpace, u32 dwOffset, u32 data);
void    P9X5X_ReadBlock (P9X5X_HANDLE hPlx, u32 dwOffset, void* buf,
                    u32 dwBytes, P9X5X_ADDR addrSpace, P9X5X_MODE mode);
void    P9X5X_WriteBlock (P9X5X_HANDLE hPlx, u32 dwOffset, void* buf,
                     u32 dwBytes, P9X5X_ADDR addrSpace, P9X5X_MODE mode);

// interrupt functions
//int P9X5X_IntIsEnabled (P9X5X_HANDLE hPlx);
//int P9X5X_IntEnable (P9X5X_HANDLE hPlx, P9X5X_INT_HANDLER funcIntHandler);
//void P9X5X_IntDisable (P9X5X_HANDLE hPlx);

// access registers
u32   P9X5X_ReadReg (P9X5X_HANDLE hPlx, u32 dwReg);
void    P9X5X_WriteReg (P9X5X_HANDLE hPlx, u32 dwReg, u32 dwData);

// access PCI configuration registers
u32   P9X5X_ReadPCIReg(P9X5X_HANDLE hPlx, u32 dwReg);
void    P9X5X_WritePCIReg(P9X5X_HANDLE hPlx, u32 dwReg, u32 dwData);

// Start DMA to/from card.
// fIsRead - TRUE: read from card to buffer.  FALSE: write from buffer to card
// dwBytes - number of bytes to transfer (must be a multiple of 4)
// mode - local bus width.
// dwLocalAddr - local address on card to write to / read from
// channel - uses channel 0 or 1 of the 9X5X
// buf - the buffer to transfer
P9X5X_DMA_HANDLE P9X5X_DMAOpen (P9X5X_HANDLE hPlx, u32 dwLocalAddr, void* buf,
    u32 dwBytes, int fIsRead, P9X5X_MODE mode, P9X5X_DMA_CHANNEL dmaChannel);
void P9X5X_DMAClose (P9X5X_HANDLE hPlx, P9X5X_DMA_HANDLE hDma);
int P9X5X_DMAIsDone (P9X5X_HANDLE hPlx, P9X5X_DMA_HANDLE hDma);
int P9X5X_DMAReadWriteBlock (P9X5X_HANDLE hPlx, u32 dwLocalAddr, void* buf,
    u32 dwBytes, int fIsRead, P9X5X_MODE mode, P9X5X_DMA_CHANNEL dmaChannel);

int P9X5X_EEPROMReau32(P9X5X_HANDLE hPlx, u32 dwOffset, u16* pwData);
int P9X5X_EEPROMWriteWord(P9X5X_HANDLE hPlx, u32 dwOffset, u16 wData);
int P9X5X_EEPROMValid(P9X5X_HANDLE hPlx);

void P9X5X_EnableSpace(P9X5X_HANDLE hPlx, u32 space, u32 offset);
void P9X5X_DisableSpace(P9X5X_HANDLE hPlx, u32 space);
void P9X5X_LocalReset(P9X5X_HANDLE hPlx);
void P9X5X_ReadLCR1(P9X5X_HANDLE hPlx, PLX_LCR1 *plxLCR1);
void P9X5X_ReadLCR2(P9X5X_HANDLE hPlx, PLX_LCR2 *plxLCR2);
void P9X5X_WriteLCR1(P9X5X_HANDLE hPlx, PLX_LCR1 *plxLCR1);
void P9X5X_WriteLCR2(P9X5X_HANDLE hPlx, PLX_LCR2 *plxLCR2);
void	P9X5X_WriteDoorbell(P9X5X_HANDLE hPlx, u32 x);
u32	P9X5X_ReadDoorbell(P9X5X_HANDLE hPlx);
void	P9X5X_Readsu32(P9X5X_HANDLE hPlx, P9X5X_ADDR addrSpace, u32 dwOffset, void *pc_addr, u32 cnt);
void	P9X5X_Writesu32(P9X5X_HANDLE hPlx, P9X5X_ADDR addrSpace, u32 dwOffset, void *pc_addr, u32 cnt);

u32   P9X5X_dmaReadRequest(   size_t MEM_BASE,
                              u32 chan,
                              u32 isIrq,
                              u32 pcAddr,
                              u32 dspAddr,
                              u32 pciCount,
			      u32 mode);
u32   P9X5X_dmaWriteRequest(  size_t MEM_BASE,
                              u32 chan,
                              u32 isIrq,
                              u32 pcAddr,
                              u32 dspAddr,
                              u32 pciCount,
			      u32 mode);

int     P9X5X_checkDMARequest(size_t MEM_BASE, u32 chan);
void    P9X5X_DMAReset(size_t MEM_BASE,u32 chan);

#endif // __P9X5X_H
