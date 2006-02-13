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


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>

#include "pcf.h"
#include "message.h"

#define TPCF_DEFAULT_FORMAT     0
#define TPCF_INKBOUNDS          2
#define TPCF_ACCEL_W_INKBOUNDS  1
#define TPCF_COMPRESSED_METRICS 1

#define	TPCF_SEM_ERROR			(-1)
#define	TPCF_FILE_ERROR			(-2)
#define	TPCF_FILE_NOT_PCF		(-3)
#define	TPCF_FILE_NOT_TERMFONT		(-4)
#define	TPCF_MALLOC			(-5)
#define	TPCF_SUCCESS			0	/* Do not change this value. */


#define TPCF_PROPERTIES		(1 << 0)
#define TPCF_ACCELERATORS	(1 << 1)
#define TPCF_METRICS		(1 << 2)
#define TPCF_BITMAPS		(1 << 3)
#define TPCF_INK_METRICS	(1 << 4)
#define TPCF_BDF_ENCODINGS	(1 << 5)
#define TPCF_SWIDTHS		(1 << 6)
#define TPCF_GLYPH_NAMES	(1 << 7)
#define TPCF_BDF_ACCELERATORS	(1 << 8)

#define TPCF_INDEX_SIZE		(4+4+4+4)
#define TPCF_PROP_SIZE		(4+1+4)

static u_char read_u8(FILE* fp)
{
	u_char b;
	if (fread(&b, 1, 1, fp) != 1) die_file_eof(fp);
	return b;
}

static u_int read_u16l(FILE* fp)
{
	u_char b[2];
	if (fread(b, 2, 1, fp) != 1) die_file_eof(fp);
	return b[0]+(b[1]<<8);
}

static u_int read_u16b(FILE* fp)
{
	u_char b[2];
	if (fread(b, 2, 1, fp) != 1) die_file_eof(fp);
	return b[1]+(b[0]<<8);
}

static u_int read_u16(FILE* fp, int e)
{
	return (e&1) ? read_u16b(fp) : read_u16l(fp);
}

static int read_s16l(FILE* fp)
{
	u_int t = read_u16l(fp);
	if (t >= 0x8000) t |= ~0xffff;
	return (int)t;
}

static int read_s16b(FILE* fp)
{
	u_int t = read_u16b(fp);
	if (t >= 0x8000) t |= ~0xffff;
	return (int)t;
}

static int read_s16(FILE* fp, int e)
{
	return (e&1) ? read_s16b(fp) : read_s16l(fp);
}

static u_int read_u32l(FILE* fp)
{
	u_char b[4];
	if (fread(b, 4, 1, fp) != 1) die_file_eof(fp);
	return b[0]+(b[1]<<8)+(b[2]<<16)+(b[3]<<24);
}

static u_int read_u32b(FILE* fp)
{
	u_char b[4];
	if (fread(b, 4, 1, fp) != 1) die_file_eof(fp);
	return b[3]+(b[2]<<8)+(b[1]<<16)+(b[0]<<24);
}

static u_int read_u32(FILE* fp, int e)
{
	return (e&1) ? read_u32b(fp) : read_u32l(fp);
}

static int read_s32l(FILE* fp)
{
	u_char b[4];
	if (fread(b, 4, 1, fp) != 1) die_file_eof(fp);
	return b[0]+(b[1]<<8)+(b[2]<<16)+(b[3]<<24);
}

static int read_s32b(FILE* fp)
{
	u_char b[4];
	if (fread(b, 4, 1, fp) != 1) die_file_eof(fp);
	return b[3]+(b[2]<<8)+(b[1]<<16)+(b[0]<<24);
}

static int read_s32(FILE* fp, int e)
{
	return (e&1) ? read_s32b(fp) : read_s32l(fp);
}

static void skip_nbyte(FILE* fp, size_t len)
{
	size_t d;
	char dmy[1024];

	while (len) {
		d = len > 1024 ? 1024 : len;
		if (fread(dmy, d, 1, fp) != 1) die_file_eof(fp);
		len -= d;
	}
}

