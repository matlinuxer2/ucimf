/* This software is subject to the terms of the Common Public License
 * You must accept the terms of this license to use this software.
 *
 * Copyright (C) 2002, International Business Machines Corporation
 * and others.  All Rights Reserved.
 *
 * Further information about Common Public License Version 0.5 is obtained
 * from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#include <iterm/core.h>
#include <iterm/unix/ttyio.h>

#include "gtkiterm.h"
#include "VTScreenView.h"

/* local data */
static GtkWidgetClass *parent_class = NULL;

/* signals */
enum {
    CLIENT_EXIT_SIGNAL,
    LAST_SIGNAL
};

#ifdef _AIX
#define DEFAULT_SHELL "/usr/bin/ksh"
#else
#define DEFAULT_SHELL "/bin/sh"
#endif

static gint iterm_signals[LAST_SIGNAL] = {0};

static TerminalIO *init_io(int cols, int rows, int loginShell)
{
  char *defaultShell = DEFAULT_SHELL;
  char *shell;
  char *login_shell;
  char *program = defaultShell;
  char *argv[2];

  argv[0] = program;
  argv[1] = NULL;

  putenv("TERM=xterm");

  shell = getenv("SHELL");
  if(shell != NULL && shell[0] != '\0')
  {
    program = shell;
    argv[0] = shell;
  }
  
  if(loginShell)
  {
    int length = strlen(program) + 1;
    login_shell = malloc(length + 1);
    login_shell[0] = '-';
    memcpy(login_shell+1,program,length);
    argv[0] = login_shell;
  }

  return TtyTerminalIO_new(cols,rows,program,argv);
}

static void dispatch(gpointer data, gint source, GdkInputCondition cond)
{
  GtkIterm *iterm = GTK_ITERM(data);
  VTCore_dispatch(iterm->vtcore);
}

static gint key_press(GtkWidget *widget, GdkEventKey *event)
{
  GtkIterm *iterm = GTK_ITERM(widget);
  GdkEventKey *key_event = (GdkEventKey *)event;
  VTCore_write(iterm->vtcore,key_event->string,key_event->length);
  return TRUE;
}

static void
gtk_iterm_destroy (GtkObject *object)
{
  GtkIterm *iterm;

  g_return_if_fail (object != NULL);
  g_return_if_fail (GTK_IS_ITERM (object));

  iterm = GTK_ITERM (object);

  if(iterm->vtcore)
      VTCore_destroy(iterm->vtcore);
  iterm->vtcore = NULL;
  if(iterm->io)
        TtyTerminalIO_destroy(iterm->io); 
  iterm->io = NULL;
  
  
  if (GTK_OBJECT_CLASS (parent_class)->destroy)
      (* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static void
gtk_iterm_init (GtkIterm *iterm)
{
  GTK_WIDGET_SET_FLAGS (iterm, GTK_CAN_FOCUS);
  iterm->num_cols = 80;
  iterm->num_rows = 24;
  iterm->full_height = 0;
  iterm->full_width = 0;
  iterm->cell_width_in_pixel = 0;
  iterm->cell_height_in_pixel = 0;
}

GtkWidget*
gtk_iterm_new ()
{
  GtkIterm *iterm;

  iterm = gtk_type_new (gtk_iterm_get_type ());

  return GTK_WIDGET (iterm);
}

static void exit_callback(VTScreenView *view)
{
  gtk_signal_emit(GTK_OBJECT(view->object),
                  iterm_signals[CLIENT_EXIT_SIGNAL]);
}

static void
gtk_iterm_realize (GtkWidget *widget)
{
  GtkIterm *iterm;
  GdkWindowAttr attributes;
  gint attributes_mask;
  PangoRectangle logical,ink;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_ITERM (widget));

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
  iterm = GTK_ITERM (widget);

  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = (gtk_widget_get_events (widget) |
                           GDK_EXPOSURE_MASK|GDK_KEY_PRESS_MASK);
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
  widget->window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);
  gdk_window_set_user_data(widget->window,iterm);
  widget->style = gtk_style_attach (widget->style, widget->window);

  gdk_window_set_user_data (widget->window, widget);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);

  iterm->layout = gtk_widget_create_pango_layout(GTK_WIDGET(widget),NULL);
  pango_layout_set_text(iterm->layout,"E",1);
  pango_layout_get_pixel_extents(iterm->layout,&ink,&logical);
  iterm->cell_width_in_pixel = logical.width;
  iterm->cell_height_in_pixel = logical.height;
  iterm->full_width = iterm->cell_width_in_pixel * iterm->num_cols;
  iterm->full_height = iterm->cell_height_in_pixel * iterm->num_rows;
  iterm->ascent = -logical.y;
  
  iterm->io = init_io(iterm->num_cols,iterm->num_rows,0);
  iterm->vtcore = VTCore_new(iterm->io,iterm->num_cols,iterm->num_rows,100);

  gdk_color_parse("black",&iterm->colors[0]);
  gdk_color_parse("red3",&iterm->colors[1]);
  gdk_color_parse("green3",&iterm->colors[2]);
  gdk_color_parse("yellow3",&iterm->colors[3]);
  gdk_color_parse("blue3",&iterm->colors[4]);
  gdk_color_parse("magenta3",&iterm->colors[5]);
  gdk_color_parse("cyan3",&iterm->colors[6]);
  gdk_color_parse("gray90",&iterm->colors[7]);
  iterm->foreground = 0;
  iterm->background = 0;
  
  iterm->view = VTScreenView_new(widget);
  VTCore_set_screen_view(iterm->vtcore,iterm->view);
  VTCore_set_exit_callback(iterm->vtcore,exit_callback);
  gdk_input_add(TtyTerminalIO_get_associated_fd(iterm->io),
                GDK_INPUT_READ,dispatch,widget);
  gtk_widget_grab_focus(widget);
      {
        GtkStyle *style;
        style = gtk_widget_get_style(widget);
        style->bg[GTK_STATE_NORMAL] = style->white;
        gtk_style_set_background(style,widget->window,GTK_STATE_NORMAL);
      }
}

