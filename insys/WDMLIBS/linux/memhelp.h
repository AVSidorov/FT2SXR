
#ifndef _MEMHELP_H_
#define _MEMHELP_H_

//-----------------------------------------------------------------------------

#include "dmachan.h"

//-----------------------------------------------------------------------------

int lock_user_memory( SHARED_MEMORY_DESCRIPTION *MemDscr, void* userSpaceAddress, size_t userSpaceSize );
int unlock_user_memory( SHARED_MEMORY_DESCRIPTION *MemDscr );

//-----------------------------------------------------------------------------

#endif //_MEMHELP_H_
