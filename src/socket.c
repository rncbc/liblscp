// socket.c
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

#include "lscp/socket.h"


//-------------------------------------------------------------------------
// Socket info debugging.

#if defined(WIN32)

static struct {

    int         iError;
    const char *pszError;
    
} _wsaErrorCodes[] = {

    { 0,                  "No error" },
    { WSAEINTR,           "Interrupted system call" },
    { WSAEBADF,           "Bad file number" },
    { WSAEACCES,          "Permission denied" },
    { WSAEFAULT,          "Bad address" },
    { WSAEINVAL,          "Invalid argument" },
    { WSAEMFILE,          "Too many open sockets" },
    { WSAEWOULDBLOCK,     "Operation would block" },
    { WSAEINPROGRESS,     "Operation now in progress" },
    { WSAEALREADY,        "Operation already in progress" },
    { WSAENOTSOCK,        "Socket operation on non-socket" },
    { WSAEDESTADDRREQ,    "Destination address required" },
    { WSAEMSGSIZE,        "Message too long" },
    { WSAEPROTOTYPE,      "Protocol wrong type for socket" },
    { WSAENOPROTOOPT,     "Bad protocol option" },
    { WSAEPROTONOSUPPORT, "Protocol not supported" },
    { WSAESOCKTNOSUPPORT, "Socket type not supported" },
    { WSAEOPNOTSUPP,      "Operation not supported on socket" },
    { WSAEPFNOSUPPORT,    "Protocol family not supported" },
    { WSAEAFNOSUPPORT,    "Address family not supported" },
    { WSAEADDRINUSE,      "Address already in use" },
    { WSAEADDRNOTAVAIL,   "Can't assign requested address" },
    { WSAENETDOWN,        "Network is down" },
    { WSAENETUNREACH,     "Network is unreachable" },
    { WSAENETRESET,       "Net connection reset" },
    { WSAECONNABORTED,    "Software caused connection abort" },
    { WSAECONNRESET,      "Connection reset by peer" },
    { WSAENOBUFS,         "No buffer space available" },
    { WSAEISCONN,         "Socket is already connected" },
    { WSAENOTCONN,        "Socket is not connected" },
    { WSAESHUTDOWN,       "Can't send after socket shutdown" },
    { WSAETOOMANYREFS,    "Too many references, can't splice" },
    { WSAETIMEDOUT,       "Connection timed out" },
    { WSAECONNREFUSED,    "Connection refused" },
    { WSAELOOP,           "Too many levels of symbolic links" },
    { WSAENAMETOOLONG,    "File name too long" },
    { WSAEHOSTDOWN,       "Host is down" },
    { WSAEHOSTUNREACH,    "No route to host" },
    { WSAENOTEMPTY,       "Directory not empty" },
    { WSAEPROCLIM,        "Too many processes" },
    { WSAEUSERS,          "Too many users" },
    { WSAEDQUOT,          "Disc quota exceeded" },
    { WSAESTALE,          "Stale NFS file handle" },
    { WSAEREMOTE,         "Too many levels of remote in path" },
    { WSASYSNOTREADY,     "Network system is unavailable" },
    { WSAVERNOTSUPPORTED, "Winsock version out of range" },
    { WSANOTINITIALISED,  "WSAStartup not yet called" },
    { WSAEDISCON,         "Graceful shutdown in progress" },
    { WSAHOST_NOT_FOUND,  "Host not found" },
    { WSANO_DATA,         "No host data of that type was found" },
    { 0, NULL }
};

void lscp_socket_perror ( const char *pszPrefix )
{
    int         iError   = WSAGetLastError();
    const char *pszError = "Unknown error code";
    int         i;

    for (i = 0; _wsaErrorCodes[i].pszError; i++) {
        if (_wsaErrorCodes[i].iError == iError) {
            pszError = _wsaErrorCodes[i].pszError;
            break;
        }
    }
    
    fprintf(stderr, "%s: %s (%d)\n", pszPrefix, pszError, iError);
}

void lscp_socket_herror ( const char *pszPrefix )
{
    lscp_socket_perror(pszPrefix);
}

#else

void lscp_socket_perror ( const char *pszPrefix )
{
    perror(pszPrefix);
}

void lscp_socket_herror ( const char *pszPrefix )
{
    herror(pszPrefix);
}

#endif


static void _lscp_socket_getopt_bool ( lscp_socket_t sock, const char *pszOptName, int iOptName )
{
    int iSockOpt;
    int iSockLen = sizeof(int);
    char szPrefix[33];

    sprintf(szPrefix, "  %s\t", pszOptName);
    if (getsockopt(sock, SOL_SOCKET, iOptName, (char *) &iSockOpt, &iSockLen) == SOCKET_ERROR)
        lscp_socket_perror(szPrefix);
    else
        fprintf(stderr, "%s: %s\n", szPrefix, (iSockOpt ? "ON" : "OFF"));
}

