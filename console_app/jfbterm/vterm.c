/*
 * JFBTERM - 
 * Copyright (C) 2003  Fumitoshi UKAI <ukai@debian.or.jp>
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

#include	<stdio.h>
#include	<unistd.h>
#include	<string.h>
#include	<termios.h>
#include	<malloc.h>
#include	<sys/types.h>
#include	<sys/ioctl.h>
#include	<sys/kd.h>
#include	<errno.h>

#include	"config.h"
#include	"util.h"
#include	"term.h"
#include	"vterm.h"
#include	"font.h"
#include	"csv.h"

#include	"sequence.h"
#include	"message.h"

#define	LEN_REPORT	9

static void tvterm_set_default_encoding(TVterm* p, const char* en);
static void tvterm_esc_start(TVterm* p, u_char ch);
static void tvterm_esc_bracket(TVterm*, u_char);
#ifdef JFB_OTHER_CODING_SYSTEM
static void tvterm_esc_rbracket(TVterm*, u_char);
#endif
static void tvterm_esc_traditional_multibyte_fix(TVterm* p, u_char ch);
static int tvterm_find_font_index(int fsig);
static void tvterm_esc_designate_font(TVterm* p, u_char ch);
static void tvterm_esc_designate_otherCS(TVterm* p, u_char ch);
static void tvterm_invoke_gx(TVterm* p, TFontSpec* fs, u_int n);
static void tvterm_re_invoke_gx(TVterm* p, TFontSpec* fs);
void tvterm_set_window_size(TVterm* p);

static void tvterm_switch_to_ISO2022(TVterm *p);
static int tvterm_is_ISO2022(TVterm *p);
#ifdef JFB_UTF8
static void tvterm_switch_to_UTF8(TVterm *p);
static int tvterm_is_UTF8(TVterm *p);
#endif
#ifdef JFB_OTHER_CODING_SYSTEM
static void tvterm_finish_otherCS(TVterm *p);
static void tvterm_switch_to_otherCS(TVterm *p, TCodingSystem *other);
static int tvterm_is_otherCS(TVterm *p);

#endif

void tvterm_init(TVterm* p, TTerm* pt, u_int hw, u_int hh, TCaps *caps, const char* en)
{
	p->term = pt;
	p->xcap = hw;
	p->xcap4 = (hw+7) & ~3;
	p->ycap = hh;

	tpen_init(&(p->pen));
	p->savedPen = NULL;
	p->savedPenSL = NULL;
	p->scroll = 0;
	p->soft = FALSE;
	p->wrap = FALSE;
	p->esc = NULL;

	p->textHead = 0;

	/* iso 2022 's default */
	p->gDefaultL = 0;
	p->gDefaultR = 1;
	p->gDefaultIdx[0] = 0;			/* G0 <== ASCII */
	p->gDefaultIdx[1] = 1;			/* G1 <== JIS X 0208 */
	p->gDefaultIdx[2] = 0;			/* G2 <== ASCII */
	p->gDefaultIdx[3] = 0;			/* G3 <== ASCII */
#ifdef JFB_UTF8
	p->utf8DefaultIdx = 0;
	p->utf8Idx = 0;
	p->utf8remain = 0;
	p->ucs2ch = 0;
#endif
#ifdef JFB_OTHER_CODING_SYSTEM
	p->otherCS = NULL;
#endif
	p->caps = caps;
	p->altCs = FALSE;
	tvterm_set_default_encoding(p, en);
}

int
tvterm_parse_encoding(const char *en, int idx[6])
{
	static const char* table[] = {"G0", "G1", "G2", "G3", NULL}; 
	TCsv farg;
	const char *g;
	int i;
	int ig;

	tcsv_init(&farg, en);
	if (farg.cap != 6) {
		tcsv_final(&farg);
		return 0;
	}
	for (i = 0; i < 2; i++) {
		g = tcsv_get_token(&farg);
		ig = util_search_string(g, table);
		if (ig == -1)
			ig = i;
		idx[i] = ig;
	}
	for (i = 0 ; i < 4 ; i++) {
		g = tcsv_get_token(&farg);
		ig = tfont_ary_search_idx(g);
		if (ig == -1) {
			ig = 0;
		}
		idx[2+i] = ig;
	}
	tcsv_final(&farg);
	return 1;
}

static void
tvterm_switch_to_ISO2022(TVterm *p)
{
#ifdef JFB_OTHER_CODING_SYSTEM
	tvterm_finish_otherCS(p);
#endif
#ifdef JFB_UTF8
	p->utf8Idx = 0;
	p->utf8remain = 0;
	p->ucs2ch = 0;
#endif
}

static int
tvterm_is_ISO2022(TVterm *p)
{
#ifdef JFB_OTHER_CODING_SYSTEM
	if (tvterm_is_otherCS(p))
		return 0;
#endif
#ifdef JFB_UTF8
	if (tvterm_is_UTF8(p))
		return 0;
#endif
	return 1;
}

#ifdef JFB_UTF8
static int
tvterm_UTF8index(const char *en)
{
	int i;
	TCsv farg;
	const char *g;
	int id = 0;
	
	tcsv_init(&farg, en);
	g = tcsv_get_token(&farg);
	if (strcmp(g, "UTF-8") != 0)
		goto FINALIZE;
	if (farg.cap != 2)
		goto FINALIZE;
	g = tcsv_get_token(&farg);
	for (i = 0; gFont[i].name; i++) {
		if (strcmp(gFont[i].name, g) == 0) {
			if (tfont_is_loaded(&(gFont[i]))) {
				id = i;
				break;
			}
		}
	}
FINALIZE:
	tcsv_final(&farg);
	return id;
}

static void
tvterm_switch_to_UTF8(TVterm *p)
{
#ifdef JFB_OTHER_CODING_SYSTEM
	tvterm_finish_otherCS(p);
#endif
	if (p->utf8DefaultIdx == 0) {
		const char *en = tcaps_find_entry(p->caps,
						  "encoding.",
						  "UTF-8");
		if (en != NULL) {
			p->utf8DefaultIdx = tvterm_UTF8index(en);
		}
	}
	p->utf8Idx = p->utf8DefaultIdx;
	p->utf8remain = 0;
	p->ucs2ch = 0;
	return;
}

