// device.c
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

#include "common.h"


// Local prototypes.

static lscp_driver_info_t *_lscp_driver_info_query (lscp_client_t *pClient, lscp_driver_info_t *pDriverInfo, char *pszQuery);
static lscp_param_info_t  *_lscp_param_info_query  (lscp_client_t *pClient, lscp_param_info_t *pParamInfo, char *pszQuery, lscp_param_t *pDepList);


//-------------------------------------------------------------------------
// Local funtions.

// Common driver type query command.
static lscp_driver_info_t *_lscp_driver_info_query ( lscp_client_t *pClient, lscp_driver_info_t *pDriverInfo, char *pszQuery )
{
    const char *pszResult;
    const char *pszSeps = ":";
    const char *pszCrlf = "\r\n";
    char *pszToken;
    char *pch;

    // Lock this section up.
    lscp_mutex_lock(pClient->mutex);

    lscp_driver_info_reset(pDriverInfo);
    if (lscp_client_call(pClient, pszQuery) == LSCP_OK) {
        pszResult = lscp_client_get_result(pClient);
        pszToken = lscp_strtok((char *) pszResult, pszSeps, &(pch));
        while (pszToken) {
            if (strcasecmp(pszToken, "DESCRIPTION") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pDriverInfo->description = lscp_unquote(&pszToken, 1);
            }
            else if (strcasecmp(pszToken, "VERSION") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pDriverInfo->version = lscp_unquote(&pszToken, 1);
            }
            else if (strcasecmp(pszToken, "PARAMETERS") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pDriverInfo->parameters = lscp_szsplit_create(pszToken, ",");
            }
            pszToken = lscp_strtok(NULL, pszSeps, &(pch));
        }
    }
    
    // Unlock this section down.
    lscp_mutex_unlock(pClient->mutex);

    return pDriverInfo;
}


// Common parameter info query command.
static lscp_param_info_t *_lscp_param_info_query ( lscp_client_t *pClient, lscp_param_info_t *pParamInfo, char *pszQuery, lscp_param_t *pDepList )
{
    const char *pszResult;
    const char *pszSeps = ":";
    const char *pszCrlf = "\r\n";
    char *pszToken;
    char *pch;

    // Lock this section up.
    lscp_mutex_lock(pClient->mutex);

    lscp_param_info_reset(pParamInfo);
    lscp_param_concat(pszQuery, LSCP_BUFSIZ, pDepList);
    if (lscp_client_call(pClient, pszQuery) == LSCP_OK) {
        pszResult = lscp_client_get_result(pClient);
        pszToken = lscp_strtok((char *) pszResult, pszSeps, &(pch));
        while (pszToken) {
            if (strcasecmp(pszToken, "TYPE") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken) {
                    pszToken = lscp_unquote(&pszToken, 0);
                    if (strcasecmp(pszToken, "BOOL") == 0)
                        pParamInfo->type = LSCP_TYPE_BOOL;
                    else if (strcasecmp(pszToken, "INT") == 0)
                        pParamInfo->type = LSCP_TYPE_INT;
                    else if (strcasecmp(pszToken, "FLOAT") == 0)
                        pParamInfo->type = LSCP_TYPE_FLOAT;
                    else if (strcasecmp(pszToken, "STRING") == 0)
                        pParamInfo->type = LSCP_TYPE_STRING;
                }
            }
            else if (strcasecmp(pszToken, "DESCRIPTION") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pParamInfo->description = lscp_unquote(&pszToken, 1);
            }
            else if (strcasecmp(pszToken, "MANDATORY") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pParamInfo->mandatory = (strcasecmp(lscp_unquote(&pszToken, 0), "TRUE") == 0);
            }
            else if (strcasecmp(pszToken, "FIX") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pParamInfo->fix = (strcasecmp(lscp_unquote(&pszToken, 0), "TRUE") == 0);
            }
            else if (strcasecmp(pszToken, "MULTIPLICITY") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pParamInfo->multiplicity = (strcasecmp(lscp_unquote(&pszToken, 0), "TRUE") == 0);
            }
            else if (strcasecmp(pszToken, "DEPENDS") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pParamInfo->depends = lscp_szsplit_create(pszToken, ",");
            }
            else if (strcasecmp(pszToken, "DEFAULT") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pParamInfo->defaultv = lscp_unquote(&pszToken, 1);
            }
            else if (strcasecmp(pszToken, "RANGE_MIN") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pParamInfo->range_min = lscp_unquote(&pszToken, 1);
            }
            else if (strcasecmp(pszToken, "RANGE_MAX") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pParamInfo->range_max = lscp_unquote(&pszToken, 1);
            }
            else if (strcasecmp(pszToken, "POSSIBILITIES") == 0) {
                pszToken = lscp_strtok(NULL, pszCrlf, &(pch));
                if (pszToken)
                    pParamInfo->possibilities = lscp_szsplit_create(pszToken, ",");
            }
            pszToken = lscp_strtok(NULL, pszSeps, &(pch));
        }
    }

    // Unlock this section down.
    lscp_mutex_unlock(pClient->mutex);

    return pParamInfo;
}


