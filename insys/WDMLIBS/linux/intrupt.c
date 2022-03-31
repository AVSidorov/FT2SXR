
#include <linux/kernel.h>
#define __NO_VERSION__
#include <linux/version.h>
#include <linux/pagemap.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/slab.h>

#ifndef _INTRUPT_H_
        #include "intrupt.h"
#endif

//------------------------------------------------------------------------------

TInterRuptor *TInterRuptorCreate( void )
{
    TInterRuptor  *m_IR = kmalloc( sizeof( TInterRuptor ), GFP_KERNEL );

    if ( !m_IR )
        return NULL;

    memset( m_IR, 0, sizeof( TInterRuptor ) );

    TInterRuptorInit( m_IR );

    return m_IR;
}

//------------------------------------------------------------------------------

void TInterRuptorDelete( TInterRuptor *m_IR )
{
    if ( !m_IR )
        return;
    kfree( m_IR );
}

//------------------------------------------------------------------------------

void TInterRuptorInit( TInterRuptor *m_IR )
{
    atomic_set ( &m_IR->m_totalCnt, 0 );
    atomic_set ( &m_IR->m_flag, 0 );

    init_waitqueue_head( &m_IR->m_IR_wq );
}

//------------------------------------------------------------------------------

void TInterRuptorSetFlag( TInterRuptor *m_IR )
{
    atomic_set( &m_IR->m_flag, 1 );
}

//------------------------------------------------------------------------------

void TInterRuptorClearFlag( TInterRuptor *m_IR )
{
	//printk("<0>%s()\n", __FUNCTION__);
    atomic_set ( &m_IR->m_flag, 0 );
}

//------------------------------------------------------------------------------

void TInterRuptorQueueDPC( TInterRuptor *m_IR )
{
	//printk("<0>%s()\n", __FUNCTION__);

    if(!m_IR) {
    	printk ( "<0> %s: m_IR = NULL\n", __FUNCTION__ );
    	return;
    }


    atomic_inc( &m_IR->m_totalCnt );
    atomic_set( &m_IR->m_flag, 1 );

    wake_up_interruptible( &m_IR->m_IR_wq );

    //printk ( "<0>%s(): count = %d, flag = %d\n", __FUNCTION__,
    //         atomic_read ( &m_IR->m_totalCnt ), atomic_read ( &m_IR->m_flag ) );
}

//------------------------------------------------------------------------------

#define ms_to_jiffies( ms ) (HZ*ms/1000)
#define jiffies_to_ms( jf ) (jf*1000/HZ)

//------------------------------------------------------------------------------

int WaitInterrupt( TInterRuptor *pIR, u32 timeout, u32 *pTimeElapsed )
{
    unsigned long start_t;
    unsigned long end_t;
    int status = -1;

    //printk("<0>%s()\n", __FUNCTION__);

    start_t = jiffies;
#ifdef DZYTOOLS_2_4_X
    status = interruptible_sleep_on_timeout( &pIR->m_IR_wq, ms_to_jiffies(timeout) );
#else
    status = wait_event_interruptible_timeout( pIR->m_IR_wq, atomic_read(&pIR->m_flag), ms_to_jiffies(timeout) );
#endif
    end_t = jiffies;

    if(!status) {
        printk("<0>%s(): TIMEOUT\n", __FUNCTION__);
        if(pTimeElapsed)
            *pTimeElapsed = 0;
        return -ETIMEDOUT;
    }

    atomic_set ( &pIR->m_flag, 0 );

    if( pTimeElapsed )
        *pTimeElapsed = jiffies_to_ms(end_t - start_t);

    return 0;
}

//------------------------------------------------------------------------------

u32 GetCounter( TInterRuptor *pIR )
{
	//printk("<0>%s()\n", __FUNCTION__);
    return atomic_read( &pIR->m_totalCnt );
}

//------------------------------------------------------------------------------

int GrabInterrupt( TInterRuptor *pIR, u32 timeout, u32 *pTimeElapsed )
{
    //printk("<0>%s()\n", __FUNCTION__);

    if( atomic_read( &pIR->m_flag ) )
    {
        atomic_set( &pIR->m_flag, 0 );

        if( pTimeElapsed )
            *pTimeElapsed = 0;
        return 0;
    }

    return WaitInterrupt(pIR,timeout,pTimeElapsed);
}

//------------------------------------------------------------------------------

int GrabReset( TInterRuptor *pIR )
{
    //clear counters for local interrupt
    atomic_set( &pIR->m_totalCnt, 0 );
    //atomic_set(&pIR->m_wait_count, 0);
    atomic_set( &pIR->m_flag, 0 );

    return 0;
}

//------------------------------------------------------------------------------
