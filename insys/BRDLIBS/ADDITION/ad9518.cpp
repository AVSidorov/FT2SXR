// ***************************************************************
//  Ad9518.cpp   version:  1.0   ·  date: 12/05/2012
//  -------------------------------------------------------------
//  Вспомогательные функции для управления синтезатором AD9518
//  -------------------------------------------------------------
//  Copyright (C) 2012 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#include <stdio.h>
#include "ad9518.h"

//***************************************************************************************
int	AD9518_DividerMode( double *pClk, double *pRate, AD9518_TReg *paRegs, S32 arrSize, S32 *pRealSize )
{
	int		div;
	int		divPll, divDo;
	int		isOk = 0;
	U32		reg0x190, reg0x191, reg0x192, reg0x1E0, reg0x1E1;


	if( *pRate == 0.0 )
		return -1;
	
	*pRealSize = 24;
	if( arrSize < *pRealSize )
		return -1;

	//
	// Рассчитать значения двух делителей
	//
	div = ROUND(*pClk / *pRate);
	if( div < 1 )
		div = 1;
	if( div <= 32 )
	{
		divDo = div;
		divPll = 1;
		isOk = 1;
	}
	if( !isOk )
	{
		for( divPll=2; divPll<=6; divPll++ )
		{
			if( ((div%divPll)==0) && ((div/divPll)<=32) )
			{
				divDo = div/divPll;
				divPll = divPll;
				isOk = 1;
				break;
			}
		}
	}
	if( !isOk )
	{
		for( divPll=2; divPll<=6; divPll++ )
		{
			if( (div/divPll)<=32 )
			{
				divDo = div/divPll;
				divPll = divPll;
				isOk = 1;
				break;
			}
		}
	}
	if( !isOk )
	{
		divDo = 32;
		divPll = 6;
		isOk = 1;
	}

	*pRate = *pClk / divPll / divDo;

	//
	// Рассчитать коды регистров reg0x190, reg0x191, reg0x192, reg0x1E0, reg0x1E1
	//
	if( divPll == 1 )
	{
		if( divDo == 1 )
		{
			reg0x190 = 0x00;
			reg0x191 = 0x80;
			reg0x192 = 0x02;
			reg0x1E0 = 0x00;
			reg0x1E1 = 0x00;
		}
		else
		{
			reg0x190 = (divDo/2-1)<<4;
			reg0x190|= (divDo/2-1) + (divDo%2);
			reg0x191 = 0x00;
			reg0x192 = 0x00;
			reg0x1E0 = 0x00;
			reg0x1E1 = 0x09;
		}
	}
	else
	{
		reg0x190 = (divDo/2-1)<<4;
		reg0x190|= (divDo/2-1) + (divDo%2);
		reg0x191 = 0x00;
		reg0x192 = 0x00;
		reg0x1E0 = divPll - 2;
		reg0x1E1 = 0x00;
	}

	//
	// Подготовить массив делителей 
	//

	paRegs[ 0].adr = 0x000; paRegs[ 0].val = 0x18;
	paRegs[ 1].adr = 0x010; paRegs[ 1].val = 0x71;
	paRegs[ 2].adr = 0x01D; paRegs[ 2].val = 0x12;
	paRegs[ 3].adr = 0x0F0; paRegs[ 3].val = 0x04;
	paRegs[ 4].adr = 0x0F1; paRegs[ 4].val = 0x04;
	paRegs[ 5].adr = 0x0F2; paRegs[ 5].val = 0x0C;
	paRegs[ 6].adr = 0x0F3; paRegs[ 6].val = 0x08;
	paRegs[ 7].adr = 0x0F4; paRegs[ 7].val = 0x08;
	paRegs[ 8].adr = 0x0F5; paRegs[ 8].val = 0x04;

	paRegs[ 9].adr = 0x190; paRegs[ 9].val = reg0x190;
	paRegs[10].adr = 0x191; paRegs[10].val = reg0x191;
	paRegs[11].adr = 0x192; paRegs[11].val = reg0x192;
	paRegs[12].adr = 0x193; paRegs[12].val = reg0x190;
	paRegs[13].adr = 0x194; paRegs[13].val = reg0x191;
	paRegs[14].adr = 0x195; paRegs[14].val = reg0x192;
	paRegs[15].adr = 0x196; paRegs[15].val = reg0x190;
	paRegs[16].adr = 0x197; paRegs[16].val = reg0x191;
	paRegs[17].adr = 0x198; paRegs[17].val = reg0x192;
	paRegs[18].adr = 0x1E0; paRegs[18].val = reg0x1E0;
	paRegs[19].adr = 0x1E1; paRegs[19].val = reg0x1E1;

	paRegs[20].adr = 0x230; paRegs[20].val = 0x01;
	paRegs[21].adr = 0x232; paRegs[21].val = 0x01;
	paRegs[22].adr = 0x230; paRegs[22].val = 0x00;
	paRegs[23].adr = 0x232; paRegs[23].val = 0x01;

	return 0;
}

