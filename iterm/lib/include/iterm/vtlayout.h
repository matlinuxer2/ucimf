/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html
*/
#ifndef __VTLAYOUT_H__
#define __VTLAYOUT_H__

#include <stdlib.h>
struct ConcreteVTLayout;

enum {
    VT_RTL = 0x01,
    VT_LTR = 0x02,

    VT_NOMINAL = 0x04,
    VT_NATIONAL = 0x06
};

typedef struct _VTLayout {
  struct ConcreteVTLayout *concrete_layout;
  int (*transform)(struct _VTLayout *vtlayout,
                   char *logical,
                   size_t logical_length,
                   char *visual,
                   size_t *visual_length,
                   size_t *logical2visual_index,
                   size_t *visual2lobical_index,
                   size_t *embedding_level);
  
  void (*set_global_direction)(struct _VTLayout *vtlayout, int direction);
  int (*is_direction_LTR)(struct _VTLayout *vtlayout);
  
  void (*set_numerals)(struct _VTLayout *vtlayout,  int currency);
} VTLayout;

VTLayout *VTLayout_new();
void VTLayout_destroy(VTLayout *vtlayout);

#endif
