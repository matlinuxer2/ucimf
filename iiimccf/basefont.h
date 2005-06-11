// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          basefont.h  -  description
                             -------------------
    begin                : Fri Mar 16 2001
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

#ifndef BASEFONT_H
#define BASEFONT_H

/**
 *@author ejoy
 */
#include <string>
using namespace std;
//full or half hz when output
enum CharTag {
    FULL,LEFT,RIGHT
};

class BaseFont {
    public:
        BaseFont(string& fn,int w,int h);
        virtual ~BaseFont();
        int Width() const {
            return mWidth;
        }
        int Height() const {
            return mHeight;
        }
        int WidthBytes() const {
            return mWidthBytes;
        }
        char *GetChar(char c);
        char *GetChar(char c1, char c2);

    protected:
        /** mmap address of font(or buf) */
        int mFd;
        char* mpBuf;
        char* mpNull;
        unsigned mBufSize;
        int mWidth;
        int mHeight;
        long mNumChars;
        unsigned mDataOffset;
        int mByteLen;  //font size in bytes(not include blankline)
        int mWidthBytes;
        void GetInfo();
};
#endif
