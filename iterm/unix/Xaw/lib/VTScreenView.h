/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#ifndef __VTSCREEN_VIEW_H__
#define __VTSCREEN_VIEW_H__
#include "iterm/screen.h"
#include "Iterm.h"

VTScreenView *VTScreenView_new(ItermWidget vt);
void VTScreenView_destroy(VTScreenView *view);

#endif
