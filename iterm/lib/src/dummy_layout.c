/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include "iterm/vtlayout.h"

/* Dummy layout engine, even not instantiate! */
struct ConcreteVTLayout {
  int dummy;
};

VTLayout *VTLayout_new()
{
/*  VTLayout *vtlayout; */

  return NULL;
}

void VTLayout_destroy(VTLayout *vtlayout)
{
      /*
  if(vtlayout)
  {
    if(vtlayout->concrete_layout)
    {
      m_destroy_layout(vtlayout->concrete_layout->layout_obj);
      free(vtlayout->concrete_layout);
    }
    free(vtlayout);
  }
      */
}
