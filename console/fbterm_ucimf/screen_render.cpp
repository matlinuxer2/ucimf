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

#define fb_readb(addr) (*(volatile unsigned char *)(addr))
#define fb_readw(addr) (*(volatile unsigned short *)(addr))
#define fb_readl(addr) (*(volatile unsigned *)(addr))
#define fb_writeb(addr, val) (*(volatile unsigned char *)(addr) = (val))
#define fb_writew(addr, val) (*(volatile unsigned short *)(addr) = (val))
#define fb_writel(addr, val) (*(volatile unsigned *)(addr) = (val))

static unsigned ppl, ppw, ppb;
static unsigned fillColors[NR_COLORS];
static const Color palette[NR_COLORS] = {
    { 0,    0,      0 },
    { 0,    0,      0xaa },
    { 0,    0xaa,   0 },
    { 0,    0x55,   0xaa },
    { 0xaa, 0,      0 },
    { 0xaa, 0,      0xaa },
    { 0xaa, 0xaa,   0 },
    { 0xaa, 0xaa,   0xaa },
    { 0x55, 0x55,   0x55 },
    { 0x55, 0x55,   0xff },
    { 0x55, 0xff,   0x55 },
    { 0x55, 0xff,   0xff },
    { 0xff, 0x55,   0x55 },
    { 0xff, 0x55,   0xff },
    { 0xff, 0xff,   0x55 },
    { 0xff, 0xff,   0xff },
};

static inline void fill(char* dst, unsigned w, unsigned color)
{
	unsigned c = fillColors[color];

	// get better performance if write-combining not enabled for video memory
	for (unsigned i = w / ppl; i--; dst += 4) {
		fb_writel(dst, c);
	}
	
	if (w & ppw) {
		fb_writew(dst, c);
		dst += 2;
	}
	
	if (w & ppb) {
		fb_writeb(dst, c);
	}
}

static inline void draw(char *dst, char *pixmap, unsigned width, bool fillbg, unsigned fc, unsigned bc)
{
	unsigned color;
	unsigned char red, green, blue;
	unsigned char pixel;
	bool isfg;

	for (; width--; pixmap++) {
		pixel = *pixmap;

		if (bytes_per_pixel == 1) {
			isfg = (pixel & 0x80);
			color = fillColors[isfg ? fc : bc];
		} else {
			isfg = pixel;
			red = palette[bc].red + ((palette[fc].red - palette[bc].red) * pixel) / 255;
			green = palette[bc].green + ((palette[fc].green - palette[bc].green) * pixel) / 255;
			blue = palette[bc].blue + ((palette[fc].blue - palette[bc].blue) * pixel) / 255;

			color = (red >> (8 - vinfo.red.length) << vinfo.red.offset)
					| (green >> (8 - vinfo.green.length) << vinfo.green.offset)
					| (blue >> (8 - vinfo.blue.length) << vinfo.blue.offset);
		}

		switch(bytes_per_pixel)
		{
		case 1:
			if (fillbg || isfg) fb_writeb(dst, color);
			dst++;
			break;
		case 2:
			if (fillbg || isfg) fb_writew(dst, color);
			dst += 2;
			break;
		case 4:
			if (fillbg || isfg) fb_writel(dst, color);
			dst += 4;
			break;
		}
	}
}

static void initFillDraw()
{
    ppl = 4 / bytes_per_pixel;
    ppw = ppl >> 1;
    ppb = ppl >> 2;

	for (unsigned i = NR_COLORS; i--;) {
		if (vinfo.bits_per_pixel == 8) {
			fillColors[i] = (i << 24) | (i << 16) | (i << 8) | i;
		} else {
			fillColors[i] = (palette[i].red >> (8 - vinfo.red.length) << vinfo.red.offset)
					| ((palette[i].green >> (8 - vinfo.green.length)) << vinfo.green.offset)
					| ((palette[i].blue >> (8 - vinfo.blue.length)) << vinfo.blue.offset);

			if (vinfo.bits_per_pixel == 16 || vinfo.bits_per_pixel == 15) {
				fillColors[i] |= fillColors[i] << 16;
			}
        }
    }
}
