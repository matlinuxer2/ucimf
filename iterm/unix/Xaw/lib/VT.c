/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/* $TOG: Template.c /main/6 1998/02/06 12:50:51 kaleb $ */

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
#include <X11/Xaw/XawImP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/keysym.h>

#include "VTP.h"
#include "VTScreenView.h"
#include "xim.h"
#include "actions.h"
#include "scrollbar.h"

#include <stdio.h>
#include <stdlib.h>

#include <iterm/unix/ttyio.h>
#include <time.h>


#define DEFAULT_FONTSET "-misc-fixed-medium-r-normal--14-*,*"

#define DEFAULT_FONTSET_1 "-misc-fixed-medium-r-normal--10-*,*"
#define DEFAULT_FONTSET_2 "-misc-fixed-medium-r-normal--12-*,*"
#define DEFAULT_FONTSET_3 "-misc-fixed-medium-r-normal--14-*,*"
#define DEFAULT_FONTSET_4 "-misc-fixed-medium-r-normal--16-*,*"
#define DEFAULT_FONTSET_5 "-misc-fixed-medium-r-normal--20-*,*"
#define DEFAULT_FONTSET_6 "-misc-fixed-medium-r-normal--24-*,*"

/*
 * Class Methods
 */
static void VTInitialize(Widget, Widget, ArgList, Cardinal*);
static void VTRealize(Widget,XtValueMask*,XSetWindowAttributes *);
static void VTDestroy(Widget w);
static void VTExpose(Widget w, XEvent *event, Region region);
static void VTResize(Widget w);

/*
 * Event Handler
 */
static void HandleFocusChange(Widget w,XtPointer client_data, XEvent *event,
  Boolean *continue_dispatch);
static void HandleNonMaskableEvent(Widget w,XtPointer client_data,
                                   XEvent *event, Boolean *continue_dispatch);
/*
 * Initialization
 */
