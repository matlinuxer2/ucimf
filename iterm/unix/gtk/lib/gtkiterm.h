/* This software is subject to the terms of the Common Public License
 * You must accept the terms of this license to use this software.
 *
 * Copyright (C) 2002, International Business Machines Corporation
 * and others.  All Rights Reserved.
 *
 * Further information about Common Public License Version 0.5 is obtained
 * from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */
#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>
#include <iterm/core.h>

#define GTK_ITERM(obj) GTK_CHECK_CAST (obj, gtk_iterm_get_type (), GtkIterm)
#define GTK_ITERM_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gtk_iterm_get_type (), GtkItermClass)
#define GTK_IS_ITERM(obj)       GTK_CHECK_TYPE (obj, gtk_iterm_get_type ())

typedef struct _GtKIterm GtkIterm;
typedef struct _GtkItermClass GtkItermClass;

struct _GtKIterm
{
  GtkWidget widget;
  GdkColor colors[8];
  int foreground;
  int background;
  int underline;
  int bold;
  int inverse;
  PangoLayout *layout;

  gint num_cols;
  gint num_rows;
  gint cell_width_in_pixel;
  gint cell_height_in_pixel;
  gint full_height;
  gint full_width;
  gint ascent;

  VTCore *vtcore;
  TerminalIO *io;
  VTScreenView *view;
};

struct _GtkItermClass
{
  GtkWidgetClass parent_class;
  void (*connection_closed)(GtkIterm *iterm);
};

GtkWidget *gtk_iterm_new();
GtkType gtk_iterm_get_type(void);

