/*
 ****************** File SdramSrv.h *************************
 *
 *  Definitions of user application interface
 *	structures and constants
 *	for BRD_ctrl : Sdram section
 *
 * (C) InSys by Dorokhin Andrey May, 2004
 *
 ************************************************************
*/

#ifndef _SDRAMAMBVMESRV_H
 #define _SDRAMAMBVMESRV_H

#include "ctrlsdram.h"
#include "service.h"
#include "adm2if.h"
#include "mainregs.h"
#include "sdramregs.h"
#include "sdramsrv.h"

const int SDRAMAMBVME_TETR_ID = 0x18; // tetrad id

class CSdramAmbvmeSrv : public CSdramSrv
{

protected:

	virtual void GetSdramTetrNum(CModule* pModule);
	
public:

	CSdramAmbvmeSrv(int idx, int srv_num, CModule* pModule, PSDRAMSRV_CFG pCfg); // constructor

};

#endif // _SDRAMAMBVMESRV_H

//
// End of file
//