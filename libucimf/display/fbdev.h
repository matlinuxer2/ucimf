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

#ifndef FBDEV_H
#define FBDEV_H

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
        virtual void SaveRect(int x1, int y1, int x2, int y2, char** buffer ) = 0;
        virtual void RstrRect(int x1, int y1, int x2, int y2, char** buffer ) = 0;

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


