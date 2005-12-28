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
//#include "basefont.h"

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

struct BitMap {
    char* pBuf;         // point to bitmap
    int BufLen;         // include blank line height
    int h;              // height
    int w;              // width
};

enum OPEN_RC { NORMAL, UNSUPPORT, FAILURE };

class GraphDev {
    public:
        static bool Open();
        static bool Open(int xres, int yres, int depth);
        static void Close();
        virtual ~GraphDev() {};

        static GraphDev *mpGraphDev;

        int Height() {
            return mYres;
        }
        int Width() {
            return mXres;
        }

        // font
	/*
        static void SetAscFont(BaseFont *pAscFont);
        static void SetDblFont(BaseFont *pDblFont);
        void OutChar(int x, int y, int fg, int bg, char c);
        void OutChar(int x, int y, int fg, int bg, char c1, char c2);
	*/

        // char display
        virtual void DrawChar(int x,int y,int fg,int bg,struct CharBitMap* pFont) = 0;
        static int BlockHeight() {
            return mBlockHeight;
        }
        static int BlockWidth() {
            return mBlockWidth;
        }
        static int mBlankLineHeight;

        void ClearScr();
        void DrawLine(int x1,int y1,int x2,int y2,int color);
        void DrawRect(int x1,int y1,int x2,int y2,int color);

        virtual void PutPixel(int x,int y,int color) = 0;
        virtual void FillRect(int x1,int y1,int x2,int y2,int color) = 0;
        virtual void RevRect(int x1,int y1,int x2,int y2) = 0;
        virtual void SaveRect(int x1, int y1, int x2, int y2, struct BitMap* pBuffer );
        virtual void RstrRect(int x1, int y1, int x2, int y2, struct BitMap* pBuffer );
        
        virtual void SwitchToGraph() {};
        virtual void SwitchToText() {};
    protected:
        static int mXres, mYres;
        // font
	/*
	static BaseFont *mpAscFont;
        static BaseFont *mpDblFont;
	*/

        static int mBlockWidth;
        static int mBlockHeight;
        static struct CharBitMap mAsc;
        static struct CharBitMap mDbl;
};

#define fb_readb(addr) (*(volatile __u8 *) (addr))
#define fb_readw(addr) (*(volatile __u16 *) (addr))
#define fb_readl(addr) (*(volatile __u32 *) (addr))
#define fb_writeb(b,addr) (*(volatile __u8 *) (addr) = (b))
#define fb_writew(b,addr) (*(volatile __u16 *) (addr) = (b))
#define fb_writel(b,addr) (*(volatile __u32 *) (addr) = (b))

#endif
