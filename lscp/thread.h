// thread.h
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

#ifndef __LSCP_THREAD_H
#define __LSCP_THREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if (defined(_WIN32) || defined(__WIN32__))
#if (!defined(WIN32))
#define WIN32
#endif
#endif

#if defined(WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "lscp/version.h"

#if defined(__cplusplus)
extern "C" {
#endif

//-------------------------------------------------------------------------
// Status.

typedef enum _lscp_status_t
{
    LSCP_OK      =  0,
    LSCP_FAILED  = -1,
    LSCP_ERROR   = -2,
    LSCP_WARNING = -3,
    LSCP_TIMEOUT = -4,
    LSCP_QUIT    = -5

} lscp_status_t;

//-------------------------------------------------------------------------
// Mutexes.

#if defined(WIN32)
typedef HANDLE lscp_mutex_t;
#define lscp_mutex_init(m)      { (m) = CreateMutex(NULL, 0, NULL); }
#define lscp_mutex_destroy(m)   if (m) { CloseHandle(m); }
#define lscp_mutex_lock(m)      WaitForSingleObject((m), INFINITE)
#define lscp_mutex_unlock(m)    ReleaseMutex(m)
#else
typedef pthread_mutex_t lscp_mutex_t;
#define lscp_mutex_init(m)      pthread_mutex_init(&(m), NULL)
#define lscp_mutex_destroy(m)   pthread_mutex_destroy(&(m))
#define lscp_mutex_lock(m)      pthread_mutex_lock(&(m))
#define lscp_mutex_unlock(m)    pthread_mutex_unlock(&(m))
#endif

//-------------------------------------------------------------------------
// Simple condition variables (FIXME: probably incorrect on WIN32).

#if defined(WIN32)
typedef HANDLE lscp_cond_t;
#define lscp_cond_init(c)       { (c) = CreateEvent(NULL, FALSE, FALSE, NULL); }
#define lscp_cond_destroy(c)    if (c) { CloseHandle(c); }
#define lscp_cond_wait(c, m)    { lscp_mutex_unlock(m); WaitForSingleObject((c), INFINITE); lscp_mutex_lock(m); }
#define lscp_cond_signal(c)     SetEvent(c)
#else
typedef pthread_cond_t lscp_cond_t;
#define lscp_cond_init(c)       pthread_cond_init(&(c), NULL)
#define lscp_cond_destroy(c)    pthread_cond_destroy(&(c))
#define lscp_cond_wait(c, m)    pthread_cond_wait(&(c), &(m))
#define lscp_cond_signal(c)     pthread_cond_signal(&(c))
#endif

//-------------------------------------------------------------------------
// Threads.

struct _lscp_thread_t;

typedef void (*lscp_thread_proc_t)(void *pvData);

typedef struct _lscp_thread_t lscp_thread_t;

lscp_thread_t *lscp_thread_create  (lscp_thread_proc_t pfnProc, void *pvData, int iDetach);
lscp_status_t  lscp_thread_join    (lscp_thread_t *pThread);
lscp_status_t  lscp_thread_cancel  (lscp_thread_t *pThread);
lscp_status_t  lscp_thread_destroy (lscp_thread_t *pThread);

#if defined(WIN32)
#define lscp_thread_exit()  ExitThread(0)
#else
#define lscp_thread_exit()  pthread_exit(NULL)
#endif

#if defined(__cplusplus)
}
#endif

#endif // __LSCP_THREAD_H

// end of thread.h
