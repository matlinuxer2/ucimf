/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include "../config.h"
#include "screen_row.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Char Blank = {(unsigned char *)" ",1,1,1};
enum {
    SPECIAL_GRAPHICS = '0',
    UK = 'A',
    ASCII = 'B'
};

#define ON_ERROR(row, func) \
if(row->buffer.value_max < row->buffer.value_length) \
  fprintf(stderr,"%s: value_max %d, value_length %d\n", \
          func, row->buffer.value_max ,row->buffer.value_length); \

#define IS_SAME_INDEX(row,col1,col2) (row->buffer.cell_to_char[(col1)] == row->buffer.cell_to_char[(col2)])
#define GET_INDEX(row,col) ((col < row->num_columns && row->buffer.cell_to_char[(col)] >= 0) ? row->buffer.cell_to_char[(col)] : row->buffer.value_length)
#define SET_INDEX(row,col,val) (row->buffer.cell_to_char[(col)] = (val))
#define CLEAR_CELL(row,col) (row->buffer.cell_to_char[(col)] = -1)
#define EXPAND_BUFFER(row,size,msg) \
do { \
    int columns_needs; \
    int last_col=1; \
    while(last_col < row->num_columns && Row_is_char_drawn(row,last_col)) \
          last_col++; \
    columns_needs = row->num_columns - last_col;\
    if(columns_needs < (size)) \
        columns_needs = (size); \
    row->buffer.value = realloc(row->buffer.value, \
                                 row->buffer.value_max+columns_needs); \
    row->buffer.value_max += columns_needs; \
} while(0)

#define COPY_TO(row,index,mbchar) \
do { \
 memcpy(row->buffer.value+index, Char_get_string(mbchar), Char_get_length(mbchar)); \
} while(0)
 
#define CellAttr_init(attr, bold, blink, inverse, underline, foreground, background, charset) \
do {\
  memset(attr,0,sizeof(CellAttributes)); \
  if(bold) (attr)[ATTRIBUTES]|=BOLD; \
  if(blink) (attr)[ATTRIBUTES]|=BLINK; \
  if(inverse)(attr)[ATTRIBUTES]|=INVERSE; \
  if(underline)(attr)[ATTRIBUTES]|=UNDERLINE; \
  (attr)[COLOR]|=foreground; \
  (attr)[COLOR]|=(background<<4); \
  (attr)[CHARSET]|=charset; \
} while(0)

static void simple_clear_cells(Row *row, int start, int end)
{
  int i;
#ifdef DEBUG
  if(start < 0 || row->num_columns < start ||
     end < 0 || row->num_columns < end ||
     end < start)
  {
    fprintf(stderr,
            "simple clear cells, row->num_columns = %d start = %d end = %d\n",
            row->num_columns,start,end);
    return;
  }
  
#endif
  for(i=start;i<end;i++)
  {
    CLEAR_CELL(row,i);
  }
  Row_set_updated(row,start,end);
}

static void RowBuffer_shrink(Row *row, int size)
{
  if(Row_is_char_drawn(row,size-1))
  {
        /* edge character is double width */
    if(IS_SAME_INDEX(row,size-1,size))
    {
          /* delete edge char */
      row->buffer.value_length = GET_INDEX(row,(size-1));
      CLEAR_CELL(row,(size-1));
    }
    else if(Row_is_char_drawn(row,size))
    {
      row->buffer.value_length = GET_INDEX(row,size);
    }
  }

  row->buffer.value_max = size < row->buffer.value_length ? row->buffer.value_length : size;
  row->buffer.value = (char *)realloc(row->buffer.value,
                                      row->buffer.value_max);
#ifdef DEBUG  
  if(row->num_columns < row->buffer.value_max ||
     row->num_columns < row->buffer.value_length ||
     row->buffer.value_max < row->buffer.value_length)
      fprintf(stderr,"row->num_columns %d, row->buffer.value_max %d\n",
              row->num_columns, row->buffer.value_max);
#endif  
}

