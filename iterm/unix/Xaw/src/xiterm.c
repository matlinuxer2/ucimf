/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/SimpleMenu.h>

#include "ItermP.h"
#include "VTP.h"
#include "popup_menu.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define menu_item_bitmap_width 8
#define menu_item_bitmap_height 8
static unsigned char menu_item_bitmap_bits[] = {
   0x10, 0x20, 0x40, 0xff, 0xff, 0x40, 0x20, 0x10};
Widget toplevel,iterm;
Pixmap menu_item_bitmap;
Widget font_popup;
Widget option_popup;

static XrmOptionDescRec optionDescList[] = {
    {"-fn", "*VT*fontSet",     XrmoptionSepArg, (caddr_t) NULL},
    {"-fb", "*VT*boldFontSet", XrmoptionSepArg, (caddr_t) NULL},
    {"-bw", "*borderWidth", XrmoptionSepArg, (caddr_t) NULL},
    {"-vb", "*visualBell",  XrmoptionNoArg,  (caddr_t) "on"},
    {"+vb", "*visualBell",  XrmoptionNoArg,  (caddr_t) "off"},
    {"-cr", "*cursorColor",   XrmoptionSepArg, (caddr_t) NULL},
    {"-rv", "*reverseVideo",  XrmoptionNoArg, (caddr_t) "on"},
    {"+rv", "*reverseVideo",  XrmoptionNoArg, (caddr_t) "off"},
    {"-sb", "*scrollBar",  XrmoptionNoArg, (caddr_t) "on"},
    {"+sb", "*scrollBar",  XrmoptionNoArg, (caddr_t) "off"},
    {"-sbl", "*scrollBarLeft",  XrmoptionNoArg, (caddr_t) "on"},
    {"+sbl", "*scrollBarLeft",  XrmoptionNoArg, (caddr_t) "off"},
    {"-ls",  "*VT*loginShell", XrmoptionNoArg, (caddr_t) "on"},
    {"+ls",  "*VT*loginShell", XrmoptionNoArg, (caddr_t) "off"},
    {"-sl",  "*VT*saveLines", XrmoptionSepArg, (caddr_t) "512"},
    {"-inputMethod", "*inputMethod",  XrmoptionSepArg, (caddr_t) NULL},
    {"-e", NULL,  XrmoptionSkipLine, (caddr_t) NULL},
};

static char *fallback_resources[] = {
    "XIterm*SimpleMenu*menuLabel.vertSpace: 100",
    "XIterm*SimpleMenu*HorizontalMargins: 16",
    "XIterm*SimpleMenu*Sme.height: 16",
    "XIterm*SimpleMenu*Cursor: left_ptr",
/*    "XIterm*fontMenu.Label:  Fonts (no app-defaults)",
    "XIterm*fontMenu*default*Label: Default",
    "XIterm*fontMenu*fontSet1*Label: Font 1",
    "XIterm*fontMenu*fontSet2*Label: Font 2",
    "XIterm*fontMenu*fontSet3*Label: Font 3",
    "XIterm*fontMenu*fontSet4*Label: Font 4",
    "XIterm*fontMenu*fontSet5*Label: Font 5",
    "XIterm*fontMenu*fontSet6*Label: Font 6",
    "XIterm*optionMenu.Label:  Options (no app-defaults)",
    "XIterm*optionMenu*enableScrollbar*Label: Enable Scrollbar",
    "XIterm*optionMenu*enableScrollbarLeft*Label: Enable Scrollbar Left", */
    NULL
 };

static Arg shellArgs[] = {{XtNinput,True},
                          {XtNallowShellResize,True},};

static XtAppContext app_con;


static void finalize()
{
  XFreePixmap(XtDisplay((Widget)iterm),menu_item_bitmap);
  XtDestroyWidget(font_popup);
  XtDestroyWidget(option_popup);
  XtDestroyWidget(iterm);
  XtDestroyWidget(toplevel);
  XtAppSetExitFlag(app_con);
}

static void handle_wm_delete_window(Widget w, XEvent *e,
                            String *params, Cardinal *n_params)
{
  finalize();
}

static void baseSizeChanged(Widget w, XtPointer p1, XtPointer p2)
{
  XtVaSetValues((Widget)p1,
                XtNbaseHeight,ItermGetBaseHeight(w),
                XtNbaseWidth,ItermGetBaseWidth(w),
                NULL);
}

