/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/* $TOG: Iterm.c /main/6 1998/02/06 12:50:51 kaleb $ */

/*

Copyright 1987, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

*/
/* $XFree86: xc/lib/Xaw/Template.c,v 1.6 1999/06/06 08:48:13 dawes Exp $ */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/cursorfont.h>
#include "ItermP.h"
#include "VTP.h"

#include "scrollbar.h"
#include "VTScreenView.h"

#include <stdlib.h>


/*
 * Class Methods
 */
static void ItermInitialize(Widget, Widget, ArgList, Cardinal*);
static void ItermDestroy(Widget w);
static void ItermRealize(Widget,XtValueMask*,XSetWindowAttributes *);
static void ItermResize(Widget w);

/*
 * Actions
 */
static void ItermAction(Widget, XEvent*, String*, Cardinal*);

/*
 * CallBacks 
 */
static void statusLineChanged(Widget w, XtPointer p1, XtPointer p2);

/*
 * Initialization
 */
#define offset(field) XtOffsetOf(ItermRec, iterm.field)
static XtResource resources[] = {
/*{
    name,
    class,
    type,
    size,
    offset,
    default_type,
    default_addr
  },*/
    {XtNscrollBar,XtCScrollBar, XtRBoolean,sizeof(Boolean),
     offset(scrollbar_on),XtRString, "false"},
    {XtNscrollBarLeft,XtCScrollBarLeft, XtRBoolean,sizeof(Boolean),
     offset(scrollbar_left),XtRString, "false"},
    {XtNbaseSizeChange,XtCCallback, XtRCallback,sizeof(XtCallbackList),
     offset(baseSizeChanged),XtRCallback, NULL},
    {XtNcellSizeChange,XtCCallback, XtRCallback,sizeof(XtCallbackList),
     offset(cellSizeChanged),XtRCallback, NULL},
    {XtNconnectionClosed,XtCCallback, XtRCallback,sizeof(XtCallbackList),
     offset(connectionClosed),XtRCallback, NULL},
    {XtNvideoChanged,XtCCallback, XtRCallback,sizeof(XtCallbackList),
     offset(videoChanged),XtRCallback, NULL},
    {XtNreverseVideo,XtCReverseVideo,XtRBoolean, sizeof(Boolean),
     offset(reverse_video),XtRString, (XtPointer) "false"},
};
#undef offset

static XtActionsRec actions[] =
{
    /*{name,		procedure},*/
    {"iterm",	ItermAction},
};

static char translations[] =
"<Btn3Down>:"	"iterm()\n"
;

#define Superclass	(&widgetClassRec)
ItermClassRec itermClassRec = {
  /* core */
  {
    (WidgetClass)Superclass,		/* superclass */
    "Iterm",				/* class_name */
    sizeof(ItermRec),		/* widget_size */
    NULL,				/* class_initialize */
    NULL,				/* class_part_initialize */
    False,				/* class_inited */
    ItermInitialize,			/* initialize */
    NULL,				/* initialize_hook */
    ItermRealize,			/* realize */
    actions,				/* actions */
    XtNumber(actions),			/* num_actions */
    resources,				/* resources */
    XtNumber(resources),		/* num_resources */
    NULLQUARK,				/* xrm_class */
    True,				/* compress_motion */
    True,				/* compress_exposure */
    True,				/* compress_enterleave */
    False,				/* visible_interest */
    ItermDestroy,			/* destroy */
    ItermResize,			/* resize */
    NULL,				/* expose */
    NULL,				/* set_values */
    NULL,				/* set_values_hook */
    XtInheritSetValuesAlmost,		/* set_values_almost */
    NULL,				/* get_values_hook */
    NULL,				/* accept_focus */
    XtVersion,				/* version */
    NULL,				/* callback_private */
    translations,			/* tm_table */
    XtInheritQueryGeometry,		/* query_geometry */
    XtInheritDisplayAccelerator,	/* display_accelerator */
    NULL,				/* extension */
  },
  /* iterm */
  {
    NULL,				/* extension */
  }
};

WidgetClass itermWidgetClass = (WidgetClass)&itermClassRec;


static void connection_closed(VTScreenView *view)
{
  ItermWidget iterm = (ItermWidget)view->object;
  XtCallCallbacks((Widget)iterm,XtNconnectionClosed,NULL);
}

static void ItermSetVideo(ItermWidget iterm, Boolean reverse)
{
  XColor colors[2];
  if(reverse)
  {
    colors[0].pixel = ((VTWidget)iterm->iterm.vt)->core.background_pixel;
    colors[1].pixel = ((VTWidget)iterm->iterm.vt)->vt.foreground;
  }
  else
  {
    colors[0].pixel = ((VTWidget)iterm->iterm.vt)->vt.foreground;
    colors[1].pixel = ((VTWidget)iterm->iterm.vt)->core.background_pixel;
  }
  XQueryColors(XtDisplay(iterm),
               DefaultColormap(XtDisplay(iterm),
                               DefaultScreen(XtDisplay(iterm))),
               colors,2);
  XRecolorCursor(XtDisplay(iterm),iterm->iterm.cursor,colors,colors+1);
}

