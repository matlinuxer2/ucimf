/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#ifndef __ACTIONS_H__
#define __ACTIONS_H__
/*
 * Actions
 */
void HandleKeys(Widget, XEvent*, String*, Cardinal*);
void VTClicked(Widget, XEvent*, String*, Cardinal*);
void HandleString(Widget, XEvent*, String*, Cardinal*);
void HandleInsertSelection(Widget, XEvent*, String*, Cardinal*);
void HandlePopup(Widget, XEvent*, String*, Cardinal*);
void HandleStartSelection(Widget , XEvent *, String*, Cardinal*);
void HandleStartSelection3(Widget , XEvent *, String*, Cardinal*);
void HandleExtendSelection(Widget , XEvent *, String*, Cardinal*);
void HandleEndSelection(Widget , XEvent *, String*, Cardinal*);
void HandleScrollUp(Widget , XEvent *, String*, Cardinal*);
void HandleScrollDown(Widget , XEvent *, String*, Cardinal*);
void HandleButtonDown(Widget w, XEvent *event,
                      String *params, Cardinal *num_params);
void HandleButtonUp(Widget w, XEvent *event,
                    String *params, Cardinal *num_params);

#ifdef BIDI
void HandleSwitchDirection(Widget , XEvent *, String*, Cardinal*);
#endif

#endif
