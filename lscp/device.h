// device.h
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

#ifndef __LSCP_DEVICE_H
#define __LSCP_DEVICE_H

#include "lscp/client.h"

#if defined(__cplusplus)
extern "C" {
#endif


//-------------------------------------------------------------------------
// Device driver information structures.

/** Parameter value type. */
typedef enum _lscp_type_t
{
    LSCP_TYPE_BOOL = 0,
    LSCP_TYPE_INT,
    LSCP_TYPE_FLOAT,
    LSCP_TYPE_STRING

} lscp_type_t;


/** Parameter value union type. */
typedef union _lscp_value_t
{
    int   i;
    float f;
    char *psz;

} lscp_value_t;


/** Common and simple key/value pair parameter tuple. */
typedef struct _lscp_param_t
{
    char *        key;
    lscp_value_t  value;

} lscp_param_t;


/** Common parameter info cache struct. */
typedef struct _lscp_param_info_t
{
    lscp_type_t   type;
    char *        description;
    int           mandatory;
    int           fix;
    int           multiplicity;
    char **       depends;
    lscp_value_t  defaultv;
    lscp_value_t  range_min;
    lscp_value_t  range_max;
    char **       possibilities;

} lscp_param_info_t;


/** Common driver type info cache struct. */
typedef struct _lscp_driver_info_t
{
    char *        description;
    char *        version;
    char **       parameters;

} lscp_driver_info_t;


/** Common device info cache struct. */
typedef struct _lscp_device_info_t
{
    char *        driver;
    lscp_param_t *parameters;

} lscp_device_info_t;


/** Common device channel info cache struct. */
typedef struct _lscp_device_channel_info_t
{
    char *        name;
    lscp_param_t *parameters;

} lscp_device_channel_info_t;


//-------------------------------------------------------------------------
// Audio driver control functions.

const char **           lscp_get_available_audio_drivers(lscp_client_t *pClient);

lscp_driver_info_t *    lscp_get_audio_driver_info      (lscp_client_t *pClient, const char *pszAudioDriver);
lscp_param_info_t *     lscp_get_audio_driver_param_info(lscp_client_t *pClient, const char *pszAudioDriver, const char *pszParam, lscp_param_t *pDepList);

//-------------------------------------------------------------------------
// Audio device control functions.

int                     lscp_create_audio_device        (lscp_client_t *pClient, const char *pszAudioDriver, lscp_param_t *pParams);
lscp_status_t           lscp_destroy_audio_device       (lscp_client_t *pClient, int iAudioDevice);

int *                   lscp_get_audio_devices          (lscp_client_t *pClient);
lscp_device_info_t *    lscp_get_audio_device_info      (lscp_client_t *pClient, int iAudioDevice);
lscp_status_t           lscp_set_audio_device_param     (lscp_client_t *pClient, int iAudioDevice, lscp_param_t *pParam);

lscp_device_channel_info_t *lscp_get_audio_channel_info (lscp_client_t *pClient, int iAudioDevice, int iAudioChannel );

lscp_param_info_t *     lscp_get_audio_channel_param_info   (lscp_client_t *pClient, int iAudioDevice, int iAudioChannel, const char *pszParam);
lscp_status_t           lscp_set_audio_channel_param        (lscp_client_t *pClient, int iAudioDevice, int iAudioChannel, lscp_param_t *pParam);


//-------------------------------------------------------------------------
// MIDI driver control functions.

const char **           lscp_get_available_midi_drivers (lscp_client_t *pClient);

lscp_driver_info_t *    lscp_get_midi_driver_info       (lscp_client_t *pClient, const char *pszMidiDriver);
lscp_param_info_t *     lscp_get_midi_driver_param_info (lscp_client_t *pClient, const char *pszMidiDriver, const char *pszParam, lscp_param_t *pDepList);

//-------------------------------------------------------------------------
// MIDI device control functions.

int                     lscp_create_midi_device         (lscp_client_t *pClient, const char *pszMidiDriver, lscp_param_t *pParams);
lscp_status_t           lscp_destroy_midi_device        (lscp_client_t *pClient, int iMidiDevice);

int *                   lscp_get_midi_devices           (lscp_client_t *pClient);
lscp_device_info_t *    lscp_get_midi_device_info       (lscp_client_t *pClient, int iMidiDevice);
lscp_status_t           lscp_set_midi_device_param      (lscp_client_t *pClient, int iMidiDevice, lscp_param_t *pParam);

lscp_device_channel_info_t *lscp_get_midi_port_info     (lscp_client_t *pClient, int iMidiDevice, int iMidiPort);

lscp_param_info_t *     lscp_get_midi_port_param_info   (lscp_client_t *pClient, int iMidiDevice, int iMidiPort, const char *pszParam);
lscp_status_t           lscp_set_midi_port_param        (lscp_client_t *pClient, int iMidiDevice, int iMidiPort, lscp_param_t *pParam);


#if defined(__cplusplus)
}
#endif

#endif // __LSCP_DEVICE_H

// end of device.h
