#ifndef _INTRUPT_H_
#define _INTRUPT_H_

/***************************************************************************
 *            intrupt.h
 *
 *  Wed Dec  7 11:34:22 2005
 *  Copyright  2005  Vladimir
 *  karakozov@inbox.ru
 ****************************************************************************/

//-----------------------------------------------------------------------------

typedef struct TInterRuptor
{
    wait_queue_head_t 	m_IR_wq;
    atomic_t       	m_totalCnt;
    atomic_t       	m_flag;

} TInterRuptor;

//------------------------------------------------------------------------------
TInterRuptor  *TInterRuptorCreate ( void );
void           TInterRuptorInit ( TInterRuptor * m_IR );
void           TInterRuptorDelete ( TInterRuptor * m_IR );
void           TInterRuptorClearFlag ( TInterRuptor * m_IR );
void           TInterRuptorSetFlag ( TInterRuptor * m_IR );
void           TInterRuptorQueueDPC ( TInterRuptor * m_IR );
int            WaitInterrupt ( TInterRuptor * pIR, u32 timeout,
                               u32 * pTimeElapsed );
int            GrabInterrupt ( TInterRuptor * pIR, u32 timeout,
                               u32 * pTimeElapsed );
int            GrabReset ( TInterRuptor * pIR );			       
u32            GetCounter ( TInterRuptor * p_IR );

//------------------------------------------------------------------------------

#endif
