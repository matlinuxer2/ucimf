/*
 * UCIMF - Unicode Console InputMethod Framework                                                    
 *
 * Copyright (c) 2007 Chun-Yu Lee (Mat) and Open RazzmatazZ Laboratory (OrzLab)
 *                                                                                                  
 * Parts taken from zhcon project.
 * Copyright (c) 2001 rick <rick@chinaren.com>
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

