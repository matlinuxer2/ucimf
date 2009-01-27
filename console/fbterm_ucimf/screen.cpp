/*
 *   Copyright © 2008 dragchan <zgchan317@gmail.com>
 *   This file is part of FbTerm.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include "font.h"
#include "screen.h"

static fb_fix_screeninfo finfo;
static fb_var_screeninfo vinfo;
static unsigned bytes_per_pixel;

#include "screen_render.cpp"

DEFINE_INSTANCE(Screen)

RotateType Screen::mRotate;
unsigned Screen::mScreenw;
unsigned Screen::mScreenh;

/* is_double_width() is based on the wcwidth() implementation by
 * Markus Kuhn -- 2007-05-26 (Unicode 5.0)
 * Latest version: http://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
 */
struct interval {
	unsigned first;
	unsigned last;
};

static bool bisearch(unsigned ucs, const struct interval *table, unsigned max)
{
	unsigned min = 0;
	unsigned mid;

	if (ucs < table[0].first || ucs > table[max].last)
		return 0;
	while (max >= min) {
		mid = (min + max) / 2;
		if (ucs > table[mid].last)
			min = mid + 1;
		else if (ucs < table[mid].first)
			max = mid - 1;
		else
			return 1;
	}
	return 0;
}

bool is_double_width(unsigned ucs)
{
	static const struct interval double_width[] = {
		{ 0x1100, 0x115F}, { 0x2329, 0x232A}, { 0x2E80, 0x303E},
		{ 0x3040, 0xA4CF}, { 0xAC00, 0xD7A3}, { 0xF900, 0xFAFF},
		{ 0xFE10, 0xFE19}, { 0xFE30, 0xFE6F}, { 0xFF00, 0xFF60},
		{ 0xFFE0, 0xFFE6}, { 0x20000, 0x2FFFD}, { 0x30000, 0x3FFFD}
	};
	return bisearch(ucs, double_width, sizeof(double_width) / sizeof(struct interval) - 1);
}

Screen *Screen::createInstance()
{
	char devname[32];
	int devFd;
	for (unsigned i = 0; i < FB_MAX; i++) {
		snprintf(devname, sizeof(devname), "/dev/fb%d", i);
		devFd = open(devname, O_RDWR);
        if (devFd >= 0) break;
	}

	if (devFd < 0) {
		fprintf(stderr, "can't open framebuffer device!\n");
		return 0;
	}

	fcntl(devFd, F_SETFD, fcntl(devFd, F_GETFD) | FD_CLOEXEC);
	ioctl(devFd, FBIOGET_FSCREENINFO, &finfo);
	ioctl(devFd, FBIOGET_VSCREENINFO, &vinfo);

	if (finfo.type != FB_TYPE_PACKED_PIXELS) {
		fprintf(stderr, "unsupported framebuffer device!\n");
		return 0;
	}

	switch (vinfo.bits_per_pixel) {
	case 8:
		if (finfo.visual != FB_VISUAL_PSEUDOCOLOR) {
			fprintf(stderr, "only support pseudocolor visual with 8bpp depth!\n");
			return 0;
		}
		break;

	case 15:
	case 16:
	case 32:
		if (finfo.visual != FB_VISUAL_TRUECOLOR && finfo.visual != FB_VISUAL_DIRECTCOLOR) {
			fprintf(stderr, "only support truecolor/directcolor visual with 15/16/32bpp depth!\n");
			return 0;
		}
		break;

	default:
		fprintf(stderr, "only support framebuffer device with 8/15/16/32 color depth!\n");
		return 0;
	}

	if (mRotate == Rotate0 || mRotate == Rotate180) {
		mScreenw = vinfo.xres;
		mScreenh = vinfo.yres;
	} else {
		mScreenw = vinfo.yres;
		mScreenh = vinfo.xres;
	}
	
	if (!Font::instance()) return 0;

	if (mScreenw / W(1) == 0 || mScreenh / H(1) == 0) {
		fprintf(stderr, "font size is too huge!\n");
		return 0;
	}
	
	if (vinfo.bits_per_pixel == 15) bytes_per_pixel = 2;
	else bytes_per_pixel = (vinfo.bits_per_pixel >> 3);

	initFillDraw();	
	return new Screen(devFd);
}

Screen::Screen(int fd)
{
	mFd = fd;
	mpMemStart = (char *)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, mFd, 0);
}

Screen::~Screen()
{
	munmap(mpMemStart, finfo.smem_len);
	close(mFd);
	Font::uninstance();
}

void Screen::updateYOffset()
{
	ioctl(mFd, FBIOGET_VSCREENINFO, &vinfo);
}

void Screen::drawText(unsigned x, unsigned y, unsigned fc, unsigned bc, unsigned num, unsigned short *text, bool *dw)
{
	if (x >= mScreenw || y >= mScreenh || fc >= NR_COLORS || bc >= NR_COLORS || !num || !text || !dw) return;

	unsigned startx, startnum, fw = W(1);
	unsigned short *starttext;
	bool *startdw, draw_space = false, draw_text = false;
	

	for (; num; num--, text++, dw++, x += fw) {
		if (*text == 0x20) {
			if (draw_text) {
				draw_text = false;
				drawGlyphs(startx, y, x - startx, fc, bc, startnum - num, starttext, startdw);
			}

			if (!draw_space) {
				draw_space = true;
				startx = x;
			}
		} else {
			if (draw_space) {
				draw_space = false;
				fillRect(startx, y, x - startx, H(1), bc);
			}

			if (!draw_text) {
				draw_text = true;
				starttext = text;
				startdw = dw;
				startnum = num;
				startx = x;
			}

			if (*dw) x += fw;
		}
	}

	if (draw_text) {
		drawGlyphs(startx, y, x - startx, fc, bc, startnum - num, starttext, startdw);
	} else if (draw_space) {
		fillRect(startx, y, x - startx, H(1), bc);
	}
}