static void shift_buffer(Row *row, int dest, int src, int length)
{
  if( row->buffer.value_max < dest + length )
  {
    EXPAND_BUFFER(row,(dest+length-row->buffer.value_max),"shift_buffer");
  }
  memmove(row->buffer.value + dest, row->buffer.value + src, length);
#ifdef DEBUG  
  if(row->buffer.value_max < row->buffer.value_length) 
      fprintf(stderr,"shift_buffer: row->num_columns %d, row->buffer.value_max %d\n",
              row->num_columns, row->buffer.value_max);
#endif  
}

static void shift_index(Row *row, int start, int index_diff)
{
  int index = start;
  while(index < row->num_columns && Row_is_char_drawn(row,index))
  {
        /* row->buffer.cell_to_char[index] += index_diff; */
    SET_INDEX(row,index,(GET_INDEX(row,index)+index_diff));
    index++;
  }
}

static void shift_cells(Row *row, int start , int value)
{
  int length = row->num_columns - start;
  if(value > 0)
  {
    length -= value;
  }
  memmove(row->buffer.cell_to_char + start + value,
          row->buffer.cell_to_char + start,
          length*sizeof(int));
  memmove(row->buffer.attrs + start + value,
          row->buffer.attrs + start,
          length*sizeof(CellAttributes));
  Row_set_updated(row,start,row->num_columns);
  if(value < 0)
  {
    simple_clear_cells(row,row->num_columns+value,row->num_columns);
  }
}


static void RowBuffer_add_char(Row *row, int col, Char *mbchar)
{
  int index;
  int w;
  index = GET_INDEX(row,col);

  if(row->buffer.value_max < index + Char_get_length(mbchar))
  {
    EXPAND_BUFFER(row,
                  (index + Char_get_length(mbchar) - row->buffer.value_max),
                  "RowBuffer_add_char()");
  }
  for(w=0;w<Char_get_width(mbchar);w++) 
  {
        /* do this row->buffer.cell_to_char[col+w] = index; */
    SET_INDEX(row,(col+w),index);
  }
      /* terminate column */
  if(col+w < row->num_columns)
      CLEAR_CELL(row,col+w);
  COPY_TO(row,index,mbchar);
  row->buffer.value_length = index + Char_get_length(mbchar);
  Row_set_updated(row,col,col+Char_get_width(mbchar));
#ifdef DEBUG  
  if(row->buffer.value_max < row->buffer.value_length)
      fprintf(stderr,"add_char, row->num_columns %d, row->buffer.value_max %d\n",
              row->num_columns, row->buffer.value_max);
#endif  
}

static int get_end_index(Row *row, int col, int col_width)
{
  if(row->num_columns < col + col_width ||
     (!Row_is_char_drawn(row,(col+col_width))) )
      return row->buffer.value_length;
  return GET_INDEX(row,col+col_width);
}

static int has_next_char(Row *row, int col, int width)
{
  if(col == row->num_columns - width) /* col is right edge of row */
      return 0;
  if(!Row_is_char_drawn(row,col+1)) /* col+1 has no char */
      return 0;
  if(width == 1)
  {
    if(IS_SAME_INDEX(row,col,col+1))
        return has_next_char(row,col+1,width);
    else
        return 1;
  }
      
  return has_next_char(row,col+1,width-1); /* col+width is multicolumn,
                                              so check next */
}

