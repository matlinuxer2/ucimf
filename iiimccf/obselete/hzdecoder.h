// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          hzdecoder.h  -  description
                             -------------------
    begin                : Thu Mar 22 2001
    copyright            : (C) 2001 by ejoy
    email                : ejoy@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HZDECODER_H
#define HZDECODER_H

/**
 *@author ejoy
 */

class HzDecoder {
    public:
        HzDecoder();
        virtual ~HzDecoder();

        virtual bool IsCode2(char c) = 0;
        virtual bool IsCode1(char c) = 0;
        virtual unsigned int Index(char c1,char c2) = 0;
};
#endif
