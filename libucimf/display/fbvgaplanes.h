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

#ifndef FBVGAPLANES_H
#define FBVGAPLANES_H

#include "fbdev.h"

/**
 *@author huyong, based on kernel drivers/video/fbcon-vga-planes.c
 */
class FBVgaPlanes : public FBDev {
    public:
        FBVgaPlanes();
        ~FBVgaPlanes();

        void FillRect(int x1,int y1,int x2,int y2,int color);
        void RevRect(int x1,int y1,int x2,int y2);
        void PutPixel(int x,int y,int color);
        void DrawChar(int x,int y,int fg,int bg,struct CharBitMap* pFont);

    private:
        // EGA/VGA special routine
        /* Set the Graphics Mode Register.  Bits 0-1 are write mode, bit 3 is
           read mode. */
        static inline void setmode(int mode);
        /* Select the Bit Mask Register. */
        static inline void selectmask(void);
        /* Set the value of the Bit Mask Register.  It must already have been
           selected with selectmask(). */
        static inline void setmask(int mask);
        /* Set the Data Rotate Register.  Bits 0-2 are rotate count, bits 3-4
           are logical operation (0=NOP, 1=AND, 2=OR, 3=XOR). */
        static inline void setop(int op);
        /* Set the Enable Set/Reset Register.  The code here always uses value
           0xf for this register.  */
        static inline void setsr(int sr);
        /* Set the Set/Reset Register. */
        static inline void setcolor(int color);
        /* Set the value in the Graphics Address Register. */
        static inline void setindex(int index);

        static void SetDefaultMode();
};

#endif

