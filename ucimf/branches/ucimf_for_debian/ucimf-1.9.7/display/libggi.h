// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          libggi.h  -  description
                             -------------------
    begin                : Fri July 20 2001
    copyright            : (C) 2001 by huyong
    email                : ccpaging@online.sh.cn

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