static void RowBuffer_replace_char(Row *row, int col, Char *mbchar)
{
  int start_index = GET_INDEX(row,col);
  int ex_end_index = get_end_index(row,col,Char_get_width(mbchar));
  int bytes_length =  ex_end_index - start_index;
  int w;

  if(bytes_length ==  Char_get_length(mbchar))
      COPY_TO(row, start_index, mbchar);
  else
  {
    int diff = Char_get_length(mbchar) - bytes_length;
    shift_buffer(row,start_index + Char_get_length(mbchar), ex_end_index,
                 row->buffer.value_length - ex_end_index);
    shift_index(row,col+Char_get_width(mbchar),diff);
    COPY_TO(row, start_index, mbchar);
    row->buffer.value_length += diff;
  }

  for(w=1;w<Char_get_width(mbchar);w++)
  {
        /* do this row->buffer.cell_to_char[col+w] = start_index; */
    SET_INDEX(row,(col+w),start_index);
  }
  Row_set_updated(row,col,col+Char_get_width(mbchar));
#ifdef DBUG  
  if(row->buffer.value_max < row->buffer.value_length)
  {
    fprintf(stderr,
            "replace_char after : col %d, num_columns %d, value_length %d, value_max %d\n",
            col, row->num_columns, row->buffer.value_length,
            row->buffer.value_max);
    fprintf(stderr,
            "replace_char after: bytes_length %d, mbchar->length %d ex_end_index %d\n",
            bytes_length, Char_get_length(mbchar), ex_end_index);
  }
#endif  
}

static void RowBuffer_simple_replace_space(Row *row, int col)
{
  row->buffer.value[GET_INDEX(row,col)] = ' ';
  row->buffer.attrs[col][0] = row->buffer.attrs[col][1] = row->buffer.attrs[col][3] = 0;
  row->buffer.attrs[col][2] = ASCII; /* ASCII */
  Row_set_updated(row,col,(col+1));
}

Row *Row_new(int col)
{
  Row *row = NULL;
  row = (Row *)malloc(sizeof(Row));
  if(row == NULL)
  {
    perror("Row_new(1)");
    return NULL;
  }
  row->buffer.cell_to_char = NULL;
  row->buffer.value = NULL;
  row->buffer.attrs = NULL;

  row->buffer.cell_to_char = (int *)calloc(col,sizeof(int));
  if(row->buffer.cell_to_char == NULL)
  {
    perror("Row_new(2)");
    goto FAIL;
  }

  row->buffer.value = (char *)calloc(col,sizeof(char));
  if(row->buffer.value == NULL)
  {
    perror("Row_new(3)");
    goto FAIL;
  }
  
  row->buffer.attrs = (CellAttributes *)calloc(col,sizeof(CellAttributes));
  if(row->buffer.attrs == NULL)
  {
    perror("Row_new(4)");
    goto FAIL;
  }

  row->buffer.value_max = col;
  row->num_columns = col;
  Row_clear(row);
  Row_clear_updated(row);
  return row;
  FAIL:
  if(row)
  {
    if(row->buffer.cell_to_char)
        free(row->buffer.cell_to_char);
    if(row->buffer.value)
        free(row->buffer.value);
    if(row->buffer.attrs)
        free(row->buffer.attrs);
    free(row);
  }
  return NULL;
}

void Row_destroy(Row *row)
{
  if(row)
  {
    if(row->buffer.cell_to_char)
        free(row->buffer.cell_to_char);
    if(row->buffer.value)
        free(row->buffer.value);
    if(row->buffer.attrs)
        free(row->buffer.attrs);
    free(row);
  }
}

int Row_is_multicolumn_start(Row *row, int col)
{
#ifdef DEBUG
  if(row->num_columns <= col || col < 0)
  {
    fprintf(stderr,
            "Row_is_multicolumn_start, row->num_cols = %d, col = %d\n",
            row->num_columns,col);
    return -1;
  }
#endif
  if(col == 0 || row->num_columns < col)
      return 1;

  if(Row_is_char_drawn(row,col) &&
     !(IS_SAME_INDEX(row,col-1,col)) )
  {
    return 1;
  }
  return 0;
}

