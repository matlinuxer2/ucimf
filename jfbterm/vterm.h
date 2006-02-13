/*
 * JFBTERM -
 * Copyright (c) 2003  Fumitoshi UKAI <ukai@debian.or.jp>
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
 * THIS SOFTWARE IS PROVIDED BY NORITOSHI MASUICH AND TAKASHI MANABE ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
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

#ifndef INCLUDE_VTERM_H
#define INCLUDE_VTERM_H

#include	<sys/types.h>

#include	"config.h"
#include	"mytypes.h"
#include	"pen.h"
#include	"font.h"

#ifdef JFB_OTHER_CODING_SYSTEM
#include <iconv.h>
#endif

#define	LEN_REPORT	9


typedef struct Raw_TFontSpec {
	u_int invokedGn;	/* �ƤӽФ�����Ƥ��� Gn : n = 0..3 */
	u_int idx;		/* ʸ�������gFont[]�Ǥΰ��� */
	u_int type; 		/* ʸ������ζ�ʬ */
	FONTSET_HALF half;	/* ʸ�������G0,G1 �Τɤ����ȤäƤ��뤫 */
} TFontSpec;

#ifdef JFB_OTHER_CODING_SYSTEM
typedef struct Raw_TCodingSystem {
	u_int fch;
	/* iconv state */
	char *fromcode;
	char *tocode;
	iconv_t cd;
#define MAX_MULTIBYTE_CHARLEN	6
	char inbuf[MAX_MULTIBYTE_CHARLEN];
	int inbuflen;
	char outbuf[MAX_MULTIBYTE_CHARLEN];

	/* saved state */
	u_int gSavedL;
	u_int gSavedR;
	u_int gSavedIdx[4];
	u_int utf8SavedIdx;
} TCodingSystem;

#endif

typedef struct Raw_TCursor {
	u_int x;
	u_int y;
	TBool on;
	TBool shown;
	TBool wide;
	u_int width;
	u_int height;

} TCursor;

typedef struct Raw_TVterm {
	struct Raw_TTerm* term;
	int xmax;
	int ymax;
	int ymin;
	int xcap;			/* �ϡ���Ū��1�Ԥ�����ʸ���� */
	int ycap;			/* �ϡ���Ū�ʹԿ� */
	u_int tab;			/* TAB ������ */
	
	TPen pen;
	TPen* savedPen;
	TPen* savedPenSL;		/* ���ơ������饤���� */
	int scroll;			/* ��������Կ� */
	/* -- */

	u_char knj1;			/* first byte of 2 byte code */
	FONTSET_HALF knj1h;
	u_int knj1idx;

	/* ISO-2022 �б� */
	u_int escSignature;
	u_int escGn;
	TFontSpec tgl;	/* ����ʸ����GL�ΤȤ��˻Ȥ�ʸ������ */
	TFontSpec tgr;	/* ����ʸ����GR�ΤȤ��˻Ȥ�ʸ������ */
	TFontSpec gl;	/* GL �˸ƤӽФ���Ƥ���ʸ������ */
	TFontSpec gr;	/* GR �˸ƤӽФ���Ƥ���ʸ������ */
	u_int gIdx[4];	/* Gn �˻ؼ�����Ƥ���ʸ�������gFont[]�Ǥΰ��� */
	/* --- */
	u_int gDefaultL;
	u_int gDefaultR;
	u_int gDefaultIdx[4];
	/* --- */
	enum {
		SL_NONE,
		SL_ENTER,
		SL_LEAVE
	} sl;
#ifdef JFB_UTF8
	u_int utf8DefaultIdx;
	u_int utf8Idx;
	u_int utf8remain;
	u_int ucs2ch;
#endif
#ifdef JFB_OTHER_CODING_SYSTEM
	TCodingSystem *otherCS;
#endif
	TBool altCs;
	TCaps *caps;

	TBool soft;
	TBool wrap;
	TBool ins;			/* �����⡼�� */
	TBool active;			/* ���Υ����ߥʥ뤬�����ƥ��� */
	TBool busy;			/* �ӥ������� */
	TBool sw;
	TBool release;
	TBool textClear;
	void (*esc)(struct Raw_TVterm* p, u_char ch);
	/* �������� */
	TCursor cursor;

        /*  */
        struct winsize win;
	/* ESC Report Buffer */
	char report[LEN_REPORT];
	/* low level half */
	u_int textHead;
	u_int xcap4; /* 4 bytes ���������礷�����(xcap + 0 ... 3) */
	u_int tsize; /* == xcap4 * ycap */
	/* */
	u_int* text; /* 1 ʸ�������� 4 bytes */
	u_char* attr;
	u_char* flag;
} TVterm;

void tvterm_insert_n_chars(TVterm* p, int n);
void tvterm_delete_n_chars(TVterm* p, int n);
void tvterm_text_scroll_down(TVterm* p, int line);
void tvterm_text_scroll_up(TVterm* p, int line);
void tvterm_text_move_down(TVterm* p, int top, int btm, int line);
void tvterm_text_move_up(TVterm* p, int top, int btm, int line);
void tvterm_text_clear_eol(TVterm* p, u_char mode);
void tvterm_text_clear_eos(TVterm* p, u_char mode);
void tvterm_wput(TVterm* p, u_int idx, u_char ch1, u_char ch2);
void tvterm_sput(TVterm* p, u_int idx, u_char ch);
#ifdef JFB_UTF8
void tvterm_uput1(TVterm* p, u_int idx, u_int ch);
void tvterm_uput2(TVterm* p, u_int idx, u_int ch);
#endif
void tvterm_text_clear_all(TVterm* p);

void tvterm_emulate(TVterm* p, const char *buff, int nchars);
void tvterm_refresh(TVterm* p);

void tvterm_init(TVterm* p, struct Raw_TTerm* tp, u_int hx, u_int hy, TCaps *caps, const char* en);
void tvterm_start(TVterm* p);
void tvterm_final(TVterm* p);

void tvterm_unregister_signal(void);
void tvterm_register_signal(TVterm* p);

void tvterm_show_sequence(FILE *fp, TCaps *cap, const char *en);


/*

  flagBuff:
  |      7|      6|      5|4||3|2|1|0|
  |CLEAN_S|LATCH_2|LATCH_1| ||<----->|
  |0=latch|  byte2|  byte1| ||   LANG|

  */

#endif /* INCLUDE_VTERM_H */