static void cellSizeChanged(Widget w, XtPointer p1, XtPointer p2)
{
  XtVaSetValues((Widget)p1,
                XtNheightInc,ItermGetCellHeight(w),
                XtNwidthInc,ItermGetCellWidth(w),
                NULL);
}

static void connectionClosed(Widget w, XtPointer p1, XtPointer p2)
{
  finalize();
}

XtActionsRec actionProcs[] = {
    {"DeleteWindow", handle_wm_delete_window},
};


static String translations = "<ClientMessage>WM_PROTOCOLS: DeleteWindow() \n";

int main(int argc, char *argv[])
{
  Atom wm_delete_window;
  char **command = NULL;

  XtSetLanguageProc(NULL,NULL,NULL);
  XtToolkitThreadInitialize();

  toplevel = XtAppInitialize(&app_con,"XIterm",
                             optionDescList,XtNumber(optionDescList),
                             &argc,argv,fallback_resources,NULL,0);

  XtSetValues(toplevel,shellArgs,XtNumber(shellArgs));   
  XtAppAddActions(app_con,actionProcs,XtNumber(actionProcs));

  for(; argc > 0;argc--, argv++)  
  {
    if(argv[0][0] != '-')
      continue;
    switch(argv[0][1])
    {
      case 'e':
	argc--;
	if(argc <= 0)
	{
	   fprintf(stderr,"bad option: -e needs arguments\n");
	   return 1;
	}
           
	argv++;
	command = argv;
	goto break_loop;
      default:
	/* do nothing */;
    }
  }
break_loop:
  XawSimpleMenuAddGlobalActions(app_con);
/*  XtRegisterGrabAction(HandlePopup, True,
                       (ButtonPressMask|ButtonReleaseMask),
                       GrabModeAsync, GrabModeAsync);*/

  iterm = XtVaCreateManagedWidget("iterm",itermWidgetClass,toplevel,
                                  XtNx,0,
                                  XtNy,0,
                                  XtNcommand,command,
                                  NULL);

  XtSetKeyboardFocus(toplevel,iterm);

  menu_item_bitmap = XCreateBitmapFromData (
      XtDisplay(toplevel),
      RootWindowOfScreen(XtScreen(toplevel)),
      (char *)menu_item_bitmap_bits,
      menu_item_bitmap_width,
      menu_item_bitmap_height);

  /* handle -geometry option */
  if(((ShellWidget)toplevel)->shell.geometry != NULL)
  {
    int x,y,w,h;
    XParseGeometry(((ShellWidget)toplevel)->shell.geometry,&x,&y,&w,&h);
    if(w > 0 && h > 0)
    {	    
      Arg args[2];
      XtResource r[] = {
	    {XtNcol,XtCCol,XtRInt, sizeof(Cardinal),
	     XtOffsetOf(VTPart,num_cols),
	     XtRString,(XtPointer)NULL},
	    {XtNrow,XtCRow,XtRInt, sizeof(Cardinal),
	     XtOffsetOf(VTPart,num_rows),XtRString,(XtPointer)NULL},
       };
      args[0].name = XtNcol;
      args[0].value = w;
      args[1].name = XtNrow;
      args[1].value = h;
      XtGetApplicationResources(((ItermWidget)iterm)->iterm.vt,
		                &((VTWidget)((ItermWidget)iterm)->iterm.vt)->vt,
		                r,2,args,2);
    }
  }

  XtRealizeWidget(toplevel);
  XtVaSetValues(toplevel,
                XtNheightInc,ItermGetCellHeight(iterm),
                XtNwidthInc,ItermGetCellWidth(iterm), 
                XtNbaseHeight,ItermGetBaseHeight(iterm),
                XtNbaseWidth,ItermGetBaseWidth(iterm),
                NULL);

  font_popup = init_font_menu(toplevel);
  option_popup = init_option_menu(toplevel);
  
  XtAddCallback(iterm,XtNbaseSizeChange,baseSizeChanged,toplevel); 
  XtAddCallback(iterm,XtNcellSizeChange,cellSizeChanged,toplevel); 
  XtAddCallback(iterm,XtNconnectionClosed,connectionClosed,toplevel); 

  set_callbacks(iterm);
  wm_delete_window = XInternAtom(XtDisplay(toplevel),
                                 "WM_DELETE_WINDOW",
                                 False);
  XtOverrideTranslations(toplevel,
                         XtParseTranslationTable(translations));
  
  XSetWMProtocols (XtDisplay(toplevel), XtWindow(toplevel),
                   &wm_delete_window, 1);

  XtAppMainLoop(app_con);
  return 0;
}
