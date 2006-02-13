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

#include <stdio.h>
#include <string.h>
#include <endian.h>
#include <linux/fb.h>

#include "fbdpsp.h"
#include "fbcommon.h"

#ifndef min
#  define min(a, b)   (((a)>(b))?(b):(a))
#endif

#ifdef JFB_2BPP
/* 2bpp */

static u_char mask_2bpp_msb_left[] = {
	0xc0, 0x30, 0x0c, 0x03
};
static u_char mask_2bpp_msb_right[] = {
	0x03, 0x0c, 0x30, 0xc0
};
static u_char* mask_2bpp = mask_2bpp_msb_left;

void set_most_left(__u32 bpp, struct fb_bitfield red){
	if(red.offset != 0) return;
	if(red.length != bpp) return;
	/*
		This probing mechanism is known to work on:
		NEC MobileGear(DOS model), NEC MobileGear-II, 
		Psion s5. (but not work on Psion 5mx...)
		Call for more precise way...
	*/
	if(red.msb_right){
		mask_2bpp = mask_2bpp_msb_right;
	}
}

static u_char color_map_2bpp[] = {
	0x00,   // 0 (black)
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0xff,   // 7 (normal white)
	0x55,   // 8 (reverse underlined background)
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	0xff    // 15 (hilight white)
};

static inline u_char tfbm_select_2_color(u_int color)
{
#ifdef JFB_REVERSEVIDEO
	return (~ color_map_2bpp[color & 0xf]);
#else
	return (color_map_2bpp[color & 0xf]);
#endif
}

static inline void set_pixel_2bpp_packed(
	u_char *p, u_int x, u_int icol)
{
	// p points byte to be written
	u_int b = x / 4;        // Byte offset
	p[b] = ((p[b] & ~mask_2bpp[x & 3]) | (mask_2bpp[x & 3] & icol));
}

static inline void set_byte_2bpp_packed(
	u_char *bp, u_int icol)
{
	*bp = icol;
}

static inline void reverse_pixel_2bpp_packed(
	u_char *p, u_int x, u_int icol)
{
	*p = (*p & ~mask_2bpp[x & 3]) | ((*p ^ icol) & mask_2bpp[x & 3]);
}

void tfbm_fill_rect_2bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	u_int y;
	u_int icol = tfbm_select_2_color(color);
	for (y = sy ; y < sy+ly ; y++) {
		u_char *line = p->smem + y * p->bytePerLine;
		u_char *bp = line + (sx / 4);   // byte position
		u_int xx = sx % 4;
		u_int pixels = lx;
		if (xx != 0) {
			// Fill until byte boundary
			while (xx % 4 != 0 && pixels != 0) {
				set_pixel_2bpp_packed(bp, xx++, icol);
				pixels--;
			}
			bp++;
		}
		while (pixels >= 4) {
			// Fill with byte
			set_byte_2bpp_packed(bp, icol);
			pixels -= 4;
			bp++;
		}
		xx = 0;
		while (pixels != 0) {
			// Fill rest pixels
			set_pixel_2bpp_packed(bp, xx++, icol);
			pixels--;
		}
	}
}

void tfbm_clear_all_2bpp_packed(
	TFrameBufferMemory* p, u_int color)
{
	u_int icol = tfbm_select_2_color(color);
	memset(p->smem, icol, p->slen);
}