//***************************************************************************************
int	AD9518_PllMode( double *pFref, double *pRate, AD9518_TReg *paRegs, S32 arrSize, S32 *pRealSize, U32 nSyntType )
{
	typedef struct { double minFvco, maxFvco; } TRangeFvco;
	TRangeFvco		aRangeFvco[5] =
					{
						{ 2550000000.0, 2950000000.0 },
						{ 2300000000.0, 2650000000.0 },
						{ 2050000000.0, 2330000000.0 },
						{ 1750000000.0, 2250000000.0 },
						{ 1450000000.0, 1800000000.0 }
					};
	double	minFvco, maxFvco, dFvco;
	int		divPll, divDo;
	int		divP, divR, divA, divB;
	int		divRmin, divRmax;
	int		isOk;

	U32		reg0x011, reg0x012, reg0x013, reg0x014, reg0x015, reg0x016;
	U32		reg0x190, reg0x191, reg0x192, reg0x1E0, reg0x1E1;

	*pRealSize = 36;
	if( arrSize < *pRealSize )
		return -1;

	if( *pRate == 0.0 )
		return -1;

	//
	// Опредклить диапазон настройки ГУН
	//
	if( (nSyntType < 1) || (nSyntType > 5) )
		return -1;
	minFvco = aRangeFvco[nSyntType - 1].minFvco;
	maxFvco = aRangeFvco[nSyntType - 1].maxFvco;

	//
	// Рассчитать значение Fvco
	//
	isOk = 0;
	for( divPll=2; divPll<=6; divPll++ )
	{	for( divDo=1; divDo<=32; divDo++ )
		{
			dFvco = (*pRate) * divPll * divDo;
			if( (dFvco>=minFvco) && (dFvco<=maxFvco) )
			{
				isOk = 1;
				break;
			}
		}
		if( isOk )
			break;
	}
	if( 0 == isOk )
		return -1;

	//
	// Рассчитать делитель P
	//
	divP = (int)floor(dFvco/300000000.0);
	if( divP <= 8 )
		divP  = 8;
	else
		divP  = 16;

	//
	// Рассчитать границы делителя R
	//
	divRmin = (int)floor( (*pFref) / 100000000.0 );
	if( divRmin < 1 )
		divRmin = 1;

	divRmax = (int)ceil( (*pFref) * (divP * 8191 + 63) / dFvco );
	if( divRmax > 16383 )
		divRmax = 16383;

	//
	// Поиск коэффициентов A, B и R
	//
	double		dDiff = 1000000000.0;
	double		dDiffTmp, dFvcoTmp;
	int			iiA, iiR;
	double		tmpB;

	for( iiA=0; iiA<64; iiA++ )
	{
		for( iiR = divRmin; iiR<=divRmax; iiR++ )
		{
			tmpB = (dFvco * iiR / (*pFref) - iiA) / divP;
			tmpB = floor( tmpB + 0.5 );
			if( (3.0>tmpB) || (tmpB>8191.0) )
				continue;
			dFvcoTmp = (*pFref) * (divP*tmpB + iiA) /  iiR;
			dDiffTmp = 1 - dFvcoTmp/dFvco;
			if( 0>dDiffTmp )
				dDiffTmp = -dDiffTmp;
			if( dDiffTmp < dDiff )
			{
				dDiff = dDiffTmp;
				divA = iiA;
				divR = iiR;
				divB = (int)tmpB;
			}
			if( dDiff <= 0.000000001 )
				break;
		}
		if( dDiff <= 0.000000001 )
			break;
	}
	if( dDiff > 1000000.0 )
		return -1;

	//
	// SubClock = 200 МГц
	// Sampling Rate = 250 МГц
	//
	//divPll = 2;
	//divDo  = 4;

	//divP = 8;
	//divA = 0;
	//divB = 5;
	//divR = 4;
	double dRate;
	dFvco = (*pFref) * (divP*tmpB + iiA) /  iiR;
	dRate = dFvco / divDo / divPll;
//printf( "\n>>PLL Mode: fref = %f, rate = %f (kHz)\n", (*pFref)/1000.0,dRate/1000.0 );
//printf( ">>Pll=%d, Do=%d, P=%d, R=%d, A=%d, B=%d\n\n", divPll, divDo, divP, divR, divA, divB );
	
	*pRate = dRate;

	//
	// Рассчитать коды регистров reg0x011 - reg0x016
	//
	reg0x011 = 0xFF & divR;
	reg0x012 = 0x3F & (divR>>8);
	reg0x013 = 0x3F & divA;
	reg0x014 = 0xFF & divB;
	reg0x015 = 0x1F & (divB>>8);
	reg0x016 = (divP==8) ? 0x04 : 0x05;


	//
	// Рассчитать коды регистров reg0x190, reg0x191, reg0x192, reg0x1E0, reg0x1E1
	// Учти, что divPll имеет значения 2..6
	{
		reg0x190 = ((divDo/2-1) + (divDo%2)) << 4;
		reg0x190|= (divDo/2-1);
		reg0x191 = (divDo==1) ? 0x80 : 0x00;
		reg0x192 = 0x00;
		reg0x1E0 = divPll - 2;
		reg0x1E1 = 0x02;
	}

	//
	// Подготовить массив делителей 
	//
	paRegs[ 0].adr = 0x000;  paRegs[ 0].val = 0x18;
	paRegs[ 1].adr = 0x010;  paRegs[ 1].val = 0x0C;
	paRegs[ 2].adr = 0x011;  paRegs[ 2].val = reg0x011;
	paRegs[ 3].adr = 0x012;  paRegs[ 3].val = reg0x012;
	paRegs[ 4].adr = 0x013;  paRegs[ 4].val = reg0x013;
	paRegs[ 5].adr = 0x014;  paRegs[ 5].val = reg0x014;
	paRegs[ 6].adr = 0x015;  paRegs[ 6].val = reg0x015;
	paRegs[ 7].adr = 0x016;  paRegs[ 7].val = reg0x016;
	paRegs[ 8].adr = 0x017;  paRegs[ 8].val = 0x00;
	paRegs[ 9].adr = 0x018;  paRegs[ 9].val = 0x06;
	paRegs[10].adr = 0x019;  paRegs[10].val = 0x80;
	paRegs[11].adr = 0x01A;  paRegs[11].val = 0x00;
	paRegs[12].adr = 0x01B;  paRegs[12].val = 0x00;
	paRegs[13].adr = 0x01C;  paRegs[13].val = 0x11;
	paRegs[14].adr = 0x01D;  paRegs[14].val = 0x12;

	paRegs[15].adr = 0x0F0;  paRegs[15].val = 0x00;
	paRegs[16].adr = 0x0F1;  paRegs[16].val = 0x00;
	paRegs[17].adr = 0x0F2;  paRegs[17].val = 0x00;
	paRegs[18].adr = 0x0F3;  paRegs[18].val = 0x00;
	paRegs[19].adr = 0x0F4;  paRegs[19].val = 0x00;
	paRegs[20].adr = 0x0F5;  paRegs[20].val = 0x00;	

	paRegs[21].adr = 0x190;  paRegs[21].val = reg0x190;
	paRegs[22].adr = 0x191;  paRegs[22].val = reg0x191;
	paRegs[23].adr = 0x192;  paRegs[23].val = reg0x192;
	paRegs[24].adr = 0x193;  paRegs[24].val = reg0x190;
	paRegs[25].adr = 0x194;  paRegs[25].val = reg0x191;
	paRegs[26].adr = 0x195;  paRegs[26].val = reg0x192;
	paRegs[27].adr = 0x196;  paRegs[27].val = reg0x190;
	paRegs[28].adr = 0x197;  paRegs[28].val = reg0x191;
	paRegs[29].adr = 0x198;  paRegs[29].val = reg0x192;
	paRegs[30].adr = 0x1E0;  paRegs[30].val = reg0x1E0;
	paRegs[31].adr = 0x1E1;  paRegs[31].val = reg0x1E1;

	paRegs[32].adr = 0x230;  paRegs[32].val = 0x01;
	paRegs[33].adr = 0x232;  paRegs[33].val = 0x01;
	paRegs[34].adr = 0x230;  paRegs[34].val = 0x00;
	paRegs[35].adr = 0x232;  paRegs[35].val = 0x01;

	return 0;
}

