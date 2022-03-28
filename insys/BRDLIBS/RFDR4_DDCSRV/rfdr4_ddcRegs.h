/*
 ****************** File rfdr4_ddcRegs.h *******************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for RFDR4
 *
 * (C) InSys by Sklyarov A. Aug. 2014
 *
 *************************************************************
*/

#ifndef _RFDR4_DDCREGS_H_
 #define _RFDR4_DDCREGS_H_

#pragma pack(push,1)

typedef union _CONTROL0 {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG porog_det_clk	: 4,	// Порог детектора тактовой: 1 - 15
		 mode_det_clk	: 1,	// Режим детектора тактовой: 0 программный(вызов функции), 1- автомат (с периодои 100 мс
		 led_control	: 1,	// Режим управления светодиодом: 0 - программный, 1 - от выхода детектора тактовой 
		 led_level		: 1,	// Управления светодиодом в программном режиме: 0 - "выключен", 1 - "включен" 
		 res			: 9;	// Резерв
  } ByBits;
} REG_CONTROL0, *PREG_CONTROL0;


typedef union _CONTROL1 {
  ULONG AsWhole;	// Register as a Whole Word
  struct {			// Register as Bit Pattern
   ULONG startDdc	: 1,	// 1 - старт DDC
		 nco_enb	: 1,	// не используется
	     nco_rst	: 1,	// не используется
	     frameOn	: 1,	// 1 - включить кадровый режим ввода
	     headerOn	: 1,	// 1 - вставлять заголовок
		 res1		: 1,	// Резерв
		 correctOn	: 1,	// 1 - включить коррекцию
		 start_rst_nco	: 1,	// 1 - автоматически производить reset NCO при старте
		 start_wr	: 1,	// не используется
	     scan_enb	: 1,	// 1 - включить режим сканирования
		 rst_nco	: 1,	// не используется
	     start_rst_cic : 1,	// 1 - автоматически производить reset CIC при старте
	     start_rst_fir : 1,	// 1 - автоматически производить reset FIR при старте
	     wrscaner	: 1,	// Ввыбор сканера для записи: 0 или 1
		 rdscaner	: 1,	// Ввыбор сканера для работы: 0 или 1
		 res2		: 1;	// Резерв
  } ByBits;
} REG_CONTROL1, *PREG_CONTROL1;


//-------------------------------------------------------------------------
// Numbers of Command Registers
typedef enum _RFDR4_DDC_NUM_CMD_REGS {
	DDCnr_GETDETECT		= 0x15,		// Старт процедуры обнаружения тактовой
	DDCnr_CONTROL0		= 0x16,		// Регистр управления 0
	DDCnr_CONTROL1		= 0x17,		// Регистр управления 1
	DDCnr_NCO			= 0x18,		// Регистр для записи кода частоты NCO 
	DDCnr_PROG			= 0x19,		// Не используется
	DDCnr_WRNCO			= 0x1A,		// Запись кода частоты в теневые регистры DDC
	DDCnr_NCO_MASK		= 0x1B,		// Маска каналов  при записи в теневые регистры DDC
	DDCnr_COR_COEF		= 0x1C,		// Регистр для записи корректирующих коффициентов 
	DDCnr_COEF_WR		= 0x1D,		// Запись  корректирующих коффициентов 
	DDCnr_MEM_WR		= 0x1E,		// Регистр записи памяти частот сканирования 
	DDCnr_MEM_RST		= 0x1F,		// Регистр cброса  памяти частот сканирования (обнуления адреса) 
} RFDR4_READ_WRITE_REGS;

// Номера специфических косвенных регистров тетрады
//
enum 
{
	DDCnr_SKIPSAMP		= 0x210,	// Число отсчётов пропускаемых в нвчале кадра: 0...65535 
	DDCnr_GETSAMP		= 0x211,	// Число отсчётов собираемых в кадре:   0..65535
	DDCnr_SYNCWORD		= 0x212,	// Синхрослово заголовка 1: 0х0...0xFFFF
	DDCnr_DELAYSTART	= 0x213,	// Задержка импульса внешнего старта в тактах частоты 50 МГц(20 нсек)
	DDCnr_USERWORD		= 0x214,	// Синхрослово заголовка 2:  0х0...0xFFFF
	DDCnr_NSCAN			= 0x215,	// Число срезов сканирования ( один срез - 8 каналов)
};





#pragma pack(pop)

#endif //_RFDR4_DDCREGS_H_

//
// End of file
//