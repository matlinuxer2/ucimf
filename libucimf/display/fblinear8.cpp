// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          fblinear8.cpp  -  description
                             -------------------
    begin                : Fri July 20 2001
    copyright            : (C) 2001 by ejoy, huyong
    email                : ejoy@users.sourceforge.net
                           ccpaging@online.sh.cn
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <assert.h>
#include <string.h>
#include "fblinear8.h"

//gray scale
__u32 FBLinear8::nibbletab_cfb8[] = {
0x00000000,0xff000000,0x00ff0000,0xffff0000,
0x0000ff00,0xff00ff00,0x00ffff00,0xffffff00,
0x000000ff,0xff0000ff,0x00ff00ff,0xffff00ff,
0x0000ffff,0xff00ffff,0x00ffffff,0xffffffff
};

FBLinear8::FBLinear8() {
    mNextLine = mNextLine ? mNextLine : mXres;
}

void FBLinear8::FillRect(int x1,int y1,int x2,int y2,int color) {
    assert( x1 >= 0 && x1 < mXres && y1 >=0 && y1 < mYres);
    assert( x2 >= 0 && x2 < mXres && y2 >=0 && y2 < mYres);
    assert(x1 <= x2 && y1 <= y2);
    for(int y = y1;y <= y2; y++)
        memset(mpBuf + mNextLine * y + x1,color,x2 - x1 + 1);
}

void FBLinear8::RevRect(int x1,int y1,int x2,int y2) {
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    assert(x1 <= x2 && y1 <= y2);
    __u8* dest = (__u8*)mpBuf + mNextLine * y1 + x1;

    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    int cnt;
    __u8* dest8;
    __u16* dest16;
    __u32 *dest32;
    for(; height--; dest += mNextLine) {
        dest32 = (__u32*)dest;
        for (cnt = width/4; cnt--;) {
            fb_writel(fb_readl(dest32) ^ 0x0f0f0f0f, dest32++);
        }
        if (width & 2) {
            dest16 = (__u16*)dest32;
            fb_writew(fb_readw(dest16) ^ 0x0f0f, dest16++);
            dest32 = (__u32*)dest16;
        }
        if (width & 1) {
            dest8 = (__u8*)dest32;
            fb_writeb(fb_readb(dest8) ^ 0x0f, dest8);
        }
    }
}

void FBLinear8::SaveRect(int x1,int y1,int x2,int y2, char* *buffer) {
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    assert(x1 <= x2 && y1 <= y2);
    __u8* dest = (__u8*)mpBuf + mNextLine * y1 + x1 * 1;
    __u8* buf= (__u8*)(*buffer);
  
    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    int buffer_bytes = height * width * 1;
   
    // allocate memory for saving
    if( *buffer != NULL )
    {
      delete [] (*buffer);
      *buffer = NULL;
    }

    *buffer = (char*) new char[buffer_bytes];
    
    __u8* dest8;
    __u8* buf8;
    int cnt;
    for(; height--; dest += mNextLine) {
        dest8 = (__u8*)dest;
        buf8 = (__u8*)buf;
        for (cnt = width * 1; cnt--;) {
            *buf8 = fb_readb(dest8);
            dest8++;
            buf8++;
        }
        buf += mNextLine;
    }
    
}

void FBLinear8::RstrRect(int x1,int y1,int x2,int y2, char* *buffer) {
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    assert(x1 <= x2 && y1 <= y2);
    __u8* dest = (__u8*)mpBuf + mNextLine * y1 + x1 * 1;
    __u8* buf= (__u8*)(*buffer);

    assert ( *buffer != NULL );
    
    __u8* dest8;
    __u8*  buf8;
    int cnt;
    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    for(; height--; dest += mNextLine) {
        dest8 = (__u8*)dest;
        buf8 = (__u8*)buf;
        for (cnt = width * 1; cnt--;) {
            fb_writeb( *buf8, dest8 );
            dest8++;
            buf8++;
        }
        buf += mNextLine;
    }
    // release memory 
    delete [] (*buffer);
    *buffer = NULL;
}

void FBLinear8::PutPixel(int x,int y,int color) {
    *(mpBuf + mNextLine * y + x) = color;
}