static int
tvterm_is_UTF8(TVterm *p) 
{
	return (p->utf8Idx != 0);
}

#endif

#ifdef JFB_OTHER_CODING_SYSTEM
int
tvterm_parse_otherCS(const char *en, TCodingSystem* otherCS)
{
	TCsv farg;
	const char *g;

	memset(otherCS, 0, sizeof(*otherCS));
	tcsv_init(&farg, en);

	if (farg.cap != 4) {
		tcsv_final(&farg);
		return 0;
	}
	g = tcsv_get_token(&farg); /* g == "other" */

	g = tcsv_get_token(&farg);
	otherCS->fromcode = strdup(g);

	g = tcsv_get_token(&farg); /* g == "iconv" */

	g = tcsv_get_token(&farg);
	otherCS->tocode = strdup(g);

	tcsv_final(&farg);

	return 1;
}

static void
tvterm_finish_otherCS(TVterm *p)
{
	/* XXX restore ISO-2022 state? */
	if (p->otherCS) {
		TCodingSystem *o = p->otherCS;
		int i;
		/* restore designate/invoke state */
		for (i = 0; i < 4; i++)
			p->gIdx[i] = o->gSavedIdx[i];
		tvterm_invoke_gx(p, &(p->gl), o->gSavedL);
		tvterm_invoke_gx(p, &(p->gr), o->gSavedR);
		p->tgl = p->gl;
		p->tgr = p->gr;
		p->knj1 = 0;
		p->utf8Idx = o->utf8SavedIdx;
		p->utf8remain = 0;
		p->ucs2ch = 0;

		if (o->cd != (iconv_t)(-1)) {
			iconv_close(o->cd);
		}
		o->cd = (iconv_t)(-1);
		if (o->fromcode)
			free(o->fromcode);
		o->fromcode = NULL;
		if (o->tocode)
			free(o->tocode);
		o->tocode = NULL;
	}
	p->otherCS = NULL;
}

static void 
tvterm_switch_to_otherCS(TVterm *p, TCodingSystem *ocs)
{
	static TCodingSystem otherCS;
	char *tocode = ocs->tocode;
	int i;

	tvterm_finish_otherCS(p);
	/* save current designate/invoke */
	ocs->gSavedL = p->gl.invokedGn;
	ocs->gSavedR = p->gr.invokedGn;
	for (i = 0; i < 4; i++)
		ocs->gSavedIdx[i] = p->gIdx[i];
	ocs->utf8SavedIdx = p->utf8Idx;

retry:
	if (strcmp(ocs->tocode, "UTF-8") == 0) {
		tvterm_switch_to_UTF8(p);
		tocode = "UCS-2BE"; /* XXX */
	} else {
		int idx[6];
		char *en;

		en = tcaps_find_entry(p->caps, "encoding.", ocs->tocode);
		if (en == NULL) {
			free(ocs->tocode);
			ocs->tocode = strdup("UTF-8");
			goto retry;
		}

		if (tvterm_parse_encoding(en, idx) == 0) {
			free(ocs->tocode);
			ocs->tocode = strdup("UTF-8");
			goto retry;
		}
		for (i = 0; i < 4; i++)
			p->gIdx[i] = idx[2+i];
		tvterm_invoke_gx(p, &(p->gl), idx[0]);
		tvterm_invoke_gx(p, &(p->gr), idx[1]);
		p->tgl = p->gl;
		p->tgr = p->gr;
		p->knj1 = 0;
	}
	ocs->cd = iconv_open(tocode, ocs->fromcode);
	if (ocs->cd == (iconv_t)(-1)) {
		if (strcmp(ocs->tocode, "UTF-8") == 0) {
			free(ocs->tocode);
			ocs->tocode = strdup("UTF-8");
			goto retry;
		}
		goto FINALIZE;
	}
	/* ok */
	memcpy(&otherCS, ocs, sizeof(otherCS));
	memset(otherCS.inbuf, 0, sizeof(otherCS.inbuf));
	otherCS.inbuflen = 0;
	memset(otherCS.outbuf, 0, sizeof(otherCS.outbuf));
	p->otherCS = &otherCS;
	return;
FINALIZE:
	free(ocs->fromcode);
	free(ocs->tocode);
	ocs->fromcode = NULL;
	ocs->tocode = NULL;
	return;
}

static int
tvterm_is_otherCS(TVterm *p)
{
	return (p->otherCS != NULL);
}

#endif


void tvterm_set_default_encoding(TVterm* p, const char* en)
{
	const char *g;
	int i;
	int idx[6];
	TCsv farg;
	int ncap;

	tcsv_init(&farg, en);
	ncap = farg.cap;
	/* first token */
	g = tcsv_get_token(&farg);
#ifdef JFB_OTHER_CODING_SYSTEM
	if (strcmp(g, "other") == 0) {
		/* other,<coding-system>,iconv,<std-coding-system> */
		static TCodingSystem otherCS;
		if (tvterm_parse_otherCS(en, &otherCS) == 0)
			goto FINALIZE;
		tvterm_switch_to_otherCS(p, &otherCS);
		goto FINALIZE;
	}
#endif
#ifdef JFB_UTF8
	if (strcmp(g, "UTF-8") == 0) {
		/* UTF-8,<fontsetname> */
		p->utf8DefaultIdx = tvterm_UTF8index(en);
		tvterm_switch_to_UTF8(p);
		goto FINALIZE;
	}
#endif
	/* ISO-2022 */
	if (tvterm_parse_encoding(en, idx) == 0) {
		goto FINALIZE;
	}
	/* GL */
	p->gDefaultL = idx[0];
	/* GR */
	p->gDefaultR = idx[1];
	/* G0 .. G3 */
	for (i = 0 ; i < 4 ; i++) {
		p->gDefaultIdx[i] = idx[2+i];
	}
FINALIZE:	
	tcsv_final(&farg);
}