#define offset(field) XtOffsetOf(VTRec, vt.field)
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
/*  {XtNvtResource, XtCVTResource, XtRVTResource, sizeof(char*),
    offset(resource), XtRString, (XtPointer)"default" }, */
  {XtNfontSet,XtCFontSet, XtRString,sizeof(char *),
   offset(fontset_string),XtRString, DEFAULT_FONTSET },
  {XtNboldFontSet,XtCBoldFontSet, XtRString,sizeof(char *),
   offset(bold_fontset_string),XtRString, NULL },
  {XtNfontSet1, XtCFontSet1, XtRString, sizeof(char *),
   offset(fontsets[0]),XtRString, DEFAULT_FONTSET_1},
  {XtNfontSet2, XtCFontSet2, XtRString, sizeof(char *),
   offset(fontsets[1]),XtRString, DEFAULT_FONTSET_2},
  {XtNfontSet3, XtCFontSet3, XtRString, sizeof(char *),
   offset(fontsets[2]),XtRString, DEFAULT_FONTSET_3},
  {XtNfontSet4, XtCFontSet4, XtRString, sizeof(char *),
   offset(fontsets[3]),XtRString, DEFAULT_FONTSET_4},
  {XtNfontSet5, XtCFontSet5, XtRString, sizeof(char *),
   offset(fontsets[4]),XtRString, DEFAULT_FONTSET_5},
  {XtNfontSet6, XtCFontSet6, XtRString, sizeof(char *),
   offset(fontsets[5]),XtRString, DEFAULT_FONTSET_6},
  {XtNtextColor0, XtCTextColor0, XtRPixel, sizeof(Pixel),
   offset(textcolor[0]),XtRString, "black"},
  {XtNtextColor1, XtCTextColor1, XtRPixel, sizeof(Pixel),
   offset(textcolor[1]),XtRString, "red3"},
  {XtNtextColor2, XtCTextColor2, XtRPixel, sizeof(Pixel),
   offset(textcolor[2]),XtRString, "green3"},
  {XtNtextColor3, XtCTextColor3, XtRPixel, sizeof(Pixel),
   offset(textcolor[3]),XtRString, "yellow3"},
  {XtNtextColor4, XtCTextColor4, XtRPixel, sizeof(Pixel),
   offset(textcolor[4]),XtRString, "blue3"},
  {XtNtextColor5, XtCTextColor5, XtRPixel, sizeof(Pixel),
   offset(textcolor[5]),XtRString, "magenta3"},
  {XtNtextColor6, XtCTextColor6, XtRPixel, sizeof(Pixel),
   offset(textcolor[6]),XtRString, "cyan3"},
  {XtNtextColor7, XtCTextColor7, XtRPixel, sizeof(Pixel),
   offset(textcolor[7]),XtRString, "gray90"},
  {XtNtextColor8, XtCTextColor8, XtRPixel, sizeof(Pixel), 
  offset(textcolor[8]),XtRString, "gray30"},
  {XtNtextColor9, XtCTextColor9, XtRPixel, sizeof(Pixel),
   offset(textcolor[9]),XtRString, "red"},
  {XtNtextColor10, XtCTextColor10, XtRPixel, sizeof(Pixel),
   offset(textcolor[10]),XtRString, "green"},
  {XtNtextColor11, XtCTextColor11, XtRPixel, sizeof(Pixel),
   offset(textcolor[11]),XtRString, "yellow"},
  {XtNtextColor12, XtCTextColor12, XtRPixel, sizeof(Pixel),
   offset(textcolor[12]),XtRString, "blue"},
  {XtNtextColor13, XtCTextColor13, XtRPixel, sizeof(Pixel),
   offset(textcolor[13]),XtRString, "magenta"},
  {XtNtextColor14, XtCTextColor14, XtRPixel, sizeof(Pixel),
   offset(textcolor[14]),XtRString, "cyan"},
  {XtNtextColor15, XtCTextColor15, XtRPixel, sizeof(Pixel),
   offset(textcolor[15]),XtRString, "white"},
  {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
   offset(foreground),XtRString,  "XtDefaultForeground"},
  {XtNcursorColor, XtCForeground, XtRPixel, sizeof(Pixel),
   offset(cursor_color),XtRString,  "XtDefaultForeground"},
  {XtNvisualBell, XtCVisualBell, XtRBoolean, sizeof(Boolean),
   offset(visual_bell_on), XtRString, (XtPointer) "false"},
  {XtNinputMethod,XtCInputMethod, XtRString,sizeof(char *),
   offset(input_method),XtRString, (XtPointer)NULL },
  {XtNscrollBar,XtCScrollBar, XtRBoolean,sizeof(Boolean),
   offset(scrollbar_on),XtRString, "true"},
  {XtNstatusLineChange,XtCCallback, XtRCallback,sizeof(XtCallbackList),
   offset(statusLineChanged),XtRCallback, NULL},
  {XtNrow, XtCRow, XtRInt, sizeof(Cardinal),
   offset(num_rows),XtRString,(XtPointer)"24"},
  {XtNcol, XtCCol, XtRInt, sizeof(Cardinal),
   offset(num_cols),XtRString,(XtPointer)"80"},
  {XtNsaveLines, XtCSaveLines, XtRInt, sizeof(Cardinal),
   offset(num_history),XtRString,(XtPointer)"500"},
  {XtNloginShell, XtCLoginShell, XtRBoolean, sizeof(Boolean),
   offset(login_shell), XtRString, (XtPointer) "false"},
  {XtNprogram, XtCProgram, XtRStringArray, sizeof(char **),
   offset(program), XtRStringArray, (XtPointer) NULL},
};
#undef offset