int Row_find_multicolum_char_start_column(Row *row, int col)
{
  int c = col;
#ifdef DEBUG
  if(row->num_columns <= col || col < 0)
  {
    fprintf(stderr,
            "Row_find_multicolumn_char_start_column, row->num_cols = %d, col = %d\n",
            row->num_columns,col);
    return -1;
  }
#endif
  if(row->num_columns < col)
      return -1;
  while(0 < c)
  {
    if(Row_is_multicolumn_start(row,c))
        return c;
      c--;
  }
  return c;
}

int Row_get_cell_width(Row *row,int col)
{
  int w=1;
#ifdef DEBUG
  if(row->num_columns <= col || col < 0)
  {
    fprintf(stderr,
            "Row_get_cell_width, row->num_cols = %d, col = %d\n",
            row->num_columns,col);
    return -1;
  }
#endif
  if(row->num_columns <= col)
      return -1;
  if(col == row->num_columns - 1)
      return 1;
  if(IS_SAME_INDEX(row,col,col+1))
      w += Row_get_cell_width(row,col+1);
  return w;
}


int Row_add_char(Row *row, int col, Char *mbchar, int bold, int blink,
                 int inverse, int underline, int foreground, int background,
                 char charset)
{
  int w;

  if(col < 0 || /* invalid */
     row->num_columns <= col || /* invalid */
     row->num_columns < col + Char_get_width(mbchar)) /* no colunm space left */
  {
#ifdef DEBUG
    fprintf(stderr,
            "Row_add_char: row->num_cols %d, col %d, mbchar->width %d\n",
            row->num_columns, col, Char_get_width(mbchar));
    
#endif
    return 1;
  }

  if(IS_COMBINED(mbchar))
  {
    int c=col-1;
    while(0<=c)
    {
      if(Row_is_char_drawn(row,c) &&
         Row_is_multicolumn_start(row,c))
      {
            /* found proper cell */
        int last_index = get_end_index(row,c,Row_get_cell_width(row,c));
        shift_buffer(row, last_index + Char_get_length(mbchar), last_index,
                     row->buffer.value_length - last_index);
        COPY_TO(row, last_index, mbchar);
        row->buffer.value_length += Char_get_length(mbchar);
        row->buffer.attrs[c][NUM_COMBINED]++;
        return 0;
      }
      c--;
    }
        /* failed to find proper cell to add combined char */
    return 1;
  }
  
  for(w=0;w<Char_get_width(mbchar);w++)
  {
    CellAttr_init(row->buffer.attrs[col+w],bold,blink,inverse,underline,
                  foreground,background,charset);
  }

  if(Row_is_char_drawn(row,col))
  {
        /* take care of character over the 2nd column
         of multibyte character */
    if(!Row_is_multicolumn_start(row,col))
    {
          /* This only cares double column character.
             Do I need to care more than 2 ? */
      RowBuffer_simple_replace_space(row, col-1);
          /* do this row->buffer.cell_to_char[col] += 1; */
      SET_INDEX(row,col,(GET_INDEX(row,col)+1));
    }

        /* take care of multi column character over the
           only the first column of multi column character  */
    if(Char_get_width(mbchar) > 1 && Row_get_cell_width(row,col+1) > 1)
    {
      SET_INDEX(row,(col+Char_get_width(mbchar)),
                (get_end_index(row,col+Char_get_width(mbchar),1) - 1));
      RowBuffer_simple_replace_space(row, col+Char_get_width(mbchar));
      Row_set_updated(row,col,col+Char_get_width(mbchar)+1);
    }
    
    if(has_next_char(row,col,Char_get_width(mbchar)))
    {
      if(Char_get_width(mbchar) == 1 &&
         Row_get_cell_width(row,col) != Char_get_width(mbchar))
      {
/* do this, row->buffer.cell_to_char[col+1] = get_end_index(row,col+1,1)-1 */
        SET_INDEX(row,(col+1),(get_end_index(row,col+1,1) - 1));
        RowBuffer_simple_replace_space(row, col+1);
      }
      RowBuffer_replace_char(row,col,mbchar);
    }
    else
    {
          /* take care if the last char width is wider than this one */
      if(Char_get_width(mbchar) < Row_get_cell_width(row,col))
      {
        Row_set_updated(row,col,col+Row_get_cell_width(row,col));
      }
      RowBuffer_add_char(row,col,mbchar);
    }
  }
  else
  {
    if(0 <= col-1 && !(Row_is_char_drawn(row,col-1)))
    {
      Row_add_char(row,col-1,&Blank,0,0,0,0,0,0,ASCII);
    }
    RowBuffer_add_char(row,col,mbchar);
  }
#ifdef DEBUG  
  if(row->buffer.value_max < row->buffer.value_length)
      fprintf(stderr,
              "Row_add_char: col %d, num_columns %d, value_length %d, value_max %d\n",
              col, row->num_columns, row->buffer.value_length,
              row->buffer.value_max);
#endif
  return 0;
}