void tvterm_set_default_invoke_and_designate(TVterm* p)
{
	p->gIdx[0] = p->gDefaultIdx[0];		/* G0 <== ASCII */
	p->gIdx[1] = p->gDefaultIdx[1];		/* G1 <== JIS X 0208 */
	p->gIdx[2] = p->gDefaultIdx[2];		/* G2 <== ASCII */
	p->gIdx[3] = p->gDefaultIdx[3];		/* G3 <== ASCII */
	tvterm_invoke_gx(p, &(p->gl), p->gDefaultL);/* GL <== G0 */
	tvterm_invoke_gx(p, &(p->gr), p->gDefaultR);/* GR <== G1 */
	p->tgl = p->gl;				/* Next char's GL <== GL */
	p->tgr = p->gr;				/* Next char's GR <== GR */
	p->knj1 = 0;
#ifdef JFB_UTF8
	p->utf8Idx = p->utf8DefaultIdx;
	p->utf8remain = 0;
	p->ucs2ch = 0;
#endif
	p->altCs = FALSE;
}

void tvterm_start(TVterm* p)
{
	p->pen.x = 0;
	p->pen.y = 0;
	p->xmax = p->xcap;
	p->ymax = p->ycap;
	p->tab = 8;
	p->pen.fcol = 7;
	p->pen.bcol = 0;
	p->pen.attr = 0;
	p->esc = NULL;
	p->soft = FALSE;
	p->ins = FALSE;
	p->wrap = FALSE;
	p->active = TRUE;
	p->altCs = FALSE;

	/* ISO-2022 */
	p->escSignature = 0;
	p->escGn = 0;
	tvterm_set_default_invoke_and_designate(p);

	p->cursor.x = 0;
	p->cursor.y = 0;
	p->cursor.on = TRUE;
	p->cursor.shown = FALSE;
	p->cursor.wide = FALSE;
	p->cursor.width = gFontsWidth;
	p->cursor.height = gFontsHeight;

	p->tsize = p->xcap4 * p->ycap;
	p->text = (u_int *)calloc(p->xcap4, p->ycap * sizeof(u_int));
	p->attr = (u_char *)calloc(p->xcap4, p->ycap);
	p->flag = (u_char *)calloc(p->xcap4, p->ycap);

	ioctl(0, KDSETMODE, KD_GRAPHICS);
	/*
	 * ioctl(0, TIOCGWINSZ, &text_win);
	 * cInfo.shown = FALSE;
	 * saved = FALSE;
	 * GraphMode();
	ioctl(0, TIOCGWINSZ, p->winrect);
	 */
        /*  */
	tvterm_register_signal(p);
	tvterm_set_window_size(p);
}

void tvterm_final(TVterm* p)
{
	ioctl(0, KDSETMODE, KD_TEXT);
	tpen_final(&(p->pen));
	if (p->savedPen) {
		tpen_final(p->savedPen);
		free(p->savedPen);
		p->savedPen = NULL;
	}
	if (p->savedPenSL) {
		tpen_final(p->savedPenSL);
		free(p->savedPenSL);
		p->savedPenSL = NULL;
	}
	p->textHead = 0;
	util_free(p->text);
	util_free(p->attr);
	util_free(p->flag);
}

void tvterm_push_current_pen(TVterm* p, TBool b)
{
	TPen* t;
	TPen** base;
	base = b ? &(p->savedPen) : &(p->savedPenSL);
	t = (TPen*)malloc(sizeof(TPen));

	if (!t) {
		return;
	}
	tpen_init(t);
	tpen_copy(t, &(p->pen));
	t->prev = *base;
	*base = t;
}

void tvterm_pop_pen_and_set_currnt_pen(TVterm* p, TBool b)
{
	TPen* t;
	TPen** base;
	base = b ? &(p->savedPen) : &(p->savedPenSL);
	t = *base;

	if (!t) {
		return;
	}
	tpen_copy(&(p->pen), t);

	if (p->pen.y < p->ymin) p->pen.y = p->ymin;
	if (p->pen.y >= p->ymax-1) p->pen.y = p->ymax -1;
	
	*base = t->prev;
	free(t);
}

/*---------------------------------------------------------------------------*/

static inline int IS_GL_AREA(TVterm* p, u_char ch)
{
	return (p->tgl.type & TFONT_FT_96CHAR) ? (0x1F < ch && ch < 0x80) :
						(0x20 < ch && ch < 0x7F);
}
static inline int IS_GR_AREA(TVterm* p, u_char ch)
{
	return (p->tgr.type & TFONT_FT_96CHAR) ? (0x9F < ch) :
						(0xA0 < ch && ch < 0xFF);
}

static inline void INSERT_N_CHARS_IF_NEEDED(TVterm* p, int n)
{
	if (p->ins) {
		tvterm_insert_n_chars(p, n);
	}
}

static inline void SET_WARP_FLAG_IF_NEEDED(TVterm* p)
{
	if (p->pen.x == p->xmax-1) {
		p->wrap = TRUE;
	}
}


