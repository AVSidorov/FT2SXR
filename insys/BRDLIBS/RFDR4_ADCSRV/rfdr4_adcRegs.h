/*
 ****************** File rfdr4_adcRegs.h *******************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for RFDR4_ADC
 *
 * (C) InSys by Sklyarov A. Aug 2014
 *
 *************************************************************
*/

#ifndef _RFDR4_ADCREGS_H_
 #define _RFDR4_ADCREGS_H_

#pragma pack(push,1)

typedef union _CONTROL1 {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG calibr			: 1,	// Управление калибратором
		 clkSource		: 1,	// Источник тактовой: 0 - внутренний, 1 - внешний
		 enbblkrf		: 1,	// разрешение блокировка входа МШУ
		 res			: 13;	// Резерв
  } ByBits;
} REG_CONTROL1, *PREG_CONTROL1;

// Номера индивидуальных косвенных регистров тетрады
enum 
{
	ADCnr_CONTROL1		= 0x17, 
};

// Номера специфических косвенных регистров тетрады
//
enum 
{
	ADCnr_ADCMASK		= 13, // 0x0D
	ADCnr_CLRFLG		= 0x200,
	ADCnr_SPD_DEVICE	= 0x203,
	ADCnr_SPD_CTRL		= 0x204,
	ADCnr_SPD_ADDR		= 0x205,
	ADCnr_SPD_DATA		= 0x206,
	ADCnr_ADC_OVR		= 0x208,
	
	ADCnr_EXTSTART		= 0x209,
	ADCnr_STD_DELAY		= 0x2f0,
};
enum 
{
	ADCnr_ATTEN0		= 0x210,
	ADCnr_ATTEN1		= 0x211,
	ADCnr_ATTEN2		= 0x212,
	ADCnr_ATTEN3		= 0x213,
};



#pragma pack(pop)

#endif //_RFDR4_ADCREGS_H_

//
// End of file
//