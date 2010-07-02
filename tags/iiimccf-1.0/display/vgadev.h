// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          vgadev.h  -  description
                             -------------------
    begin                : Wed Sept 05 2001
    copyright            : (C) 2001 by rick, huyong
    email                : rick@chinaren.com
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

#ifndef VGADEV_H
#define VGADEV_H

//#include "basefont.h"

/**
 *@author Rick Lei
 */

#include "graphdev.h"
class VGADev : public GraphDev {
    public:
        static bool TryOpen();
        VGADev();
        ~VGADev();

        void PutPixel(int x,int y,int color);
        void FillRect(int x1,int y1,int x2,int y2,int color);
        void RevRect(int x1,int y1,int x2,int y2);
        void DrawChar(int x,int y,int fg,int bg,struct CharBitMap* pFont);

        void SwitchToGraph();
        void SwitchToText();

    private:
        static void EnableIOPerm();
        static void DisableIOPerm();
        static bool SetVideoMode(int mode);
        static void SetDefaultMode();
        static void SetWriteMode(int mode);
        static void SetBitMask(char mask);
        static void SetOper(int op);
        static void SetColor(int color);

    private:
        static char *mpBuf;
        static int mBufLen;
        static unsigned long mNextLine; // offset to one line below
        static int mFd;
};

#endif