static int tvterm_put_normal_char(TVterm* p, u_char ch)
{
	if (p->pen.x == p->xmax) {
		p->wrap = TRUE;
		p->pen.x --;
	}
	if (p->wrap) {
		p->pen.x -= p->xmax -1;
		if (p->pen.y == p->ymax -1) {
			p->scroll++;
		} else {
			p->pen.y ++;
		}
		p->wrap = FALSE;
		return -1;
	}
	if (p->knj1) { /* 漢字 第 2 バイト */
		INSERT_N_CHARS_IF_NEEDED(p, 2);
		tvterm_wput(p, p->knj1idx,
			p->knj1h == FH_RIGHT ? p->knj1 | 0x80 : p->knj1 & 0x7F,
			p->knj1h == FH_RIGHT ? ch | 0x80 : ch & 0x7F);
		p->pen.x += 2;
		p->knj1 = 0;
		p->tgr = p->gr; p->tgl = p->gl;
	} else if (IS_GL_AREA(p, ch)) {
		if (p->tgl.type & TFONT_FT_DOUBLE) {
			SET_WARP_FLAG_IF_NEEDED(p);
			p->knj1 = ch;
			p->knj1h = p->tgl.half;
			p->knj1idx = p->tgl.idx;
		} else {
			INSERT_N_CHARS_IF_NEEDED(p, 1);
			tvterm_sput(p, p->tgl.idx,
				p->tgl.half == FH_RIGHT ? ch | 0x80: ch);
			p->pen.x ++;
			p->tgr = p->gr; p->tgl = p->gl;
		}
	} else if (IS_GR_AREA(p, ch)) {
		if (p->tgr.type & TFONT_FT_DOUBLE) {
			SET_WARP_FLAG_IF_NEEDED(p);
			p->knj1 = ch;
			p->knj1h = p->tgr.half;
			p->knj1idx = p->tgr.idx;
		} else {
			INSERT_N_CHARS_IF_NEEDED(p, 1);
			tvterm_sput(p, p->tgr.idx,
				p->tgr.half == FH_LEFT ? ch & ~0x80: ch);
			p->pen.x ++;
			p->tgr = p->gr; p->tgl = p->gl;
		}
	} else if (ch == 0x20) {
		INSERT_N_CHARS_IF_NEEDED(p, 1);
		tvterm_sput(p, 0, 0x20);
		p->pen.x ++;
		p->tgr = p->gr; p->tgl = p->gl;
	}
	return 0;
}

#ifdef JFB_UTF8
static int tvterm_put_uchar(TVterm* p, u_int ch)
{
	TFont *pf = &gFont[p->utf8Idx];
	u_int w;
	if (p->pen.x == p->xmax) {
		p->wrap = TRUE;
		p->pen.x --;
	}
	if (p->wrap) {
		p->pen.x -= p->xmax -1;
		if (p->pen.y == p->ymax -1) {
			p->scroll++;
		} else {
			p->pen.y ++;
		}
		p->wrap = FALSE;
		return -1;
	}
	pf->conv(pf, ch, &w);
	if (pf->width == w) {
		INSERT_N_CHARS_IF_NEEDED(p, 1);
		tvterm_uput1(p, p->utf8Idx, ch);
		p->pen.x ++;
	} else {
		INSERT_N_CHARS_IF_NEEDED(p, 2);
		tvterm_uput2(p, p->utf8Idx, ch);
		p->pen.x += 2;
	}
	p->utf8remain = 0;
	p->ucs2ch = 0;
	return 0;
}
#endif

int tvterm_iso_C0_set(TVterm* p, u_char ch)
{
	switch(ch) {
	case ISO_BS:
		if (p->pen.x) {
			p->pen.x --;
		}
		p->wrap = FALSE;
		break;
	case ISO_HT:
		p->pen.x += p->tab - (p->pen.x % p->tab);
		p->wrap = FALSE;
		if (p->pen.x < p->xmax) {
			break;
		}
		p->pen.x -= p->xmax;
		/* fail into next case */
	case ISO_VT:
	case ISO_FF:
#ifdef JFB_OTHER_CODING_SYSTEM
		if (!tvterm_is_otherCS(p))
#endif
			tvterm_set_default_invoke_and_designate(p);
		/* fail into next case */
	case ISO_LF:
		p->wrap = FALSE;
		if (p->pen.y == p->ymax -1) {
			p->scroll ++;
		} else  {
			p->pen.y ++;
		}
		break;
	case ISO_CR:
		p->pen.x = 0;
		p->wrap = FALSE;
		break;
	case UNIVERSAL_ESC:
		p->esc = tvterm_esc_start;
		p->escSignature = 0;
		p->escGn = 0;
		return 1;
	case ISO_LS1:
		if (!tvterm_is_ISO2022(p))
			break;
		tvterm_invoke_gx(p, &(p->gl), 1); /* GL <== G1 */
		p->tgl = p->gl;
		return 1;
	case ISO_LS0:
		if (!tvterm_is_ISO2022(p))
			break;
		tvterm_invoke_gx(p, &(p->gl), 0); /* GL <== G0 */
		p->tgl = p->gl;
		return 1;
	default:
		break;
	}
	return 0;
}

int tvterm_iso_C1_set(TVterm* p, u_char ch)
{
	switch (ch) {
	case ISO_SS2:	/* single shift 2 */
		tvterm_invoke_gx(p, &(p->tgr), 2); /* GR <== G2 */
		return 1;
	case ISO_SS3:	/* single shift 3 */
		tvterm_invoke_gx(p, &(p->tgr), 3); /* GR <== G3 */
		return 1;
	default:
		break;
	}
	return 0;
}

#ifdef JFB_UTF8
#define UTF8_CHECK_START(p) { \
	if ((p)->utf8remain != 0) { \
		(p)->ucs2ch = 0; \
		(p)->utf8remain = 0; \
	} \
}

int
tvterm_put_utf8_char(TVterm *p, u_char ch)
{
	int rev;
	if (ch < 0x7F) {
		UTF8_CHECK_START(p);
		p->ucs2ch = ch;
	} else if ((ch & 0xC0) == 0x80) {
		if (p->utf8remain == 0) {
			/* illegal UTF-8 sequence? */
			p->ucs2ch = ch;
		} else {
			p->ucs2ch <<= 6;
			p->ucs2ch |= (ch & 0x3F);
			p->utf8remain--;
			if (p->utf8remain > 0) {
				return p->utf8remain;
			}
		}
	} else if ((ch & 0xE0) == 0xC0) {
		UTF8_CHECK_START(p);
		p->utf8remain = 1;
		p->ucs2ch = (ch & 0x1F);
		return p->utf8remain;
	} else if ((ch & 0xF0) == 0xE0) {
		UTF8_CHECK_START(p);
		p->utf8remain = 2;
		p->ucs2ch = (ch & 0x0F);
		return p->utf8remain;
	} else {
		/* XXX: support UCS2 only */
	}
	if (p->altCs 
	    && (p->ucs2ch < 127)
	    && (IS_GL_AREA(p, (u_char)(p->ucs2ch & 0x0ff))))
		rev = tvterm_put_normal_char(p, (p->ucs2ch & 0x0ff));
	else
		rev = tvterm_put_uchar(p, p->ucs2ch);
	if (rev < 0) {
		p->ucs2ch >>= 6;
		if (p->ucs2ch)
			p->utf8remain ++;
	}
	return rev;
}
#endif

