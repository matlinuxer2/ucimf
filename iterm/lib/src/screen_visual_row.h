/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/**
 * @file screen_visual_row.h
 * VisualRow class definition.
 * This is a Model of VisualRow, which is for Complex Text Layout engine
 */
#ifndef __VISUAL_ROW_H__
#define __VISUAL_ROW_H__

#include "screen_row.h"
#include "iterm/vtlayout.h"
typedef struct _VisualRow {
  Row *vrow;
  Row *row;
  Row *target;
  size_t *logical2visual_index;
  size_t *visual2logical_index;
} VisualRow;

VisualRow *VisualRow_new();
void VisualRow_destroy(VisualRow *vrow);
Row *VisualRow_get_visual_row(VisualRow *vrow, Row *row,VTLayout *vtlayout);
int VisualRow_translate_logical_position(VisualRow *vrow,
                                         int logical_column_position);
void VisualRow_set_selection(VisualRow *vrow, int scol, int ecol,int selected);
int VisualRow_copy_logical_buffer(VisualRow *vrow,
                                  int visual_start_col, int visual_end_col,
                                  char *buffer,int buffer_length,
                                  int *total_length,int *total_width);
#endif
