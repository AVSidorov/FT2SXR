///////////////////////////////////////////////////////////////
//  Wambpmc.mc - event log message file
//  Portions Copyright (c) 1996-2001, BSQUARE CORPORATION
///////////////////////////////////////////////////////////////
//
//  Status values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-------------------------+-------------------------------+
//  |Sev|C|       Facility          |               Code            |
//  +---+-+-------------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//

//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_DRIVERDEFINED_WAMBP_ERROR_CODE 0x7
#define FACILITY_RPC_STUBS               0x3
#define FACILITY_RPC_RUNTIME             0x2
#define FACILITY_IO_ERROR_CODE           0x4


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: MISSING_REGISTRY_INFORMATION
//
// MessageText:
//
//  Information needed to start device %2 is missing from the registry.  The key missing is %3.
//
#define MISSING_REGISTRY_INFORMATION     ((NTSTATUS)0xC0070001L)

//
// MessageId: FAILED_TO_INITIALIZE_HARDWARE
//
// MessageText:
//
//  The hardware failed to initialize properly device %2.
//
#define FAILED_TO_INITIALIZE_HARDWARE    ((NTSTATUS)0xC0070002L)

//
// MessageId: FAILED_TO_OBTAIN_INTERRUPT
//
// MessageText:
//
//  The device %2 failed to obtain the interrupt number %3.
//
#define FAILED_TO_OBTAIN_INTERRUPT       ((NTSTATUS)0xC0070003L)

//
// MessageId: FAILED_TO_MAP_ADDRESS
//
// MessageText:
//
//  The device %2 failed to map the address %3 for a length of %4.  Another device may be using this address.
//
#define FAILED_TO_MAP_ADDRESS            ((NTSTATUS)0xC0070004L)

//
// MessageId: INIT_SUCCESS
//
// MessageText:
//
//  The device %2 initialize successed.
//
#define INIT_SUCCESS                     ((NTSTATUS)0x40070005L)

//
// MessageId: IRQ0_SUCCESS
//
// MessageText:
//
//  The device %2 IRQ0 successed.
//
#define IRQ0_SUCCESS                     ((NTSTATUS)0x40070006L)

//
// MessageId: IRQ1_SUCCESS
//
// MessageText:
//
//  The device %2 IRQ1 successed.
//
#define IRQ1_SUCCESS                     ((NTSTATUS)0x40070007L)

