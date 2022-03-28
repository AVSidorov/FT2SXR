#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/wait.h>
#include <asm/io.h>

#include "captlist.h"

//=******************* CAPT_CompareString *****************
//=********************************************************
int CAPT_CompareString ( const char *pStr0, const char *pStr1 )
{
    for ( ;; )
    {
        if ( *pStr0 != *pStr1 )
            return -1;
        if ( *pStr0 == '\0' )
            break;
        pStr0++;
        pStr1++;
    }
    return 0;
}

//=******************* TCaptureList::TCaptureListCreate *********
//=********************************************************

TCaptureList  *CaptureListCreate ( int maxSize )
{
    TCaptureList  *cl = kmalloc ( sizeof ( TCaptureList ), GFP_KERNEL );

    if ( !cl )
        return NULL;

    memset ( cl, 0, sizeof ( TCaptureList ) );

    CaptureListInit ( cl, maxSize );

    return cl;
}

//=******************* TCaptureList::CaptureListDelete *********
//=********************************************************

void CaptureListDelete ( TCaptureList * cl )
{
    if ( !cl )
        return;

    CaptureListClean( cl );

    kfree ( cl );
}

//=******************* TCaptureList::TCaptureList *********
//=********************************************************
void CaptureListInit ( TCaptureList * cl, int maxSize )
{
    TCaptureListItem *pList;
    int            idx;

    cl->m_maxSize = 0;
    cl->m_pList = NULL;

    if ( maxSize <= 0 )
    {
        return;
    }

    //
    // Allocate List
    //
    pList =
        ( TCaptureListItem * ) kmalloc ( maxSize * sizeof ( TCaptureListItem ),
                                         GFP_KERNEL );
    if ( pList == NULL )
        return;

    for ( idx = 0; idx < maxSize; idx++ )
    {
        pList[idx].name[0] = '\0';
        pList[idx].password = idx + 1;
        atomic_set ( &pList[idx].status, 1 );

        InitKevent( &pList[idx].event );
    }

    cl->m_maxSize = maxSize;
    cl->m_pList = pList;

}

//=******************* TCaptureList::~TCaptureList ********
//=********************************************************
void CaptureListClean ( TCaptureList * cl )
{
    if( cl->m_pList )
        kfree ( cl->m_pList );

    cl->m_pList = NULL;
    cl->m_maxSize = 0;
    cl->m_pList = NULL;
}

//=******************* TCaptureList::Fill *****************
//=********************************************************
int Fill ( TCaptureList * cl, const char *objName )
{
    int            ii,
                   idx;

    //
    // Search Item
    //
    idx = SearchItem ( cl, "" );
    if ( idx < 0 )
        return -1;

    //
    // Fill Item
    //
    for ( ii = 0; ii < CAPTLIST_NAMEMAX; ii++ )
    {
        cl->m_pList[idx].name[ii] = objName[ii];
        if ( objName[ii] == '\0' )
            break;
    }
    cl->m_pList[idx].name[CAPTLIST_NAMEMAX - 1] = '\0';

    atomic_set ( &cl->m_pList[idx].status, 0 );

    SetEvent( &cl->m_pList[idx].event );

    return 0;
}

//=******************* TCaptureList::Capture **************
//=********************************************************
int Capture ( TCaptureList * cl, const char *objName, int timeout )
{

    int            idx;
    u32            status;

    //
    // Search Item
    //
    idx = SearchItem ( cl, objName );
    if ( idx < 0 )
        return 0;

    //
    // Capture
    //
    status = atomic_inc_and_test ( &cl->m_pList[idx].status );
    if ( status == 0 )
    {
        ResetEvent( &cl->m_pList[idx].event );
        return cl->m_pList[idx].password;
    }
    if ( timeout == 0 )
        return 0;

    //
    // Try to Capture once more
    //
    for ( ;; )
    {
        //
        // Wait for Event
        //
        if ( WaitEvent ( &cl->m_pList[idx].event, timeout ) < 0 )
        {
            return 0;
        }

        //
        // Check Status
        //
        status = atomic_inc_and_test ( &cl->m_pList[idx].status );
        if ( status == 0 )
            break;
    }

    ResetEvent( &cl->m_pList[idx].event );

    return cl->m_pList[idx].password;
}

//=******************* TCaptureList::Release **************
//=********************************************************
int Release ( TCaptureList * cl, const char *objName, int password )
{
    int            idx;

    //
    // Search Item
    //
    idx = SearchItem ( cl, objName );
    if ( idx < 0 )
        return -1;
    if ( cl->m_pList[idx].password != password )
        return -1;
    if ( atomic_read ( &cl->m_pList[idx].status ) == 0 )
        return -1;

    //
    // Release
    //
    cl->m_pList[idx].password++;
    if ( cl->m_pList[idx].password == 0 )
        cl->m_pList[idx].password = 1;

    atomic_set ( &cl->m_pList[idx].status, 0 );
    SetEvent ( &cl->m_pList[idx].event );

    return 0;
}

//=******************* TCaptureList::SearchItem ***********
//=********************************************************
int SearchItem ( TCaptureList * cl, const char *objName )
{
    int            idx;

    //
    // Search Item
    //
    for ( idx = 0; idx < cl->m_maxSize; idx++ )
    {

        if ( CAPT_CompareString ( cl->m_pList[idx].name, objName ) == 0 )
            break;
        if ( idx >= cl->m_maxSize )
            return -1;
    }
    return idx;
}

//
// End of File
//
