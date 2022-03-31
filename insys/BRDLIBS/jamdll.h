// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the JAMDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// JAMDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef JAMDLL_EXPORTS
#define JAMDLL_API __declspec(dllexport)
#else
#define JAMDLL_API __declspec(dllimport)
#endif

typedef struct
{
	int size;
	int (*jtag_io)(int tms, int tdi, int read_tdo); 
} JAM_IO;

JAMDLL_API int jam_play( char* filename, char* action, JAM_IO *io );
JAMDLL_API char* jam_error( int nCode );
