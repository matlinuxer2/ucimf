/*
 * JFBTERM - 
 * Copyright (c) 2003 Fumitoshi UKAI <ukai@debian.or.jp>
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

#include "font.h"
#include "pcf.h"
#include "util.h"
#include "message.h"
#include "csv.h"
#include "picofont.h"
#include "sequence.h"

u_int gFontsWidth = 0;
u_int gFontsHeight = 0;

u_char sgFontsDefaultGlyph[] = {0x80};
u_char sgFontsDefaultDGlyph[] = {0xC0};

#define FMACRO_94__FONT(final, aline, fontname) {\
	.conv = tfont_default_glyph, \
	.name = fontname, \
	.width = 1, \
	.height = 1, \
	.fsignature = TFONT_FT_SINGLE | TFONT_FT_94CHAR| final,\
	.fhalf = aline, \
	.aliasF = TFONT_ALIAS, \
	.glyph = NULL, \
	.glyph_width = NULL, \
	.dglyph = sgFontsDefaultGlyph, \
	.bitmap = NULL,\
	.colf = 0xFF, \
	.coll = 0x00, \
	.rowf = 0xFF, \
	.rowl = 0x00, \
	.colspan = 0x00, \
	.bytew = 2, \
	.bytec = 1 \
}

#define FMACRO_96__FONT(final, aline, fontname) {\
	.conv = tfont_default_glyph, \
	.name = fontname, \
	.width = 1, \
	.height = 1, \
	.fsignature = TFONT_FT_SINGLE | TFONT_FT_96CHAR| final,\
	.fhalf = aline, \
	.aliasF = TFONT_ALIAS, \
	.glyph = NULL, \
	.glyph_width = NULL, \
	.dglyph = sgFontsDefaultGlyph, \
	.bitmap = NULL,\
	.colf = 0xFF, \
	.coll = 0x00, \
	.rowf = 0xFF, \
	.rowl = 0x00, \
	.colspan = 0x00, \
	.bytew = 2, \
	.bytec = 1 \
}

#define FMACRO_94N_FONT(final, aline, fontname) {\
	.conv = tfont_default_glyph, \
	.name = fontname, \
	.width = 2, \
	.height = 1, \
	.fsignature = TFONT_FT_DOUBLE | TFONT_FT_94CHAR| final,\
	.fhalf = aline, \
	.aliasF = TFONT_ALIAS, \
	.glyph = NULL, \
	.glyph_width = NULL, \
	.dglyph = sgFontsDefaultDGlyph, \
	.bitmap = NULL,\
	.colf = 0xFF, \
	.coll = 0x00, \
	.rowf = 0xFF, \
	.rowl = 0x00, \
	.colspan = 0x00, \
	.bytew = 2, \
	.bytec = 2 \
}

TFont gFont[] = {
	FMACRO_94__FONT(0x40, FH_LEFT , "iso646-1973irv"),

	/* CJK Character sets */
	FMACRO_94N_FONT(0x40, FH_LEFT , "jisc6226-1978"),
	FMACRO_94N_FONT(0x41, FH_LEFT , "gb2312-80"),
	FMACRO_94N_FONT(0x42, FH_LEFT , "jisx0208-1983"),
#if 0
	FMACRO_94N_FONT(0x42, FH_LEFT , "jisx0208-1990"),
