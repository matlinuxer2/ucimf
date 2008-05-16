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
        void SaveRect(int x1, int y1, int x2, int y2, char* *buffer );
        void RstrRect(int x1, int y1, int x2, int y2, char* *buffer );
        void DrawChar(int x,int y,int fg,int bg,struct CharBitMap* pFont);
        void PutPixel(int x,int y,int color);

    protected:  // accessed by fbcon15
        void InitColorMap();
        __u16 cfb16[16];
        static __u32 tab_cfb16[];
};

#endif
