// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          fblinear15.h  -  description
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

#ifndef FBLINEAR15_H
#define FBLINEAR15_H

#include "fblinear16.h"

/**
 *@author huyong
 */
class FBLinear15 : public FBLinear16 {
    public:
        FBLinear15();

    private:
        void InitColorMap();
};
#endif
