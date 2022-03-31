/*
 ****************** File Adc210x1gRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADM210x1G
 *
 * (C) InSys by Ekkore Okt 2009
 *
 ************************************************************
*/

#ifndef _ADC210x1GREGS_H_
#define _ADC210x1GREGS_H_

//
// Номера специфических косвенных регистров тетрады
//
enum 
{
	ADCnr_ADCREGADDR		= 26, // 0x1A
	ADCnr_ADCREGDATA		= 27, // 0x1B
	ADCnr_I2C_CTRL			= 0x204,
	ADCnr_I2C_ADDR			= 0x205,
	ADCnr_I2C_DATAL			= 0x206,
	ADCnr_I2C_DATAH			= 0x207,
};

#pragma pack(push,1)

//
// Внутренние структуры битовых полей специфических косвенных регистров
//

//
// ADC Frequency Divider register (+20)
//
typedef union  
{
	U32	AsWhole;		// Register as a Whole Word
	struct 				// Register as Bit Pattern
	{	U32	Div	: 4,	// 0x0 - 1, 0x1 - 2, 0x2 - 4, 0x3 - 8, 0x4 - 16
			Res	: 12;	// not use
	} ByBits;	
} ADC_FDVR, *PADC_FDVR;

//
// ADC Control register (+23)
//
typedef union  
{
	U32	AsWhole;		// Register as a Whole Word
	struct 				// Register as Bit Pattern
	{	U32	Res0		: 2,	// not use
			Clbr		: 1,	// 1 - calibration by zero
			Res			: 1;	// not use
		U32	StartSrc	: 3,	// select source of start: 0 - channel 0, 1 - channel 1, 2 - external input X5, 3 - program
			StartInv	: 1,	// Reserve. 1 - inverting start
			StartSl		: 1,	// 1 - start Slave mode
			ClockSl		: 1,	// 1 - clock Slave mode
			Res1		: 6;	// not use
	} ByBits;
} ADC_CTRL, *PADC_CTRL;

//
// PLL control register (low word) (+24)
//
typedef union  
{
	U32	AsWhole;		// Register as a Whole Word
	struct 				// Register as Bit Pattern
	{	U32 DataL	: 16;	// PLL control word (low part)
	} ByBits;
} ADC_LOWPLL, *PADC_LOWPLL;

//
// PLL control register (high word) (+25)
//
typedef union  
{
	U32	AsWhole;		// Register as a Whole Word
	struct				// Register as Bit Pattern
	{	U32 DataH	: 8;	// PLL control word (high part)
	} ByBits;
} ADC_HIPLL, *PADC_HIPLL;

//
// ADC chip address register (+26)
//
typedef union  
{
	U32	AsWhole;		// Register as a Whole Word
	struct 				// Register as Bit Pattern
	{	U32 Addr	: 4;	// ADC chip address 
	} ByBits;
} ADC_ADCREGADDR, *PADC_ADCREGADDR;

//
// ADC chip data register (+27)
//
typedef union  
{
	U32	AsWhole;		// Register as a Whole Word
	struct {			// Register as Bit Pattern
		U32 Data	: 16;	// ADC chip data 
	} ByBits;
} ADC_ADCREGDATA, *PADC_ADCREGDATA;

//
// GEN1 chip control register (I2E accessible) (0x204)
//
typedef union  
{
	U32	AsWhole;		// Register as a Whole Word
	struct 				// Register as Bit Pattern
	{	U32 IsWrite	: 1,	// 1 - операция записи в устройство с интерфейсом I2C 
			IsRead	: 1,	// 1 - операция чтения из устройства с интерфейсом I2C 
			IsPage	: 1,	// 1 - признак страничного режима (только чтение)
			Res		: 5,	// резерв
			SlotNum	: 3;	// номер слота (всегда 0)
	} ByBits;
} ADC_I2CCTRL, *PADC_I2CCTRL;

//
// GEN1 chip address register (I2E accessible) (0x205)
//
typedef union  
{
	U32	AsWhole;		// Register as a Whole Word
	struct 				// Register as Bit Pattern
	{	U32 Adr	: 8;	//  GEN1 chip address
	} ByBits;
} ADC_I2CADDR, *PADC_I2CADDR;

//
// GEN1 chip data low register (I2E accessible) (0x206)
//
typedef union  
{
	U32	AsWhole;		// Register as a Whole Word
	struct 				// Register as Bit Pattern
	{	U32 Data	: 8;	// GEN1 chip data 
	} ByBits;
} ADC_I2CDATAL, *PADC_I2CDATAL;

#pragma pack(pop)

#endif //_ADC210x1GREGS_H_

//
// End of file
//