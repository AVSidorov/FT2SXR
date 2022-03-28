/*
	***** fmove32.h *****
	Moves functions
*/

#ifndef _FMOVE32_H_
#define _FMOVE32_H_

#include "utypes.h"

#ifndef __linux__
//------------------------------------------------------------------------------
__inline UINT08 Inp08(UINT16 port)
{
    UINT08 data;

    __asm mov dx, port
    __asm in  al, dx
    __asm mov data,  al

    return data;
}

//------------------------------------------------------------------------------
__inline UINT16 Inp16(UINT16 port)
{
    UINT16 data;

    __asm mov dx,   port
    __asm in  ax,   dx
    __asm mov data, ax

    return data;
}

//------------------------------------------------------------------------------
__inline UINT32 Inp32(UINT16 port)
{
    UINT32 data;

    __asm mov dx,   port
    __asm in  eax,  dx
    __asm mov data, eax
    return data;
}

//------------------------------------------------------------------------------
__inline void Outp08(UINT16 port, UINT08 data)
{
    __asm mov dx, port
    __asm mov al, data
    __asm out dx, al
}

//------------------------------------------------------------------------------
__inline void Outp16(UINT16 port, UINT16 data)
{
    __asm mov dx, port
    __asm mov ax, data
    __asm out dx, ax
}

//------------------------------------------------------------------------------
__inline void Outp32(UINT16 port, UINT32 data)
{
    __asm mov dx,  port
    __asm mov eax, data
    __asm out dx,  eax
}

//------------------------------------------------------------------------------
// Read array from port 8 bits
__inline void	Ins08(UINT32 port, void *p, UINT32 len)
{
	  __asm	mov  edx, port
	  __asm mov  ecx, len
	  __asm	mov  edi, p
	  __asm	rep  ins
}

//------------------------------------------------------------------------------
// Read array from port 16 bits
__inline void	Ins16(UINT32 port, void *p, UINT32 len)
{
	  __asm	mov  edx, port
	  __asm mov  ecx, len
	  __asm	mov  edi, p
	  __asm	rep  insw
}

//------------------------------------------------------------------------------
// Read array from port 32 bits
__inline void	Ins32(UINT32 port, void *p, UINT32 len)
{
	  __asm	mov  edx, port
	  __asm mov  ecx, len
	  __asm	mov  edi, p
	  __asm	rep  insd
}

//------------------------------------------------------------------------------
// Write array to port 8 bits
__inline void	Outs08(UINT32 port, void *p, UINT32 len)
{
	  __asm	mov  edx, port
	  __asm mov  ecx, len
	  __asm	mov  esi, p
	  __asm	rep  outs
}

//------------------------------------------------------------------------------
// Write array to port 16 bits
__inline void	Outs16(UINT32 port, void *p, UINT32 len)
{
	  __asm	mov  edx, port
	  __asm mov  ecx, len
	  __asm	mov  esi, p
	  __asm	rep  outsw
}

//------------------------------------------------------------------------------
// Write array to port 32 bits
__inline void	Outs32(UINT32 port, void *p, UINT32 len)
{
	  __asm	mov  edx, port
	  __asm mov  ecx, len
	  __asm	mov  esi, p
	  __asm	rep  outsd
}

//------------------------------------------------------------------------------
__inline void	Move08(void *src, void *dst, UINT32 count)
{
	__asm{
		cld
		mov	ecx, count
		mov	edi, dst
		mov	esi, src
		rep	movsb
	}
}

//------------------------------------------------------------------------------
//	Move memory to memory 32 bits
__inline void	Move16(void *src, void *dst, UINT32 count)
{
	__asm{
		cld
		mov	ecx, count
		mov	edi, dst
		mov	esi, src
		rep	movsw
	}
}

//------------------------------------------------------------------------------
__inline void	Move32(void *src, void *dst, UINT32 count)
{
	__asm{
		cld
		mov	ecx, count
		mov	edi, dst
		mov	esi, src
		rep	movsd
	}
}
#else
//------------------------------------------------------------------------------
__inline void	Move32(void *src, void *dst, UINT32 count)
{
    memcpy(dst, src, count * 4);
}
#endif


#endif // _FMOVE32_H_

//
//  End of File
///

