// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          fblinear16.h  -  description
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

#ifndef FBLINEAR16_H
#define FBLINEAR16_H

#include "fbdev.h"

/**
 *@author huyong
 */
class FBLinear16 : public FBDev {
    public:
        FBLinear16();

        void FillRect(int x1,int y1,int x2,int y2,int color);
        void RevRect(int x1,int y1,int x2,int y2);
        void DrawChar(int x,int y,int fg,int bg,struct CharBitMap* pFont);
        void PutPixel(int x,int y,int color);

    protected:  // accessed by fbcon15
        void InitColorMap();
        __u16 cfb16[16];
        static __u32 tab_cfb16[];
};

#endif
