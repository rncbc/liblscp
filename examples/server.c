// server.c
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

#include "server.h"

#define LSCP_SERVER_SLEEP   30          // Period in seconds for watchdog wakeup (idle loop).


// Local prototypes.

static lscp_connect_t  *_lscp_connect_create            (lscp_server_t *pServer, lscp_socket_t sock, struct sockaddr_in *pAddr, int cAddr);
static lscp_status_t    _lscp_connect_destroy           (lscp_connect_t *pConnect);
static lscp_status_t    _lscp_connect_recv              (lscp_connect_t *pConnect);

static void             _lscp_connect_list_append       (lscp_connect_list_t *pList, lscp_connect_t *pItem);
static void             _lscp_connect_list_remove       (lscp_connect_list_t *pList, lscp_connect_t *pItem);
static void             _lscp_connect_list_remove_safe  (lscp_connect_list_t *pList, lscp_connect_t *pItem);
static void             _lscp_connect_list_free         (lscp_connect_list_t *pList);
static lscp_connect_t  *_lscp_connect_list_find_sock    (lscp_connect_list_t *pList, lscp_socket_t sock);

static void             _lscp_server_thread_proc        (lscp_server_t *pServer);
static void             _lscp_server_select_proc        (lscp_server_t *pServer);

static void             _lscp_server_agent_proc         (void *pvServer);


//-------------------------------------------------------------------------
// Server-side client connection list methods.

static void _lscp_connect_list_init ( lscp_connect_list_t *pList )
{
//  fprintf(stderr, "_lscp_connect_list_init: pList=%p.\n", pList);

    pList->first = NULL;
    pList->last  = NULL;
    pList->count = 0;

    lscp_mutex_init(pList->mutex);
}


static void _lscp_connect_list_append ( lscp_connect_list_t *pList, lscp_connect_t *pItem )
{
//  fprintf(stderr, "_lscp_connect_list_append: pList=%p pItem=%p.\n", pList, pItem);

    lscp_mutex_lock(pList->mutex);

    pItem->prev = pList->last;
    pItem->next = NULL;

    if (pList->last)
        (pList->last)->next = pItem;
    else
        pList->first = pItem;

    pList->last = pItem;

    pList->count++;

    lscp_mutex_unlock(pList->mutex);
}


static void _lscp_connect_list_remove ( lscp_connect_list_t *pList, lscp_connect_t *pItem )
{
//  fprintf(stderr, "_lscp_connect_list_remove: pList=%p pItem=%p.\n", pList, pItem);

    if (pItem->next)
        (pItem->next)->prev = pItem->prev;
    else
        pList->last = pItem->prev;

    if (pItem->prev)
        (pItem->prev)->next = pItem->next;
    else
        pList->first = pItem->next;

    pItem->next = NULL;
    pItem->prev = NULL;

    pList->count--;
}


static void _lscp_connect_list_remove_safe ( lscp_connect_list_t *pList, lscp_connect_t *pItem )
{
    lscp_connect_t *p;

//  fprintf(stderr, "_lscp_connect_list_remove_safe: pList=%p pItem=%p.\n", pList, pItem);

    lscp_mutex_lock(pList->mutex);

    for (p = pList->first; p; p = p->next) {
        if (p == pItem) {
            _lscp_connect_list_remove(pList, pItem);
            break;
        }
    }

    lscp_mutex_unlock(pList->mutex);
}


static void _lscp_connect_list_free ( lscp_connect_list_t *pList )
{
    lscp_connect_t *p, *pNext;

//  fprintf(stderr, "_lscp_connect_list_free: pList=%p.\n", pList);

    lscp_mutex_lock(pList->mutex);

    for (p = pList->first; p; p = pNext) {
        pNext = p->next;
        _lscp_connect_list_remove(pList, p);
        _lscp_connect_destroy(p);
    }

    lscp_mutex_unlock(pList->mutex);
    
    lscp_mutex_destroy(pList->mutex);
}