void ItermReshape(ItermWidget iterm, Boolean resize_request)
{
  int height;
  int vt_width;
  int all_width,all_height;
  int vt_x, scrollbar_x;

  if( iterm->iterm.scrollbar_on )
      XtMapWidget(iterm->iterm.scrollbar);
  else if(!iterm->iterm.scrollbar_on )
      XtUnmapWidget(iterm->iterm.scrollbar);
  XtCallCallbacks((Widget)iterm,XtNbaseSizeChange,NULL);
  XtCallCallbacks((Widget)iterm,XtNcellSizeChange,NULL);
      
  vt_width=iterm->iterm.vt->core.width;
  if(!iterm->iterm.scrollbar_on)
  {
    vt_x = scrollbar_x = 0;
    all_width = vt_width + iterm->core.border_width*2;
  }
  else
  {
    if(iterm->iterm.scrollbar_left) 
    {
      scrollbar_x = -1;
      vt_x = iterm->iterm.scrollbar->core.width+
  iterm->iterm.scrollbar->core.border_width*2;
    }
    else
    {
      vt_x = 0;
      scrollbar_x = iterm->iterm.vt->core.width +
  iterm->iterm.vt->core.border_width*2;
    }
    all_width=vt_width + iterm->iterm.scrollbar->core.width + iterm->iterm.scrollbar->core.border_width*2; 
  }
  height=iterm->iterm.vt->core.height;
  all_height = height + iterm->core.border_width*2;

  XtConfigureWidget(iterm->iterm.scrollbar,
                    (Position)scrollbar_x,(Position)-1,
                    iterm->iterm.scrollbar->core.width,height,
                    iterm->iterm.scrollbar->core.border_width);
  
  XtConfigureWidget(iterm->iterm.vt,
                    vt_x,0,
                    vt_width,height,
                    iterm->iterm.vt->core.border_width);
  if(resize_request)
      XtMakeResizeRequest((Widget)iterm,all_width,all_height, NULL,NULL);
}

/*
 * Implementation
 */
/*
 * Function:
 *	ItermInitialize
 *
 * Parameters:
 *	request - requested widget
 *	w	- the widget
 *	args	- arguments
 *	num_args - number of arguments
 *
 * Description:
 *	Initializes widget instance.
 */
/*ARGSUSED*/
static void
ItermInitialize(Widget request, Widget w, ArgList args, Cardinal *num_args)
{
  ItermWidget new = (ItermWidget)w;
  ItermWidget req = (ItermWidget)request;
  static char **program = NULL;

  new->iterm.scrollbar_on =  new->iterm.scrollbar_on;
  new->iterm.scrollbar_left = new->iterm.scrollbar_left;
      /* dummy size */
  new->core.width = 10;
  new->core.height = 10;
  new->core.border_width = req->core.border_width;
  {
    int i;
    for(i=0;i<*num_args;i++)
    {
      if(strcmp(args[i].name,XtNcommand) == 0 || 
         strcmp(args[i].name,XtCCommand) == 0 )
      {
	program = (char **)args[i].value;
      }
      
    }
  }
  new->iterm.vt = XtVaCreateWidget("vt",vtWidgetClass,w,
                                   XtNx,0,
                                   XtNy,0,
				   XtNprogram,program,
                                   NULL);
}

static void ItermRealize(Widget w ,XtValueMask *value_mask,
                         XSetWindowAttributes *attributes)
{
  ItermWidget new = (ItermWidget)w;
  XtCreateWindow(w,InputOutput,CopyFromParent, *value_mask,attributes);

  XtRealizeWidget(new->iterm.vt);
  XtAddCallback(new->iterm.vt,XtNstatusLineChange,statusLineChanged,w);
  VTSetConnectionClosedCallback(new->iterm.vt,connection_closed);
  
  new->iterm.scrollbar = InitScrollbar(w,(VTWidget)new->iterm.vt);
  XtRealizeWidget(new->iterm.scrollbar);
  
  XtMapWidget(new->iterm.vt);

  new->iterm.view = VTScreenView_new(new);
  VTCore_set_screen_view(((VTWidget)new->iterm.vt)->vt.vtcore,new->iterm.view);

  
  ItermReshape(new,True);
  new->iterm.cursor = XCreateFontCursor(XtDisplay(w),XC_xterm);
  XDefineCursor(XtDisplay(w),XtWindow(w),new->iterm.cursor);

  if(new->iterm.reverse_video)
  {
    ItermSetVideo(new,False);
    VTCore_set_reverse_video(((VTWidget)new->iterm.vt)->vt.vtcore,1);
  }

  XtSetKeyboardFocus(w,new->iterm.vt);
}

