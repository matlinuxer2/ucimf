// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          fblinear4.h  -  description
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

#ifndef FBLINEAR4_H
#define FBLINEAR4_H

#include "fbdev.h"

/**
 *@author huyong
 */
class FBLinear4 : public FBDev {
    public:
        FBLinear4();

        void PutPixel(int x,int y,int color);
        void FillRect(int x1,int y1,int x2,int y2,int color);
        void DrawChar(int x,int y,int fg,int bg,struct CharBitMap* pFont);

    private:
        void InitColorMap();
        static __u16 nibbletab_cfb4[];
};

#endif