static size_t seek_section(FILE* fp, size_t cur, size_t tag)
{
	size_t n;
	size_t d;
	char dmy[1024];

	if (cur > tag) {
		die("(FONT): backward seeking\n");
	} else if (cur == tag) {
		return tag;
	}
	n = tag - cur;
	while (n) {
		d = n > 1024 ? 1024 : n;
		if (fread(dmy, d, 1, fp) != 1) die_file_eof(fp);
		n -= d;
	}
	return tag;
}


/*---------------------------------------------------------------------------*/
void tpcfformat_load(TPcfFormat* p, FILE* fp)
{
	u_int t = read_u32l(fp);
#if PCF_DEBUG
	printf("[[FORMAT: %x = ", t);
#endif
	p->id = (t >> 8) & 0xfff;
	p->scan = 1 << ((t >> 4) & 0x3);
	p->obit = (t >> 3) & 1;
	p->obyte = (t >> 2) & 1;
	p->glyphaline = t & 0x3;
#if PCF_DEBUG
	switch (p->id) {
	case TPCF_DEFAULT_FORMAT: printf("default "); break;
	case TPCF_INKBOUNDS: printf("inkbounds "); break;
	case TPCF_ACCEL_W_INKBOUNDS: printf("accel/ink|compr/metric "); break;
	default: printf("???"); break;
	}
	printf("scan:%d bytes ", p->scan);
	if (p->obit)
		printf("MSB ");
	else
		printf("LSB ");
	if (p->obyte)
		printf("BE ");
	else
		printf("LE ");
	printf("g:%d (%d bytes)", p->glyphaline, 1<<p->glyphaline);
	printf("]]\n");
#endif
}

/*---------------------------------------------------------------------------*/
static u_int to_u32l(u_char* p)
{
	return  p[0]+(p[1]<<8)+(p[2]<<16)+(p[3]<<24);
}
	
/*---------------------------------------------------------------------------*/
void tpcfindex_load(TPcfIndex* p, FILE* fp)
{
	p->type = read_u32l(fp);
	tpcfformat_load(&(p->fmt), fp);
	p->size = read_s32l(fp);
	p->offset = read_s32l(fp);
}

void tpcfindex_debug(TPcfIndex* p)
{
	printf("type=%u:", p->type);
	switch (p->type) {
	case TPCF_PROPERTIES: printf("prop "); break;
	case TPCF_ACCELERATORS: printf("accel "); break;
	case TPCF_METRICS: printf("metric "); break;
	case TPCF_BITMAPS: printf("bitmaps "); break;
	case TPCF_INK_METRICS: printf("inkmetric "); break;
	case TPCF_BDF_ENCODINGS: printf("bdfencodings "); break;
	case TPCF_SWIDTHS: printf("swidth "); break;
	case TPCF_GLYPH_NAMES: printf("glyphnames "); break;
	case TPCF_BDF_ACCELERATORS: printf("bdfaccel "); break;
	default: printf("???"); break;
	}
	printf(", size=%u, offset=%u",
	 	p->size, p->offset);
}

/*---------------------------------------------------------------------------*/
void tpcfprop_load(TPcfProp* p, FILE* fp, int e)
{
	p->name = read_s32(fp, e);
	p->strq = read_u8(fp);
	p->val = read_s32(fp, e);
}

/*---------------------------------------------------------------------------*/
void tpcfprops_init(TPcfProps* p) {
	p->nProp = 0;
	p->props = NULL;
	p->nSlen = 0;
	p->strings = NULL;
}

void tpcfprops_final(TPcfProps* p) {
	free(p->props);
	free(p->strings);
}

