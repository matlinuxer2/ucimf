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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef USING_VGA
#include <assert.h>
#include <unistd.h>
#include "debug.h"
#include "fbvgaplanes.h"

#if defined(linux)
    #include <sys/io.h>
#elif defined(__FreeBSD__)
    #include <machine/sysarch.h>
static inline void outb(char value, unsigned short port)
{
    __asm__ ("outb %0, %1"
        :
        : "a" ((char) value),
        "d" ((unsigned short) port));
}
#endif

/* based on kernel
 * Force strict CPU ordering.
 * And yes, this is required on UP too when we're talking
 * to devices.
 *
 * For now, "wmb()" doesn't actually do anything, as all
 * Intel CPU's follow what Intel calls a *Processor Order*,
 * in which all writes are seen in the program order even
 * outside the CPU.
 *
 * I expect future Intel CPU's to have a weaker ordering,
 * but I'd also expect them to finally get their act together
 * and add some real memory barriers if so.
 */

#if defined(linux)
    #define mb()    __asm__ __volatile__ ("lock; addl $0,0(%%esp)": : :"memory")
    #define rmb()   mb()
    #define wmb()   __asm__ __volatile__ ("": : :"memory")
#elif defined(__FreeBSD__)
    #define mb()
    #define rmb()
    #define wmb()
#endif

#define GRAPHICS_ADDR_REG 0x3ce     /* Graphics address register. */
#define GRAPHICS_DATA_REG 0x3cf     /* Graphics data register. */

#define SET_RESET_INDEX 0           /* Set/Reset Register index. */
#define ENABLE_SET_RESET_INDEX 1    /* Enable Set/Reset Register index. */
#define DATA_ROTATE_INDEX 3         /* Data Rotate Register index. */
#define GRAPHICS_MODE_INDEX 5       /* Graphics Mode Register index. */
#define BIT_MASK_INDEX 8            /* Bit Mask Register index. */

/* Set the Graphics Mode Register.  Bits 0-1 are write mode, bit 3 is
   read mode. */
void FBVgaPlanes::setmode(int mode)
{
    outb(GRAPHICS_MODE_INDEX, GRAPHICS_ADDR_REG);
    outb(mode, GRAPHICS_DATA_REG);
}

/* Select the Bit Mask Register. */
void FBVgaPlanes::selectmask(void)
{
    outb(BIT_MASK_INDEX, GRAPHICS_ADDR_REG);
}

/* Set the value of the Bit Mask Register.  It must already have been
   selected with selectmask(). */
void FBVgaPlanes::setmask(int mask)
{
    outb(mask, GRAPHICS_DATA_REG);
}

/* Set the Data Rotate Register.  Bits 0-2 are rotate count, bits 3-4
   are logical operation (0=NOP, 1=AND, 2=OR, 3=XOR). */
void FBVgaPlanes::setop(int op)
{
    outb(DATA_ROTATE_INDEX, GRAPHICS_ADDR_REG);
    outb(op, GRAPHICS_DATA_REG);
}

/* Set the Enable Set/Reset Register.  The code here always uses value
   0xf for this register.  */
void FBVgaPlanes::setsr(int sr)
{
    outb(ENABLE_SET_RESET_INDEX, GRAPHICS_ADDR_REG);
    outb(sr, GRAPHICS_DATA_REG);
}

/* Set the Set/Reset Register. */
void FBVgaPlanes::setcolor(int color)
{
    outb(SET_RESET_INDEX, GRAPHICS_ADDR_REG);
    outb(color, GRAPHICS_DATA_REG);
}

/* Set the value in the Graphics Address Register. */
void FBVgaPlanes::setindex(int index)
{
    outb(index, GRAPHICS_ADDR_REG);
}

FBVgaPlanes::FBVgaPlanes() {
    mNextLine = mNextLine ? mNextLine : mXres>>3;

    // based on cce
#if defined(linux)
    ioperm(GRAPHICS_ADDR_REG, 1, 1);
    ioperm(GRAPHICS_DATA_REG, 1, 1);
#elif defined(__FreeBSD__)
    i386_set_ioperm(GRAPHICS_ADDR_REG, 1, 1);
    i386_set_ioperm(GRAPHICS_DATA_REG, 1, 1);
#endif
    SetDefaultMode();
}

FBVgaPlanes::~FBVgaPlanes() {
#if defined(linux)
    ioperm(GRAPHICS_ADDR_REG, 1, 0);
    ioperm(GRAPHICS_DATA_REG, 1, 0);
#elif defined(__FreeBSD__)
    i386_set_ioperm(GRAPHICS_ADDR_REG, 1, 0);
    i386_set_ioperm(GRAPHICS_DATA_REG, 1, 0);
#endif
}

