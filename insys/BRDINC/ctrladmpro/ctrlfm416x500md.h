/*
 ****************** File ctrldac416x1g5d.h *************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : DAC416X1G5D section
 *
 * (C) InSys, Ekkore, Mar 2014
 *
 *
 ************************************************************
*/

#ifndef _CTRL_DAC416X1G5D_H
#define _CTRL_DAC416X1G5D_H

#include "ctrldac.h"

#pragma pack(push, 1)    

const int BRD_CLKDIVCNT		= 5; // Number of clock dividers = 5 (1, 2, 4, 8, 16)

// DAC416X1G5D Clock sources
enum {
	BRDclks_DAC_DISABLED	= 0x0,	// Clock not set
	BRDclks_DAC_SUBGEN		= 0x1,	// Internal SubModule generator (Si570/571)
	BRDclks_DAC_BASEGEN		= 0x2,	// Internal Base Module generator
	BRDclks_DAC_EXTCLK		= 0x81,	// External SubModule clock (CLKIN) 
};
// DAC416X1G5D Reference sources
enum {
	BRDclks_DAC_INTREFPLL	= 0,	// Internal reference
	BRDclks_DAC_EXTREFPLL	= 1,	// External reference
	BRDclks_DAC_REFNOTSET	= 0xff,	// Referens not set
};

// Numbers of Specific Command
typedef enum 
{
	DAC416X500MDcmd_SETMODULATOR	= 129,
	DAC416X500MDcmd_SETAUXDAC,
	DAC416X500MDcmd_SETFSC,
	DAC416X500MDcmd_SETMIXMODE,

} DAC416X500MD_NUM_CMD;

// ��������� ������ ��� ������������� ������
typedef struct
{
	U32		chanMask;
	S32		att;		// ����������: 1-�������, 0-��������, -1 - ���� �� ����������
	S32		ampLo;		// �������� ��������� ������� ����������: �� �� �����
	S32		ampHi;		// �������� ��������� ������� ����������: �� �� �����
	S32		flt0;		// ���������� 120 ���: �� �� �����
	S32		flt1;		// ���������� 30 ���: �� �� �����
} BRD_DacSetInp;

typedef struct _BRD_DacIntReg
{
	U32		addr;			// register address into DAC chip for programming
	U32		data;			// data for writing / getting
} BRD_DacIntReg, *PBRD_DacIntReg;

typedef struct
{
	U32		power0;			//1 - ��������� ���������� 0
	U32		power1;			//1 - ��������� ���������� 1
	U32		mode0;			// ����� ���������� 0: 0-����. ��������, 1-�����. ��������
	U32		mode1;			// ����� ���������� 1: 0-����. ��������, 1-�����. ��������	
} BRD_DacSetModulator, *PBRD_DacSetModulator;

typedef struct
{	
	U32		isDac0;		// 1 - �������� ��������� ��� ���0
	U32		param01;
	U32		param02;
	U32		isDac1;		// 1 - �������� ��������� ��� ���1
	U32		param11;
	U32		param12;
} BRD_DacSetAdditional, *PBRD_DacSetAdditional;

#pragma pack(pop)    

#endif // _CTRL_DAC416X1G5D_H

//
// End of file
//