// ***************************************************************
//  Ad9518.h   version:  1.0   ·  date: 12/05/2012
//  -------------------------------------------------------------
//  Вспомогательные функции для управления синтезатором AD9518
//  -------------------------------------------------------------
//  Copyright (C) 2012 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#ifndef _AD9518_H
#define _AD9518_H

#include "utypes.h"
#include "useful.h"

typedef struct { U32 adr, val; } AD9518_TReg;
typedef struct { int divA, divB, divP, divR, divDo, divPll; double rate; } AD9518_TDividers;

int	AD9518_DividerMode( double *pClk, double *pRate, AD9518_TReg *paRegs, S32 arrSize, S32 *realSize );
int	AD9518_PllMode( double *pClk, double *pRate, AD9518_TReg *paRegs, S32 arrSize, S32 *realSize, U32 nSyntType );
int	AD9518_DirectPllMode( AD9518_TDividers *pDividers, AD9518_TReg *paRegs, S32 arrSize, S32 *realSize );

#endif //_AD9518_H
//
// End of File
//