static char translations[] =
"Shift <KeyPress> Prior:" "scroll-up(1,halfpage) \n"
"Shift <KeyPress> Next:" "scroll-down(1,halfpage) \n"
"Shift <Btn4Down>,<Btn4Up>:" "scroll-up(1,line) \n"
"Shift <Btn5Down>,<Btn5Up>:" "scroll-down(1,line) \n"
"Ctrl <Btn4Down>,<Btn4Up>:" "scroll-up(1,page) \n"
"Ctrl <Btn5Down>,<Btn5Up>:" "scroll-down(1,page) \n"
#ifdef BIDI
"Alt <Key>Return:" "switch-direction() \n"
#endif
"<Btn4Down>,<Btn4Up>:" "scroll-up(1,halfpage) \n"
"<Btn5Down>,<Btn5Up>:" "scroll-down(1,halfpage) \n"
"<Key>Home:" "string(\033OH) \n "
"<Key>End:" "string(\033OF) \n "
"<Key>BackSpace:" "string(\177) \n "
"~Ctrl ~Meta <Btn2Up>:" "insert-selection() \n "
"<Btn1Motion>:" "extend-selection() \n"
"<Btn1Up>:" "end-selection() \n"
"~Ctrl ~Meta <Btn3Down>:" "extend-selection() \n"
"~Meta <Btn3Motion>:" "extend-selection()\n"
"<Btn3Up>:" "end-selection() \n"
"<Btn1Down>:" "start-selection() \n"
"<BtnDown>:" "button-down() \n"
"<BtnUp>:" "button-up() \n"
"<KeyPress>:"	"insert-keys() ";


static XtActionsRec actions[] =
{
    {"scroll-up", HandleScrollUp},
    {"scroll-down", HandleScrollDown},
    {"insert-keys",	HandleKeys},
    {"string",  HandleString},
    {"insert-selection",HandleInsertSelection},
    {"start-selection",HandleStartSelection},
    {"extend-selection",HandleExtendSelection},
    {"end-selection",HandleEndSelection},
    {"button-down",HandleButtonDown},
    {"button-up",HandleButtonUp},
#ifdef BIDI
    {"switch-direction",HandleSwitchDirection},
#endif    
/*    {"clicked",   VTClicked},
      {"popup",  HandlePopup}, */
};

#define Superclass	(&widgetClassRec)
VTClassRec vtClassRec = {
  /* core */
  {
    (WidgetClass)Superclass,		/* superclass */
    "VT",				/* class_name */
    sizeof(VTRec),		        /* widget_size */
    NULL,				/* class_initialize */
    NULL,				/* class_part_initialize */
    False,				/* class_inited */
    VTInitialize,			/* initialize */
    NULL,				/* initialize_hook */
    VTRealize,			        /* realize */ 
    actions,				/* actions */
    XtNumber(actions),			/* num_actions */
    resources,				/* resources */
    XtNumber(resources),		/* num_resources */
    NULLQUARK,				/* xrm_class */
    True,				/* compress_motion */
    False,				/* compress_exposure */
    True,				/* compress_enterleave */
    False,				/* visible_interest */
    VTDestroy,				/* destroy */
    VTResize,				/* resize */
    VTExpose,				/* expose */
    NULL,				/* set_values */
    NULL,				/* set_values_hook */
    XtInheritSetValuesAlmost,		/* set_values_almost */
    NULL,				/* get_values_hook */
    NULL,			        /* accept_focus */
    XtVersion,				/* version */
    NULL,				/* callback_private */
    translations,			/* tm_table */
    XtInheritQueryGeometry,		/* query_geometry */
    XtInheritDisplayAccelerator,	/* display_accelerator */
    NULL,				/* extension */
  },
  /* vt */
  {
    NULL,				/* extension */
  }
};

WidgetClass vtWidgetClass = (WidgetClass)&vtClassRec;

/* utility functions */
static void host_thread_dispatcher(XtPointer p, int *source, XtInputId *id)
{
  VTCore_dispatch((VTCore *)p);
}

