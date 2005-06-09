/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#if defined(HAVE_CONFIG_H)
# include "../config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __USE_XOPEN
#include <wchar.h>
#include "screen_row.h"
#include "screen_visual_row.h"
#include "iterm/vtlayout.h"
/*
  - Implementation Algorithm Note

  step1. layout character array(multi byte array).
  step2. construct logical char2cell_index
  step3. take one character
         add the character into visual buffer
         construct visual char2cell_index
         construct visual <-> logical cell cross index
         goto step 3. fisrt untli last character.
*/

#define debug 0

#define construct_char2cell_index(row, char2cell_index) \
do { \
  int col = 0; \
  int ch = 0; \
  while(col<row->num_columns) \
  { \
    char2cell_index[ch] = col; \
    if(Row_is_char_drawn(row,col)) { \
      if(Row_get_num_combined(row,col)) { \
        int i ;\
        for(i=0;i<Row_get_num_combined(row,col);i++) { \
           char2cell_index[++ch] = col; \
        } \
      } \
      col += Row_get_cell_width(row,col); \
    } else { \
     col++; \
    } \
    ch++; \
  } \
} while(0)

static inline void do_layout(VisualRow *vrow, VTLayout *vtlayout)
{
#define SIZE 4096
  size_t logical_char2cell_index[SIZE];
  size_t in2out[SIZE];
  size_t out2in[SIZE];
  char logical[SIZE]; /* is this enough? */
  int logical_width, logical_length;
  char visual[SIZE];  /* is this enough? */
  size_t visual_length = SIZE;
  wchar_t wc;
  mbstate_t ps;
  size_t mb_len;
  int consumed = 0;
  int cursor_pos = 0;
  int current_char = 0;
  int width;
  int logical_cell_index;
  Char mbchar;

  memset(logical_char2cell_index,0,SIZE);
  memset(logical,0,SIZE);
  memset(visual,0,SIZE);
  memset(in2out,0,SIZE);
  memset(out2in,0,SIZE);
  memset(&ps,0,sizeof(ps));

  construct_char2cell_index(vrow->row,logical_char2cell_index);

  Row_compose_bytes(vrow->row,
                    0,vrow->row->num_columns,
                    logical,SIZE,
                    &logical_length,&logical_width,1);
#undef SIZE  
#if  debug
  printf("logical: [%s]\n",logical);
  printf("logical_length = %u\n",logical_length);
#endif

  vtlayout->transform(vtlayout,
                     logical, logical_length,
                     visual, &visual_length,
                     in2out,out2in,NULL);
#if debug
  printf("visual : [%s]\n",visual);
  printf("visual_length  = %u\n\n",visual_length);
#endif  

      /* Well off course, I need to refine this code ;-) */
  while(consumed < visual_length)
  {
    mb_len = mbrtowc(&wc,visual+consumed,visual_length-consumed,&ps);
    if(mb_len == 0 || mb_len == (size_t)-1 || mb_len == (size_t)-2 )
    {
#if debug      
      printf("found invalid or NULL character(%u)\n",mb_len); 
#endif
      break;
    }
    else
    {
      logical_cell_index = logical_char2cell_index[out2in[current_char]];
#if debug      
      printf("logical char index[%d] <-> visual char index[%d]\n",
             out2in[current_char],current_char);
#endif      
      if(Row_is_char_drawn(vrow->row,logical_cell_index))
                          
      {
        width = wcwidth(wc);
        Char_init(mbchar,visual+consumed,mb_len,width,(width>0));
        Row_add_char(vrow->vrow,
                     cursor_pos,
                     &mbchar,
                     Row_is_bold(vrow->row,logical_cell_index),
                     Row_is_blink(vrow->row,logical_cell_index),
                     Row_is_inverse(vrow->row,logical_cell_index),
                     Row_is_underline(vrow->row,logical_cell_index),
                     Row_get_foreground(vrow->row,logical_cell_index),
                     Row_get_background(vrow->row,logical_cell_index),
                     Row_get_charset(vrow->row,logical_cell_index));
      }
      else
      {
        Row_set_char_drawn(vrow->vrow,cursor_pos,0);
        width = 1;
      }
      
      vrow->visual2logical_index[cursor_pos] = logical_cell_index;
      vrow->logical2visual_index[logical_cell_index] = cursor_pos;
#if debug      
      printf("logical cell index[%d] <-> visual  cell index[%d]\n",
             logical_cell_index,cursor_pos);
#endif      
                   
    }
        /* process for multi column character */
    if(width > 1 && Row_is_char_drawn(vrow->row,logical_cell_index) &&
       width == Row_get_cell_width(vrow->row,logical_cell_index) )
    {
      int i;
      for(i=1;i<width;i++)
      {
        vrow->visual2logical_index[cursor_pos+i] = logical_cell_index+i;
        vrow->logical2visual_index[logical_cell_index+i] = cursor_pos+i;
      }
    }
    cursor_pos+=width;
    current_char++;
    consumed+=mb_len;
  }
      /* The visual row possibly has ligatures */
  if(cursor_pos < vrow->row->num_columns &&
     !vtlayout->is_direction_LTR(vtlayout))
      Row_insert_cells(vrow->vrow,0,vrow->row->num_columns-cursor_pos);
      
}

