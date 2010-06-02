// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          fblinear8.h  -  description
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

#ifndef FBLINEAR8_H
#define FBLINEAR8_H

#include "fbdev.h"

/**
 *@author huyong
 */
class FBLinear8 : public FBDev {
    public:
        FBLinear8();

        void FillRect(int x1,int y1,int x2,int y2,int color);
        void RevRect(int x1,int y1,int x2,int y2);
        void SaveRect(int x1, int y1, int x2, int y2, char* *buffer);
        void RstrRect(int x1, int y1, int x2, int y2, char* *buffer);
        void PutPixel(int x,int y,int color);

    private:
        static __u32 nibbletab_cfb8[];
};

#endif

