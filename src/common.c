// client.c
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

#include <ctype.h>


// Chunk size magic:
// LSCP_SPLIT_CHUNK1 = 2 ^ LSCP_SPLIT_CHUNK2
#define LSCP_SPLIT_CHUNK1   4
#define LSCP_SPLIT_CHUNK2   2
// Chunk size legal calculator.
#define LSCP_SPLIT_SIZE(n) ((((n) >> LSCP_SPLIT_CHUNK2) + 1) << LSCP_SPLIT_CHUNK2)


//-------------------------------------------------------------------------
// General utility functions.

// Trimming left spaces...
char *lscp_ltrim ( char *psz )
{
    while (isspace(*psz))
        psz++;
    return psz;
}

// Unquote an in-split string.
char *lscp_unquote ( char **ppsz, int dup )
{
    char  chQuote;
    char *psz = *ppsz;

    while (isspace(*psz))
        ++psz;
    if (*psz == '\"' || *psz == '\'') {
        chQuote = *psz++;
        while (isspace(*psz))
            ++psz;
        if (dup)
            psz = strdup(psz);
        *ppsz = psz;
        if (*ppsz) {
            while (**ppsz && **ppsz != chQuote)
                ++(*ppsz);
            if (**ppsz) {
                while (isspace(*(*ppsz - 1)) && *ppsz > psz)
                    --(*ppsz);
                *(*ppsz)++ = (char) 0;
            }
        }
    }
    else if (dup) {
        psz = strdup(psz);
        *ppsz = psz;
    }

    return psz;
}


// Custom tokenizer.
char *lscp_strtok ( char *pchBuffer, const char *pszSeps, char **ppch )
{
    char *pszToken;

    if (pchBuffer == NULL)
        pchBuffer = *ppch;

    pchBuffer += strspn(pchBuffer, pszSeps);
    if (*pchBuffer == '\0')
        return NULL;

    pszToken  = pchBuffer;
    pchBuffer = strpbrk(pszToken, pszSeps);
    if (pchBuffer == NULL) {
        *ppch = strchr(pszToken, '\0');
    } else {
        *pchBuffer = '\0';
        *ppch = pchBuffer + 1;
        while (**ppch && strchr(pszSeps, **ppch))
            (*ppch)++;
    }

    return pszToken;
}


// Split a comma separated string into a null terminated array of strings.
char **lscp_szsplit_create ( const char *pszCsv, const char *pszSeps )
{
    char *pszHead, *pch;
    int iSize, i, j, cchSeps;
    char **ppszSplit, **ppszNewSplit;

    // Initial size is one chunk away.
    iSize = LSCP_SPLIT_CHUNK1;
    // Allocate and split...
    ppszSplit = (char **) malloc(iSize * sizeof(char *));
    if (ppszSplit == NULL)
        return NULL;

    // Make a copy of the original string.
    i = 0;
    pszHead = (char *) pszCsv;
    if ((ppszSplit[i++] = lscp_unquote(&pszHead, 1)) == NULL) {
        free(ppszSplit);
        return NULL;
    }

    // Go on for it...
    cchSeps = strlen(pszSeps);
    while ((pch = strpbrk(pszHead, pszSeps)) != NULL) {
        // Pre-advance to next item.
        pszHead = pch + cchSeps;
        // Trim and null terminate current item.
        while (isspace(*(pch - 1)) && pch > ppszSplit[0])
            --pch;
        *pch = (char) 0;
        // Make it official.
        ppszSplit[i++] = lscp_unquote(&pszHead, 0);
        // Do we need to grow?
        if (i >= iSize) {
            // Yes, but only grow in chunks.
            iSize += LSCP_SPLIT_CHUNK1;
            // Allocate and copy to new split array.
            ppszNewSplit = (char **) malloc(iSize * sizeof(char *));
            if (ppszNewSplit) {
                for (j = 0; j < i; j++)
                    ppszNewSplit[j] = ppszSplit[j];
                free(ppszSplit);
                ppszSplit = ppszNewSplit;
            }
        }
    }

    // NULL terminate split array.
    for ( ; i < iSize; i++)
        ppszSplit[i] = NULL;

    return ppszSplit;
}


// Free allocated memory of a legal null terminated array of strings.
void lscp_szsplit_destroy ( char **ppszSplit )
{
    // Our split string is always the first item, if any.
    if (ppszSplit && ppszSplit[0])
        free(ppszSplit[0]);
    // Now free the array itself.
    if (ppszSplit)
        free(ppszSplit);
}


#ifdef LSCP_SZSPLIT_COUNT

// Return the number of items of a null terminated array of strings.
int lscp_szsplit_count ( char **ppszSplit )
{
    int i = 0;
    while (ppszSplit && ppszSplit[i])
        i++;
    return i;
}