static void reverse_video(VTWidget vt)
{
  Pixel tmp;
  XGCValues       xgcv;
  tmp = vt->vt.foreground;
  vt->vt.foreground = vt->core.background_pixel;
  vt->core.background_pixel = tmp;
 
  xgcv.foreground = vt->vt.foreground;
  xgcv.background = vt->core.background_pixel;
  XChangeGC(XtDisplay((Widget)vt),vt->vt.normal_gc,
                      GCForeground|GCBackground, &xgcv);

  xgcv.foreground = vt->core.background_pixel;
  xgcv.background = vt->vt.foreground;
  XChangeGC(XtDisplay((Widget)vt),vt->vt.reverse_gc,
                      GCForeground|GCBackground, &xgcv);
  XSetWindowBackground(XtDisplay((Widget)vt),
                       XtWindow((Widget)vt),
                       vt->core.background_pixel);
  
      /* swap foreground and background color, then redraw all screen */
  UpdateIMPreeditAttributes(vt);
  if(vt->vt.status_line)
      UpdateIMStatusAttributes(vt);
  XClearArea(XtDisplay((Widget)vt),XtWindow((Widget)vt),
             0,0,vt->core.width,vt->core.height,TRUE);
  XtVaSetValues((Widget)vt,XtNborderColor,vt->core.background_pixel,NULL);
  XFlush(XtDisplay((Widget)vt));
}

static TerminalIO *init_io(int cols, int rows, char **programs,
	                   Boolean loginShell)
{
  char *defaultShell = "/bin/sh";
  char *shell;
  char *login_shell;
  char *program = defaultShell;
  char *argv[2];

  putenv("TERM=xterm");
  if(programs && programs[0] != NULL)
  {
    return TtyTerminalIO_new(cols,rows,programs[0],programs);
  }
  else
  {
    argv[0] = program;
    argv[1] = NULL;

    shell = getenv("SHELL");
    if(shell != NULL && shell[0] != '\0')
    {
      program = shell;
      argv[0] = shell;
    }

    if(loginShell)
    {
      int length = strlen(program) + 1;
      login_shell = (char *)malloc(length + 1);
      login_shell[0] = '-';
      memcpy(login_shell+1,program,length);
      argv[0] = login_shell;
    }
  }
  return TtyTerminalIO_new(cols,rows,program,argv);
}

static int fontset_column_width_in_pixel(XFontSet fontset)
{
    XFontStruct **font_struct = NULL;
    char **font_name = NULL;
    int number_of_fonts = 0;
    int minimum = 0;
    int i;

    number_of_fonts = XFontsOfFontSet(fontset,&font_struct,&font_name);

    minimum = font_struct[0]->max_bounds.width;

    if(number_of_fonts == 1)
    {
          /*
            ad-hoc hack for bi-width fonts
            asume that min_bounds.width represents single column char width
            and man_bounds.width represents double columns char width
           */
      int min = font_struct[0]->min_bounds.width;
      if(min == (minimum/2))
          minimum = min;
    }
    else
    {
      for(i=1;i<number_of_fonts;i++)
      {
        if(font_struct[i]->max_bounds.width <  minimum)
            minimum = font_struct[i]->max_bounds.width;
      }
    }
    return minimum;
}

static int fontset_load(Display *d,char *fontset_string, XFontSet *fontset)
{
  XFontSet set = NULL;

  char **missingfonts;
  int count,i;
  char *defonts;
  
  if(!strlen(fontset_string))
      return 1;

  set = XCreateFontSet(d,fontset_string, &missingfonts,&count,&defonts);

  if(set)
  {
    if(*fontset)
        XFreeFontSet(d,*fontset);
    *fontset = XCreateFontSet(d,fontset_string,
                              &missingfonts,&count,&defonts);
    XFreeFontSet(d,set);
  }
  else
    return 1;
  
  for(i=0;i<count;i++)
  {
      fprintf(stderr,"missing: %s for Window\n",&*missingfonts[i]);
  }
  return 0;
}