void Screen::drawGlyphs(unsigned x, unsigned y, unsigned w, unsigned fc, unsigned bc, unsigned num, unsigned short *text, bool *dw)
{
	if (Font::instance()->isMonospace()) {
		for (; num--; text++, dw++) {
			drawGlyph(x, y, fc, bc, *text, *dw, true);
			x += *dw ? W(2) : W(1);
		}
	} else {
		fillRect(x, y, w, H(1), bc);

		for (; num--; text++, dw++) {
			x += drawGlyph(x, y, fc, bc, *text, *dw, false);
		}
	}
}

void Screen::rotateRect(unsigned &x, unsigned &y, unsigned &w, unsigned &h)
{
	unsigned tmp;
	switch (mRotate) {
	case Rotate0:
		break;

	case Rotate90:
		tmp = x;
		x = mScreenh - y - h;
		y = tmp;

		tmp = w;
		w = h;
		h = tmp;
		break;

	case Rotate180:
		x = mScreenw - x - w;
		y = mScreenh - y - h;
		break;

	case Rotate270:
		tmp = y;
		y = mScreenw - x - w;
		x = tmp;

		tmp = w;
		w = h;
		h = tmp;
		break;
	}
}

void Screen::rotatePoint(unsigned W, unsigned H, unsigned &x, unsigned &y)
{
	unsigned tmp;
	switch (mRotate) {
	case Rotate0:
		break;

	case Rotate90:
		tmp = x;
		x = H - y - 1;
		y = tmp;
		break;

	case Rotate180:
		x = W - x - 1;
		y = H - y - 1;
		break;

	case Rotate270:
		tmp = y;
		y = W - x - 1;
		x = tmp;
		break;
	}
}

static inline unsigned offsetY(unsigned y)
{
	y += vinfo.yoffset;
	if (y >= vinfo.yres_virtual) y -= vinfo.yres_virtual;
	return y;
}

void Screen::fillRect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned color)
{
	if (x >= mScreenw || y >= mScreenh || !w || !h || color >= NR_COLORS) return;
	if (x + w > mScreenw) w = mScreenw - x;
	if (y + h > mScreenh) h = mScreenh - y;

	rotateRect(x, y, w, h);

	char *dst8 = mpMemStart + x * bytes_per_pixel;
	for(; h--; y++)
	{
		char *dst = dst8 + offsetY(y) * finfo.line_length;
		fill(dst, w, color);
	}
}

int Screen::drawGlyph(unsigned x, unsigned y, unsigned fc, unsigned bc, unsigned short code, bool dw, bool fillbg)
{
	if (x >= mScreenw || y >= mScreenh) return 0;

	int w = (dw ? W(2) : W(1)), h = H(1);
	if (x + w > mScreenw) w = mScreenw - x;
	if (y + h > mScreenh) h = mScreenh - y;

	Font::Glyph *glyph = (Font::Glyph *)Font::instance()->getGlyph(code);
	if (!glyph) {
		if (fillbg) fillRect(x, y, w, h, bc);
		return w;
	}

	int top = glyph->top;
	if (top < 0) top = 0;

	int left = glyph->left;
	if ((int)x + left < 0) left = -x;
	
	int width = glyph->width;
	if (width > w - left) width = w - left;
	if ((int)x + left + width > (int)mScreenw) width = mScreenw - ((int)x + left);
	if (width < 0) width = 0;

	int height = glyph->height;
	if (height > h - top) height = h - top;
	if (y + top + height > mScreenh) height = mScreenh - (y + top);
	if (height < 0) height = 0;
	
	if (fillbg) {
		if (top) fillRect(x, y, w, top, bc);
		if (left > 0) fillRect(x, y + top, left, height, bc);
	
		int right = width + left;
		if (w > right) fillRect((int)x + right, y + top, w - right, height, bc);
	
		int bot = top + height;
		if (h > bot) fillRect(x, y + bot, w, h - bot, bc);
	}

	x += left;
	y += top;
	if (x >= mScreenw || y >= mScreenh || !width || !height) return glyph->advance;

	unsigned nwidth = width, nheight = height;
	rotateRect(x, y, nwidth, nheight);

	char *pixmap = glyph->pixmap;
	unsigned wdiff = glyph->width - width, hdiff = glyph->height - height;
	
	if (wdiff) {
		if (mRotate == Rotate180) pixmap += wdiff;
		else if (mRotate == Rotate270) pixmap += wdiff * glyph->pitch;
	}
	
	if (hdiff) {
		if (mRotate == Rotate90) pixmap += hdiff;
		else if (mRotate == Rotate180) pixmap += hdiff * glyph->pitch;
	}
	
	char *dst, *dst_line = mpMemStart + x * bytes_per_pixel;

	for (; nheight--; y++, pixmap += glyph->pitch) {
		dst = dst_line + offsetY(y) * finfo.line_length;
		draw(dst, pixmap, nwidth, fillbg, fc, bc);
	}

	return glyph->advance;
}
