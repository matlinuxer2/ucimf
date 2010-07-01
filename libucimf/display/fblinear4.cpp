/*
 * UCIMF - Unicode Console InputMethod Framework                                                    
 *
 * Copyright (c) 2007 Chun-Yu Lee (Mat) and Open RazzmatazZ Laboratory (OrzLab)
 *                                                                                                  
 * Parts taken from zhcon project.
 * Copyright (c) 2001 ejoy <ejoy@users.sourceforge.net>
 *                    huyong <ccpaging@online.sh.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <assert.h>
#include <string.h>
//#include "global.h"
#include "fblinear4.h"

__u16 FBLinear4::nibbletab_cfb4[] = {
//    0x0000,0x000f,0x00f0,0x00ff,
//    0x0f00,0x0f0f,0x0ff0,0x0fff,
//    0xf000,0xf00f,0xf0f0,0xf0ff,
//    0xff00,0xff0f,0xfff0,0xffff
0x0000,0xf000,0x0f00,0xff00,
0x00f0,0xf0f0,0x0ff0,0xfff0,
0x000f,0xf00f,0x0f0f,0xff0f,
0x00ff,0xf0ff,0x0fff,0xffff
};

FBLinear4::FBLinear4() {
    InitColorMap();
    mNextLine = mNextLine ? mNextLine : mXres>>1;
}

void FBLinear4::InitColorMap() {}

// based on libggi
void FBLinear4::FillRect(int x1,int y1,int x2,int y2,int color) {
    assert( x1 >= 0 && x1 < mXres && y1 >=0 && y1 < mYres);
    assert( x2 >= 0 && x2 < mXres && y2 >=0 && y2 < mYres);
    assert(x1 <= x2 && y1 <= y2);

    int lines = y2 - y1 + 1, row;
    int width = x2 - x1 + 1;

    __u8 *destx1, *dest;
    __u8 bits;
    __u8 fg;
    int w;
    
    destx1 = (__u8*)mpBuf + mNextLine * y1 + x1 / 2;
    fg = (__u8)color | (color<<4);

    for (row = lines; row-- ; destx1 += mNextLine) {
        dest = destx1;
        w = width;

        // x is odd.  Read-modify-write right pixel.
        if (x1 & 0x01) {
            bits = fb_readb(dest);
            fb_writeb((bits & 0x0f) | (fg & 0xf0), dest);
            dest++;
            w--;
        }
    
        memset(dest, fg, w/2);

        // Dangling right pixel.
        if (w & 0x01) {
            dest += w/2;
            bits = fb_readb(dest);
            fb_writeb((bits & 0x0f) | (fg & 0xf0), dest);
        }
    }
}

// base on libggi
inline void FBLinear4::PutPixel(int x,int y,int color) {
    __u8 *dest;
    __u8 bits;
    __u8 xs;
    
    dest = (__u8*)mpBuf + mNextLine * y + x / 2;
    bits = fb_readb(dest);

    xs = (x & 1) << 2;
    fb_writeb((bits & (0x0F << xs)) | ((color & 0x0f) << (xs ^ 4)), dest);
}
