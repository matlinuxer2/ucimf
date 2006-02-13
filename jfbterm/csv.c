/*
 * JFBTERM -
 * Copyright (c) 1999 Noritoshi Masuichi (nmasu@ma3.justnet.ne.jp)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in the
 *	documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NORITOSHI MASUICHI ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE TERRENCE R. LAMBERT BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <string.h>
#include "csv.h"
#include "util.h"

void tcsv_init(TCsv* p, const char* s)
{
	char* cp;
	char* cq;

	p->buffer = strdup(s);
	p->pnt = p->buffer;
	p->cap = 0;
	p->count = 0;

	if (!(p->buffer)) {
		return;
	}

	p->cap = 1;
	for (cp = p->buffer , cq = p->buffer ; *cp ; cp++) {
		switch (*cp) {
		case ',':
			*cq++ = '\0';
			p->cap++;
			break;
		default:
			*cq++ = *cp;
			break;
		}
	}
	*cq = '\0';
	
	return;
}

void tcsv_final(TCsv* p)
{
	util_free(p->buffer);
}

const char* tcsv_get_token(TCsv* p)
{
	char* r;

	if (p->count >= p->cap) {
		return NULL;
	}

	for (r = p->pnt ; *(p->pnt) ; p->pnt++) {
		/* no body */
	}
	p->pnt++;
	p->count++;
	return r;
}
