// common.h
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

#ifndef __LSCP_COMMON_H
#define __LSCP_COMMON_H

#include "lscp/client.h"
#include "lscp/device.h"


// Case unsensitive comparison substitutes.
#if defined(WIN32)
#define strcasecmp      stricmp
#define strncasecmp     strnicmp
#endif

//-------------------------------------------------------------------------
// Client opaque descriptor struct.

struct _lscp_client_t
{
    // Client socket stuff.
    lscp_client_proc_t  pfnCallback;
    void *              pvData;
    lscp_socket_agent_t cmd;
    lscp_socket_agent_t evt;
    // Subscribed events.
    lscp_event_t        events;
    // Client struct persistent caches.
    char **             audio_drivers;
    char **             midi_drivers;
    int  *              audio_devices;
    int  *              midi_devices;
    char **             engines;
    int  *              channels;
    // Client struct volatile caches.
    lscp_driver_info_t  audio_info;
    lscp_driver_info_t  midi_info;
    lscp_param_info_t   audio_param_info;
    lscp_param_info_t   midi_param_info;
    lscp_engine_info_t  engine_info;
    lscp_channel_info_t channel_info;
    // Result and error status.
    char *              pszResult;
    int                 iErrno;
    // Stream buffers status.
    lscp_buffer_fill_t *buffer_fill;
    int                 iStreamCount;
    // Transaction call timeout (msecs).
    int                 iTimeout;
    lscp_mutex_t        mutex;
};


//-------------------------------------------------------------------------
// Local client request executive.

lscp_status_t   lscp_client_call            (lscp_client_t *pClient, const char *pszQuery);
void            lscp_client_set_result      (lscp_client_t *pClient, char *pszResult, int iErrno);

//-------------------------------------------------------------------------
// General utility function prototypes.

char *          lscp_strtok                 (char *pchBuffer, const char *pszSeps, char **ppch);
char *          lscp_ltrim                  (char *psz);
char *          lscp_unquote                (char **ppsz, int dup);

char **         lscp_szsplit_create         (const char *pszCsv, const char *pszSeps);
void            lscp_szsplit_destroy        (char **ppszSplit);
#ifdef LSCP_SZSPLIT_COUNT
int             lscp_szsplit_count          (char **ppszSplit);
int             lscp_szsplit_size           (char **ppszSplit);
#endif

int *           lscp_isplit_create          (const char *pszCsv, const char *pszSeps);
void            lscp_isplit_destroy         (int *piSplit);
#ifdef LSCP_ISPLIT_COUNT
int             lscp_isplit_count           (int *piSplit);
int             lscp_isplit_size            (int *piSplit);
#endif

lscp_param_t *  lscp_psplit_create          (const char *pszCsv, const char *pszSep1, const char *pszSep2);
void            lscp_psplit_destroy         (lscp_param_t *ppSplit);
#ifdef LSCP_PSPLIT_COUNT
int             lscp_psplit_count           (lscp_param_t *ppSplit);
int             lscp_psplit_size            (lscp_param_t *ppSplit);
#endif

//-------------------------------------------------------------------------
// Engine struct helper functions.

void            lscp_engine_info_init       (lscp_engine_info_t *pEngineInfo);
void            lscp_engine_info_reset      (lscp_engine_info_t *pEngineInfo);

//-------------------------------------------------------------------------
// Channel struct helper functions.

void            lscp_channel_info_init      (lscp_channel_info_t *pChannelInfo);
void            lscp_channel_info_reset     (lscp_channel_info_t *pChannelInfo);

//-------------------------------------------------------------------------
// Driver struct helper functions.

void            lscp_driver_info_init       (lscp_driver_info_t *pDriverInfo);
void            lscp_driver_info_reset      (lscp_driver_info_t *pDriverInfo);

//-------------------------------------------------------------------------
// Parameter struct helper functions.

void            lscp_param_info_init        (lscp_param_info_t *pParamInfo);
void            lscp_param_info_reset       (lscp_param_info_t *pParamInfo);

//-------------------------------------------------------------------------
// Concatenate a parameter list (key='value'...) into a string.

int             lscp_param_concat           (char *pszBuffer, int cchMaxBuffer, lscp_param_t *pParams);


#endif // __LSCP_COMMON_H

// end of common.h
