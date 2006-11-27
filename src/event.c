// event.c
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

#include "lscp/event.h"

// Case unsensitive comparison substitutes.
#if defined(WIN32)
#define strcasecmp      stricmp
#define strncasecmp     strnicmp
#endif


//-------------------------------------------------------------------------
// Simple event helper functions.


/**
 *  Getting the text string representation of a single event.
 *
 *  @param event  Event to convert to text string.
 *
 *  @returns The text string representation of the event.
 */
const char *lscp_event_to_text ( lscp_event_t event )
{
    const char *pszText = NULL;

    switch (event) {
      case LSCP_EVENT_CHANNEL_COUNT:
        pszText = "CHANNEL_COUNT";
        break;
      case LSCP_EVENT_VOICE_COUNT:
        pszText = "VOICE_COUNT";
        break;
      case LSCP_EVENT_STREAM_COUNT:
        pszText = "STREAM_COUNT";
        break;
      case LSCP_EVENT_BUFFER_FILL:
        pszText = "BUFFER_FILL";
        break;
      case LSCP_EVENT_CHANNEL_INFO:
        pszText = "CHANNEL_INFO";
        break;
      case LSCP_EVENT_TOTAL_VOICE_COUNT:
        pszText = "TOTAL_VOICE_COUNT";
        break;
      case LSCP_EVENT_MISCELLANEOUS:
        pszText = "MISCELLANEOUS";
        break;
      case LSCP_EVENT_NONE:
      default:
        break;
    }

    return pszText;
}


/**
 *  Getting an event from a text string.
 *
 *  @param pszText  Text string to convert to event.
 *
 *  @returns The event correponding to the text string.
 */
lscp_event_t lscp_event_from_text ( const char *pszText )
{
    lscp_event_t event = LSCP_EVENT_NONE;

    if (pszText) {
        if (strcasecmp(pszText, "CHANNEL_COUNT") == 0)
            event = LSCP_EVENT_CHANNEL_COUNT;
        else if (strcasecmp(pszText, "VOICE_COUNT") == 0)
            event = LSCP_EVENT_VOICE_COUNT;
        else if (strcasecmp(pszText, "STREAM_COUNT") == 0)
            event = LSCP_EVENT_STREAM_COUNT;
        else if (strcasecmp(pszText, "BUFFER_FILL") == 0)
            event = LSCP_EVENT_BUFFER_FILL;
        else if (strcasecmp(pszText, "CHANNEL_INFO") == 0)
            event = LSCP_EVENT_CHANNEL_INFO;
        else if (strcasecmp(pszText, "TOTAL_VOICE_COUNT") == 0)
            event = LSCP_EVENT_TOTAL_VOICE_COUNT;
        else if (strcasecmp(pszText, "MISCELLANEOUS") == 0)
            event = LSCP_EVENT_MISCELLANEOUS;
    }

    return event;
}


// end of event.c