#endif
	FMACRO_94N_FONT(0x43, FH_LEFT , "ksc5601-1987"),
	FMACRO_94N_FONT(0x44, FH_LEFT , "jisx0212-1990"),
	FMACRO_94N_FONT(0x45, FH_LEFT , "ccitt-extended-gb" /*aka iso-ir-165*/),
	FMACRO_94N_FONT(0x47, FH_LEFT , "cns11643-1992.1"),
	FMACRO_94N_FONT(0x48, FH_LEFT , "cns11643-1992.2"),
	FMACRO_94N_FONT(0x49, FH_LEFT , "cns11643-1992.3"),
	FMACRO_94N_FONT(0x4a, FH_LEFT , "cns11643-1992.4"),
	FMACRO_94N_FONT(0x4b, FH_LEFT , "cns11643-1992.5"),
	FMACRO_94N_FONT(0x4c, FH_LEFT , "cns11643-1992.6"),
	FMACRO_94N_FONT(0x4d, FH_LEFT , "cns11643-1992.7"),
	/* Domestic ISO 646 Character sets */
	FMACRO_94__FONT(0x41, FH_LEFT , "bs4730"),
	FMACRO_94__FONT(0x42, FH_LEFT , "ansix3.4-1968" /* aka ASCII */),
	FMACRO_94__FONT(0x43, FH_LEFT , "nats1-finland-sweden"),
	FMACRO_94__FONT(0x47, FH_LEFT , "sen850200b"),
	FMACRO_94__FONT(0x48, FH_LEFT , "sen850200c"),
	FMACRO_94__FONT(0x4a, FH_LEFT , "jisc6220-1969roman"),
	FMACRO_94__FONT(0x59, FH_LEFT , "uni0204-70"),
	FMACRO_94__FONT(0x4c, FH_LEFT , "olivetti-portuguese"),
	FMACRO_94__FONT(0x5a, FH_LEFT , "olivetti-spanish"),
	FMACRO_94__FONT(0x4b, FH_LEFT , "din66003"),
	FMACRO_94__FONT(0x52, FH_LEFT , "nfz62-010-1973"),
	FMACRO_94__FONT(0x54, FH_LEFT , "gb1988-80"),
	FMACRO_94__FONT(0x60, FH_LEFT , "ns4551-1"),
	FMACRO_94__FONT(0x61, FH_LEFT , "ns4551-2"),
	FMACRO_94__FONT(0x66, FH_LEFT , "nfz62-010-1982"),
	FMACRO_94__FONT(0x67, FH_LEFT , "ibm-portuguese"),
	FMACRO_94__FONT(0x68, FH_LEFT , "ibm-spanish"),
	FMACRO_94__FONT(0x69, FH_LEFT , "msz7795.3"),
	FMACRO_94__FONT(0x6e, FH_LEFT , "jisc6229-1984ocr-b"),
	FMACRO_94__FONT(0x75, FH_LEFT , "ccitt-t.61-1"),
	FMACRO_94__FONT(0x77, FH_LEFT , "csaz243.4-1985alt1-1"),
	FMACRO_94__FONT(0x78, FH_LEFT , "csaz243.4-1985alt1-2"),
	FMACRO_94__FONT(0x7a, FH_LEFT , "jusi.b1.002"),
#if 0
	/* Domestic ISO 646 Character sets with I-oct */
	FMACRO_94__FONT(0x41, FH_LEFT , "nc99-10:81"),
	FMACRO_94__FONT(0x42, FH_LEFT , "iso646-1992invariants"),
#endif
	/* Quasi ISO 646 Character sets */
	FMACRO_94__FONT(0x45, FH_LEFT , "nats1-denmark-norway"),
	FMACRO_94__FONT(0x55, FH_LEFT , "honeywell-bull-latin-greek"),
	FMACRO_94__FONT(0x56, FH_LEFT , "british-post-office-teletext"),
	FMACRO_94__FONT(0x57, FH_LEFT , "inis-irv-subset"),
	/* Greek Character sets */
	FMACRO_94__FONT(0x5b, FH_LEFT , "olivetti-greek"),
	FMACRO_94__FONT(0x5c, FH_LEFT , "olivetti-latin-greek"),
	FMACRO_94__FONT(0x58, FH_LEFT , "iso5428-1974"),
	FMACRO_94__FONT(0x53, FH_LEFT , "iso5428-1980"),
#if 0
	/* Greek Character sets  with I-oct */
	FMACRO_94__FONT(0x40, FH_LEFT , "ccitt-greek1"),