static void ItermResize(Widget w)
{
  ItermWidget iterm = (ItermWidget)w;
  int vt_width;

  if(!XtIsRealized(w))
      return;
  
  vt_width = iterm->core.width-iterm->core.border_width*2;
  vt_width = iterm->iterm.scrollbar_on ?
 vt_width-iterm->iterm.scrollbar->core.width:vt_width;
  if(XtIsRealized(iterm->iterm.scrollbar))
  {
    XtResizeWidget(iterm->iterm.scrollbar,
                   iterm->iterm.scrollbar->core.width,
                   w->core.height-w->core.border_width*2,
                   iterm->iterm.scrollbar->core.border_width);
  }
  if(XtIsRealized(iterm->iterm.vt))
  {
    XtResizeWidget(iterm->iterm.vt,
                   vt_width-w->core.border_width*2,
                   w->core.height-w->core.border_width*2,
                   iterm->iterm.vt->core.border_width);
  }
  ItermReshape(iterm,False);
}

static void ItermDestroy(Widget w)
{
  ItermWidget iterm = (ItermWidget)w;

  XtDestroyWidget(iterm->iterm.scrollbar);
  XtDestroyWidget(iterm->iterm.vt);
  free(iterm->iterm.view);
}
/*
 * Function:
 *	ItermAction
 *
 * Parameters:
 *	w	   - iterm widget
 *	event	   - event that caused this action
 *	params	   - parameters
 *	num_params - number of parameters
 *
 * Description:
 *	This function does nothing.
 */
/*ARGSUSED*/
static void
ItermAction(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  printf("w = %p\n",w);
}

/* callback to call callbacks ;-) */
static void statusLineChanged(Widget w, XtPointer p1, XtPointer p2)
{
  ItermWidget iterm = (ItermWidget)p1;
  ItermReshape(iterm,True);
  XtCallCallbacks((Widget)iterm,XtNbaseSizeChange,NULL);
}


/* utility methods */
int ItermGetBaseWidth(Widget w)
{
  ItermWidget iterm = (ItermWidget)w;
  int border = iterm->iterm.vt->core.border_width*2  +
 iterm->core.border_width*2 ;
  return iterm->iterm.scrollbar_on ? iterm->iterm.scrollbar->core.width +
 iterm->iterm.scrollbar->core.border_width*2 + border
 : border;
}

int ItermGetBaseHeight(Widget w)
{
  ItermWidget iterm = (ItermWidget)w;

  return VTGetStatusLineHeight(iterm->iterm.vt)+
 iterm->iterm.vt->core.border_width*2 +
 iterm->core.border_width*2; 
}

int ItermGetCellWidth(Widget w)
{
  ItermWidget iterm = (ItermWidget)w;
  return VTGetCellWidth(iterm->iterm.vt);
}

int ItermGetCellHeight(Widget w)
{
  ItermWidget iterm = (ItermWidget)w;
  return VTGetCellHeight(iterm->iterm.vt);
}

void ItermSelectFont(Widget w, int font_num)
{
  ItermWidget iterm = (ItermWidget)w;
  VTSelectFont(iterm->iterm.vt,font_num);
  ItermReshape(iterm,True);
}

void ItermSetScrollbar(Widget w, Boolean on)
{
  ItermWidget iterm = (ItermWidget)w;
  iterm->iterm.scrollbar_on = on;
  ItermReshape(iterm,True);
}

void ItermSetScrollbarLeft(Widget w, Boolean on)
{
  ItermWidget iterm = (ItermWidget)w;
  iterm->iterm.scrollbar_left = on;
  ItermReshape(iterm,True);
}

Boolean ItermIsScrollbarOn(Widget w)
{
  ItermWidget iterm = (ItermWidget)w;
  return iterm->iterm.scrollbar_on;
}

Boolean ItermIsScrollbarLeft(Widget w)
{
  ItermWidget iterm = (ItermWidget)w;
  return iterm->iterm.scrollbar_left;
}

void ItermSetVisualBell(Widget w, Boolean on)
{
  ItermWidget iterm = (ItermWidget)w;
  VTSetVisualBell(iterm->iterm.vt,on);
}

Boolean ItermIsVisualBell(Widget w)
{
  ItermWidget iterm = (ItermWidget)w;
  return VTIsVisualBell(iterm->iterm.vt);
}

void ItermSetReverseVideo(Widget w, Boolean on)
{
  ItermWidget iterm = (ItermWidget)w;
  if (iterm->iterm.reverse_video != on)
  {
    ItermSetVideo(iterm,True);
    iterm->iterm.reverse_video = on;
    VTSwapVideo(iterm->iterm.vt);
    ScrollBarSwapVideo(iterm->iterm.scrollbar);
    VTCore_set_reverse_video(((VTWidget)iterm->iterm.vt)->vt.vtcore,on);
  }
}

Boolean ItermIsReverseVideo(Widget w)
{
  ItermWidget iterm = (ItermWidget)w;
  return iterm->iterm.reverse_video;
}