//-------------------------------------------------------------------------
// Audio driver control functions.

/**
 *  Getting all available audio output drivers.
 *  GET AVAILABLE_AUDIO_OUTPUT_DRIVERS
 *
 *  @param pClient  Pointer to client instance structure.
 *
 *  @returns A NULL terminated array of audio output driver type
 *  name strings, or NULL in case of failure.
 */
const char ** lscp_get_available_audio_drivers ( lscp_client_t *pClient )
{
    const char *pszSeps = ",";

    // Lock this section up.
    lscp_mutex_lock(pClient->mutex);

    if (pClient->audio_drivers) {
        lscp_szsplit_destroy(pClient->audio_drivers);
        pClient->audio_drivers = NULL;
    }

    if (lscp_client_call(pClient, "GET AVAILABLE_AUDIO_OUTPUT_DRIVERS\r\n") == LSCP_OK)
        pClient->audio_drivers = lscp_szsplit_create(lscp_client_get_result(pClient), pszSeps);

    // Unlock this section down.
    lscp_mutex_unlock(pClient->mutex);

    return (const char **) pClient->audio_drivers;
}


/**
 *  Getting informations about a specific audio output driver.
 *  GET AUDIO_OUTPUT_DRIVER INFO <audio-output-type>
 *
 *  @param pClient          Pointer to client instance structure.
 *  @param pszAudioDriver   Audio driver type string (e.g. "ALSA").
 *
 *  @returns A pointer to a @ref lscp_driver_info_t structure, with
 *  the given audio driver information, or NULL in case of failure.
 */
lscp_driver_info_t* lscp_get_audio_driver_info ( lscp_client_t *pClient, const char *pszAudioDriver )
{
    char szQuery[LSCP_BUFSIZ];

    if (pszAudioDriver == NULL)
        return NULL;

    sprintf(szQuery, "GET AUDIO_OUTPUT_DRIVER INFO %s\r\n", pszAudioDriver);
    return _lscp_driver_info_query(pClient, &(pClient->audio_info), szQuery);
}


/**
 *  Getting informations about specific audio output driver parameter.
 *  GET AUDIO_OUTPUT_DRIVER_PARAMETER INFO <audio-output-driver> <param> [<dep-list>]
 *
 *  @param pClient          Pointer to client instance structure.
 *  @param pszAudioDriver   Audio driver type string (e.g. "ALSA").
 *  @param pszParam         Audio driver parameter name.
 *  @param pDepList         Pointer to specific dependencies parameter list.
 *
 *  @returns A pointer to a @ref lscp_param_info_t structure, with
 *  the given audio driver parameter information, or NULL in case of failure.
 */
lscp_param_info_t *lscp_get_audio_driver_param_info ( lscp_client_t *pClient, const char *pszAudioDriver, const char *pszParam, lscp_param_t *pDepList )
{
    char szQuery[LSCP_BUFSIZ];

    if (pClient == NULL)
        return NULL;
    if (pszAudioDriver == NULL)
        return NULL;
    if (pszParam == NULL)
        return NULL;

    sprintf(szQuery, "GET AUDIO_OUTPUT_DRIVER_PARAMETER INFO %s %s\r\n", pszAudioDriver, pszParam);
    return _lscp_param_info_query(pClient, &(pClient->audio_param_info), szQuery, pDepList);
}


//-------------------------------------------------------------------------
// Audio device control functions.

/**
 *  Creating an audio output device.
 *  CREATE AUDIO_OUTPUT_DEVICE <audio-output-driver> [<params>]
 *
 *  @param pClient          Pointer to client instance structure.
 *  @param pszAudioDriver   Audio driver type string (e.g. "ALSA").
 *  @param pParams          Pointer to specific parameter list.
 *
 *  @returns The new audio device number identifier on success,
 *  or -1 in case of failure.
 */