#endif
	/* Cyrillic Character sets */
	FMACRO_94__FONT(0x4e, FH_LEFT , "iso5427-1981"),
	FMACRO_94__FONT(0x5e, FH_LEFT , "inis-cyrillic-extension"),
	FMACRO_94__FONT(0x51, FH_LEFT , "iso5427-1981extension"),
	FMACRO_96__FONT(0x40, FH_LEFT , "ecma-94cyrillic"),
	FMACRO_94__FONT(0x7b, FH_LEFT , "jusi.b1.003"),
	FMACRO_94__FONT(0x7d, FH_LEFT , "jusi.b1.004"),
	FMACRO_96__FONT(0x4f, FH_LEFT , "stsev358-88"),
	/* ISO 8859 variants */
	FMACRO_96__FONT(0x41, FH_RIGHT, "iso8859.1-1987"),
	FMACRO_96__FONT(0x42, FH_RIGHT, "iso8859.2-1987"),
	FMACRO_96__FONT(0x43, FH_RIGHT, "iso8859.3-1988"),
	FMACRO_96__FONT(0x44, FH_RIGHT, "iso8859.4-1988"),
	FMACRO_96__FONT(0x4c, FH_RIGHT, "iso8859.5-1988"),
	FMACRO_96__FONT(0x47, FH_RIGHT, "iso8859.6-1987"),
	FMACRO_96__FONT(0x46, FH_RIGHT, "iso8859.7-1987"),
	FMACRO_96__FONT(0x48, FH_RIGHT, "iso8859.8-1988"),
	FMACRO_96__FONT(0x4d, FH_RIGHT, "iso8859.9-1989"),
	FMACRO_96__FONT(0x56, FH_RIGHT, "iso8859.10-1992"),
	FMACRO_96__FONT(0x54, FH_RIGHT, "iso8859.11-2001"),
	FMACRO_96__FONT(0x59, FH_RIGHT, "iso8859.13-1998"),
	FMACRO_96__FONT(0x5f, FH_RIGHT, "iso8859.14-1998"),
	FMACRO_96__FONT(0x62, FH_RIGHT, "iso8859.15-1999"),
	FMACRO_96__FONT(0x66, FH_RIGHT, "iso8859.16-2001"),
	/* CCITT T.101 Mosaic Charactor Sets */
	FMACRO_94__FONT(0x79, FH_LEFT , "ccitt-mosaic-1"),
	FMACRO_96__FONT(0x7d, FH_LEFT , "ccitt-mosaic-sup1"),
	FMACRO_94__FONT(0x63, FH_LEFT , "ccitt-mosaic-sup2"),
	FMACRO_94__FONT(0x64, FH_LEFT , "ccitt-mosaic-sup3"),
	/* Japano */
	FMACRO_94__FONT(0x49, FH_LEFT , "jisc6220-1969kana"),
	/* JISX0213 */
	FMACRO_94N_FONT(0x4f, FH_LEFT , "jisx0213-2000-1"),
	FMACRO_94N_FONT(0x50, FH_LEFT , "jisx0213-2000-2"),
	/* Linux Private */
	FMACRO_94__FONT(0x30, FH_LEFT , "vt100-graphics"),
#ifdef JFB_UTF8
	{
	    .conv = tfont_default_glyph,
	    .name = "iso10646.1",  /* Unicode 2.0 */
	    .width = 1, 
	    .height = 1,
	    .fsignature = TFONT_FT_OTHER,
	    .fhalf = FH_UNI, 
	    .aliasF = TFONT_ALIAS, 
	    .glyph = NULL, 
	    .glyph_width = NULL,
	    .dglyph = sgFontsDefaultDGlyph, 
	    .bitmap = NULL,
	    .colf = 0xFF, 
	    .coll = 0x00, 
	    .rowf = 0xFF, 
	    .rowl = 0x00, 
	    .colspan = 0x00, 
	    .bytew = 1, 
	    .bytec = 1
	},
#endif
	{
	    .conv = tfont_default_glyph,
	    .name = NULL, 
	    .width = 1, 
	    .height = 1,
	    .fsignature = 0x00000000,
	    .fhalf = FH_LEFT, 
	    .aliasF = TFONT_ALIAS, 
	    .glyph = NULL, 
	    .glyph_width = NULL,
	    .dglyph = sgFontsDefaultDGlyph, 
	    .bitmap = NULL,
	    .colf = 0xFF, 
	    .coll = 0x00, 
	    .rowf = 0xFF, 
	    .rowl = 0x00, 
	    .colspan = 0x00, 
	    .bytew = 1, 
	    .bytec = 1
	},
};

void tfont_ary_show_list(FILE* fp)
{
	TFont* ce = gFont;
	char c;
	u_int n;
	u_int u;
	u_int d;
	u_int f1;
	u_int f2;
	u_int uni;
	while (ce->name) {
		n = ce->fsignature;
		c = n & 255;
		u = (n >> 4) & 15;
		d = (n >> 0) & 15;
		f1 = n & TFONT_FT_DOUBLE;
		f2 = n & TFONT_FT_96CHAR;
		uni = ce->fhalf == FH_UNI ? 1 : 0;
		if (uni) {
		    fprintf(fp, " [--/--(-) uni ] %s\n", ce->name);
		} else {
		    fprintf(fp, " [%02u/%02u(%c) %s%s] %s\n",
			    u, d, c,
			    f2 ? "96" : "94",
			    f1 ? "^N" : "  ",
			    ce->name);
		}
		ce++;
	}
	
}

