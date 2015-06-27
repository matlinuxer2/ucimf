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

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <assert.h>

#include "fbdev.h"
#include "fblinear4.h"
#include "fblinear8.h"
#include "fblinear15.h"
#include "fblinear16.h"
#include "fblinear24.h"
#include "fblinear32.h"
#ifdef USING_VGA
#include "fbvgaplanes.h"
#endif

// mmap framebuffer address
char *FBDev::mpBuf = NULL;
int FBDev::mpBufLen = 0;
// FrameBuffer file handle
int FBDev::mFd = -1;
unsigned long FBDev::mNextLine = 0;
unsigned long FBDev::mNextPlane = 0;
int FBDev::mCurrentMode = 0;
//colormap
__u16 FBDev::red16[] = {
0x0000, 0x0000, 0x0000, 0x0000, 0xaaaa, 0xaaaa, 0xaaaa, 0xaaaa,
0x5555, 0x5555, 0x5555, 0x5555, 0xffff, 0xffff, 0xffff, 0xffff
};
__u16 FBDev::green16[] = {
0x0000, 0x0000, 0xaaaa, 0xaaaa, 0x0000, 0x0000, 0x5555, 0xaaaa,
0x5555, 0x5555, 0xffff, 0xffff, 0x5555, 0x5555, 0xffff, 0xffff
};
__u16 FBDev::blue16[] = {
0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa,
0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff
};

#if defined(linux)
    #include <linux/fb.h>
    #define MAX_DEV_LEN 63
#endif
OPEN_RC FBDev::TryOpen() {
    OPEN_RC rc = NORMAL;
#if defined(linux)
    char devname[MAX_DEV_LEN+1];
    int fbNo;
    // Find a framebuffer to open
    for (fbNo=0; fbNo < 32; fbNo++) {
        sprintf(devname, "/dev/fb%d", fbNo);
        mFd = open(devname, O_RDWR);
        if (mFd >= 0) break;
        sprintf(devname, "/dev/fb/%d", fbNo);
        mFd = open(devname, O_RDWR);
        if (mFd >= 0) break;
    }

    if (fbNo >= 32) {
        return FAILURE;
    }

    rc = NORMAL;
    static struct fb_fix_screeninfo Finfo;
    static struct fb_var_screeninfo Vinfo;
    ioctl(mFd, FBIOGET_FSCREENINFO, &Finfo);
    ioctl(mFd, FBIOGET_VSCREENINFO, &Vinfo);
    switch (Finfo.type) {
        case FB_TYPE_PACKED_PIXELS:      // truecolor packed pixels
           rc = LinearSet(Vinfo);
           break;
        case FB_TYPE_PLANES:             // Non interleaved planes
           // throw(runtime_error("Can not support FB_TYPE, planes not interleaved.\n"));
           rc = UNSUPPORT;
           break;
        case FB_TYPE_INTERLEAVED_PLANES: // Interleaved planes
           // throw(runtime_error("Can not support FB_TYPE, planes interleaved.\n"));
           rc = UNSUPPORT;
           break;
        case FB_TYPE_TEXT:               // Text/attributes
           // throw(runtime_error("Can not support FB_TYPE, VGA text mode.\n"));
           rc = UNSUPPORT;
           break;
#ifdef USING_VGA
        case FB_TYPE_VGA_PLANES:         // EGA/VGA planes
           VGAPlaneSet();
           // throw(runtime_error("Can not support FB_TYPE, EGA/VGA mode.\n"));
           break;
#endif
        default:
           // throw(runtime_error("Unknow FrameBuffer type.\n"));
           rc = UNSUPPORT;
           break;
    }
    if (rc != NORMAL) {
        close(mFd);
        return rc;
    }

    mXres=Finfo.line_length/(Vinfo.bits_per_pixel/8);
    mYres = Vinfo.yres;
    mpBufLen = Finfo.smem_len;
    mpBuf = (char*)mmap(0,mpBufLen,PROT_READ | PROT_WRITE, MAP_SHARED, mFd, 0);
    if (mpBuf == MAP_FAILED)
        //throw(runtime_error("fb device mmap failed!"));
    mNextLine = Finfo.line_length;
#endif
    return rc;
}

OPEN_RC FBDev::LinearSet( struct fb_var_screeninfo &Vinfo ) {
    OPEN_RC rc = NORMAL;
#if defined(linux)
    switch (Vinfo.bits_per_pixel) {
        //case 4:
            //mpGraphDev = new FBLinear4;
            //break;
        case 8:
            mpGraphDev = new FBLinear8;
            break;
        case 15:
            mpGraphDev = new FBLinear15;
            break;
         case 16:
            if (Vinfo.green.length == 5)
                mpGraphDev = new FBLinear15;
            else
                mpGraphDev = new FBLinear16;
            break;
         case 24:
            mpGraphDev = new FBLinear24;
            break;
         case 32:
            mpGraphDev = new FBLinear32;
            break;
         default:
            rc = UNSUPPORT;
            //throw(runtime_error("4bpp color depth unsupported in this version\n"
            //                    "use 8bpp, 16bpp, 24bpp, 32bpp instead!"));
            break;
    }
#endif
    return rc;
}