static lscp_connect_t *_lscp_connect_list_find_sock ( lscp_connect_list_t *pList, lscp_socket_t sock )
{
    lscp_connect_t *p;

//  fprintf(stderr, "_lscp_connect_list_find_sock: pList=%p sock=%d.\n", pList, sock);

    for (p = pList->first; p; p = p->next) {
        if (sock == p->client.sock)
            return p;
    }

    return NULL;
}

//-------------------------------------------------------------------------
// Server-side threaded client connections.

static void _lscp_connect_proc ( void *pvConnect )
{
    lscp_connect_t *pConnect = (lscp_connect_t *) pvConnect;
    lscp_server_t  *pServer  = pConnect->server;

    while (pServer->agent.iState && pConnect->client.iState) {
        if (_lscp_connect_recv(pConnect) != LSCP_OK)
            pConnect->client.iState = 0;
    }

    (*pServer->pfnCallback)(pConnect, NULL, LSCP_CONNECT_CLOSE, pServer->pvData);
    _lscp_connect_list_remove_safe(&(pServer->connects), pConnect);
    closesocket(pConnect->client.sock);
}

static lscp_connect_t *_lscp_connect_create ( lscp_server_t *pServer, lscp_socket_t sock, struct sockaddr_in *pAddr, int cAddr )
{
    lscp_connect_t *pConnect;

    if (pServer == NULL || sock == INVALID_SOCKET || pAddr == NULL) {
        fprintf(stderr, "_lscp_connect_create: Invalid connection arguments.\n");
        return NULL;
    }

    pConnect = (lscp_connect_t *) malloc(sizeof(lscp_connect_t));
    if (pConnect == NULL) {
        fprintf(stderr, "_lscp_connect_create: Out of memory.\n");
        closesocket(sock);
        return NULL;
    }
    memset(pConnect, 0, sizeof(lscp_connect_t));

    pConnect->server = pServer;
    pConnect->events = LSCP_EVENT_NONE;

#ifdef DEBUG
    fprintf(stderr, "_lscp_connect_create: pConnect=%p: sock=%d addr=%s port=%d.\n", pConnect, sock, inet_ntoa(pAddr->sin_addr), ntohs(pAddr->sin_port));
#endif

    lscp_socket_agent_init(&(pConnect->client), sock, pAddr, cAddr);

    if (pServer->mode == LSCP_SERVER_THREAD) {
        if (lscp_socket_agent_start(&(pConnect->client), _lscp_connect_proc, pConnect, 0) != LSCP_OK) {
            closesocket(sock);
            free(pConnect);
            return NULL;
        }
    }

    return pConnect;
}


static lscp_status_t _lscp_connect_destroy ( lscp_connect_t *pConnect )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pConnect == NULL)
        return ret;

#ifdef DEBUG
    fprintf(stderr, "_lscp_connect_destroy: pConnect=%p.\n", pConnect);
#endif

    lscp_socket_agent_free(&(pConnect->client));

    free(pConnect);

    return ret;
}


lscp_status_t _lscp_connect_recv ( lscp_connect_t *pConnect )
{
    lscp_status_t ret = LSCP_FAILED;
    lscp_server_t *pServer;
    char achBuffer[LSCP_BUFSIZ];
    int cchBuffer;

    if (pConnect == NULL)
        return ret;

    pServer = pConnect->server;
    if (pServer == NULL)
        return ret;

    cchBuffer = recv(pConnect->client.sock, achBuffer, sizeof(achBuffer), 0);
    if (cchBuffer > 0)
        ret = (*pServer->pfnCallback)(pConnect, achBuffer, cchBuffer, pServer->pvData);
    else if (cchBuffer < 0)
        lscp_socket_perror("_lscp_connect_recv: recv");

    return ret;
}


//-------------------------------------------------------------------------
// Command service (stream oriented).