void tfont_ary_final(void)
{
	TFont* ce = gFont;
	while (ce->name) {
		if (ce->aliasF & TFONT_OWNER) {
			tfont_final(ce);
		}
		ce++;
	}
}

int tfont_ary_search_idx(const char* na)
{
	TFont* p;
	int i = 0;
	for (p = gFont ; p->name ; p++, i++) {
		if (strncasecmp(p->name, na, strlen(p->name)) == 0) {
			return i;
		}
	}	
	return -1;
}

TFont* tfont_ary_search(const char* na)
{
	TFont* p;
	for (p = gFont ; p->name ; p++) {
		if (strncasecmp(p->name, na, strlen(p->name)) == 0) {
			return p;
		}
	}	
	return NULL;
}

void tfont_final(TFont* p)
{
	util_free(p->glyph);
	util_free(p->dglyph);
	util_free(p->bitmap);
}

const u_char* tfont_default_glyph(
	TFont* p,
	u_int chlw,
	u_int* width)
{
	static u_char gly[PICOFONT_HEIGHT * 2];

	u_char* cp = gly;
	u_int i;
	u_int a;
	u_int b;
	*width = p->width;

	*cp++ = 0x80;
	*cp++ = 0x00;
	for (i = 0 ; i < 16 ; i+=4) {
		b = (chlw >> (12-i)) & 0xf;
		b <<= 1;
		a = (chlw >> (28-i)) & 0xf;
		a <<= 1;
		*cp++ = 0x80 | gPicoFontLeft[b++];
		*cp++ = gPicoFontLeft[a++];
		*cp++ = 0x80 | gPicoFontLeft[b];
		*cp++ = gPicoFontLeft[a];
		*cp++ = 0x80;
		*cp++ = 0x00;
	}
	*cp++ = 0x80;
	*cp++ = 0x00;
	if (p->bytec == 2) {
		*cp++ = 0xff;
		*cp = 0xfe;
	} else {
		*cp++ = 0xfe;
		*cp = 0x00;
	}

	return gly;
}

const u_char* tfont_standard_glyph(
	TFont* p,
	u_int chlw,
	u_int *width)
{
	u_char b2 = (chlw >> 8) & 0xFF;
	u_char b1 = chlw & 0xFF;
	u_int i;

	*width = p->width;
	if (b1 < p->colf || p->coll < b1 || b2 < p->rowf || p->rowl < b2) {
		return p->dglyph;	
	} else {
		i = (b1 - p->colf) + (b2  - p->rowf) * p->colspan;
		if (p->glyph_width)
			*width = p->glyph_width[i];
		return p->glyph[i];
	}
}

static FILE *font_open(const char *fname)
{
	int nf = strlen(fname);

	if ((nf > 3 && strcmp(".gz", fname+nf-3) == 0) ||
	    (nf > 2 && strcmp(".Z", fname+nf-2) == 0)) {
		pid_t pid;
		int filedes[2];
		if (pipe(filedes) < 0) {
			return NULL;
		}
		pid = fork();
		if (pid == -1) {
			goto err;
		} else if (pid == 0) {
			/* child */
			int fd0 = open(fname, O_RDONLY);
			if (fd0 < 0) {
				goto err;
			}
			close(filedes[0]);
			close(0); close(1);
			dup2(fd0, 0);
			dup2(filedes[1], 1);
#ifndef JFB_GZIP_PATH
#error			JFB_GZIP_PATH is not set.
#else
			execl(JFB_GZIP_PATH, JFB_GZIP_PATH, "-dc", NULL);
#endif
			exit(0);
		}
		close(filedes[1]);
		return fdopen(filedes[0], "r");
err:
		close(filedes[0]);
		close(filedes[1]);
		return NULL;
	} else {
		return fopen(fname, "r");
	}
	return NULL;
}

static void tfont_setup_font(TFont* pf, const char* fname, FONTSET_HALF hf)
{
	FILE* fp;
	TPcf pcf;
	tpcf_init(&pcf);

	if (access(fname, R_OK) != 0) {
		print_message("PCF : CANNOTOPEN : %s\n", fname);
		return;
	}
	fp = font_open(fname);
	if (!fp) {
		print_message("PCF : CANNOTOPEN : %s\n", fname);
		return;
	}
	tpcf_load(&pcf, fp);
	fclose(fp);

	tpcf_as_tfont(&pcf, pf);
	pf->fhalf = hf;
	pf->conv = tfont_standard_glyph;
	tpcf_final(&pcf);
}

