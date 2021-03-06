#ifndef _FM402SREGS_H
#define _FM402SREGS_H

#pragma pack(push,1)
typedef union _FM402S_STATUS {
	ULONG AsWhole;
	struct {
		ULONG	CmdRdy : 1,
			FifoRdy : 1,
			Empty : 1,
			AlmostEmpty : 1,
			HalfFull : 1,
			AlmostFull : 1,
			Full : 1,
			Overflow : 1,
			Underflow : 1,
			Reserved : 1,
			QSFP_LINE_UP_0 : 1,
			QSFP_LINE_UP_1 : 1,
			QSFP_LINE_UP_2 : 1,
			QSFP_LINE_UP_3 : 1,
			ChanUp : 1,
			PLL_Lock : 1;
	} ByBits;
} FM402S_STATUS, *PFM402S_STATUS;

typedef union _FM402S_SFP_CONTROL {//0x15
	ULONG AsWhole;
	struct {
		ULONG SFP_Reset		: 1,
			LowPowerMode	: 1,
			SEL				: 1,
			Reserved		: 13;
	} ByBits;
} FM402S_SFP_CONTROL, *PFM402S_SFP_CONTROL;

typedef union _FM402S_TEST_SEQUENCE {//0x0C
	ULONG AsWhole;
	struct {
		ULONG Reserved1 : 9,
			Gen_Enable	: 1,
			Cnt_Enable	: 1,
			Reserved2	: 5;
	} ByBits;
} FM402S_TEST_SEQUENCE, *PFM402S_TEST_SEQUENCE;

typedef union _FM402S_MODE1 {//0x9
	ULONG AsWhose;
	struct {
		ULONG StartTx : 1,
			Reserved : 15;
	} ByBits;
} FM402S_MODE1, *PFM402S_MODE1;

#pragma pack(pop)

const ULONG FM402S_REG_TEST_SEQUENCE = 0x0C;
const ULONG FM402S_REG_ERR_ADR = 0x10;
const ULONG FM402S_REG_SFP_CONTROL = 0x15;
const ULONG FM402S_REG_LC_FLAG = 0x16;
const ULONG FM402S_REG_CONTROL = 0x17;

const ULONG FM402S_SPD_DEVICE = 0x205;
const ULONG FM402S_SPD_CTRL = 0x205;
const ULONG FM402S_SPD_ADDR = 0x205;
const ULONG FM402S_SPD_DATA = 0x205;
const ULONG FM402S_SPD_DATAH = 0x205;

const ULONG SPDdev_FM402S_GEN = 0x01;

const ULONG FM402S_SPD_REG_CNT_TX_L = 0x200;
const ULONG FM402S_SPD_REG_CNT_TX_H = 0x201;
const ULONG FM402S_SPD_REG_CNT_RX_L = 0x202;
const ULONG FM402S_SPD_REG_CNT_RX_H = 0x203;

const ULONG FM402S_SPD_REG_ERR_DATA = 0x204;

const ULONG FM402S_ERR_ADDR_CH0_DATA = 0x0;
const ULONG FM402S_ERR_ADDR_CH1_DATA = 0x1000;
const ULONG FM402S_ERR_ADDR_CH2_DATA = 0x2000;
const ULONG FM402S_ERR_ADDR_CH3_DATA = 0x3000;
const ULONG FM402S_ERR_ADDR_BLOCK_RD_L = 0x8000;
const ULONG FM402S_ERR_ADDR_BLOCK_RD_H = 0x8001;
const ULONG FM402S_ERR_ADDR_BLOCK_OK_L = 0x8002;
const ULONG FM402S_ERR_ADDR_BLOCK_OK_H = 0x8003;
const ULONG FM402S_ERR_ADDR_BLOCK_ERR_L = 0x8004;
const ULONG FM402S_ERR_ADDR_BLOCK_ERR_H = 0x8005;
const ULONG FM402S_ERR_ADDR_BLOCK_WR_L = 0x8006;
const ULONG FM402S_ERR_ADDR_BLOCK_WR_H = 0x8007;

const ULONG FM402S_ERR_DATA_READ_D0 = 0x0;
const ULONG FM402S_ERR_DATA_READ_D1 = 0x1;
const ULONG FM402S_ERR_DATA_READ_D2 = 0x2;
const ULONG FM402S_ERR_DATA_READ_D3 = 0x3;
const ULONG FM402S_ERR_DATA_EXPECT_D0 = 0x4;
const ULONG FM402S_ERR_DATA_EXPECT_D1 = 0x5;
const ULONG FM402S_ERR_DATA_EXPECT_D2 = 0x6;
const ULONG FM402S_ERR_DATA_EXPECT_D3 = 0x7;
const ULONG FM402S_ERR_DATA_INDEX = 0x8;
const ULONG FM402S_ERR_DATA_BLOCK_D0 = 0x9;
const ULONG FM402S_ERR_DATA_BLOCK_D1 = 0xA;

const ULONG FM402S_ERR_DATA_TOTAL_ERR_L = 0x100;
const ULONG FM402S_ERR_DATA_TOTAL_ERR_H = 0x101;

#endif //_FM402SREGS_H