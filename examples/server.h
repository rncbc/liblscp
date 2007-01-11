// server.h
//
/****************************************************************************
   liblscp - LinuxSampler Control Protocol API
   Copyright (C) 2004-2007, rncbc aka Rui Nuno Capela. All rights reserved.

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

#ifndef __LSCP_SERVER_H
#define __LSCP_SERVER_H

#include "lscp/socket.h"
#include "lscp/event.h"

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
	lscp_event_t            events;
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
	lscp_socket_agent_t agent;

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

lscp_status_t   lscp_server_broadcast   (lscp_server_t *pServer, lscp_event_t event, const char *pchData, int cchData);

lscp_status_t   lscp_server_result      (lscp_connect_t *pConnect, const char *pchBuffer, int cchBuffer);

lscp_status_t   lscp_server_subscribe   (lscp_connect_t *pConnect, lscp_event_t event);
lscp_status_t   lscp_server_unsubscribe (lscp_connect_t *pConnect, lscp_event_t event);


#if defined(__cplusplus)
}
#endif

#endif // __LSCP_SERVER_H

// end of server.h