void Row_resize(Row *row, int size)
{
  if(row && row->num_columns != size)
  {
    int old = row->num_columns;
    if(size < old)
    {
      RowBuffer_shrink(row,size);
    }
    row->buffer.attrs = (CellAttributes *)realloc(row->buffer.attrs,
                                                  size*sizeof(CellAttributes));
    row->buffer.cell_to_char = (int *)realloc(row->buffer.cell_to_char,
                                              size*sizeof(int));
    row->num_columns = size;
    if(old < row->num_columns)
    {
      simple_clear_cells(row,old,row->num_columns);
    }
  }
#ifdef DEBUG
  ON_ERROR(row,"Row_resize");
#endif
}

int Row_clear_cells(Row *row, int start_col, int end_col)
{
  int i,j;
  int ecol = -1;
  int scol = -1;
#ifdef DEBUG
  if(row->num_columns < start_col || start_col < 0 ||
     row->num_columns < end_col || end_col < 0 ||
     end_col < start_col)
      fprintf(stderr,
              "Row_clear_cells: row->num_cols = %d, start_col = %d end_col = %d\n",
              row->num_columns,start_col,end_col);
#endif
  if( end_col < start_col)
      return 1;
  scol = 0 < start_col ? start_col : 0;
  ecol = end_col < row->num_columns ? end_col : row->num_columns ;

  if(!Row_is_char_drawn(row,scol))
      return 0; /* do not need to clear */

  scol = Row_find_multicolum_char_start_column(row,scol);

  if(ecol < row->num_columns &&
     Row_is_char_drawn(row,ecol) &&
     !Row_is_multicolumn_start(row,ecol))
  {
    ecol = Row_find_multicolum_char_start_column(row,ecol);
    ecol += Row_get_cell_width(row,ecol);
  }
  
  if(row->num_columns <= ecol || !Row_is_char_drawn(row,ecol))
  { /*  we have no string on the right.  just clear cells */
    row->buffer.value_length = GET_INDEX(row,scol);
    simple_clear_cells(row,scol,ecol);
  }
  else
  { /* we have string on the right.  This case, insert SPACES */
    int old_length = old_length = GET_INDEX(row,ecol) - GET_INDEX(row,scol);
    int new_length = ecol - scol; /* white space consumes just one byte */

    shift_buffer(row,
                 GET_INDEX(row,scol) + new_length,
                 GET_INDEX(row,ecol),
                 row->buffer.value_length - GET_INDEX(row,ecol));
    shift_index(row, ecol, new_length - old_length);
    for(i=scol,j=0;i<ecol;i++,j++)
    {
          /* do this, row->buffer.cell_to_char[i] = GET_INDEX(row,scol) + j; */
      SET_INDEX(row,i,(GET_INDEX(row,scol) + j));
      RowBuffer_simple_replace_space(row,i);
    }
    row->buffer.value_length += (new_length - old_length);
  }
  Row_set_wrapped(row,NOT_WRAPPED);
#ifdef DEBUG
  ON_ERROR(row,"Row_clear_cells");
#endif
  return 0;
}

