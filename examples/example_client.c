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

void  client_test_start   ( clock_t *pclk ) { *pclk = clock(); }
float client_test_elapsed ( clock_t *pclk ) { return (float) ((long) clock() - *pclk) / (float) CLOCKS_PER_SEC; }

#define CLIENT_TEST(p, x) { clock_t c; void *v;\
    printf(#x ":\n"); client_test_start(&c); v = (void *) (x); \
    printf("  elapsed=%gs\n", client_test_elapsed(&c)); \
    printf("  ret=%p (%d)\n", v, (int) v); \
    printf("  errno=%d\n", lscp_client_get_errno(p)); \
    printf("  result=\"%s\"\n", lscp_client_get_result(p)); }

void client_test ( lscp_client_t *pClient )
{
    const char **ppszAudioDrivers, **ppszMidiDrivers, **ppszEngines;
    const char *pszAudioDriver, *pszMidiDriver, *pszEngine;
    int iAudioDriver, iMidiDriver, iEngine;
    int iSamplerChannel;

    CLIENT_TEST(pClient, ppszAudioDrivers = lscp_get_available_audio_drivers(pClient));
    if (ppszAudioDrivers == NULL) {
        fprintf(stderr, "client_test: No audio drivers available.\n");
        return;
    }

    CLIENT_TEST(pClient, ppszMidiDrivers = lscp_get_available_midi_drivers(pClient));
    if (ppszMidiDrivers == NULL) {
        fprintf(stderr, "client_test: No MIDI drivers available.\n");
        return;
    }

    CLIENT_TEST(pClient, ppszEngines = lscp_get_available_engines(pClient));
    if (ppszEngines == NULL) {
        fprintf(stderr, "client_test: No engines available.\n");
        return;
    }

    CLIENT_TEST(pClient, lscp_get_audio_devices(pClient));
    CLIENT_TEST(pClient, lscp_list_audio_devices(pClient));

    CLIENT_TEST(pClient, lscp_get_midi_devices(pClient));
    CLIENT_TEST(pClient, lscp_list_midi_devices(pClient));

    for (iAudioDriver = 0; ppszAudioDrivers[iAudioDriver]; iAudioDriver++) {
     pszAudioDriver = ppszAudioDrivers[iAudioDriver];
     printf("\n--- pszAudioDriver=\"%s\" ---\n", pszAudioDriver);
     CLIENT_TEST(pClient, lscp_get_audio_driver_info(pClient, pszAudioDriver));
     for (iMidiDriver = 0; ppszMidiDrivers[iMidiDriver]; iMidiDriver++) {
      pszMidiDriver = ppszMidiDrivers[iMidiDriver];
      printf("\n--- pszMidiDriver=\"%s\" ---\n", pszMidiDriver);
      CLIENT_TEST(pClient, lscp_get_midi_driver_info(pClient, pszMidiDriver));
      for (iEngine = 0; ppszEngines[iEngine]; iEngine++) {
        pszEngine = ppszEngines[iEngine];
        printf("\n--- pszEngine=\"%s\" ---\n", pszEngine);
        CLIENT_TEST(pClient, lscp_get_engine_info(pClient, pszEngine));
        CLIENT_TEST(pClient, lscp_get_channels(pClient));
        CLIENT_TEST(pClient, lscp_list_channels(pClient));
        CLIENT_TEST(pClient, iSamplerChannel = lscp_add_channel(pClient));
        printf(">>> iSamplerChannel=\"%d\"\n", iSamplerChannel);
        CLIENT_TEST(pClient, lscp_get_channel_info(pClient, iSamplerChannel));
        CLIENT_TEST(pClient, lscp_load_engine(pClient, pszEngine, iSamplerChannel));
        CLIENT_TEST(pClient, lscp_load_instrument(pClient, "DefaultInstrument.gig", 0, iSamplerChannel));
        CLIENT_TEST(pClient, lscp_get_channel_voice_count(pClient, iSamplerChannel));
        CLIENT_TEST(pClient, lscp_get_channel_stream_count(pClient, iSamplerChannel));
        CLIENT_TEST(pClient, lscp_get_channel_buffer_fill(pClient, LSCP_USAGE_BYTES, iSamplerChannel));
        CLIENT_TEST(pClient, lscp_get_channel_buffer_fill(pClient, LSCP_USAGE_PERCENTAGE, iSamplerChannel));
        CLIENT_TEST(pClient, lscp_set_channel_audio_type(pClient, iSamplerChannel, pszAudioDriver));
        CLIENT_TEST(pClient, lscp_set_channel_audio_device(pClient, iSamplerChannel, 0));
        CLIENT_TEST(pClient, lscp_set_channel_audio_channel(pClient, iSamplerChannel, 0, 1));
        CLIENT_TEST(pClient, lscp_set_channel_midi_type(pClient, iSamplerChannel, pszMidiDriver));
        CLIENT_TEST(pClient, lscp_set_channel_midi_device(pClient, iSamplerChannel, 0));
        CLIENT_TEST(pClient, lscp_set_channel_midi_channel(pClient, iSamplerChannel, 0));
        CLIENT_TEST(pClient, lscp_set_channel_midi_port(pClient, iSamplerChannel, 0));
        CLIENT_TEST(pClient, lscp_set_channel_volume(pClient, iSamplerChannel, 0.5));
        CLIENT_TEST(pClient, lscp_get_channel_info(pClient, iSamplerChannel));
        CLIENT_TEST(pClient, lscp_reset_channel(pClient, iSamplerChannel));
        CLIENT_TEST(pClient, lscp_remove_channel(pClient, iSamplerChannel));
        printf("\n");
      }
     }
    }

}

////////////////////////////////////////////////////////////////////////

void client_usage (void)
{
    printf("\n  %s %s (Build: %s)\n", lscp_client_package(), lscp_client_version(), lscp_client_build());

    fputs("\n  Available client commands: help, test, exit, quit, subscribe, unsubscribe", stdout);
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
