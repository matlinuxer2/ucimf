/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include "fbiterm.h"

static void
VTScreenView_draw_text (VTScreenView * view, int col, int row,
			char *mbstring, int length, int width)
{
  pIterm->fb->draw_text (col, row, mbstring, length, width);
  return;
}

static void
VTScreenView_clear_rect (VTScreenView * view, int s_col, int s_row,
			 int e_col, int e_row)
{
  pIterm->fb->clear_rect (s_col, s_row, e_col, e_row);
  return;
}

static void
VTScreenView_set_rendition (VTScreenView * view, int bold, int blink,
			    int inverse, int underline,
			    int foreground, int background, char charset)
{
  pIterm->fb->set_rendition (bold, blink, inverse, underline, foreground,
			     background, charset);
  return;
}

static void
VTScreenView_scroll_view (VTScreenView * view, int dest, int src, int num_line)
{
  int desty;
  int srcy;
  int length;

  if(pIterm->lock)
    return;

  if(dest < src)
  {
    desty = dest * pIterm->fb->line_length * pIterm->cell_height;
    srcy =  src * pIterm->fb->line_length * pIterm->cell_height;
    length = num_line * pIterm->fb->line_length * pIterm->cell_height;
    memmove(pIterm->fb->buf + desty, pIterm->fb->buf + srcy, length);
#if 0 /* could be faster than memmove ;-) */
    int i;
    length = pIterm->fb->line_length * pIterm->cell_height;
    for(i=0;i < num_line;i++)
    {
      desty = (dest+i) * pIterm->fb->line_length * pIterm->cell_height;
      srcy =  (src+i) * pIterm->fb->line_length * pIterm->cell_height;
      memcpy(pIterm->fb->buf + desty, pIterm->fb->buf + srcy, length);
      }
#endif
  }
  else if(src < dest) /* basically this code doesn' need
                         but faster than memmove */
  {
    int i;
    length = pIterm->fb->line_length * pIterm->cell_height;
    for(i=num_line-1;0 <= i;i--)
    {
      desty = (dest+i) * pIterm->fb->line_length * pIterm->cell_height;
      srcy =  (src+i) * pIterm->fb->line_length * pIterm->cell_height;
      memcpy(pIterm->fb->buf + desty, pIterm->fb->buf + srcy, length);
    }
  }
  return;
}

VTScreenView *
VTScreenView_new (void)
{
  VTScreenView *view;

  view = malloc (sizeof (VTScreenView));
  if (view == NULL)
    {
      perror ("VTScreenView_new");
      return NULL;
    }

  VTScreenView_init (view);
  view->draw_text = VTScreenView_draw_text;
  view->clear_rect = VTScreenView_clear_rect;
  view->set_rendition = VTScreenView_set_rendition;
  view->scroll_view = VTScreenView_scroll_view;
  return view;
}

void
VTScreenView_destroy (VTScreenView * view)
{
  if (view)
    free (view);
  return;
}
