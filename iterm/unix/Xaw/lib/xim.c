/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "VTP.h"

#include <stdio.h>
#include "xim.h"


void update_status_line(VTWidget vt, Bool on);

int InitIM(VTWidget w)
{
      /* over the spot only  */
  XIMStyle supported_styles[] = {
      (XIMPreeditPosition | XIMStatusArea),   /* Over the Spot*/
      (XIMPreeditPosition | XIMStatusNothing),   /* Over the Spot*/
      (XIMPreeditNothing | XIMStatusNothing), /* Root */
      (XIMPreeditNothing | XIMStatusNone),    /* AIX Single Byte */
      0};
  XIMStyle *supported_style;
  XIMStyles *im_styles = NULL;
  Boolean found = False;
  XPoint spot;
  XVaNestedList   preedit_attr, status_attr;
  int i;

  if(w->vt.im) /* already opened */
      return 0;
  
  if ((w->vt.im = XOpenIM(XtDisplay((Widget)w),NULL,NULL,NULL)) == NULL)
  {
    fprintf(stderr,"Can't open IM\n");
    w->vt.status_line = False;
    return 1;
  }
  
  if(XGetIMValues(w->vt.im, XNQueryInputStyle, &im_styles, NULL) || !im_styles)
  {
    fprintf(stderr,"input method doesn't support any style\n");
    XCloseIM(w->vt.im);
    return 1;
  }

      /* find suitable IM style */
  if (im_styles != (XIMStyles *) NULL) {
    for (i = 0; i < im_styles->count_styles; i++) {
      for(supported_style = supported_styles;
          *supported_style != 0;
          supported_style++)
      {
        if(*supported_style == im_styles->supported_styles[i])
        {
          found = True;
          goto loop_end;
        }
      }
    }
    loop_end:
    ;
  }
  XFree(im_styles);

  if(!found)
  {
    fprintf(stderr, "non supported input method style\n");
    XCloseIM(w->vt.im);
    return 1;
  }

  spot.x = 0;
  spot.y = 0;
  preedit_attr = XVaCreateNestedList(0,
                                     XNSpotLocation, &spot,
                                     XNFontSet,w->vt.fontset,
                                     NULL);

  status_attr = XVaCreateNestedList(0,
                                    XNFontSet,w->vt.fontset,
                                    NULL);
  w->vt.ic = XCreateIC(w->vt.im,
                       XNInputStyle,*supported_style,
                       XNClientWindow,XtWindow(w),
                       XNFocusWindow,XtWindow(w),
                       XNPreeditAttributes, preedit_attr,
                       XNStatusAttributes, status_attr, 
                       NULL);

  XFree(preedit_attr);
  XFree(status_attr);

  if(w->vt.ic == NULL)
  {
    fprintf(stderr,"can't create IC\n");
    XCloseIM(w->vt.im);
    return 1;
  }

  if (*supported_style & XIMStatusArea)
  {
    w->vt.status_line = True;
    update_status_line(w,w->vt.status_line);
    UpdateIMStatusAttributes(w);
  }
  else
  {
    w->vt.status_line = False;
  }
  
  UpdateIMPreeditAttributes(w);
  return 0;
}



int UpdateIMPreeditAttributes(VTWidget w)
{
  if (XtIsRealized((Widget)w) && w->vt.ic != (XIC)NULL) {
    XVaNestedList preedit_attr;
    XRectangle area;

    area.x = 0;
    area.y = 0;
    area.width = w->vt.full_width;
    area.height = w->vt.full_height;

    preedit_attr = XVaCreateNestedList(0,
                                       XNArea, &area,
                                       XNForeground, w->vt.foreground, 
                                       XNBackground, w->core.background_pixel,
                                       NULL);
    XSetICValues(w->vt.ic,
                 XNPreeditAttributes, preedit_attr,
                 NULL);
    XFree(preedit_attr);
  }
  return 0;
}

int UpdateIMStatusAttributes(VTWidget w)
{
  if (XtIsRealized((Widget)w) && w->vt.ic != (XIC)NULL) {
    XVaNestedList status_attr;
    XRectangle area;

    /* status area */
    area.x = 0;
    area.y = w->vt.full_height + 1;
    area.width = w->vt.full_width;
    area.height = w->vt.cell_height_pixel;
    status_attr = XVaCreateNestedList(0,
                                      XNArea, &area,
                                      XNForeground, w->vt.foreground, 
                                      XNBackground, w->core.background_pixel,
                                      NULL);
    XSetICValues(w->vt.ic,
                 XNStatusAttributes, status_attr,
                 NULL);
    XFree(status_attr);
  }
  return 0;
}
  
int UpdateFontset(VTWidget w)
{
  if (XtIsRealized((Widget)w) && w->vt.ic != (XIC)NULL) {
    XVaNestedList attr;


    attr = XVaCreateNestedList(0,
                               XNFontSet,w->vt.fontset,
                               NULL);
    XSetICValues(w->vt.ic,
                 XNStatusAttributes, attr,
                 XNPreeditAttributes, attr,
                 NULL);
    XFree(attr);
  }
  return 0;
}
  
int DrawStatusLine(VTWidget w)
{
  if (XtIsRealized((Widget)w) && w->vt.ic != (XIC)NULL && w->vt.status_line)
  {
    int sx,sy;
    int ex,ey;
    XGCValues       xgcv;
    Pixel fore;
    Pixel back;
    sx = 0;
    ey = sy = w->vt.full_height;
    ex = w->vt.full_width;

    fore = w->vt.foreground;
    back = w->core.background_pixel;
    xgcv.foreground = w->vt.foreground;
    xgcv.background = w->core.background_pixel;
    XChangeGC(XtDisplay((Widget)w),w->vt.normal_gc,
              GCForeground|GCBackground, &xgcv);

    XDrawLine(XtDisplay(w),XtWindow(w),w->vt.normal_gc,sx,sy,ex,ey);

    xgcv.foreground = fore;
    xgcv.background = back;
    XChangeGC(XtDisplay((Widget)w),w->vt.normal_gc,
              GCForeground|GCBackground, &xgcv);
  }
  return 0;
}

void update_status_line(VTWidget vt, Bool on)
{
  int base_inc;
  int delta_height;

  if(on)
  {
    base_inc = delta_height = vt->vt.cell_height_pixel;
  }
  else
  {
    delta_height = -vt->vt.cell_height_pixel;
    base_inc = 0;
  }
  
      /* finally resize for status line */
  XtMakeResizeRequest((Widget)vt,
                      (Dimension)vt->core.width,
                      (Dimension)vt->core.height+delta_height,
                      NULL,NULL);

      /* if there is a status line, change baseWidth, baseHeight
         and minimum height  */
  XtCallCallbacks((Widget)vt,XtNstatusLineChange,NULL);

  XFlush(XtDisplay((Widget)vt));
}

static void im_destroy(XIM xim, XPointer w, XPointer no_use)
{
  VTWidget vt = (VTWidget)w;
  vt->vt.im = NULL;
  vt->vt.ic = NULL;
  if(vt->vt.status_line)
  {
    vt->vt.status_line = False;
    update_status_line(vt,False);
  }
}

void IMCallback(Display *d, XPointer w, XPointer no_use)
{
  VTWidget vt = (VTWidget)w;
  XIMCallback ximcallback;
  if(InitIM(vt))
  {
    vt->vt.im = NULL;
    vt->vt.ic = NULL;
    return ;
  }
  ximcallback.callback = im_destroy;
  ximcallback.client_data = w;
  XSetIMValues(vt->vt.im, XNDestroyCallback, &ximcallback, NULL);
}
