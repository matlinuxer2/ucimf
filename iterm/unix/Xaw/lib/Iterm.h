/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/* $TOG: Iterm.h /main/7 1998/02/06 12:50:56 kaleb $ */

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
/* $XFree86: xc/lib/Xaw/Template.h,v 1.4 1998/10/03 08:42:23 dawes Exp $ */

#ifndef _Iterm_h
#define _Iterm_h

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/

/* define any special resource names here that are not in <X11/StringDefs.h> */
#define XtNbaseSizeChange "baseSizeChange"
#define XtNcellSizeChange "cellSizeChange"

#define XtNscrollBarLeft "scrollBarLeft"
#define XtCScrollBarLeft "ScrollBarLeft"

#define XtNconnectionClosed "connectionClosed"
#define XtCConnectionClosed "ConnectionClosed"

#define XtNvideoChanged "videoChanged"
#define XtCvideoChanged "VideoChanged"

#define XtNcommand "command"
#define XtCCommand "Command"

/* declare specific ItermWidget class and instance datatypes */
typedef struct _ItermClassRec *ItermWidgetClass;
typedef struct _ItermRec *ItermWidget;

/* declare the class constant */
extern WidgetClass itermWidgetClass;

/* utility methods */
int ItermGetBaseWidth(Widget w);
int ItermGetBaseHeight(Widget w);
int ItermGetCellWidth(Widget w);
int ItermGetCellHeight(Widget w);
void ItermSelectFont(Widget w, int font_num);
void ItermSetScrollbar(Widget w, Boolean on);
void ItermSetScrollbarLeft(Widget w, Boolean on);
Boolean ItermIsScrollbarOn(Widget w);
Boolean ItermIsScrollbarLeft(Widget w);
void ItermSetVisualBell(Widget w, Boolean on);
Boolean ItermIsVisualBell(Widget w);
void ItermSetReverseVideo(Widget w, Boolean on);
Boolean ItermIsReverseVideo(Widget w);

#endif /* _Iterm_h */
