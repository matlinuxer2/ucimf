/*
 * JFBTERM -
 * Copyright (C) 1999 Noritoshi MASUICHI (nmasu@ma3.justnet.ne.jp)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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
/*
 * This code is based on the following :
 * 
 * KON2 - Kanji ON Console -
 * Copyright (C) 1992-1996 Takashi MANABE (manabe@papilio.tutics.tut.ac.jp)
 */

#ifndef INCLUDE_GETCAP_H
#define INCLUDE_GETCAP_H

typedef struct Raw_TCapValue {
	struct Raw_TCapValue* next;
	char *value;			 /* Value for this capability. */
} TCapValue;

typedef struct Raw_TCapability {
	struct Raw_TCapability* next;
	char *name;			 /* Name of capability label */
	TCapValue *values;		 /* Value for this capability. */
} TCapability;

typedef struct Raw_TCaps {
	struct Raw_TCapability* next;
} TCaps;

void tcapability_init(TCapability* p);
void tcapability_final(TCapability* p);
void tcapability_set(TCapability* p, const char* name, const char* vale);
void tcaps_init(TCaps* p);
void tcaps_final(TCaps* p);
TCapability* tcaps_find(TCaps* p, const char* name);
char* tcaps_find_first(TCaps* p, const char* name);
char *tcaps_find_entry(TCaps* p, const char* prefix, const char *name);
void tcaps_read(TCaps* p, const char *filename);
void tcaps_read_args(TCaps* p, int argc, char *argv[]);

#endif /* INCLUDE_GETCAP_H */
