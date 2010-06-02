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
#include "fblinear24.h"
#include <iostream>
#include "debug.h"

using namespace std;

FBLinear24::FBLinear24() {
    InitColorMap();
    mNextLine = mNextLine ? mNextLine : mXres*3;
}

void FBLinear24::InitColorMap() {
    int i;
    __u32 red, green, blue;
    for(i = 0; i < 16; i++) {
        red = red16[i];
        green = green16[i];
        blue = blue16[i];

        cfb24[i] = ((red  & 0xff00) << 8) |
                   (green & 0xff00) |
                   (blue >> 8);
    }
}

#define convert4to3(in1, in2, in3, in4, out1, out2, out3) \
    do  { \
        out1 = in1       | (in2<<24); \
        out2 = (in2>> 8) | (in3<<16); \
        out3 = (in3>>16) | (in4<< 8); \
    } while (0);
inline void FBLinear24::store4pixels(__u32 d1, __u32 d2, __u32 d3, __u32 d4, __u32 *dest) {
    __u32 o1, o2, o3;
    convert4to3(d1, d2, d3, d4, o1, o2, o3);
    fb_writel (o1, dest++);
    fb_writel (o2, dest++);
    fb_writel (o3, dest++);
}

void FBLinear24::FillRect(int x1,int y1,int x2,int y2,int color) {
    assert( x1 >= 0 && x1 < mXres && y1 >=0 && y1 < mYres);
    assert( x2 >= 0 && x2 < mXres && y2 >=0 && y2 < mYres);
    assert(x1 <= x2 && y1 <= y2);

    __u8* dest = ((__u8*)mpBuf + mNextLine * y1 + x1 * 3);

    __u32 fgx = cfb24[color];

    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    int cnt;
    __u8* dest8;
    __u16* dest16;
    __u32 *dest32;
    for(; height--; dest += mNextLine) {
        dest32 = (__u32*)dest;
        for (cnt = width/4; cnt--;) {
            store4pixels(fgx, fgx, fgx, fgx, dest32);
            dest32 += 3;
        }
        if (width & 2) {
            fb_writel(fgx | fgx<<24, dest32++);
            dest16 = (__u16*)dest32;
            fb_writew(fgx>>8, dest16++);
            dest32 = (__u32*)dest16;
        }
        if (width & 1) {
            dest16 = (__u16*)dest32;
            fb_writew(fgx, dest16++);
            dest8 = (__u8*)dest16;
            fb_writeb(fgx>>16, dest8);
        }
    }
    /* base on ole, more simple
    __u8 color0 = cfb24[color];
    __u8 color1 = cfb24[color]>>8;
    __u8 color2 = cfb24[color]>>16;

    __u8* p;
    int col;
    int width = x2 - x1 + 1;
    int height = y2 - y1 + 1;
    for(; height > 0; height--, dest += mNextLine) {
        p = dest;
        for(col = width; col > 0; col--) {
            fb_writeb(color0, p++);
            fb_writeb(color1, p++);
            fb_writeb(color2, p++);
        }
    }
    */
}

void FBLinear24::RevRect(int x1,int y1,int x2,int y2) {
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    assert(x1 <= x2 && y1 <= y2);
    __u8* dest = (__u8*)mpBuf + mNextLine * y1 + x1 * 3;

    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    int cnt;
    __u8* dest8;
    __u16* dest16;
    __u32* dest32;
    for(; height--; dest += mNextLine) {
        dest32 = (__u32*)dest;
        for (cnt = width/4; cnt--;) {
            fb_writel(fb_readl(dest32) ^ 0xffffffff, dest32++);
            fb_writel(fb_readl(dest32) ^ 0xffffffff, dest32++);
            fb_writel(fb_readl(dest32) ^ 0xffffffff, dest32++);
        }
        if (width & 2) {
            fb_writel(fb_readl(dest32) ^ 0xffffffff, dest32++);
            dest16 = (__u16*)dest32;
            fb_writew(fb_readw(dest16) ^ 0xffff, dest16++);
            dest32 = (__u32*)dest16;
        }
        if (width & 1) {
            dest16 = (__u16*)dest32;
            fb_writew(fb_readw(dest16) ^ 0xffff, dest16++);
            dest8 = (__u8*)dest16;
            fb_writeb(fb_readb(dest8) ^ 0xff, dest8);
        }
    }
    /* more simple
    for(; height--; dest += mNextLine) {
        dest8 = (__u8*)dest;
        for (cnt = width * 3; cnt--;) {
            fb_writeb(fb_readb(dest8) ^ 0xff, dest8++);
        }
    }
    */
}


void FBLinear24::SaveRect(int x1,int y1,int x2,int y2, char* *buffer) {
 UrDEBUG("FBLinear24::SaveRect( %d, %d, %d, %d) \n", x1,y1,x2,y2 );
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    assert(x1 <= x2 && y1 <= y2);

    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    int buffer_bytes = height * width * 3;
    
    // allocate memory for saving
    if( *buffer != NULL )
    {
      delete [] (*buffer);
      *buffer = NULL;
    }

    *buffer = (char*) new char[buffer_bytes];
    
    __u8* dest = (__u8*)mpBuf + mNextLine * y1 + x1 * 3;
    __u8* buf= (__u8*)(*buffer);

    for(; height--; dest += mNextLine) {
        __u8* dest8 = (__u8*)dest;
        for ( int cnt = width; cnt--;) {
            fb_writeb( fb_readb(dest8++) , buf++ );
            fb_writeb( fb_readb(dest8++) , buf++ );
            fb_writeb( fb_readb(dest8++) , buf++ );
        }
    }
 UrDEBUG("FBLinear24::SaveRect() End\n");
}

void FBLinear24::RstrRect(int x1,int y1,int x2,int y2, char* *buffer) {
 UrDEBUG("FBLinear24::RstrRect( %d, %d, %d, %d )\n", x1, y1, x2, y2 );
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    assert(x1 <= x2 && y1 <= y2);

    assert( *buffer != NULL ); 
    
    __u8* buf= (__u8*)(*buffer);
    __u8* dest = (__u8*)mpBuf + mNextLine * y1 + x1 * 3;


    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    for(; height--; dest += mNextLine) {
        __u8* dest8 = (__u8*)dest;
        for ( int cnt = width ; cnt--;) {
            fb_writew( fb_readw(buf++), dest8++ );
            fb_writew( fb_readw(buf++), dest8++ );
            fb_writew( fb_readw(buf++), dest8++ );
        }
    }
    
    // release memory 
    delete [] (*buffer);
    *buffer = NULL;

 UrDEBUG("FBLinear24::RstrRect() End\n");
}

inline void FBLinear24::PutPixel(int x,int y,int color) {
    assert( x >= 0 && x < mXres && y >=0 && y < mYres);
    __u8* dest = ((__u8*)mpBuf + mNextLine * y + x * 3);
    fb_writew( (cfb24[color]<<8) & (cfb24[color]>>8), dest);
    fb_writeb( cfb24[color]>>16, dest + 2);
}
