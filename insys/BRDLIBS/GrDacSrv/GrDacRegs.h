/*
 ****************** File GrDacRegs.h *******************
 *
 *  Definitions of tetrad register
 *	structures and constants
 *	for QM9957
 *
 * (C) InSys by Sklyarov A. Feb. 2011
 *
 *************************************************************
*/

#ifndef _GRDACREGS_H_
 #define _GRDACREGS_H_

#pragma pack(push,1)


// GRDAC Mode1 Register (+9)
typedef union _GRDAC_MODE1 {
	ULONG AsWhole;	// Mode1 Register as a Whole Word
	struct {		// Mode1 Register as Bit Pattern
	ULONG	mute		: 1, // pin MUTE chip (1- disable out)
			fmt			: 2, // pins FMT chip (Format input data)
			mono		: 1, // pin MONO chip ( 0-stereo, 1- mono) 
			chsl		: 1, // pin CHSL chip ( 0-left channel, 1-right channel)
			dem			: 1, // pin CHSL chip
			test		: 2, // output: 0-?,1-?,2-?,3-?
			rate		: 2, // sample rate: 0-48kHz, 1-96kHz,2-192kHz
			res			: 6; // Reserved
	} ByBits;
} GRDAC_MODE1, *PGRDAC_MODE1;

//#define GRADC_PROG 0x201;


#pragma pack(pop)

#endif //__GRDACREGS_H_

//
// End of file
//