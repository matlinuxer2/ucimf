/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include "VTP.h"
#include "ItermP.h"
#include "VTScreenView.h"
#include "iterm/screen.h"
#include <stdio.h>
#include <stdlib.h>

#define COL2PIXEL(vtpart,col)  ((col) * vtpart.cell_width_pixel)
#define ROW2PIXEL(vtpart,row)  ((row) * vtpart.cell_height_pixel + vtpart.fontset_ascent)

#define COL2LEFT(vtpart,col)   COL2PIXEL(vtpart,col)
#define ROW2TOP(vtpart,row)    ((row) * vtpart.cell_height_pixel)

/*
  This function supposed to be called from GUI thread,
  by calling VTScreen::redraw
  
*/
static void VTScreenView_draw_text(VTScreenView *view, int col, int row,
                                   char *mbstring, int length,int width)
{
  VTWidget w = (VTWidget)((ItermWidget)view->object)->iterm.vt;
  int x;
  int y;

  x = COL2PIXEL(w->vt,col);
  y = ROW2PIXEL(w->vt,row);

  switch(w->vt.charset)
  {
    int i;
    case SPECIAL_GRAPHICS:
        for(i=0;i<length;i++)
            if(0x5E <mbstring[i] && mbstring[i] < 0x7F)
                mbstring[i] = ((mbstring[i]==0x5F)? 0x7F : mbstring[i]-0x5F);
        break;
    case UK_CHARSET:
        for(i=0;i<length;i++)
            if(mbstring[i] == '#')
                mbstring[i] = 0x1E; /* pound sign */
    default:
        break;
  }

  if(w->vt.bold || w->vt.blink)
  {
    if(w->vt.bold_fontset)
    {
      XmbDrawImageString(XtDisplay((Widget)w),XtWindow((Widget)w),
                         w->vt.bold_fontset,w->vt.current_gc,
                         x,y,mbstring,length);
    }
    else
    { 
      XmbDrawImageString(XtDisplay((Widget)w),XtWindow((Widget)w),
                         w->vt.fontset, w->vt.current_gc,
                         x,y,mbstring,length);
      XmbDrawString(XtDisplay((Widget)w),XtWindow((Widget)w),
                    w->vt.fontset,w->vt.current_gc,
                    x+1,y,mbstring,length);
    }
  }
  else
      XmbDrawImageString(XtDisplay((Widget)w),XtWindow((Widget)w),
                         w->vt.fontset, w->vt.current_gc,
                         x,y,mbstring,length);
  if(w->vt.underline || w->vt.blink)
  {
    XDrawLine(XtDisplay((Widget)w),XtWindow((Widget)w),w->vt.current_gc,
              x,y+1,x+width*w->vt.cell_width_pixel,y+1);
  }
  XFlush(XtDisplay((Widget)w));
}

static void VTScreenView_clear_rect(VTScreenView *view,int s_col,int s_row,
                                    int e_col, int e_row)
{
  VTWidget w = (VTWidget)((ItermWidget)view->object)->iterm.vt;
  int sx,sy,ex,ey;

  sx = COL2LEFT(w->vt,s_col);
  sy = ROW2TOP(w->vt,s_row);
  ex = COL2LEFT(w->vt,e_col);
  ey = ROW2TOP(w->vt,e_row);
  XClearArea(XtDisplay((Widget)w),XtWindow((Widget)w),
             sx,sy,ex-sx,ey-sy,
             FALSE);
}

static void VTScreenView_set_GC(VTWidget w,
                                Pixel foreground, Pixel background)
{
  XGCValues       xgcv;
  xgcv.foreground = foreground;
  xgcv.background = background;
  XChangeGC(XtDisplay((Widget)w),w->vt.normal_gc,
                      GCForeground|GCBackground, &xgcv);
  xgcv.foreground = background;
  xgcv.background = foreground;
  XChangeGC(XtDisplay((Widget)w),w->vt.reverse_gc,
                      GCForeground|GCBackground, &xgcv);
}

static void VTScreenView_set_rendition(VTScreenView *view, int bold,
                                       int blink, int inverse, int underline,
                                       int foreground, int background,
                                       char charset)
{
  VTWidget w = (VTWidget)((ItermWidget)view->object)->iterm.vt;
  Pixel fore_pixel;
  Pixel back_pixel;

  if( 0 < background && background < 9)
      back_pixel = w->vt.textcolor[background-1];
  else
      back_pixel = w->core.background_pixel;

  if( 0 < foreground && foreground < 9)
      fore_pixel = w->vt.textcolor[foreground-1];
  else
      fore_pixel = w->vt.foreground;

      /* When background and foreground is same color
         use substitute color
       */
  if(back_pixel == fore_pixel)
  {
    foreground = foreground ? foreground : foreground + 1;
    fore_pixel = w->vt.textcolor[foreground+8-1];
  }
  
  VTScreenView_set_GC(w,fore_pixel,back_pixel);

  w->vt.bold =  bold ? True : False;
  w->vt.blink =  blink ? True : False;
  w->vt.underline = underline ? True : False;
  if(inverse)
      w->vt.current_gc = w->vt.reverse_gc;
  else
      w->vt.current_gc = w->vt.normal_gc;
  w->vt.charset = charset;
}

static void VTScreenView_swap_video(VTScreenView *view)
{

  ItermWidget w = (ItermWidget)view->object;
      /* swap foreground and background color, then redraw all screen */
  ItermSetReverseVideo((Widget)w,!w->iterm.reverse_video);
/*  XtCallCallbacks((Widget)w,XtNvideoChanged,(XtPointer)w->iterm.reverse_video); */
}