static void _lscp_server_thread_proc ( lscp_server_t *pServer )
{
    lscp_socket_t sock;
    struct sockaddr_in addr;
    int cAddr;
    lscp_connect_t *pConnect;

#ifdef DEBUG
    fprintf(stderr, "_lscp_server_thread_proc: Server listening for connections.\n");
#endif

    while (pServer->agent.iState) {
        cAddr = sizeof(struct sockaddr_in);
        sock = accept(pServer->agent.sock, (struct sockaddr *) &addr, &cAddr);
        if (sock == INVALID_SOCKET) {
            lscp_socket_perror("_lscp_server_thread_proc: accept");
            pServer->agent.iState = 0;
        } else {
            pConnect = _lscp_connect_create(pServer, sock, &addr, cAddr);
            if (pConnect) {
                _lscp_connect_list_append(&(pServer->connects), pConnect);
                (*pServer->pfnCallback)(pConnect, NULL, LSCP_CONNECT_OPEN, pServer->pvData);
            }
        }
    }

#ifdef DEBUG
    fprintf(stderr, "_lscp_server_thread_proc: Server closing.\n");
#endif
}


static void _lscp_server_select_proc ( lscp_server_t *pServer )
{
    fd_set master_fds;  // Master file descriptor list.
    fd_set select_fds;  // temp file descriptor list for select().
    int fd, fdmax;      // Maximum file descriptor number.
    struct timeval tv;  // For specifying a timeout value.
    int iSelect;        // Holds select return status.

    lscp_socket_t sock;
    struct sockaddr_in addr;
    int cAddr;
    lscp_connect_t *pConnect;

#ifdef DEBUG
    fprintf(stderr, "_lscp_server_select_proc: Server listening for connections.\n");
#endif
    FD_ZERO(&master_fds);
    FD_ZERO(&select_fds);

    // Add the listener to the master set
    FD_SET((unsigned int) pServer->agent.sock, &master_fds);

    // Keep track of the biggest file descriptor;
    // So far, it's ourself, the listener.
    fdmax = (int) pServer->agent.sock;

    // Main loop...
    while (pServer->agent.iState) {

        // Use a copy of the master.
        select_fds = master_fds;
        // Use the timeout feature for watchdoggin.
        tv.tv_sec = LSCP_SERVER_SLEEP;
        tv.tv_usec = 0;
        // Wait for events...
        iSelect = select(fdmax + 1, &select_fds, NULL, NULL, &tv);

        if (iSelect < 0) {
            lscp_socket_perror("_lscp_server_select_proc: select");
            pServer->agent.iState = 0;
        }
        else if (iSelect > 0) {
            // Run through the existing connections looking for data to read...
            for (fd = 0; fd < fdmax + 1; fd++) {
                if (FD_ISSET(fd, &select_fds)) {    // We got one!!
                    // Is it ourselves, the command listener?
                    if (fd == (int) pServer->agent.sock) {
                        // Accept the connection...
                        cAddr = sizeof(struct sockaddr_in);
                        sock = accept(pServer->agent.sock, (struct sockaddr *) &addr, &cAddr);
                        if (sock == INVALID_SOCKET) {
                            lscp_socket_perror("_lscp_server_select_proc: accept");
                            pServer->agent.iState = 0;
                        } else {
                            // Add to master set.
                            FD_SET((unsigned int) sock, &master_fds);
                            // Keep track of the maximum.
                            if ((int) sock > fdmax)
                                fdmax = (int) sock;
                            // And do create the client connection entry.
                            pConnect = _lscp_connect_create(pServer, sock, &addr, cAddr);
                            if (pConnect) {
                                _lscp_connect_list_append(&(pServer->connects), pConnect);
                                (*pServer->pfnCallback)(pConnect, NULL, LSCP_CONNECT_OPEN, pServer->pvData);
                            }
                        }
                        // Done with one new connection.
                    } else {
                        // Otherwise it's trivial transaction...
                        lscp_mutex_lock(pServer->connects.mutex);
                        // Find the connection on our cache...
                        pConnect = _lscp_connect_list_find_sock(&(pServer->connects), (lscp_socket_t) fd);
                        // Handle data from a client.
                        if (_lscp_connect_recv(pConnect) != LSCP_OK) {
                            // Say bye bye!
                            if (pConnect) {
                                (*pServer->pfnCallback)(pConnect, NULL, LSCP_CONNECT_CLOSE, pServer->pvData);
                                _lscp_connect_list_remove(&(pServer->connects), pConnect);
                                _lscp_connect_destroy(pConnect);
                            }
                            // Remove from master set.
                            FD_CLR((unsigned int) fd, &master_fds);
                        }
                        lscp_mutex_unlock(pServer->connects.mutex);
                    }
                }
            }
            // Done (iSelect > 0)
        }
    }

#ifdef DEBUG
    fprintf(stderr, "_lscp_server_select_proc: Server closing.\n");
#endif
}


