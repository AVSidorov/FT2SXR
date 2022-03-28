/***************************************************
*
* PLX9080.H
*
* PLX Control macros.
*
* (C) Instr.Sys. by Ekkore Jul, 1998
*
****************************************************/


#ifndef	__PLX9080_H_
#define	__PLX9080_H_


//#include	"utypes.h"

/*
*=== PLX9080 registers addresses
*/

/* PCI Configuration Registers */
enum
{
	pciPCIIDR   = 0x00,    locPCIIDR   = 0x00,	  /* Tab 4-10, page 55 */
	pciPCICR    = 0x04,    locPCICR    = 0x04,    /* Tab 4-11, page 55 */
	pciPCISR    = 0x06,    locPCISR    = 0x06,    /* Tab 4-12, page 56 */
	pciPCIREV   = 0x08,    locPCIREV   = 0x08,    /* Tab 4-13, page 56 */
	pciPCICCR   = 0x09,    locPCICCR   = 0x09,    /* Tab 4-14, page 57 */
	pciPCICLSR  = 0x0C,    locPCICLSR  = 0x0C,    /* Tab 4-15, page 57 */
	pciPCILTR   = 0x0D,    locPCILTR   = 0x0D,    /* Tab 4-16, page 57 */
	pciPCIHTR   = 0x0E,    locPCIHTR   = 0x0E,    /* Tab 4-17, page 57 */
	pciPCIBIST  = 0x0F,    locPCIBIST  = 0x0F,    /* Tab 4-18, page 58 */
	pciPCIBAR0  = 0x10,    locPCIBAR0  = 0x10,    /* Tab 4-19, page 58 */
	pciPCIBAR1  = 0x14,    locPCIBAR1  = 0x14,    /* Tab 4-20, page 59 */
	pciPCIBAR2  = 0x18,    locPCIBAR2  = 0x18,    /* Tab 4-21, page 59 */
	pciPCIBAR3  = 0x1C,    locPCIBAR3  = 0x1C,    /* Tab 4-22, page 60 */
	pciPCIBAR4  = 0x20,    locPCIBAR4  = 0x20,    /* Tab 4-23, page 60 */
	pciPCIBAR5  = 0x24,    locPCIBAR5  = 0x24,    /* Tab 4-24, page 60 */
	pciPCICIS   = 0x28,    locPCICIS   = 0x28,    /* Tab 4-25, page 61 */
	pciPCIVID   = 0x2C,    locPCIVID   = 0x2C,    /* Tab 4-26, page 61 */
	pciPCISID   = 0x2E,    locPCISID   = 0x2E,    /* Tab 4-27, page 61 */
	pciPCIERBAR = 0x30,    locPCIERBAR = 0x30,    /* Tab 4-28, page 61 */
	pciPCIILR   = 0x3C,    locPCIILR   = 0x3C,    /* Tab 4-29, page 61 */
	pciPCIIPR   = 0x3D,    locPCIIPR   = 0x3D,    /* Tab 4-30, page 62 */
	pciPCIMGR   = 0x3E,    locPCIMGR   = 0x3E,    /* Tab 4-31, page 62 */
	pciPCIMLR   = 0x3F,    locPCIMLR   = 0x3F,    /* Tab 4-32, page 62 */
};

/* Local Configuration Registers */
enum
{
	pciLAS0RR   = 0x00,    locLAS0RR   = 0x80,    /* Tab 4-33, page 63 */
	pciLAS0BA   = 0x04,    locLAS0BA   = 0x84,    /* Tab 4-34, page 63 */
	pciMARBR    = 0x08,    locMARBR    = 0x88,    /* Tab 4-35, page 64 */
	pciBIGEND   = 0x0C,    locBIGEND   = 0x8C,    /* Tab 4-36, page 65 */
	pciEROMRR   = 0x10,    locEROMRR   = 0x90,    /* Tab 4-37, page 66 */
	pciEROMBA   = 0x14,    locEROMBA   = 0x94,    /* Tab 4-38, page 66 */
	pciLBRD0    = 0x18,    locLBRD0    = 0x98,    /* Tab 4-39, page 67 */
	pciDMRR     = 0x1C,    locDMRR     = 0x9C,    /* Tab 4-40, page 68 */
	pciDMLBAM   = 0x20,    locDMLBAM   = 0xA0,    /* Tab 4-41, page 68 */
	pciDMLBAI   = 0x24,    locDMLBAI   = 0xA4,    /* Tab 4-42, page 68 */
	pciDMPBAM   = 0x28,    locDMPBAM   = 0xA8,    /* Tab 4-43, page 69 */
	pciDMCFGA   = 0x2C,    locDMCFGA   = 0xAC,    /* Tab 4-44, page 70 */
	pciLAS1RR   = 0xF0,    locLAS1RR   = 0x170,   /* Tab 4-45, page 70 */
	pciLAS1BA   = 0xF4,    locLAS1BA   = 0x174,   /* Tab 4-46, page 71 */
	pciLBRD1    = 0xF8,    locLBRD1    = 0x178,   /* Tab 4-47, page 71 */
};