static void _lscp_socket_getopt_int  ( lscp_socket_t sock, const char *pszOptName, int iOptName )
{
    int iSockOpt;
    int iSockLen = sizeof(int);
    char szPrefix[33];

    sprintf(szPrefix, "  %s\t", pszOptName);
    if (getsockopt(sock, SOL_SOCKET, iOptName, (char *) &iSockOpt, &iSockLen) == SOCKET_ERROR)
        lscp_socket_perror(szPrefix);
    else
        fprintf(stderr, "%s: %d\n", szPrefix, iSockOpt);
}

void lscp_socket_getopts ( const char *pszPrefix, lscp_socket_t sock )
{
    fprintf(stderr, "%s: sock=%d:\n", pszPrefix, sock);
    
    _lscp_socket_getopt_bool(sock, "SO_BROADCAST",  SO_BROADCAST);
    _lscp_socket_getopt_bool(sock, "SO_DEBUG",      SO_DEBUG);
#if defined(WIN32)
    _lscp_socket_getopt_bool(sock, "SO_DONTLINGER", SO_DONTLINGER);
#endif
    _lscp_socket_getopt_bool(sock, "SO_DONTROUTE",  SO_DONTROUTE);
    _lscp_socket_getopt_bool(sock, "SO_KEEPALIVE",  SO_KEEPALIVE);
    _lscp_socket_getopt_bool(sock, "SO_OOBINLINE",  SO_OOBINLINE);
    _lscp_socket_getopt_int (sock, "SO_RCVBUF",     SO_RCVBUF);
    _lscp_socket_getopt_bool(sock, "SO_REUSEADDR",  SO_REUSEADDR);
    _lscp_socket_getopt_int (sock, "SO_SNDBUF",     SO_SNDBUF);
}

void lscp_socket_trace ( const char *pszPrefix, struct sockaddr_in *pAddr, const char *pchBuffer, int cchBuffer )
{
    char *pszBuffer;

    fprintf(stderr, "%s: addr=%s port=%d:\n",
        pszPrefix,
        inet_ntoa(pAddr->sin_addr),
        htons(pAddr->sin_port)
    );
    
    if (pchBuffer && cchBuffer > 0) {
        pszBuffer = (char *) malloc(cchBuffer + 1);
        if (pszBuffer) {
            memcpy(pszBuffer, pchBuffer, cchBuffer);
            while (cchBuffer > 0 && (pszBuffer[cchBuffer - 1] == '\n' || pszBuffer[cchBuffer- 1] == '\r'))
                cchBuffer--;
            pszBuffer[cchBuffer] = (char) 0;
            fprintf(stderr, "< %s\n", pszBuffer);
            free(pszBuffer);
        }
    } 
    else fprintf(stderr, "< (null)\n");
}


//-------------------------------------------------------------------------
// Threaded socket agent struct helpers.

void lscp_socket_agent_init ( lscp_socket_agent_t *pAgent, lscp_socket_t sock, struct sockaddr_in *pAddr, int cAddr )
{
    memset(pAgent, 0, sizeof(lscp_socket_agent_t));

    pAgent->sock = sock;
    pAgent->pThread = NULL;
    pAgent->iState = 0;

    if (pAddr)
        memmove((char *) &(pAgent->addr), pAddr, cAddr);
}


lscp_status_t lscp_socket_agent_start ( lscp_socket_agent_t *pAgent, lscp_thread_proc_t pfnProc, void *pvData, int iDetach )
{
    if (pAgent->iState)
        pAgent->iState = 0;
    if (pAgent->pThread)
        lscp_thread_destroy(pAgent->pThread);

    pAgent->iState  = 1;
    pAgent->pThread = lscp_thread_create(pfnProc, pvData, iDetach);

    return (pAgent->pThread == NULL ? LSCP_FAILED : LSCP_OK);
}


lscp_status_t lscp_socket_agent_join ( lscp_socket_agent_t *pAgent )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pAgent->pThread)
        ret = lscp_thread_join(pAgent->pThread);

    return ret;
}


lscp_status_t lscp_socket_agent_free ( lscp_socket_agent_t *pAgent )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pAgent->iState)
        pAgent->iState = 0;

    if (pAgent->sock != INVALID_SOCKET)
        closesocket(pAgent->sock);
    pAgent->sock = INVALID_SOCKET;

    if (pAgent->pThread)
        ret = lscp_thread_destroy(pAgent->pThread);
    pAgent->pThread = NULL;

    return ret;
}


// end of socket.c