size_t tpcfprops_load(TPcfProps* p, FILE* fp)
{
	int i;
	int dmy;
	TPcfFormat fmt;
	tpcfformat_load(&fmt, fp);
	p->e = fmt.obit;
	p->nProp = read_s32(fp, p->e);
#if 0
	printf("Prop = %d\n", p->nProp);
#endif
	if (!(p->props = (TPcfProp*)malloc(sizeof(TPcfProp)*p->nProp))) {
		die("(FONT): malloc error\n");
	}
	for (i = 0 ; i < p->nProp ; i++) {
		tpcfprop_load(&(p->props[i]), fp, p->e);
	}
	dmy = 3 - (((TPCF_PROP_SIZE*p->nProp)+3)%4);
	for (i = 0 ; i < dmy ; i++) {
		read_u8(fp);
	}
	p->nSlen = read_s32(fp, p->e);
	if (!(p->strings = (u_char*)malloc(sizeof(u_char)*p->nSlen))) {
		die("(FONT): malloc error\n");
	}
	if (fread(p->strings, p->nSlen, 1, fp) != 1) die_file_eof(fp);

	
	return 8 +p->nProp*TPCF_PROP_SIZE +dmy +4 +p->nSlen;
}

void tpcfprops_debug(TPcfProps* p)
{
	int i;
	for (i = 0 ; i < p->nProp ; i++) {
		if (p->props[i].strq) {
			printf("%s = \"%s\"\n",
				p->strings + (p->props[i].name),	
				p->strings + (p->props[i].val));	
		} else {
			printf("%s = %d\n",
				p->strings + (p->props[i].name),	
				p->props[i].val);	
		}
	}
}

/*---------------------------------------------------------------------------*/
size_t tpcfmetric_load(TPcfMetric* p, FILE* fp, int e)
{
	p->leftsb = read_s16(fp, e);
	p->rightsb = read_s16(fp, e);
	p->width = read_s16(fp, e);
	p->ascent = read_s16(fp, e);
	p->descent = read_s16(fp, e);
	p->attr = read_s16(fp, e);
	return 2*6;
}

size_t tpcfcmetric_load(TPcfMetric* p, FILE* fp, int e)
{
	p->leftsb = read_u8(fp) - 0x80;
	p->rightsb = read_u8(fp) - 0x80;
	p->width = read_u8(fp) - 0x80;
	p->ascent = read_u8(fp) - 0x80;
	p->descent = read_u8(fp) - 0x80;
	p->attr = 0;
	return 1*5;
}

void tpcfmetric_debug(TPcfMetric* p)
{
	printf("[METRIC:%d<>%d(%d):%d^v%d:%d]",
		p->leftsb, p->rightsb, p->width,
		p->ascent, p->descent, p->attr);
}


void tpcfmetrics_init(TPcfMetrics* p) {
	p->nMetrics = 0;
	p->metric = NULL;
}

void tpcfmetrics_final(TPcfMetrics* p) {
	p->nMetrics = 0;
	free(p->metric);
}

size_t tpcfmetrics_load(TPcfMetrics* p, FILE* fp)
{
	TPcfFormat fmt;
	int i;
	tpcfformat_load(&fmt, fp);
	size_t r = 0;

	if (fmt.id == TPCF_DEFAULT_FORMAT) {
		p->nMetrics = read_s32(fp, fmt.obit);
#ifdef PCF_DEBUG
		printf("nmetric(default): %d\n", p->nMetrics);
#endif
		if ((p->metric = (TPcfMetric*)malloc(sizeof(TPcfMetric) * p->nMetrics)) == NULL) {
			die("(FONT): malloc error (metrics table)\n");
		}
		for (i = 0; i < p->nMetrics; i++) {
			r += tpcfmetric_load(&(p->metric[i]), fp, fmt.obit);
		}
		return 8 + r;
	} else if (fmt.id == TPCF_COMPRESSED_METRICS) {
		p->nMetrics = read_u16(fp, fmt.obit);
#ifdef PCF_DEBUG
		printf("nmetric(compressed): %d\n", p->nMetrics);
#endif
		if ((p->metric = (TPcfMetric*)malloc(sizeof(TPcfMetric) * p->nMetrics)) == NULL) {
			die("(FONT): malloc error (metrics table)\n");
		}
		for (i = 0; i < p->nMetrics; i++) {
			r += tpcfcmetric_load(&(p->metric[i]), fp, fmt.obit);
		}
		return 6 + r;
	}
	return 4;
}


