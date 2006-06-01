// example_server.c
//
/****************************************************************************
   Copyright (C) 2004-2006, rncbc aka Rui Nuno Capela. All rights reserved.

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

#include "server.h"
#include "parser.h"

#include <time.h>

#define SERVER_PORT 8888

#if defined(WIN32)
static WSADATA _wsaData;
#endif

////////////////////////////////////////////////////////////////////////

lscp_status_t server_callback ( lscp_connect_t *pConnect, const char *pchBuffer, int cchBuffer, void *pvData )
{
    lscp_status_t ret = LSCP_OK;
    lscp_parser_t tok;
    const char *pszResult = NULL;
    char szTemp[256];
    int i;
    static int iSamplerChannel = 0;
    static int iAudioDevice = 0;
    static int iMidiDevice  = 0;

    if (pchBuffer == NULL) {
        fprintf(stderr, "server_callback: addr=%s port=%d: ",
            inet_ntoa(pConnect->client.addr.sin_addr),
            htons(pConnect->client.addr.sin_port));
        switch (cchBuffer) {
          case LSCP_CONNECT_OPEN:
            fprintf(stderr, "New client connection.\n");
            break;
          case LSCP_CONNECT_CLOSE:
            fprintf(stderr, "Connection closed.\n");
            break;
        }
        return ret;
    }

    lscp_socket_trace("server_callback", &(pConnect->client.addr), pchBuffer, cchBuffer);

    lscp_parser_init(&tok, pchBuffer, cchBuffer);

    if (lscp_parser_test(&tok, "GET")) {
        if (lscp_parser_test(&tok, "CHANNEL")) {
            if (lscp_parser_test(&tok, "INFO")) {
                // Getting sampler channel informations:
                // GET CHANNEL INFO <sampler-channel>
                pszResult = "ENGINE_NAME: DummyEngine\r\n"
                            "INSTRUMENT_FILE: DummyInstrument.gig\r\n"
                            "INSTRUMENT_NR: 0\r\n"
                            "INSTRUMENT_NAME: Dummy Instrument\r\n"
                            "INSTRUMENT_STATUS: 100\r\n"
                            "AUDIO_OUTPUT_DEVICE: 0\r\n"
                            "AUDIO_OUTPUT_CHANNELS: 2\r\n"
                            "AUDIO_OUTPUT_ROUTING: 0,1\r\n"
                            "MIDI_INPUT_DEVICE: 0\r\n"
                            "MIDI_INPUT_PORT: 0\r\n"
                            "MIDI_INPUT_CHANNEL: ALL\r\n"
                            "VOLUME: 0.5\r\n"
                            "MUTE: FALSE\r\n"
                            "SOLO: TRUE\r\n";
            }
            else if (lscp_parser_test(&tok, "VOICE_COUNT")) {
                // Current number of active voices:
                // GET CHANNEL VOICE_COUNT <sampler-channel>
                sprintf(szTemp, "%d", rand() % 100);
                pszResult = szTemp;
            }
            else if (lscp_parser_test(&tok, "STREAM_COUNT")) {
                // Current number of active disk streams:
                // GET CHANNEL STREAM_COUNT <sampler-channel>
                pszResult = "3\r\n";
            }
            else if (lscp_parser_test(&tok, "BUFFER_FILL")) {
                if (lscp_parser_test(&tok, "BYTES")) {
                    // Current fill state of disk stream buffers:
                    // GET CHANNEL BUFFER_FILL BYTES <sampler-channel>
                    sprintf(szTemp, "[1]%d,[2]%d,[3]%d\r\n", rand(), rand(), rand());
                    pszResult = szTemp;
                }
                else if (lscp_parser_test(&tok, "PERCENTAGE")) {
                    // Current fill state of disk stream buffers:
                    // GET CHANNEL BUFFER_FILL PERCENTAGE <sampler-channel>
                    sprintf(szTemp, "[1]%d%%,[2]%d%%,[3]%d%%\r\n", rand() % 100, rand() % 100, rand() % 100);
                    pszResult = szTemp;
                }
                else ret = LSCP_FAILED;
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test(&tok, "CHANNELS")) {
            // Current number of sampler channels:
            // GET CHANNELS
            sprintf(szTemp, "%d", iSamplerChannel);
            pszResult = szTemp;
        }
        else if (lscp_parser_test(&tok, "AVAILABLE_AUDIO_OUTPUT_DRIVERS")) {
            // Getting all available audio output driver count.
            // GET AVAILABLE_AUDIO_OUTPUT_DRIVERS
            pszResult = "2\r\n";
        }
        else if (lscp_parser_test(&tok, "AVAILABLE_MIDI_INPUT_DRIVERS")) {
            // Getting all available MIDI input driver count.
            // GET AVAILABLE_MIDI_INPUT_DRIVERS
            pszResult = "1\r\n";
        }
        else if (lscp_parser_test2(&tok, "AUDIO_OUTPUT_DRIVER", "INFO")) {
            // Getting informations about a specific audio output driver.
            // GET AUDIO_OUTPUT_DRIVER INFO <audio-output-type>
            if (lscp_parser_test(&tok, "Alsa")) {
                pszResult = "DESCRIPTION: 'ALSA PCM'\r\n"
                            "VERSION: '1.0'\r\n"
                            "PARAMETERS: channels,samplerate,active\r\n";
            }
            else if (lscp_parser_test(&tok, "Jack")) {
                pszResult = "DESCRIPTION: JACK Audio Connection Kit\r\n"
                            "VERSION: 0.98.1\r\n"
                            "PARAMETERS: channels,samplerate,active\r\n";
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test2(&tok, "MIDI_INPUT_DRIVER", "INFO")) {
            // Getting informations about a specific MIDI input driver.
            // GET MIDI_INPUT_DRIVER INFO <midi-input-type>
            if (lscp_parser_test(&tok, "Alsa")) {
                pszResult = "DESCRIPTION: ALSA Sequencer\r\n"
                            "VERSION: 1.0\r\n"
                            "PARAMETERS: ports,active\r\n";
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test2(&tok, "AUDIO_OUTPUT_DRIVER_PARAMETER", "INFO")) {
            // Getting informations about a specific audio output driver parameter.
            // GET AUDIO_OUTPUT_DRIVER_PARAMETER INFO <audio-output-type> <param>
            if (lscp_parser_test(&tok, "Alsa")) {
                if (lscp_parser_test(&tok, "active")) {
                    pszResult = "DESCRIPTION: 'ALSA PCM active state'\r\n"
                                "TYPE: BOOL\r\n"
                                "MANDATORY: TRUE\r\n"
                                "FIX: TRUE\r\n"
                                "MULTIPLICITY: FALSE\r\n"
                                "DEPENDS: channels,samplerate,card\r\n"
                                "DEFAULT: TRUE\r\n"
                                "RANGE_MIN: FALSE\r\n"
                                "RANGE_MAX: TRUE\r\n"
                                "POSSIBILITIES: FALSE,TRUE\r\n";
                }
                else if (lscp_parser_test(&tok, "channels")) {
                    pszResult = "DESCRIPTION: 'Number of ALSA PCM channels'\r\n"
                                "TYPE: INT\r\n"
                                "MANDATORY: TRUE\r\n"
                                "FIX: TRUE\r\n"
                                "MULTIPLICITY: FALSE\r\n"
                                "DEFAULT: 2\r\n";
                }
                else if (lscp_parser_test(&tok, "samplerate")) {
                    pszResult = "DESCRIPTION: 'ALSA PCM sample rate'\r\n"
                                "TYPE: INT\r\n"
                                "MANDATORY: TRUE\r\n"
                                "FIX: TRUE\r\n"
                                "MULTIPLICITY: TRUE\r\n"
                                "DEFAULT: 44100\r\n"
                                "POSSIBILITIES: 44100,48000,96000\r\n";
                }
                else ret = LSCP_FAILED;
            }
            else if (lscp_parser_test(&tok, "Jack")) {
                if (lscp_parser_test(&tok, "active")) {
                    pszResult = "DESCRIPTION: 'JACK active state'\r\n"
                                "TYPE: BOOL\r\n"
                                "MANDATORY: TRUE\r\n"
                                "FIX: TRUE\r\n"
                                "MULTIPLICITY: FALSE\r\n"
                                "DEPENDS: channels,samplerate\r\n"
                                "DEFAULT: TRUE\r\n"
                                "RANGE_MIN: FALSE\r\n"
                                "RANGE_MAX: TRUE\r\n"
                                "POSSIBILITIES: FALSE,TRUE\r\n";
                }
                else if (lscp_parser_test(&tok, "channels")) {
                    pszResult = "DESCRIPTION: 'Number of JACK audio channels'\r\n"
                                "TYPE: INT\r\n"
                                "MANDATORY: TRUE\r\n"
                                "FIX: TRUE\r\n"
                                "MULTIPLICITY: FALSE\r\n"
                                "DEFAULT: 2\r\n";
                }
                else if (lscp_parser_test(&tok, "samplerate")) {
                    pszResult = "DESCRIPTION: 'JACK sample rate'\r\n"
                                "TYPE: INT\r\n"
                                "MANDATORY: TRUE\r\n"
                                "FIX: TRUE\r\n"
                                "MULTIPLICITY: TRUE\r\n"
                                "DEFAULT: 44100\r\n"
                                "POSSIBILITIES: 44100,48000,96000\r\n";
                }
                else ret = LSCP_FAILED;
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test2(&tok, "MIDI_INPUT_DRIVER_PARAMETER", "INFO")) {
            // Getting informations about a specific MIDI input driver parameter.
            // GET MIDI_INPUT_DRIVER_PARAMETER INFO <midi-input-type> <param>
            if (lscp_parser_test(&tok, "Alsa")) {
                if (lscp_parser_test(&tok, "active")) {
                    pszResult = "DESCRIPTION: 'ALSA Sequencer device active state'\r\n"
                                "TYPE: BOOL\r\n"
                                "MANDATORY: TRUE\r\n"
                                "FIX: TRUE\r\n"
                                "MULTIPLICITY: FALSE\r\n"
                                "DEPENDS: channels,ports\r\n"
                                "DEFAULT: TRUE\r\n"
                                "RANGE_MIN: FALSE\r\n"
                                "RANGE_MAX: TRUE\r\n"
                                "POSSIBILITIES: FALSE,TRUE\r\n";
                }
                else if (lscp_parser_test(&tok, "ports")) {
                    pszResult = "DESCRIPTION: 'Number of ALSA Sequencer ports'\r\n"
                                "TYPE: INT\r\n"
                                "MANDATORY: FALSE\r\n"
                                "FIX: FALSE\r\n"
                                "MULTIPLICITY: FALSE\r\n"
                                "DEFAULT: 1\r\n"
                                "RANGE_MIN: 1\r\n"
                                "RANGE_MAX: 4\r\n";
                }
                else ret = LSCP_FAILED;
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test2(&tok, "AUDIO_OUTPUT_DEVICE", "INFO")) {
            // Getting informations about a specific audio output device.
            // GET AUDIO_OUTPUT_DEVICE INFO <audio-device-id>
            if (lscp_parser_nextint(&tok) < iAudioDevice) {
                pszResult = "driver: Alsa\r\n"
                            "active: TRUE\r\n"
                            "channels: 2\r\n"
                            "samplerate: 44100\r\n";
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test2(&tok, "MIDI_INPUT_DEVICE", "INFO")) {
            // Getting informations about a specific MIDI input device.
            // GET MIDI_INPUT_DEVICE INFO <midi-device-id>
            if (lscp_parser_nextint(&tok) < iMidiDevice) {
                pszResult = "driver: Alsa\r\n"
                            "active: TRUE\r\n"
                            "channels: 16\r\n"
                            "ports: 1\r\n";
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test2(&tok, "AUDIO_OUTPUT_CHANNEL", "INFO")) {
            // Getting informations about an audio channel.
            // GET AUDIO_OUTPUT_CHANNEL INFO <audio-device-id> <audio-channel>
            if (lscp_parser_nextint(&tok) < iAudioDevice) {
                pszResult = "name: DummyMonitor\r\n"
                            "is_mix_channel: FALSE\r\n"
                            "mix_channel_destination: 0\r\n";
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test2(&tok, "MIDI_INPUT_PORT", "INFO")) {
            // Getting informations about a MIDI port.
            // GET MIDI_INPUT_PORT INFO <midi-device-id> <midi-port>
            if (lscp_parser_nextint(&tok) < iMidiDevice) {
                pszResult = "name: DummyKeyboard\r\n"
                            "alsa_seq_bindings: '64:0'\r\n";
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test2(&tok, "AUDIO_OUTPUT_CHANNEL_PARAMETER", "INFO")) {
            // Getting informations about specific audio channel parameter.
            // GET AUDIO_OUTPUT_CHANNEL_PARAMETER INFO <audio-device-id> <audio-channel> <param>
            if (lscp_parser_nextint(&tok) < iAudioDevice) {
                lscp_parser_nextint(&tok);
                if (lscp_parser_test(&tok, "is_mix_channel")) {
                    pszResult = "DESCRIPTION: 'Whether this is an audio mix channel'\r\n"
                                "TYPE: BOOL\r\n"
                                "MANDATORY: TRUE\r\n"
                                "FIX: FALSE\r\n"
                                "MULTIPLICITY: FALSE\r\n"
                                "POSSIBILITIES: FALSE,TRUE\r\n";
                }
                else if (lscp_parser_test(&tok, "mix_channel_destination")) {
                    pszResult = "DESCRIPTION: 'Audio mix channel destination'\r\n"
                                "TYPE: INT\r\n"
                                "MANDATORY: TRUE\r\n"
                                "FIX: FALSE\r\n"
                                "MULTIPLICITY: TRUE\r\n"
                                "POSSIBILITIES: 0,1\r\n";
                }
                else ret = LSCP_FAILED;
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test2(&tok, "MIDI_INPUT_PORT_PARAMETER", "INFO")) {
            // Getting informations about specific MIDI port parameter.
            // GET MIDI_INPUT_PORT_PARAMETER INFO <midi-device-id> <midi-port> <param>
            if (lscp_parser_nextint(&tok) < iMidiDevice) {
                lscp_parser_nextint(&tok);
                if (lscp_parser_test(&tok, "alsa_seq_bindings")) {
                    pszResult = "DESCRIPTION: 'Alsa sequencer port bindings'\r\n"
                                "TYPE: STRING\r\n"
                                "MANDATORY: TRUE\r\n"
                                "FIX: FALSE\r\n"
                                "MULTIPLICITY: TRUE\r\n"
                                "POSSIBILITIES: '64:0','68:0','68:1'\r\n";
                }
                else ret = LSCP_FAILED;
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test(&tok, "AUDIO_OUTPUT_DEVICES")) {
            // Getting all created audio output device count.
            // GET AUDIO_OUTPUT_DEVICES
            sprintf(szTemp, "%d\r\n", iAudioDevice);
            pszResult = szTemp;
        }
        else if (lscp_parser_test(&tok, "MIDI_INPUT_DEVICES")) {
            // Getting all created MID input device count.
            // GET MIDI_INPUT_DEVICES
            sprintf(szTemp, "%d\r\n", iMidiDevice);
            pszResult = szTemp;
        }
        else if (lscp_parser_test(&tok, "AVAILABLE_ENGINES")) {
            // Getting all available engine count:
            // GET AVAILABLE_ENGINES
            pszResult = "3\r\n";
        }
        else if (lscp_parser_test2(&tok, "SERVER", "INFO")) {
            // Getting information about the server.
            // GET SERVER INFO
            sprintf(szTemp, "DESCRIPTION: example_server (%s) %s\r\n"
                "VERSION: %s\r\n", lscp_server_package(),
                lscp_server_build(), lscp_server_version());
            pszResult = szTemp;
        }
        else if (lscp_parser_test2(&tok, "ENGINE", "INFO")) {
            // Getting information about an engine.
            // GET ENGINE INFO <engine-name>
            if (lscp_parser_test(&tok, "GigEngine")) {
                pszResult = "DESCRIPTION: GigaSampler Engine\r\n"
                            "VERSION: 0.3\r\n";
            }
            else if (lscp_parser_test(&tok, "DLSEngine")) {
                pszResult = "DESCRIPTION: 'DLS Generic Engine'\r\n"
                            "VERSION: 0.2\r\n";
            }
            else if (lscp_parser_test(&tok, "AkaiEngine")) {
                pszResult = "DESCRIPTION: Akai Sampler Engine\r\n"
                            "VERSION: 0.1\r\n";
            }
            else ret = LSCP_FAILED;
        }
        else ret = LSCP_FAILED;
    }
    else if (lscp_parser_test(&tok, "LIST")) {
        if (lscp_parser_test(&tok, "CHANNELS")) {
            // Getting all created sampler channel list.
            // LIST CHANNELS
            if (iSamplerChannel > 0) {
                strcpy(szTemp, "0");
                for (i = 1; i < iSamplerChannel && strlen(szTemp) < sizeof(szTemp) - 8; i++)
                    sprintf(szTemp + strlen(szTemp), ",%d", i);
                strcat(szTemp, "\r\n");
                pszResult = szTemp;
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test(&tok, "AVAILABLE_ENGINES")) {
            // Getting all available engines:
            // LIST AVAILABLE_ENGINES
            pszResult = "GigEngine,DLSEngine,AkaiEngine\r\n";
        }
        else if (lscp_parser_test(&tok, "AVAILABLE_AUDIO_OUTPUT_DRIVERS")) {
            // Getting all available audio output drivers.
            // LIST AVAILABLE_AUDIO_OUTPUT_DRIVERS
            pszResult = "ALSA,JACK\r\n";
        }
        else if (lscp_parser_test(&tok, "AVAILABLE_MIDI_INPUT_DRIVERS")) {
            // Getting all available MIDI input drivers.
            // LIST AVAILABLE_MIDI_INPUT_DRIVERS
            pszResult = "ALSA\r\n";
        }
        else if (lscp_parser_test(&tok, "AUDIO_OUTPUT_DEVICES")) {
            // Getting all created audio output device list.
            // LIST AUDIO_OUTPUT_DEVICES
            if (iAudioDevice > 0) {
                strcpy(szTemp, "0");
                for (i = 1; i < iAudioDevice && strlen(szTemp) < sizeof(szTemp) - 8; i++)
                    sprintf(szTemp + strlen(szTemp), ",%d", i);
                strcat(szTemp, "\r\n");
                pszResult = szTemp;
            }
            else ret = LSCP_FAILED;
        }
        else if (lscp_parser_test(&tok, "MIDI_INPUT_DEVICES")) {
            // Getting all created MID input device list.
            // LIST MIDI_INPUT_DEVICES
            if (iMidiDevice > 0) {
                strcpy(szTemp, "0");
                for (i = 1; i < iMidiDevice && strlen(szTemp) < sizeof(szTemp) - 8; i++)
                    sprintf(szTemp + strlen(szTemp), ",%d", i);
                strcat(szTemp, "\r\n");
                pszResult = szTemp;
            }
            else ret = LSCP_FAILED;
        }
        else ret = LSCP_FAILED;
    }
    else if (lscp_parser_test(&tok, "SET")) {
        if (lscp_parser_test(&tok, "CHANNEL")) {
            if (lscp_parser_test(&tok, "VOLUME")) {
                // Setting channel volume:
                // SET CHANNEL VOLUME <sampler-channel> <volume>
            }
            else if (lscp_parser_test(&tok, "MUTE")) {
                // Muting a sampler channel:
                // SET CHANNEL MUTE <sampler-channel> <mute>
            }
            else if (lscp_parser_test(&tok, "SOLO")) {
                // Soloing a sampler channel:
                // SET CHANNEL SOLO <sampler-channel> <solo>
            }
            else if (lscp_parser_test(&tok, "AUDIO_OUTPUT_TYPE")) {
                // Setting audio output type:
                // SET CHANNEL AUDIO_OUTPUT_TYPE <sampler-channel> <audio-output-type>
            }
            else if (lscp_parser_test(&tok, "AUDIO_OUTPUT_DEVICE")) {
                // Setting audio output device:
                // SET CHANNEL AUDIO_OUTPUT_DEVICE <sampler-channel> <device-id>
            }
            else if (lscp_parser_test(&tok, "AUDIO_OUTPUT_CHANNEL")) {
                // Setting audio output channel:
                // SET CHANNEL AUDIO_OUTPUT_CHANNEL <sampler-channel> <audio-in> <audio-out>
            }
            else if (lscp_parser_test(&tok, "MIDI_INPUT_TYPE")) {
                // Setting MIDI input type:
                // SET CHANNEL MIDI_INPUT_TYPE <sampler-channel> <midi-input-type>
            }
            else if (lscp_parser_test(&tok, "MIDI_INPUT_DEVICE")) {
                // Setting MIDI input device:
                // SET CHANNEL MIDI_INPUT_DEVICE <sampler-channel> <device-id>
            }
            else if (lscp_parser_test(&tok, "MIDI_INPUT_PORT")) {
                // Setting MIDI input port:
                // SET CHANNEL MIDI_INPUT_PORT <sampler-channel> <midi-input-port>
            }
            else if (lscp_parser_test(&tok, "MIDI_INPUT_CHANNEL")) {
                // Setting MIDI input channel:
                // SET CHANNEL MIDI_INPUT_CHANNEL <sampler-channel> <midi-input-chan>
            }
            else ret = LSCP_FAILED;
        }
        else ret = LSCP_FAILED;
    }
    else if (lscp_parser_test(&tok, "LOAD")) {
        if (lscp_parser_test(&tok, "ENGINE")) {
            // Loading a sampler engine:
            // LOAD ENGINE <engine-name> <sampler-channel>
        }
        else if (lscp_parser_test(&tok, "INSTRUMENT")) {
            // Loading an instrument:
            // LOAD INSTRUMENT [NON_MODAL] <filename> <instr-index> <sampler-channel>
        }
        else ret = LSCP_FAILED;
    }
    else if (lscp_parser_test2(&tok, "ADD", "CHANNEL")) {
        // Adding a new sampler channel:
        // ADD CHANNEL
        if (iSamplerChannel < 16) {
            sprintf(szTemp, "OK[%d]", iSamplerChannel++);
            pszResult = szTemp;
        } else {
            iSamplerChannel = 0;
            ret = LSCP_FAILED;
        }
    }
    else if (lscp_parser_test2(&tok, "REMOVE", "CHANNEL")) {
        // Removing a sampler channel:
        // REMOVE CHANNEL <sampler-channel>
        if (lscp_parser_nextint(&tok) < iSamplerChannel)
            iSamplerChannel--;
        else
            ret = LSCP_FAILED;
    }
    else if (lscp_parser_test(&tok, "RESET")) {
        if (lscp_parser_test(&tok, "CHANNEL")) {
            // Resetting a sampler channel:
            // RESET CHANNEL <sampler-channel>
            if (lscp_parser_nextint(&tok) > iSamplerChannel)
                ret = LSCP_FAILED;
        } else {
            // Reset sampler:
            // RESET
            iSamplerChannel = 0;
            iAudioDevice = 0;
            iMidiDevice = 0;
        }
    }
    else if (lscp_parser_test(&tok, "CREATE")) {
        if (lscp_parser_test(&tok, "AUDIO_OUTPUT_DEVICE")) {
            // Creating an audio output device.
            // CREATE AUDIO_OUTPUT_DEVICE <audio-output-driver> [<params>]
            if (iAudioDevice < 8) {
                sprintf(szTemp, "OK[%d]", iAudioDevice++);
                pszResult = szTemp;
            } else {
                iAudioDevice = 0;
                ret = LSCP_FAILED;
            }
        }
        else if (lscp_parser_test(&tok, "MIDI_INPUT_DEVICE")) {
            // Creating an MIDI input device.
            // CREATE MIDI_INPUT_DEVICE <midi-input-driver> [<params>]
            if (iMidiDevice < 8) {
                sprintf(szTemp, "OK[%d]", iMidiDevice++);
                pszResult = szTemp;
            } else {
                iMidiDevice = 0;
                ret = LSCP_FAILED;
            }
        }
        else ret = LSCP_FAILED;
    }
    else if (lscp_parser_test(&tok, "DESTROY")) {
        if (lscp_parser_test(&tok, "AUDIO_OUTPUT_DEVICE")) {
            // Destroying an audio output device.
            // DESTROY AUDIO_OUTPUT_DEVICE <audio-device-id>
            if (lscp_parser_nextint(&tok) < iAudioDevice)
                iAudioDevice--;
            else
                ret = LSCP_FAILED;
        }
        else if (lscp_parser_test(&tok, "MIDI_INPUT_DEVICE")) {
            // Destroying an MIDI intput device.
            // DESTROY MIDI_INPUT_DEVICE <midi-device-id>
            if (lscp_parser_nextint(&tok) < iMidiDevice)
                iMidiDevice--;
            else
                ret = LSCP_FAILED;
        }
        else ret = LSCP_FAILED;
    }
    else if (lscp_parser_test(&tok, "SUBSCRIBE")) {
        // Register frontend for receiving event notification messages:
        // SUBSCRIBE <event>
        ret = lscp_server_subscribe(pConnect, lscp_event_from_text(lscp_parser_next(&tok)));
    }
    else if (lscp_parser_test(&tok, "UNSUBSCRIBE")) {
        // Deregister frontend for not receiving event notification messages anymore:
        // UNSUBSCRIBE <event>
        ret = lscp_server_unsubscribe(pConnect, lscp_event_from_text(lscp_parser_next(&tok)));
    }
    else if (lscp_parser_test(&tok, "QUIT")) {
        // Close client connection:
        // QUIT
        lscp_parser_free(&tok);
        return LSCP_FAILED; // Disconnect.
    }
    else ret = LSCP_FAILED;

    lscp_parser_free(&tok);

    if (pszResult == NULL)
        pszResult = (ret == LSCP_OK ? "OK\r\n" : "ERR:1:Failed\r\n");

    fprintf(stderr, "> %s", pszResult);

    return lscp_server_result(pConnect, pszResult, strlen(pszResult));
}

////////////////////////////////////////////////////////////////////////


void server_usage (void)
{
    printf("\n  %s %s (Build: %s)\n", lscp_server_package(), lscp_server_version(), lscp_server_build());

    fputs("\n  Available server commands: help, exit, quit, list", stdout);
    fputs("\n  (all else are broadcast verbatim to subscribers)\n\n", stdout);
}

void server_prompt (void)
{
    fputs("lscp_server> ", stdout);
}

int main (int argc, char *argv[] )
{
    lscp_server_t *pServer;
    char szLine[200];
    int cchLine;
    lscp_connect_t *p;

#if defined(WIN32)
    if (WSAStartup(MAKEWORD(1, 1), &_wsaData) != 0) {
        fprintf(stderr, "lscp_server: WSAStartup failed.\n");
        return -1;
    }
#endif

    srand(time(NULL));

    pServer = lscp_server_create(SERVER_PORT, server_callback, NULL);
    if (pServer == NULL)
        return -1;

    server_usage();
    server_prompt();

    while (fgets(szLine, sizeof(szLine), stdin)) {

        cchLine = strlen(szLine);
        while (cchLine > 0 && (szLine[cchLine - 1] == '\n' || szLine[cchLine - 1] == '\r'))
            cchLine--;
        szLine[cchLine] = '\0';

        if (strcmp(szLine, "exit") == 0 || strcmp(szLine, "quit") == 0)
            break;
        else
        if (strcmp(szLine, "list") == 0) {
            for (p = pServer->connects.first; p; p = p->next) {
                printf("client: sock=%d addr=%s port=%d events=0x%04x.\n",
                    p->client.sock,
                    inet_ntoa(p->client.addr.sin_addr),
                    ntohs(p->client.addr.sin_port),
                    (int) p->events
                );
            }
        }
        else
        if (cchLine > 0 && strcmp(szLine, "help") != 0)
            lscp_server_broadcast(pServer, LSCP_EVENT_MISCELLANEOUS, szLine, strlen(szLine));
        else
            server_usage();

        server_prompt();
    }

    lscp_server_destroy(pServer);

#if defined(WIN32)
    WSACleanup();
#endif

    return 0;
}

// end of example_server.c