static void _lscp_server_agent_proc ( void *pvServer )
{
    lscp_server_t *pServer = (lscp_server_t *) pvServer;

    if (pServer->mode == LSCP_SERVER_THREAD)
        _lscp_server_thread_proc(pServer);
    else
        _lscp_server_select_proc(pServer);
}


//-------------------------------------------------------------------------
// Server versioning teller fuunction.

/** Retrieve the current server library version string. */
const char* lscp_server_package (void) { return LSCP_PACKAGE; }

/** Retrieve the current server library version string. */
const char* lscp_server_version (void) { return LSCP_VERSION; }

/** Retrieve the current server library build timestamp string. */
const char* lscp_server_build   (void) { return __DATE__ " " __TIME__; }


//-------------------------------------------------------------------------
// Server sockets.

/**
 *  Create a server instance, listening on the given port for client
 *  connections. A server callback function must be suplied that will
 *  handle every and each client request.
 *
 *  @param iPort        Port number where the server will bind for listening.
 *  @param pfnCallback  Callback function to receive and handle client requests.
 *  @param pvData       Server context opaque data, that will be passed
 *                      to the callback function without change.
 *
 *  @returns The new server instance pointer @ref lscp_server_t if successfull,
 *  which shall be used on all subsequent server calls, NULL otherwise.
 */
lscp_server_t* lscp_server_create ( int iPort, lscp_server_proc_t pfnCallback, void *pvData )
{
    return lscp_server_create_ex(iPort, pfnCallback, pvData, LSCP_SERVER_SELECT);
}


/**
 *  Create a server instance, listening on the given port for client
 *  connections. A server callback function must be suplied that will
 *  handle every and each client request. A server threading model
 *  maybe specified either as multi-threaded (one thread per client)
 *  or single thread multiplex mode (one thread serves all clients).
 *
 *  @param iPort        Port number where the server will bind for listening.
 *  @param pfnCallback  Callback function to receive and handle client requests.
 *  @param pvData       Server context opaque data, that will be passed
 *                      to the callback function without change.
 *  @param mode         Server mode of operation, regarding the internal
 *                      threading model, either @ref LSCP_SERVER_THREAD for
 *                      a multi-threaded server, or @ref LSCP_SERVER_SELECT
 *                      for a single-threaded multiplexed server.
 *
 *  @returns The new server instance pointer if successfull, which shall be
 *  used on all subsequent server calls, NULL otherwise.
 */
