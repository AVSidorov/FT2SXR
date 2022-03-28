///////////////////////////////////////////////////////////////////////
//	BASEDACREGS.H - ADP-board definitions
//	Copyright (c) 2004, Instrumental Systems,Corp.
///////////////////////////////////////////////////////////////////////

#ifndef _BASEDACREGS_H_
#define _BASEDACREGS_H_

#pragma pack(push,1)

// BASE DAC Mode1 register (+9)
typedef union _BASEDAC_MODE1 {
  ULONG AsWhole; // BASE DAC Mode1 Register as a Whole Word
  struct { // BASE DAC Mode1 Register as Bit Pattern
   ULONG ChanSel	: 2,	// Channel select
		 Reserved	: 14;	// Reserved
  } ByBits;
} BASEDAC_MODE1, *PBASEDAC_MODE1;

#pragma pack(pop)

#endif //_BASEDACREGS_H_