int Row_clear(Row *row)
{
  Row_set_wrapped(row,NOT_WRAPPED);
  simple_clear_cells(row,0,row->num_columns);
  row->buffer.value_length = 0;
#ifdef DEBUG
  ON_ERROR(row,"Row_clear");
#endif
  return 0;
}

void Row_set_selection(Row *row ,int scol, int ecol, int selected)
{
  int c;
#ifdef DEBUG
  if(row->num_columns < scol || scol < 0 ||
     row->num_columns < ecol || ecol < 0 ||
     ecol < scol)
      fprintf(stderr,
              "Row_set_selection, row->num_cols = %d, scol = %d ecol = %d\n",
              row->num_columns,scol,ecol);
#endif
  if(selected)
  {
    for(c=scol;c<ecol;c++)
    {
      row->buffer.attrs[c][ATTRIBUTES] |= SELECTED;
    }
  }
  else
  {
    for(c=scol;c<ecol;c++)
    {
      row->buffer.attrs[c][ATTRIBUTES] &= (~SELECTED);
    }
  }
  Row_set_updated(row,scol,ecol);
#ifdef DEBUG
  ON_ERROR(row,"Row_set_selection");
#endif
}

int Row_compose_bytes(Row *row, int col_start, int col_end,
                      char *str, int str_len,
                      int *total_len, int *total_width, int pad)
{
  int scol;
  int ecol;
  int buf_end;
  int buf_start;
  *total_len = 0;
#ifdef DEBUG
  if(row->num_columns < col_start || col_start < 0 ||
     col_end < 0 || col_end < col_start)
  {
    fprintf(stderr,
            "Row_compose_bytes, row->num_cols = %d, col_start = %d col_end = %d\n",
            row->num_columns,col_start,col_end);
  }
#endif

  if(row->buffer.value_length <= 0)
  {
    if(pad)
    {
      int i;
      int min = str_len < row->num_columns ? str_len : row->num_columns;
      for(i=0;i<min;i++)
      {
        memcpy(str+i," ",1);
      }
      *total_len = min;
      *total_width = min;
    }
    else
    {
      *total_len = 0;
      *total_width = 0;
    }
    return 0;
  }
  if(col_end <= col_start)
      return 1;
  
  if(col_start < 0)
      col_start = 0;
  if(col_end > row->num_columns)
      col_end = row->num_columns;
  
  scol = col_start;
  ecol = col_end;
  
  while(scol < row->num_columns && !Row_is_multicolumn_start(row,scol))
      scol++;
  while(scol < ecol && !Row_is_char_drawn(row,ecol-1))
      ecol--;

  buf_start = GET_INDEX(row,scol);
  buf_end = GET_INDEX(row,ecol);
  *total_len = buf_end - buf_start;

  if(str_len < *total_len)
      *total_len = str_len;
  memcpy(str, (row->buffer.value + buf_start) ,*total_len);

  *total_width = ecol - scol;
  if(pad && *total_width < (col_end - col_start))
  {
    int num_spaces = row->num_columns - *total_width;
    int num_left = (str_len >  *total_len) ? (str_len - *total_len) : 0;
    int min = num_spaces < num_left ? num_spaces : num_left;
    int c;

    for(c=0;c<min;c++)
    {
      memcpy(str + *total_len + c, " ", 1);
    }
    *total_width += min;
    *total_len += min;
  }
#ifdef DEBUG
  ON_ERROR(row,"Row_compose_bytes");
#endif
  return 0;
}

static void simple_delete_right(Row *row, int col_start)
{
  col_start = Row_find_multicolum_char_start_column(row,col_start);
  row->buffer.value_length = GET_INDEX(row,col_start);
  simple_clear_cells(row,col_start,row->num_columns);
  Row_set_updated(row,col_start,row->num_columns);
}

