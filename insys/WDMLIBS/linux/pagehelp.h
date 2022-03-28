
#ifndef _PAGEHELP_H_
#define _PAGEHELP_H_

//-----------------------------------------------------------------------------

int lock_pages( void *va, u32 size );
int unlock_pages( void *va, u32 size );
int check_address( void *pMemUserAddr );
int check_size( size_t userSize );

//--------------------------------------------------------------------

#endif //_PAGEHELP_H_
