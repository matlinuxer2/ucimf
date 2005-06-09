/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include "fbiterm.h"

static unsigned char fgcmap = 7;
static unsigned char bgcmap = 0;

void
fb_true8_set_palette ()
{
  return;
}

void
fb_true8_clear_rect (int s_col, int s_row, int e_col, int e_row)
{
  int sx, sy, ex, ey, i, len, c;

  if (pIterm->lock)
    return;

  sx = s_col * pIterm->cell_width;
  sy = s_row * pIterm->cell_height;
  ex = e_col * pIterm->cell_width;
  ey = e_row * pIterm->cell_height;
  len = ex - sx;

  c = 0x00;
  for (i = sy; i < ey; i++)
    memset ((pIterm->fb->buf + sx + (i * pIterm->fb->line_length)), c, len);
  return;
}

void
fb_true8_set_rendition (int bold, int blink, int inverse, int underline,
			int foreground, int background, char charset)
{
  unsigned char tmp;

  if (pIterm->lock)
    return;

  if (foreground == 0)
    fgcmap = 7;			/* white */
  else
    {
      fgcmap = foreground - 1;
      if (fgcmap > 0 && fgcmap < 7)
	fgcmap = (fgcmap == 0x02
		  || fgcmap ==
		  0x05) ? fgcmap : (fgcmap & 0x02) | (~fgcmap & 0x05);
    }

  if (background == 0)
    bgcmap = 0;			/* black */
  else
    {
      bgcmap = background - 1;
      if (bgcmap > 0 && bgcmap < 7)
	bgcmap = (bgcmap == 0x02
		  || bgcmap ==
		  0x05) ? bgcmap : (bgcmap & 0x02) | (~bgcmap & 0x05);
    }

  if (inverse)
    {
      tmp = fgcmap;
      fgcmap = bgcmap;
      bgcmap = tmp;
    }

  pIterm->charset = charset;
  pIterm->underline = underline;
  pIterm->bold = bold;
  return;
}

void
fb_true8_draw_char (unsigned char *dst, int offset,
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
	  unsigned char *dst8;
	  dst8 = (unsigned char *) (dst + offset + x);

	  if (pIterm->underline && (height - y < 2))
	    *(dst8) = fgcmap;
	  else
	    {
	      if (c & 0x80000000)
		{
		  if (pIterm->bold)
		    *(dst8) = (char) fgcmap | 0x08;
		  else
		    {
		      if (getenv ("ITERM_BOLD") != NULL)
			*(dst8) = (char) fgcmap | 0x08;
		      else
			*(dst8) = (char) fgcmap;
		    }
		}
	      else
		*(dst8) = (char) bgcmap;
	    }
	  c <<= 1;
	}
      dst += pIterm->fb->line_length;
    }
  return;
}

void
fb_true8_draw_text (int col, int row, char *mbstring, int length, int width)
{
  int startx, starty, i, height, codepoint, len;
  int w;
  unsigned char *bits;
  char *dst;
  mbstate_t ps;

  if (pIterm->lock)
    return;

  starty = row * pIterm->fb->line_length * pIterm->cell_height;
  startx = col * pIterm->asc_font->cell_width;

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
      fb_true8_draw_char (dst, startx, bits, w, height);
      startx += w;
    }
  return;
}