lscp_server_t* lscp_server_create_ex ( int iPort, lscp_server_proc_t pfnCallback, void *pvData, lscp_server_mode_t mode )
{
    lscp_server_t *pServer;
    lscp_socket_t sock;
    struct sockaddr_in addr;
    int cAddr;
    int iSockOpt = (-1);

    if (pfnCallback == NULL) {
        fprintf(stderr, "lscp_server_create: Invalid server callback function.\n");
        return NULL;
    }

    // Allocate server descriptor...

    pServer = (lscp_server_t *) malloc(sizeof(lscp_server_t));
    if (pServer == NULL) {
        fprintf(stderr, "lscp_server_create: Out of memory.\n");
        return NULL;
    }
    memset(pServer, 0, sizeof(lscp_server_t));

    _lscp_connect_list_init(&(pServer->connects));

    pServer->mode = mode;
    pServer->pfnCallback = pfnCallback;
    pServer->pvData = pvData;

#ifdef DEBUG
    fprintf(stderr, "lscp_server_create: pServer=%p: iPort=%d.\n", pServer, iPort);
#endif

    // Prepare the command stream server socket...

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        lscp_socket_perror("lscp_server_create: socket");
        free(pServer);
        return NULL;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &iSockOpt, sizeof(int)) == SOCKET_ERROR)
        lscp_socket_perror("lscp_server_create: setsockopt(SO_REUSEADDR)");
#if defined(WIN32)
    if (setsockopt(sock, SOL_SOCKET, SO_DONTLINGER, (char *) &iSockOpt, sizeof(int)) == SOCKET_ERROR)
        lscp_socket_perror("lscp_server_create: setsockopt(SO_DONTLINGER)");
#endif

#ifdef DEBUG
    lscp_socket_getopts("lscp_server_create", sock);
#endif

    cAddr = sizeof(struct sockaddr_in);
    memset((char *) &addr, 0, cAddr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((short) iPort);

    if (bind(sock, (const struct sockaddr *) &addr, cAddr) == SOCKET_ERROR) {
        lscp_socket_perror("lscp_server_create: bind");
        closesocket(sock);
        free(pServer);
        return NULL;
    }

    if (listen(sock, 10) == SOCKET_ERROR) {
        lscp_socket_perror("lscp_server_create: listen");
        closesocket(sock);
        free(pServer);
        return NULL;
    }

    if (iPort == 0) {
        if (getsockname(sock, (struct sockaddr *) &addr, &cAddr) == SOCKET_ERROR) {
            lscp_socket_perror("lscp_server_create: getsockname");
            closesocket(sock);
            free(pServer);
        }
    }

    lscp_socket_agent_init(&(pServer->agent), sock, &addr, cAddr);

#ifdef DEBUG
    fprintf(stderr, "lscp_server_create: sock=%d addr=%s port=%d.\n", pServer->agent.sock, inet_ntoa(pServer->agent.addr.sin_addr), ntohs(pServer->agent.addr.sin_port));
#endif

    // Now's finally time to startup threads...

    // Command service thread...
    if (lscp_socket_agent_start(&(pServer->agent), _lscp_server_agent_proc, pServer, 0) != LSCP_OK) {
        lscp_socket_agent_free(&(pServer->agent));
        free(pServer);
        return NULL;
    }

    // Finally we've some success...
    return pServer;
}


/**
 *  Wait for a server instance to terminate graciously.
 *
 *  @param pServer  Pointer to server instance structure.
 */
lscp_status_t lscp_server_join ( lscp_server_t *pServer )
{
    if (pServer == NULL)
        return LSCP_FAILED;

#ifdef DEBUG
    fprintf(stderr, "lscp_server_join: pServer=%p.\n", pServer);
#endif

    lscp_socket_agent_join(&(pServer->agent));

    return LSCP_OK;
}


/**
 *  Terminate and destroy a server instance.
 *
 *  @param pServer  Pointer to server instance structure.
 */
lscp_status_t lscp_server_destroy ( lscp_server_t *pServer )
{
    if (pServer == NULL)
        return LSCP_FAILED;

#ifdef DEBUG
    fprintf(stderr, "lscp_server_destroy: pServer=%p.\n", pServer);
#endif

    _lscp_connect_list_free(&(pServer->connects));
    lscp_socket_agent_free(&(pServer->agent));

    free(pServer);

    return LSCP_OK;
}


