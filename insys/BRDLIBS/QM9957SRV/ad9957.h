//********************* File AD9956.h ******************
//*********************************************************

#ifndef _AD9957_H
 #define _AD9957_H

#pragma pack(1)    

typedef struct _R2BYTES {
  BYTE Byte1;  // 
  BYTE Byte0;  // 
} R2BYTES;

typedef struct _R3BYTES {
  BYTE Byte2;  // 
  BYTE Byte1;  // 
  BYTE Byte0;  // 
} R3BYTES;

typedef struct _R4BYTES {
  BYTE Byte3;  // 
  BYTE Byte2;  // 
  BYTE Byte1;  // 
  BYTE Byte0;  // 
} R4BYTES;

typedef struct _R5BYTES {
  BYTE Byte4;  // 
  BYTE Byte3;  // 
  BYTE Byte2;  // 
  BYTE Byte1;  // 
  BYTE Byte0;  // 
} R5BYTES;

typedef struct _R6BYTES {
  BYTE Byte5;  // 
  BYTE Byte4;  // 
  BYTE Byte3;  // 
  BYTE Byte2;  // 
  BYTE Byte1;  // 
  BYTE Byte0;  // 
} R6BYTES;

typedef struct _R8BYTES {
  BYTE Byte7;  // 
  BYTE Byte6;  // 
  BYTE Byte5;  // 
  BYTE Byte4;  // 
  BYTE Byte3;  // 
  BYTE Byte2;  // 
  BYTE Byte1;  // 
  BYTE Byte0;  // 
} R8BYTES;

// Control Functional Register 1 (address - 0x00, bit width - 32)
typedef struct _AD9957_CFR1 {
    BYTE Byte3;  
    BYTE Byte2;  
    BYTE Byte1;  
    BYTE Byte0;  
} AD9957_CFR1, *PAD9957_CFR1;

// Control Functional Register 2 (address - 0x01, bit width - 32)
typedef struct _AD9957_CFR2 {
    BYTE Byte3;  
    BYTE Byte2;  
    BYTE Byte1;  
    BYTE Byte0;  
} AD9957_CFR2, *PAD9957_CFR2;

// Control Functional Register 3 (address - 0x02, bit width - 32)
typedef struct _AD9957_CFR3 {
    BYTE Byte3;  
    BYTE Byte2;  
    BYTE Byte1;  
    BYTE Byte0;  
} AD9957_CFR3, *PAD9957_CFR3;

// Auxiliary DAC Control Register  (address - 0x03, bit width - 32)
typedef struct _AD9957_ADCR {
    BYTE Byte3; 
    BYTE Byte2; 
    BYTE Byte1; 
    BYTE Byte0; 
} AD9957_ADCR, *PAD9957_ADCR;

// I/O Update Rate  Register  (address - 0x04, bit width - 32)
typedef struct _AD9957_IOUR {
    BYTE Byte3;  
    BYTE Byte2;  
    BYTE Byte1;  
    BYTE Byte0;  
} AD9957_IOUR, *PAD9957_IOUR;

// RAM Segment  Register 1  (address - 0x05, bit width - 48)
typedef struct _AD9957_RSR1 {
    BYTE Byte5; 
    BYTE Byte4; 
    BYTE Byte3; 
    BYTE Byte2; 
    BYTE Byte1; 
    BYTE Byte0; 
} AD9957_RSR1, *PAD9957_RSR1;

// RAM Segment  Register 2  (address - 0x06, bit width - 48)
typedef struct _AD9957_RSR2 {
    BYTE Byte5; 
    BYTE Byte4; 
    BYTE Byte3; 
    BYTE Byte2; 
    BYTE Byte1; 
    BYTE Byte0; 
} AD9957_RSR2, *PAD9957_RSR2;

// Amplitude Scale Factor   Register  (address - 0x09, bit width - 32)
typedef struct _AD9957_ASFR {
    BYTE Byte3;  
    BYTE Byte2;  
    BYTE Byte1;  
    BYTE Byte0;  
} AD9957_ASFR, *PAD9957_ASFR;

// Maltichip Sync    Register  (address - 0x0A:0x15, bit width - 32)
typedef struct _AD9957_MSR {
    BYTE Byte3;  
    BYTE Byte2;  
    BYTE Byte1;  
    BYTE Byte0;  
} AD9957_MSR, *PAD9957_MSR;

// Profile    Registers  (address - 0x0E..0x15, bit width - 64)
typedef struct _AD9957_PROFILE {
    BYTE Byte7;  
    BYTE Byte6;  
    BYTE Byte5;  
    BYTE Byte4;  
    BYTE Byte3;  
    BYTE Byte2;  
    BYTE Byte1;  
    BYTE Byte0;  
} AD9957_PROFILE, *PAD9957_PROFILE;

// RAM    Register  (address - 0x16, bit width - 32)
typedef struct _AD9957_RAMR {
    BYTE Byte3;  
    BYTE Byte2;  
    BYTE Byte1;  
    BYTE Byte0;  
} AD9957_RAMR, *PAD9957_RAMR;

// GPIO Configuration    Register  (address - 0x18, bit width - 32)
typedef struct _AD9957_GPIOC {
    BYTE Byte1;  
    BYTE Byte0;  
} AD9957_GPIOC, *PAD9957_GPIOC;

// GPIO Data    Register  (address - 0x19, bit width - 32)
typedef struct _AD9957_GPIOD {
    BYTE Byte1;  
    BYTE Byte0;  
} AD9957_GPIOD, *PAD9957_GPIOD;

// Chip configuration data structures
typedef struct _AD9957_CFG {
	U32	RegCFR1;			
	U32	RegCFR2;			
	U32	RegCFR3;			
	U32	RegADCR;			
	U32	RegIOUR;
	U32	RegRSR1;
	U32	RegRSR2;
	U32	RegASFR;
	U32	RegMSR;
	__int64	RegProfile[8];
	U32	RegRAMR;
	U32	RegGPIOC;
	U32	RegGPIOD;
} AD9957_CFG, *PAD9957_CFG;

#pragma pack()    
/*
enum AD9956ErrorCodes {
  AD9956_errOK,
  AD9956_errHIGHRATE = 0xE0001000UL,
  AD9956_errLOWRATE,
  AD9956_errBADPARAM,
  AD9956_errILLEGAL
};

void AD9956_setDefaults(PAD9956_CFG cfg);

UINT AD9956_getPhase(PAD9956_CFG cfg);
int AD9956_setPhase(PAD9956_CFG cfg, UINT mask);
double AD9956_getFreq(PAD9956_CFG cfg, double SysClk);
int AD9956_setFreq(PAD9956_CFG cfg, double& fc, double SysClk);
double AD9956_getFreqFbl(PAD9956_CFG cfg, double SysClk);
int AD9956_setFreqFbl(PAD9956_CFG cfg, double& fc, double SysClk);
*/
#endif // _AD9957_H

// ****************** End of file AD9957.h *****************
