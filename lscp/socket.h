// socket.h
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

#ifndef __LSCP_SOCKET_H
#define __LSCP_SOCKET_H

#include "lscp/thread.h"

#if defined(WIN32)
#include <winsock.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

//-------------------------------------------------------------------------
// Sockets.

#if defined(WIN32)
typedef SOCKET lscp_socket_t;
#else
typedef int lscp_socket_t;
#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1
#define closesocket(s)  close(s)
#endif

#define LSCP_BUFSIZ     1024

void lscp_socket_perror (const char *pszPrefix);

void lscp_socket_getopts (const char *pszPrefix, lscp_socket_t sock);
void lscp_socket_trace   (const char *pszPrefix, struct sockaddr_in *pAddr, const char *pchBuffer, int cchBuffer);


//-------------------------------------------------------------------------
// Threaded socket agent struct helpers.

typedef struct _lscp_socket_agent_t {

    lscp_socket_t       sock;
    struct sockaddr_in  addr;
    lscp_thread_t      *pThread;
    int                 iState;

} lscp_socket_agent_t;

void          lscp_socket_agent_init  (lscp_socket_agent_t *pAgent, lscp_socket_t sock, struct sockaddr_in *pAddr, int cAddr);
lscp_status_t lscp_socket_agent_start (lscp_socket_agent_t *pAgent, lscp_thread_proc_t pfnProc, void *pvData, int iDetach);
lscp_status_t lscp_socket_agent_join  (lscp_socket_agent_t *pAgent);
lscp_status_t lscp_socket_agent_free  (lscp_socket_agent_t *pAgent);

#if defined(__cplusplus)
}
#endif


#endif // __LSCP_SOCKET_H

// end of socket.h
