/*
 ****************** File mfm814x250m.h ************************
 *
 * Definitions of structures and constants
 * for FM814x250M 
 * Data Acquisition Daughter Boards.
 *
 * (C) InSys by Ekkore. Okt. 2009
 *
 ******************************************************
*/

#ifndef _MFM814x250M_H_
 #define _MFM814x250M_H_

#include	"brderr.h"
#include	"brdapi.h"

#include	"useful.h"
#include	"adcsubmodule.h"
#include	"icr.h"
#include	"Icr009D.h"

#include	"AdcFm814x250mSrv.h"

//#include	<vector>

//using namespace std;

// Constants
#define	VER_MAJOR		0x00010000L
#define	VER_MINOR		0x00000000L

#define	MAX_ADC_SRV 2

#pragma pack(push,1)

typedef struct _ADM_Cfg 
{
	BRDCHAR	sDlgDllName[DLGDLLNAME_SIZE];	// название диалоговой dll
	ADC_CFG	rAdcCfg;		//
	U32		ExtClk;			// frequency of external clock (частота внешнего тактирования (Гц))
	U32		nAdcCnt;		// ADC counter (количество АЦП: 4, 8)

	U32	nVersion;	// версия субмодуля
	U32	nAdcType;	// тип кристалла АЦП: 0-всегда
	U32	nDacType;	// тип кристалла ЦАП: 0-всегда
	U32	nDacRange;	// шкала преобразования ЦАП: (мВ)
	U32	nGenType;	// тип кристалла внутр. генератора: 0-не програм-ый, 1-Si571
	U32	nGenAdr;	// адресный код внутр. генератора: 0x49 по умолчанию
	U32	nGenRef;    // заводская установка частоты внутр. генератора (Гц)
	U32	nGenRefMax; // максимальная частота внутр. генератора (Гц)
	U32	nInpType;	// тип входа: 0-закр., 1-откр.

	//U32	nRefGen0;             // опорный генератор 0 (Гц)
	//U32	nRefGen1;             // опорный генератор 1 (Гц)
	//U32	nRefGen1Min;          // опорный генератор 1 (Гц)
	//U32	nRefGen1Max;          // опорный генератор 1 (Гц)
	//U32	nRefGen1Type;         // тип опорного генератора 1
	//U32	nRefGen2;             // опорный генератор 2 (Гц)
	//U32	nRefGen2Type;         // тип опорного генератора 2
	//U32	nLpfPassBand;		  // частота среза ФНЧ (Гц)
	//U32 aRange[4];			// шкалы преобразования для Standard (мВ) (default 10000, 2000, 500, 100)

} ADM_Cfg, *PADM_Cfg;

const ADM_Cfg AdmCfg_dflt = 
{
	_BRDC("fm814x250msrvdlg"), // dialog dll name
	{	14,			// bits
		1,			// encoding
		1000000,	// min rate 1 Mhz
		1000000000,  // max rate 200 Mhz
		500,		// входной диапазон (мВ)
		16384 * 4,	// ADC FIFO size
		(U32)-1			// tetrad number
	},
	100000000,	// external clock (100 MHz)
	2,			// Adc counter
	0x10,		//nVersion;	 
	0,			//nAdcType;	 
	0,			//nDacType;	 
	2500,		//nDacRange;
	1,			//nGenType;	 
	0x49,		//nGenAdr;	 
	100000000,	//nGenRef;   
	2000000000,	//nGenRefMax;
	0			//nInpType
};

#pragma pack(pop)

// ADMxxx SubModule Class definition
class Cmfm814x250m : public CAdcSubModule
{

protected:

	ADM_Cfg		*m_pIniData;	// Data from Registry or INI File
	ADM_Cfg		m_rSubCfg;		// Submodule configuration
	U32			m_nVersion;		// версия субмодуля
	U32			m_nSubunitId;	// ID типа субмодуля: 0x468 - 214x400M, 0x258 - 212x500M

	CFm814x250mSrv*	m_pAdcSrv[MAX_ADC_SRV];

  	const BRDCHAR* getClassName(void) { return _BRDC("Cmfm814x250m"); }

public:

	Cmfm814x250m();
	Cmfm814x250m( BRD_InitData *pInitData, long sizeInitData );
	~Cmfm814x250m();

	S32 Init(PUCHAR pSubmodEEPROM, BRDCHAR *pIniString = NULL);

	void GetDeviceInfo(BRD_Info* info);

protected:
	void GetICR( void *pCfgMem, ULONG nRealCfgSize, 
				ICR_CfgAdm     *pCfgAdm, 
				ICR_CfgAdc     *pCfgAdc, 
				ICR_CfgAdcFifo *pCfgAdcFifo);

	S32		SetConfig( ICR_CfgAdm *pCfgAdm,
					  ICR_CfgAdc *pAdc,
					  ICR_CfgAdcFifo *pAdcFifo);

public:
	S32		SetServices();
	void	SetSrvList( DEV_CMD_ServListItem *paServList );

};

#endif	// _MFM814x250M_H_

// ****************** End of file mfm814x250m.h **********************


 