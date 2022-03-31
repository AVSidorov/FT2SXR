//=********************************************************
// CaptList.h - Class TCaptureList Declaration
//
// (C) Copyright Ekkore 2004
//
// Created: 
//              04.06.2004 by Ekkore
// Modified:
//
//=********************************************************

#ifndef		_CAPTLIST_H_
#define		_CAPTLIST_H_

#include "event.h"

#define		CAPTLIST_NAMEMAX		32

typedef struct
{
    char           		name[CAPTLIST_NAMEMAX];
    atomic_t       		status;                       // 0-released, 1-captured
    //wait_queue_head_t 		event;
    KEVENT			event;
    int            		password;

} TCaptureListItem;

//=******************* class TCaptureList *****************
//=********************************************************
typedef struct _TCaptureList
{

    int            m_maxSize;
    TCaptureListItem *m_pList;

} TCaptureList;

TCaptureList  *CaptureListCreate ( int maxSize );
void           CaptureListDelete ( TCaptureList * cl );
void           CaptureListClean ( TCaptureList * cl );
void           CaptureListInit ( TCaptureList * cl, int maxSize );
int            Fill ( TCaptureList * cl, const char *objName );
int            Capture ( TCaptureList * cl, const char *objName, int timeout );
int            Release ( TCaptureList * cl, const char *objName, int password );
int            SearchItem ( TCaptureList * cl, const char *objName );



#endif //_CAPTLIST_H_

//
// End of File
//