static int load_fontset(VTWidget w, char *normal_font, char *bold_font)
{
  XFontSetExtents *extents;

  if(fontset_load(XtDisplay(w),normal_font,&w->vt.fontset))
  {
    fprintf(stderr,"can't load fontset: %s\n",normal_font);
    w->vt.fontset_string = DEFAULT_FONTSET;
    fprintf(stderr,"switch to open default fontset: %s\n",DEFAULT_FONTSET);
    if(fontset_load(XtDisplay(w),w->vt.fontset_string,&w->vt.fontset))
    {
      fprintf(stderr,"can't load fontset: %s\n",normal_font);
      fprintf(stderr,"aborting\n");
      exit(1);
    }
  }
  else
  {
    if(bold_font)
        fontset_load(XtDisplay(w),bold_font,&w->vt.bold_fontset);
  }

  extents = XExtentsOfFontSet(w->vt.fontset);

  w->vt.cell_height_pixel= extents->max_logical_extent.height;
  w->vt.fontset_ascent = -extents->max_logical_extent.y;
  w->vt.cell_width_pixel = fontset_column_width_in_pixel(w->vt.fontset);
  
  return 0;
}

static void VTReshape(VTWidget w)
{
  w->vt.full_width =  w->vt.cell_width_pixel * w->vt.num_cols;
  w->vt.full_height = w->vt.cell_height_pixel * w->vt.num_rows;

  XtMakeResizeRequest((Widget)w,
                      w->vt.full_width + w->core.border_width*2,
                      w->vt.full_height + w->core.border_width*2+
                      VTGetStatusLineHeight((Widget)w),
                      NULL,NULL);
  UpdateIMPreeditAttributes(w);
  if(w->vt.status_line)
      UpdateIMStatusAttributes(w);
}
/*
 * Implementation
 */
/*
 * Function:
 *	TemplateInitialize
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
VTInitialize(Widget request, Widget w, ArgList args, Cardinal *num_args)
{
  XGCValues gcvalues;
  XtGCMask valueMask;
  VTWidget new = (VTWidget)w;
  VTWidget req = (VTWidget)request;

  new->vt.fontset = NULL;
  new->vt.cell_width_pixel = 0;
  new->vt.cell_height_pixel = 0;
  new->vt.fontset_ascent = 0;

  new->vt.pixmap_buffer = 0;
  new->vt.current_gc = NULL;
  new->vt.normal_gc = NULL;
  new->vt.reverse_gc = NULL;

  memmove(new->vt.textcolor,req->vt.textcolor,sizeof(Pixel)*MAX_NUM_COLOR);
  new->vt.foreground = req->vt.foreground;
  new->vt.fontset_string = req->vt.fontset_string ;
  new->vt.bold_fontset_string = req->vt.bold_fontset_string ;
  new->vt.bold_fontset = NULL;
  new->vt.fontset = NULL;
  load_fontset(new,new->vt.fontset_string,new->vt.bold_fontset_string);

      /* dummy size */
  new->core.width = 10 ;
  new->core.height = 10 ;

  valueMask = GCForeground | GCBackground;
  gcvalues.background = new->core.background_pixel;
  gcvalues.foreground = new->vt.foreground;
  new->vt.current_gc = new->vt.normal_gc = XtGetGC(w,valueMask,&gcvalues);

  gcvalues.foreground = new->vt.cursor_color;
  new->vt.cursor_gc = XtGetGC(w,valueMask,&gcvalues);
  
  
  gcvalues.background = new->vt.foreground;
  gcvalues.foreground = new->core.background_pixel;
  new->vt.reverse_gc = XtGetGC(w,valueMask,&gcvalues);

  new->vt.scrollbar = NULL;
  
  new->vt.im = NULL;
  new->vt.ic = NULL;
  new->vt.ex_cursor_position.x = -1;
  new->vt.ex_cursor_position.y = -1;
}

