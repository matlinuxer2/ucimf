/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#ifndef __XIM_H__
#define __XIM_H__

int UpdateIMPreeditAttributes(VTWidget w);
int UpdateIMStatusAttributes(VTWidget w);
int DrawStatusLine(VTWidget w);
int UpdateFontset(VTWidget w);
void IMCallback(Display *d, XPointer w, XPointer no_use);
int InitIM(VTWidget w);
#endif