int lscp_create_audio_device ( lscp_client_t *pClient, const char *pszAudioDriver, lscp_param_t *pParams )
{
    int iAudioDevice = -1;

    if (pClient == NULL)
        return -1;
    if (pszAudioDriver == NULL)
        return -1;
    if (pParams == NULL)
        return -1;

    return iAudioDevice;
}


/**
 *  Destroying an audio output device.
 *  DESTROY AUDIO_OUTPUT_DEVICE <audio-device-id>
 *
 *  @param pClient      Pointer to client instance structure.
 *  @param iAudioDevice Audio device number identifier.
 *
 *  @returns LSCP_OK on success, LSCP_FAILED otherwise.
 */
lscp_status_t lscp_destroy_audio_device ( lscp_client_t *pClient, int iAudioDevice )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pClient == NULL)
        return ret;
    if (iAudioDevice < 0)
        return ret;

    return ret;
}


/**
 *  Getting all created audio output device count.
 *  GET AUDIO_OUTPUT_DEVICES
 *
 *  @param pClient  Pointer to client instance structure.
 *
 *  @returns The current total number of audio devices on success,
 *  -1 otherwise.
 */
int lscp_get_audio_devices ( lscp_client_t *pClient )
{
    int iAudioDevices = -1;

    // Lock this section up.
    lscp_mutex_lock(pClient->mutex);

    if (lscp_client_call(pClient, "GET AUDIO_OUTPUT_DEVICES\r\n") == LSCP_OK)
        iAudioDevices = atoi(lscp_client_get_result(pClient));

    // Unlock this section down.
    lscp_mutex_unlock(pClient->mutex);

    return iAudioDevices;
}


/**
 *  Getting all created audio output device list.
 *  LIST AUDIO_OUTPUT_DEVICES
 *
 *  @param pClient  Pointer to client instance structure.
 *
 *  @returns An array of audio device number identifiers,
 *  terminated with -1 on success, or NULL in case of failure.
 */
int *lscp_list_audio_devices ( lscp_client_t *pClient )
{
    const char *pszSeps = ",";

    if (pClient == NULL)
        return NULL;

    // Lock this section up.
    lscp_mutex_lock(pClient->mutex);

    if (pClient->audio_devices) {
        lscp_isplit_destroy(pClient->audio_devices);
        pClient->audio_devices = NULL;
    }

    if (lscp_client_call(pClient, "LIST AUDIO_OUTPUT_DEVICES\r\n") == LSCP_OK)
        pClient->audio_devices = lscp_isplit_create(lscp_client_get_result(pClient), pszSeps);

    // Unlock this section down.
    lscp_mutex_unlock(pClient->mutex);

    return pClient->audio_devices;
}


/**
 *  Getting current settings of an audio output device.
 *  GET AUDIO_OUTPUT_DEVICE INFO <audio-device-id>
 *
 *  @param pClient      Pointer to client instance structure.
 *  @param iAudioDevice Audio device number identifier.
 *
 *  @returns A pointer to a @ref lscp_device_info_t structure, with
 *  the given audio device information, or NULL in case of failure.
 */
lscp_device_info_t *lscp_get_audio_device_info ( lscp_client_t *pClient, int iAudioDevice )
{
    lscp_device_info_t *pDeviceInfo = NULL;

    if (pClient == NULL)
        return NULL;
    if (iAudioDevice < 0)
        return NULL;

    return pDeviceInfo;
}


/**
 *  Changing settings of audio output devices.
 *  SET AUDIO_OUTPUT_DEVICE_PARAMETER <audio-device-id> <param> <value>
 *
 *  @param pClient      Pointer to client instance structure.
 *  @param iAudioDevice Audio device number identifier.
 *  @param pParam       Pointer to a key-valued audio device parameter.
 *
 *  @returns LSCP_OK on success, LSCP_FAILED otherwise.
 */
lscp_status_t lscp_set_audio_device_param ( lscp_client_t *pClient, int iAudioDevice, lscp_param_t *pParam )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pClient == NULL)
        return ret;
    if (iAudioDevice < 0)
        return ret;
    if (pParam == NULL)
        return ret;

    return ret;
}


/**
 *  Getting informations about an audio channel.
 *  GET AUDIO_OUTPUT_CHANNEL INFO <audio-device-id> <audio-channel>
 *
 *  @param pClient          Pointer to client instance structure.
 *  @param iAudioDevice     Audio device number identifier.
 *  @param iAudioChannel    Audio channel number.
 *
 *  @returns A pointer to a @ref lscp_device_channel_info_t structure,
 *  with the given audio channel information, or NULL in case of failure.
 */
