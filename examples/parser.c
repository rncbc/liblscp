// parser.c
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

#include "parser.h"

// Case unsensitive comparison substitutes.
#if defined(WIN32)
#define strcasecmp      stricmp
#define strncasecmp     strnicmp
#endif

//-------------------------------------------------------------------------
// Simple token parser.

const char *lscp_parser_strtok ( char *pchBuffer, const char *pszDelim, char **ppch )
{
    const char *pszToken;

    if (pchBuffer == NULL)
        pchBuffer = *ppch;

    pchBuffer += strspn(pchBuffer, pszDelim);
    if (*pchBuffer == '\0')
        return NULL;

    pszToken  = pchBuffer;
    pchBuffer = strpbrk(pszToken, pszDelim);
    if (pchBuffer == NULL) {
        *ppch = strchr(pszToken, '\0');
    } else {
        *pchBuffer = '\0';
        *ppch = pchBuffer + 1;
        while (strchr(pszDelim, **ppch))
            (*ppch)++;
    }

    return pszToken;
}


void lscp_parser_init ( lscp_parser_t *pParser, const char *pchBuffer, int cchBuffer )
{
    memset(pParser, 0, sizeof(lscp_parser_t));

    pParser->pchBuffer = (char *) malloc(cchBuffer + 1);
    if (pParser->pchBuffer) {
        memcpy(pParser->pchBuffer, pchBuffer, cchBuffer);
        pParser->pchBuffer[cchBuffer] = (char) 0;
        pParser->pszToken = lscp_parser_strtok(pParser->pchBuffer, " \t\r\n", &(pParser->pch));
    }

}


const char *lscp_parser_next ( lscp_parser_t *pParser )
{
    const char *pszToken = pParser->pszToken;

    if (pParser->pszToken)
        pParser->pszToken = lscp_parser_strtok(NULL, " \t\r\n", &(pParser->pch));

    return pszToken;
}

int lscp_parser_nextint ( lscp_parser_t *pParser )
{
    int ret = 0;

    if (pParser->pszToken) {
        ret = atoi(pParser->pszToken);
        lscp_parser_next(pParser);
    }

    return ret;
}

float lscp_parser_nextnum ( lscp_parser_t *pParser )
{
    float ret = 0;

    if (pParser->pszToken) {
        ret = (float) atof(pParser->pszToken);
        lscp_parser_next(pParser);
    }

    return ret;
}

int lscp_parser_test ( lscp_parser_t *pParser, const char *pszToken )
{
    int ret = (pParser->pszToken != NULL);
    if (ret)
        ret = (strcasecmp(pParser->pszToken, pszToken) == 0);
    if (ret)
        lscp_parser_next(pParser);

    return ret;
}

int lscp_parser_test2 ( lscp_parser_t *pParser, const char *pszToken, const char *pszToken2 )
{
    int ret = lscp_parser_test(pParser, pszToken);
    if (ret)
        ret = lscp_parser_test(pParser, pszToken2);

    return ret;
}

void lscp_parser_free ( lscp_parser_t *pParser )
{
    if (pParser->pchBuffer)
        free(pParser->pchBuffer);
    pParser->pchBuffer = NULL;
}


// end of parser.c