#ifdef JFB_OTHER_CODING_SYSTEM
int 
tvterm_put_otherCS_char(TVterm *p, u_char ch)
{
	int rev;
	char *inbuf;
	size_t inbuflen;
	char *outbuf;
	size_t outbuflen;
	size_t s;
	int i;

	if (p->otherCS->inbuflen >= sizeof(p->otherCS->inbuf)-1) {
		/* XXX: too long? */
		p->otherCS->inbuflen = 0;
	}
	p->otherCS->inbuf[p->otherCS->inbuflen] = ch;
	p->otherCS->inbuflen++;

	inbuf = p->otherCS->inbuf;
	inbuflen = p->otherCS->inbuflen;
	outbuf = p->otherCS->outbuf;
	outbuflen = sizeof(p->otherCS->outbuf);

	s = iconv(p->otherCS->cd, &inbuf, &inbuflen, &outbuf, &outbuflen);
	if (s == (size_t)(-1)) {
		switch (errno) {
		case E2BIG:
			p->otherCS->inbuflen = 0;
			return 0;
		case EILSEQ: /* illegal sequence */
			p->otherCS->inbuflen = 0;
			return 0;
		case EINVAL: /* incomplete multibyte */
			return 0;
		}
	}
	/* output bytes */
	s = sizeof(p->otherCS->outbuf) - outbuflen;

	if (strcmp(p->otherCS->tocode, "UTF-8") == 0 && p->altCs == FALSE) {
		p->ucs2ch = 0;
		for (i = 0; i < s; i++) {
			p->ucs2ch <<= 8;
			p->ucs2ch |= (p->otherCS->outbuf[i] & 0x0ff);
		}

		rev = tvterm_put_uchar(p, p->ucs2ch);
		if (rev < 0) {
			p->otherCS->inbuflen--;
			return rev;
		}
		p->otherCS->inbuflen = 0;
		return rev;
	} else {
		for (i = 0; i < s; i++) {
			u_char c = p->otherCS->outbuf[i];
			rev = tvterm_put_normal_char(p, c);
			if (rev < 0) {
				p->otherCS->inbuflen--;
				return rev;
			}
		}
		p->otherCS->inbuflen = 0;
		return 0;
	}
}
#endif

void tvterm_emulate(TVterm* p, const char *buff, int nchars)
{
	u_char	ch;
	int rev;
	int cn;

	while (nchars-- > 0) {
		ch = *(buff++);
		if (!ch) {
			continue;
		} else if (p->esc) {
			p->esc(p, ch);
		} else if (ch < 0x20) {
			cn = tvterm_iso_C0_set(p, ch);
			if (cn) {
				continue;
			}
#ifdef JFB_OTHER_CODING_SYSTEM
		} else if (tvterm_is_otherCS(p)) {
			rev = tvterm_put_otherCS_char(p, ch);
			if (rev >= 0) {
				continue;
			} else if (rev < 0) {
				nchars -= rev;
				buff += rev;
			}
#endif
#ifdef JFB_UTF8
		} else if (tvterm_is_UTF8(p)) {
			rev = tvterm_put_utf8_char(p, ch);
			if (rev >= 0) {
				continue;
			} else if (rev < 0) {
				nchars -= rev;
				buff += rev;
			}
#endif
		} else if ((0x7F < ch) && (ch < 0xA0)) {
			cn = tvterm_iso_C1_set(p, ch);
			if (cn) {
				continue;
			}
		} else if (ch == ISO_DEL) {
			/* nothing to do. */
		} else {
			rev = tvterm_put_normal_char(p, ch);
			if (rev == 0) {
				continue;
			} else if (rev < 0) {
				nchars -= rev;
				buff += rev;
			}
		}
		if (p->scroll > 0) {
			tvterm_text_scroll_up(p, p->scroll);
		} else if (p->scroll < 0) {
			tvterm_text_scroll_down(p, -(p->scroll));
		}
		p->scroll = 0;
	}
}

#define ESC_ISO_GnDx(n, x) \
	{\
		p->escSignature |= (x); p->escGn = (n);\
		p->esc = tvterm_esc_designate_font;\
	}

#define Fe(x)	((x)-0x40)

