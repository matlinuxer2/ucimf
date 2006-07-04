// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          graphdev.h  -  description
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

#ifndef GRPATHDEV_H
#define GRPATHDEV_H
#if defined(linux)
    #include <asm/types.h>
#elif defined(__FreeBSD__)
    #include "utypes.h"
#endif
#include <stdexcept>

struct CharBitMap {
    char* pBuf;         // point to bitmap
    int BufLen;         // include blank line height
    char* pLast;        // point to last scan line
    int ExtLen;         // blank linebyte's bitmap len
    int h;              // height
    int w;              // width
    int wBytes;         // width / 8
    bool isMulti8;      // width is multi 8 or not?
};

enum OPEN_RC { NORMAL, UNSUPPORT, FAILURE };

class GraphDev {
    public:
        static bool Open();
        static bool Open(int xres, int yres, int depth);
        static void Close();
        virtual ~GraphDev() {};

        static GraphDev *mpGraphDev;

        int Height() { return mYres; }
        int Width() { return mXres; }

        // font
        void OutChar(int x, int y, int fg, int bg, unsigned int c);

        // char display
        virtual void DrawChar(int x,int y,int fg,int bg,struct CharBitMap* pFont) = 0;
        static int BlockHeight() { return mBlockHeight; }
        static int BlockWidth() { return mBlockWidth; }
        static int mBlankLineHeight;

        void ClearScr();
        void DrawLine(int x1,int y1,int x2,int y2,int color);
        void DrawRect(int x1,int y1,int x2,int y2,int color);

        virtual void PutPixel(int x,int y,int color) = 0;
        virtual void FillRect(int x1,int y1,int x2,int y2,int color) = 0;
        virtual void RevRect(int x1,int y1,int x2,int y2) = 0;
        virtual void SaveRect(int x1, int y1, int x2, int y2, CharBitMap& pBuffer ) = 0;
        virtual void RstrRect(int x1, int y1, int x2, int y2, CharBitMap& pBuffer ) = 0;
        
    protected:
        static int mXres, mYres;
        static int mBlockWidth;
        static int mBlockHeight;
        static struct CharBitMap mAsc;
};

#define fb_readb(addr) (*(volatile __u8 *) (addr))
#define fb_readw(addr) (*(volatile __u16 *) (addr))
#define fb_readl(addr) (*(volatile __u32 *) (addr))
#define fb_writeb(b,addr) (*(volatile __u8 *) (addr) = (b))
#define fb_writew(b,addr) (*(volatile __u16 *) (addr) = (b))
#define fb_writel(b,addr) (*(volatile __u32 *) (addr) = (b))

#endif
