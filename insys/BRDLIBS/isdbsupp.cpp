//=********************************************************
// ISDBSUPP.CPP - ISDB1394 Support Function Definitions
//
// Used with BASE Driver 
//
// (C) Instr.Sys. by Ekkore Sep. 2004
//
//=********************************************************

#include	<string.h>
#include	<windows.h>
#include	<winioctl.h>

#include	"utypes.h"
#include	"isdbsupp.h"
#include	"..\isdb1394\wdm\ddisdb.h"

//=************************ ISDB_AsyncRead **************
//=******************************************************
S32		ISDB_AsyncRead( HANDLE hWDM, U32 adrHi, U32 adrLo, void *pBuf, U32 sizeb, S32 isSwap )
{
	TIAsyncRead	rParam;

    ULONG		length;       
    DWORD		err;
	DWORD		cmd = (isSwap==0) ? IOCTL_ISDB1394_ASYNC_READ : IOCTL_ISDB1394_ASYNC_READ_N_SWAP;


	rParam.adrHi = adrHi;
	rParam.adrLo = adrLo;
	rParam.size  = sizeb;

    if( !DeviceIoControl( hWDM, cmd, &rParam, sizeof(rParam), pBuf, sizeb, &length, NULL)) 
	{
        err = GetLastError();
        return -1;
    }

    return 0;
}

//=************************ ISDB_AsyncWrite *************
//=******************************************************
S32		ISDB_AsyncWrite( HANDLE hWDM, U32 adrHi, U32 adrLo, void *pBuf, U32 sizeb, S32 isSwap )
{
	TIAsyncWrites	rParam;

    ULONG		length;       
    DWORD		err;
	DWORD		cmd;
	
	if( sizeb==0 )
		cmd = IOCTL_ISDB1394_ASYNC_WRITE;
	else
		cmd = (isSwap==0) ? IOCTL_ISDB1394_ASYNC_WRITES: IOCTL_ISDB1394_ASYNC_WRITES_N_SWAP;

	rParam.adrHi = adrHi;
	rParam.adrLo = adrLo;
	rParam.size  = sizeb;
	rParam.ptr   = (U32)pBuf;

    if( !DeviceIoControl( hWDM, cmd, &rParam, sizeof(rParam), NULL, 0, &length, NULL)) 
	{
        err = GetLastError();
        return -1;
    }

    return 0;
}

//=************************ ISDB_ReadConfROM ************
//=******************************************************
S32		ISDB_ReadConfROM( HANDLE hWDM, void *pBuf, U32 sizeb )
{
	return ISDB_AsyncRead( hWDM, 0xFFFF, 0xF0000400, pBuf, sizeb );
}

//=************************ ISDB_ReadConfROM ************
//=******************************************************
S32		ISDB_GetBoardType( HANDLE hWDM, U32 *pPid, U32 *pType )
{
	U32		anConfRom[128];

	if( 0>ISDB_ReadConfROM( hWDM, anConfRom, sizeof(anConfRom) ) )
		return -1;

	//
	// Form PID
	//
	*pPid = anConfRom[4] >> 8;
	*pPid|= anConfRom[3] << 24;

	//
	// Form Module Type
	//
	U32		lenBI;	// Length of Bus_Info
	U32		lenRD;	// Length of Root_Directory
	U32		lenC3;	// Length of Vendor_Info_Directory (0xC3)
	U32		idxC3;	// Index  of Vendor_Info_Directory (0xC3)
	U32		ii;

	*pType = 0;

	lenBI = 0xFF & (anConfRom[0] >> 24);
	lenRD = 0xFFFF & (anConfRom[lenBI+1] >> 16);
	idxC3 = 0;
	for( ii=0; ii<lenRD; ii++ )
	{
		if( (anConfRom[lenBI+2+ii] >> 24) == 0xC3 )
		{
			idxC3 = lenBI+2+ii + (anConfRom[lenBI+2+ii] & 0xFFFF);
			break;
		}
	}
	if( idxC3==0 )
		return 0;

	lenC3 = 0xFFFF & (anConfRom[idxC3] >> 16);
	for( ii=0; ii<lenC3; ii++ )
	{
		if( (anConfRom[idxC3+1+ii] >> 24) == 0x17 )
		{
			*pType = anConfRom[idxC3+1+ii] & 0xFFFF;
			break;
		}
	}

	return 0;
}

//
// End of File
//


