//=********************************************************
// Suspendr.h - Class TStreamSuspender Declaration
//
// (C) Copyright Ekkore 2003-2004
//
// Created: 
//              30.05.2006 by Ekkore
// Modified:
//
//=********************************************************

#ifndef		_SUSPENDR_H_
#define		_SUSPENDR_H_

enum
{ SUSPENDR_OK = 0, SUSPENDR_SUSPEND, SUSPENDR_RESUME };

typedef struct
{
    int            lead;
} SUSP_Criteria, *PSUSP_Criteria;

//=******************** TStreamSuspender ******************
//=********************************************************
typedef struct _TStreamSuspender
{
    int            m_on;                         // Suspender is switched ON (1) or OFF (0)
    int            m_blkNum;                     // Number of Blocks
    int            m_blkSize;                    // Size of every Block (bytes)
    int            m_isStart;                    // 1 - Stream is Started
    int            m_isSuspend;                  // 1 - Stream is Suspended
    int            m_blkEOT;                     // Number of Transferred Block
    int            m_blkMark;                    // Number of Block, that generate Supending
    int            m_lead;                       // Criterion to Calculate Mark
} TStreamSuspender;

TStreamSuspender *TStreamSuspenderCreate ( int blkNum, int blkSize );
void           TStreamSuspenderDelete ( TStreamSuspender * Ss );

int            TStreamSuspenderInit ( TStreamSuspender * Ss, int blkNum,
                                      int blkSize );
int            TStreamSuspenderStart ( TStreamSuspender * Ss );
int            TStreamSuspenderStop ( TStreamSuspender * Ss );
int            TStreamSuspenderAdjust ( TStreamSuspender * Ss, int isAdjust );
int            TStreamSuspenderDone ( TStreamSuspender * Ss, int blkNo );
int            TStreamSuspenderEOT ( TStreamSuspender * Ss, int blkNo );
int            TStreamSuspenderSetCriteria ( TStreamSuspender * Ss,
                                             SUSP_Criteria * pCriteria );
int            TStreamSuspenderIsSuspend ( TStreamSuspender * Ss );
int            TStreamSuspenderCalcMark ( TStreamSuspender * Ss, int blkNo );

#endif //_SUSPENDR_H_


//
// End of File
//
