/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#ifndef __SCREEN_ROW_H__
#define __SCREEN_ROW_H__

#include "mbchar.h"
typedef unsigned char CellAttributes[4];
/**<
       attr[3]   attr[2] attr[1] attr[0]
   |num_combined|charset| color | attrs |
 */
enum {
    ATTRIBUTES = 0,
    COLOR = 1,
    CHARSET = 2,
    NUM_COMBINED = 3
};

enum {
    BOLD = 0x01,
    BLINK = 0x02,
    INVERSE = 0x04,
    UNDERLINE = 0x08,
    SELECTED = 0x10
};

enum {
    FOREGROUND_MASK = 0x0F,
    BACKGROUND_MASK = 0xF0
};

typedef unsigned char RowAttributes;
/**< row attributes */
enum {
    NOT_WRAPPED = 0x00,
    WRAPPED = 0x01,
    REVERSE_WRAPPED = 0x02,
    UPDATED = 0x10
};

typedef struct _Row{
  int num_columns;
  RowAttributes attrs;
  int updated_first;
  int updated_last;
  struct RowBuffer {
    CellAttributes *attrs;
    int *cell_to_char;
    char *value;
    int value_max;
    int value_length;
  } buffer;
}Row;

Row *Row_new(int col);
void Row_destroy(Row *row);
int Row_add_char(Row *row, int col, Char *mbchar, int bold, int blink,
                 int inverse, int underline, int foreground, int background,
                 char charset);
void Row_resize(Row *row, int size);
void Row_set_selection(Row *row ,int scol, int ecol, int selected);
int Row_insert_cells(Row *row, int col_start, int num_spaces);
int Row_delete_cells(Row *row, int col_start, int num);
int Row_find_multicolum_char_start_column(Row *row, int col);
int Row_copy(Row *src, Row *dest);
int Row_clear(Row *row);
int Row_clear_cells(Row *row, int start_col, int end_col);
int Row_compose_bytes(Row *row, int col_start, int col_end,
                      char *str, int str_len,
                      int *total_len, int *total_width, int pad);

int Row_is_multicolumn_start(Row *row, int col);
int Row_get_cell_width(Row *row,int col);
void Row_set_char_drawn(Row *row, int col,int value);

#define Row_get_updated_first(row) (row->updated_first)
#define Row_get_updated_last(row) (row->updated_last)
#define Row_clear_updated(row) \
do { \
 row->attrs &= ~UPDATED; \
 row->updated_first = row->num_columns; \
 row->updated_last = 0; \
} while(0)
#define Row_set_updated(row,scol,ecol) \
do {\
 row->attrs |= UPDATED; \
 row->updated_first = scol < row->updated_first ? scol : row->updated_first; \
 row->updated_last = row->updated_last < ecol  ? ecol : row->updated_last; \
} while(0)

#define Row_is_updated(row) (row->attrs & UPDATED)

#define Row_is_char_drawn(row, col)  ((col < row->num_columns) && (row)->buffer.cell_to_char[col] >= 0)

#define Row_is_same_attributes(row1,col1,row2,col2) \
 (Row_is_char_drawn(row1,col1) && Row_is_char_drawn(row2,col2) && \
  row1->buffer.attrs[col1][0] == row2->buffer.attrs[col2][0] && \
  row1->buffer.attrs[col1][1] == row2->buffer.attrs[col2][1] && \
  row1->buffer.attrs[col1][2] == row2->buffer.attrs[col2][2] )

#define Row_is_bold(row, col) ((row)->buffer.attrs[(col)][0]&BOLD)
#define Row_is_blink(row, col) ((row)->buffer.attrs[(col)][0]&BLINK)
#define Row_is_inverse(row, col) ( (((row)->buffer.attrs[(col)][0]&INVERSE) == INVERSE) ^ (((row)->buffer.attrs[(col)][0]&SELECTED) == SELECTED) )
#define Row_is_underline(row, col) ((row)->buffer.attrs[(col)][0]&UNDERLINE)

#define Row_get_foreground(row,col) (((row)->buffer.attrs[(col)][1])&FOREGROUND_MASK)
#define Row_get_background(row,col) ((((row)->buffer.attrs[(col)][1])&BACKGROUND_MASK) >> 4)

#define Row_get_charset(row,col) (((row)->buffer.attrs[(col)][2])&0xFF)
#define Row_get_num_combined(row,col) (((row)->buffer.attrs[(col)][3])&0xFF)

#define Row_is_wrapped(row) (row->attrs&WRAPPED)
#define Row_set_wrapped(row,flag) (row->attrs=((row->attrs&0x0F0)|flag))

#endif /* __SCREEN_ROW_H__ */
