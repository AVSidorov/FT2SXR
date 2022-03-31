//=********************************************************
//
// TSGDAQAPI.H
//
// InSys (C) written by Dorokhin A. Apr 2004
//
// Modifications:
//
//=********************************************************

#ifndef	__TSGDAQAPI_H_
#define	__TSGDAQAPI_H_

enum
{
	TSGDAQcmd_REGREADDIR,
	TSGDAQcmd_REGREADSDIR,
	TSGDAQcmd_REGREADIND,
	TSGDAQcmd_REGREADSIND,
	TSGDAQcmd_REGWRITEDIR,
	TSGDAQcmd_REGWRITESDIR,
	TSGDAQcmd_REGWRITEIND,
	TSGDAQcmd_REGWRITESIND
};

enum
{
	TSGDAQfmt_08 = 1,
	TSGDAQfmt_16 = 2,
	TSGDAQfmt_32 = 4,
	TSGDAQfmt_64 = 8
};

#pragma pack(push, 1)    

typedef struct
{  
	S32	cmd; 
	S32 tetr; 
	S32 reg; 
	U32 val;
	U32 format;
	S32 maxBytes;
} TSGDAQ_CMD_Reg, *PTSGDAQ_CMD_Reg;

#pragma pack(pop)

#endif	// __TSGDAQAPI_H_

//
// End of File
//

 