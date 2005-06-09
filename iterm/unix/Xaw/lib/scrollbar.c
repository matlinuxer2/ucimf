/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
/*#include <X11/Shell.h> */

#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/ScrollbarP.h>

#include "VTP.h"


#include <stdio.h>

void set_thumb_values(Widget w, float top, float shown)
{
  if(sizeof(float) > sizeof(XtArgVal))
  {
    XtVaSetValues(w,
                  XtNshown,(XtArgVal)&shown,
                  XtNtopOfThumb,(XtArgVal)&top,
                  NULL);
  }
  else
  {
    XtArgVal *top_val = (XtArgVal *)&top;
    XtArgVal *shown_val = (XtArgVal *)&shown;
    XtVaSetValues(w,
                  XtNshown,*shown_val,
                  XtNtopOfThumb,*top_val,
                  NULL);
  }
}

static void scroll_callback(Widget scrollbarWidget,
                            XtPointer client_data,
                            XtPointer call_data)
{
  VTWidget vt = (VTWidget)client_data;

  if( (int)call_data < 0)
      VTCore_scroll_up(vt->vt.vtcore,vt->vt.num_rows/2);
  else
      VTCore_scroll_down(vt->vt.vtcore,vt->vt.num_rows/2);
}

static void jump_callback(Widget scrollbarWidget,
                            XtPointer client_data,
                            XtPointer call_data)
{
  VTWidget vt = (VTWidget)client_data;
  float *val = (float *)call_data;

  VTCore_scroll(vt->vt.vtcore,*val);
}

Widget InitScrollbar(Widget parent, VTWidget vt)
{
  Widget scrollbar = NULL;
  scrollbar = XtVaCreateWidget("scrollbar",scrollbarWidgetClass,
                               parent,
                               XtNx, 0,
                               XtNy, 0,
                               XtNheight,vt->core.height,
                               XtNbackground,vt->core.background_pixel,
                               XtNorientation,XtorientVertical,
                               XtNborderWidth,1,
                               NULL);
  XtAddCallback(scrollbar,XtNscrollProc,scroll_callback,(XtPointer)vt);
  XtAddCallback(scrollbar,XtNjumpProc,jump_callback,(XtPointer)vt);

  set_thumb_values(scrollbar,0.0,1.0);
  return scrollbar;
}

void ScrollBarSwapVideo(Widget w)
{
  Pixel fore,back;

  XtVaGetValues(w,
                XtNbackground,&back,
                XtNforeground,&fore,
                NULL);
  XtVaSetValues(w,
                XtNbackground,fore,
                XtNforeground,back,
                XtNborderColor,back,
                NULL);
}