// may EGA are included
#ifdef USING_VGA
void FBDev::VGAPlaneSet( void ) {
    mpGraphDev = new FBVgaPlanes;
}
#endif

#if defined(__FreeBSD__)
    #include <sys/types.h>
    #include <sys/ioctl.h>
    //#include <machine/console.h>
    #include <sys/consio.h>
    #include <sys/fbio.h>
    #include <machine/sysarch.h>
#endif
bool FBDev::TryOpen(int xres, int yres, int depth) {
#if defined(__FreeBSD__)
    video_info_t mode = {0};
    mode.vi_width = xres;
    mode.vi_height = yres;
    mode.vi_depth = depth;
    if (ioctl(0, FBIO_FINDMODE, &mode)) {
        return false;
        throw(runtime_error("Cannot find graphics mode. Check your zhcon.conf please.\n"));
    }
    if ((mode.vi_mode >= M_B40x25) && (mode.vi_mode <= M_VGA_M90x60))
        mCurrentMode = _IO('S', mode.vi_mode);
    if ((mode.vi_mode >= M_TEXT_80x25) && (mode.vi_mode <= M_TEXT_132x60))
        mCurrentMode = _IO('S', mode.vi_mode);
    if ((mode.vi_mode >= M_VESA_CG640x400) && (mode.vi_mode <= M_VESA_FULL_1280))
        mCurrentMode = _IO('V', mode.vi_mode - M_VESA_BASE);

    // FreeBSD mode query bug workaround -- Rick
    if (depth == 4 && mCurrentMode == SW_VGA11)
        mCurrentMode = SW_VGA12;

    if (ioctl(0, mCurrentMode, 0) != 0) {
        throw(runtime_error("Cannot switch to graphics mode. Refer to README/FAQ"
                            " and check your zhcon.cfg please!\n"));
    }

    mXres = mode.vi_width;
    mYres = mode.vi_height;
    mpBufLen = mode.vi_buffer_size;
    if (mode.vi_mem_model == V_INFO_MM_PLANAR) {
        mpBufLen = mode.vi_width * mode.vi_height / 8;
    }

    // printf ("len %d\n", mpBufLen);
    // use /dev/mem crash in Virtual PC
//#if (__FreeBSD__ <= 3)
    if ((mFd = open("/dev/vga", O_RDWR | O_NDELAY)) < 0) {
//#else
//    if ((mFd = open("/dev/mem", O_RDWR | O_NDELAY)) < 0) {
//#endif
        throw(runtime_error("Can not open vga device.\n"));
    }

    switch (depth) {
        case 4:
#ifdef USING_VGA
            mpGraphDev = new FBVgaPlanes;
#else
            throw(runtime_error("color depth unsupported in this version\n"
                "use 8bpp instead!"));
#endif
            break;
        case 8:
            mpGraphDev = new FBLinear8;
            break;
        case 15:
            mpGraphDev = new FBLinear15;
            break;
        case 16:
            mpGraphDev = new FBLinear16;
            break;
        case 24:
            mpGraphDev = new FBLinear24;
            break;
        case 32:
            mpGraphDev = new FBLinear32;
            break;
        default:
            throw(runtime_error("color depth unsupported in this version\n"
                "use 8bpp instead!"));
    }
//#if (__FreeBSD__ <= 3)
#define GRAPH_BASE 0x0
//#else
//#define GRAPH_BASE 0xA0000
//#endif
    mpBuf = static_cast<char *>(mmap(0, mpBufLen, PROT_READ | PROT_WRITE,
            MAP_FILE|MAP_SHARED|MAP_FIXED, mFd, GRAPH_BASE));

    if (mpBuf == MAP_FAILED)
        throw(runtime_error("mmap() failed!"));

    // undefine mNextLine, checked by low level
#endif
    return true;
}

FBDev::~FBDev() {
    if (mpBuf != NULL)
        munmap(mpBuf, mpBufLen);
    if (mFd >= 0)
        close(mFd);
#if defined(__FreeBSD__)
    ioctl(0, SW_TEXT_80x25, 0);
#endif
}

void FBDev::SwitchToGraph() {
#if defined(__FreeBSD__)
    if (mCurrentMode == 0)
        return;
    ioctl(0, mCurrentMode, 0);
#endif
}

void FBDev::SwitchToText() {
#if defined(__FreeBSD__)
    ioctl(0, SW_TEXT_80x25, 0);
#endif
}