static void tfont_alias(TFont* dst, TFont* src, FONTSET_HALF hf)
{
	dst->conv = src->conv;
	dst->width = src->width;
	dst->height = src->height;
	dst->fhalf = hf;
	dst->aliasF = TFONT_ALIAS;
	dst->glyph = src->glyph;
	dst->glyph_width = src->glyph_width;
	dst->dglyph = src->dglyph;
	dst->bitmap = src->bitmap;
	dst->colf = src->colf; 
	dst->coll = src->coll;
	dst->rowf = src->rowf;
	dst->rowl = src->rowl;
	dst->colspan = src->colspan;
	dst->bytew = src->bytew;
	dst->bytec = src->bytec;
}

static void tfont_fontlist_glyph_size(void)
{
	TFont* p = gFont;
	u_int w;
	u_int dw;

	while (p->name) {
		if (gFontsHeight < p->height) {
			gFontsHeight = p->height;
		}
		w = p->width;
		w = (p->fsignature & TFONT_FT_DOUBLE) ? (w+1)/2 : w;
		if (gFontsWidth < w) {
			gFontsWidth = w;
		}
		p++;
	}

	if (PICOFONT_WIDTH <= gFontsWidth && PICOFONT_HEIGHT <= gFontsHeight) {
		p = gFont;
		while (p->name) {
			if (p->conv == tfont_default_glyph) {
				dw = p->width;
				p->height = PICOFONT_HEIGHT;
				p->width = PICOFONT_WIDTH * dw;
			}
			p++;
		}
	}

}

int tfont_is_valid(TFont* p)
{
	return (p->width == 0 || p->height == 0) ? 0 : 1;
}

int tfont_is_loaded(TFont *p)
{
	return (p->conv != tfont_default_glyph);
}

void tfont_setup_fontlist(TCapValue* vp)
{
	static const char* types[] = {"pcf", "alias", NULL}; 
	static const char* sides[] = {"L", "R", "U", NULL}; 

	const char* srn;
	const char* sty;
	const char* shf;
	const char* sph;

	int type = 0;
	int side = 0;

	TFont* dst;
	TFont* src;

	TCsv farg;
	
	for (; vp ; vp = vp->next) {
		tcsv_init(&farg, vp->value);
		if (farg.cap != 4) {
			print_message("FONT : Skipped (BAD FORMAT)\n");
			goto FINALIZE;
		}
		srn = tcsv_get_token(&farg);
		sty = tcsv_get_token(&farg);
		shf = tcsv_get_token(&farg);
		sph = tcsv_get_token(&farg);

		print_message("FONT : (%d) [%s]/%s/%s://%s/\n", farg.cap,
				 srn, sty, shf, sph);

		type = util_search_string(sty, types);
		side = util_search_string(shf, sides);
		if (type == -1 || side == -1) {
			print_message("FONT : Skipped (BAD FORMAT)\n");
			goto FINALIZE;
		}

		if (!(dst = tfont_ary_search(srn))) {
			print_message("FONT : Skipped (Unknown FONTSET=`%s'.)\n", srn);
			goto FINALIZE;
		}
		switch (side) {
		case 0: side = FH_LEFT; break;
		case 1: side = FH_RIGHT; break;
		case 2: side = FH_UNI; break;
		}

		switch (type) {
		case 0:		/* pcf file */
			tfont_setup_font(dst, sph, side);
			break;
		case 1:		/* alias */
			if (!(src = tfont_ary_search(sph))) {
				print_message("FONT : Skipped (Unknown ALIAS FONTSET=`%s'.)\n", sph);
				continue;
			}
			tfont_alias(dst, src, side);
			break;
		}

	FINALIZE:	
		tcsv_final(&farg);
	}

	tfont_fontlist_glyph_size();

	if (tfont_is_loaded(&(gFont[0])) == 0) {
#ifdef JFB_UTF8
		int i, found = 0;
		for (i = 1; gFont[i].fsignature; i++) {
			if (gFont[i].fhalf == FH_UNI) {
				if (tfont_is_loaded(&(gFont[i]))) {
					found = 1;
				}
				break;
			}
		}
		if (!found)
			die("FONT : ISO8859/ISO10646 not loaded.\n");

#else
		die("FONT : ISO8859 not loaded.\n");
#endif
	}
}
