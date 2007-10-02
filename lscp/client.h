// client.h
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

#ifndef __LSCP_CLIENT_H
#define __LSCP_CLIENT_H

#include "lscp/socket.h"
#include "lscp/event.h"

#if defined(__cplusplus)
extern "C" {
#endif

//-------------------------------------------------------------------------
// MIDI channel omni mode.

#define LSCP_MIDI_CHANNEL_ALL   16


//-------------------------------------------------------------------------
// Client data structures.

/** Server info cache struct. */
typedef struct _lscp_server_info_t
{
	char *        description;
	char *        version;
	char *        protocol_version;

} lscp_server_info_t;


/** Engine info cache struct. */
typedef struct _lscp_engine_info_t
{
	char *        description;
	char *        version;

} lscp_engine_info_t;


/** Channel info cache struct. */
typedef struct _lscp_channel_info_t
{
	char *        engine_name;
	int           audio_device;
	int           audio_channels;
	int  *        audio_routing;
	char *        instrument_file;
	int           instrument_nr;
	char *        instrument_name;
	int           instrument_status;
	int           midi_device;
	int           midi_port;
	int           midi_channel;
	int           midi_map;
	float         volume;
	int           mute;
	int           solo;

} lscp_channel_info_t;


/** Buffer fill cache struct. */
typedef struct _lscp_buffer_fill_t
{
	unsigned int  stream_id;
	unsigned long stream_usage;

} lscp_buffer_fill_t;


/** Buffer fill stream usage types. */
typedef enum _lscp_usage_t
{
	LSCP_USAGE_BYTES = 0,
	LSCP_USAGE_PERCENTAGE

} lscp_usage_t;


/** Effect send info cache struct. */
typedef struct _lscp_fxsend_info_t
{
	char *        name;
	int           midi_controller;
	int *         audio_routing;
	float         level;

} lscp_fxsend_info_t;


/** MIDI instrument parameter struct. */
typedef struct _lscp_midi_instrument_t
{
	int           map;
	int           bank;
	int           prog;

} lscp_midi_instrument_t;


/** MIDI instrument load mode. */
typedef enum _lscp_load_mode_t
{
	LSCP_LOAD_DEFAULT = 0,
	LSCP_LOAD_ON_DEMAND,
	LSCP_LOAD_ON_DEMAND_HOLD,
	LSCP_LOAD_PERSISTENT

} lscp_load_mode_t;


/** MIDI instrument info cache struct. */
typedef struct _lscp_midi_instrument_info_t
{
	char *           name;
	char *           engine_name;
	char *           instrument_file;
	int              instrument_nr;
	char *           instrument_name;
	lscp_load_mode_t load_mode;
	float            volume;

} lscp_midi_instrument_info_t;


/** MIDI instrument map mode. */
typedef enum _lscp_midi_map_mode_t
{
	LSCP_MIDI_MAP_NONE = -1,
	LSCP_MIDI_MAP_DEFAULT = -2,
	LSCP_MIDI_MAP_ALL = -3

} lscp_midi_map_mode_t;


//-------------------------------------------------------------------------
// Client socket main structure.

/** Client opaque descriptor struct. */
typedef struct _lscp_client_t lscp_client_t;

/** Client event callback procedure prototype. */
typedef lscp_status_t (*lscp_client_proc_t)
(
	struct _lscp_client_t *pClient,
	lscp_event_t event,
	const char *pchData,
	int cchData,
	void *pvData
);

//-------------------------------------------------------------------------
// Client versioning teller function.

const char *            lscp_client_package             (void);
const char *            lscp_client_version             (void);
const char *            lscp_client_build               (void);

//-------------------------------------------------------------------------
// Client socket functions.

lscp_client_t *         lscp_client_create              (const char *pszHost, int iPort, lscp_client_proc_t pfnCallback, void *pvData);
lscp_status_t           lscp_client_join                (lscp_client_t *pClient);
lscp_status_t           lscp_client_destroy             (lscp_client_t *pClient);

lscp_status_t           lscp_client_set_timeout         (lscp_client_t *pClient, int iTimeout);
int                     lscp_client_get_timeout         (lscp_client_t *pClient);

//-------------------------------------------------------------------------
// Client common protocol functions.

lscp_status_t           lscp_client_query               (lscp_client_t *pClient, const char *pszQuery);
const char *            lscp_client_get_result          (lscp_client_t *pClient );
int                     lscp_client_get_errno           (lscp_client_t *pClient );

//-------------------------------------------------------------------------
// Client registration protocol functions.

lscp_status_t           lscp_client_subscribe           (lscp_client_t *pClient, lscp_event_t events);
lscp_status_t           lscp_client_unsubscribe         (lscp_client_t *pClient, lscp_event_t events);

lscp_event_t            lscp_client_get_events          (lscp_client_t *pClient);

//-------------------------------------------------------------------------
// Client command protocol functions.

lscp_status_t           lscp_load_instrument            (lscp_client_t *pClient, const char *pszFileName, int iInstrIndex, int iSamplerChannel);
lscp_status_t           lscp_load_instrument_non_modal  (lscp_client_t *pClient, const char *pszFileName, int iInstrIndex, int iSamplerChannel);
lscp_status_t           lscp_load_engine                (lscp_client_t *pClient, const char *pszEngineName, int iSamplerChannel);
int                     lscp_get_channels               (lscp_client_t *pClient);
int *                   lscp_list_channels              (lscp_client_t *pClient);
int                     lscp_add_channel                (lscp_client_t *pClient);
lscp_status_t           lscp_remove_channel             (lscp_client_t *pClient, int iSamplerChannel);

int                     lscp_get_available_engines      (lscp_client_t *pClient);
const char **           lscp_list_available_engines     (lscp_client_t *pClient);

lscp_engine_info_t *    lscp_get_engine_info            (lscp_client_t *pClient, const char *pszEngineName);
lscp_channel_info_t *   lscp_get_channel_info           (lscp_client_t *pClient, int iSamplerChannel);

int                     lscp_get_channel_voice_count    (lscp_client_t *pClient, int iSamplerChannel);
int                     lscp_get_channel_stream_count   (lscp_client_t *pClient, int iSamplerChannel);
int                     lscp_get_channel_stream_usage   (lscp_client_t *pClient, int iSamplerChannel);

lscp_buffer_fill_t *    lscp_get_channel_buffer_fill    (lscp_client_t *pClient, lscp_usage_t iUsageType, int iSamplerChannel);

lscp_status_t           lscp_set_channel_audio_type     (lscp_client_t *pClient, int iSamplerChannel, const char *pszAudioType);
lscp_status_t           lscp_set_channel_audio_device   (lscp_client_t *pClient, int iSamplerChannel, int iAudioDevice);
lscp_status_t           lscp_set_channel_audio_channel  (lscp_client_t *pClient, int iSamplerChannel, int iAudioOut, int iAudioIn);

lscp_status_t           lscp_set_channel_midi_type      (lscp_client_t *pClient, int iSamplerChannel, const char *pszMidiType);
lscp_status_t           lscp_set_channel_midi_device    (lscp_client_t *pClient, int iSamplerChannel, int iMidiDevice);
lscp_status_t           lscp_set_channel_midi_port      (lscp_client_t *pClient, int iSamplerChannel, int iMidiPort);
lscp_status_t           lscp_set_channel_midi_channel   (lscp_client_t *pClient, int iSamplerChannel, int iMidiChannel);
lscp_status_t           lscp_set_channel_midi_map       (lscp_client_t *pClient, int iSamplerChannel, int iMidiMap);

lscp_status_t           lscp_set_channel_volume         (lscp_client_t *pClient, int iSamplerChannel, float fVolume);

lscp_status_t           lscp_set_channel_mute           (lscp_client_t *pClient, int iSamplerChannel, int iMute);
lscp_status_t           lscp_set_channel_solo           (lscp_client_t *pClient, int iSamplerChannel, int iSolo);

lscp_status_t           lscp_reset_channel              (lscp_client_t *pClient, int iSamplerChannel);

lscp_status_t           lscp_reset_sampler              (lscp_client_t *pClient);

lscp_server_info_t *    lscp_get_server_info            (lscp_client_t *pClient);

int                     lscp_get_total_voice_count      (lscp_client_t *pClient);
int                     lscp_get_total_voice_count_max  (lscp_client_t *pClient);

float                   lscp_get_volume                 (lscp_client_t *pClient );
lscp_status_t           lscp_set_volume                 (lscp_client_t *pClient, float fVolume);

//-------------------------------------------------------------------------
// Effect sends control functions.

int                     lscp_create_fxsend              (lscp_client_t *pClient, int iSamplerChannel, int iMidiController, const char *pszFxName);
lscp_status_t           lscp_destroy_fxsend             (lscp_client_t *pClient, int iSamplerChannel, int iFxSend);

int                     lscp_get_fxsends                (lscp_client_t *pClient, int iSamplerChannel);
int *                   lscp_list_fxsends               (lscp_client_t *pClient, int iSamplerChannel);

lscp_fxsend_info_t *    lscp_get_fxsend_info            (lscp_client_t *pClient, int iSamplerChannel, int iFxSend);

lscp_status_t           lscp_set_fxsend_audio_channel   (lscp_client_t *pClient, int iSamplerChannel, int iFxSend, int iAudioSrc, int iAudioDst);
lscp_status_t           lscp_set_fxsend_midi_controller (lscp_client_t *pClient, int iSamplerChannel, int iFxSend, int iMidiController);
lscp_status_t           lscp_set_fxsend_level           (lscp_client_t *pClient, int iSamplerChannel, int iFxSend, float fLevel);

//-------------------------------------------------------------------------
// MIDI instrument mapping control functions.

int                     lscp_add_midi_instrument_map    (lscp_client_t *pClient, const char *pszMapName);
lscp_status_t           lscp_remove_midi_instrument_map (lscp_client_t *pClient, int iMidiMap);

int                     lscp_get_midi_instrument_maps   (lscp_client_t *pClient);
int *                   lscp_list_midi_instrument_maps  (lscp_client_t *pClient);

const char *            lscp_get_midi_instrument_map_name (lscp_client_t *pClient, int iMidiMap);
lscp_status_t           lscp_set_midi_instrument_map_name (lscp_client_t *pClient, int iMidiMap, const char *pszMapName);

lscp_status_t           lscp_map_midi_instrument        (lscp_client_t *pClient, lscp_midi_instrument_t *pMidiInstr, const char *pszEngineName, const char *pszFileName, int iInstrIndex, float fVolume, lscp_load_mode_t load_mode, const char *pszName);
lscp_status_t           lscp_unmap_midi_instrument      (lscp_client_t *pClient, lscp_midi_instrument_t *pMidiInstr);

int                     lscp_get_midi_instruments       (lscp_client_t *pClient, int iMidiMap);
lscp_midi_instrument_t *lscp_list_midi_instruments      (lscp_client_t *pClient, int iMidiMap);

lscp_midi_instrument_info_t *lscp_get_midi_instrument_info(lscp_client_t *pClient, lscp_midi_instrument_t *pMidiInstr);

lscp_status_t           lscp_clear_midi_instruments     (lscp_client_t *pClient, int iMidiMap);

//-------------------------------------------------------------------------
// Instrument editor functions.

lscp_status_t           lscp_edit_instrument            (lscp_client_t *pClient, int iSamplerChannel);

#if defined(__cplusplus)
}
#endif

#endif // __LSCP_CLIENT_H

// end of client.h
