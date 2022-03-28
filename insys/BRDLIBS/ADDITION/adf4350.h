// ***************************************************************
//  Adf4350.h   version:  1.0   ·  date: 12/05/2012
//  -------------------------------------------------------------
//  Вспомогательные функции для управления синтезатором ADF4350
//  -------------------------------------------------------------
//  Copyright (C) 2012 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#ifndef _ADF4350_H
#define _ADF4350_H

#include "utypes.h"
#include "useful.h"

typedef struct { U32 adr, val; } ADF4350_TReg;
typedef struct { int divA, divB, divP, divR, divDo, divPll; double rate; } ADF4350_TDividers;

int	ADF4350_PllMode( double *pClk, double *pRate, ADF4350_TReg *paRegs, S32 arrSize, S32 *realSize );

#endif //_ADF4350_H
//
// End of File
//

