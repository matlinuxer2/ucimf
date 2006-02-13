/*
 * JFBTERM -
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

#ifndef INCLUDE_FBCOMMON_H
#define INCLUDE_FBCOMMON_H

#include <sys/types.h>

struct Raw_TFrameBufferMemory;

typedef struct Raw_TFrameBufferCapability {
	u_int bitsPerPixel;
	u_int fbType;
	u_int fbVisual;
	void (*fill)(struct Raw_TFrameBufferMemory* p, u_int x, u_int y,
			u_int lx, u_int ly, u_int color);
	void (*overlay)(struct Raw_TFrameBufferMemory* p, u_int xd, u_int yd,
			const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color);
	void (*clear_all)(struct Raw_TFrameBufferMemory* p, u_int color);
	void (*reverse)(struct Raw_TFrameBufferMemory* p, u_int x, u_int y,
			u_int lx, u_int ly, u_int color);
} TFrameBufferCapability;

typedef struct Raw_TFrameBufferMemory {
	u_int height;
	u_int width;
	u_int bytePerLine;
	/* --- */
	int fh;
	u_long sstart;
	u_long soff;
	u_long slen;
	u_long mstart;
	u_long moff;
	u_long mlen;
	u_char* smem;
	u_char* mmio;
	/* function hooks */
	TFrameBufferCapability cap;
#ifdef JFB_ENABLE_DIMMER
	u_short my_vt;
#endif
	int ttyfd;
#if 0
	(*init)(void),		 /* 初期化 */
	(*text_mode)(void),	 /* テキストモードに切替え */
	(*graph_mode)(void),	 /* グラフィックモードに切替え */
	(*wput)(u_char *code, u_char fc, u_char bc), /* 漢字出力 */
	(*sput)(u_char *code, u_char fc, u_char bc), /* ANK出力 */
	(*set_cursor_address)(struct cursorInfo *c, u_int x, u_int y),
	/* カーソル c のアドレスを (x,y) に設定 */
	(*set_address)(u_int i),
	/* 文字書き込みアドレスを i 文字目に設定 */
	(*cursor)(struct cursorInfo *),	/* カーソルをトグル */
	(*screen_saver)(bool),	 /* スクリーンブランク/アンブランク */
	(*detatch)(void),	 /* ドライバ解放 */
#endif
} TFrameBufferMemory;

extern TFrameBufferMemory gFramebuffer;

void tfbm_init(TFrameBufferMemory* p);
void tfbm_open(TFrameBufferMemory* p);
void tfbm_close(TFrameBufferMemory* p);

u_int tfbm_select_32_color(u_int);
u_short tfbm_select_16_color(u_int);

extern float fbgamma;

#endif /* INCLUDE_FBCOMMON_H */

