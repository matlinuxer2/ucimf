/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include "VTScreenView.h"
#include "iterm/screen.h"
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <pango/pango.h>
#include "gtkiterm.h"

#define COL2PIXEL(iterm,col)  ((col) * iterm->cell_width_in_pixel)
#define ROW2PIXEL(iterm,row)  ((row) * iterm->cell_height_in_pixel + iterm->ascent)

#define COL2LEFT(iterm,col)   COL2PIXEL(iterm,col)
#define ROW2TOP(iterm,row)    ((row) * iterm->cell_height_in_pixel)

static void VTScreenView_draw_text(VTScreenView *view, int col, int row,
                                   char *mbstring, int length,int width)
{
  GtkWidget *widget;
  GtkIterm *iterm;
  GdkColor *foreground = NULL;
  GdkColor *background = NULL;
  PangoAttrList *attrs;
  PangoAttribute *attr;
  gchar logical_string[1024];
  int x;
  int y;

  if(!view)
      return;
  widget = (GtkWidget *)view->object;
  if(!widget)
      return;
  iterm  = GTK_ITERM(widget);
  x = COL2PIXEL(iterm,col);
  y = ROW2PIXEL(iterm,row);

  
  VTCore_copy_buffer(iterm->vtcore,col,row,col+width,row,logical_string,
                         1024);

  if(logical_string[0] == '\0') /* This case, vtcore tries to draw cursor */
      pango_layout_set_text(iterm->layout,mbstring,length);
  else
      pango_layout_set_text(iterm->layout,
                            logical_string,strlen(logical_string));
  
  foreground = &widget->style->fg[widget->state];
  background = &widget->style->bg[widget->state];
  if(iterm->foreground >= 0)
  {
    foreground = &iterm->colors[iterm->foreground];
  }
  if(iterm->background >= 0)
  {
    background = &iterm->colors[iterm->background];
  }

  if(iterm->inverse)
  {
    GdkColor *t;
    t = foreground;
    foreground = background;
    background = t;
  }
  attrs = pango_attr_list_new();
  if(iterm->underline)
    attr = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
  else
    attr = pango_attr_underline_new(PANGO_UNDERLINE_NONE);
  pango_attr_list_insert(attrs,attr);
  if(iterm->bold)
    attr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
  else
    attr = pango_attr_weight_new(PANGO_WEIGHT_NORMAL);
  pango_attr_list_insert(attrs,attr);
  
  pango_layout_set_attributes(iterm->layout,attrs);

  gdk_draw_layout_with_colors(widget->window,
                              widget->style->text_gc[widget->state],
                              x,y,iterm->layout,foreground,background);

  
/*  
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
                mbstring[i] = 0x1E;
    default:
        break;
  }
*/
}

static void VTScreenView_clear_rect(VTScreenView *view,int s_col,int s_row,
                                    int e_col, int e_row)
{
  GtkWidget *widget = (GtkWidget *)view->object;
  GtkIterm *iterm = GTK_ITERM(widget);
  int sx,sy,ex,ey;

  sx = COL2LEFT(iterm,s_col);
  sy = ROW2TOP(iterm,s_row);
  ex = COL2LEFT(iterm,e_col);
  ey = ROW2TOP(iterm,e_row);
  gdk_window_clear_area (widget->window,
			 sx, sy,
                         ex-sx,ey-sy);
}

static void VTScreenView_set_rendition(VTScreenView *view, int bold,
                                       int blink, int inverse, int underline,
                                       int foreground, int background,
                                       char charset)
{
  GtkWidget *widget = (GtkWidget *)view->object;
  GtkIterm *iterm = GTK_ITERM(widget);
  if(foreground)
  {
    iterm->foreground = foreground - 1;
  }
  else
  {
    iterm->foreground = - 1;
  }
  if(background)
  {
    iterm->background = background - 1;
  }
  else
  {
    iterm->background = - 1;
  }

  iterm->inverse = inverse;
  iterm->underline = underline;
  iterm->bold = bold;
}

static void VTScreenView_swap_video(VTScreenView *view)
{
  GtkWidget *widget = (GtkWidget *)view->object;
  GtkIterm *iterm = GTK_ITERM(widget);
  GdkColor tmp;

  tmp = widget->style->fg[widget->state];
  widget->style->fg[widget->state] = widget->style->bg[widget->state];
  widget->style->bg[widget->state] = tmp;

  gtk_style_set_background(gtk_widget_get_style(widget),
                           widget->window,widget->state);
  VTCore_redraw(iterm->vtcore,0,0,iterm->num_cols,iterm->num_rows);
}


static void VTScreenView_ring(VTScreenView *view)
{
  gdk_beep();
}

static void VTScreenView_resize_request(VTScreenView *view,int cols, int rows)
{
}

static void VTScreenView_update_cursor_position(VTScreenView *view,
                                                int cols, int rows)
{
}

static void VTScreenView_notify_osc(VTScreenView *view, int type,
                                    char *string, int length)
{
  GtkWidget *widget = (GtkWidget *)view->object;
  widget = gtk_widget_get_toplevel(widget);

  switch(type)
  {
    case 0:
        gdk_window_set_title(widget->window,string);
        gdk_window_set_icon_name(widget->window,string);
        break;
    case 1:
        gdk_window_set_icon_name(widget->window,string);
        break;
    case 2:
        gdk_window_set_title(widget->window,string);
        break;
    default:
        /* do nothing */;
  }
}

static void VTScreenView_update_scrollbar(VTScreenView *view, int max,
                                   int top, int shown)
{
}

static void VTScreenView_scroll_view(VTScreenView *view,
                                     int dest_row,int src_row, int num_line)
{
}

VTScreenView *VTScreenView_new(GtkWidget *widget)
{
  VTScreenView *view;
  if((view = malloc(sizeof(VTScreenView))) == NULL)
  {
    perror("VTScreenView_new");
    return NULL;
  }

  VTScreenView_init(view);
  view->object = widget;
  view->draw_text = VTScreenView_draw_text;
  view->clear_rect = VTScreenView_clear_rect;
  view->set_rendition = VTScreenView_set_rendition;
  view->notify_osc = VTScreenView_notify_osc;
  view->ring = VTScreenView_ring;
  view->swap_video = VTScreenView_swap_video;
      /*
  view->resize_request = VTScreenView_resize_request;
  view->update_cursor_position = VTScreenView_update_cursor_position;
  view->update_scrollbar = VTScreenView_update_scrollbar;
  view->scroll_view = VTScreenView_scroll_view;
      */
  return view;
}

void VTScreenView_destroy(VTScreenView *view)
{
  if(view)
      free(view);
}
