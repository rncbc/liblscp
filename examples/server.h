// server.h
//
/****************************************************************************
   liblscp - LinuxSampler Control Protocol API
   Copyright (C) 2004, rncbc aka Rui Nuno Capela. All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*****************************************************************************/

#ifndef __LSCP_SERVER_H
#define __LSCP_SERVER_H

#include "lscp/socket.h"

#if defined(__cplusplus)
extern "C" {
#endif

//-------------------------------------------------------------------------
// Server socket modes.

/** Server thread model. */
typedef enum _lscp_server_mode_t
{
    LSCP_SERVER_THREAD = 0,
    LSCP_SERVER_SELECT = 1

} lscp_server_mode_t;


/** Connection mode notification. */
typedef enum _lscp_connect_mode_t
{
    LSCP_CONNECT_OPEN  = 0,
    LSCP_CONNECT_CLOSE = 1

} lscp_connect_mode_t;


//-------------------------------------------------------------------------
// Server socket structures.

struct _lscp_server_t;

/** Client connection descriptor struct. */
typedef struct _lscp_connect_t
{
    struct _lscp_server_t  *server;
    lscp_socket_agent_t     client;
    int                     port;
    int                     ping;
    char *                  sessid;
    struct _lscp_connect_t *prev;
    struct _lscp_connect_t *next;

} lscp_connect_t;

/** Client connection list struct. */
typedef struct _lscp_connect_list_t
{
    lscp_connect_t *first;
    lscp_connect_t *last;
    unsigned int    count;
    lscp_mutex_t    mutex;

} lscp_connect_list_t;

/** Server callback procedure prototype. */
typedef lscp_status_t (*lscp_server_proc_t)
(
    lscp_connect_t *pConnect,
    const char *pchBuffer,
    int cchBuffer,
    void *pvData
);

/** Server descriptor struct. */
typedef struct _lscp_server_t
{
    lscp_server_mode_t  mode;
    lscp_connect_list_t connects;
    lscp_server_proc_t  pfnCallback;
    void               *pvData;
    lscp_socket_agent_t cmd;
    lscp_socket_agent_t evt;
    lscp_thread_t      *pWatchdog;
    int                 iWatchdog;
    int                 iSleep;

} lscp_server_t;


//-------------------------------------------------------------------------
// Server versioning teller fuunctions.

const char *    lscp_server_package     (void);
const char *    lscp_server_version     (void);
const char *    lscp_server_build       (void);


//-------------------------------------------------------------------------
// Server socket functions.

lscp_server_t * lscp_server_create      (int iPort, lscp_server_proc_t pfnCallback, void *pvData);
lscp_server_t * lscp_server_create_ex   (int iPort, lscp_server_proc_t pfnCallback, void *pvData, lscp_server_mode_t mode);
lscp_status_t   lscp_server_join        (lscp_server_t *pServer);
lscp_status_t   lscp_server_destroy     (lscp_server_t *pServer);

lscp_status_t   lscp_server_broadcast   (lscp_server_t *pServer, const char *pchBuffer, int cchBuffer);

lscp_status_t   lscp_server_result      (lscp_connect_t *pConnect, const char *pchBuffer, int cchBuffer);

lscp_status_t   lscp_server_subscribe   (lscp_connect_t *pConnect, int iPort);
lscp_status_t   lscp_server_unsubscribe (lscp_connect_t *pConnect, const char *pszSessID);


#if defined(__cplusplus)
}
#endif

#endif // __LSCP_SERVER_H

// end of server.h