void tfbm_overlay_2bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color)
{
	u_int y;
	u_char* wp;
	const u_char* tps;
	u_int i;
	u_int sb;
	u_int icol = tfbm_select_2_color(color);

	for (y = yd ; y < yd+ly ; y++) {
		u_int xx = xd % 4;
		tps = ps;
		wp = p->smem + y * p->bytePerLine + (xd / 4);
		for (i = lx ; i >= 8 ; i -= 8) {
			sb = *tps++;
			if (sb & 0x80)
				set_pixel_2bpp_packed(wp, xx, icol);
			xx++;
			if (sb & 0x40)
				set_pixel_2bpp_packed(wp, xx, icol);
			xx++;
			if (sb & 0x20)
				set_pixel_2bpp_packed(wp, xx, icol);
			xx++;
			if (sb & 0x10)
				set_pixel_2bpp_packed(wp, xx, icol);
			xx++;
			if (sb & 0x08)
				set_pixel_2bpp_packed(wp, xx, icol);
			xx++;
			if (sb & 0x04)
				set_pixel_2bpp_packed(wp, xx, icol);
			xx++;
			if (sb & 0x02)
				set_pixel_2bpp_packed(wp, xx, icol);
			xx++;
			if (sb & 0x01)
				set_pixel_2bpp_packed(wp, xx, icol);
			xx++;
		}
		if (i) {
			sb = *tps++;
			switch (i) {
			case 7:
				if (sb & 0x02)
					set_pixel_2bpp_packed(wp, xx + 6, icol);
			case 6:
				if (sb & 0x04)
					set_pixel_2bpp_packed(wp, xx + 5, icol);
			case 5:
				if (sb & 0x08)
					set_pixel_2bpp_packed(wp, xx + 4, icol);
			case 4:
				if (sb & 0x10)
					set_pixel_2bpp_packed(wp, xx + 3, icol);
			case 3:
				if (sb & 0x20)
					set_pixel_2bpp_packed(wp, xx + 2, icol);
			case 2:
				if (sb & 0x40)
					set_pixel_2bpp_packed(wp, xx + 1, icol);
			case 1:
				if (sb & 0x80)
					set_pixel_2bpp_packed(wp, xx + 0, icol);
			}
		}
		ps += gap;
	}
}

void tfbm_reverse_2bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	u_int y;
	u_int x;
	u_int icol = tfbm_select_2_color(color);
	for (y = sy ; y < min(sy+ly, p->height) ; y++) {
		unsigned char *line = p->smem + y * p->bytePerLine;
		for (x = sx ; x < sx + lx ; x++) {
			unsigned char *bp = line + (x / 4);     // byte position
			reverse_pixel_2bpp_packed(bp, x, icol);
		}
	}
}

#endif /* JFB_2BPP */


#ifdef JFB_8BPP
/* 8 bpp */

#ifdef EXPERMINAL
typedef u_int T_DWORD;
typedef u_short T_WORD;
typedef u_char T_BYTE;

T_DWORD mask_8bpp_packed[16] = {
#ifdef BIG_ENDIAN
	0x00000000, 0x000000FF, 0x0000FF00, 0x0000FFFF,
	0x00FF0000, 0x00FF00FF, 0x00FFFF00, 0x00FFFFFF,
	0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFF00FFFF,
	0xFFFF0000, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF,
#else
#ifdef LITTLE_ENDIAN
	0x00000000, 0xFF000000, 0x00FF0000, 0xFFFF0000,
	0x0000FF00, 0xFF00FF00, 0x00FFFF00, 0xFFFFFF00,
	0x000000FF, 0xFF0000FF, 0x00FF00FF, 0xFFFF00FF,
	0x0000FFFF, 0xFF00FFFF, 0x00FFFFFF, 0xFFFFFFFF,
#else
#error "Byte ordering have to be defined. Cannot continue."
#endif
#endif
};
#endif /* EXPERMINAL*/

void tfbm_fill_rect_8bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	u_int y;
	for (y = sy ; y < min(sy+ly, p->height) ; y++) {
#if 0
 		memset(p->smem + y * p->bytePerLine + sx, color, lx);
#else
		unsigned char *line = p->smem + y * p->bytePerLine + sx;
		u_int x;
		for (x = 0 ; x < lx ; x++) {
			line[x] = color;
		}
#endif
	}
}

void tfbm_clear_all_8bpp_packed(
	TFrameBufferMemory* p, u_int color)
{
	memset(p->smem, color, p->slen);
}

