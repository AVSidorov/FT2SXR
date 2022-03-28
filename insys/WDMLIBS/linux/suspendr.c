//=********************************************************
// Suspendr.cpp - Class TStreamSuspender Definition
//
// (C) Copyright Ekkore 2003-2004
//
// Created:
//              30.05.2006 by Ekkore
// Modified:
//
//=********************************************************

#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/version.h>
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <linux/slab.h>

#ifndef _SUSPENDR_H_
        #include		"suspendr.h"
#endif

//=********* TStreamSuspender::TStreamSuspender ***********
//=********************************************************
TStreamSuspender *TStreamSuspenderCreate ( int blkNum, int blkSize )
{
    TStreamSuspender *Ss = kmalloc ( sizeof ( TStreamSuspender ), GFP_KERNEL );

    if ( !Ss )
        return NULL;

    memset ( Ss, 0, sizeof ( TStreamSuspender ) );

    Ss->m_on = 0;
    Ss->m_blkNum = 0;
    Ss->m_blkSize = 0;
    Ss->m_isStart = 0;
    Ss->m_isSuspend = 0;
    Ss->m_blkEOT = 0;
    Ss->m_blkMark = -1;
    Ss->m_lead = 4;

    TStreamSuspenderInit ( Ss, blkNum, blkSize );

    return Ss;
}

//=********* TStreamSuspender::~TStreamSuspender **********
//=********************************************************
void TStreamSuspenderDelete ( TStreamSuspender * Ss )
{
    if ( !Ss )
        return;

    kfree ( Ss );
}

//=************** TStreamSuspender::Init ******************
//=********************************************************
int TStreamSuspenderInit ( TStreamSuspender * Ss, int blkNum, int blkSize )
{
    //m_on       = 0; // Suspender could be already switched ON
    Ss->m_blkNum = blkNum;
    Ss->m_blkSize = blkSize;
    Ss->m_isStart = 0;
    Ss->m_isSuspend = 0;

    if ( Ss->m_lead >= blkNum )
        Ss->m_lead = 2;

    return 0;
}

//=************** TStreamSuspender::Start *****************
//=********************************************************
int TStreamSuspenderStart ( TStreamSuspender * Ss )
{
//      if( !m_isStart )
//      {
//              if( m_on )
    Ss->m_blkMark = TStreamSuspenderCalcMark ( Ss, -1 );
    Ss->m_isSuspend = 0;
    Ss->m_isStart = 1;
//      }

    return 0;
}

//=************** TStreamSuspender::Stop ******************
//=********************************************************
int TStreamSuspenderStop ( TStreamSuspender * Ss )
{
    Ss->m_on = 0;
    Ss->m_isStart = 0;
    Ss->m_isSuspend = 0;

    return 0;
}

//=************** TStreamSuspender::Adjust ****************
//=********************************************************
int TStreamSuspenderAdjust ( TStreamSuspender * Ss, int isAdjust )
{
    int            ret = SUSPENDR_OK;

    //
    // Switch OFF
    //
    if ( isAdjust == 0 )
    {
        if ( Ss->m_on && Ss->m_isStart && Ss->m_isSuspend )
            ret = SUSPENDR_RESUME;

        Ss->m_isSuspend = 0;
        Ss->m_on = 0;
    }

    //
    // Switch ON
    //
    else
    {
        Ss->m_on = 1;
    }

    return ret;
}

//=************** TStreamSuspender::Done ******************
//=********************************************************
int TStreamSuspenderDone ( TStreamSuspender * Ss, int blkNo )
{
    int            ret = SUSPENDR_OK;

    //
    // Check blkNo
    //
    if ( blkNo < 0 || blkNo >= Ss->m_blkNum )
        return ret;

    //
    // Remember blkNo
    //
    Ss->m_blkMark = TStreamSuspenderCalcMark ( Ss, blkNo );

    //
    // If switched off
    //
    if ( !Ss->m_on )
        return ret;

    //
    // Suspend or Resume
    //
    if ( Ss->m_blkMark == Ss->m_blkEOT )
    {
        if ( !Ss->m_isSuspend )
        {
            Ss->m_isSuspend = 1;
            ret = SUSPENDR_SUSPEND;
        }
    }
    else
    {
        if ( Ss->m_isSuspend )
        {
            Ss->m_isSuspend = 0;
            ret = SUSPENDR_RESUME;
        }
    }

    return ret;
}

//=************** TStreamSuspender::EOT *******************
//=********************************************************
int TStreamSuspenderEOT ( TStreamSuspender * Ss, int blkNo )
{
    int            ret = SUSPENDR_OK;

    Ss->m_blkEOT = blkNo;

    if ( !Ss->m_on )
    {
        //printk("<0>%s(): SUSPENDR_OK\n", __FUNCTION__);
        return SUSPENDR_OK;
    }

    if ( !Ss->m_isSuspend )
    {
        if ( Ss->m_blkMark == Ss->m_blkEOT )
        {
            Ss->m_isSuspend = 1;
            ret = SUSPENDR_SUSPEND;
            //printk("<0>%s(): SUSPENDR_SUSPEND\n", __FUNCTION__);
        }
    }

    return ret;
}

//=************** TStreamSuspender::CalcMark **************
//=********************************************************
int TStreamSuspenderCalcMark ( TStreamSuspender * Ss, int blkNo )
{
    int            blkMark;

    blkMark = blkNo + Ss->m_blkNum;
    blkMark -= Ss->m_lead;      // Use Criterion
    if ( blkMark >= Ss->m_blkNum )
        blkMark -= Ss->m_blkNum;

    return blkMark;
}

//=************** TStreamSuspender::SetCriteria ***********
//=********************************************************
int TStreamSuspenderSetCriteria ( TStreamSuspender * Ss,
                                  SUSP_Criteria * pCriteria )
{
    if ( pCriteria->lead >= 0 && pCriteria->lead < Ss->m_blkNum )
        Ss->m_lead = pCriteria->lead;

    return 0;
}

int TStreamSuspenderIsSuspend ( TStreamSuspender * Ss )
{
    return Ss->m_isSuspend;
};

//
// End of File
//