int Row_insert_cells(Row *row, int col_start, int num_spaces)
{
  int i;
  int src_index = GET_INDEX(row,col_start);
  int length = GET_INDEX(row,row->num_columns - num_spaces) - GET_INDEX(row,col_start);

  if( (col_start < 0) ||
      (row->num_columns-1 <= col_start) ||
      (!Row_is_char_drawn(row,col_start)) )
  {
#ifdef DEBUG
    fprintf(stderr,"Row_insert_cells: invalid colmun: %d\n", col_start);
#endif    
    return 0;
  }

      /* inserting cells till right side */
  if( row->num_columns <= col_start + num_spaces )
  {
    row->buffer.value_length = GET_INDEX(row,col_start);
    simple_delete_right(row,col_start);
    return 0;
  }
  
  if(Row_get_cell_width(row,row->num_columns-num_spaces-1 ) > 1)
  {
        /* double column on the edge should be removed */
    row->buffer.value_length = GET_INDEX(row,row->num_columns-num_spaces-1);
    CLEAR_CELL(row,row->num_columns-num_spaces-1);
  }

  if(!Row_is_multicolumn_start(row,col_start))
  {
    if(Row_is_char_drawn(row,col_start+1))
    { 
      RowBuffer_simple_replace_space(row, col_start-1);
          /* do this, row->buffer.cell_to_char[col_start] += 1; */
      SET_INDEX(row,col_start,(GET_INDEX(row,col_start)+1));
      Row_add_char(row,col_start,&Blank,0,0,0,0,0,0,ASCII);
    }
    else
    {
      Row_clear_cells(row,col_start-1,col_start+1);
      return 0;
    }
  }

      /* Now we have to insert SPACES */
  if(Row_is_char_drawn(row,row->num_columns-1-num_spaces))
      row->buffer.value_length = GET_INDEX(row,row->num_columns-num_spaces);
  row->buffer.value_length += num_spaces;
  
  shift_cells(row,col_start, num_spaces);
  shift_index(row, (col_start + num_spaces), num_spaces);
  shift_buffer(row,src_index+num_spaces, src_index, length);

  i=col_start;
  RowBuffer_simple_replace_space(row, i);
  for(i++;i<col_start+num_spaces;i++)
  {
        /* do this, row->buffer.cell_to_char[i] = GET_INDEX(row,(i-1))+1; */
    SET_INDEX(row,i,(GET_INDEX(row,(i-1))+1));
    RowBuffer_simple_replace_space(row, i);
  }
#ifdef DEBUG  
  if(row->buffer.value_max < row->buffer.value_length)
  {
    fprintf(stderr,"Row_insert_cells after : num_columns %d, value_max = %d, value_length %d\n", row->num_columns, row->buffer.value_max, row->buffer.value_length);
  }
#endif  
  return 0;
}

/* col_start must be coordinated pointing at the fisrt column of
   multi column character */
static void simple_delete_cells(Row *row, int col_start, int num)
{
  int dest_index;
  int src_index;
  int diff;

  dest_index = GET_INDEX(row,col_start);
  src_index = GET_INDEX(row,col_start+num);
  diff = src_index - dest_index;
  
  shift_cells(row,col_start+num,-num);
  shift_buffer(row,dest_index,src_index,row->buffer.value_max-src_index);
  shift_index(row,col_start,-diff);
  row->buffer.value_length -= diff;
  Row_set_updated(row,col_start,row->num_columns);
}

