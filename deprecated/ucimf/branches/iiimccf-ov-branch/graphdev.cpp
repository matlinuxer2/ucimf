// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          graphdev.cpp  -  description
                             -------------------
    begin                : Sun Auguest 26 2001
    copyright            : (C) 2001 by huyong, rick
    email                : ccpaging@online.sh.cn
                           rick@chinaren.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <assert.h>

#include "display/fbdev.h"
#include "display/vgadev.h"
#if defined(linux) || defined(__FreeBSD__)
    #ifdef HAVE_GGI_LIB
    #include "display/libggi.h"
    #endif
#endif

// mmap framebuffer address
GraphDev *GraphDev::mpGraphDev = NULL;

// font
int GraphDev::mXres = 0;
int GraphDev::mYres = 0;
/*
BaseFont *GraphDev::mpAscFont = NULL;
BaseFont *GraphDev::mpDblFont = NULL;
*/

// char display
int GraphDev::mBlockWidth = 0;
int GraphDev::mBlockHeight = 0;
int GraphDev::mBlankLineHeight = 0;
struct CharBitMap GraphDev::mAsc = {0};
struct CharBitMap GraphDev::mDbl = {0};

bool GraphDev::Open() {
#ifdef HAVE_GGI_LIB
    #ifndef NDEBUG
    //try libggi first for debug
    char *ggiDriveName = getenv("GGI_DISPLAY");
    if (ggiDriveName) {
        if (LIBGGI::TryOpen(ggiDriveName))
            return true;
        throw(runtime_error("ggi open error, check env GGI_DISPLAY!"));
    }
    #endif
#endif
    //first try fbdev
    OPEN_RC rc = FBDev::TryOpen();
    switch (rc) {
        case NORMAL:
            return true;
        case UNSUPPORT:
#ifdef HAVE_GGI_LIB
            //try libggi
            {
            char *ggiDriveName = getenv("GGI_DISPLAY");
            if (ggiDriveName)
                if (LIBGGI::TryOpen(ggiDriveName))
                    return true;
            }
#endif
            //libggi failed,now try vga
        case FAILURE:
#if defined(linux)
#ifdef USING_VGA
            if (VGADev::TryOpen())
                return true;
            break;
#endif
#endif
        default:  // vga fail
            return false;
    }
    return false;
}

bool GraphDev::Open(int xres, int yres, int depth) {
    return FBDev::TryOpen(xres, yres, depth);
}

void GraphDev::Close() {
    if (mpGraphDev)
        mpGraphDev->ClearScr();
    if (mAsc.pBuf)
        delete[] mAsc.pBuf;
    if (mDbl.pBuf)
        delete[] mDbl.pBuf;
    delete mpGraphDev;
}

void GraphDev::ClearScr() {
    FillRect(0, 0, Width() - 1, Height() - 1, 0);
}

void GraphDev::DrawLine(int x1,int y1,int x2,int y2,int color) {
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    int dx = x2 - x1;
    int dy = y2 - y1;
    int ax = abs(dx) << 1;
    int ay = abs(dy) << 1;
    int sx = (dx >= 0) ? 1 : -1;
    int sy = (dy >= 0) ? 1 : -1;

    int x  = x1;
    int y  = y1;

    if (ax > ay) {
        int d = ay - (ax >> 1);
        while (x != x2) {
            PutPixel(x, y,color);

            if ((d > 0) || ((d == 0) && (sx == 1))) {
                y += sy;
                d -= ax;
            }
            x += sx;
            d += ay;
        }
    } else {
        int d = ax - (ay >> 1);
        while (y != y2) {
            PutPixel(x, y,color);

            if ((d > 0) || ((d == 0) && (sy == 1))) {
                x += sx;
                d -= ay;
            }
            y += sy;
            d += ax;
        }
    }
    PutPixel(x, y,color);
}

void GraphDev::DrawRect(int x1,int y1,int x2,int y2,int color) {
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    DrawLine(x1,y1,x2,y1,color);
    DrawLine(x1,y1,x1,y2,color);
    DrawLine(x1,y2,x2,y2,color);
    DrawLine(x2,y1,x2,y2,color);
}

/*
void GraphDev::SetAscFont(BaseFont *pAscFont) {
    mpAscFont = pAscFont;

    mBlockWidth = pAscFont->Width();
    mBlockHeight = pAscFont->Height() + mBlankLineHeight;
    
    mAsc.h = pAscFont->Height();
    mAsc.w = pAscFont->Width();
    mAsc.wBytes = pAscFont->WidthBytes();
    mAsc.BufLen = mBlockHeight * mAsc.wBytes;
    if (mAsc.pBuf)
        delete[] mAsc.pBuf;
    mAsc.pBuf = (char *) new char[mAsc.BufLen];
    mAsc.pLast = mAsc.pBuf + (mAsc.h  - 1)* mAsc.wBytes;
    mAsc.ExtLen = mBlankLineHeight * mAsc.wBytes;
    mAsc.isMulti8 = (mAsc.w % 8) ? false : true;
}

void GraphDev::SetDblFont(BaseFont *pDblFont) {
    mpDblFont = pDblFont;

    mDbl.h = pDblFont->Height();
    mDbl.w = pDblFont->Width();
    mDbl.wBytes = pDblFont->WidthBytes();
    mDbl.BufLen = (mDbl.h + mBlankLineHeight) * mDbl.wBytes;
    if (mDbl.pBuf)
        delete[] mDbl.pBuf;
    mDbl.pBuf = (char *) new char[mDbl.BufLen];
    mDbl.pLast = mDbl.pBuf + (mDbl.h  - 1) * mDbl.wBytes;
    mDbl.ExtLen = mBlankLineHeight * mDbl.wBytes;
    mDbl.isMulti8 = (mDbl.w % 8) ? false : true;
}

//draw a ascii char
void GraphDev::OutChar(int x, int y, int fg, int bg, char c) {
    assert( x >= 0 && x + mBlockWidth <= Width()
            && y >=0 && y + mBlockHeight <= Height());
    char *p = mpAscFont->GetChar(c);
    memcpy(mAsc.pBuf, p, mAsc.BufLen);
    if (mBlankLineHeight > 0) {
        if (c >= 0xb0 && c <= 0xdf) {
            char *pExt = mAsc.pLast + mAsc.wBytes;
            int row = mBlankLineHeight;
            for(; row--; pExt += mAsc.wBytes) {
                memcpy(pExt, mAsc.pLast, mAsc.wBytes);
            }
        } else {
            memset(mAsc.pLast + mAsc.wBytes, 0, mAsc.ExtLen);
        }
    }
    DrawChar(x,y,fg,bg,&mAsc);  // true set extend to blank
}

// draw a hz at (x,y) and fill bottom with blank
// note:this routine can draw half hz at (x,y)
// depend on the value of tag
void GraphDev::OutChar(int x, int y, int fg, int bg, char c1, char c2) {
    assert(c1);
    assert(c2);
    assert( x >= 0 && x + mBlockWidth * 2 <= Width() && y >=0 &&
            y + mBlockHeight <= Height());

    char *p = mpDblFont->GetChar(c1, c2);
    memcpy(mDbl.pBuf, p, mDbl.BufLen);
    if (mBlankLineHeight > 0) {
        memset(mDbl.pLast + mDbl.wBytes, 0, mDbl.ExtLen);
    }
    DrawChar(x,y,fg,bg,&mDbl);
}
*/
