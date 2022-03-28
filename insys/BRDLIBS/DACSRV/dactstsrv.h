/*
 ****************** File DactstSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : DAС service
 *
 * (C) InSys by Dorokhin A. Oct 2009
 *
 ************************************************************
*/

#ifndef _DACTSTSRV_H
 #define _DACTSTSRV_H

//#include "ctrldac216x400m.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "dacsrv.h"
#include "mainregs.h"
//#include "dac216x400mregs.h"

//#include "dac216x400msrvinfo.h"

// DAC Clock sources
enum {
	BRDclks_DAC_DISABLED = 0x0,	// Clock not set
	BRDclks_DAC_SUBGEN = 0x1,	// Internal SubModule generator
	BRDclks_DAC_BASEGEN = 0x2,	// Internal Base Module generator
	BRDclks_DAC_EXTCLK = 0x81,	// External SubModule clock (CLKIN) 
};

const int DAC214X125M_TETR_ID		= 0x0C; // tetrad id
const int DAC3224X192_TETR_ID		= 0x24; // tetrad id
const int DAC818X800_TETR_ID		= 0x2A; // tetrad id
const int DAC1624X192_TETR_ID		= 0x3E; // tetrad id
const int DAC216X400M_TETR_ID		= 0x50; // tetrad id
const int VK3DAC_TETR_ID			= 0x66; // tetrad id
const int ADMDAC216x400MT_TETR_ID	= 0xA7;
const int DACFM416x1G5D_TETR_ID		= 0xB2;						
const int DACFM214x2G5D_TETR_ID		= 0xBA;						

#pragma pack(push,1)

typedef struct _DACTSTSRV_CFG {
	DACSRV_CFG DacCfg;
	ULONG		ClkSrc;					// источник такта
	double	dSamplingRate;	// Частота дискретизации, после выплнения SetRate()
	double	dClkValue;			// частота тактирования (Гц)
	S32		nDacInterpFactor;		// Желательный коэф. интерполяции ЦАП
} DACTSTSRV_CFG, *PDACTSTSRV_CFG;

#pragma pack(pop)

class CDacTstSrv: public CDacSrv
{

protected:

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetDacTetrNum(CModule* pModule);

	virtual int SetProperties(CModule* pDev, BRDCHAR* iniFilePath, BRDCHAR* SectionName);
	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);
	virtual int SetInterpFactor(CModule *pModule, ULONG nInterpFactor);
	virtual int GetInterpFactor(CModule *pModule, ULONG& nInterpFactor);

public:

	CDacTstSrv(int idx, int srv_num, CModule* pModule, PDACSRV_CFG pCfg); // constructor

};

#endif // _DACTSTSRV_H

//
// End of file
//