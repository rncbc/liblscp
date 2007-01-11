// parser.h
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

#ifndef __LSCP_PARSER_H
#define __LSCP_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if (defined(_WIN32) || defined(__WIN32__))
#if (!defined(WIN32))
#define WIN32
#endif
#endif

#if defined(__cplusplus)
extern "C" {
#endif

//-------------------------------------------------------------------------
// Simple token parser.

typedef struct _lscp_parser_t
{
	char       *pchBuffer;
	int         cchBuffer;
	const char *pszToken;
	char       *pch;

} lscp_parser_t;

const char *lscp_parser_strtok  (char *pchBuffer, const char *pszDelim, char **ppch);

void        lscp_parser_init    (lscp_parser_t *pParser, const char *pchBuffer, int cchBuffer);
const char *lscp_parser_next    (lscp_parser_t *pParser);
int         lscp_parser_nextint (lscp_parser_t *pParser);
float       lscp_parser_nextnum (lscp_parser_t *pParser);
int         lscp_parser_test    (lscp_parser_t *pParser, const char *pszToken);
int         lscp_parser_test2   (lscp_parser_t *pParser, const char *pszToken, const char *pszToken2);
void        lscp_parser_free    (lscp_parser_t *pParser);

#if defined(__cplusplus)
}
#endif

#endif // __LSCP_PARSER_H

// end of parser.h
