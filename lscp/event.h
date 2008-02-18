// event.h
//
/****************************************************************************
   liblscp - LinuxSampler Control Protocol API
   Copyright (C) 2004-2008, rncbc aka Rui Nuno Capela. All rights reserved.

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

#ifndef __LSCP_EVENT_H
#define __LSCP_EVENT_H

#include "lscp/socket.h"

#if defined(__cplusplus)
extern "C" {
#endif


/** Subscribable event notification types. */
typedef enum _lscp_event_t
{
	LSCP_EVENT_NONE                      = 0x0000,
	LSCP_EVENT_CHANNEL_COUNT             = 0x0001,
	LSCP_EVENT_VOICE_COUNT               = 0x0002,
	LSCP_EVENT_STREAM_COUNT              = 0x0004,
	LSCP_EVENT_BUFFER_FILL               = 0x0008,
	LSCP_EVENT_CHANNEL_INFO              = 0x0010,
	LSCP_EVENT_TOTAL_VOICE_COUNT         = 0x0020,
	LSCP_EVENT_AUDIO_OUTPUT_DEVICE_COUNT = 0x0040,
	LSCP_EVENT_AUDIO_OUTPUT_DEVICE_INFO  = 0x0080,
	LSCP_EVENT_MIDI_INPUT_DEVICE_COUNT   = 0x0100,
	LSCP_EVENT_MIDI_INPUT_DEVICE_INFO    = 0x0200,
	LSCP_EVENT_MIDI_INSTRUMENT_MAP_COUNT = 0x0400,
	LSCP_EVENT_MIDI_INSTRUMENT_MAP_INFO  = 0x1000,
	LSCP_EVENT_MIDI_INSTRUMENT_COUNT     = 0x2000,
	LSCP_EVENT_MIDI_INSTRUMENT_INFO      = 0x4000,
	LSCP_EVENT_MISCELLANEOUS             = 0x8000,
	// from these new events on, we simply enumerate them,
	// no dedicated bit flags anymore ...
	LSCP_EVENT_CHANNEL_MIDI              = 0x00010000,
	LSCP_EVENT_DEVICE_MIDI               = 0x00020000
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