static void tvterm_esc_start(TVterm* p, u_char ch)
{
	p->esc = NULL;
	switch(ch) {
	case Fe(ISO_CSI):	/* 5/11 [ */
		p->esc = tvterm_esc_bracket;
		break;
#ifdef JFB_OTHER_CODING_SYSTEM
	case Fe(ISO_OSC):	/* 5/13 ] */
		p->esc = tvterm_esc_rbracket;
		break;
#endif
	case ISO__MBS:		/* 2/4 $ */
		p->esc = tvterm_esc_traditional_multibyte_fix;
		p->escSignature = TFONT_FT_DOUBLE;
		break;
	case ISO_DOCS:		/* 2/5 % */
		p->esc = tvterm_esc_designate_otherCS;
		break;
	case ISO_GZD4:		/* 2/8 ( */
		ESC_ISO_GnDx(0, TFONT_FT_94CHAR);
		break;
	case MULE__GZD6:	/* 2/12 , */
		ESC_ISO_GnDx(0, TFONT_FT_96CHAR);
		break;
	case ISO_G1D4:		/* 2/9 ) */
		ESC_ISO_GnDx(1, TFONT_FT_94CHAR);
		break;
	case ISO_G1D6:		/* 2/13 - */
		ESC_ISO_GnDx(1, TFONT_FT_96CHAR);
		break;
	case ISO_G2D4:		/* 2/10 * */
		ESC_ISO_GnDx(2, TFONT_FT_94CHAR);
		break;
	case ISO_G2D6:		/* 2/14 . */
		ESC_ISO_GnDx(2, TFONT_FT_96CHAR);
		break;
	case ISO_G3D4:		/* 2/11 + */
		ESC_ISO_GnDx(3, TFONT_FT_94CHAR);
		break;
	case ISO_G3D6:		/* 2/15 / */
		ESC_ISO_GnDx(3, TFONT_FT_96CHAR);
		break;
	case Fe(ISO_NEL):	/* 4/5 E */
		p->pen.x = 0;
		p->wrap = FALSE;
	case Fe(TERM_IND): 	/* 4/4 D */
		if (p->pen.y == p->ymax -1) {
			p->scroll ++;
		} else {
			p->pen.y ++;
		}
		break;
	case Fe(ISO_RI):	/* 4/13 M */
		if (p->pen.y == p->ymin) {
			p->scroll --;
		} else {
			p->pen.y --;
		}
		break;
	case Fe(ISO_SS2):	/* 4/14 N *//* 7bit single shift 2 */
		tvterm_invoke_gx(p, &(p->tgl), 2); /* GL <== G2 */
		break;
	case Fe(ISO_SS3):	/* 4/15 O *//* 7bit single shift 3 */
		tvterm_invoke_gx(p, &(p->tgl), 3); /* GL <== G3 */
		break;

	case ISO_RIS:		/* 6/3 c */
		p->pen.fcol = 7;
		p->pen.bcol = 0;
		p->pen.attr = 0;
		p->wrap = FALSE;
		tvterm_set_default_invoke_and_designate(p);
		/* fail into next case */
		/* TERM_CAH: - conflicts with ISO_G2D4, no terminfo */
		p->pen.x = 0;
		p->pen.y = 0;
		p->wrap = FALSE;
		tvterm_text_clear_all(p);
		break;
	case DEC_SC:		/* 3/7 7 */
		tvterm_push_current_pen(p, TRUE);
		break;
	case DEC_RC:		/* 3/8 8 */
		tvterm_pop_pen_and_set_currnt_pen(p, TRUE);
		p->wrap = FALSE;
		break;
	case ISO_LS2:		/* 6/14 n */
		if (!tvterm_is_ISO2022(p))
			break;
		tvterm_invoke_gx(p, &(p->gl), 2); /* GL <= G2 */
		p->tgl = p->gl;
		break;
	case ISO_LS3:		/* 6/15 o */
		if (!tvterm_is_ISO2022(p))
			break;
		tvterm_invoke_gx(p, &(p->gl), 3); /* GL <= G3 */
		p->tgl = p->gl;
		break;
	case ISO_LS3R:		/* 7/12 | */
		if (!tvterm_is_ISO2022(p))
			break;
		tvterm_invoke_gx(p, &(p->gr), 3); /* GR <= G3 */
		p->tgr = p->gr;
		break;
	case ISO_LS2R:		/* 7/13 } */
		if (!tvterm_is_ISO2022(p))
			break;
		tvterm_invoke_gx(p, &(p->gr), 2); /* GR <= G2 */
		p->tgr = p->gr;
		break;
	case ISO_LS1R:		/* 7/14 ~ */
		if (!tvterm_is_ISO2022(p))
			break;
		tvterm_invoke_gx(p, &(p->gr), 1); /* GR <= G1 */
		p->tgr = p->gr;
		break;
	}
}

/*---------------------------------------------------------------------------*/
void tvterm_esc_set_attr(TVterm* p, int col)
{
	static int acsIdx = 0;

	switch(col) {
	case 0: tpen_off_all_attribute(&(p->pen));
		break;
	case 1: tpen_higlight(&(p->pen));
		break;
	case 21: tpen_dehiglight(&(p->pen));
		break;
	case 4:	tpen_underline(&(p->pen));
		break;
	case 24: tpen_no_underline(&(p->pen));
		break;
	case 7:	tpen_reverse(&(p->pen));
		break;
	case 27: tpen_no_reverse(&(p->pen));
		break;
	case 10:	/* rmacs, rmpch */
		p->gIdx[0] = 0;
		tvterm_re_invoke_gx(p, &(p->gl));
		tvterm_re_invoke_gx(p, &(p->gr));
		p->tgl = p->gl; p->tgr = p->gr;
		p->altCs = FALSE;
		break;
	case 11:	/* smacs, smpch */
		if (acsIdx == 0) {
			acsIdx = tvterm_find_font_index(0x30|TFONT_FT_94CHAR);
		}
		if (acsIdx > 0) {
			p->gIdx[0] = acsIdx;
			tvterm_re_invoke_gx(p, &(p->gl));
			tvterm_re_invoke_gx(p, &(p->gr));
			p->tgl = p->gl; p->tgr = p->gr;
			p->altCs = TRUE;
		}
		break;
	default: tpen_set_color(&(p->pen), col);
		break;
	}
}

static void tvterm_set_mode(TVterm* p, u_char mode, TBool sw)
{
	switch(mode) {
	case 4:
		p->ins = sw;
		break;
	case 25:
		p->sw = sw;
		break;
	}
}

static void tvterm_esc_report(TVterm* p, u_char mode, u_short arg)
{
	p->report[0] = '\0';

	switch(mode) {
	case 'n':
		if (arg == 6) {
			int x = (p->pen.x < p->xmax-1) ? p->pen.x : p->xmax-1;
			int y = (p->pen.y < p->ymax-1) ? p->pen.y : p->ymax-1;
			sprintf(p->report, "\x1B[%d;%dR", y, x);
		} else if (arg == 5) {
			strcpy(p->report, "\x1B[0n\0");
		}
		break;
	case 'c':
		if (arg == 0) {
			strcpy(p->report, "\x1B[?6c\0");
		}
		break;
	}
	write(p->term->ptyfd, p->report, strlen(p->report));
}

