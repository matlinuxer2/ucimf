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

#ifndef LIBGGI_H
#define LIBGGI_H

#include <ggi/ggi.h>
//#include "basefont.h"
#include "graphdev.h"

class LIBGGI : public GraphDev  {
    public:
        static bool TryOpen(char *ggiDriveName);
        virtual ~LIBGGI();

        void PutPixel(int x,int y,int color);
        void FillRect(int x1,int y1,int x2,int y2,int color);
        void RevRect(int x1,int y1,int x2,int y2);
        void DrawChar(int x,int y,int fg,int bg,struct CharBitMap* pFont);

        void SwitchToGraph();
        void SwitchToText();
    private:
        int GetPixMap(char* expanded,int fg,int bg,struct CharBitMap* pFont);

        static char* mpixMapBuf;
        static size_t mPixMapBufSize;
        static char* GetPixMapBuf(size_t size);
        
        static ggi_visual_t visual;
        static ggi_mode mode;
        static ggi_pixel mPixelColor[16];
        static int mBytesPixel;
};

#endif