// Return the allocated number of items of a splitted string array.
int lscp_szsplit_size ( char **ppszSplit )
{
    return LSCP_SPLIT_SIZE(lscp_szsplit_count(ppszSplit));
}

#endif // LSCP_SZSPLIT_COUNT


// Split a comma separated string into a -1 terminated array of positive integers.
int *lscp_isplit_create ( const char *pszCsv, const char *pszSeps )
{
    char *pchHead, *pch;
    int iSize, i, j, cchSeps;
    int *piSplit, *piNewSplit;

    // Initial size is one chunk away.
    iSize = LSCP_SPLIT_CHUNK1;
    // Allocate and split...
    piSplit = (int *) malloc(iSize * sizeof(int));
    if (piSplit == NULL)
        return NULL;

    // Make a copy of the original string.
    i = 0;
    pchHead = (char *) pszCsv;
    if ((piSplit[i++] = atoi(pchHead)) < 0) {
        free(piSplit);
        return NULL;
    }

    // Go on for it...
    cchSeps = strlen(pszSeps);
    while ((pch = strpbrk(pchHead, pszSeps)) != NULL) {
        // Pre-advance to next item.
        pchHead = pch + cchSeps;
        // Make it official.
        piSplit[i++] = atoi(pchHead);
        // Do we need to grow?
        if (i >= iSize) {
            // Yes, but only grow in chunks.
            iSize += LSCP_SPLIT_CHUNK1;
            // Allocate and copy to new split array.
            piNewSplit = (int *) malloc(iSize * sizeof(int));
            if (piNewSplit) {
                for (j = 0; j < i; j++)
                    piNewSplit[j] = piSplit[j];
                free(piSplit);
                piSplit = piNewSplit;
            }
        }
    }

    // NULL terminate split array.
    for ( ; i < iSize; i++)
        piSplit[i] = -1;

    return piSplit;
}


// Destroy a integer splitted array.
void lscp_isplit_destroy ( int *piSplit )
{
    if (piSplit)
        free(piSplit);
}


#ifdef LSCP_ISPLIT_COUNT

// Compute a string list valid item count.
int lscp_isplit_count ( int *piSplit )
{
    int i = 0;
    while (piSplit && piSplit[i] >= 0)
        i++;
    return i;
}

// Compute a string list size.
int lscp_isplit_size ( int *piSplit )
{
    return LSCP_SPLIT_SIZE(lscp_isplit_count(piSplit));
}

#endif // LSCP_ISPLIT_COUNT


//-------------------------------------------------------------------------
// Engine info struct helper functions.

void lscp_engine_info_init ( lscp_engine_info_t *pEngineInfo )
{
    pEngineInfo->description = NULL;
    pEngineInfo->version     = NULL;
}

void lscp_engine_info_reset ( lscp_engine_info_t *pEngineInfo )
{
    if (pEngineInfo->description)
        free(pEngineInfo->description);
    if (pEngineInfo->version)
        free(pEngineInfo->version);

    lscp_engine_info_init(pEngineInfo);
}


//-------------------------------------------------------------------------
// Channel info struct helper functions.

void lscp_channel_info_init ( lscp_channel_info_t *pChannelInfo )
{
    pChannelInfo->engine_name     = NULL;
    pChannelInfo->audio_device    = 0;
    pChannelInfo->audio_channels  = 0;
    pChannelInfo->audio_routing   = NULL;
    pChannelInfo->instrument_file = NULL;
    pChannelInfo->instrument_nr   = 0;
    pChannelInfo->midi_device     = 0;
    pChannelInfo->midi_port       = 0;
    pChannelInfo->midi_channel    = 0;
    pChannelInfo->volume          = 0.0;
}

void lscp_channel_info_reset ( lscp_channel_info_t *pChannelInfo )
{
    if (pChannelInfo->engine_name)
        free(pChannelInfo->engine_name);
    if (pChannelInfo->audio_routing)
        lscp_szsplit_destroy(pChannelInfo->audio_routing);
    if (pChannelInfo->instrument_file)
        free(pChannelInfo->instrument_file);

    lscp_channel_info_init(pChannelInfo);
}


//-------------------------------------------------------------------------
// Driver info struct functions.

void lscp_driver_info_init ( lscp_driver_info_t *pDriverInfo )
{
    pDriverInfo->description = NULL;
    pDriverInfo->version     = NULL;
    pDriverInfo->parameters  = NULL;
}

void lscp_driver_info_reset ( lscp_driver_info_t *pDriverInfo )
{
    if (pDriverInfo->description)
        free(pDriverInfo->description);
    if (pDriverInfo->version)
        free(pDriverInfo->version);
    lscp_szsplit_destroy(pDriverInfo->parameters);

    lscp_driver_info_init(pDriverInfo);
}


// end of common.c