/* Runtime Registers */
enum
{
	pciMBOX0    = 0x78,    locMBOX0    = 0xC0,    /* Tab 4-48, page 72 */
	pciMBOX1    = 0x7C,    locMBOX1    = 0xC4,    /* Tab 4-49, page 72 */
	pciMBOX2    = 0x48,    locMBOX2    = 0xC8,    /* Tab 4-50, page 72 */
	pciMBOX3    = 0x4C,    locMBOX3    = 0xCC,    /* Tab 4-51, page 72 */
	pciMBOX4    = 0x50,    locMBOX4    = 0xD0,    /* Tab 4-52, page 72 */
	pciMBOX5    = 0x54,    locMBOX5    = 0xD4,    /* Tab 4-53, page 73 */
	pciMBOX6    = 0x58,    locMBOX6    = 0xD8,    /* Tab 4-54, page 73 */
	pciMBOX7    = 0x5C,    locMBOX7    = 0xDC,    /* Tab 4-55, page 73 */
	pciP2LDBELL = 0x60,    locP2LDBELL = 0xE0,    /* Tab 4-56, page 73 */
	pciL2PDBELL = 0x64,    locL2PDBELL = 0xE4,    /* Tab 4-57, page 73 */
	pciINTCSR   = 0x68,    locINTCSR   = 0xE8,    /* Tab 4-58, page 74 */
	pciCNTRL    = 0x6C,    locCNTRL    = 0xEC,    /* Tab 4-59, page 76 */
	pciPCIHIDR  = 0x70,    locPCIHIDR  = 0xF0,    /* Tab 4-60, page 77 */
	pciPCIHREV  = 0x74,    locPCIHREV  = 0xF4,    /* Tab 4-61, page 77 */
};

/* DMA Registers */
enum
{
	pciDMAMODE0 = 0x80,    locDMAMODE0 = 0x100,   /* Tab 4-62, page 78 */
	pciDMAPADR0 = 0x84,    locDMAPADR0 = 0x104,   /* Tab 4-63, page 79 */
	pciDMALADR0 = 0x88,    locDMALADR0 = 0x108,   /* Tab 4-64, page 79 */
	pciDMASIZ0  = 0x8C,    locDMASIZ0  = 0x10C,   /* Tab 4-65, page 79 */
	pciDMADPR0  = 0x90,    locDMADPR0  = 0x110,   /* Tab 4-66, page 79 */
	pciDMAMODE1 = 0x94,    locDMAMODE1 = 0x114,   /* Tab 4-67, page 80 */
	pciDMAPADR1 = 0x98,    locDMAPADR1 = 0x118,   /* Tab 4-68, page 81 */
	pciDMALADR1 = 0x9C,    locDMALADR1 = 0x11C,   /* Tab 4-69, page 81 */
	pciDMASIZ1  = 0xA0,    locDMASIZ1  = 0x120,   /* Tab 4-70, page 81 */
	pciDMADPR1  = 0xA4,    locDMADPR1  = 0x124,   /* Tab 4-71, page 81 */
	pciDMACSR0  = 0xA8,    locDMACSR0  = 0x128,   /* Tab 4-72, page 82 */
	pciDMACSR1  = 0xA9,    locDMACSR1  = 0x129,   /* Tab 4-73, page 82 */
	pciDMAARB   = 0xAC,    locDMAARB   = 0x12C,   /* Tab 4-35, page 82 */
	pciDMATHR   = 0xB0,    locDMATHR   = 0x130,   /* Tab 4-74, page 83 */
};

/* Messaging Queue Registers */
enum
{
	pciOPLFIS   = 0x30,    locOPLFIS   = 0x1B0,   /* Tab 4-75, page 84 */
	pciOPLFIM   = 0x34,    locOPLFIM   = 0x1B4,   /* Tab 4-76, page 84 */
	pciIQP      = 0x40,  /*locIQP      = 0x??,*/  /* Tab 4-77, page 84 */
	pciOQP      = 0x44,  /*locOQP      = 0x??,*/  /* Tab 4-78, page 85 */
	pciMQCP     = 0xC0,    locMQCP     = 0x140,   /* Tab 4-79, page 85 */
	pciQBAR     = 0xC4,    locQBAR     = 0x144,   /* Tab 4-80, page 85 */
	pciIFHPR    = 0xC8,    locIFHPR    = 0x148,   /* Tab 4-81, page 86 */
	pciIFTPR    = 0xCC,    locIFTPR    = 0x14C,   /* Tab 4-82, page 86 */
	pciIPHPR    = 0xD0,    locIPHPR    = 0x150,   /* Tab 4-83, page 86 */
	pciIPTPR    = 0xD4,    locIPTPR    = 0x154,   /* Tab 4-84, page 86 */
	pciOFHPR    = 0xD8,    locOFHPR    = 0x158,   /* Tab 4-85, page 87 */
	pciOFTPR    = 0xDC,    locOFTPR    = 0x15C,   /* Tab 4-86, page 87 */
	pciOPHPR    = 0xE0,    locOPHPR    = 0x160,   /* Tab 4-87, page 87 */
	pciOPTPR    = 0xE4,    locOPTPR    = 0x164,   /* Tab 4-88, page 87 */
	pciQSR      = 0xE8,    locQSR      = 0x168,   /* Tab 4-89, page 88 */
};


#endif	/* __PLX9080_H_ */

/*
*  End of File
*/


