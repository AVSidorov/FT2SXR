
#include "brdapi.h"
#include "stdio.h"

/*

Usage:



*/

class RegLog
{
public:

	static FILE *f;

	static void Log( U32 cmd, PDEVS_CMD_Reg pCmd );

	static const int READ = 1;
	static const int WRITE = 2;
	
	static int mode;

protected:

	static BRDCHAR* TetrName( U32 id );
	static BRDCHAR* RegName( U32 trd,U32 id );

};

class RegLogFile
{
public:
	RegLogFile( BRDCHAR *filename )
	{
#ifdef _WIN64
		RegLog::f = _wfopen( filename, _BRDC("w") );
#else
		RegLog::f = fopen( filename, "w" );
#endif
	}

};

#define REGLOG_MODE( rwx ) int RegLog::mode = rwx;////READ | WRITE
#define REGLOG_FILE( f ) RegLogFile RegLogFile__f( _BRDC( f ) );