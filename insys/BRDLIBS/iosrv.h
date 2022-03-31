/*
 ****************** File Fm216x250mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : ADC section
 *
 * (C) InSys by Ekkore Nov 2011
 *
 ************************************************************
*/

#ifndef _IOSRV_H
 #define _IOSRV_H


#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "adcsrv.h"
#include "mainregs.h"





const int IO_TETR_ID = 0x9A; // tetrad id






class CIOSrv
{
protected:
	DEVS_CMD_Reg _iobuf[4096];

	volatile int _iobuf_len;

	int _iomode;
	volatile int _ionn;

	int pack;
	
	CModule* dev();

	CModule *_pModule;

public:

	
	CIOSrv( VOID *pModule );
	
	void detect();
	
	void _beginio();
	void _endio(bool br=false);

	S32 _io( U32 cmd , DEVS_CMD_Reg *pCmd );

	U32 _ri( U32 tetr, U32 reg );
	U32 _rd( U32 tetr, U32 reg );
	U32 _rdx( U32 tetr, U32 reg, U32 *pBuf, U32 size );

	void _wi( U32 tetr, U32 reg, U32 val );
	void _wd( U32 tetr, U32 reg, U32 val );

	U32 ri(  U32 reg );
	U32 wi(  U32 reg,  U32 val );

};

#endif // _MODBUSSRV_H

//
// End of file
//