lscp_device_channel_info_t* lscp_get_audio_channel_info ( lscp_client_t *pClient, int iAudioDevice, int iAudioChannel )
{
    lscp_device_channel_info_t *pDevChannelInfo = NULL;

    if (pClient == NULL)
        return NULL;
    if (iAudioDevice < 0)
        return NULL;
    if (iAudioChannel < 0)
        return NULL;

    return pDevChannelInfo;
}


/**
 *  Getting informations about specific audio channel parameter.
 *  GET AUDIO_OUTPUT_CHANNEL_PARAMETER INFO <audio-device-id> <audio-channel> <param>
 *
 *  @param pClient          Pointer to client instance structure.
 *  @param iAudioDevice     Audio device number identifier.
 *  @param iAudioChannel    Audio channel number.
 *  @param pszParam         Audio channel parameter name.
 *
 *  @returns A pointer to a @ref lscp_param_info_t structure, with
 *  the given audio channel parameter information, or NULL in case of failure.
 */
lscp_param_info_t* lscp_get_audio_channel_param_info ( lscp_client_t *pClient, int iAudioDevice, int iAudioChannel, const char *pszParam )
{
    lscp_param_info_t *pParamInfo = NULL;

    if (pClient == NULL)
        return NULL;
    if (iAudioDevice < 0)
        return NULL;
    if (iAudioChannel < 0)
        return NULL;
    if (pszParam == NULL)
        return NULL;

    return pParamInfo;
}


/**
 *  Changing settings of audio output channels.
 *  SET AUDIO_OUTPUT_CHANNEL_PARAMETER <audio-device-id> <audio-channel> <param> <value>
 *
 *  @param pClient          Pointer to client instance structure.
 *  @param iAudioDevice     Audio device number identifier.
 *  @param iAudioChannel    Audio channel number.
 *  @param pParam           Pointer to a key-valued audio channel parameter.
 *
 *  @returns LSCP_OK on success, LSCP_FAILED otherwise.
 */
lscp_status_t lscp_set_audio_channel_param ( lscp_client_t *pClient, int iAudioDevice, int iAudioChannel, lscp_param_t *pParam )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pClient == NULL)
        return ret;
    if (iAudioDevice < 0)
        return ret;
    if (iAudioChannel < 0)
        return ret;
    if (pParam == NULL)
        return ret;

    return ret;
}


//-------------------------------------------------------------------------
// MIDI driver control functions.

/**
 *  Getting all available MIDI input drivers.
 *  GET AVAILABLE_MIDI_INPUT_DRIVERS
 *
 *  @param pClient  Pointer to client instance structure.
 *
 *  @returns A NULL terminated array of MIDI input driver type
 *  name strings, or NULL in case of failure.
 */
const char** lscp_get_available_midi_drivers ( lscp_client_t *pClient )
{
    const char *pszSeps = ",";

    // Lock this section up.
    lscp_mutex_lock(pClient->mutex);

    if (pClient->midi_drivers) {
        lscp_szsplit_destroy(pClient->midi_drivers);
        pClient->midi_drivers = NULL;
    }

    if (lscp_client_call(pClient, "GET AVAILABLE_MIDI_INPUT_DRIVERS\r\n") == LSCP_OK)
        pClient->midi_drivers = lscp_szsplit_create(lscp_client_get_result(pClient), pszSeps);

    // Unlock this section up.
    lscp_mutex_unlock(pClient->mutex);

    return (const char **) pClient->midi_drivers;
}


/**
 *  Getting informations about a specific MIDI input driver.
 *  GET MIDI_INPUT_DRIVER INFO <midi-input-type>
 *
 *  @param pClient          Pointer to client instance structure.
 *  @param pszMidiDriver    MIDI driver type string (e.g. "ALSA").
 *
 *  @returns A pointer to a @ref lscp_driver_info_t structure, with
 *  the given MIDI driver information, or NULL in case of failure.
 */
lscp_driver_info_t* lscp_get_midi_driver_info ( lscp_client_t *pClient, const char *pszMidiDriver )
{
    char szQuery[LSCP_BUFSIZ];

    if (pszMidiDriver == NULL)
        return NULL;

    sprintf(szQuery, "GET MIDI_INPUT_DRIVER INFO %s\r\n", pszMidiDriver);
    return _lscp_driver_info_query(pClient, &(pClient->midi_info), szQuery);
}