/*---------------------------------------------------------------------------*/
size_t tpcfaccel_load(TPcfAccel* p, FILE* fp)
{
	TPcfFormat fmt;
	size_t r;

	tpcfformat_load(&fmt, fp);

	/* bool8: noOverlap */
	/* bool8: constantMetrics */
	skip_nbyte(fp, 2);

	/* bool8: terminalFont */
	p->termf = read_u8(fp);
#if 0 /* XXX: unifont is not terminal font */
	if (!p->termf) {
		die("(FONT) : Not terminal font %02x.\n", p->termf);
	}
#endif
	/* bool8: contantWidth */
	/* bool8: inkInside */
	/* bool8: inkMetrics */
	/* bool8: drawDirection */
	/* bool8: dummy (padding) */
	/* int32: fontAscent */
	/* int32: fontDescent */
	/* int32: maxOverlap */
	skip_nbyte(fp, 5+4*3);

	/* metric: minBounds */
	r = tpcfmetric_load(&(p->metric), fp, fmt.obit);

	/* metric: maxBounds */
	
	/* if format.id == PCF_ACCEL_W_INKBOUNDS */
	/*   metric: ink_minBounds */
	/*   metric: ink_maxBounds */
	/* endif */
	return 24+r;
}

void tpcfaccel_debug(TPcfAccel* p)
{
	printf("[ACCEL:");
	tpcfmetric_debug(&(p->metric));
	printf("]\n");
}

/*---------------------------------------------------------------------------*/
void tpcfbitmap_init(TPcfBitmap* p) {
	p->maps = 0;
	p->offsets = NULL;
	p->bitmap = NULL;
}

void tpcfbitmap_final(TPcfBitmap* p) {
	p->maps = 0;
	free(p->offsets);
	free(p->bitmap);
}

void tpcfbitmap_swap(TPcfBitmap* p, int e, int swp, int aline)
{
	u_int len = p->mapsize[aline];
	u_int i;
	u_char* bp = p->bitmap;
	u_char c;
	u_char d;

	if (!e) {	/* LSB <==> MSB */
		for (i = 0 ; i < len ; i++) {
			c = *bp;
			d  = (c & 0x01) ? 0x80 : 0x00;
			d |= (c & 0x02) ? 0x40 : 0x00;
			d |= (c & 0x04) ? 0x20 : 0x00;
			d |= (c & 0x08) ? 0x10 : 0x00;
			d |= (c & 0x10) ? 0x08 : 0x00;
			d |= (c & 0x20) ? 0x04 : 0x00;
			d |= (c & 0x40) ? 0x02 : 0x00;
			d |= (c & 0x80) ? 0x01 : 0x00;
			*bp++ = d;
		}	
	}
	
}

size_t tpcfbitmap_load(TPcfBitmap* p, FILE* fp)
{
	TPcfFormat fmt;
	size_t r;
	int i;

	tpcfformat_load(&fmt, fp);
	p->aline = 1 << fmt.glyphaline;
	p->galine = fmt.glyphaline;

	p->maps = read_s32(fp, fmt.obit);
	if (!(p->offsets = (u_int*)malloc(sizeof(u_int) * p->maps))) {
		die("(FONT): malloc error (offset table)\n");
	}
	for (i = 0 ; i < p->maps ; i++) {
		p->offsets[i] = read_u32(fp, fmt.obit);
	}
	for (i = 0 ; i < 4 ; i++) {
		p->mapsize[i] = read_u32(fp, fmt.obit);
	}
	r = p->mapsize[fmt.glyphaline];
#if PCF_DEBUG
	printf("BITMAP SIZE : %d bytes - %d (%d bytes/line)\n", r, fmt.glyphaline, 
	       p->aline);
#endif
	if (!(p->bitmap = (u_char*)malloc(r))) {
		die("(FONT): malloc error(bitmap)\n");
	}
	if (fread(p->bitmap, r, 1, fp) != 1) die_file_eof(fp);
	
	tpcfbitmap_swap(p, fmt.obit, fmt.obyte, fmt.glyphaline);

	return 8+p->maps*4+4*4+r;
}

/*---------------------------------------------------------------------------*/
void tpcfencode_init(TPcfEncode* p) {
	p->table = NULL;
}