void tfbm_overlay_8bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color)
{
#ifdef EXPERMINAL
	const u_char* tps;
	u_int y;
	u_int x;
	u_int glyph;
	u_int bm;
	T_DWORD icol;
	T_DWORD *wp;

	icol = color; icol |= icol << 8; icol |= icol << 16;
	for (y = yd ; y < yd+ly ; y++) {
		tps = ps;
		wp = (T_DWORD*)(p->smem + y * p->bytePerLine + (xd & ~3u));
		glyph = *tps++;

		bm = glyph >> (xd & 0x3);
		x = lx + (xd & 0x3);
		while (x > 7) {
			*wp ^= mask_8bpp_packed[bm >> 4] & (icol ^ *wp);
			wp++;
			*wp ^= mask_8bpp_packed[bm & 0xF] & (icol ^ *wp);
			wp++;
			bm = (glyph << (8 - (xd & 0x3))) & 0xFF;
			glyph = *tps++;
			bm |= glyph >> (xd & 0x3);
			x -= 8;
		}
		if (x > 0) {
			bm &= 0xff00 >> x;
			*wp ^= mask_8bpp_packed[bm >> 4] & (icol ^ *wp);
			if (x > 4) {
				wp++;
				*wp ^= mask_8bpp_packed[bm & 0xF] & (icol ^ *wp);
			}
		}
		ps += gap;
	}
#else
	u_int y;
	u_char* wp;
	const u_char* tps;
	u_int i;
	u_int sb;

	for (y = yd ; y < yd+ly ; y++) {
		tps = ps;
		wp = p->smem + y * p->bytePerLine + xd;
		for (i = lx ; i >= 8 ; i -= 8) {
			sb = *tps++;
			if (sb & 0x80) wp[0] = color;
			if (sb & 0x40) wp[1] = color;
			if (sb & 0x20) wp[2] = color;
			if (sb & 0x10) wp[3] = color;
			if (sb & 0x08) wp[4] = color;
			if (sb & 0x04) wp[5] = color;
			if (sb & 0x02) wp[6] = color;
			if (sb & 0x01) wp[7] = color;
			wp += 8;
		}
		if (i) {
			sb = *tps++;
			switch (i) {
			case 7:	if (sb & 0x02) wp[6] = color;
			case 6:	if (sb & 0x04) wp[5] = color;
			case 5:	if (sb & 0x08) wp[4] = color;
			case 4:	if (sb & 0x10) wp[3] = color;
			case 3:	if (sb & 0x20) wp[2] = color;
			case 2:	if (sb & 0x40) wp[1] = color;
			case 1:	if (sb & 0x80) wp[0] = color;
			}
		}
		ps += gap;
	}
#endif
}

void tfbm_reverse_8bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
#ifdef EXPERMINAL
	u_int y;
	u_int x;
	u_int bm;
	T_DWORD icol;
	T_DWORD *wp;

	icol = color; icol |= icol << 8; icol |= icol << 16;
	for (y = sy ; y < min(sy+ly, p->height) ; y++) {
		wp = (T_DWORD*)(p->smem + y * p->bytePerLine + (sx & ~3u));
		bm = 0xF >> (sx & 3);
		x = lx + (sx & 3);
		while (x > 3) {
			*wp++ ^= mask_8bpp_packed[bm] & icol;
			bm = 0xF;
			x -= 4;
		}
		if (x > 0) {
			bm &= 0xf0 >> x;
			*wp ^= mask_8bpp_packed[bm] & icol;
		}
	}
#else
	u_int y;
	u_int x;
	for (y = sy ; y < min(sy+ly, p->height) ; y++) {
		for (x = sx ; x < sx+lx ; x++) {
			p->smem[y * p->bytePerLine + x] ^= color;
		}
	}
#endif
}
#endif /* JFB_8BPP */


#ifdef JFB_15BPP
/* 15 bpp */
void tfbm_fill_rect_15bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	u_int	x,y;
	u_short	*d;
	u_short icol;

	icol = tfbm_select_16_color(color);
	for (y = sy ; y < min(sy+ly, p->height) ; y++) {
		d=(u_short*)(p->smem + y * p->bytePerLine + sx * 2);
		for(x=0;x<lx;x++){
			*d = icol;
			d++;
		}
	}
}

void tfbm_clear_all_15bpp_packed(
	TFrameBufferMemory* p, u_int color)
{
	u_int lp;
	u_short	*d=(u_short*)(p->smem);
	u_short icol;

	icol = tfbm_select_16_color(color);
	for(lp=0;lp<((p->slen)/2);lp++){
		d[lp]=icol;
	}
}