static void VTScreenView_ring(VTScreenView *view)
{
  VTWidget w = (VTWidget)((ItermWidget)view->object)->iterm.vt;
  
  if(w->vt.visual_bell_on)
  {
    XFillRectangle(XtDisplay((Widget)w),
                   XtWindow((Widget)w), 
                   w->vt.reverse_gc,
                   0, 0,
                   (unsigned) w->core.width,
                   (unsigned) w->core.height);
    XFlush(XtDisplay((Widget)view->object));
    XClearArea(XtDisplay((Widget)w),
               XtWindow((Widget)w), 
               0, 0,
               (unsigned)w->core.width,
               (unsigned)w->core.height,
               TRUE);
    XFlush(XtDisplay((Widget)view->object));
  }
  else
  {
    XBell(XtDisplay((Widget)w),0);
  }
}

static void VTScreenView_resize_request(VTScreenView *view,int cols, int rows)
{
  ItermWidget iterm = (ItermWidget)view->object;
  VTWidget vt = (VTWidget)iterm->iterm.vt;
  int width,height;
  void ItermReshape(ItermWidget iterm, Boolean resize_request);
      
  if(vt->vt.status_line)
      rows++;
  width = vt->vt.cell_width_pixel * cols + 2*vt->core.border_width;
  height = vt->vt.cell_height_pixel * rows + 2*vt->core.border_width;
  XtResizeWidget((Widget)vt,
                 (Dimension)width,(Dimension)height,
                 vt->core.border_width);
  ItermReshape(iterm,True); 
}

static void VTScreenView_update_cursor_position(VTScreenView *view,
                                                int cols, int rows)
{
  VTWidget vt = (VTWidget)((ItermWidget)view->object)->iterm.vt;
      /* update cursor potision */
      /* we only need this information when ic is enabled */
  if(vt->vt.ic)
  {
    XVaNestedList preedit_attr;
    XPoint spot;
    
    spot.x = cols * vt->vt.cell_width_pixel;
    spot.y = rows * vt->vt.cell_height_pixel + vt->vt.fontset_ascent;
    preedit_attr = XVaCreateNestedList(0,
                                       XNSpotLocation, &spot,
                                       NULL);
    XSetICValues(vt->vt.ic,
                 XNPreeditAttributes, preedit_attr,
                 NULL);
    XFree(preedit_attr);
  }

  return;
}

static void VTScreenView_notify_osc(VTScreenView *view, int type,
                                    char *string, int length)
{
  VTWidget vt = (VTWidget)((ItermWidget)view->object)->iterm.vt;
  Widget shell;

  shell = XtParent(vt);
  while(!XtIsShell(shell))
      shell = XtParent(shell);
  string[length] = '\0'; /* terminate but ad-hoc */

  switch(type)
  {
    case 0:
        XtVaSetValues(shell,
                      XtNiconName,(XtArgVal)string,
                      XtNtitle,(XtArgVal)string,
                      NULL);
        break;
    case 1:
        XtVaSetValues(shell,
                      XtNiconName,(XtArgVal)string,
                      NULL);
        break;
    case 2:
        XtVaSetValues(shell,
                      XtNtitle,(XtArgVal)string,
                      NULL);
        break;
    default:
        /* do nothing */;
  }
  return;
}
void VTScreenView_update_scrollbar(VTScreenView *view, int max,
                                   int top, int shown)
{
  ItermWidget iterm = (ItermWidget)view->object;
  void set_thumb_values(Widget w, float top, float shown);
  
  set_thumb_values(iterm->iterm.scrollbar,(float)top/max,(float)shown/max);
}

void VTScreenView_scroll_view(VTScreenView *view,
                              int dest_row,int src_row, int num_line)
{
  VTWidget vt = (VTWidget)((ItermWidget)view->object)->iterm.vt;
  int pix = num_line * vt->vt.cell_height_pixel;
  int src = src_row * vt->vt.cell_height_pixel;
  int dest = dest_row * vt->vt.cell_height_pixel;
/*  int height=vt->core.height-vt->core.border_width*2; */

#ifdef DEBUG
  if(pix > height || src > height || dest > height)
  {
    printf("src_row = %d, num_line = %d, dest_row = %d\n",
           src_row,num_line,dest_row);
  }
#endif  
  XCopyArea(XtDisplay((Widget)vt),
            XtWindow((Widget)vt),XtWindow((Widget)vt),
            vt->vt.current_gc,
            0,src,
            vt->core.width,
            pix,
            0,dest);
}

VTScreenView *VTScreenView_new(ItermWidget iterm)
{
  VTScreenView *view;
  if((view = malloc(sizeof(VTScreenView))) == NULL)
  {
    perror("VTScreenView_new");
    return NULL;
  }

  VTScreenView_init(view);
  view->object = iterm;
  view->draw_text = VTScreenView_draw_text;
  view->clear_rect = VTScreenView_clear_rect;
  view->set_rendition = VTScreenView_set_rendition;
  view->swap_video = VTScreenView_swap_video;
  view->resize_request = VTScreenView_resize_request;
  view->update_cursor_position = VTScreenView_update_cursor_position;
  view->ring = VTScreenView_ring;
  view->notify_osc = VTScreenView_notify_osc;
  view->update_scrollbar = VTScreenView_update_scrollbar;
  view->scroll_view = VTScreenView_scroll_view; 
  return view;
}

void VTScreenView_destroy(VTScreenView *view)
{
  if(view)
      free(view);
}
