/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/* $TOG: VT.h /main/7 1998/02/06 12:50:56 kaleb $ */

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

#ifndef _VT_h
#define _VT_h

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
/*
#define XtNvtResource "vtResource"

#define XtCVTResource "VTResource"
*/
#define XtNtextColor0  "textColor0"
#define XtNtextColor1  "textColor1"
#define XtNtextColor2  "textColor2"
#define XtNtextColor3  "textColor3"
#define XtNtextColor4  "textColor4"
#define XtNtextColor5  "textColor5"
#define XtNtextColor6  "textColor6"
#define XtNtextColor7  "textColor7"
#define XtNtextColor8  "textColor8"
#define XtNtextColor9  "textColor9"
#define XtNtextColor10 "textColor10"
#define XtNtextColor11 "textColor11"
#define XtNtextColor12 "textColor12"
#define XtNtextColor13 "textColor13"
#define XtNtextColor14 "textColor14"
#define XtNtextColor15 "textColor15"
#define XtCTextColor0  "TextColor0"
#define XtCTextColor1  "TextColor1"
#define XtCTextColor2  "TextColor2"
#define XtCTextColor3  "TextColor3"
#define XtCTextColor4  "TextColor4"
#define XtCTextColor5  "TextColor5"
#define XtCTextColor6  "TextColor6"
#define XtCTextColor7  "TextColor7"
#define XtCTextColor8  "TextColor8"
#define XtCTextColor9  "TextColor9"
#define XtCTextColor10 "TextColor10"
#define XtCTextColor11 "TextColor11"
#define XtCTextColor12 "TextColor12"
#define XtCTextColor13 "TextColor13"
#define XtCTextColor14 "TextColor14"
#define XtCTextColor15 "TextColor15"
#define MAX_NUM_COLOR 16

#define XtNfontSet1 "fontSet1"
#define XtNfontSet2 "fontSet2"
#define XtNfontSet3 "fontSet3"
#define XtNfontSet4 "fontSet4"
#define XtNfontSet5 "fontSet5"
#define XtNfontSet6 "fontSet6"
#define XtCFontSet1 "FontSet1"
#define XtCFontSet2 "FontSet2"
#define XtCFontSet3 "FontSet3"
#define XtCFontSet4 "FontSet4"
#define XtCFontSet5 "FontSet5"
#define XtCFontSet6 "FontSet6"
#define NUM_FONTSETS 6

#define XtNvisualBell "visualBell"
#define XtCVisualBell "VisualBell"

#define XtNcursorColor "cursorColor"

#define XtNboldFontSet "boldFontSet"
#define XtCBoldFontSet "BoldFontSet"

#define XtNscrollBar   "scrollBar"
#define XtCScrollBar   "ScrollBar"

#define XtNstatusLineChange "stateLineChage"

#define XtNloginShell "loginShell"
#define XtCLoginShell "LoginShell"

#define XtCRow "Rows"
#define XtNrow "rows"
#define XtCCol "Cols"
#define XtNcol "cols"
#define XtCSaveLines "SaveLines"
#define XtNsaveLines "saveLines"

#define XtNprogram "program"
#define XtCProgram "Program"

/* declare specific TemplateWidget class and instance datatypes */
typedef struct _VTClassRec *VTWidgetClass;
typedef struct _VTRec *VTWidget;

/* declare the class constant */
extern WidgetClass vtWidgetClass;

/* Utility Methods */
int VTGetCellWidth(Widget w);
int VTGetCellHeight(Widget w);
int VTGetStatusLineHeight(Widget w);
void VTSelectFont(Widget w, int font_num);
void VTSwapVideo(Widget w);
void VTSetConnectionClosedCallback(Widget w, void (*proc)());
void  VTSetVisualBell(Widget w, Boolean on);
Boolean VTIsVisualBell(Widget w);

#endif /* _VT_h */
