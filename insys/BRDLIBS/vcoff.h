/***************************************************
*
* VCOFF.H - define structures for COFF files.
*
* Support:
*
*	TMS320C2x/3x/4x/5x
*	TMS320C6x
*
* (C) Instrumental Systems Corp. Ekkore, Dec. 1997
*
****************************************************/



#ifndef _VCOFF_H_
#define _VCOFF_H_

#include	"utypes.h"

#pragma pack(1)

typedef struct tagSCoffFileHdr
{
	UINT16	magic;
	UINT16	sec_no;
	SINT32	time;
	SINT32	sym_adr;
	SINT32	sym_noent;
	UINT16	opt_len;
	UINT16	flags;
}
	SCoffFileHdr;
#define sizeofSCoffFileHdr    20

typedef struct tagSCoffFileHdr6x
{
	UINT16	magic;
	UINT16	sec_no;
	SINT32	time;
	SINT32	sym_adr;
	SINT32	sym_noent;
	UINT16	opt_len;
	UINT16	flags;
	UINT16	id;
}
	SCoffFileHdr6x;
#define sizeofSCoffFileHdr6x    22

typedef struct tagSCoffOptFileHdr
{
	SINT16	magic;
	SINT16	ver;
	SINT32	exesiz;
	SINT32	iniword;
	SINT32	uninibit;
	SINT32	codadr;
	SINT32	Unknown;
	SINT32	inidatadr;
}
	SCoffOptFileHdr, SCoffOptFileHdr6x;
#define sizeofSCoffOptFileHdr    28
#define sizeofSCoffOptFileHdr6x  28

typedef struct tagSCoffSectHdr
{
	SINT08	name[8];
	SINT32	phys;
	SINT32	virt;
	SINT32	size;
	SINT32	fpdata;
	SINT32	fprelc;
	SINT32	fplino;
	UINT16	relno;
	UINT16	linono;
	UINT16	flags;
	SINT08	reserved;
	UINT08	pageno;
}
	SCoffSectHdr;
#define sizeofSCoffSectHdr    40

typedef struct tagSCoffSectHdr6x
{
	SINT08	name[8];
	SINT32	phys;
	SINT32	virt;
	SINT32	size;
	SINT32	fpdata;
	SINT32	fprelc;
	SINT32	fplino;
	UINT32	relno;
	UINT32	linono;
	UINT32	flags;
	SINT16	reserved;
	UINT16	pageno;
}
	SCoffSectHdr6x;
#define sizeofSCoffSectHdr6x    48

typedef struct tagSCoffRelInfo
{
	SINT32	vadr;
	UINT16	symidx;
	UINT16	reserved;
	UINT16	type;
}
	SCoffRelInfo, SCoffRelInfo6x;
#define sizeofSCoffRelInfo    10
#define sizeofSCoffRelInfo6x  10

typedef struct tagSCoffLineNum
{
	SINT32	arg0;
	UINT16	arg1;
}
	SCoffLineNum, SCoffLineNum6x;
#define sizeofSCoffLineNum    6
#define sizeofSCoffLineNum6x  6

typedef struct tagSCoffSymTabEntry
{
	union{		SINT08	name[8];
				SINT32	stptr[2];
		 } 	arg0;
	SINT32	value;
	SINT16	secnum;
	UINT16	type;
	SINT08	stoclass;
	SINT08	auxen;
}
	SCoffSymTabEntry, SCoffSymTabEntry6x;
#define sizeofSCoffSymTabEntry    18
#define sizeofSCoffSymTabEntry6x  18

#pragma pack(4)

#define	F_RELFLG	1
#define	F_EXEC		2
#define	F_LNNO		4
#define F_LSYMS		0x10
#define F_AR32WR	0x40


/******************** Section Header Flags **************/
#define	STYP_REG	0
#define	STYP_DSECT	1
#define	STYP_NOLOAD	2
#define	STYP_GROUP	4
#define	STYP_PAD	8
#define	STYP_COPY	0x10
#define	STYP_TEXT	0x20
#define	STYP_DATA	0x40
#define	STYP_BSS	0x80
#define	STYP_ALIGN	0x100
#define	STYP_CLINK	0x4000


/************** Simbol Storage classes ******************/
#define		C_NULL		0
#define		C_AUTO      1
#define		C_EXT       2
#define		C_STAT      3
#define		C_REG       4
#define		C_EXTDEF    5
#define		C_LABEL     6
#define		C_ULABEL    7
#define		C_MOS       8
#define		C_ARG       9
#define		C_STRTAG    10
#define		C_MOU       11
#define		C_UNTAG     12
#define		C_TPDEF     13
#define		C_USTATIC   14
#define		C_ENTAG     15
#define		C_MOE       16
#define		C_REGPARAM  17
#define		C_FIELD     18
#define		C_BLOCK     100
#define		C_FCN       101
#define		C_EOS       102
#define		C_FILE      103
#define		C_LINE      104


#define		MAGIC_DSP21K	0x521C
#define		MAGIC_C25		0x0092
#define		MAGIC_C30		0x0093
#define		MAGIC_C6X		0x00C2

#endif	//_VCOFF_H_

//
// End of File
//



