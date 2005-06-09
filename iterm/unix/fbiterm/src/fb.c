/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "fbiterm.h"

typedef struct _ViewMethod
{
  unsigned short bpp;
  SetPaletteProc set_palette;
  ClearRectProc clear_rect;
  SetRenditionProc set_rendition;
  DrawTextProc draw_text;
}
ViewMethod;

static ViewMethod vm[] = {
  {8, fb_true8_set_palette, fb_true8_clear_rect,
   fb_true8_set_rendition, fb_true8_draw_text},
  {16, fb_true16_set_palette, fb_true16_clear_rect,
   fb_true16_set_rendition, fb_true16_draw_text},
  {24, fb_true24_set_palette, fb_true24_clear_rect,
   fb_true24_set_rendition, fb_true24_draw_text},
  {32, fb_true32_set_palette, fb_true32_clear_rect,
   fb_true32_set_rendition, fb_true32_draw_text},
};

void
fb_end ()
{
  /* clear all data in frame buffer */
  memset (pIterm->fb->buf, 0x0, pIterm->fb->smem_len);

  /* unmap frame buffer */
  if (pIterm->fb->buf)
    munmap (pIterm->fb->buf, pIterm->fb->smem_len);

  /* close frame buffer device */
  close (pIterm->fb->fbfp);
  return;
}

int
InitFb ()
{
  struct fb_fix_screeninfo fscinfo;
  struct fb_var_screeninfo vscinfo;
  int cellWidth, cellHeight;

  /* if FRAMEBUFFER env. variable is already set, use it */
  if (getenv (FB_ENV))
    pIterm->fb->fbdev = getenv (FB_ENV);
  else
    pIterm->fb->fbdev = FB_DEFDEV;

  /* open frame buffer device */
  pIterm->fb->fbfp = open (pIterm->fb->fbdev, O_RDWR);
  if (pIterm->fb->fbfp == -1)
    {
      fprintf (stderr, "Frame buffer device %s : open error\r\n",
	       pIterm->fb->fbdev);
      return -1;
    }

  /* get fixed framebuffer info. for smem_len/visual/line_length */
  if (ioctl (pIterm->fb->fbfp, FBIOGET_FSCREENINFO, &fscinfo))
    {
      fprintf (stderr, "ioctl (FBIOGET_FSCREENINFO) system call failed.\n");
      return -1;
    }

  /* get variable framebuffer info. for bits_per_pixel/[rgb].[len,offset] */
  if (ioctl (pIterm->fb->fbfp, FBIOGET_VSCREENINFO, &vscinfo))
    {
      fprintf (stderr, "ioctl (FBIOGET_VSCREENINFO) system call failed.\n");
      return -1;
    }

  /* reset x and y offsets */
  if (vscinfo.xoffset != 0 || vscinfo.yoffset != 0)
    {
      vscinfo.xoffset = vscinfo.yoffset = 0;
      if (ioctl (pIterm->fb->fbfp, FBIOPAN_DISPLAY, &vscinfo))
	{
	  fprintf (stderr, "ioctl (FBIOPAN_DISPLAY) system call failed.\n");
	  return -1;
	}
    }

  /* set framebuffer params */
  pIterm->fb->line_length = fscinfo.line_length;
  pIterm->fb->smem_len = fscinfo.smem_len;

  /* calculate total columns */
  cellWidth = pIterm->cell_width;
  if (cellWidth > 0)
    pIterm->cols = vscinfo.xres / cellWidth;

  /* calculate total rows */
  cellHeight = pIterm->cell_height;
  if (cellHeight > 0)
    /* pIterm->rows = vscinfo.yres / cellHeight - 1; */
    pIterm->rows = vscinfo.yres / cellHeight;

  /* set view method to global structure */
  if (fscinfo.type == FB_TYPE_PACKED_PIXELS)
    {
      int i, found;
      for (i = 0, found = False; i < sizeof (vm) / sizeof (ViewMethod); i++)
	{
	  if (vm[i].bpp == vscinfo.bits_per_pixel)
	    {
	      pIterm->fb->set_palette = vm[i].set_palette;
	      pIterm->fb->draw_text = vm[i].draw_text;
	      pIterm->fb->clear_rect = vm[i].clear_rect;
	      pIterm->fb->set_rendition = vm[i].set_rendition;
	      found = True;
	      break;
	    }
	}
      if (found != True)
	{
	  fprintf (stderr,
		   "unsupported visual mode (mode[%x] bits_per_pixel[%d])\n",
		   fscinfo.visual, vscinfo.bits_per_pixel);
	  close (pIterm->fb->fbfp);
	  return -1;
	}
    }
  else
    {
      fprintf (stderr,
	       "unsupported visual mode (mode[%x] bits_per_pixel[%d])\n",
	       fscinfo.visual, vscinfo.bits_per_pixel);
      close (pIterm->fb->fbfp);
      return -1;
    }

  /* set palette */
  pIterm->fb->set_palette ();

  /* mmap frame buffer area to memory */
  pIterm->fb->buf = (unsigned char *) mmap (NULL, fscinfo.smem_len,
					    PROT_READ | PROT_WRITE,
					    MAP_SHARED, pIterm->fb->fbfp,
					    (off_t) 0);

  if (pIterm->fb->buf == MAP_FAILED)
    {
      if (pIterm->fb->buf)
	munmap (pIterm->fb->buf, fscinfo.smem_len);
      close (pIterm->fb->fbfp);
      fprintf (stderr, "mmap system call failed.\n");
      return -1;
    }
  return 0;
}
