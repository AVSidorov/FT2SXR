/*
 ****************** File Adc212x1gRegs.h *************************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for ADM212x1G
 *
 * (C) InSys by Ekkore Okt 2010
 *
 ************************************************************
*/

#ifndef _ADC212x1GREGS_H_
#define _ADC212x1GREGS_H_

//
// Номера специфических косвенных регистров тетрады
//
enum 
{
	ADCnr_PLL_LOW			= 24, // 0x18
	ADCnr_PLL_HIGH			= 25, // 0x19
	ADCnr_ADCREGADDR		= 26, // 0x1A
	ADCnr_ADCREGDATA		= 27, // 0x1B
	ADCnr_I2C_CTRL			= 0x204,
	ADCnr_I2C_ADDR			= 0x205,
	ADCnr_I2C_DATAL			= 0x206,
	ADCnr_I2C_DATAH			= 0x207,
	ADCnr_STARTDISCNTR		= 0x220, //спец. прошивка для руднева
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
			Res1		: 1;	// not use
		U32	StartSrc	: 3,	// select source of start: 0 - channel 0, 1 - channel 1, 2 - external input X5, 3 - program
			StartInv	: 1,	// Reserve
			StartSl		: 1,	// 1 - start Slave mode
			ClockSl		: 1,	// 1 - clock Slave mode
			stResist	: 1,	// Start Resist: 0-2.5 kOm, 1-50 Om
			Res3		: 5;	// not use
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

// ADF4106 PLL Reference Counter Latch
typedef union _PLL_RCNT {
  DWORD AsWhole; // Register as a Whole Word
  struct { // Register as Words
    WORD LoWord;
    WORD HiWord;
  } ByWord;
  struct { // Register as Bit Pattern
    UINT Addr		: 2,	// address 00
		 R_CNTR		: 14,	//
		 AB_WIDTH	: 2,	//
		 TEST_MODE	: 2,	//
		 LDP		: 1,	//
		 Res		: 3;	// must be 0
  } ByBits;
} PLL_RCNT, *PPLL_RCNT;

// ADF4106 PLL N Counter Latch
typedef union _PLL_NCNT {
  DWORD AsWhole; // Register as a Whole Word
  struct { // Register as Bit Pattern
    WORD LoWord;
    WORD HiWord;
  } ByWord;
  struct { // Register as Bit Pattern
    UINT Addr		: 2,	// address 01
		 A_CNTR		: 6,	// 
		 B_CNTR		: 13,	//
		 CP_GAIN	: 1,	//
		 Res1		: 2;	//
  } ByBits;
} PLL_NCNT, *PPLL_NCNT;

// ADF4106 PLL Function Latch
typedef union _PLL_FUNC {
  DWORD AsWhole; // Register as a Whole Word
  struct { // Register as Bit Pattern
    WORD LoWord;
    WORD HiWord;
  } ByWord;
  struct { // Register as Bit Pattern
    UINT Addr		: 2,	// address 10
		 CNT_RST	: 1,	//
		 PWDN_1		: 1,	//
		 MUX_OUT	: 3,	//
		 PD_POL		: 1,	//
		 CP_3STATE	: 1,	//
		 FL_ENBL	: 1,	//
		 FL_MODE	: 1,	//
		 TIMER_CNT	: 4,	//
		 CUR_SET_1	: 3,	//
		 CUR_SET_2	: 3,	//
		 PWDN_2		: 1,	//
		 PRESCALER	: 2;	//
  } ByBits;
} PLL_FUNC, *PPLL_FUNC;

const double PLL_F_CMP_MAX = 100.e6; // 100. MHz

#pragma pack(pop)

#endif //_ADC212x1GREGS_H_

//
// End of file
//