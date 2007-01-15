// example_client.c
//
/****************************************************************************
   liblscp - LinuxSampler Control Protocol API
   Copyright (C) 2004-2007, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#include "lscp/client.h"
#include "lscp/device.h"

#include <time.h>

#define SERVER_PORT 8888

#if defined(WIN32)
static WSADATA _wsaData;
#endif

////////////////////////////////////////////////////////////////////////

lscp_status_t client_callback ( lscp_client_t *pClient, lscp_event_t event, const char *pchData, int cchData, void *pvData )
{
	lscp_status_t ret = LSCP_FAILED;

	char *pszData = (char *) malloc(cchData + 1);
	if (pszData) {
		memcpy(pszData, pchData, cchData);
		pszData[cchData] = (char) 0;
		printf("client_callback: event=%s (0x%04x) [%s]\n", lscp_event_to_text(event), (int) event, pszData);
		free(pszData);
		ret = LSCP_OK;
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////

int client_test_int ( int i )
{
	printf("%d\n", i);
	return (i >= 0 ? 0 : 1);
}


int client_test_status ( lscp_status_t s )
{
	const char *pszStatus;

	switch (s) {
	case LSCP_OK:         pszStatus = "OK";       break;
	case LSCP_FAILED:     pszStatus = "FAILED";   break;
	case LSCP_ERROR:      pszStatus = "ERROR";    break;
	case LSCP_WARNING:    pszStatus = "WARNING";  break;
	case LSCP_TIMEOUT:    pszStatus = "TIMEOUT";  break;
	case LSCP_QUIT:       pszStatus = "QUIT";     break;
	default:              pszStatus = "NONE";     break;
	}
	printf("%s\n", pszStatus);
	return (s == LSCP_OK ? 0 : 1);
}


int client_test_isplit ( int *piSplit )
{
	int i;

	printf("{");
	for (i = 0; piSplit && piSplit[i] >= 0; i++) {
		if (i > 0)
			printf(",");
		printf(" %d", piSplit[i]);
	}
	printf(" }\n");
	return 0;
}


int client_test_szsplit ( char **ppszSplit )
{
	int i;

	printf("{");
	for (i = 0; ppszSplit && ppszSplit[i]; i++) {
		if (i > 0)
			printf(",");
		printf(" %s", ppszSplit[i]);
	}
	printf(" }\n");
	return 0;
}


int client_test_params ( lscp_param_t *pParams )
{
	int i;

	printf("{");
	for (i = 0; pParams && pParams[i].key; i++) {
		if (i > 0)
			printf(",");
		printf(" %s='%s'", pParams[i].key, pParams[i].value);
	}
	printf(" }\n");
	return 0;
}


int client_test_midi_instruments ( lscp_midi_instrument_t *pInstrs )
{
	int i;

	printf("{");
	for (i = 0; pInstrs && pInstrs[i].prog >= 0; i++) {
		if (i > 0)
			printf(",");
		printf("{%d,%d,%d}", pInstrs[i].map, pInstrs[i].bank, pInstrs[i].prog);
	}
	printf(" }\n");
	return 0;
}


int client_test_param_info ( lscp_param_info_t *pParamInfo )
{
	const char *pszType;

	if (pParamInfo == NULL) {
		printf("(nil)\n");
		return 1;
	}
	switch (pParamInfo->type) {
	case LSCP_TYPE_BOOL:      pszType = "BOOL";   break;
	case LSCP_TYPE_INT:       pszType = "INT";    break;
	case LSCP_TYPE_FLOAT:     pszType = "FLOAT";  break;
	case LSCP_TYPE_STRING:    pszType = "STRING"; break;
	default:                  pszType = "NONE";   break;
	}
	printf("{\n");
	printf("    param_info.type          = %d (%s)\n", (int) pParamInfo->type, pszType);
	printf("    param_info.description   = %s\n", pParamInfo->description);
	printf("    param_info.mandatory     = %d\n", pParamInfo->mandatory);
	printf("    param_info.fix           = %d\n", pParamInfo->fix);
	printf("    param_info.multiplicity  = %d\n", pParamInfo->multiplicity);
	printf("    param_info.depends       = "); client_test_szsplit(pParamInfo->depends);
	printf("    param_info.defaultv      = %s\n", pParamInfo->defaultv);
	printf("    param_info.range_min     = %s\n", pParamInfo->range_min);
	printf("    param_info.range_max     = %s\n", pParamInfo->range_max);
	printf("    param_info.possibilities = "); client_test_szsplit(pParamInfo->possibilities);
	printf("  }\n");
	return 0;
}


int client_test_driver_info ( lscp_driver_info_t *pDriverInfo )
{
	if (pDriverInfo == NULL) {
		printf("(nil)\n");
		return 1;
	}
	printf("{\n");
	printf("    driver_info.description = %s\n", pDriverInfo->description);
	printf("    driver_info.version     = %s\n", pDriverInfo->version);
	printf("    driver_info.parameters  = "); client_test_szsplit(pDriverInfo->parameters);
	printf("  }\n");
	return 0;
}


int client_test_device_info ( lscp_device_info_t *pDeviceInfo )
{
	if (pDeviceInfo == NULL) {
		printf("(nil)\n");
		return 1;
	}
	printf("{\n");
	printf("    device_info.driver = %s\n", pDeviceInfo->driver);
	printf("    device_info.params = "); client_test_params(pDeviceInfo->params);
	printf("  }\n");
	return 0;
}


int client_test_device_port_info ( lscp_device_port_info_t *pDevicePortInfo )
{
	if (pDevicePortInfo == NULL) {
		printf("(nil)\n");
		return 1;
	}
	printf("{\n");
	printf("    device_port_info.name   = %s\n", pDevicePortInfo->name);
	printf("    device_port_info.params = "); client_test_params(pDevicePortInfo->params);
	printf("  }\n");
	return 0;
}


int client_test_server_info ( lscp_server_info_t *pServerInfo )
{
	if (pServerInfo == NULL) {
		printf("(nil)\n");
		return 1;
	}
	printf("{\n");
	printf("    server_info.description      = %s\n", pServerInfo->description);
	printf("    server_info.version          = %s\n", pServerInfo->version);
	printf("    server_info.protocol_version = %s\n", pServerInfo->protocol_version);
	printf("  }\n");
	return 0;
}


int client_test_engine_info ( lscp_engine_info_t *pEngineInfo )
{
	if (pEngineInfo == NULL) {
		printf("(nil)\n");
		return 1;
	}
	printf("{\n");
	printf("    engine_info.description = %s\n", pEngineInfo->description);
	printf("    engine_info.version     = %s\n", pEngineInfo->version);
	printf("  }\n");
	return 0;
}


int client_test_channel_info ( lscp_channel_info_t *pChannelInfo )
{
	if (pChannelInfo == NULL) {
		printf("(nil)\n");
		return 1;
	}
	printf("{\n");
	printf("    channel_info.engine_name       = %s\n", pChannelInfo->engine_name);
	printf("    channel_info.audio_device      = %d\n", pChannelInfo->audio_device);
	printf("    channel_info.audio_channels    = %d\n", pChannelInfo->audio_channels);
	printf("    channel_info.audio_routing     = "); client_test_isplit(pChannelInfo->audio_routing);
	printf("    channel_info.instrument_file   = %s\n", pChannelInfo->instrument_file);
	printf("    channel_info.instrument_nr     = %d\n", pChannelInfo->instrument_nr);
	printf("    channel_info.instrument_name   = %s\n", pChannelInfo->instrument_name);
	printf("    channel_info.instrument_status = %d\n", pChannelInfo->instrument_status);
	printf("    channel_info.midi_device       = %d\n", pChannelInfo->midi_device);
	printf("    channel_info.midi_port         = %d\n", pChannelInfo->midi_port);
	printf("    channel_info.midi_channel      = %d\n", pChannelInfo->midi_channel);
	printf("    channel_info.midi_map          = %d\n", pChannelInfo->midi_map);
	printf("    channel_info.volume            = %g\n", pChannelInfo->volume);
	printf("    channel_info.mute              = %d\n", pChannelInfo->mute);
	printf("    channel_info.solo              = %d\n", pChannelInfo->solo);
	printf("  }\n");
	return 0;
}


int client_test_fxsend_info ( lscp_fxsend_info_t *pFxSendInfo )
{
	if (pFxSendInfo == NULL) {
		printf("(nil)\n");
		return 1;
	}
	printf("{\n");
	printf("    fxsend_info.engine_name     = %s\n", pFxSendInfo->name);
	printf("    fxsend_info.midi_controller = %d\n", pFxSendInfo->midi_controller);
	printf("    fxsend_info.audio_routing   = "); client_test_isplit(pFxSendInfo->audio_routing);
	printf("    fxsend_info.level           = %g\n", pFxSendInfo->level);
	printf("  }\n");
	return 0;
}


int client_test_buffer_fill ( lscp_buffer_fill_t *pBufferFill )
{
	if (pBufferFill == NULL) {
		printf("(nil)\n");
		return 1;
	}
	printf("{ <%p> }\n", pBufferFill);
	return 0;
}


int client_test_load_mode ( lscp_load_mode_t load_mode )
{
	const char *pszLoadMode;

	switch (load_mode) {
	case LSCP_LOAD_ON_DEMAND:      pszLoadMode = "ON_DEMAND";      break;
	case LSCP_LOAD_ON_DEMAND_HOLD: pszLoadMode = "ON_DEMAND_HOLD"; break;
	case LSCP_LOAD_PERSISTENT:     pszLoadMode = "PERSISTENT";     break;
	default:                       pszLoadMode = "DEFAULT";        break;
	}
	printf("%s\n", pszLoadMode);
	return 0;
}


int client_test_midi_instrument_info ( lscp_midi_instrument_info_t *pInstrInfo )
{
	if (pInstrInfo == NULL) {
		printf("(nil)\n");
		return 1;
	}
	printf("{\n");
	printf("    midi_instrument_info.name            = %s\n", pInstrInfo->name);
	printf("    midi_instrument_info.engine_name     = %s\n", pInstrInfo->engine_name);
	printf("    midi_instrument_info.instrument_file = %s\n", pInstrInfo->instrument_file);
	printf("    midi_instrument_info.instrument_nr   = %d\n", pInstrInfo->instrument_nr);
	printf("    midi_instrument_info.instrument_name = %s\n", pInstrInfo->instrument_name);
	printf("    midi_instrument_info.load_mode       = "); client_test_load_mode(pInstrInfo->load_mode);
	printf("    midi_instrument_info.volume          = %g\n", pInstrInfo->volume);
	printf("  }\n");
	return 0;
}


////////////////////////////////////////////////////////////////////////

static int g_test_step  = 0;
static int g_test_count = 0;
static int g_test_fails = 0;


void  client_test_start   ( clock_t *pclk ) { *pclk = clock(); }
float client_test_elapsed ( clock_t *pclk ) { return (float) ((long) clock() - *pclk) / (float) CLOCKS_PER_SEC; }

typedef int *                        isplit;
typedef char **                      szsplit;
typedef lscp_status_t                status;
typedef lscp_driver_info_t *         driver_info;
typedef lscp_device_info_t *         device_info;
typedef lscp_device_port_info_t *    device_port_info;
typedef lscp_param_info_t  *         param_info;
typedef lscp_server_info_t *         server_info;
typedef lscp_engine_info_t *         engine_info;
typedef lscp_channel_info_t *        channel_info;
typedef lscp_buffer_fill_t *         buffer_fill;
typedef lscp_fxsend_info_t *         fxsend_info;
typedef lscp_midi_instrument_t *     midi_instruments;
typedef lscp_midi_instrument_info_t *midi_instrument_info;

#define CLIENT_TEST(p, t, x) { clock_t c; void *v; g_test_count++; \
	printf("\n" #x ":\n"); client_test_start(&c); v = (void *) (x); \
	printf("  elapsed=%gs  errno=%d  result='%s...' ret=", \
		client_test_elapsed(&c), \
		lscp_client_get_errno(p), \
		lscp_client_get_result(p)); \
	if (client_test_##t((t)(v))) { g_test_fails++; getchar(); } \
	else if (g_test_step) getchar(); }


void client_test_engine ( lscp_client_t *pClient, const char *pszEngine, const char *pszAudioDriver, int iAudioDevice, const char *pszMidiDriver, int iMidiDevice )
{
	int iSamplerChannel;
	int iFxSend;

	printf("\n--- pszEngine=\"%s\" pszAudioDevice=\"%s\" iAudioDevice=%d pszMidiDevice=\"%s\" iMidiDevice=%d ---\n", pszEngine, pszAudioDriver, iAudioDevice, pszMidiDriver, iMidiDevice);
	CLIENT_TEST(pClient, engine_info, lscp_get_engine_info(pClient, pszEngine));
	CLIENT_TEST(pClient, int, lscp_get_total_voice_count(pClient));
	CLIENT_TEST(pClient, int, lscp_get_total_voice_count_max(pClient));
	CLIENT_TEST(pClient, int, lscp_get_channels(pClient));
	CLIENT_TEST(pClient, isplit, lscp_list_channels(pClient));
	CLIENT_TEST(pClient, int, iSamplerChannel = lscp_add_channel(pClient));
	printf("\n--- iSamplerChannel=%d ---\n", iSamplerChannel);
	CLIENT_TEST(pClient, channel_info, lscp_get_channel_info(pClient, iSamplerChannel));
	CLIENT_TEST(pClient, status, lscp_load_engine(pClient, pszEngine, iSamplerChannel));
	CLIENT_TEST(pClient, status, lscp_load_instrument(pClient, "DefaultInstrument.gig", 0, iSamplerChannel));
	CLIENT_TEST(pClient, int, lscp_get_channel_voice_count(pClient, iSamplerChannel));
	CLIENT_TEST(pClient, int, lscp_get_channel_stream_count(pClient, iSamplerChannel));
	CLIENT_TEST(pClient, int, lscp_get_channel_stream_usage(pClient, iSamplerChannel));
	CLIENT_TEST(pClient, buffer_fill, lscp_get_channel_buffer_fill(pClient, LSCP_USAGE_BYTES, iSamplerChannel));
	CLIENT_TEST(pClient, buffer_fill, lscp_get_channel_buffer_fill(pClient, LSCP_USAGE_PERCENTAGE, iSamplerChannel));
	CLIENT_TEST(pClient, status, lscp_set_channel_audio_type(pClient, iSamplerChannel, pszAudioDriver));
	CLIENT_TEST(pClient, status, lscp_set_channel_audio_device(pClient, iSamplerChannel, 0));
	CLIENT_TEST(pClient, status, lscp_set_channel_audio_channel(pClient, iSamplerChannel, 0, 1));
	CLIENT_TEST(pClient, status, lscp_set_channel_midi_type(pClient, iSamplerChannel, pszMidiDriver));
	CLIENT_TEST(pClient, status, lscp_set_channel_midi_device(pClient, iSamplerChannel, 0));
	CLIENT_TEST(pClient, status, lscp_set_channel_midi_channel(pClient, iSamplerChannel, 0));
	CLIENT_TEST(pClient, status, lscp_set_channel_midi_port(pClient, iSamplerChannel, 0));
	CLIENT_TEST(pClient, status, lscp_set_channel_volume(pClient, iSamplerChannel, 0.5));
	CLIENT_TEST(pClient, status, lscp_set_channel_mute(pClient, iSamplerChannel, 1));
	CLIENT_TEST(pClient, status, lscp_set_channel_solo(pClient, iSamplerChannel, 1));
	CLIENT_TEST(pClient, int, iFxSend = lscp_create_fxsend(pClient, iSamplerChannel, 90, "DummyFxSend"));
	CLIENT_TEST(pClient, int, lscp_get_fxsends(pClient, iSamplerChannel));
	CLIENT_TEST(pClient, isplit, lscp_list_fxsends(pClient, iSamplerChannel));
	CLIENT_TEST(pClient, fxsend_info, lscp_get_fxsend_info(pClient, iSamplerChannel, iFxSend));
	CLIENT_TEST(pClient, status, lscp_set_fxsend_midi_controller(pClient, iSamplerChannel, iFxSend, 99));
	CLIENT_TEST(pClient, status, lscp_set_fxsend_audio_channel(pClient, iSamplerChannel, iFxSend, 0, 1));
	CLIENT_TEST(pClient, status, lscp_set_fxsend_level(pClient, iSamplerChannel, iFxSend, 0.12f));
	CLIENT_TEST(pClient, status, lscp_destroy_fxsend(pClient, iSamplerChannel, iFxSend));
	CLIENT_TEST(pClient, channel_info, lscp_get_channel_info(pClient, iSamplerChannel));
	CLIENT_TEST(pClient, status, lscp_reset_channel(pClient, iSamplerChannel));
	CLIENT_TEST(pClient, status, lscp_remove_channel(pClient, iSamplerChannel));
}


void client_test_midi_port ( lscp_client_t *pClient, int iMidiDevice, int iMidiPort )
{
	lscp_device_port_info_t *pMidiPortInfo;
	const char *pszParam;
	int i;

	printf("\n--- iMidiDevice=%d iMidiPort=%d ---\n", iMidiDevice, iMidiPort);
	CLIENT_TEST(pClient, device_port_info, pMidiPortInfo = lscp_get_midi_port_info(pClient, iMidiDevice, iMidiPort));
	if (pMidiPortInfo && pMidiPortInfo->params) {
		for (i = 0; pMidiPortInfo->params[i].key; i++) {
			pszParam = pMidiPortInfo->params[i].key;
			printf("\n--- iMidiDevice=%d iMidiPort=%d pszParam=\"%s\" ---\n", iMidiDevice, iMidiPort, pszParam);
			CLIENT_TEST(pClient, param_info, lscp_get_midi_port_param_info(pClient, iMidiDevice, iMidiPort, pszParam));
		}
	}
}


void client_test_audio_channel ( lscp_client_t *pClient, int iAudioDevice, int iAudioChannel )
{
	lscp_device_port_info_t *pAudioChannelInfo;
	const char *pszParam;
	int i;

	printf("\n--- iAudioDevice=%d iAudioChannel=%d ---\n", iAudioDevice, iAudioChannel);
	CLIENT_TEST(pClient, device_port_info, pAudioChannelInfo = lscp_get_audio_channel_info(pClient, iAudioDevice, iAudioChannel));
	if (pAudioChannelInfo && pAudioChannelInfo->params) {
		for (i = 0; pAudioChannelInfo->params[i].key; i++) {
			pszParam = pAudioChannelInfo->params[i].key;
			printf("\n--- iAudioDevice=%d iAudioChannel=%d pszParam=\"%s\" ---\n", iAudioDevice, iAudioChannel, pszParam);
			CLIENT_TEST(pClient, param_info, lscp_get_audio_channel_param_info(pClient, iAudioDevice, iAudioChannel, pszParam));
		}
	}
}


void client_test_midi_device ( lscp_client_t *pClient, int iMidiDevice )
{
	lscp_device_info_t *pDeviceInfo;
	const char *pszValue;
	int iMidiPort, iMidiPorts;

	printf("\n--- iMidiDevice=%d ---\n", iMidiDevice);
	CLIENT_TEST(pClient, device_info, pDeviceInfo = lscp_get_midi_device_info(pClient, iMidiDevice));
	if (pDeviceInfo && pDeviceInfo->params) {
		pszValue = lscp_get_param_value(pDeviceInfo->params, "ports");
		if (pszValue) {
			iMidiPorts = atoi(pszValue);
			for (iMidiPort = 0; iMidiPort < iMidiPorts; iMidiPort++)
				client_test_midi_port(pClient, iMidiDevice, iMidiPort);
		}
	}
}


void client_test_audio_device ( lscp_client_t *pClient, int iAudioDevice )
{
	lscp_device_info_t *pDeviceInfo;
	const char *pszValue;
	int iAudioChannel, iAudioChannels;

	printf("\n--- iAudioDevice=%d ---\n", iAudioDevice);
	CLIENT_TEST(pClient, device_info, pDeviceInfo = lscp_get_audio_device_info(pClient, iAudioDevice));
	if (pDeviceInfo && pDeviceInfo->params) {
		pszValue = lscp_get_param_value(pDeviceInfo->params, "channels");
		if (pszValue) {
			iAudioChannels = atoi(pszValue);
			for (iAudioChannel = 0; iAudioChannel < iAudioChannels; iAudioChannel++)
				client_test_audio_channel(pClient, iAudioDevice, iAudioChannel);
		}
	}
}


void client_test_midi_driver ( lscp_client_t *pClient, const char *pszMidiDriver )
{
	lscp_driver_info_t *pDriverInfo;
	const char *pszParam;
	int i;

	printf("\n--- pszMidiDriver=\"%s\" ---\n", pszMidiDriver);
	CLIENT_TEST(pClient, driver_info, pDriverInfo = lscp_get_midi_driver_info(pClient, pszMidiDriver));
	if (pDriverInfo && pDriverInfo->parameters) {
		for (i = 0; pDriverInfo->parameters[i]; i++) {
			pszParam = pDriverInfo->parameters[i];
			printf("\n--- pszMidiDriver=\"%s\" pszParam=\"%s\" ---\n", pszMidiDriver, pszParam);
			CLIENT_TEST(pClient, param_info, lscp_get_midi_driver_param_info(pClient, pszMidiDriver, pszParam, NULL));
		}
	}
}


void client_test_audio_driver ( lscp_client_t *pClient, const char *pszAudioDriver )
{
	lscp_driver_info_t *pDriverInfo;
	const char *pszParam;
	int i;

	printf("\n--- pszAudioDriver=\"%s\" ---\n", pszAudioDriver);
	CLIENT_TEST(pClient, driver_info, pDriverInfo = lscp_get_audio_driver_info(pClient, pszAudioDriver));
	if (pDriverInfo && pDriverInfo->parameters) {
		for (i = 0; pDriverInfo->parameters[i]; i++) {
			pszParam = pDriverInfo->parameters[i];
			printf("\n--- pszAudioDriver=\"%s\" pszParam=\"%s\" ---\n", pszAudioDriver, pszParam);
			CLIENT_TEST(pClient, param_info, lscp_get_audio_driver_param_info(pClient, pszAudioDriver, pszParam, NULL));
		}
	}
}


void client_test_all ( lscp_client_t *pClient, int step )
{
	const char **ppszAudioDrivers, **ppszMidiDrivers, **ppszEngines;
	const char *pszAudioDriver, *pszMidiDriver, *pszEngine;
	int iAudioDriver, iMidiDriver, iEngine;
	int iAudio, iAudioDevice, iMidi, iMidiDevice;
	int iNewAudioDevice, iNewMidiDevice;
	int *piAudioDevices, *piMidiDevices;
	lscp_midi_instrument_t midi_instr;
	int i, j, k;

	g_test_step  = step;
	g_test_count = 0;
	g_test_fails = 0;

	CLIENT_TEST(pClient, server_info, lscp_get_server_info(pClient));

	CLIENT_TEST(pClient, int, lscp_get_available_audio_drivers(pClient));
	CLIENT_TEST(pClient, szsplit, ppszAudioDrivers = lscp_list_available_audio_drivers(pClient));
	if (ppszAudioDrivers == NULL) {
		fprintf(stderr, "client_test: No audio drivers available.\n");
		return;
	}

	CLIENT_TEST(pClient, int, lscp_get_available_midi_drivers(pClient));
	CLIENT_TEST(pClient, szsplit, ppszMidiDrivers = lscp_list_available_midi_drivers(pClient));
	if (ppszMidiDrivers == NULL) {
		fprintf(stderr, "client_test: No MIDI drivers available.\n");
		return;
	}

	CLIENT_TEST(pClient, int, lscp_get_available_engines(pClient));
	CLIENT_TEST(pClient, szsplit, ppszEngines = lscp_list_available_engines(pClient));
	if (ppszEngines == NULL) {
		fprintf(stderr, "client_test: No engines available.\n");
		return;
	}

	for (iAudioDriver = 0; ppszAudioDrivers[iAudioDriver]; iAudioDriver++) {
		pszAudioDriver = ppszAudioDrivers[iAudioDriver];
		client_test_audio_driver(pClient, pszAudioDriver);
		CLIENT_TEST(pClient, int, iNewAudioDevice = lscp_create_audio_device(pClient, pszAudioDriver, NULL));
		CLIENT_TEST(pClient, int, lscp_get_audio_devices(pClient));
		CLIENT_TEST(pClient, isplit, piAudioDevices = lscp_list_audio_devices(pClient));
		for (iAudio = 0; piAudioDevices && piAudioDevices[iAudio] >= 0; iAudio++) {
			iAudioDevice = piAudioDevices[iAudio];
			client_test_audio_device(pClient, iAudioDevice);
			for (iMidiDriver = 0; ppszMidiDrivers[iMidiDriver]; iMidiDriver++) {
				pszMidiDriver = ppszMidiDrivers[iMidiDriver];
				client_test_midi_driver(pClient, pszMidiDriver);
				CLIENT_TEST(pClient, int, iNewMidiDevice = lscp_create_midi_device(pClient, pszMidiDriver, NULL));
				CLIENT_TEST(pClient, int, lscp_get_midi_devices(pClient));
				CLIENT_TEST(pClient, isplit, piMidiDevices = lscp_list_midi_devices(pClient));
				for (iMidi = 0; piMidiDevices && piMidiDevices[iMidi] >= 0; iMidi++) {
					iMidiDevice = piMidiDevices[iMidi];
					client_test_midi_device(pClient, iMidiDevice);
					for (iEngine = 0; ppszEngines[iEngine]; iEngine++) {
						pszEngine = ppszEngines[iEngine];
						client_test_engine(pClient, pszEngine, pszAudioDriver, iAudioDevice, pszMidiDriver, iMidiDevice);
					}
				}
				CLIENT_TEST(pClient, status, lscp_destroy_midi_device(pClient, iNewMidiDevice));
			}
		}
		CLIENT_TEST(pClient, status, lscp_destroy_audio_device(pClient, iNewAudioDevice));
	}

	for (i = 0; i < 2; i++) {
		CLIENT_TEST(pClient, int, lscp_add_midi_instrument_map(pClient, NULL));
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 8; k++) {
				midi_instr.map  = i;
				midi_instr.bank = j;
				midi_instr.prog = k;
				CLIENT_TEST(pClient, status, lscp_map_midi_instrument(pClient, &midi_instr, pszEngine, "DefaultInstrument.gig", 0, 1.0f, LSCP_LOAD_ON_DEMAND, "DummyName"));
			}
		}
		CLIENT_TEST(pClient, status, lscp_set_midi_instrument_map_name(pClient, i, "DummyMapName"));
	}
	CLIENT_TEST(pClient, int, lscp_get_midi_instruments(pClient, LSCP_MIDI_MAP_ALL));
	CLIENT_TEST(pClient, midi_instruments, lscp_list_midi_instruments(pClient, LSCP_MIDI_MAP_ALL));
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 8; k++) {
				midi_instr.map  = i;
				midi_instr.bank = j;
				midi_instr.prog = k;
				CLIENT_TEST(pClient, midi_instrument_info, lscp_get_midi_instrument_info(pClient, &midi_instr));
				CLIENT_TEST(pClient, status, lscp_unmap_midi_instrument(pClient, &midi_instr));
			}
		}
		CLIENT_TEST(pClient, int, lscp_remove_midi_instrument_map(pClient, i));
	}	
	CLIENT_TEST(pClient, status, lscp_clear_midi_instruments(pClient, LSCP_MIDI_MAP_ALL));
	
	CLIENT_TEST(pClient, status, lscp_set_volume(pClient, 0.123f));
	CLIENT_TEST(pClient, int, (int) (100.0f * lscp_get_volume(pClient)));

	CLIENT_TEST(pClient, status, lscp_reset_sampler(pClient));
	
	printf("\n\n");
	printf("  Total: %d tests, %d failed.\n\n", g_test_count, g_test_fails);
}


////////////////////////////////////////////////////////////////////////

void client_usage (void)
{
	printf("\n  %s %s (Build: %s)\n", lscp_client_package(), lscp_client_version(), lscp_client_build());

	fputs("\n  Available commands: help, test[step], exit, quit, subscribe, unsubscribe", stdout);
	fputs("\n  (all else are sent verbatim to server)\n\n", stdout);

}

void client_prompt (void)
{
	fputs("lscp_client> ", stdout);
}

int main (int argc, char *argv[] )
{
	lscp_client_t *pClient;
	char *pszHost = "localhost";
	char  szLine[1024];
	int  cchLine;
	lscp_status_t ret;

#if defined(WIN32)
	if (WSAStartup(MAKEWORD(1, 1), &_wsaData) != 0) {
		fprintf(stderr, "lscp_client: WSAStartup failed.\n");
		return -1;
	}
#endif

	if (argc > 1)
		pszHost = argv[1];

	pClient = lscp_client_create(pszHost, SERVER_PORT, client_callback, NULL);
	if (pClient == NULL)
		return -1;

	client_usage();
	client_prompt();

	while (fgets(szLine, sizeof(szLine) - 3, stdin)) {

		cchLine = strlen(szLine);
		while (cchLine > 0 && (szLine[cchLine - 1] == '\n' || szLine[cchLine - 1] == '\r'))
			cchLine--;
		szLine[cchLine] = '\0';

		if (strcmp(szLine, "exit") == 0 || strcmp(szLine, "quit") == 0)
			break;
		else
		if (strcmp(szLine, "subscribe") == 0)
			lscp_client_subscribe(pClient, LSCP_EVENT_MISCELLANEOUS);
		else
		if (strcmp(szLine, "unsubscribe") == 0)
			lscp_client_unsubscribe(pClient, LSCP_EVENT_MISCELLANEOUS);
		else
		if (strcmp(szLine, "test") == 0)
			client_test_all(pClient, 0);
		else
		if (strcmp(szLine, "teststep") == 0 || strcmp(szLine, "test step") == 0)
			client_test_all(pClient, 1);
		else
		if (cchLine > 0 && strcmp(szLine, "help") != 0) {
			szLine[cchLine++] = '\r';
			szLine[cchLine++] = '\n';
			szLine[cchLine]   = '\0';
			ret = lscp_client_query(pClient, szLine);
			printf("%s\n(errno = %d)\n", lscp_client_get_result(pClient), lscp_client_get_errno(pClient));
			if (ret == LSCP_QUIT)
				break;
		}
		else client_usage();

		client_prompt();
	}

	lscp_client_destroy(pClient);

#if defined(WIN32)
	WSACleanup();
#endif

	return 0;
}

// end of example_client.c
