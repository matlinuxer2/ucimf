/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <sys/layout.h>
#include <errno.h>

#include "iterm/vtlayout.h"


struct ConcreteVTLayout {
  LayoutObject layout_obj;
};

static void set_layout_values(LayoutObject layout_obj)
{
  LayoutValueRec layout_values[2];
  LayoutTextDescriptorRec layout_desc;
  int index = 0;
  
  layout_values[0].name = TypeOfText|TextShaping|Orientation|Numerals;
  layout_values[0].value = &layout_desc;
  layout_values[1].name = 0;
  layout_values[1].value = 0;

  layout_desc.inp =TEXT_IMPLICIT|TEXT_NOMINAL|ORIENTATION_LTR|NUMERALS_NOMINAL;
  layout_desc.out= TEXT_VISUAL|TEXT_SHAPED|ORIENTATION_LTR|NUMERALS_NOMINAL;
  m_setvalues_layout(layout_obj,layout_values,&index);
}

static int VTLayout_transform(VTLayout *vtlayout,
                              char *logical,
                              size_t logical_length,
                              char *visual,
                              size_t *visual_length,
                              size_t *logical2visual_index,
                              size_t *visual2lobical_index,
                              size_t *embedding_level)
{
  int index = 0;
  return m_transform_layout(vtlayout->concrete_layout->layout_obj,
                            logical,
                            logical_length,
                            visual,
                            visual_length,
                            logical2visual_index,
                            visual2lobical_index,
                            embedding_level,&index);
}


static void VTLayout_set_global_direction(VTLayout *vtlayout,
                                         int direction)
{
  LayoutValueRec layout_values[2];
  LayoutTextDescriptorRec layout_desc;
  int index = 0;

  layout_values[0].name = Orientation;
  layout_values[0].value = &layout_desc;
  layout_values[1].name = 0;
  layout_values[1].value = 0;
  layout_desc.out= ORIENTATION_LTR;
  switch(direction)
  {
    case VT_RTL:
        layout_desc.inp = ORIENTATION_RTL;
        m_setvalues_layout(vtlayout->concrete_layout->layout_obj,
                           layout_values,&index);
        break;
    case VT_LTR:
        layout_desc.inp = ORIENTATION_LTR;
        m_setvalues_layout(vtlayout->concrete_layout->layout_obj,
                           layout_values,&index);
        break;
    default:
        /* do nothing */;
  }
}

static int VTLayout_is_direction_LTR(VTLayout *vtlayout)
{
  LayoutValueRec layout_values[2];
  LayoutTextDescriptor layout_desc;
  int index = 0;
  int result;

  layout_desc = (LayoutTextDescriptor)malloc(sizeof(LayoutTextDescriptorRec));
  layout_values[0].name = Orientation;
  layout_values[0].value = &layout_desc;
  layout_values[1].name = 0;
  layout_values[1].value = 0;
  layout_desc->inp = 0;
  layout_desc->out = 0;

  m_getvalues_layout(vtlayout->concrete_layout->layout_obj,
                     layout_values,&index);
  result = (layout_desc->inp & ORIENTATION_LTR);
  free(layout_desc);
  return result;
}

static void VTLayout_set_numerals(VTLayout *vtlayout, int type)
{
  LayoutValueRec layout_values[2];
  LayoutTextDescriptorRec layout_desc;
  int index = 0;

  layout_values[0].name = Numerals;
  layout_values[0].value = &layout_desc;
  layout_values[1].name = 0;
  layout_values[1].value = 0;

  layout_desc.inp =NUMERALS_NOMINAL;

  switch(type)
  {
    case VT_NOMINAL:
        layout_desc.out= NUMERALS_NOMINAL;
        m_setvalues_layout(vtlayout->concrete_layout->layout_obj,
                           layout_values,&index);
        break;
    case VT_NATIONAL:
        layout_desc.out= NUMERALS_NATIONAL;
        m_setvalues_layout(vtlayout->concrete_layout->layout_obj,
                           layout_values,&index);
        break;
    default:
        /* do nothing */;
  }
}

VTLayout *VTLayout_new()
{
  VTLayout *vtlayout;
  LayoutObject layout_obj;

  if((vtlayout = (VTLayout *)malloc(sizeof(VTLayout))) == NULL)
      return NULL;

  if((vtlayout->concrete_layout =
      (struct ConcreteVTLayout *)malloc(sizeof(struct ConcreteVTLayout)))
     == NULL)
  {
    free(vtlayout);
    return NULL;
  }

  errno = 0;
  layout_obj = m_create_layout(NULL,NULL);
  if(layout_obj == NULL || errno )
  {
    if(layout_obj)
        m_destroy_layout(layout_obj);
    free(vtlayout->concrete_layout);
    free(vtlayout);
    return NULL;
  }
  set_layout_values(layout_obj);
  vtlayout->concrete_layout->layout_obj = layout_obj;
  vtlayout->transform = VTLayout_transform;
  vtlayout->set_global_direction = VTLayout_set_global_direction;
  vtlayout->is_direction_LTR = VTLayout_is_direction_LTR;
  vtlayout->set_numerals = VTLayout_set_numerals;
  return vtlayout;
}

void VTLayout_destroy(VTLayout *vtlayout)
{
  if(vtlayout)
  {
    if(vtlayout->concrete_layout)
    {
      m_destroy_layout(vtlayout->concrete_layout->layout_obj);
      free(vtlayout->concrete_layout);
    }
    free(vtlayout);
  }
}
