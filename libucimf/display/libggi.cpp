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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_GGI_LIB

#include <stdio.h>
#include <cassert>
#include "debug.h"
#include "libggi.h"

ggi_visual_t LIBGGI::visual;
ggi_mode LIBGGI::mode = { /* This will cause the default mode to be set */
    1,                      /* 1 frame [???] */
    {GGI_AUTO,GGI_AUTO},    /* Default size */
    {GGI_AUTO,GGI_AUTO},    /* Virtual */
    {0,0},                  /* size in mm don't care */
    GT_AUTO,               /* Mode */
    {GGI_AUTO,GGI_AUTO}     /* Font size */
};
    
int LIBGGI::mBytesPixel = 0;
ggi_pixel LIBGGI::mPixelColor[16];

char* LIBGGI::mpixMapBuf = NULL;
size_t LIBGGI::mPixMapBufSize = 0;

// based on minigui
typedef struct RGB
{
    char r;
    char g;
    char b;
};

// Convertion between rgb8 to rgb16
inline static int rgb8to16 (int rgb)
{
    return rgb * (0xFFFF / 0xFF);
}

bool LIBGGI::TryOpen(char *ggiDriveName) {
    int i;
    const ggi_pixelformat* pf;

    if (ggiInit() != 0) {
       UrDEBUG("unable to initialize LibGGI, exiting.\n");
        return false;
    }

    visual = ggiOpen (ggiDriveName);
    if (visual == NULL) {
       UrDEBUG("unable to open default visual, exiting.\n");
        ggiExit();
        return false;
    }

    // ggiSetFlags (visual, GGIFLAG_ASYNC);
    ggiCheckMode (visual, &mode);
    if (ggiSetMode (visual, &mode))    { 
       UrDEBUG("Can't set mode\n");
        ggiClose (visual);
        ggiExit ();
        return false;
    }

    mXres = mode.visible.x;
    mYres = mode.visible.y;

    if (GT_SCHEME (mode.graphtype) == GT_PALETTE) {
        ggiSetColorfulPalette (visual);
    }

    const RGB DefaultColor16[] = {
        {0x00, 0x00, 0x00},     // black         --0
        {0x00, 0x00, 0x80},     // dark blue     --1
        {0x00, 0x80, 0x00},     // dark green    --2
        {0x00, 0x80, 0x80},     // dark cyan     --3
        {0x80, 0x00, 0x00},     // dark red      --4
        {0x80, 0x00, 0x80},     // dark magenta  --5
        {0x80, 0x80, 0x00},     // dark yellow   --6
        {0x80, 0x80, 0x80},     // dark gray     --7
        {0xC0, 0xC0, 0xC0},     // light gray    --8
        {0x00, 0x00, 0xFF},     // blue          --9
        {0x00, 0xFF, 0x00},     // green         --10
        {0x00, 0xFF, 0xFF},     // cyan          --11
        {0xFF, 0x00, 0x00},     // red           --12
        {0xFF, 0x00, 0xFF},     // magenta       --13
        {0xFF, 0xFF, 0x00},     // yellow        --14
        {0xFF, 0xFF, 0xFF}      // light white   --15
    };

    ggi_color color;
    for (i = 0; i < 16; i++) {
        color.r = rgb8to16 (DefaultColor16[i].r);
        color.g = rgb8to16 (DefaultColor16[i].g);
        color.b = rgb8to16 (DefaultColor16[i].b);
        mPixelColor[i] = ggiMapColor (visual, &color);
    }
    
    pf = ggiGetPixelFormat (visual);
    mBytesPixel = (pf->size + 7) >> 3;
    /*
    bytes_per_phypixel = (pf->size + 7) >> 3;
    bits_per_phypixel  = pf->depth;
    width_phygc        = mode.visible.x;
    height_phygc       = mode.visible.y;
    if (pf->depth == 32)
        colors_phygc   = 1 << 24;
    else
        colors_phygc   = 1 << pf->depth;
    grayscale_screen   = false;

#ifdef _DEBUG
    fprintf (stderr, "GGI Mode: bpp %d, depth %d, width %d, height %d.\n",
            bytes_per_phypixel,
            bits_per_phypixel,
            width_phygc,
            height_phygc);
#endif

    bytesperpixel      = bytes_per_pixel;
    bitsperpixel       = bits_per_pixel;
    */
    mpGraphDev = new LIBGGI;
    return true;
}


void LIBGGI::SwitchToGraph() {
    // restore color palette
    if (GT_SCHEME (mode.graphtype) == GT_PALETTE) {
        ggiSetColorfulPalette (visual);
    }
}

