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
//#include "global.h"
#include "fblinear32.h"

FBLinear32::FBLinear32() {
    InitColorMap();
    mNextLine = mNextLine ? mNextLine : mXres<<2;
}

void FBLinear32::InitColorMap() {
    int i;
    __u32 red, green, blue;
    for(i = 0; i < 16; i++) {
        red = red16[i];
        green = green16[i];
        blue = blue16[i];

        cfb32[i] = ((red  & 0xff00) << 8) |
                   (green & 0xff00) |
                   (blue >> 8);
    }
}

void FBLinear32::FillRect(int x1,int y1,int x2,int y2,int color) {
    assert( x1 >= 0 && x1 < mXres && y1 >=0 && y1 < mYres);
    assert( x2 >= 0 && x2 < mXres && y2 >=0 && y2 < mYres);
    assert(x1 <= x2 && y1 <= y2);

    __u8* dest = ((__u8*)mpBuf + mNextLine * y1 + x1 * 4);
    __u32* dest32;

    __u32 fgx = cfb32[color];

    int width = x2 - x1 + 1;
    int height = y2 - y1 + 1;
    int cnt;
    for(; height--; dest += mNextLine) {
        dest32 = (__u32*)dest;
        for (cnt = width; cnt--;) {
            fb_writel(fgx, dest32++);
        }
    }
}

void FBLinear32::RevRect(int x1,int y1,int x2,int y2) {
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    assert(x1 <= x2 && y1 <= y2);
    __u8* dest = (__u8*)mpBuf + mNextLine * y1 + x1 * 4;
    __u32* dest32;

    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    int cnt;
    for(; height--; dest += mNextLine) {
        dest32 = (__u32*)dest;
        for (cnt = width; cnt--;) {
            fb_writel(fb_readl(dest32) ^ 0xffffffff, dest32++);
        }
    }
}

void FBLinear32::SaveRect(int x1,int y1,int x2,int y2, char* *buffer) {
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    assert(x1 <= x2 && y1 <= y2);

    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    int buffer_bytes = height * width * 4;
    
    // allocate memory for saving
    if( *buffer != NULL )
    {
      delete [] (*buffer);
      *buffer = NULL;
    }

    *buffer = (char*) new char[buffer_bytes];
    
    __u8* dest = (__u8*)mpBuf + mNextLine * y1 + x1 * 4;
    __u32* buf= (__u32*)(*buffer);
    int cnt;
    for(; height--; dest += mNextLine) {
        __u32* dest32 = (__u32*)dest;
        for ( int cnt = width ; cnt--;) {
            fb_writel( fb_readl(dest32++) , buf++ );
        }
    }
}

void FBLinear32::RstrRect(int x1,int y1,int x2,int y2, char* *buffer) {
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    assert(x1 <= x2 && y1 <= y2);
    
    assert ( *buffer != NULL );
    
    __u32* buf= (__u32*)(*buffer);
    __u8* dest = (__u8*)mpBuf + mNextLine * y1 + x1 * 4;


    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    for(; height--; dest += mNextLine) {
        __u32* dest32 = (__u32*)dest;
        for ( int cnt = width ; cnt--;) {
            fb_writel( fb_readl(buf++), dest32++ );
        }
    }
    
    // release memory 
    delete [] (*buffer);
    *buffer = NULL;
}

inline void FBLinear32::PutPixel(int x,int y,int color) {
    assert( x >= 0 && x < mXres && y >=0 && y < mYres);
    fb_writel(cfb32[color],mpBuf + mNextLine * y + x * 4);
}

void FBLinear32::DrawChar(int x,int y,int fg,int bg,struct CharBitMap* pFont) {
    __u32 fgx,bgx,eorx;
    fgx = cfb32[fg];
    bgx = cfb32[bg];
    eorx = fgx ^ bgx;

    __u8* dest = ((__u8*)mpBuf + mNextLine * y + x * 4);
    __u32 *dest32;

    char* cdat = pFont->pBuf;
    int row, cnt;
    for (row = mBlockHeight; row--; dest += mNextLine) {
        dest32 = (__u32*)dest;
        for (cnt = (pFont->w/8); cnt--;) {
            fb_writel((-(*cdat >> 7) & eorx) ^ bgx, dest32++);
            fb_writel((-(*cdat >> 6 & 1) & eorx) ^ bgx, dest32++);
            fb_writel((-(*cdat >> 5 & 1) & eorx) ^ bgx, dest32++);
            fb_writel((-(*cdat >> 4 & 1) & eorx) ^ bgx, dest32++);
            fb_writel((-(*cdat >> 3 & 1) & eorx) ^ bgx, dest32++);
            fb_writel((-(*cdat >> 2 & 1) & eorx) ^ bgx, dest32++);
            fb_writel((-(*cdat >> 1 & 1) & eorx) ^ bgx, dest32++);
            fb_writel((-(*cdat & 1) & eorx) ^ bgx, dest32++);
            cdat++;
        }
        if (pFont->isMulti8)
            continue;

        if (pFont->w & 4) {
            fb_writel((-(*cdat >> 7) & eorx) ^ bgx, dest32++);
            fb_writel((-(*cdat >> 6 & 1) & eorx) ^ bgx, dest32++);
            fb_writel((-(*cdat >> 5 & 1) & eorx) ^ bgx, dest32++);
            fb_writel((-(*cdat >> 4 & 1) & eorx) ^ bgx, dest32++);
        }
        if (pFont->w & 2) {
            fb_writel((-(*cdat >> 3 & 1) & eorx) ^ bgx, dest32++);
            fb_writel((-(*cdat >> 2 & 1) & eorx) ^ bgx, dest32++);
        }
        if (pFont->w & 1) {
            fb_writel((-(*cdat >> 1 & 1) & eorx) ^ bgx, dest32);
        }
        cdat++;
    }
}