VisualRow *VisualRow_new()
{
  VisualRow *vrow;

  if( (vrow = (VisualRow *)malloc(sizeof(VisualRow))) == NULL)
      return NULL;

  /* dummy */
  vrow->vrow = Row_new(1);
  vrow->logical2visual_index = NULL;
  vrow->visual2logical_index = NULL;

  return vrow;
}

void VisualRow_destroy(VisualRow *vrow)
{
  if(vrow)
  {
    if(vrow->vrow)
        Row_destroy(vrow->vrow);
    if(vrow->visual2logical_index)
        free(vrow->visual2logical_index);
    if(vrow->logical2visual_index)
        free(vrow->logical2visual_index);
    free(vrow);
  }
}

static void VisualRow_clear(VisualRow *vrow)
{
  Row_clear(vrow->vrow);
  memset(vrow->logical2visual_index,0,vrow->vrow->num_columns*sizeof(size_t));
  memset(vrow->visual2logical_index,0,vrow->vrow->num_columns*sizeof(size_t));
}

static void VisualRow_resize(VisualRow *vrow,int size)
{
  Row_resize(vrow->vrow,size);

  if(vrow->visual2logical_index)
      free(vrow->visual2logical_index);
  if(vrow->logical2visual_index)
      free(vrow->logical2visual_index);

  if((vrow->logical2visual_index = (size_t *)calloc(size,
                                                    sizeof(size_t))) == NULL)
      goto FAIL;

  if((vrow->visual2logical_index = (size_t *)calloc(size,
                                                    sizeof(size_t))) == NULL)
      goto FAIL;

  return;
  FAIL:
  if(vrow->visual2logical_index)
      free(vrow->visual2logical_index);
  if(vrow->logical2visual_index)
      free(vrow->logical2visual_index);
}

static void VisualRow_set_row(VisualRow *vrow,
                              Row *row,
                              VTLayout *vtlayout)
{
  if(vrow->vrow->num_columns != row->num_columns)
      VisualRow_resize(vrow,row->num_columns);
  VisualRow_clear(vrow);
  vrow->target = vrow->vrow;
  do_layout(vrow,vtlayout);
}

/**
 * get visual ordered row
 * @param vrow referee to VisualRow object
 * @param row referee tological ordered Row object
 * @return visual row object associated of row(logical)
 */
Row *VisualRow_get_visual_row(VisualRow *vrow,
                              Row *row,
                              VTLayout *vtlayout)
{
  vrow->row = row;
  if(vtlayout)
      VisualRow_set_row(vrow,row,vtlayout);
  else
    vrow->target = row;;

  return vrow->target;
}

int VisualRow_copy_logical_buffer(VisualRow *vrow,
                                  int visual_start_col,
                                  int visual_end_col,
                                  char *buffer,int buffer_length,
                                  int *total_length,int *total_width)
{
  if(vrow->target == vrow->row)
  {
    return Row_compose_bytes(vrow->target,visual_start_col,
                             visual_end_col,
                             buffer,buffer_length,
                             total_length,total_width,0);
  }
  else
  {
    int c = 0;
    int length;
    int width;
    *total_length = 0;
    *total_width = 0;
    
    while(c<vrow->row->num_columns)
    {
      if((visual_start_col <= vrow->logical2visual_index[c]) &&
         (vrow->logical2visual_index[c] < visual_end_col))
      {
        Row_compose_bytes(vrow->row,c,c+Row_get_cell_width(vrow->row,c),
                           buffer+(*total_length),
                           buffer_length-(*total_length),
                           &length,&width,0);
        *total_length+=length;
        *total_width+=width;
        if(width > 0)
            c += width;
        else
            c++;
      }
      else
      {
        c++;
      }
    }
    return 0;
  }
  return 1;
}

int VisualRow_compose_visual_buffer(VisualRow *vrow,
                                    int visual_start_col,
                                    int visual_end_col,
                                    char *buffer,int buffer_length,
                                    int *total_lenth,int *total_width)
{
  return Row_compose_bytes(vrow->target,visual_start_col,visual_end_col,
                           buffer,buffer_length,
                           total_lenth,total_width,0);
}

/**
 * translate logical column potion to visual column position
 * @param vrow referee to VisualRow object
 * @param logical_column_position logical column position
 * @return visual column potion associated of logical_column_position
 */
int VisualRow_translate_logical_position(VisualRow *vrow,
                                         int logical_column_position)
{
  if(vrow->target == vrow->row)
      return logical_column_position;
  else
      return vrow->logical2visual_index[logical_column_position];
}

void VisualRow_set_selection(VisualRow *vrow, int scol, int ecol, int selected)
{
  int c;
  int logical_index;
  if(vrow->visual2logical_index)
  {
    for(c=scol;c<ecol;c++)
    {
      logical_index = vrow->visual2logical_index[c];
      if(Row_is_char_drawn(vrow->row,logical_index))
      {
        Row_set_selection(vrow->row,logical_index,logical_index+1,selected);
        Row_set_selection(vrow->vrow,c,c+1,selected);
      }
    }
  }
  else
  {
    Row_set_selection(vrow->row,scol,ecol,selected);
  }
}