void tpcfencode_final(TPcfEncode* p) {
	free(p->table);
}

size_t tpcfencode_load(TPcfEncode* p, FILE* fp)
{
	TPcfFormat fmt;
	size_t r;
	int i;

	tpcfformat_load(&fmt, fp);

	p->colf = read_s16(fp, fmt.obit);
	p->coll = read_s16(fp, fmt.obit);
	p->rowf = read_s16(fp, fmt.obit);
	p->rowl = read_s16(fp, fmt.obit);
	p->defa = read_s16(fp, fmt.obit);

	r = (p->coll - p->colf +1) * (p->rowl  - p->rowf +1);
	if (!(p->table = (u_int*)malloc(sizeof(int) * r))) {
		die("(FONT): malloc error\n");
	}
	for (i = 0 ; i < r ; i++) {
		p->table[i] = read_u16(fp, fmt.obit);
	}
	
	return 14+2*r;
}

void tpcfencode_debug(TPcfEncode* p)
{
	printf("ENCODE[");
	printf("col:%d-%d[%x-%x]", p->colf, p->coll, p->colf, p->coll);
	printf("row:%d-%d[%x-%x]", p->rowf, p->rowl, p->rowf, p->rowl);
	printf("defch: %d ", p->defa);
	printf("n = %d]\n", (p->coll - p->colf + 1) * (p->rowl - p->rowf + 1));
}

/*---------------------------------------------------------------------------*/
void tpcf_init(TPcf* p) {
	p->nIdx = 0;
	p->idxs = NULL;
	tpcfprops_init(&(p->props));
	tpcfmetrics_init(&(p->metrics));
	tpcfbitmap_init(&(p->bitmap));
	tpcfencode_init(&(p->encode));
}

void tpcf_final(TPcf* p) {
	free(p->idxs);
	tpcfprops_final(&(p->props));
	tpcfmetrics_final(&(p->metrics));
	tpcfbitmap_final(&(p->bitmap));
	tpcfencode_final(&(p->encode));
}

static TPcfIndex* tpcf_search_section(
	TPcf* p,
	u_int type)
{
	int i;
	for (i = 0 ; i < p->nIdx ;  i++) {
		if (type == p->idxs[i].type) {
			return &(p->idxs[i]);
		}
	}
	return NULL;
}