inline void FBVgaPlanes::SetDefaultMode() {
    setmode(3);
    setop(0);
    setsr(0xf);
    selectmask();
    setmask(0xff);
}

void FBVgaPlanes::FillRect(int x1,int y1,int x2,int y2,int color) {
#ifndef NDEBUG
    //debug<<"FillRect ("<<x1<<","<<y1<<")-("<<x2<<","<<y2<<")"<<endl;
#endif
    assert( x1 >= 0 && x1 < mXres && y1 >=0 && y1 < mYres);
    assert( x2 >= 0 && x2 < mXres && y2 >=0 && y2 < mYres);
    assert(x1 <= x2 && y1 <= y2);

    SetDefaultMode();
    setcolor(color);

    __u8* destorig = (__u8*)mpBuf + mNextLine * y1 + (x1>>3);
    __u8* dest = destorig;

    int width = x2 - x1 + 1;
    int left, right, midbytes;
    left = 8 - (x1 & 7);
    // left maybe 8
    if (left == 8) left = 0;

    if (left >= width) {
        right = 0;
        midbytes = 0;
    } else {
        right = (x1 + width) & 7;
        midbytes = (width - left - right) / 8;
    }
#ifndef NDEBUG
    if (left + right > width || midbytes < 0) {
        debug<<"FillRect x1="<<x1<<" width="<<width<<" left="<<left;
        debug<<" midbytes="<<midbytes<<" right="<<right<<endl;
    }
#endif
    
    int height;
    if (left) {
        __u8 leftmask = 0xff >> (8 - left);
        if (left > width) leftmask &= 0xff << (left - width);
        rmb();
        for (height = y2 - y1 + 1; height--; dest += mNextLine) {
            fb_readb(dest); // fill latches
            *dest = leftmask;
        }
        wmb();
        destorig++;
    }
    if (midbytes) {
        dest = destorig;
        for (height = y2 - y1 + 1; height--; dest += mNextLine) {
            memset(dest, 0xff, midbytes);
        }
        wmb();
        destorig += midbytes;
    }
    if (right) {
        dest = destorig;
        __u8 rightmask = 0xff << (8 - right);
        rmb();
        for (height = y2 - y1 + 1; height--; dest += mNextLine) {
            fb_readb(dest); // fill latches
            *dest = rightmask;
        }
        wmb();
    }
    /*
    for(int height = y2 - y1 + 1; height--; dest += mNextLine) {
        dest8 = dest;
        if (left) {
            fb_readb(dest8); // fill latches
            *dest8++ = leftmask;
        }
        if (midbytes) {
            dest8 += midbytes;
        }
        if (right) {
            fb_readb(dest8); // fill latches
            *dest8++ = rightmask;
        }
    }
    wmb();
    */
}

void FBVgaPlanes::RevRect(int x1,int y1,int x2,int y2) {
    assert( x1 >= 0 && x1 < Width() && y1 >=0 && y1 < Height());
    assert( x2 >= 0 && x2 < Width() && y2 >=0 && y2 < Height());
    assert(x1 <= x2 && y1 <= y2);

    __u8* dest = (__u8*)mpBuf + mNextLine * y1 + (x1>>3);
    __u8* dest8;

    setmode(3);
    setop(0x18);
    setsr(0xf);
    setcolor(0xf);
    selectmask();
    setmask(0xff);
    
    int width = x2 - x1 + 1;
    int left, right, midbytes;
    left = 8 - (x1 & 7);
    // left maybe 8
    if (left == 8) left = 0;

    if (left >= width) {
        right = 0;
        midbytes = 0;
    } else {
        right = (x1 + width) & 7;
        midbytes = (width - left - right) / 8;
    }

    __u8 leftmask = 0xff >> (8 - left);
    if (left > width) leftmask &= 0xff << (left - width);
    __u8 rightmask = 0xff << (8 - right);
    int cnt;
    rmb();
    for(int height = y2 - y1 + 1; height--; dest += mNextLine) {
        dest8 = dest;
        if (left) {
            fb_readb(dest8);
            fb_writeb(leftmask, dest8++);
        }
        if (midbytes) {
            for(cnt = midbytes; cnt--; ) {
                fb_readb(dest8);
                fb_writeb(0xff, dest8++);
            }
        }
        if (right) {
            fb_readb(dest8);
            fb_writeb(rightmask, dest8);
        }
    }
    wmb();
    SetDefaultMode();
}

void FBVgaPlanes::PutPixel(int x,int y,int color) {
    SetDefaultMode();
    setcolor(color);

    __u8* dest = (__u8*)mpBuf + mNextLine * y + (x >> 3);

    rmb();
    fb_readb(dest); // fill latches
    *dest = 0x80 >> (x & 7);
    wmb();
}

#endif //USING_VGA