static void tvterm_set_region(TVterm* p,int ymin, int ymax)
{
	/* FIX ME ! : 1999/10/30 */
	/* XXX: ESC[?1001r is used for mouse control by w3m. ukai 1999/10/27*/
	if (ymin < 0 || ymin >= p->ycap || ymin > ymax || ymax > p->ycap) {
	        /* ignore */
		return;
	}
	p->ymin = ymin;
	p->ymax = ymax;
	p->pen.x = 0;
	if (p->pen.y < p->ymin || p->pen.y > p->ymax-1) p->pen.y = p->ymin-1;
	p->wrap = FALSE;
	if (p->ymin || p->ymax != p->ycap) {
		p->soft = TRUE;
	} else {
		p->soft = FALSE;
	}
}

void tvterm_set_window_size(TVterm* p)
{
	struct winsize win;

	win.ws_row = p->ymax;
	win.ws_col = p->xcap;
	win.ws_xpixel = 0;
	win.ws_ypixel = 0;
	ioctl(p->term->ttyfd, TIOCSWINSZ, &win);
}


static void tvterm_esc_status_line(TVterm* p, u_char mode)
{
	switch(mode) {
	case 'T':	/* To */
		if (p->sl == SL_ENTER) break;
		if (!p->savedPenSL) {
			tvterm_push_current_pen(p, FALSE);
		}
	case 'S':	/* Show */
		if (p->sl == SL_NONE) {
			p->ymax = p->ycap - 1;
			tvterm_set_window_size(p);
		}
		if (mode == 'T') {
			p->sl = SL_ENTER;
			tvterm_set_region(p, p->ycap-1, p->ycap);
		}
		break;
	case 'F':	/* From */
		if (p->sl == SL_ENTER) {
			p->sl = SL_LEAVE;
			tvterm_set_region(p, 0, p->ycap -1);
			if (p->savedPenSL) {
				tvterm_pop_pen_and_set_currnt_pen(p, FALSE);
			}
			p->savedPenSL = NULL;
		}
		break;
	case 'H':	/* Hide */
	case 'E':	/* Erase */
		if (p->sl == SL_NONE) {
			break;
		}
		tvterm_set_region(p, 0, p->ycap);
		tvterm_set_window_size(p);
		p->sl = SL_NONE;
		break;
	default:
		p->esc = tvterm_esc_bracket;
		tvterm_esc_bracket(p, mode);
		return;
	}
	p->wrap = FALSE;
	p->esc = NULL;
}

#define	MAX_NARG	8

static void tvterm_esc_bracket(TVterm* p, u_char ch)
{
	u_char	n;
	static u_short varg[MAX_NARG], narg, question;

	if (ch >= '0' && ch <= '9') {
		varg[narg] = (varg[narg] * 10) + (ch - '0');
	} else if (ch == ';') {
		/* 引数は MAX_NARG までしかサポートしない!! */
		if (narg < MAX_NARG) {
			narg ++;
			varg[narg] = 0;
		} else {
			p->esc = NULL;
		}
	} else {
		p->esc = NULL;
		switch(ch) {
		case 'K':
			tvterm_text_clear_eol(p, varg[0]);
			break;
		case 'J':
			tvterm_text_clear_eos(p, varg[0]);
			break;
		case ISO_CS_NO_CUU:
			p->pen.y -= varg[0] ? varg[0]: 1;
			if (p->pen.y < p->ymin) {
				p->scroll -= p->pen.y - p->ymin;
				p->pen.y = p->ymin;
			}
			break;
		case ISO_CS_NO_CUD:
			p->pen.y += varg[0] ? varg[0]: 1;
			if (p->pen.y >= p->ymax) {
				p->scroll += p->pen.y - p->ymin;
				p->pen.y = p->ymax-1;
			}
			break;
		case ISO_CS_NO_CUF:
			p->pen.x += varg[0] ? varg[0]: 1;
			p->wrap = FALSE;
			break;
		case ISO_CS_NO_CUB:
			p->pen.x -= varg[0] ? varg[0]: 1;
			p->wrap = FALSE;
			break;
		case 'G':
			p->pen.x = varg[0] ? varg[0] - 1: 0;
			p->wrap = FALSE;
			break;
		case 'P':
			tvterm_delete_n_chars(p, varg[0] ? varg[0]: 1);
			break;
		case '@':
			tvterm_insert_n_chars(p, varg[0] ? varg[0]: 1);
			break;
		case 'L':
			tvterm_text_move_down(p, p->pen.y, p->ymax,
						varg[0] ? varg[0]: 1);
			break;
		case 'M':
			tvterm_text_move_up(p, p->pen.y, p->ymax,
						varg[0] ? varg[0]: 1);
			break;
		case 'H':
		case 'f':
			if (varg[1]) {
				p->pen.x = varg[1] - 1;
			} else {
				p->pen.x = 0;
			}
			p->wrap = FALSE;
		case 'd':
			p->pen.y = varg[0] ? varg[0] - 1: 0;
			/* XXX: resize(1) specify large x,y */
			break;
		case 'm':
			for (n = 0; n <= narg; n ++) {
				tvterm_esc_set_attr(p, varg[n]);
			}
			break;
		case 'r':
			n = varg[1];
			if (n == 0)
			    n = p->ycap;
			if (p->sl != SL_NONE) {
				if (n == p->ycap) {
					 n --;
				}
			}
			tvterm_set_region(p, varg[0] ? (varg[0] - 1): 0, n);
			break;
		case 'l':
			for (n = 0; n <= narg; n ++) {
				tvterm_set_mode(p, varg[n], FALSE);
			}
			break;
		case 'h':
			for (n = 0; n <= narg; n ++) {
				tvterm_set_mode(p, varg[n], TRUE);
			}
			break;
		case '?':
			p->esc = tvterm_esc_status_line;
	#if 0
			question = TRUE;
			p->esc = tvterm_esc_bracket;
	#endif
			break;
		case 's':
			tvterm_push_current_pen(p, TRUE);
			break;
		case 'u':
			tvterm_pop_pen_and_set_currnt_pen(p, TRUE);
			break;
		case 'n':
		case 'c':
			if (question != TRUE) {
				tvterm_esc_report(p, ch, varg[0]);
			}
			break;
		case 'R':
			break;
		}
		if (p->esc == NULL) {
			question = narg = varg[0] = varg[1] = 0;
		}
	}
}

