/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <fribidi/fribidi.h>
#include <wchar.h>
#include <stdlib.h>
#include <errno.h>

#include "iterm/vtlayout.h"

struct ConcreteVTLayout {
  int is_LTR;
  int is_nonimal;
  int is_shaped;
};


static int mbstoFriBidichars(FriBidiChar *fribidi_string,
                             int *fribidi_length,
                             const char *string)
{
      /* We asume that wchar_t is UCS-4, this sould be somehow removed */
  wchar_t *wstr = (wchar_t *)fribidi_string;
  size_t ret = 0;
  mbstate_t ps;
  int i;

  memset(&ps,0,sizeof(ps));
  errno = 0;
  ret = mbsrtowcs(wstr, &string, *fribidi_length, &ps);
  
  if( ret == (size_t)(-1) )
  {
    free(wstr);
    return 1;
  }
      /* on IA64 FriBidiChar is 8 bytes */
  if(4 < sizeof(FriBidiChar))
  {
    for(i=ret-1;0<=i;i--)
    {
      fribidi_string[i] = wstr[i];
    }
  }
  fribidi_string[ret] = 0;
  *fribidi_length = ret;

  return 0;
}

static int FriBidiCharstombs(char *out, size_t *out_len,
                             FriBidiChar *fribidi_string)
{
      /* We asume that wchar_t is UCS-4, this sould be somehow removed */
  wchar_t *wstr = (wchar_t *)fribidi_string;
  size_t ret;
  mbstate_t ps;
  int i;

      /* on IA64 FriBidiChar is 8 bytes */
  if(4 < sizeof(FriBidiChar))
  {
    for(i=0;fribidi_string[i];i++)
    {
      wstr[i] = fribidi_string[i];
    }
  }

  memset(&ps,0,sizeof(ps));
  errno = 0;
  ret = wcsrtombs(out,(const wchar_t **)&wstr,*out_len,&ps);

  if( ret == (size_t)(-1) || errno )
    return 1;

  *out_len = ret;
  return 0;
}

static int VTLayout_transform(VTLayout *vtlayout,
                              char *logical,
                              size_t logical_length,
                              char *visual,
                              size_t *visual_length,
                              size_t *logical2visual_index,
                              size_t *visual2logical_index,
                              size_t *embedding_level)
{
  int array_size;
  FriBidiChar *fribidi_logical = NULL;
  int fribidi_logical_length;
  FriBidiChar *fribidi_visual = NULL;
  int fribidi_visual_length;
  FriBidiStrIndex *l2v_index = NULL;
  FriBidiStrIndex *v2l_index = NULL;
  FriBidiLevel *embedding_level_list = NULL;
  FriBidiCharType fribidi_type;
  int i;

#define FREE_ALL() \
do { \
    if(fribidi_logical) \
      free(fribidi_logical); \
  if(fribidi_visual) \
      free(fribidi_visual); \
  if(l2v_index) \
      free(l2v_index); \
  if(v2l_index) \
      free(v2l_index); \
  if(embedding_level_list) \
      free(embedding_level_list); \
} while(0)
  
  array_size = fribidi_logical_length =
 fribidi_visual_length = logical_length+1;
  
  if( (fribidi_logical = (FriBidiChar *)
       calloc(array_size,sizeof(FriBidiChar))) == NULL)
      return 1;

  if( (fribidi_visual = (FriBidiChar *)
       calloc(array_size,sizeof(FriBidiChar))) == NULL)
      goto FAIL;

  if(logical2visual_index)
  {
    if( (l2v_index = (FriBidiStrIndex *)
         calloc(array_size,sizeof(FriBidiStrIndex))) == NULL)
        goto FAIL;
  }
      
  if(visual2logical_index)
  {
    if( (v2l_index = (FriBidiStrIndex *)
         calloc(array_size,sizeof(FriBidiStrIndex))) == NULL)
        goto FAIL;
  }

  if(embedding_level)
  {
    if( (embedding_level_list = (FriBidiLevel *)
         calloc(array_size,sizeof(FriBidiLevel))) == NULL)
        goto FAIL;
  }

  logical[logical_length] = '\0';
  if(mbstoFriBidichars(fribidi_logical,
                       &fribidi_logical_length,
                       logical))
    goto FAIL;

  if(vtlayout->concrete_layout->is_LTR)
      fribidi_type = FRIBIDI_TYPE_LTR;
  else
      fribidi_type = FRIBIDI_TYPE_RTL;
  fribidi_log2vis(fribidi_logical,
                  fribidi_logical_length,
                  &fribidi_type,
                  fribidi_visual,
                  l2v_index,
                  v2l_index,
                  embedding_level_list);

  array_size --;
  if(l2v_index)
    for(i=0;i<array_size;i++)
      logical2visual_index[i] = l2v_index[i];


  if(v2l_index)
    for(i=0;i<array_size;i++)
      visual2logical_index[i] = v2l_index[i];

  if(embedding_level_list)
    for(i=0;i<array_size;i++)
      embedding_level[i] = embedding_level_list[i];

  if(FriBidiCharstombs(visual,visual_length,fribidi_visual))
      goto FAIL;
  
  FREE_ALL();
  return 0;
  FAIL:
  FREE_ALL();
  return 1;
}

static void VTLayout_set_global_direction(VTLayout *vtlayout,
                                         int direction)
{
  switch(direction)
  {
    case VT_RTL:
        vtlayout->concrete_layout->is_LTR = 0;
        break;
    case VT_LTR:
        vtlayout->concrete_layout->is_LTR = 1;
        break;
    default:
        /* do nothing */;
  }
}

static int VTLayout_is_direction_LTR(VTLayout *vtlayout)
{
  return vtlayout->concrete_layout->is_LTR;
}


VTLayout *VTLayout_new()
{
  VTLayout *vtlayout;

  if((vtlayout = (VTLayout *)malloc(sizeof(VTLayout))) == NULL)
      return NULL;

  if((vtlayout->concrete_layout =
      (struct ConcreteVTLayout *)malloc(sizeof(struct ConcreteVTLayout)))
     == NULL)
  {
    free(vtlayout);
    return NULL;
  }

  vtlayout->concrete_layout->is_LTR = 1;
  vtlayout->concrete_layout->is_nonimal = 1;
  vtlayout->concrete_layout->is_shaped = 1;
  vtlayout->transform = VTLayout_transform;
  vtlayout->set_global_direction = VTLayout_set_global_direction;
  vtlayout->is_direction_LTR = VTLayout_is_direction_LTR;
  vtlayout->set_numerals = NULL;
  return vtlayout;

}

void VTLayout_destroy(VTLayout *vtlayout)
{
  if(vtlayout)
  {
    if(vtlayout->concrete_layout)
      free(vtlayout->concrete_layout);
    free(vtlayout);
  }
}
