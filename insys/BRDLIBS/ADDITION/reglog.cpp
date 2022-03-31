
#include <stdio.h>
#include "reglog.h"

FILE* RegLog::f = NULL;

BRDCHAR trdname[][32] = {

	_BRDC("MAIN")

};

BRDCHAR regname[][32] = {

	_BRDC("MODE0")

};

BRDCHAR* RegLog::TetrName( U32 id )
{
	static BRDCHAR Buffer[64];

	if( id < sizeof(trdname)/sizeof(trdname[0]) )
	{
		return trdname[id];
	}

	BRDC_sprintf(Buffer,_BRDC("0x%x"),id);
	 
	return Buffer;
}

BRDCHAR* RegLog::RegName( U32 trd, U32 id )
{	
	static BRDCHAR Buffer[64];

	if( id < sizeof(regname)/sizeof(regname[0]) )
	{
		return regname[id];
	}
		
	BRDC_sprintf(Buffer,_BRDC("0x%x"),id);
	 
	return Buffer;
}

void RegLog::Log( U32 cmd,  PDEVS_CMD_Reg pCmd )
{

	if( f == NULL )
		f = stdout;
		

	BRDCHAR* cname = NULL;


	DEVS_CMD_Reg *pa = (DEVS_CMD_Reg*)pCmd;
	


	switch( cmd )
	{
	case DEVScmd_REGREADDIR:
		if( mode & READ )
			cname = _BRDC("REGREADDIR");
		break;	
	
	case DEVScmd_REGREADIND:
		if( mode & READ )
			cname = _BRDC("REGREADIND");
		break;
	
	case DEVScmd_REGWRITEDIR:
		if( mode & WRITE )
			cname = _BRDC("REGWRITEDIR");
		break;	
	
	case DEVScmd_REGWRITEIND:
		if( mode & WRITE )
			cname = _BRDC("REGWRITEIND");
		break;
		
	}

	BRDCHAR *tname = TetrName( pa->tetr );
	BRDCHAR *rname = RegName(  pa->tetr, pa->reg );

	if( cname )
		BRDC_fprintf( f, _BRDC("%s %s %s 0x%x\n"), cname, tname, rname, pa->val );


}