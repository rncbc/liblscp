// event.h
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

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*****************************************************************************/

#ifndef __LSCP_EVENT_H
#define __LSCP_EVENT_H

#include "lscp/socket.h"

#if defined(__cplusplus)
extern "C" {
#endif


/** Subscribable event notification bit-wise flags. */
typedef enum _lscp_event_t
{
    LSCP_EVENT_NONE             = 0x0000,
    LSCP_EVENT_CHANNEL_COUNT    = 0x0001,
    LSCP_EVENT_VOICE_COUNT      = 0x0002,
    LSCP_EVENT_STREAM_COUNT     = 0x0004,
    LSCP_EVENT_BUFFER_FILL      = 0x0008,
    LSCP_EVENT_CHANNEL_INFO     = 0x0010,
    LSCP_EVENT_MISCELLANEOUS    = 0x1000

} lscp_event_t;


//-------------------------------------------------------------------------
// Simple event helper functions.

const char *    lscp_event_to_text      ( lscp_event_t event );
lscp_event_t    lscp_event_from_text    ( const char *pszText );


#if defined(__cplusplus)
}
#endif

#endif // __LSCP_EVENT_H

// end of event.h