void tpcf_load(TPcf* p, FILE* fp)
{
	char magic[] = {1, 'f', 'c', 'p'};
	/* --- */
	size_t	readp = 0;
	TPcfIndex* q;
	TPcfIndex* qt;
	int i;
	
	/* read header and prepare index-area */
	if (read_u32l(fp) != to_u32l(magic)) {
		die("(FONT): Specified file is not pcf\n");
	}
	p->nIdx = read_u32l(fp);
	if (!(p->idxs = (TPcfIndex*)malloc(sizeof(TPcfIndex)*p->nIdx))) {
		die("(FONT): malloc error\n");
	}
	readp += 8;

	/* read index */
	for (i = 0 ; i < p->nIdx ; i++) {
		tpcfindex_load(&(p->idxs[i]), fp);
#if PCF_DEBUG
		printf("SECT %d :", i);
		tpcfindex_debug(&(p->idxs[i]));
		printf("\n");
#endif
		readp += TPCF_INDEX_SIZE;
	}
#if PCF_DEBUG
	printf("total gain = %d\n", readp);
#endif

	/* read properties */
	q = tpcf_search_section(p, TPCF_PROPERTIES);
	if (!q) die("(FONT): Properties Section not exist.\n");

	readp = seek_section(fp, readp, q->offset);
#if PCF_DEBUG
	printf("total gain = %d\n", readp);
	printf("load PROPS\n");
#endif
	readp += tpcfprops_load(&(p->props), fp);
#if PCF_DEBUG
	tpcfprops_debug(&(p->props));
	printf("total gain = %d\n", readp);
#endif

	/* read accelerator */
	q = tpcf_search_section(p, TPCF_ACCELERATORS);
	qt = tpcf_search_section(p, TPCF_BDF_ACCELERATORS);
	if (!qt) {
		if (!q) die("(FONT): Accelerators Sections not exist.\n");
#if PCF_DEBUG
		printf("total gain = %d:%ld\n", readp, ftell(fp));
#endif
		readp = seek_section(fp, readp, q->offset);
#if PCF_DEBUG
		printf("total gain = %d:%ld\n", readp, ftell(fp));
		printf("load ACCEL\n");
#endif
		readp += tpcfaccel_load(&(p->accel), fp);
#if PCF_DEBUG
		tpcfaccel_debug(&(p->accel));
		printf("total gain = %d\n", readp);
#endif
	}
	/* read metric: for multiwidth font (unifont) ? */
	q = tpcf_search_section(p, TPCF_METRICS);
	if (!q) die("(FONT): Metric Section not exist.\n");
	readp = seek_section(fp, readp, q->offset);
#if PCF_DEBUG
	printf("total gain = %d\n", readp);
	printf("load METRIC\n");
#endif
	readp += tpcfmetrics_load(&(p->metrics), fp);
#if PCF_DEBUG
	printf("total gain = %d\n", readp);
#endif

	/* read bitmap */
	q = tpcf_search_section(p, TPCF_BITMAPS);
	if (!q) die("(FONT): Bitmap Section not exist.\n");
	readp = seek_section(fp, readp, q->offset);
#if PCF_DEBUG
	printf("total gain = %d\n", readp);
	printf("load BITMAP\n");
#endif
	readp += tpcfbitmap_load(&(p->bitmap), fp);
#if PCF_DEBUG
	printf("total gain = %d\n", readp);
#endif

	/* read encoding */
	q = tpcf_search_section(p, TPCF_BDF_ENCODINGS);
	if (!q) die("(FONT): Encoding Section not exist.\n");
	readp = seek_section(fp, readp, q->offset);
#if PCF_DEBUG
	printf("total gain = %d:%ld\n", readp, ftell(fp));
	printf("load ENCODE\n");
#endif
	readp += tpcfencode_load(&(p->encode), fp);
#if PCF_DEBUG
	tpcfencode_debug(&(p->encode));
	printf("total gain = %d:%ld\n", readp, ftell(fp));
#endif

	/* read bdf-accelerator */
	q = tpcf_search_section(p, TPCF_BDF_ACCELERATORS);
	if (!q) die("(FONT): Bdf_accelerators Section not exist.\n");
	readp = seek_section(fp, readp, q->offset);
#if PCF_DEBUG
	printf("total gain = %d\n", readp);
	printf("load BDF_ACCEL\n");
#endif
	readp += tpcfaccel_load(&(p->accel), fp);
#if PCF_DEBUG
	tpcfaccel_debug(&(p->accel));
	printf("total gain = %d\n", readp);
#endif
}
	
	
void tpcf_as_tfont(TPcf* p, TFont* q)
{
	int offset;
	int gs;
	int bs;
	int i;
	int ii;

	q->width = p->accel.metric.rightsb - p->accel.metric.leftsb;
	q->height = p->accel.metric.ascent + p->accel.metric.descent;

	bs = p->bitmap.mapsize[p->bitmap.galine];
	if (!(q->bitmap = (u_char*)malloc(bs))) {
		die("(FONT): malloc error(bitmap)\n");
	}
	memcpy(q->bitmap, p->bitmap.bitmap, bs);

	q->bytew = p->bitmap.aline;
	q->bytec = q->bytew * q->height;
	q->colf = p->encode.colf; 
	q->coll = p->encode.coll;
	q->rowf = p->encode.rowf;
	q->rowl = p->encode.rowl;
	q->colspan = q->coll-q->colf+1;

	if (!(q->dglyph = (u_char*)malloc(q->bytec))) {
		die("(FONT): malloc error(default glyph)\n");
	}
	memset(q->dglyph, 0xff, q->bytec);

	gs = (q->coll - q->colf +1) * (q->rowl  - q->rowf +1);
	if (!(q->glyph = (u_char**)malloc(sizeof(u_char*) * gs))) {
		die("(FONT): malloc error (glyph)\n");
	}
	q->glyph_width = NULL;
	if (p->accel.termf == 0) {
		/* not terminal font */
		q->glyph_width = (u_int*)malloc(sizeof(u_int)*gs);
		if (q->glyph_width == NULL) {
			die("(FONT): malloc error (glyph_width)\n");
		}
	}
	for (i = 0 ; i < gs ; i++) {
		ii = p->encode.table[i];
		if (ii == 0xffff) {
			q->glyph[i] = q->dglyph;
			if (q->glyph_width)
				q->glyph_width[i] = q->width; /* XXX */
		} else {
			offset = p->bitmap.offsets[ii];
			q->glyph[i] = q->bitmap + offset;
			if (q->glyph_width) {
				TPcfMetric *m;
				m = &p->metrics.metric[ii];
				q->glyph_width[i] = m->rightsb - m->leftsb;
			}
		}
	}
}