void tfbm_overlay_15bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color)
{
	u_int y;
	u_short* wp;
	const u_char* tps;
	u_int i;
	u_int sb;
	u_short icol;

	icol = tfbm_select_16_color(color);

	for (y = yd ; y < yd+ly ; y++) {
		tps = ps;
		wp = (u_short*)(p->smem + y * p->bytePerLine + xd * 2);
		for (i = lx ; i >= 8 ; i -= 8) {
			sb = *tps++;
			if (sb & 0x80) wp[0] = icol;
			if (sb & 0x40) wp[1] = icol;
			if (sb & 0x20) wp[2] = icol;
			if (sb & 0x10) wp[3] = icol;
			if (sb & 0x08) wp[4] = icol;
			if (sb & 0x04) wp[5] = icol;
			if (sb & 0x02) wp[6] = icol;
			if (sb & 0x01) wp[7] = icol;
			wp += 8;
		}
		if (i) {
			sb = *tps++;
			switch (i) {
			case 7:	if (sb & 0x02) wp[6] = icol;
			case 6:	if (sb & 0x04) wp[5] = icol;
			case 5:	if (sb & 0x08) wp[4] = icol;
			case 4:	if (sb & 0x10) wp[3] = icol;
			case 3:	if (sb & 0x20) wp[2] = icol;
			case 2:	if (sb & 0x40) wp[1] = icol;
			case 1:	if (sb & 0x80) wp[0] = icol;
			}
		}
		ps += gap;
	}
}

void tfbm_reverse_15bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	u_int x,y;
	u_short	*d;
	u_short icol;

	icol = tfbm_select_16_color(color);
	for (y = sy ; y < min(sy+ly, p->height) ; y++) {
		d = (u_short*)(p->smem + y * p->bytePerLine + sx * 2);
		for (x = 0 ; x < lx ; x++) {
			d[x] ^= icol;
		}
	}
}
#endif /* JFBTERM_15BPP */

#ifdef JFB_16BPP
/* 16 bpp */
void tfbm_fill_rect_16bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	u_int	x,y;
	u_short	*d;
	u_short icol;

	icol = tfbm_select_16_color(color);
	for (y = sy ; y < min(sy+ly, p->height) ; y++) {
		d=(u_short*)(p->smem + y * p->bytePerLine + sx * 2);
		for(x=0;x<lx;x++){
			*d = icol;
			d++;
		}
	}
}

void tfbm_clear_all_16bpp_packed(
	TFrameBufferMemory* p, u_int color)
{
	u_int lp;
	u_short	*d=(u_short*)(p->smem);
	u_short icol;

	icol = tfbm_select_16_color(color);
	for(lp=0;lp<((p->slen)/2);lp++){
		d[lp]=icol;
	}
}

void tfbm_overlay_16bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color)
{
	u_int y;
	u_short* wp;
	const u_char* tps;
	u_int i;
	u_int sb;
	u_short icol;

	icol = tfbm_select_16_color(color);
	for (y = yd ; y < yd+ly ; y++) {
		tps = ps;
		wp = (u_short*)(p->smem + y * p->bytePerLine + xd * 2);
		for (i = lx ; i >= 8 ; i -= 8) {
			sb = *tps++;
			if (sb & 0x80) wp[0] = icol;
			if (sb & 0x40) wp[1] = icol;
			if (sb & 0x20) wp[2] = icol;
			if (sb & 0x10) wp[3] = icol;
			if (sb & 0x08) wp[4] = icol;
			if (sb & 0x04) wp[5] = icol;
			if (sb & 0x02) wp[6] = icol;
			if (sb & 0x01) wp[7] = icol;
			wp += 8;
		}
		if (i) {
			sb = *tps++;
			switch (i) {
			case 7:	if (sb & 0x02) wp[6] = icol;
			case 6:	if (sb & 0x04) wp[5] = icol;
			case 5:	if (sb & 0x08) wp[4] = icol;
			case 4:	if (sb & 0x10) wp[3] = icol;
			case 3:	if (sb & 0x20) wp[2] = icol;
			case 2:	if (sb & 0x40) wp[1] = icol;
			case 1:	if (sb & 0x80) wp[0] = icol;
			}
		}
		ps += gap;
	}
}

#ifndef min
#  define min(a, b)    (((a) > (b))?(b):(a))
#endif

void tfbm_reverse_16bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	u_int x,y;
	u_short	*d;
	u_short icol;

	icol = tfbm_select_16_color(color);
	for (y = sy ; y < min(sy+ly, p->height) ; y++) {
		d = (u_short*)(p->smem + y * p->bytePerLine + sx * 2);
		for (x = 0 ; x < lx ; x++) {
			d[x] ^= icol;
		}
	}
}
#endif /* JFB_16BPP */

#ifdef JFB_24BPP
/* 24 bpp */
void tfbm_fill_rect_24bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	u_int	x,y;
	u_char*	d;
	u_int t, m, b;
	u_int icol;

	icol = tfbm_select_32_color(color);
