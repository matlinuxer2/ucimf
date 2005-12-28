// vi:ts=4:shiftwidth=4:expandtab
/***************************************************************************
                          fbdev.h  -  description
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

#ifndef FBDEV_H
#define FBDEV_H

//#include "basefont.h"
#include "graphdev.h"

class FBDev : public GraphDev  {
    public:
        virtual ~FBDev();
        // graphic
        static OPEN_RC TryOpen();
        // for FreeBSD
        static bool TryOpen(int xres, int yres, int depth);

        virtual void PutPixel(int x,int y,int color) = 0;
        virtual void FillRect(int x1,int y1,int x2,int y2,int color) = 0;
        virtual void DrawChar(int x,int y,int fg,int bg,struct CharBitMap* pFont) = 0;
        virtual void SaveRect(int x1, int y1, int x2, int y2, struct BitMap* pBuffer ) = 0;
        virtual void RstrRect(int x1, int y1, int x2, int y2, struct BitMap* pBuffer ) = 0;

        void SwitchToGraph();
        void SwitchToText();

    private:
        static OPEN_RC LinearSet( struct fb_var_screeninfo &Vinfo );
        static void VGAPlaneSet( void );
        static int mCurrentMode;  // for FreeBSD

    protected:
        // mmap framebuffer address
        static char *mpBuf;
        static int mpBufLen;

        static unsigned long mNextLine; // offset to one line below
        static unsigned long mNextPlane; // offset to one plane below
        // FrameBuffer file handle
        static int mFd;
        static __u16 red16[],green16[],blue16[];
};

#endif