static void 
gtk_iterm_size_request (GtkWidget      *widget,
		       GtkRequisition *requisition)
{
  GtkIterm *iterm = GTK_ITERM(widget);
  requisition->width = iterm->full_width;
  requisition->height = iterm->full_height;
}

static void
gtk_iterm_size_allocate (GtkWidget     *widget,
			GtkAllocation *allocation)
{
  GtkIterm *iterm;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_ITERM (widget));
  g_return_if_fail (allocation != NULL);

  widget->allocation = *allocation;
  if (GTK_WIDGET_REALIZED (widget))
  {
    iterm = GTK_ITERM (widget);
    
    gdk_window_move_resize (widget->window,
                            allocation->x, allocation->y,
                            allocation->width, allocation->height);
  }
}

static gint
gtk_iterm_expose (GtkWidget      *widget,
		 GdkEventExpose *event)
{
  GtkIterm *iterm;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_ITERM (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (event->count > 0)
    return FALSE;
  
  iterm = GTK_ITERM (widget);

  pango_layout_set_single_paragraph_mode (iterm->layout, TRUE);
  VTCore_redraw(iterm->vtcore,0,0,80,24);
  
  return FALSE;
}

static void
gtk_iterm_class_init (GtkItermClass *class)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;

  object_class = (GtkObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;

  parent_class = gtk_type_class (GTK_TYPE_WIDGET);

  object_class->destroy = gtk_iterm_destroy;

  widget_class->realize = gtk_iterm_realize;
  widget_class->expose_event = gtk_iterm_expose;
  widget_class->size_request = gtk_iterm_size_request;
  widget_class->size_allocate = gtk_iterm_size_allocate;
  widget_class->key_press_event = key_press;

  iterm_signals[CLIENT_EXIT_SIGNAL] = gtk_signal_new("connection_closed",
                                                     GTK_RUN_FIRST,
                                                     GTK_CLASS_TYPE(object_class),
                                                     GTK_SIGNAL_OFFSET(GtkItermClass,connection_closed),
                                                     gtk_signal_default_marshaller,
                                                     GTK_TYPE_NONE, 0);
}


GtkType
gtk_iterm_get_type ()
{
  static GtkType iterm_type = 0;

  if (!iterm_type)
  {
    static const GtkTypeInfo iterm_info =
 {
     "GtkIterm",
     sizeof (GtkIterm),
     sizeof (GtkItermClass),
     (GtkClassInitFunc) gtk_iterm_class_init,
     (GtkObjectInitFunc) gtk_iterm_init,
     /* reserved_1 */ NULL,
     /* reserved_1 */ NULL,
     (GtkClassInitFunc) NULL
 };
    
    iterm_type = gtk_type_unique (GTK_TYPE_WIDGET, &iterm_info);
  }
  
  return iterm_type;
}
