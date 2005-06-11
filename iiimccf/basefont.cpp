// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          basefont.cpp  -  description
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

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdexcept>
#include <cassert>
#include "global.h"
#include "debug.h"
#include "hzdecoder.h"
#include "basefont.h"

BaseFont::BaseFont(string & fn, int w, int h)
:mFd(0)
,mpBuf(0)
,mWidth(w)
,mHeight(h) {
    mFd = open(fn.c_str(), O_RDONLY);
    if (mFd == -1)
        throw (runtime_error("can not open gbfont!"));

    struct stat st;
    if (fstat(mFd, &st) == -1)
        throw (runtime_error("can not get gbfont size!"));
    mBufSize = st.st_size;

    if (mBufSize > 50000) {
        //hzfont use mmap
        mpBuf = (char *) mmap((caddr_t) 0, mBufSize, PROT_READ,
                    MAP_FILE | MAP_SHARED, mFd, (off_t) 0);
        if (mpBuf == MAP_FAILED)
            throw (runtime_error("error in mmap gbfont!"));
    } else {
        //ascii font read to buffer
        mpBuf = (char *) new char[mBufSize];
        unsigned nread = read(mFd, mpBuf, mBufSize);
        if (nread != mBufSize)
            throw (runtime_error("error in reading asciifont!"));
    }
    GetInfo();
    mpNull = (char *) new char[mByteLen];
    memset(mpNull, 0xff, mByteLen);
}

BaseFont::~BaseFont() {
    if (mBufSize > 50000)
        munmap(mpBuf, mBufSize);
    else
        delete[] mpBuf;

    delete[] mpNull;
    close(mFd);
}

void BaseFont::GetInfo() {
    if (mBufSize > 4 && (mpBuf[0] == 0x36 && mpBuf[1] == 0x04)) {
        if (mpBuf[2] > 3) {  // extend PCF for big charset
            mHeight = (int) mpBuf[3];
            mWidth = (int) mpBuf[4];
            mDataOffset = 9;
            memcpy(&mNumChars, &mpBuf[5], sizeof(long));
        } else {  // Linux/i386 PC Screen Font data
            mNumChars = ((mpBuf[2] == 1 || mpBuf[2] == 3) ? 512 : 256);
            mHeight = (int) mpBuf[3];
            mDataOffset = 4;
        }
        mWidthBytes = mWidth / 8 + ((mWidth % 8) ? 1: 0);
    } else {                        // RAW font data
        mWidthBytes = mWidth / 8 + ((mWidth % 8) ? 1: 0);
        mNumChars = mBufSize / mWidthBytes;
        mDataOffset = 0;
    }
    assert(mHeight > 0 && mWidth > 0 && mNumChars > 0);
    mByteLen = mHeight * mWidthBytes;
}

char *BaseFont::GetChar(char c) {
    char* p = mpBuf + mDataOffset + c * mByteLen;
    if (p > mpBuf + mBufSize - 1)
        return mpNull;
    else
        return p;
}

char *BaseFont::GetChar(char c1, char c2) {
    char* p = mpBuf + mDataOffset + mByteLen * gpDecoder->Index(c1,c2);
    if (p > mpBuf + mBufSize - 1)
        return mpNull;
    else
        return p;
}

