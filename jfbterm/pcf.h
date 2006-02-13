/*
 * JFBTERM -
 * Copyright (c) 2003 Fumitoshi UKAI <ukai@debian.or.jp>
 * Copyright (C) 1999  Noritoshi MASUICHI (nmasu@ma3.justnet.ne.jp)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NORITOSHI MASUICHI ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NORITOSHI MASUICHI BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


#ifndef INCLUDE_PCF_H
#define INCLUDE_PCF_H

#include <sys/types.h>
#include <font.h>

typedef struct Raw_TPcfFormat {
	u_int id;
	u_int scan;
	int obit;
	int obyte;
	u_int glyphaline;
} TPcfFormat;

typedef struct Raw_TPcfIndex {
	u_int type;
	TPcfFormat fmt;
	int size;
	int offset;
} TPcfIndex;

typedef struct Raw_TPcfProp {
	int name;
	u_char strq;
	int val;
} TPcfProp;

typedef struct Raw_TPcfProps {
	int e;
	int nProp;
	TPcfProp* props; 
	int nSlen;
	char* strings;
} TPcfProps;

typedef struct Raw_TPcfMetric {
	int leftsb;
	int rightsb;
	int width;
	int ascent;
	int descent;
	int attr;
} TPcfMetric;

typedef struct Raw_TPcfMetrics {
	int nMetrics;
	TPcfMetric *metric;
} TPcfMetrics;

typedef struct Raw_TPcfAccel {
	int termf;
	TPcfMetric metric;
} TPcfAccel;

typedef struct Raw_TPcfBitmap {
	int maps;
	u_int aline;
	u_int galine;
	u_int mapsize[4];
	u_int* offsets;
	u_char* bitmap;
} TPcfBitmap;

typedef struct Raw_TPcfEncode {
	int colf;
	int coll;
	int rowf;
	int rowl;
	int defa;
	int* table;
} TPcfEncode;

typedef struct Raw_TPcf {
	u_int nIdx;
	TPcfIndex* idxs; 
	TPcfProps props;
	TPcfMetrics metrics;
	TPcfAccel accel;
	TPcfBitmap bitmap;
	TPcfEncode encode;
} TPcf;

void tpcf_init(TPcf* p);
void tpcf_final(TPcf* p);
void tpcf_load(TPcf* p, FILE* fp);
void tpcf_as_tfont(TPcf* p, TFont* q);
const char* tpcf_get_string_prop(TPcf* p, const char* key);
	
#endif /* INCLUDE_PCF_H */