int Row_delete_cells(Row *row, int col_start, int num)
{

      /* invalid case, do nothing */
  if(col_start < 0 || row->num_columns -1 <= col_start || num < 0)
  {
#ifdef DEBUG
    if(row->num_columns <= col_start || col_start < 0 || num < 0)
    {
      fprintf(stderr,
              "Row_delete_cells, row->num_cols = %d, col_start = %d\n",
              row->num_columns,col_start);
    }
#endif
    return 1;
  }

      /* No need to delete characters */
  if(!Row_is_char_drawn(row,col_start))
      return 0;

      /* delete right side, we don't need to shift right side string */
  if( (row->num_columns <= col_start + num) ||
      !Row_is_char_drawn(row,col_start+num) )
  {
    simple_delete_right(row,col_start);
    return 0;
  }
      /* if we will try to delete only fisrt(or some) column(s)
         of multicolumn char */
  else if(!Row_is_multicolumn_start(row,col_start+num))
  {
    int end_col = Row_find_multicolum_char_start_column(row,col_start+num);
        /* if the char has another char on right */
    if(has_next_char(row,end_col,
                     end_col + Row_get_cell_width(row, end_col)))
    {
/* do this,
   row->buffer.cell_to_char[col_start+num]=GET_INDEX(row,col_start+num+1)-1 */
      SET_INDEX(row,(col_start+num),(GET_INDEX(row,col_start+num+1)-1));
      RowBuffer_simple_replace_space(row, col_start+num);
    }
    else /* if that multi column char is the last char */
    {
      simple_delete_right(row,col_start);
      return 0;
    }
  }
      /* Now we have to delete and shift right side of string */

      /* if col_start is not first column of multicolumn char
         replace left side columns as spaces 
       */
  if(!Row_is_multicolumn_start(row,col_start))
  {
        /* only handle double width char */
/* do this,
   row->buffer.cell_to_char[col_start] = GET_INDEX(row,col_start-1) + 1; */
    SET_INDEX(row,col_start,(GET_INDEX(row,col_start-1) + 1));
    RowBuffer_simple_replace_space(row, col_start-1);
  }
  simple_delete_cells(row,col_start,num);
  
#ifdef DEBUG
  ON_ERROR(row,"Row_delete_cells");
#endif
  return 0;
}

int Row_copy(Row *src, Row *dest)
{
  int num_col = src->num_columns < dest->num_columns ? src->num_columns : dest->num_columns;
  int last_index = GET_INDEX(src,num_col);

  if(dest->buffer.value_max < last_index)
  {
    dest->buffer.value = realloc(dest->buffer.value,last_index);
    dest->buffer.value_max = last_index;
  }

  memcpy(dest->buffer.value,src->buffer.value,last_index);
  dest->buffer.value_length = last_index;

  memcpy(dest->buffer.cell_to_char,
         src->buffer.cell_to_char,
         num_col*sizeof(int));
  memcpy(dest->buffer.attrs,
         src->buffer.attrs,
         num_col*sizeof(CellAttributes));
  if(num_col < dest->num_columns)
  {
    simple_clear_cells(dest,num_col,dest->num_columns);
  }
#ifdef DEBUG
  ON_ERROR(src,"Row_copy(src)");
  ON_ERROR(dest,"Row_copy(dest)");
#endif
  return 0;
}

void Row_set_char_drawn(Row *row, int col,int value)
{
#ifdef DEBUG
  if(row->num_columns <= col || col < 0)
      fprintf(stderr,
              "Row_set_char_drawn, row->num_cols = %d, col = %d\n",
              row->num_columns,col);
#endif
  if(!value && Row_is_char_drawn(row,col))
  {
    if(has_next_char(row,col,Row_get_cell_width(row,col)))
    {
      int end_index = GET_INDEX(row,col+Row_get_cell_width(row,col));
      int start_index = GET_INDEX(row,col);

      if(end_index > start_index+1)
      {
        int diff = end_index - start_index - 1;
        shift_buffer(row,end_index,start_index+1,row->num_columns-end_index);
        shift_index(row,start_index+1,diff);
        
      }
      RowBuffer_simple_replace_space(row,col);
    }
    else
    {
      CLEAR_CELL(row,col);
    }
  }
#ifdef DEBUG
  ON_ERROR(row,"Row_set_char_drawn");
#endif
}