#if __BYTE_ORDER == __BIG_ENDIAN
	b = 0xff & (icol >> 16); m = 0xff & (icol >> 8); t = 0xff & icol;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	t = 0xff & (icol >> 16); m = 0xff & (icol >> 8); b = 0xff & icol;
#else
#	error FIXME : No endianness ?
#endif 

	for (y = sy ; y < sy+ly ; y++) {
		d=p->smem + y * p->bytePerLine + sx * 3;
		for(x=0;x<lx;x++){
			d[0] = t;
			d[1] = m;
			d[2] = b;
			d+=3;
		}
	}
}

void tfbm_clear_all_24bpp_packed(
	TFrameBufferMemory* p, u_int color)
{
	u_int lp;
	u_int t, m, b;
	u_int icol;

	icol = tfbm_select_32_color(color);
#if __BYTE_ORDER == __BIG_ENDIAN
	b = 0xff & (icol >> 16); m = 0xff & (icol >> 8); t = 0xff & icol;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	t = 0xff & (icol >> 16); m = 0xff & (icol >> 8); b = 0xff & icol;
#else
#	error FIXME : No endianness ?
#endif 

	for(lp=0;lp<((p->slen)-2);lp+=3){
		p->smem[lp]   = t;
		p->smem[lp+1] = m;
		p->smem[lp+2] = b;
	}
}

void tfbm_overlay_24bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color)
{
	u_int y,i,sb;
	u_char* wp;
	const u_char* tps;
	u_int t, m, b;
	u_int icol;

	icol = tfbm_select_32_color(color);
#if __BYTE_ORDER == __BIG_ENDIAN
	b = 0xff & (icol >> 16); m = 0xff & (icol >> 8); t = 0xff & icol;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	t = 0xff & (icol >> 16); m = 0xff & (icol >> 8); b = 0xff & icol;
#else
#	error FIXME : No endianness ?
#endif 

	for (y = yd ; y < yd+ly ; y++) {
		tps = ps;
		wp = p->smem + y * p->bytePerLine + xd * 3;
		for (i = lx ; i >= 8 ; i -= 8) {
			sb = *tps++;
			if (sb & 0x80) wp[0] = t,wp[1] = m,wp[2] = b;
			if (sb & 0x40) wp[3] = t,wp[4] = m,wp[5] = b;
			if (sb & 0x20) wp[6] = t,wp[7] = m,wp[8] = b;
			if (sb & 0x10) wp[9] = t,wp[10] = m,wp[11] = b;
			if (sb & 0x08) wp[12] = t,wp[13] = m,wp[14] = b;
			if (sb & 0x04) wp[15] = t,wp[16] = m,wp[17] = b;
			if (sb & 0x02) wp[18] = t,wp[19] = m,wp[20] = b;
			if (sb & 0x01) wp[21] = t,wp[22] = m,wp[23] = b;
			wp += 24;
		}
		if (i) {
			sb = *tps++;
			switch (i) {
			case 7:	if (sb & 0x02) wp[18] = t,wp[19] = m,wp[20] = b;
			case 6:	if (sb & 0x04) wp[15] = t,wp[16] = m,wp[17] = b;
			case 5:	if (sb & 0x08) wp[12] = t,wp[13] = m,wp[14] = b;
			case 4:	if (sb & 0x10) wp[9] = t,wp[10] = m,wp[11] = b;
			case 3:	if (sb & 0x20) wp[6] = t,wp[7] = m,wp[8] = b;
			case 2:	if (sb & 0x40) wp[3] = t,wp[4] = m,wp[5] = b;
			case 1:	if (sb & 0x80) wp[0] = t,wp[1] = m,wp[2] = b;
			}
		}
		ps += gap;
	}
}

void tfbm_reverse_24bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	u_int x,y;
	u_int t, m, b;
	u_int icol;

	icol = tfbm_select_32_color(color);
#if __BYTE_ORDER == __BIG_ENDIAN
	b = 0xff & (icol >> 16); m = 0xff & (icol >> 8); t = 0xff & icol;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	t = 0xff & (icol >> 16); m = 0xff & (icol >> 8); b = 0xff & icol;
#else
#	error FIXME : No endianness ?
#endif 

	for (y = sy ; y < sy+ly ; y++) {
		for (x = 0 ; x < (lx*3) ; x+=3) {
			p->smem[y * p->bytePerLine + sx * 3 + x] ^= t;
			p->smem[y * p->bytePerLine + sx * 3 + x + 1] ^= m;
			p->smem[y * p->bytePerLine + sx * 3 + x + 2] ^= b;
		}
	}
}
#endif /* JFB_24BPP */

