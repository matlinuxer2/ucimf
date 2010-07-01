/*
 * JFBTERM -
 * Copyright (C) 1999  Noritoshi MASUICHI (nmasu@ma3.justnet.ne.jp)
 *
 * KON2 - Kanji ON Console -
 * Copyright (C) 1992-1996 Takashi MANABE (manabe@papilio.tutics.tut.ac.jp)
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
 * THIS SOFTWARE IS PROVIDED BY TAKASHI MANABE ``AS IS'' AND ANY
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

#include	<stdlib.h>
#include	<sys/types.h>

#include	"pen.h"

void tpen_init(TPen* p)
{
	p->prev = NULL;
	p->x = p->y = 0;
	p->fcol = 7;
	p->bcol = 0;
	p->attr = 0;
}

void tpen_final(TPen* p)
{
	TPen* q = p->prev;
	p->prev = NULL;
	if (q) {
		tpen_final(q);
		free(q);
	}
}

void tpen_copy(TPen* p, TPen* q)
{
	p->prev = NULL;
	p->x = q->x;
	p->y = q->y;
	p->fcol = q->fcol;
	p->bcol = q->bcol;
	p->attr = q->attr;
}

inline void tpen_off_all_attribute(TPen* p)
{
	p->bcol = 0;
	p->fcol = 7;
	p->attr = 0;
}

void tpen_higlight(TPen* p)
{
	p->attr |= ATTR_HIGH;
	if (p->fcol) p->fcol |= 8;
}

void tpen_dehiglight(TPen* p)
{
	p->attr &= ~ATTR_HIGH;
	p->fcol &= ~8;
}

void tpen_underline(TPen* p)
{
	p->attr |= ATTR_ULINE;
	p->bcol |= 8;
}

void tpen_no_underline(TPen* p)
{
	p->attr &= ~ATTR_ULINE;
	p->bcol &= ~8;
}

static u_char gsTPenReversTable[] = {0, 4, 2, 6, 1, 5, 3, 7};

void tpen_reverse(TPen* p)
{
	u_char swp;

	if (!(p->attr & ATTR_REVERSE)) {
		p->attr |= ATTR_REVERSE;
		swp = p->fcol & 7;
		if (p->attr & ATTR_ULINE) swp |= 8;
		p->fcol = p->bcol & 7;
		if (p->attr & ATTR_HIGH && p->fcol) p->fcol |= 8;
		p->bcol = swp;
	}
}

void tpen_no_reverse(TPen* p)
{
	u_char swp;

	if (p->attr & ATTR_REVERSE) {
		p->attr &= ~ATTR_REVERSE;
		swp = p->fcol & 7;
		if (p->attr & ATTR_ULINE) swp |= 8;
		p->fcol = p->bcol & 7;
		if (p->attr & ATTR_HIGH && p->fcol) p->fcol |= 8;
		p->bcol = swp;
	}
}

void tpen_set_color(TPen* p, int col)
{
	u_char t;

	if (col >= 30 && col <= 37) {
		t = gsTPenReversTable[col - 30];
		if (p->attr & ATTR_REVERSE) {
			if (p->attr & ATTR_ULINE) t |= 8;
			p->bcol = t;
		} else {
			if (p->attr & ATTR_HIGH) t |= 8;
			p->fcol = t;
		}
	} else if (col >= 40 && col <= 47) {
		t = gsTPenReversTable[col - 40];
		if (p->attr & ATTR_REVERSE) {
			if (p->attr & ATTR_HIGH) t |= 8;
			p->fcol = t;
		} else {
			if (p->attr & ATTR_ULINE) t |= 8;
			p->bcol = t;
		}
	}
}