/**
 *  Getting informations about specific MIDI input driver parameter.
 *  GET MIDI_INPUT_DRIVER_PARAMETER INFO <midi-input-driver> <param> [<dep-list>]
 *
 *  @param pClient          Pointer to client instance structure.
 *  @param pszMidiDriver    MIDI driver type string (e.g. "ALSA").
 *  @param pszParam         MIDI driver parameter name.
 *  @param pDepList         Pointer to specific dependencies parameter list.
 *
 *  @returns A pointer to a @ref lscp_param_info_t structure, with
 *  the given MIDI driver parameter information, or NULL in case of failure.
 *
 *  @returns LSCP_OK on success, LSCP_FAILED otherwise.
 */
lscp_param_info_t *lscp_get_midi_driver_param_info ( lscp_client_t *pClient, const char *pszMidiDriver, const char *pszParam, lscp_param_t *pDepList )
{
    char szQuery[LSCP_BUFSIZ];

    if (pClient == NULL)
        return NULL;
    if (pszMidiDriver == NULL)
        return NULL;
    if (pszParam == NULL)
        return NULL;

    sprintf(szQuery, "GET MIDI_INPUT_DRIVER_PARAMETER INFO %s %s\r\n", pszMidiDriver, pszParam);
    return _lscp_param_info_query(pClient, &(pClient->midi_param_info), szQuery, pDepList);
}


//-------------------------------------------------------------------------
// MIDI device control functions.

/**
 *  Creating a MIDI input device.
 *  CREATE MIDI_INPUT_DEVICE <midi-input-driver> [<params>]
 *
 *  @param pClient          Pointer to client instance structure.
 *  @param pszMidiDriver    MIDI driver type string (e.g. "ALSA").
 *  @param pParams          Pointer to specific parameter list.
 *
 *  @returns The new audio device number identifier on success,
 *  or -1 in case of failure.
 */
int lscp_create_midi_device ( lscp_client_t *pClient, const char *pszMidiDriver, lscp_param_t *pParams )
{
    int iMidiDevice = -1;

    if (pClient == NULL)
        return -1;
    if (pszMidiDriver == NULL)
        return -1;
    if (pParams == NULL)
        return -1;

    return iMidiDevice;
}


/**
 *  Destroying a MIDI input device.
 *  DESTROY MIDI_INPUT_DEVICE <midi-device-id>
 *
 *  @param pClient      Pointer to client instance structure.
 *  @param iMidiDevice  MIDI device number identifier.
 *
 *  @returns LSCP_OK on success, LSCP_FAILED otherwise.
 */
lscp_status_t lscp_destroy_midi_device ( lscp_client_t *pClient, int iMidiDevice )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pClient == NULL)
        return ret;
    if (iMidiDevice < 0)
        return ret;

    return ret;
}


/**
 *  Getting all created MIDI intput device count.
 *  GET MIDI_INPUT_DEVICES
 *
 *  @param pClient  Pointer to client instance structure.
 *
 *  @returns The current total number of MIDI devices on success,
 *  -1 otherwise.
 */
int lscp_get_midi_devices ( lscp_client_t *pClient )
{
    int iMidiDevices = -1;

    // Lock this section up.
    lscp_mutex_lock(pClient->mutex);

    if (lscp_client_call(pClient, "GET MIDI_INPUT_DEVICES\r\n") == LSCP_OK)
        iMidiDevices = atoi(lscp_client_get_result(pClient));
        
    // Unlock this section down.
    lscp_mutex_unlock(pClient->mutex);

    return iMidiDevices;
}


/**
 *  Getting all created MIDI intput device list.
 *  LIST MIDI_INPUT_DEVICES
 *
 *  @param pClient  Pointer to client instance structure.
 *
 *  @returns An array of MIDI device number identifiers,
 *  terminated with -1 on success, or NULL in case of failure.
 */
int *lscp_list_midi_devices ( lscp_client_t *pClient )
{
    const char *pszSeps = ",";

    if (pClient == NULL)
        return NULL;

    // Lock this section up.
    lscp_mutex_lock(pClient->mutex);

    if (pClient->midi_devices) {
        lscp_isplit_destroy(pClient->midi_devices);
        pClient->midi_devices = NULL;
    }

    if (lscp_client_call(pClient, "LIST MIDI_INPUT_DEVICES\r\n") == LSCP_OK)
        pClient->midi_devices = lscp_isplit_create(lscp_client_get_result(pClient), pszSeps);

    // Unlock this section down.
    lscp_mutex_unlock(pClient->mutex);

    return pClient->midi_devices;
}


