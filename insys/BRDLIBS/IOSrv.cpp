/*
 ***************** File Fm216x250mSrv.cpp ************
 *
 * BRD Driver for ADС service on FM216x250M
 *
 * (C) InSys by Ekkore Nov 2011
 *
 ******************************************************
*/
#ifndef __linux__
#include <windows.h>
#include <winioctl.h>
#endif
#include "gipcy.h"
#include "module.h"
#include "realdelay.h"
#include "IOSrv.h"
#include "submodule.h"

#define	CURRFILE "IOSRV"


//***************************************************************************************
CIOSrv::CIOSrv( PVOID pModule ) 
{
	_iobuf_len =0;
	_iomode =0;
	_ionn = 0;

	pack = 0;

	memset( (void*)_iobuf, 0, sizeof(_iobuf) );
	
	_pModule = (CModule*)pModule;

}

void CIOSrv::detect()
{
	DEVS_CMD_Reg pck;

	DEVS_CMD_Reg cmdbuf[2];

	cmdbuf[0].idxMain = DEVScmd_REGREADDIR;
	cmdbuf[0].tetr = 0;
	cmdbuf[0].reg = 0;	
	
	cmdbuf[1].idxMain = DEVScmd_REGREADDIR;
	cmdbuf[1].tetr = 0;
	cmdbuf[1].reg = 0;

	pck.bytes = sizeof(cmdbuf);
	pck.pBuf = cmdbuf;

	
	CModule *pDev = dev();	
	S32 err = pDev->RegCtrl( DEVScmd_PACKEXECUTE, &pck );

	if( err >= 0 )
		pack = 1;

}	

void CIOSrv::_beginio()
{


	if( _ionn == 0 )
	{
		_iobuf_len = 0;
		_iomode = 1;
	}	
	
	_ionn++;
}	

CModule* CIOSrv::dev()
{
	CSubModule *pDev = (CSubModule*)_pModule;
	
	return  (CModule*)pDev;
}

volatile int g_io_dump = 0;

void CIOSrv::_endio(bool br)
{
	_ionn--;

	if( (_iomode==1) && (_ionn > 0) && !br )
		return;

	_ionn = 0;

	CModule *pDev = dev();

	if( (pack == 1) && (_iobuf_len > 1) )
	{
		DEVS_CMD_Reg pck;

		pck.bytes = _iobuf_len*sizeof(DEVS_CMD_Reg);
		pck.pBuf = (void*)_iobuf;

	
		
		pDev->RegCtrl( DEVScmd_PACKEXECUTE, &pck );
	}
	else
	{

		for( int i=0; i<_iobuf_len; i++ )
		{
			pDev->RegCtrl( _iobuf[i].idxMain, (PDEVS_CMD_Reg)&_iobuf[i] );
		}
	}

	if( g_io_dump )
	{
		FILE *f = fopen( "io_dump.bin", "w"  );

		for( int i=0; i<_iobuf_len; i++ )
		{
			fprintf( f,"--------------" );
			fprintf( f,"idxMain: 0x%X", _iobuf[i].idxMain );
			fprintf( f,"reg: 0x%X", _iobuf[i].reg );
			fprintf( f,"tetr: 0x%X", _iobuf[i].tetr );
			fprintf( f,"val: 0x%X", _iobuf[i].val );
			
		}

		fclose( f );

		g_io_dump = 0;
	}

	_iomode = 0;
	_iobuf_len=0;
}	

S32 CIOSrv::_io( U32 cmd , DEVS_CMD_Reg *pCmd )
{
	

	_iobuf[_iobuf_len] = *pCmd;
	_iobuf[_iobuf_len].idxMain = cmd;
	_iobuf_len++;

	if( _iomode == 0 )
		_endio(true);

	return 0;
}

U32 CIOSrv::_ri( U32 tetr, U32 reg )
{
	CModule *pDev = dev();

	DEVS_CMD_Reg RegParam;

	RegParam.tetr = tetr;
	RegParam.reg = reg;

	if( _iomode != 0 )
		_endio(true);
		
	S32 err = pDev->RegCtrl( DEVScmd_REGREADIND, &RegParam );
	
	return RegParam.val;
}	

U32 CIOSrv::_rd( U32 tetr, U32 reg )
{
	CModule *pDev = dev();
	
	DEVS_CMD_Reg RegParam;

	RegParam.tetr = tetr;
	RegParam.reg = reg;
	
	if( _iomode != 0 )
		_endio(true);
		
	S32 err = pDev->RegCtrl( DEVScmd_REGREADDIR, &RegParam );
	
	return RegParam.val;
}	
	
U32 CIOSrv::_rdx( U32 tetr, U32 reg, U32 *pBuf, U32 size )
{
	DEVS_CMD_Reg RegParam;

	RegParam.tetr = tetr;
	RegParam.reg = reg;
	
	if( _iomode != 0 )
		_endio(true);


	_beginio();

	for( volatile int i=0; i<size; i++ )
	{
		_iobuf[_iobuf_len] = RegParam;
		_iobuf[_iobuf_len].idxMain = DEVScmd_REGREADDIR;
		_iobuf_len++;
	}

	_endio();

	if( pBuf != NULL )
	{
		for( volatile int i=0; i<size; i++ )
		{
			pBuf[i] = _iobuf[i].val;
		}
	}
	
	return 0;
}

void CIOSrv::_wi( U32 tetr, U32 reg, U32 val )
{
	DEVS_CMD_Reg RegParam;

	RegParam.tetr = tetr;
	RegParam.reg = reg;
	RegParam.val = val;

	S32 err = _io( DEVScmd_REGWRITEIND, &RegParam );
}	

void CIOSrv::_wd( U32 tetr, U32 reg, U32 val )
{
	volatile DEVS_CMD_Reg RegParam;

	RegParam.tetr = tetr;
	RegParam.reg = reg;
	RegParam.val = val;

	S32 err = _io( DEVScmd_REGWRITEDIR, (DEVS_CMD_Reg*)&RegParam );
}




// ***************** End of file Fm216x250mSrv.cpp ***********************