/* Realize function */
static void VTRealize(Widget w,
                      XtValueMask *value_mask,
                      XSetWindowAttributes *attributes)
{
  VTWidget new = (VTWidget)w;
  char *app_name;
  char *app_class;

  XtCreateWindow(w,InputOutput,CopyFromParent, *value_mask,attributes);

/*  XSetWindowBorderWidth(XtDisplay(w),XtWindow(w),new->vt.border_width); */
  XtVaSetValues(w,XtNborderColor,new->core.background_pixel,NULL);
  if(new->vt.input_method)
  {
    char modifier[256];
    strcpy(modifier,"@im=");
    strcat(modifier,new->vt.input_method);
    XSetLocaleModifiers(modifier);
  }
  else
      XSetLocaleModifiers(NULL);

      /* try to initialize IM */
  new->vt.im = NULL;
  IMCallback(XtDisplay(w),(XtPointer)new,NULL);
      /*
        Now We register callback for IM initialization
        even already opned IM, for re-run IM server 
      */
  XtGetApplicationNameAndClass(XtDisplay(w),&app_name,&app_class);
  XRegisterIMInstantiateCallback(
    XtDisplay(w),NULL,app_name,app_class,IMCallback,(XPointer)new); 

      { /*
          set window id, I need to extend to store DISPLAY as well
          this is just quick hack
         */
        char string[32];
        sprintf(string,"WINDOWID=%ld",XtWindow(new));
        putenv(string);
      }
  new->vt.io = init_io(new->vt.num_cols,new->vt.num_rows,
		       new->vt.program,new->vt.login_shell);
  new->vt.vtcore = VTCore_new(new->vt.io,
                              new->vt.num_cols,new->vt.num_rows,
			      new->vt.num_history);


      /* may need to add another event handler to change
         cursor, mouse pointer and so on so on..
  XtAddEventHandler(w, EnterWindowMask, FALSE,
                    HandleFocusChange, (Opaque)NULL);
  XtAddEventHandler(w, LeaveWindowMask, FALSE,
                    HandleFocusChange, (Opaque)NULL);
  XtAddEventHandler((Widget)new, PropertyChangeMask, FALSE,
                    PropertyChangeHandler, (Opaque)NULL);
      */
      /* Need for SetICFocus */
  XtAddEventHandler(XtParent(w), FocusChangeMask, FALSE,
                    HandleFocusChange, w);
  XtAddEventHandler(w, 0L, True,
                    HandleNonMaskableEvent, (Opaque)NULL);
  new->vt.id = XtAppAddInput(XtWidgetToApplicationContext((Widget)new),
                             TtyTerminalIO_get_associated_fd(new->vt.io),
                             (XtPointer)XtInputReadMask,host_thread_dispatcher,
                             new->vt.vtcore);
  
  VTReshape(new);
}

/* Destroy function  */
static void VTDestroy(Widget w)
{
  VTWidget vt = (VTWidget)w;

  XtRemoveInput(vt->vt.id);
  XFreeGC(XtDisplay(w),vt->vt.normal_gc);
  XFreeGC(XtDisplay(w),vt->vt.reverse_gc);
/*  if(vt->vt.cursor_gc)
    XFreeGC(XtDisplay(w),vt->vt.cursor_gc);  */
  XFreeFontSet(XtDisplay(w),vt->vt.fontset);
  if(vt->vt.bold_fontset)
      XFreeFontSet(XtDisplay(w),vt->vt.bold_fontset);
  VTCore_destroy(vt->vt.vtcore);
  TtyTerminalIO_destroy(vt->vt.io);
}
/* Exposure function */
static void VTExpose(Widget w, XEvent *event, Region region)
{
  VTWidget new = (VTWidget)w;
  if(event->type == Expose || event->type == GraphicsExpose)
  {
    XExposeEvent *ev = (XExposeEvent *)event;
    int scol,srow;
    int ecol,erow;

        /* find top left cell's col */
    scol = ev->x / new->vt.cell_width_pixel;
        /* find bottom right cell's col */
    ecol = ((ev->x + ev->width) / new->vt.cell_width_pixel) + 1;
        /* find top left cell's row */
    srow = ev->y / new->vt.cell_height_pixel;
        /* find bottom right cell's row */
    erow = ((ev->y + ev->height) / new->vt.cell_height_pixel) + 1;

    VTCore_redraw(new->vt.vtcore,scol,srow,ecol,erow);
  }
  DrawStatusLine(new);
}

