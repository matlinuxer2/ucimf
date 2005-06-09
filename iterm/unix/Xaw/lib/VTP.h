/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/* $TOG: TemplateP.h /main/8 1998/02/06 12:50:46 kaleb $ */

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

#ifndef _VTP_h
#define _VTP_h

#include "VT.h"
#include "iterm/core.h"

/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */
#define XtRVTResource "VTResource"

typedef struct {
    XtPointer extension;
} VTClassPart;

typedef struct _VTClassRec {
    CoreClassPart	core_class;
    VTClassPart	vt_class;
} VTClassRec;

extern VTClassRec vtClassRec;

typedef struct {
  Pixmap pixmap_buffer;
      /* private */
  XIM im;
  XIC ic;
  GC current_gc;
  GC normal_gc;
  GC reverse_gc;
  GC cursor_gc;

  Pixel textcolor[MAX_NUM_COLOR];
  Pixel foreground;
  Pixel cursor_color;

  XFontSet fontset;
  char *fontset_string;
  XFontSet bold_fontset;
  char *bold_fontset_string;

  char *fontsets[NUM_FONTSETS];

  Widget scrollbar;
  Boolean scrollbar_on; 

  Cardinal num_cols;
  Cardinal num_rows;
  Cardinal num_history;
  int cell_width_pixel;
  int cell_height_pixel;
  int full_height;
  int full_width;
  int fontset_ascent;
  VTCore *vtcore;
  TerminalIO *io;
  Boolean underline;
  Boolean bold;
  Boolean blink;
  Boolean status_line;
  Boolean visual_bell_on;
  Boolean login_shell;
  char *input_method;
  char charset;
  char **program;

  XtInputId id;
  XtCallbackList statusLineChanged;
  XPoint ex_cursor_position;
} VTPart;

typedef struct _VTRec {
  CorePart core;
  VTPart vt;
} VTRec;

#endif /* _VTP_h */