void LIBGGI::SwitchToText() {
}

LIBGGI::~LIBGGI() {
    ggiClose(visual);
    ggiExit();
    if (mpixMapBuf)
        delete[] mpixMapBuf;
}

void LIBGGI::PutPixel(int x,int y,int color) {
    ggiPutPixel(visual, x, y, mPixelColor[color]);
}

void LIBGGI::FillRect(int x1,int y1,int x2,int y2,int color) {
    assert( x1 >= 0 && x1 < mXres && y1 >=0 && y1 < mYres);
    assert( x2 >= 0 && x2 < mXres && y2 >=0 && y2 < mYres);
    assert(x1 <= x2 && y1 <= y2);

    ggiSetGCForeground(visual, mPixelColor[color]);
    ggiDrawBox(visual, x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

void LIBGGI::RevRect(int x1,int y1,int x2,int y2) {
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    assert(x1 <= x2 && y1 <= y2);
    int height = y2 - y1 + 1;
    int width = x2 - x1 + 1;
    char* pixMap = GetPixMapBuf(width * height * mBytesPixel);
    ggiGetBox(visual, x1, y1, width, height, pixMap);

    int cnt;
    char *pMap = pixMap;
    for (cnt = width * height * mBytesPixel; cnt--; pMap++)
            *pMap ^= 0xff;
    ggiPutBox(visual, x1, y1, width, height, pixMap);
}

void LIBGGI::DrawChar(int x,int y,int fg,int bg,struct CharBitMap* pFont) {
    char* pixMap = GetPixMapBuf(pFont->w * mBlockHeight * mBytesPixel);
    GetPixMap(pixMap,fg,bg,pFont);

    ggiPutBox(visual, x, y, pFont->w, mBlockHeight, pixMap);
}


int LIBGGI::GetPixMap(char* pixMap,int fg,int bg,struct CharBitMap* pFont) {
    int x, row;
    int b = 0;
    char* cdat = pFont->pBuf;
    char* line;
    int line_bytes = pFont->w * mBytesPixel;

    line = pixMap;
    switch (mBytesPixel) {
    case 1:
        for (row = mBlockHeight; row--;) {
            pixMap = line;
            for (x = 0; x < pFont->w; x++) {
                // assert( (pixMap - mpixMapBuf) <= mPixMapBufSize);
                if (x % 8 == 0)
                    b = *cdat++;
                if ((b & (0x80 >> (x % 8)))) {
                    *pixMap = mPixelColor[fg];
                } else {
                    *pixMap = mPixelColor[bg];
                }
                pixMap++;
            }
            line += line_bytes;
        }
        break;

    case 2:
        for (row = mBlockHeight; row--;) {
            pixMap = line;
            for (x = 0; x < pFont->w; x++) {
                if (x % 8 == 0)
                    b = *cdat++;
                if ((b & (128 >> (x % 8)))) {
                    *(ushort *) pixMap = mPixelColor[fg];
                } else {
                    *(ushort *) pixMap = mPixelColor[bg];
                }
                pixMap += 2;
            }
            line += line_bytes;
        }
        break;

    case 3:
        for (row = mBlockHeight; row--;) {
            pixMap = line;
            for (x = 0; x < pFont->w; x++) {
                if (x % 8 == 0)
                    b = *cdat++;
                if ((b & (128 >> (x % 8)))) {
                    *(ushort *) pixMap = mPixelColor[fg];
                    *(pixMap + 2) = mPixelColor[fg] >> 16;
                } else {
                    *(ushort *) pixMap = mPixelColor[bg];
                    *(pixMap + 2) = mPixelColor[bg] >> 16;
                }
                pixMap += 3;
            }
            line += line_bytes;
        }
        break;

    case 4:
        for (row = mBlockHeight; row--;) {
            pixMap = line;
            for (x = 0; x < pFont->w; x++) {
                if (x % 8 == 0)
                    b = *cdat++;
                if ((b & (128 >> (x % 8)))) {
                    *(uint *) pixMap = mPixelColor[fg];
                } else {
                    *(uint *) pixMap = mPixelColor[bg];
                }
                pixMap += 4;
            }
            line += line_bytes;
        }
    }

    return line_bytes;
}

inline char* LIBGGI::GetPixMapBuf(size_t size) {
    if (size <= mPixMapBufSize)
        return mpixMapBuf;

    if (mpixMapBuf)
        delete[] mpixMapBuf;
        
    // unit is 32 byte
    mPixMapBufSize = ((size + 31) >> 5) << 5;
    mpixMapBuf = (char *) new char[mPixMapBufSize];
    return mpixMapBuf;
}

#endif
