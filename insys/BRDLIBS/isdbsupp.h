//=********************************************************
// ISDBSUPP.H - ISDB1394 Support Function Declarations
//
// Used with BASE Driver 
//
// (C) Instr.Sys. by Ekkore Sep. 2004
//
//=********************************************************

#ifndef _ISDBSUPP_H_
#define _ISDBSUPP_H_

S32		ISDB_AsyncRead( HANDLE hWDM, U32 adrHi, U32 adrLo, void *pBuf, U32 sizeb, S32 isSwap=1 );
S32		ISDB_AsyncWrite( HANDLE hWDM, U32 adrHi, U32 adrLo, void *pBuf, U32 sizeb, S32 isSwap=1 );
S32		ISDB_ReadConfROM( HANDLE hWDM, void *pBuf, U32 sizeb );
S32		ISDB_GetBoardType( HANDLE hWDM, U32 *pPid, U32 *pType );

#endif //_ISDBSUPP_H_

//
// End of File
//


