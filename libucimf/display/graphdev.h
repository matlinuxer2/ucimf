/*
 * UCIMF - Unicode Console InputMethod Framework                                                    
 *
 * Copyright (c) 2006-2007 Chun-Yu Lee (Mat) and Open RazzmatazZ Laboratory (OrzLab)
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

#ifndef GRPATHDEV_H
#define GRPATHDEV_H

typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned int __u32;

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
        int OutChar(int x, int y, int fg, int bg, unsigned int c);

        void ClearScr();
        void DrawLine(int x1,int y1,int x2,int y2,int color);
        void DrawRect(int x1,int y1,int x2,int y2,int color);

        virtual void PutPixel(int x,int y,int color) = 0;
        virtual void FillRect(int x1,int y1,int x2,int y2,int color) = 0;
        virtual void RevRect(int x1,int y1,int x2,int y2) = 0;
        virtual void SaveRect(int x1, int y1, int x2, int y2, char** buffer ) = 0;
        virtual void RstrRect(int x1, int y1, int x2, int y2, char** buffer ) = 0;
        
    protected:
        static int mXres, mYres;
};

#define fb_readb(addr) (*(volatile __u8 *) (addr))
#define fb_readw(addr) (*(volatile __u16 *) (addr))
#define fb_readl(addr) (*(volatile __u32 *) (addr))
#define fb_writeb(b,addr) (*(volatile __u8 *) (addr) = (b))
#define fb_writew(b,addr) (*(volatile __u16 *) (addr) = (b))
#define fb_writel(b,addr) (*(volatile __u32 *) (addr) = (b))

#endif