#ifdef JFB_OTHER_CODING_SYSTEM

#define MAX_ARGLEN	48

/*
 * ESC  ]    p... F
 * 1/11 5/13 p... F
 *
 * p: 2/0 - 7/E
 * F: 0/1 - 1/15
 *
 * 0/5	designate private coding system
 *
 */
static void
tvterm_esc_rbracket(TVterm* p, u_char ch)
{
	static u_char arg[MAX_ARGLEN+1], enbuf[MAX_ARGLEN+32];
	static int argidx;

	if (ch >= 0x20 && ch <= 0x7e) {
		if (argidx < MAX_ARGLEN-1)
			arg[argidx++] = ch;
		arg[argidx] = '\0';
	} else {
		const char *en;
		TCodingSystem otherCS;

		p->esc = NULL;
		switch (ch) {
		case 0x05:	/* 0/5 designate private coding system */
			en = tcaps_find_entry(p->caps, "encoding.", arg);
			if (en == NULL) {
				sprintf(enbuf, "other,%s,iconv,UTF-8",
					arg);
				en = enbuf;
			}
			if (tvterm_parse_otherCS(en, &otherCS)) {
				tvterm_switch_to_otherCS(p, &otherCS);
			}
			break;
		}
		argidx = 0;
		memset(arg, 0, sizeof(arg));
	}
}
#endif

/* Note:
* KON2 Support
* ESC ISO_94_TO_G0 U  ==> GRAPHIC FONT SET
* ESC ISO_94_TO_G1 U  ==> ISO 8859-1 ?
* ESC ISO_94_TO_G1 0  ==> GRAPHIC FONT SET
*/

static void tvterm_invoke_gx(TVterm* p, TFontSpec* fs, u_int n)
{
	fs->invokedGn = n;
	fs->idx = p->gIdx[fs->invokedGn];
	fs->type = gFont[fs->idx].fsignature;
	fs->half = gFont[fs->idx].fhalf;
}

static void tvterm_re_invoke_gx(TVterm* p, TFontSpec* fs)
{
	fs->idx = p->gIdx[fs->invokedGn];
	fs->type = gFont[fs->idx].fsignature;
	fs->half = gFont[fs->idx].fhalf;
}

static int tvterm_find_font_index(int fsig)
{
	int i;
	for (i = 0; gFont[i].fsignature; i++) {
		if (gFont[i].fsignature == fsig) {
			return i;
		}
	}
	return -1;
}

static void tvterm_esc_designate_font(TVterm* p, u_char ch)
{
	int i;
	if (!tvterm_is_ISO2022(p)) {
		p->esc = NULL;
		return;
	}

	if ((i = tvterm_find_font_index(ch | p->escSignature)) >= 0) {
		p->gIdx[p->escGn] = i;
		tvterm_re_invoke_gx(p, &(p->gl));
		tvterm_re_invoke_gx(p, &(p->gr));
		p->tgl = p->gl; p->tgr = p->gr;
	}
	p->esc = NULL;
}

static void tvterm_esc_designate_otherCS(TVterm* p, u_char ch)
{
	switch (ch) {
	case 0x40:
		/* reset ISO-2022 */
		tvterm_switch_to_ISO2022(p);
		break;
#ifdef JFB_UTF8
	case 0x47: /* XXX: designate and invoke UTF-8 coding */
		tvterm_switch_to_UTF8(p);
		break;
#endif
	}
	p->esc = NULL;
}

static void tvterm_esc_traditional_multibyte_fix(TVterm* p, u_char ch)
{
	if (ch == 0x40 || ch == 0x41 || ch == 0x42) {
		tvterm_esc_designate_font(p, ch);
	} else {
		tvterm_esc_start(p, ch);
	}
}


void tvterm_show_sequence(FILE *tf, TCaps *cap, const char *en)
{
	static const char *invokeL[] = {"\017", "\016", "\033n", "\033o"};
	static const char *invokeR[] = {"", "\033~", "\033}", "\033|"};
	static const char csr4[] = {ISO_GZD4, ISO_G1D4, ISO_G2D4, ISO_G3D4};
	static const char csr6[] = {MULE__GZD6, ISO_G1D6, ISO_G2D6, ISO_G3D6};
	TCsv farg;
	int i;
	const char *g;
	int idx[6];

	tcsv_init(&farg, en);
	g = tcsv_get_token(&farg);
#ifdef JFB_OTHER_CODING_SYSTEM
	if (strcmp(g, "other") == 0) {
		TCodingSystem otherCS;
		if (tvterm_parse_otherCS(en, &otherCS)) {
			fprintf(tf, "%s%s\005", "\033]", otherCS.fromcode);
			free(otherCS.fromcode);
			free(otherCS.tocode);
		}
		goto end;
	}
#endif
#ifdef JFB_UTF8
	if (strcmp(g, "UTF-8") == 0) {
		fprintf(tf, "%s", "\033%G");
		goto end;
	}
#endif
	/* ISO-2022 */
	fprintf(tf, "%s", "\033%@");
	if (tvterm_parse_encoding(en, idx) == 0) {
		print_warn("ENCODING : BAD FORMAT : %s\n", en);
		goto end;
	}
	/* GL */
	fprintf(tf, "%s", invokeL[idx[0]]);
	/* GR */
	fprintf(tf, "%s", invokeR[idx[1]]);
	/* G0 .. G3 */
	for (i = 0; i < 4; i++) {
		TFont *f;
		u_int n;
		char c;
		u_int f1, f2;
		f = &gFont[idx[2+i]];
		n = f->fsignature;
		c = n & 255;
		f1 = n & TFONT_FT_DOUBLE;
		f2 = n & TFONT_FT_96CHAR;
		fprintf(tf, "\033%s%c%c", (f1 ? "$" : ""), 
			(f2 ? csr6[i] : csr4[i]), c);
	}
end:
	tcsv_final(&farg);
	return;
}

