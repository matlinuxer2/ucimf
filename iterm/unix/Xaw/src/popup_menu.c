/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <X11/Intrinsic.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/StringDefs.h>
#include "Iterm.h"

extern Widget iterm;
extern Pixmap menu_item_bitmap;

/* prototypes */
static void font_callback(Widget w, XtPointer p, XtPointer p2);
static void toggle_scrollbar(Widget w, XtPointer p, XtPointer p2);
static void toggle_scrollbar_left(Widget w, XtPointer p, XtPointer p2);
static void toggle_visual_bell(Widget w, XtPointer p, XtPointer p2);
static void toggle_reverse_video(Widget w, XtPointer p, XtPointer p2);

struct MenuEntry
{
  char *name;
  XtCallbackProc proc;
  Widget entry_widget;
};

struct MenuEntry font_menu_entry[] ={
    {"default", font_callback,NULL},
    {"fontSet1", font_callback,NULL},
    {"fontSet2", font_callback,NULL},
    {"fontSet3", font_callback,NULL},
    {"fontSet4", font_callback,NULL},
    {"fontSet5", font_callback,NULL},
    {"fontSet6", font_callback,NULL},
    {NULL,NULL,NULL}};

struct MenuEntry option_menu_entry[] ={
    {"enableScrollbar", toggle_scrollbar,NULL},
    {"enableScrollbarLeft", toggle_scrollbar_left,NULL},
    {"enableVisualBell",toggle_visual_bell,NULL},
    {"enableReverseVideo",toggle_reverse_video,NULL},
    {NULL,NULL,NULL}};

#define SetBitmap(w,bool) \
  XtVaSetValues(w, \
                XtNleftBitmap, \
                bool?menu_item_bitmap:(XtArgVal)0, \
                NULL)
    


static void font_callback(Widget w, XtPointer p, XtPointer p2)
{
  int num = (int)p;
  int i;
  struct MenuEntry *entries = font_menu_entry;
  if(0<=num && num<=6)
      ItermSelectFont(iterm,num);

  for(i=0;entries[i].name != NULL;i++)
  {
    SetBitmap(entries[i].entry_widget,i==num);
  }
}

static void toggle_scrollbar(Widget w, XtPointer p, XtPointer p2)
{
  ItermSetScrollbar(iterm,!ItermIsScrollbarOn(iterm));
  SetBitmap(w,ItermIsScrollbarOn(iterm));
}

static void toggle_scrollbar_left(Widget w, XtPointer p, XtPointer p2)
{
  ItermSetScrollbarLeft(iterm,!ItermIsScrollbarLeft(iterm));
  SetBitmap(w,ItermIsScrollbarLeft(iterm));
}

static void toggle_visual_bell(Widget w, XtPointer p, XtPointer p2)
{
  ItermSetVisualBell(iterm,!ItermIsVisualBell(iterm));
  SetBitmap(w,ItermIsVisualBell(iterm));
}

static void toggle_reverse_video(Widget w, XtPointer p, XtPointer p2)
{
  ItermSetReverseVideo(iterm,!ItermIsReverseVideo(iterm));
  SetBitmap(w,ItermIsReverseVideo(iterm));
}

static Widget create_popup_menu(Widget parent,
                                char *widget_name,
                                struct MenuEntry *entries)
{
  Widget popup;
  XtCallbackRec cb[2] = { { NULL, 0 }, { NULL, NULL }};
  Arg arg[] = {{ XtNcallback, (XtArgVal) cb},};
  int i;
  popup = XtVaCreatePopupShell(widget_name,simpleMenuWidgetClass,parent,
                               NULL);
  for(i=0;entries[i].name != NULL;i++)
  {
    cb[0].callback = entries[i].proc;
    cb[0].closure =(XtPointer)i;
    entries[i].entry_widget = XtCreateManagedWidget(entries[i].name,
                                                     smeBSBObjectClass,
                                                     popup,
                                                     arg,XtNumber(arg));
  }
  return popup;
}

Widget init_font_menu(Widget parent)
{
  Widget menu = create_popup_menu(parent,"fontMenu",font_menu_entry);
  XtVaSetValues(font_menu_entry[0].entry_widget,
                XtNleftBitmap,menu_item_bitmap,
                NULL);
  return menu;
}

Widget init_option_menu(Widget parent)
{
  Widget menu = create_popup_menu(parent,"optionMenu",option_menu_entry);
  XtVaSetValues(option_menu_entry[0].entry_widget,
                XtNleftBitmap,
                ItermIsScrollbarOn(iterm)?menu_item_bitmap:(XtArgVal)0,
                NULL);
  XtVaSetValues(option_menu_entry[1].entry_widget,
                XtNleftBitmap,
                ItermIsScrollbarLeft(iterm)?menu_item_bitmap:(XtArgVal)0,
                NULL);
  XtVaSetValues(option_menu_entry[2].entry_widget,
                XtNleftBitmap,
                ItermIsVisualBell(iterm)?menu_item_bitmap:(XtArgVal)0,
                NULL);
  XtVaSetValues(option_menu_entry[3].entry_widget,
                XtNleftBitmap,
                ItermIsReverseVideo(iterm)?menu_item_bitmap:(XtArgVal)0,
                NULL);
  
  return menu;
}

static void videoChanged(Widget w, XtPointer p1, XtPointer p2)
{
  SetBitmap((Widget)p1,p2);
}

void set_callbacks(Widget iterm)
{
  XtAddCallback(iterm,XtNvideoChanged,videoChanged,
                option_menu_entry[3].entry_widget); 

}
