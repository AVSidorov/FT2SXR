//****************** File icradmlst.h *******************************
//
// Список типов субмодулей
// ADM-Modules Types List
//
//	Copyright (c) 2002-2004, Instrumental Systems,Corp.
//	Written by Dorokhin Andrey
//
//  History:
//  17-01-03 - builded
//  31-07-03 - add ADM414x65M = 0x0450, ADMDAC214x160M = 0x0C10
//  20-08-03 - modified typedef enum ICR_ADMType for compatibility with Builder C++
//  16-12-04 - add ADM28x800M = 0x0030
//  07-02-07 - add ADM214x200M = 0x0460, ADM216x100M = 0x0640
//  22-05-07 - add ADM1624x192 = 0x0810
//
//*******************************************************************

#ifndef	__ICRADMLST_H_
#define	__ICRADMLST_H_

typedef enum _ICR_ADMType {
        ADM28x100M      = 0x0010,
        ADM8x200M       = 0x0020,
        ADM28x800M      = 0x0030,
        ADM28x1G        = 0x0031,
        ADM210x200M     = 0x0100,
		ADM210x1G       = 0x0110,
        ADM12x1M        = 0x0200,
        ADM12x3M        = 0x0201,
        ADM12x10M       = 0x0202,
        ADM212x10M      = 0x0210,
        ADM212x25M      = 0x0211,
        ADM212x40M      = 0x0212,
        ADM212x50M      = 0x0220,
        ADM212x60M      = 0x0221,
        ADM212x100M     = 0x0230,
        ADM1612x1M      = 0x0240,
        ADM212x200M     = 0x0250,
		ADM212x500M     = 0x0258,
		ADM212x500MWB   = 0x0259,
        ADM1612x65M     = 0x0260,
        ADM212x1G       = 0x0270,
		ADM212x1GWB     = 0x0271,
        ADM214x1M       = 0x0400,
        ADM214x3M       = 0x0401,
        ADM214x10M      = 0x0402,
        ADM214x10MX     = 0x0410,
        ADM214x60M      = 0x0420,
        ADM1614x160		= 0x0430,
        ADM214x100M     = 0x0440,
        ADM414x65M		= 0x0450,
        ADM214x200M     = 0x0460,
        ADM214x400M     = 0x0468,
		ADM214x400MWB   = 0x0469,
		ADM216x250		= 0x0601,
		ADM216x2M5		= 0x0610,
		ADM416x200		= 0x0620,
		ADM816x48		= 0x0630,
		ADM216x100M		= 0x0640,
		ADM818x800		= 0x0800,
		ADM1624x192		= 0x0801,
		ADM1624x128		= 0x0802,
		ADM818x1M5		= 0x0803,
        ADMDDC2NB       = 0x0A00,
        ADMDDC2NBH      = 0x0A01,
        ADMDDC2NBL      = 0x0A02,
        ADMDDC4NB       = 0x0A03,
        ADMDDC2WB       = 0x0A04,
        ADMDDC2WBL      = 0x0A05,
        ADMDDC2WBH      = 0x0A06,
        ADMDDC8WBL      = 0x0A07,
        ADMDDC4x16      = 0x0A08,
        ADMDDC8WB       = 0x0A09,
        ADMDDC2WBL16    = 0x0A0A,
        ADMDDC5016		= 0x0A0B,
        ADMDDC416x100M	= 0x0A0C,
        ADMDDC214x400M	= 0x0A0D,
		ADMDDC216x250M	= 0x0A0E,
        ADMDDS9854      = 0x0B00,
        ADMDDS9852A     = 0x0B01,
        ADMQM9854       = 0x0B10,
        ADMDAC214x125M	= 0x0C00,
        ADMDAC214x160M	= 0x0C10,
        ADMDAC3224x192	= 0x0C20,
		ADMDAC216x400M	= 0x0C30,
		ADMDAC216x400MV2 = 0x0C31,
		ADMDAC216x400MT  = 0x0C38,
        ADME1T1		    = 0x0D00,
        ADMFOTR		    = 0x0D10,
        ADMDIO32	    = 0x0F00,
        ADMDIOV		    = 0x0F10,
		FM814x125M		= 0x1010,
		FM814x250M		= 0x1012,
		FM214x250M		= 0x1020,
		FM412x500M		= 0x1030,
		FM212x1G		= 0x1040,
		FM816x250M		= 0x1050,
		FM416x250M		= 0x1051,
		FM216x250MDA	= 0x1052,
		FM816x250M1		= 0x1053,
		FM416x100M		= 0x1060,
} ICR_ADMType;

#endif	// __ICRADMLST_H_ 

//****************** End of file icradmlst.h ***************

