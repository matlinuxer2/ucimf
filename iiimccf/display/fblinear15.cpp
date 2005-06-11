// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          fblinear15.cpp  -  description
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

#include <assert.h>
#include "global.h"
#include "fblinear15.h"

FBLinear15::FBLinear15() {
    InitColorMap();
    mNextLine = mNextLine ? mNextLine : mXres<<1;
}

void FBLinear15::InitColorMap() {
    int i;
    __u32 red, green, blue;
    for(i = 0; i < 16; i++) {
        red = red16[i];
        green = green16[i];
        blue = blue16[i];

        // 1:5:5:5
        cfb16[i] =  ((red   & 0xf800) >> 1) |
                    ((green & 0xf800) >> 6) |
                    ((blue  & 0xf800) >> 11);
    }
}