#ifdef JFB_32BPP
/* 32 bpp */
void tfbm_fill_rect_32bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	u_int x,y;
	u_int icol;
	u_int *d;

	icol = tfbm_select_32_color(color);
	for (y = sy ; y < sy+ly ; y++) {
		d=(u_int*)(p->smem + y * p->bytePerLine + sx * 4);
		for(x = 0 ; x < lx ; x++){
			*d++ = icol;
		}
	}
}

void tfbm_clear_all_32bpp_packed(
	TFrameBufferMemory* p, u_int color)
{
	u_int lp;
	u_int	*d=(u_int*)(p->smem);
	u_int icol;

	icol = tfbm_select_32_color(color);
	for(lp = 0 ; lp < ((p->slen)/2) ; lp++) {
		d[lp]=icol;
	}
}

void tfbm_overlay_32bpp_packed(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color)
{
	u_int y;
	u_int* wp;
	const u_char* tps;
	u_int i;
	u_int sb;
	u_int icol;

	icol = tfbm_select_32_color(color);
	for (y = yd ; y < yd+ly ; y++) {
		tps = ps;
		wp = (u_int*)(p->smem + y * p->bytePerLine + xd * 4);
		for (i = lx ; i >= 8 ; i -= 8) {
			sb = *tps++;
			if (sb & 0x80) wp[0] = icol;
			if (sb & 0x40) wp[1] = icol;
			if (sb & 0x20) wp[2] = icol;
			if (sb & 0x10) wp[3] = icol;
			if (sb & 0x08) wp[4] = icol;
			if (sb & 0x04) wp[5] = icol;
			if (sb & 0x02) wp[6] = icol;
			if (sb & 0x01) wp[7] = icol;
			wp += 8;
		}
		if (i) {
			sb = *tps++;
			switch (i) {
			case 7:	if (sb & 0x02) wp[6] = icol;
			case 6:	if (sb & 0x04) wp[5] = icol;
			case 5:	if (sb & 0x08) wp[4] = icol;
			case 4:	if (sb & 0x10) wp[3] = icol;
			case 3:	if (sb & 0x20) wp[2] = icol;
			case 2:	if (sb & 0x40) wp[1] = icol;
			case 1:	if (sb & 0x80) wp[0] = icol;
			}
		}
		ps += gap;
	}
}

void tfbm_reverse_32bpp_packed(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	u_int x,y;
	u_int *d;
	u_int icol;

	icol = tfbm_select_32_color(color);
	for (y = sy ; y < sy+ly ; y++) {
		d = (u_int*)(p->smem + y * p->bytePerLine + sx * 4);
		for (x = 0 ; x < lx ; x++) {
			d[x] ^= icol;
		}
	}
}
#endif /* JFB_32BPP */

#ifdef JFB_VGA16FB

#include <asm/io.h>
#include <asm/system.h>

#define GRAPHICS_ADDR_REG 0x3ce         /* Graphics address register. */
#define GRAPHICS_DATA_REG 0x3cf         /* Graphics data register. */
#define SET_RESET_INDEX 0               /* Set/Reset Register index. */
#define ENABLE_SET_RESET_INDEX 1        /* Enable Set/Reset Register index. */
#define DATA_ROTATE_INDEX 3             /* Data Rotate Register index. */
#define GRAPHICS_MODE_INDEX 5           /* Graphics Mode Register index. */
#define BIT_MASK_INDEX 8                /* Bit Mask Register index. */

static inline void rmw(volatile char *p)
{
        *p |= 1;
}
static inline void setmode(int mode)
{
        outb(GRAPHICS_MODE_INDEX, GRAPHICS_ADDR_REG);
        outb(mode, GRAPHICS_DATA_REG);
}
static inline void selectmask(void)
{
        outb(BIT_MASK_INDEX, GRAPHICS_ADDR_REG);
}
static inline void setmask(int mask)
{
        outb(mask, GRAPHICS_DATA_REG);
}
static inline void setop(int op)
{
        outb(DATA_ROTATE_INDEX, GRAPHICS_ADDR_REG);
        outb(op, GRAPHICS_DATA_REG);
}
static inline void setsr(int sr)
{
        outb(ENABLE_SET_RESET_INDEX, GRAPHICS_ADDR_REG);
        outb(sr, GRAPHICS_DATA_REG);
}
static inline void setcolor(int color)
{
        outb(SET_RESET_INDEX, GRAPHICS_ADDR_REG);
        outb(color, GRAPHICS_DATA_REG);
}
static inline void setindex(int idx)
{
        outb(idx, GRAPHICS_ADDR_REG);
}
static inline void setrplane(int idx)
{
        outb(0x4, GRAPHICS_ADDR_REG);
        outb(idx, GRAPHICS_DATA_REG);
}

