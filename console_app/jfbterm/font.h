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

#ifndef INCLUDE_FONT_H
#define INCLUDE_FONT_H

#include <sys/types.h>

#include "getcap.h"

#define TAPP_ERR_FLDD	1		/* 指定のフォントセットは設定済み */

#define TFONT_FT_SINGLE		0x00000000	/* 1 byte character set */
#define TFONT_FT_DOUBLE		0x01000000	/* 2 byte character set */
#define TFONT_FT_94CHAR		0x00000000	/* 94 or 94^n */
#define TFONT_FT_96CHAR		0x02000000	/* 96 or 96^n */
#define TFONT_FT_OTHER		0x10000000	/* other coding system */

#define TFONT_OWNER		0x00	/* この構造体がglyphを支配 */
#define TFONT_ALIAS		0x01	/* 別の構造体のglyphを参照 */

typedef enum {
	FH_LEFT,
	FH_RIGHT,
	FH_UNI,
} FONTSET_HALF;

typedef struct Raw_TFont {
	const u_char* (*conv)(struct Raw_TFont* p, u_int c, u_int* width);
	/* --- */
	const char* name;
	u_int width;		/* 一文字あたりの水平ドット数 */
	u_int height;		/* 一文字あたりの垂直ドット数 */
	/* */
	u_int fsignature;
	FONTSET_HALF fhalf;
	u_char aliasF;
	/* */
	u_char** glyph;		/* ビットマップ中の各glyph の先頭 */
	u_int* glyph_width;
	u_char* dglyph;		/* 対応するglyphが存在しない時のglyph */
	u_char* bitmap;		/* ビットマップ */
	u_int colf; 
	u_int coll;
	u_int rowf;
	u_int rowl;
	u_int colspan;		/* = coll-colf+1; */
	u_int bytew;		/* 一文字の水平１ラインのバイト数 */
	u_int bytec;		/* 一文字あたりのバイト数 */
} TFont;


const u_char* tfont_default_glyph(TFont* p, u_int c, u_int *width);
const u_char* tfont_standard_glyph(TFont* p, u_int c, u_int *width);
void tfont_final(TFont* p);
void tfont_ary_final(void);
void tfont_init(TFont* p);

void tfont_setup_fontlist(TCapValue* values);
int tfont_is_valid(TFont* p);
int tfont_is_loaded(TFont *p);

int tfont_ary_search_idx(const char* na);
void tfont_ary_show_list(FILE* fp);

extern TFont gFont[];
extern u_int gFontsWidth;
extern u_int gFontsHeight;

#endif /* INCLUDE_FONT_H */