//***************************************************************************************
int	AD9518_DirectPllMode( AD9518_TDividers *pDividers, AD9518_TReg *paRegs, S32 arrSize, S32 *realSize )
{
	U32		reg0x011, reg0x012, reg0x013, reg0x014, reg0x015, reg0x016;
	U32		reg0x190, reg0x191, reg0x192, reg0x1E0, reg0x1E1;

	//
	// Скорректировать значения делителей
	//
	if( pDividers->divPll < 2 ) pDividers->divPll = 2;
	if( pDividers->divPll > 6 ) pDividers->divPll = 6;
	if( pDividers->divDo < 1  ) pDividers->divDo = 1;
	if( pDividers->divDo > 32 ) pDividers->divDo = 32;
	if( pDividers->divA < 0  ) pDividers->divA = 0;
	if( pDividers->divA > 63 ) pDividers->divA = 63;
	if( pDividers->divB < 3  ) pDividers->divB = 3;
	if( pDividers->divB > 8191 ) pDividers->divB = 8191;
	if( pDividers->divR < 1  ) pDividers->divR = 1;
	if( pDividers->divR > 16383 ) pDividers->divR = 16383;

	if( pDividers->divP >= 32 ) pDividers->divP = 32;
	else if( pDividers->divP >= 16 ) pDividers->divP = 16;
	else if( pDividers->divP >= 8 ) pDividers->divP = 8;
	else if( pDividers->divP >= 4 ) pDividers->divP = 4;
	else if( pDividers->divP >= 2 ) pDividers->divP = 2;
	else pDividers->divP = 1;

	//printf( "\nPll=%d, Do=%d, P=%d, R=%d, A=%d, B=%d\n\n", 
	//	pDividers->divPll, pDividers->divDo, pDividers->divP, pDividers->divR, pDividers->divA, pDividers->divB );

	//
	// Рассчитать коды регистров reg0x011 - reg0x016
	//
	reg0x011 = 0xFF & pDividers->divR;
	reg0x012 = 0x3F & (pDividers->divR>>8);
	reg0x013 = 0x3F & pDividers->divA;
	reg0x014 = 0xFF & pDividers->divB;
	reg0x015 = 0x1F & (pDividers->divB>>8);
	reg0x016 = (pDividers->divP==8) ? 0x04 : 0x05;


	//
	// Рассчитать коды регистров reg0x190, reg0x191, reg0x192, reg0x1E0, reg0x1E1
	// Учти, что divPll имеет значения 2..6
	{
		reg0x190 = (pDividers->divDo/2-1)<<4;
		reg0x190|= (pDividers->divDo/2-1) + (pDividers->divDo%2);
		reg0x191 = 0x00;
		reg0x192 = 0x00;
		reg0x1E0 = pDividers->divPll - 2;
		reg0x1E1 = 0x02;
	}

	//
	// Подготовить массив делителей 
	//
	paRegs[ 0].adr = 0x000;  paRegs[ 0].val = 0x18;
	paRegs[ 1].adr = 0x010;  paRegs[ 1].val = 0x0C;
	paRegs[ 2].adr = 0x011;  paRegs[ 2].val = reg0x011;
	paRegs[ 3].adr = 0x012;  paRegs[ 3].val = reg0x012;
	paRegs[ 4].adr = 0x013;  paRegs[ 4].val = reg0x013;
	paRegs[ 5].adr = 0x014;  paRegs[ 5].val = reg0x014;
	paRegs[ 6].adr = 0x015;  paRegs[ 6].val = reg0x015;
	paRegs[ 7].adr = 0x016;  paRegs[ 7].val = reg0x016;
	paRegs[ 8].adr = 0x017;  paRegs[ 8].val = 0x00;
	paRegs[ 9].adr = 0x018;  paRegs[ 9].val = 0x06;
	paRegs[10].adr = 0x019;  paRegs[10].val = 0x80;
	paRegs[11].adr = 0x01A;  paRegs[11].val = 0x00;
	paRegs[12].adr = 0x01B;  paRegs[12].val = 0x00;
	paRegs[13].adr = 0x01C;  paRegs[13].val = 0x11;
	paRegs[14].adr = 0x01D;  paRegs[14].val = 0x12;

	paRegs[15].adr = 0x0F0;  paRegs[15].val = 0x00;
	paRegs[16].adr = 0x0F1;  paRegs[16].val = 0x00;
	paRegs[17].adr = 0x0F2;  paRegs[17].val = 0x00;
	paRegs[18].adr = 0x0F3;  paRegs[18].val = 0x00;
	paRegs[19].adr = 0x0F4;  paRegs[19].val = 0x00;
	paRegs[20].adr = 0x0F5;  paRegs[20].val = 0x00;	

	paRegs[21].adr = 0x190;  paRegs[21].val = reg0x190;
	paRegs[22].adr = 0x191;  paRegs[22].val = reg0x191;
	paRegs[23].adr = 0x192;  paRegs[23].val = reg0x192;
	paRegs[24].adr = 0x193;  paRegs[24].val = reg0x190;
	paRegs[25].adr = 0x194;  paRegs[25].val = reg0x191;
	paRegs[26].adr = 0x195;  paRegs[26].val = reg0x192;
	paRegs[27].adr = 0x196;  paRegs[27].val = reg0x190;
	paRegs[28].adr = 0x197;  paRegs[28].val = reg0x191;
	paRegs[29].adr = 0x198;  paRegs[29].val = reg0x192;
	paRegs[30].adr = 0x1E0;  paRegs[30].val = reg0x1E0;
	paRegs[31].adr = 0x1E1;  paRegs[31].val = reg0x1E1;

	paRegs[32].adr = 0x230;  paRegs[32].val = 0x01;
	paRegs[33].adr = 0x232;  paRegs[33].val = 0x01;
	paRegs[34].adr = 0x230;  paRegs[34].val = 0x00;
	paRegs[35].adr = 0x232;  paRegs[35].val = 0x01;

	return 0;
}

//
// End of File
//

