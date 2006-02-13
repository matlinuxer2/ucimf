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

#ifndef INCLUDE_FBDPSP_H
#define INCLUDE_FBDPSP_H

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif


#include <sys/types.h>

#include "fbcommon.h"

#ifdef JFB_2BPP
void tfbm_fill_rect_2bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
void tfbm_overlay_2bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color);
	void tfbm_clear_all_2bpp_packed(
	struct Raw_TFrameBufferMemory* p, u_int color);
void tfbm_reverse_2bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
#endif /* JFB_2BPP */

#ifdef JFB_MONO
#	include <fbmonop.h>
#endif

#ifdef JFB_8BPP
void tfbm_fill_rect_8bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
void tfbm_overlay_8bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color);
void tfbm_clear_all_8bpp_packed(
	struct Raw_TFrameBufferMemory* p, u_int color);
void tfbm_reverse_8bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
#endif /* JFB_8BPP */

#ifdef JFB_15BPP
void tfbm_fill_rect_15bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
void tfbm_overlay_15bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color);
void tfbm_clear_all_15bpp_packed(
	struct Raw_TFrameBufferMemory* p, u_int color);
void tfbm_reverse_15bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
#endif /* JFB_15BPP */

#ifdef JFB_16BPP
void tfbm_fill_rect_16bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
void tfbm_overlay_16bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color);
void tfbm_clear_all_16bpp_packed(
	struct Raw_TFrameBufferMemory* p, u_int color);
void tfbm_reverse_16bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
#endif /* JFB_16BPP */

#ifdef JFB_24BPP
void tfbm_fill_rect_24bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
void tfbm_overlay_24bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color);
void tfbm_clear_all_24bpp_packed(
	struct Raw_TFrameBufferMemory* p, u_int color);
void tfbm_reverse_24bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
#endif /* JFB_24BPP */

#ifdef JFB_32BPP
void tfbm_fill_rect_32bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
void tfbm_overlay_32bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color);
void tfbm_clear_all_32bpp_packed(
	struct Raw_TFrameBufferMemory* p, u_int color);
void tfbm_reverse_32bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
#endif /* JFB_32BPP */

#ifdef JFB_VGA16FB
void tfbm_fill_rect_vga16(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
void tfbm_overlay_vga16(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color);
void tfbm_clear_all_vga16(
	struct Raw_TFrameBufferMemory* p, u_int color);
void tfbm_reverse_vga16(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color);
#endif /* JFB_VGA16FB */

#endif /* INCLUDE_FBDPSP_H */
