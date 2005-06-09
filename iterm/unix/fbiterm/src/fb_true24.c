/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include "fbiterm.h"
#include "palette.h"

static unsigned long cfb24[16];
static unsigned long fgcmap;
static unsigned long bgcmap;
static unsigned long boldcmap;

void
fb_true24_set_palette ()
{
  int i;
  unsigned int red, green, blue;
  for (i = 0; i < 16; i++)
    {
      red = red16[i];
      green = green16[i];
      blue = blue16[i];
      cfb24[i] = ((blue & 0xff00) << 8) | (green & 0xff00) | (red >> 8);
    }
  fgcmap = cfb24[7];
  bgcmap = cfb24[0];
  return;
}

void
fb_true24_clear_rect (int s_col, int s_row, int e_col, int e_row)
{
  int sx, sy, ex, ey, i, len;

  if (pIterm->lock)
    return;

  sx = s_col * pIterm->cell_width * 3;
  sy = s_row * pIterm->cell_height;
  ex = e_col * pIterm->cell_width * 3;
  ey = e_row * pIterm->cell_height;
  len = ex - sx;

  for (i = sy; i < ey; i++)
    memset ((pIterm->fb->buf + sx + (i * pIterm->fb->line_length)), 0, len);
  return;
}

void
fb_true24_set_rendition (int bold, int blink, int inverse, int underline,
			 int foreground, int background, char charset)
{
  unsigned long tmp;

  if (pIterm->lock)
    return;

  if (foreground == 0)
    {
      /* white */
      fgcmap = cfb24[7];
      boldcmap = cfb24[7 | 0x08];
    }
  else
    {
      fgcmap = cfb24[foreground - 1];
      boldcmap = cfb24[(foreground - 1) | 0x08];
    }

  if (background == 0)
    /* black */
    bgcmap = cfb24[0];
  else
    bgcmap = cfb24[background - 1];


  if (inverse)
    {
      tmp = fgcmap;
      fgcmap = bgcmap;
      bgcmap = tmp;
    }

  pIterm->charset = charset;
  pIterm->underline = underline;
  pIterm->bold = bold;
  pIterm->blink = blink;
  return;
}

void
fb_true24_draw_char (unsigned char *dst, int offset,
		     unsigned char *bits, int width, int height)
{
  int x, y;
  for (y = 0; y < height; y++)
    {
      unsigned long c1, c2, c3, c4, c, cc;
      c1 = (*bits++ & 0x000000ff);
      c2 = (*bits++ & 0x000000ff);
      c3 = (*bits++ & 0x000000ff);
      c4 = (*bits++ & 0x000000ff);
      c = cc = (c1 << 24) + (c2 << 16) + (c3 << 8) + c4;
      for (x = 0; x < width; x++)
	{
	  unsigned long *dst24;
	  dst24 = (unsigned long *) (dst + offset + 3 * x);

	  if ((pIterm->underline || pIterm->blink) && (height - y < 2))
	    *(dst24) = fgcmap;
	  else
	    {
	      if (c & 0x80000000)
		{
		  if (pIterm->bold || pIterm->blink)
		    *(dst24) = boldcmap;
		  else
		    {
		      if (getenv ("ITERM_BOLD") != NULL)
			*(dst24) = boldcmap;
		      else
			*(dst24) = fgcmap;
		    }
		}
	      else
		{
		  *(dst24) = bgcmap;
		}
	    }
	  c <<= 1;
	}
      dst += pIterm->fb->line_length;
    }
  return;
}

void
fb_true24_draw_text (int col, int row, char *mbstring, int length, int width)
{
  int startx, starty, i, height, codepoint, len;
  int w;
  unsigned char *bits;
  char *dst;
  mbstate_t ps;

  if (pIterm->lock)
    return;

  starty = row * pIterm->fb->line_length * pIterm->cell_height;
  startx = 3 * col * pIterm->asc_font->cell_width;

  switch (pIterm->charset)
    {
    case SPECIAL_GRAPHICS:
      for (i = 0; i < length; i++)
	if (0x5E < mbstring[i] && mbstring[i] < 0x7F)
	  mbstring[i] = ((mbstring[i] == 0x5F) ? 0x7F : mbstring[i] - 0x5F);
      break;
    case UK_CHARSET:
      for (i = 0; i < length; i++)
	if (mbstring[i] == '#')
	  mbstring[i] = 0x1E;	/* pound sign */
    default:
      break;
    }

  for (i = 0;;)
    {
      wchar_t wc;
      int wlen;

      if (i >= length)
	break;

      /* get font glyph data */
      len = get_char_len (&mbstring[i], &ps);
      if (len >= 1)
	{
	  if (len == 1 && mbstring[i] < 0x7f)
	    {
	      codepoint = (int) (mbstring[i] & 0x000000ff);
	      bits = get_glyph (pIterm->asc_font, codepoint, &height);
	      w = pIterm->asc_font->cell_width;
	    }
	  else
	    {
	      mbrtowc (&wc, (char *) &mbstring[i], len, 0);
	      bits = get_glyph (pIterm->mb_font, wc, &height);
	      wlen = wcwidth (wc);
	      w = pIterm->asc_font->cell_width * (wlen > 0 ? wlen : 1);
	    }
	}

      i += len;
      dst = (char *) (pIterm->fb->buf + starty);
      fb_true24_draw_char (dst, startx, bits, w, height);
      startx += (3 * w);
    }
  return;
}
