// ***************************************************************
//  Adf4350.cpp   version:  1.0   ·  date: 12/05/2012
//  -------------------------------------------------------------
//  Вспомогательные функции для управления синтезатором ADF4350
//  -------------------------------------------------------------
//  Copyright (C) 2012 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#include <stdio.h>
#include "adf4350.h"


//***************************************************************************************
int	ADF4350_PllMode( double *pFref, double *pRate, ADF4350_TReg *paRegs, S32 arrSize, S32 *pRealSize )
{
	U32			divR, divINT, divFRAC, divMOD, divDO, divDOidx;
	double		divN;
	double		freqC, freqCmax, freqVco;

	*pRealSize = 7;
	if( arrSize < *pRealSize )
		return -1;

	if( *pRate < 137500000.0 )
		*pRate = 137500000.0;
	if( *pRate > 4400000000.0 )
		*pRate = 4400000000.0;

	//
	// Вычислить значение делителя DO
	//
	for( divDOidx=0; divDOidx<=5; divDOidx++ )
	{
		divDO = 1<<divDOidx;
		freqVco = (*pRate) * divDO;
		if( (freqVco>=2200000000.0) && (freqVco<=4400000000.0) )
			break;
	}
	if( divDOidx>=5 )
		return -1;

	//
	// Вычислить значение делителя R
	//
	freqCmax = freqVco / 75.5;
	if( freqCmax > 32000000.0 )
		freqCmax = 32000000.0;

	for( divR=1; divR<1024; divR++ )
	{
		freqC = (*pFref) / divR;
		if( freqC <= freqCmax )
			break;
	}

	//
	// Вычислить значение делителя N
	//
	divN = freqVco / freqC;
	divINT = (U32)floor( divN );
	divMOD = 1000;
	divFRAC = (U32)floor( (divN-divINT) * 1000.0 + 0.5 );

	//
	// Подготовить массив делителей 
	//
	paRegs[ 0].adr = 0x02;  paRegs[ 0].val = 0x0000000A;
	paRegs[ 1].adr = 0x05;  paRegs[ 1].val = 0x00010005;
	paRegs[ 2].adr = 0x04;  paRegs[ 2].val = 0x008FF1B4 | (divDOidx<<20);
	paRegs[ 3].adr = 0x03;  paRegs[ 3].val = 0x0000FFF3;
	paRegs[ 4].adr = 0x02;  paRegs[ 4].val = 0x60002EC2 | (divR<<14);
	paRegs[ 5].adr = 0x01;  paRegs[ 5].val = 0x08008001 | (divMOD<<3);
	paRegs[ 6].adr = 0x00;  paRegs[ 6].val = 0x00000000 | (divINT<<15) | (divFRAC<<3);

//printf( "\n" );
//printf( ">> Fref = %f, Frate = %f\n", *pFref, *pRate );
//printf( ">> R = %d, DO = %d, INT = %d, FRAC = %d, MOD = %d\n", divR, divDO, divINT, divFRAC, divMOD );
//for( int ridx=0; ridx<*pRealSize; ridx++ )
//	printf( ">> adr = %d, val = 0x%08X\n", paRegs[ridx].adr, paRegs[ridx].val );
//printf( "\n" );


	return 0;
}


//
// End of File
//

