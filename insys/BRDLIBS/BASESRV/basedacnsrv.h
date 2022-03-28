/*
 ****************** File Dac216x400mSrv.h *******************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : new BASEDAС service
 *
 * (C) InSys by Sklyarov A., Dorokhin A. Mar 2011
 *
 ************************************************************
*/

#ifndef _BASEDACNSRV_H
 #define _BASEDACNSRV_H

#include "ctrlbasedacn.h"
#include "ctrlcmpsc.h"
#include "service.h"
#include "adm2if.h"
#include "dacsrv.h"
#include "mainregs.h"
//#include "dac216x400mregs.h"

#include "basedacsrvinfo.h"
//#include "dac216x400msrvinfo.h"

const int BASEDACN_TETR_ID = 0x7F; // tetrad id

#pragma pack(push,1)

typedef struct _BASEDACNSRV_CFG {
	DACSRV_CFG DacCfg;
/*	U32		IntPllRef;					// frequency of PLL reference generator (значение внутреннего опорного генератора для ФАПЧ (Гц))
	U08		FreqTune;					// возможность подстройки частоты внутреннего опорного генератора
	U32		ExtPllRef;					// external frequency of PLL reference (значение внешней опорноq частоты для ФАПЧ (Гц))
	U32		OutPllFreq;					// frequency of PLL (частота, выдаваемая ФАПЧ (Гц))
	U32		SubExtClk;					// frequency of external clock (внешняя частота тактирования (Гц))
	PLL_RCNT Pll_R_CNT;					//
	PLL_NCNT Pll_N_CNT;					//
	PLL_FUNC Pll_Func;					//*/
	//U16		DacReg[8];				// internal DAC registers 0-7
} BASEDACNSRV_CFG, *PBASEDACNSRV_CFG;

#pragma pack(pop)

class CBaseDacnSrv: public CDacSrv
{

protected:

	virtual int CtrlRelease(void* pDev, void* pServData, ULONG cmd, void* args);
	virtual void GetDacTetrNum(CModule* pModule);
	//virtual void* GetInfoForDialog(CModule* pModule);
	//virtual int SetPropertyFromDialog(void	*pDev, void	*args);

	//int SetMute(CModule* pModule, ULONG mode);
	//int GetMute(CModule* pModule, ULONG& mode);

	//virtual int SetProperties(CModule* pDev, char* iniFilePath, char* SectionName);
	//virtual int SaveProperties(CModule* pDev, char* iniFilePath, char* SectionName);
/*	virtual int SetClkSource(CModule* pModule, ULONG ClkSrc);
	virtual int GetClkSource(CModule* pModule, ULONG& ClkSrc);
	virtual int SetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);
	virtual int GetClkValue(CModule* pModule, ULONG ClkSrc, double& ClkValue);

	virtual int SetClkRefSource(CModule* pModule, ULONG ClkSrc,ULONG RefSrc);
	virtual int GetClkRefSource(CModule* pModule, ULONG& ClkSrc,ULONG& RefSrc);
	virtual int SetClkRefValue(CModule* pModule, ULONG ClkSrc,ULONG RefSrc, double& ClkValue,double& RefValue);
	virtual int GetClkRefValue(CModule* pModule, ULONG ClkSrc,ULONG RefSrc, double& ClkValue,double& RefValue);
	virtual int SetDivClk(CModule* pModule, ULONG& DivClk,double& rate);
	virtual int GetDivClk(CModule* pModule, ULONG& DivClk, double& rate);

	virtual int SetRate(CModule* pModule, double& Rate, ULONG ClkSrc, double ClkValue);
	virtual int GetRate(CModule* pModule, double& Rate, double ClkValue);*/
	virtual int SetCode(CModule* pModule, ULONG type);
	virtual int GetCode(CModule* pModule, ULONG& type);

	virtual int SetSpecific(CModule* pModule, PBRD_DacSpec pSpec);
/*
	virtual int ExtraInit(CModule* pModule);
	int InitPLL(CModule* pModule);
	int OnPll(CModule* pModule, double& ClkValue, double RefValue);

	int writePllData(CModule* pModule, USHORT HiWord, USHORT LoWord);
	*/
	int SetDacReg(CModule* pModule, int regNum, int regVal);

public:

	CBaseDacnSrv(int idx, int srv_num, CModule* pModule, PBASEDACNSRV_CFG pCfg); // constructor

};

#endif // _BASEDACNSRV_H

//
// End of file
//