void tfbm_fill_rect_vga16(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	int y;
	unsigned char *wp;
	unsigned char mask;
	u_int sofs = sx % 8;
	u_int eofs = (sx+lx) % 8;
	unsigned char emask = 0xff00 >> eofs;
	u_int ix;

	setmode(0);
	setop(0);
	setsr(0xf);
	setcolor(color);
	selectmask();
	setmask(0xff);

	for (y = sy ; y < sy+ly ; y++) {
		wp = p->smem + y*p->bytePerLine + sx/8;
		mask = 0xff >> sofs;

		for (ix = lx + sofs ; ix > 7 ; ix -= 8) {
			setmask(mask);
			rmw(wp);
			wp++;
			mask = 0xff;
		}
		if (ix) {
			setmask(mask & emask);
			rmw(wp);
		}
	}
}

void tfbm_clear_all_vga16(
	TFrameBufferMemory* p, u_int color)
{
	setmode(0);
	setop(0);
	setsr(0xf);
	setcolor(color);
	selectmask();

	setmask(0xff);

	memset(p->smem, 0, p->slen);
}

void tfbm_overlay_vga16(
	TFrameBufferMemory* p,
	u_int xd, u_int yd,
	const u_char* ps, u_int lx, u_int ly, u_int gap, u_int color)
{
	int y;
	volatile unsigned char *wp;
	const unsigned char *tps;
	volatile unsigned char mask;

	u_int sofs = xd & 7;
	u_int eofs = (xd+lx) & 7;
	u_int xds8 = xd/8;
	u_int xde8 = (xd+lx+7)/8;
	unsigned char emask;
	unsigned char smask;
	u_int ix;

	eofs = eofs ? eofs : 8;
	emask = 0xff00 >> eofs;
	smask = 0xff << sofs;

	setmode(0);
	setop(0);
	setsr(0xf);
	setcolor(color);
	selectmask();

	for (y = yd ; y < yd+ly ; y++) {
		tps = ps;
		wp = p->smem + y*p->bytePerLine + xds8;

		if (xds8+1 == xde8) {
			mask = ((*tps >> sofs) & emask);
			setmask(mask);
			rmw(wp);
		} else {
			mask = (*tps++ >> sofs);
			setmask(mask);
			rmw(wp);
			wp++;
			for (ix = xds8+1 ; ix < xde8-1 ; ix++) {
				mask = (*tps >> sofs) | (*(tps-1) << (8-sofs));
				tps++;
				setmask(mask);
				rmw(wp);
				wp++;
			}
			if (eofs <= sofs) {
				if (sofs == 0) {
					mask = *tps;
				} else {
					mask = (*(tps-1) << (8-sofs)) & emask;
				}
			} else {
				mask = ((*tps >> sofs) |
					(*(tps-1) << (8-sofs))) & emask;
			}
			setmask(mask);
			rmw(wp);
		}

		ps += gap;
	}
}

void tfbm_reverse_vga16(
	TFrameBufferMemory* p,
	u_int sx, u_int sy, u_int lx, u_int ly, u_int color)
{
	int y;
	unsigned char *wp;
	unsigned char mask;
	u_int sofs = sx % 8;
	u_int eofs = (sx+lx) % 8;
	unsigned char emask = 0xff00 >> eofs;
	u_int ix;

	setmode(0);
	setop(0x18);
	setsr(0xf);
	setcolor(color);
	selectmask();
  
	for (y = sy ; y < sy+ly ; y++) {
		wp = p->smem + y*p->bytePerLine + sx/8;
		mask = 0xff >> sofs;

		for (ix = lx + sofs ; ix > 7 ; ix -= 8) {
			setmask(mask);
			rmw(wp);
			wp++;
			mask = 0xff;
		}
		if (ix) {
			setmask(mask & emask);
			rmw(wp);
		}
	}
}

#endif /* JFB_VGA16 */