const char* tpcf_get_string_prop(TPcf* p, const char* key)
{
	int i;
	const char* strb = p->props.strings;  

	for (i = 0 ; i < p->props.nProp ; i++) {
		if (p->props.props[i].strq &&
		    strcmp(strb + (p->props.props[i].name), key) == 0) {
			return strb + (p->props.props[i].val);	
		}
	}
	return NULL;
}

#if PCF_DEBUG
#include "main.h"
TApplication gApp;

/* cc -I. -o p -DPCF_DEBUG=1 pcf.c message.o */

void
dump_bits(int ch)
{
	int b;
	/* printf("%02x:", ch); */
	for (b = 7; b >= 0; --b) {
		if ((ch & (1<<b)) != 0) {
			printf("*");
		} else {
			printf("-");
		}
	}
	printf(" ");
}

int
main(int argc, char *argv[])
{
    FILE *fp;
    char buf[512];
    TPcf tpcf;
    if (argc < 2) {
	    fprintf(stderr, "usage: %s <fontfile> [idx]\n", argv[0]);
	    exit(1);
    }
    sprintf(buf, "gunzip < %s", argv[1]);
    fp = popen(buf, "r");
    
    tpcf_load(&tpcf, fp);
    if (argc > 2) {
	    int idx = strtol(argv[2], NULL, 0);
	    int eidx;
	    int offset;
	    char *p;
	    int i;
	    int height, width;
	    printf("idx:%d (0x%04x) => ", idx, idx);
	    printf("colspan=%d ", 
		    	(tpcf.encode.coll - tpcf.encode.colf + 1));
	    eidx = ((idx >> 8) - tpcf.encode.rowf) * 
		    	(tpcf.encode.coll - tpcf.encode.colf + 1)
			+ (idx & 0x0ff) - tpcf.encode.colf;
	    printf("=> %d (0x%04x)\n", eidx, eidx);
	    offset = tpcf.encode.table[eidx];
	    printf("encode.offset:%d (0x%04x)\n", offset, offset);
	    if (offset == 0xffff)
		    printf("no glyph\n");
	    else {
		    TPcfMetric *m;
		    int mheight, mwidth;
		    if (offset >= tpcf.metrics.nMetrics) {
			    printf("out of error? metric: %d > %d\n",
				   offset, tpcf.metrics.nMetrics);
			    exit(1);
		    }
		    m = &tpcf.metrics.metric[offset];
		    offset = tpcf.bitmap.offsets[offset];
		    printf("bitmap.offset:%d\n", offset);
		    p = &tpcf.bitmap.bitmap[offset];
		    height = tpcf.accel.metric.ascent + tpcf.accel.metric.descent;
		    width = tpcf.accel.metric.rightsb - tpcf.accel.metric.leftsb;
		    mheight = m->ascent + m->descent;
		    mwidth = m->rightsb - m->leftsb;
		    printf("(h:%d * w:%d) h:%d * w:%d \n", 
			   height, width, mheight, mwidth);
		    for (i = 0; i < height; i++) {
			    int j;
			    for (j = 0; j < tpcf.bitmap.aline; j++) {
				    dump_bits(*p & 0x0ff); p++;
			    }
			    printf("\n");
		    }
	    }
    }
    exit(0);
    
}
#endif
