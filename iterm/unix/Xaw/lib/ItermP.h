/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/* $TOG: ItermP.h /main/8 1998/02/06 12:50:46 kaleb $ */

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
/* $XFree86: xc/lib/Xaw/TemplateP.h,v 1.5 1999/06/06 08:48:13 dawes Exp $ */

#ifndef _ItermP_h
#define _ItermP_h

#include "iterm/screen.h"
#include "Iterm.h"
#include "VT.h"

/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */
#define XtRItermResource "ItermResource"

typedef struct {
  XtPointer extension;
} ItermClassPart;

typedef struct _ItermClassRec {
    CoreClassPart	core_class;
    ItermClassPart	iterm_class;
} ItermClassRec;

extern ItermClassRec itermClassRec;

typedef struct {
  Widget vt;
  Widget scrollbar;
  Boolean scrollbar_on;
  Boolean scrollbar_left;
  Boolean reverse_video;
  XtCallbackList baseSizeChanged;
  XtCallbackList cellSizeChanged;
  XtCallbackList connectionClosed;
  XtCallbackList videoChanged;
  VTScreenView *view;
  Cursor cursor;
} ItermPart;

typedef struct _ItermRec {
    CorePart	core;
    ItermPart	iterm;
} ItermRec;

#endif /* _ItermP_h */
