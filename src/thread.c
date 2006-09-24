// thread.c
//
/****************************************************************************
   liblscp - LinuxSampler Control Protocol API
   Copyright (C) 2004-2006, rncbc aka Rui Nuno Capela. All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#include "lscp/thread.h"


//-------------------------------------------------------------------------
// Threads.

struct _lscp_thread_t {
#if defined(WIN32)
    HANDLE              hThread;
    DWORD               dwThreadID;
#else
    pthread_t           pthread;
#endif
    lscp_thread_proc_t  pfnProc;
    void               *pvData;
    int                 iDetach;
};


#if defined(WIN32)
static DWORD WINAPI _lscp_thread_start ( LPVOID pvThread )
#else
static void *_lscp_thread_start ( void *pvThread )
#endif
{
    lscp_thread_t *pThread = (lscp_thread_t *) pvThread;
    if (pThread) {
    //  fprintf(stderr, "_lscp_thread_start: pThread=%p started.\n", pThread);
        pThread->pfnProc(pThread->pvData);
    //  fprintf(stderr, "_lscp_thread_start: pThread=%p terminated.\n", pThread);
        if (pThread->iDetach)
            free(pThread);
    }
#if defined(WIN32)
    return 0;
#else
    return NULL;
#endif
}

lscp_thread_t *lscp_thread_create ( lscp_thread_proc_t pfnProc, void *pvData, int iDetach )
{
    lscp_thread_t *pThread;
#if !defined(WIN32)
    pthread_attr_t attr;
#endif

    if (pfnProc == NULL) {
        fprintf(stderr, "lcsp_thread_create: Invalid thread function.\n");
        return NULL;
    }

    pThread = (lscp_thread_t *) malloc(sizeof(lscp_thread_t));
    if (pThread == NULL) {
        fprintf(stderr, "lcsp_thread_create: Out of memory.\n");
        return NULL;
    }
    memset(pThread, 0, sizeof(lscp_thread_t));

    pThread->pvData  = pvData;
    pThread->pfnProc = pfnProc;
    pThread->iDetach = iDetach;

//  fprintf(stderr, "lscp_thread_create: pThread=%p.\n", pThread);

#if defined(WIN32)
    pThread->hThread = CreateThread(NULL, 0, _lscp_thread_start, (LPVOID) pThread, 0, &(pThread->dwThreadID));
    if (pThread->hThread == NULL) {
        fprintf(stderr, "lcsp_thread_create: Failed to create thread.\n");
        free(pThread);
        return NULL;
    }
#else
    pthread_attr_init(&attr);
    if (iDetach)
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&pThread->pthread, &attr, _lscp_thread_start, pThread)) {
        fprintf(stderr, "lcsp_thread_create: Failed to create thread.\n");
        free(pThread);
        return NULL;
    }
#endif

  return pThread;
}


lscp_status_t lscp_thread_join( lscp_thread_t *pThread )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pThread == NULL)
        return ret;

//  fprintf(stderr, "lscp_thread_join: pThread=%p.\n", pThread);

#if defined(WIN32)
    if (pThread->hThread && WaitForSingleObject(pThread->hThread, INFINITE) == WAIT_OBJECT_0) {
        pThread->hThread = NULL;
        ret = LSCP_OK;
    }
#else
    if (pThread->pthread && pthread_join(pThread->pthread, NULL) == 0) {
        pThread->pthread = 0;
        ret = LSCP_OK;
    }
#endif

    return ret;
}


lscp_status_t lscp_thread_cancel ( lscp_thread_t *pThread )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pThread == NULL)
        return ret;

//  fprintf(stderr, "lscp_thread_cancel: pThread=%p.\n", pThread);

#if defined(WIN32)
    if (pThread->hThread) { // Should we TerminateThread(pThread->hThread, 0) ?
    /*  pThread->hThread = NULL; */
        ret = LSCP_OK;
    }
#else
    if (pThread->pthread && pthread_cancel(pThread->pthread) == 0) {
        pThread->pthread = 0;
        ret = LSCP_OK;
    }
#endif

    return ret;
}


lscp_status_t lscp_thread_destroy ( lscp_thread_t *pThread )
{
    lscp_status_t ret = lscp_thread_cancel(pThread);

    if (ret == LSCP_OK)
        ret = lscp_thread_join(pThread);
        
//  fprintf(stderr, "lscp_thread_destroy: pThread=%p.\n", pThread);

    if (ret == LSCP_OK)
        free(pThread);

    return ret;
}


// end of thread.c
