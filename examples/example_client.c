// example_client.c
//
/****************************************************************************
   Copyright (C) 2004, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

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
    printf("    param_info.type                = %d (%s)\n", (int) pParamInfo->type, pszType);
    printf("    param_info.description         = %s\n", pParamInfo->description);
    printf("    param_info.mandatory           = %d\n", pParamInfo->mandatory);
    printf("    param_info.fix                 = %d\n", pParamInfo->fix);
    printf("    param_info.multiplicity        = %d\n", pParamInfo->multiplicity);
    printf("    param_info.depends             = "); client_test_szsplit(pParamInfo->depends);
    printf("    param_info.defaultv            = %s\n", pParamInfo->defaultv);
    printf("    param_info.range_min           = %s\n", pParamInfo->range_min);
    printf("    param_info.range_max           = %s\n", pParamInfo->range_max);
    printf("    param_info.possibilities       = "); client_test_szsplit(pParamInfo->possibilities);
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
    printf("    driver_info.description        = %s\n", pDriverInfo->description);
    printf("    driver_info.version            = %s\n", pDriverInfo->version);
    printf("    driver_info.parameters         = "); client_test_szsplit(pDriverInfo->parameters);
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
    printf("    device_info.driver             = %s\n", pDeviceInfo->driver);
    printf("    device_info.params             = "); client_test_params(pDeviceInfo->params);
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
    printf("    device_port_info.name          = %s\n", pDevicePortInfo->name);
    printf("    device_port_info.params        = "); client_test_params(pDevicePortInfo->params);
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
    printf("    engine_info.description        = %s\n", pEngineInfo->description);
    printf("    engine_info.version            = %s\n", pEngineInfo->version);
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
    printf("    channel_info.audio_routing     = "); client_test_szsplit(pChannelInfo->audio_routing);
    printf("    channel_info.instrument_file   = %s\n", pChannelInfo->instrument_file);
    printf("    channel_info.instrument_nr     = %d\n", pChannelInfo->instrument_nr);
    printf("    channel_info.instrument_status = %d\n", pChannelInfo->instrument_status);
    printf("    channel_info.midi_device       = %d\n", pChannelInfo->midi_device);
    printf("    channel_info.midi_port         = %d\n", pChannelInfo->midi_port);
    printf("    channel_info.midi_channel      = %d\n", pChannelInfo->midi_channel);
    printf("    channel_info.volume            = %g\n", pChannelInfo->volume);
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

////////////////////////////////////////////////////////////////////////

static int g_test_step  = 0;
static int g_test_count = 0;
static int g_test_fails = 0;


void  client_test_start   ( clock_t *pclk ) { *pclk = clock(); }
float client_test_elapsed ( clock_t *pclk ) { return (float) ((long) clock() - *pclk) / (float) CLOCKS_PER_SEC; }

typedef int *                       isplit;
typedef char **                     szsplit;
typedef lscp_status_t               status;
typedef lscp_driver_info_t *        driver_info;
typedef lscp_device_info_t *        device_info;
typedef lscp_device_port_info_t *   device_port_info;
typedef lscp_param_info_t  *        param_info;
typedef lscp_engine_info_t *        engine_info;
typedef lscp_channel_info_t *       channel_info;
typedef lscp_buffer_fill_t *        buffer_fill;

#define CLIENT_TEST(p, t, x) { clock_t c; void *v; g_test_count++; \
    printf("\n" #x ":\n"); client_test_start(&c); v = (void *) (x); \
    printf("  elapsed=%gs  errno=%d  result='%s...' ret=", \
        client_test_elapsed(&c), \
        lscp_client_get_errno(p), \
        lscp_client_get_result(p)); \
    if (client_test_##t((t)(v))) { g_test_fails++; getchar(); } \
    else if (g_test_step) getchar(); }


void client_test ( lscp_client_t *pClient )
{
    const char **ppszAudioDrivers, **ppszMidiDrivers, **ppszEngines;
    const char *pszAudioDriver, *pszMidiDriver, *pszEngine;
    int iAudioDriver, iMidiDriver, iEngine;
    int iAudioDevice, iMidiDevice;
    int iSamplerChannel;

    g_test_count = 0;
    g_test_fails = 0;

    CLIENT_TEST(pClient, szsplit, ppszAudioDrivers = lscp_get_available_audio_drivers(pClient));
    if (ppszAudioDrivers == NULL) {
        fprintf(stderr, "client_test: No audio drivers available.\n");
        return;
    }

    CLIENT_TEST(pClient, szsplit, ppszMidiDrivers = lscp_get_available_midi_drivers(pClient));
    if (ppszMidiDrivers == NULL) {
        fprintf(stderr, "client_test: No MIDI drivers available.\n");
        return;
    }

    CLIENT_TEST(pClient, szsplit, ppszEngines = lscp_get_available_engines(pClient));
    if (ppszEngines == NULL) {
        fprintf(stderr, "client_test: No engines available.\n");
        return;
    }

    CLIENT_TEST(pClient, int, lscp_get_audio_devices(pClient));
    CLIENT_TEST(pClient, isplit, lscp_list_audio_devices(pClient));

    CLIENT_TEST(pClient, int, lscp_get_midi_devices(pClient));
    CLIENT_TEST(pClient, isplit, lscp_list_midi_devices(pClient));

    for (iAudioDriver = 0; ppszAudioDrivers[iAudioDriver]; iAudioDriver++) {
     pszAudioDriver = ppszAudioDrivers[iAudioDriver];
     printf("\n--- pszAudioDriver=\"%s\" ---\n", pszAudioDriver);
     CLIENT_TEST(pClient, driver_info, lscp_get_audio_driver_info(pClient, pszAudioDriver));
     CLIENT_TEST(pClient, param_info, lscp_get_audio_driver_param_info(pClient, pszAudioDriver, "active", NULL));
     CLIENT_TEST(pClient, int, iAudioDevice = lscp_create_audio_device(pClient, pszAudioDriver, NULL));
     CLIENT_TEST(pClient, device_info, lscp_get_audio_device_info(pClient, iAudioDevice));
     for (iMidiDriver = 0; ppszMidiDrivers[iMidiDriver]; iMidiDriver++) {
      pszMidiDriver = ppszMidiDrivers[iMidiDriver];
      printf("\n--- pszMidiDriver=\"%s\" ---\n", pszMidiDriver);
      CLIENT_TEST(pClient, driver_info, lscp_get_midi_driver_info(pClient, pszMidiDriver));
      CLIENT_TEST(pClient, param_info, lscp_get_midi_driver_param_info(pClient, pszMidiDriver, "active", NULL));
      CLIENT_TEST(pClient, int, iMidiDevice = lscp_create_midi_device(pClient, pszMidiDriver, NULL));
      CLIENT_TEST(pClient, device_info, lscp_get_midi_device_info(pClient, iMidiDevice));
      for (iEngine = 0; ppszEngines[iEngine]; iEngine++) {
        pszEngine = ppszEngines[iEngine];
        printf("\n--- pszEngine=\"%s\" ---\n", pszEngine);
        CLIENT_TEST(pClient, engine_info, lscp_get_engine_info(pClient, pszEngine));
        CLIENT_TEST(pClient, int, lscp_get_channels(pClient));
        CLIENT_TEST(pClient, isplit, lscp_list_channels(pClient));
        CLIENT_TEST(pClient, int, iSamplerChannel = lscp_add_channel(pClient));
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
        CLIENT_TEST(pClient, channel_info, lscp_get_channel_info(pClient, iSamplerChannel));
        CLIENT_TEST(pClient, status, lscp_reset_channel(pClient, iSamplerChannel));
        CLIENT_TEST(pClient, status, lscp_remove_channel(pClient, iSamplerChannel));
      }
      CLIENT_TEST(pClient, status, lscp_destroy_midi_device(pClient, iMidiDevice));
     }
     CLIENT_TEST(pClient, status, lscp_destroy_audio_device(pClient, iAudioDevice));
    }
    printf("\n");
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
            client_test(pClient);
        else
        if (strcmp(szLine, "teststep") == 0 || strcmp(szLine, "test step") == 0) {
            g_test_step = 1;
            client_test(pClient);
            g_test_step = 0;
        }
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