static void ScreenResized(VTWidget vt, int width, int height)
{
  int n_cols = width / vt->vt.cell_width_pixel;
  int n_rows = height / vt->vt.cell_height_pixel;
  if(vt->vt.status_line)
      n_rows--;
  vt->vt.num_cols = n_cols;
  vt->vt.num_rows = n_rows;
  vt->vt.full_width =  n_cols * vt->vt.cell_width_pixel;
  vt->vt.full_height = n_rows * vt->vt.cell_height_pixel;

  UpdateIMPreeditAttributes(vt);
  if(vt->vt.status_line)
      UpdateIMStatusAttributes(vt);
  DrawStatusLine(vt);
  VTCore_set_screen_size(vt->vt.vtcore,n_cols,n_rows);
}
 

/* Resize function */
static void VTResize(Widget widget)
{
  VTWidget w = (VTWidget)widget;
  ScreenResized(w,
                w->core.width-w->core.border_width*2,
                w->core.height-w->core.border_width*2);
}

/* Event Handler */

static
void HandleFocusChange(Widget w,XtPointer client_data, XEvent *event,
                       Boolean *continue_dispatch)
{
  VTWidget vt = (VTWidget)client_data;

  if(event->type == FocusIn)
  {
    if (vt->vt.ic)
        XSetICFocus(vt->vt.ic); 

    *continue_dispatch = False;
  }
  else if (event->type == FocusOut)
  {
    if (vt->vt.ic)
        XUnsetICFocus(vt->vt.ic);
    *continue_dispatch = False;
  }
  else
  {
      /* printf("event->type == %d\n",event->type); */
  }
}

static void HandleNonMaskableEvent(Widget w,XtPointer client_data,
                                   XEvent *event, Boolean *continue_dispatch)
{
  switch(event->type)
  {
    case GraphicsExpose:
        VTExpose(w,event,(Region)NULL);
        break;
    default:
        /* do nothing */;
  }
}

/* Utility Methods */
int VTGetCellWidth(Widget w)
{
  VTWidget vt = (VTWidget)w;
  return vt->vt.cell_width_pixel;
}

int VTGetCellHeight(Widget w)
{
  VTWidget vt = (VTWidget)w;
  return vt->vt.cell_height_pixel;
}

int VTGetStatusLineHeight(Widget w)
{
  VTWidget vt = (VTWidget)w;
  return vt->vt.status_line ? vt->vt.cell_height_pixel : 0;
}

void VTSelectFont(Widget w, int font_num)
{
  VTWidget vt = (VTWidget)w;

  if(font_num < 1 || NUM_FONTSETS < font_num )
      load_fontset(vt,vt->vt.fontset_string,vt->vt.bold_fontset_string);
  else
      load_fontset(vt,vt->vt.fontsets[font_num-1],NULL);
  vt->vt.ex_cursor_position.x = -1;
  vt->vt.ex_cursor_position.y = -1;
  UpdateFontset(vt);
  VTReshape(vt);
}

void VTSwapVideo(Widget w)
{
  reverse_video((VTWidget)w);
}

void VTSetConnectionClosedCallback(Widget w, void (*proc)())
{
  VTWidget vt = (VTWidget)w;
  VTCore_set_exit_callback(vt->vt.vtcore, proc);
}

void  VTSetVisualBell(Widget w, Boolean on)
{
  VTWidget vt = (VTWidget)w;
  vt->vt.visual_bell_on = on;
}

Boolean VTIsVisualBell(Widget w)
{
  VTWidget vt = (VTWidget)w;
  return vt->vt.visual_bell_on;
}