/**
 *  Send an event notification message to all subscribed clients.
 *
 *  @param pServer  Pointer to server instance structure.
 *  @param event    Event type flag to send to all subscribed clients.
 *  @param pchData  Pointer to event data to be sent to all clients.
 *  @param cchData  Length of the event data to be sent in bytes.
 *
 *  @returns LSCP_OK on success, LSCP_FAILED otherwise.
 */
lscp_status_t lscp_server_broadcast ( lscp_server_t *pServer, lscp_event_t event, const char *pchData, int cchData )
{
    lscp_connect_t *p;
    const char *pszEvent;
    char  achBuffer[LSCP_BUFSIZ];
    int   cchBuffer;

    if (pServer == NULL)
        return LSCP_FAILED;
    if (pchData == NULL || cchData < 1)
        return LSCP_FAILED;

    // Which (single) event?
    pszEvent = lscp_event_to_text(event);
    if (pszEvent == NULL)
        return LSCP_FAILED;

    // Build the event message string...
    cchBuffer = sprintf(achBuffer, "NOTIFY:%s:", pszEvent);
    if (pchData) {
        if (cchData > LSCP_BUFSIZ - cchBuffer - 2)
            cchData = LSCP_BUFSIZ - cchBuffer - 2;
        strncpy(&achBuffer[cchBuffer], pchData, cchData);
        cchBuffer += cchData;
    }
    achBuffer[cchBuffer++] = '\r';
    achBuffer[cchBuffer++] = '\n';

    // And do the direct broadcasting...
    
    lscp_mutex_lock(pServer->connects.mutex);

    for (p = pServer->connects.first; p; p = p->next) {
        if (p->events & event)
            send(p->client.sock, achBuffer, cchBuffer, 0);
    }

    lscp_mutex_unlock(pServer->connects.mutex);

    return LSCP_OK;
}


/**
 *  Send response for the current client callback request.
 *
 *  @param pConnect     Pointer to client connection instance structure.
 *  @param pchBuffer    Pointer to data to be sent to the client as response.
 *  @param cchBuffer    Length of the response data to be sent in bytes.
 *
 *  @returns LSCP_OK on success, LSCP_FAILED otherwise.
 */
lscp_status_t lscp_server_result ( lscp_connect_t *pConnect, const char *pchBuffer, int cchBuffer )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pConnect == NULL)
        return ret;
    if (pchBuffer == NULL || cchBuffer < 1)
        return ret;

    if (send(pConnect->client.sock, pchBuffer, cchBuffer, 0) != cchBuffer)
        lscp_socket_perror("lscp_server_result");
    else
        ret = LSCP_OK;

    return ret;
}


/**
 *  Register client as a subscriber of event broadcast messages.
 *
 *  @param pConnect Pointer to client connection instance structure.
 *  @param event    Event type flag of the requesting client subscription.
 *
 *  @returns LSCP_OK on success, LSCP_FAILED otherwise.
 */
lscp_status_t lscp_server_subscribe ( lscp_connect_t *pConnect, lscp_event_t event )
{
    if (pConnect == NULL)
        return LSCP_FAILED;
    if (event == LSCP_EVENT_NONE)
        return LSCP_FAILED;

    pConnect->events |= event;

    return LSCP_OK;
}


/**
 *  Deregister client as subscriber of event broadcast messages.
 *
 *  @param pConnect Pointer to client connection instance structure.
 *  @param event    Event type flag of the requesting client unsubscription.
 *
 *  @returns LSCP_OK on success, LSCP_FAILED otherwise.
*/
lscp_status_t lscp_server_unsubscribe ( lscp_connect_t *pConnect, lscp_event_t event )
{
    if (pConnect == NULL)
        return LSCP_FAILED;
    if (event == LSCP_EVENT_NONE)
        return LSCP_FAILED;

    pConnect->events &= ~event;

    return LSCP_OK;
}


// end of server.c