/**
 *  Getting current settings of a MIDI input device.
 *  GET MIDI_INPUT_DEVICE INFO <midi-device-id>
 *
 *  @param pClient      Pointer to client instance structure.
 *  @param iMidiDevice  MIDI device number identifier.
 *
 *  @returns A pointer to a @ref lscp_device_info_t structure, with
 *  the given MIDI device information, or NULL in case of failure.
 */
lscp_device_info_t* lscp_get_midi_device_info ( lscp_client_t *pClient, int iMidiDevice )
{
    lscp_device_info_t *pDeviceInfo = NULL;

    if (pClient == NULL)
        return NULL;
    if (iMidiDevice < 0)
        return NULL;

    return pDeviceInfo;
}


/**
 *  Changing settings of MIDI input devices.
 *  SET MIDI_INPUT_DEVICE_PARAMETER <midi-device-id> <param> <value>
 *
 *  @param pClient      Pointer to client instance structure.
 *  @param iMidiDevice  MIDI device number identifier.
 *  @param pParam       Pointer to a key-valued MIDI device parameter.
 *
 *  @returns LSCP_OK on success, LSCP_FAILED otherwise.
 */
lscp_status_t lscp_set_midi_device_param ( lscp_client_t *pClient, int iMidiDevice, lscp_param_t *pParam )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pClient == NULL)
        return ret;
    if (iMidiDevice < 0)
        return ret;
    if (pParam == NULL)
        return ret;

    return ret;
}


/**
 *  Getting informations about a MIDI port.
 *  GET MIDI_INPUT_PORT INFO <midi-device-id> <midi-port>
 *
 *  @param pClient      Pointer to client instance structure.
 *  @param iMidiDevice  MIDI device number identifier.
 *  @param iMidiPort    MIDI port number.
 *
 *  @returns A pointer to a @ref lscp_device_channel_info_t structure,
 *  with the given MIDI port information, or NULL in case of failure.
 */
lscp_device_channel_info_t* lscp_get_midi_port_info ( lscp_client_t *pClient, int iMidiDevice, int iMidiPort )
{
    lscp_device_channel_info_t *pDevChannelInfo = NULL;

    if (pClient == NULL)
        return NULL;
    if (iMidiDevice < 0)
        return NULL;
    if (iMidiPort < 0)
        return NULL;

    return pDevChannelInfo;
}


/**
 *  Getting informations about specific MIDI port parameter.
 *  GET MIDI_INPUT_PORT_PARAMETER INFO <midi-device-id> <midi-port> <param>
 *
 *  @param pClient      Pointer to client instance structure.
 *  @param iMidiDevice  MIDI device number identifier.
 *  @param iMidiPort    MIDI port number.
 *  @param pszParam     MIDI port parameter name.
 *
 *  @returns A pointer to a @ref lscp_param_info_t structure, with
 *  the given MIDI port parameter information, or NULL in case of failure.
 */
lscp_param_info_t* lscp_get_midi_port_param_info ( lscp_client_t *pClient, int iMidiDevice, int iMidiPort, const char *pszParam )
{
    lscp_param_info_t *pParamInfo = NULL;

    if (pClient == NULL)
        return NULL;
    if (iMidiDevice < 0)
        return NULL;
    if (iMidiPort < 0)
        return NULL;
    if (pszParam == NULL)
        return NULL;

    return pParamInfo;
}


/**
 *  Changing settings of MIDI input ports.
 *  SET MIDI_INPUT_PORT_PARAMETER <midi-device-id> <midi-port> <param> <value>
 *
 *  @param pClient      Pointer to client instance structure.
 *  @param iMidiDevice  MIDI device number identifier.
 *  @param iMidiPort    MIDI port number.
 *  @param pParam       Pointer to a key-valued MIDI port parameter.
 *
 *  @returns LSCP_OK on success, LSCP_FAILED otherwise.
 */
lscp_status_t lscp_set_midi_port_param ( lscp_client_t *pClient, int iMidiDevice, int iMidiPort, lscp_param_t *pParam )
{
    lscp_status_t ret = LSCP_FAILED;

    if (pClient == NULL)
        return ret;
    if (iMidiDevice < 0)
        return ret;
    if (iMidiPort < 0)
        return ret;
    if (pParam == NULL)
        return ret;

    return ret;
}

// end of device.c

