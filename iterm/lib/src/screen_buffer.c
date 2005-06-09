/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include "screen_row.h"
#include "screen_buffer.h"
#include "mbchar.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Constructor of Buffer object.
 * @param row number of rowss in the buffer.  must be 0<
 * @param col number of columns in the rows.  must be 0<
 * @return reference to new buffer oject, if failed to create, return NULL
 */
VTScreenBuffer *VTScreenBuffer_new(int col, int row)
{
  VTScreenBuffer *buf;
  int i;

  if(col <=0 || row <= 0)
      return NULL;
  
  if( (buf = (VTScreenBuffer *)malloc(sizeof(VTScreenBuffer))) == NULL )
  {
    perror("VTScreenBuffer_new\n");
    return NULL;
  }

  if( (buf->rows = (Row **)calloc(sizeof(Row *),row)) == NULL )
  {
    perror("VTScreenBuffer_new\n");
    free(buf);
    return NULL;

  }

  for(i=0;i<row;i++)
  {
    if ( (buf->rows[i] = Row_new(col)) == NULL )
        goto invalid;
  }

  buf->attr = 0;
  buf->num_cols = col;
  buf->num_rows = row;

  return buf;

  invalid:
  VTScreenBuffer_destroy(buf);
  return NULL;
}


/**
 * Destructor of buffer  object.
 * @param *buf reference to Buffer object to destruct
 * rows, and columns will be recursively destructed.
 */
void VTScreenBuffer_destroy(VTScreenBuffer *buf)
{
  if(buf)
  {
    int i;
    for(i=0;i<buf->num_rows;i++)
        Row_destroy(buf->rows[i]);

    if(buf->rows)
        free(buf->rows);
    free(buf);
  }
}


/**
 * adding Char object into specified col/row of the buffer object.
 * @param *buf reference to Buffer object to destruct
 * @param col column of Cell to add character. must be 0<=, <buf->num_cols
 * @param row row of Cell to add character. must be 0<= , <buf->num_rows
 * @param mbchar Char object to add
 * @param bold     attribute of the Cell
 * @param inverse  attribute of the Cell
 * @param underline attribute of the Cell
 * @return 0 if successfully added else 1
 */
int VTScreenBuffer_add_char(VTScreenBuffer *buf, int col, int row,
                            Char *mbchar, int bold, int blink,int inverse,
                            int underline, int foreground, int background,
                            char charset)
{
  if(col < 0 || buf->num_cols <= col ||
     row < 0 || buf->num_rows <= row)
      return 1;

  if(IS_COMBINED(mbchar))
  {
    if(col == 0)
    {
      if(row == 0)
          return 1;
      else
          return Row_add_char(buf->rows[row-1],buf->num_cols-1,
                              mbchar,bold,blink,inverse,underline,
                              foreground,background,charset);
    }
  }

  return Row_add_char(buf->rows[row],col,mbchar,
                      bold,blink,inverse,underline,foreground,background,
                      charset);
}

/**
 * find character start column 
 * @param *buf reference to Buffer object to destruct
 * @param col column of Cell to add character. must be 0<=, <buf->num_cols
 * @param row row of Cell to add character. must be 0<= , <buf->num_rows
 * @return start point of the character or -1 when failed
 */
/* Replaced to Macro
int VTScreenBuffer_find_multicolum_char_start_column(VTScreenBuffer *buf,
                                                     int row,int col)
{
  return Row_find_multicolum_char_start_column(buf->rows[row],col);
}
*/

int static VTScreenBuffer_shiftup_with_margin(VTScreenBuffer *buf,
                                              int shift_start_row,
                                              int shift_end_row,
                                              int num_line)
{

  int x;
  int size = sizeof(struct Row*);
  Row *rows;

  if((rows = (Row *)malloc(size*num_line)) == NULL)
      return -1;

  if(shift_end_row - shift_start_row < num_line)
      num_line = shift_end_row - shift_start_row;
  
  x = shift_end_row - shift_start_row - num_line;
  if(x < 0)
      x =0;
  else
  {
    memcpy(rows,buf->rows+shift_start_row,num_line*size);
    memmove(buf->rows+shift_start_row,
            buf->rows+shift_start_row+num_line,x*size);
    memcpy(buf->rows+shift_start_row+x,rows,num_line*size);
  }

  VTScreenBuffer_clear_rows(buf,
                            shift_start_row+x,
                            shift_end_row);
  free(rows);
  return 0;
}

int static VTScreenBuffer_shiftdown_with_margin(VTScreenBuffer *buf,
                                                int shift_start_row,
                                                int shift_end_row,
                                                int num_line)
{
  int x;
  int size = sizeof(struct Row*);
  Row *rows;

  if((rows = (Row *)malloc(size*num_line)) == NULL)
      return -1;

  if(shift_end_row - shift_start_row < num_line)
      num_line = shift_end_row - shift_start_row;

  x = shift_end_row - shift_start_row - num_line;

  if(x <= 0)
      x = 0;
  else
  {
    memcpy(rows,buf->rows+shift_start_row+x,num_line*size);
    memmove(buf->rows+shift_start_row+num_line,
            buf->rows+shift_start_row,
            x*size);
    memcpy(buf->rows+shift_start_row,rows,num_line*size);
  }

  VTScreenBuffer_clear_rows(buf,
                            shift_start_row,
                            shift_start_row+num_line);
  free(rows);
  return 0;
}

/**
 * shifting rows with margin.
 * @param *buf reference to Buffer object to destruct
 * @param shift_start_row row of start to scroll. must be 0<=, <shift_end_row
 * @param shift_end_row row of end of scroll exclusive. must be shift_start_row <, <=buf->num_rows
 * @param num_line number of line to shit up/down, positive number will shift up, negative number shift down
 * @return 0 if succeeded else 1
 * requires shift_start_row < shift_end_row.
 */
int VTScreenBuffer_shift_row_with_margin(VTScreenBuffer *buf,
                                         int shift_start_row,
                                         int shift_end_row,
                                         int num_line)
{
  if(num_line < 0)
      return VTScreenBuffer_shiftdown_with_margin(buf,
                                                  shift_start_row,
                                                  shift_end_row,
                                                  -num_line);
  else
      return VTScreenBuffer_shiftup_with_margin(buf,
                                                shift_start_row,
                                                shift_end_row,
                                                num_line);
}


/**
 * copy contents of Buffer object recursively.
 * @param src source Buffer object
 * @param dest destination Buffer object
 * @return 0 if succeeded else 1
 * number of rows and number of columns will be shrunk. Caller must allocate memory of destination Buffer object
 */
int VTScreenBuffer_copy(VTScreenBuffer *src,
                        VTScreenBuffer *dest)
{
  int rows;
  int r;

  rows = src->num_rows<dest->num_rows ? src->num_rows : dest->num_rows;
  
  for(r=0;r<rows;r++)
  {
    if(src->rows[r])
    {
      Row_copy(src->rows[r],dest->rows[r]);
    }
  }
  return 0;
}

/**
 * shifting rows.
 * @param *buf reference to Buffer object to destruct
 * @param num_line number of line to shift.  positive number will shift up,
 negative number will shift down the buffer
 * @return 0 if succeeded else 1
 */
int VTScreenBuffer_shift_row(VTScreenBuffer *buf, int num_line)
{
  if(num_line < 0)
      return VTScreenBuffer_shiftdown_with_margin(buf,0,buf->num_rows,
                                                  -num_line);
  else
      return VTScreenBuffer_shiftup_with_margin(buf,0,buf->num_rows,
                                                num_line);
}

/**
 * Compose bytes of specified row
 * @param *buf reference to Buffer object to destruct
 * @param row row number
 * @param col_start start column
 * @param col_end end column exclusive
 * @param str pointer to char array, caller must allocate memory
 * @param str_len length of str
 * @param total_len total bytes length
 * @param total_width total width of character array
 * @return 0 if succeeded else 1
 */
/* Replaced to Macro
int VTScreenBuffer_compose_row_bytes(VTScreenBuffer *buf, int row ,
                                    int col_start, int col_end,
                                     char *str, int str_len,
                                     int *total_len, int *total_width)
{
  return Row_compose_bytes(buf->rows[row],
                           col_start,col_end,str,str_len,
                           total_len,total_width);
}
*/
/**
 * insert specified length of blank cells from specified Cell
 * @param *buf reference to Buffer object to destruct
 * @param row row to insert blanks
 * @param col_start start Cell to insert blank cells
 * @param num number of cells to insret
 * @return 0 if succeeded else 1
 */
/* Replaced to Macro
int VTScreenBuffer_insert_cells(VTScreenBuffer *buf, int row,
                              int col_start, int num)
{
  return Row_insert_cells(buf->rows[row],col_start,num);
}
*/

/**
 * delete specified length of  cell from specified Cell
 * @param *buf reference to Buffer object to destruct
 * @param row row to insert blanks
 * @param col_start start Cell to delete cells
 * @param num number of cells to delte
 * @return 0 if succeeded else 1
 */
/* Replaced to Macro
int VTScreenBuffer_delete_cells(VTScreenBuffer *buf, int row,
                              int col_start, int num)
{
  return Row_delete_cells(buf->rows[row],col_start,num);
}
*/

/**
 * clear rows in the buffer.
 * @param *buf reference to Buffer object to destruct
 * @param start_row starting row to clear
 * @param start_row ending row to clear exclusive
 * @return 0 if succeeded else 1
 */
int VTScreenBuffer_clear_rows(VTScreenBuffer *buf, int start_row, int end_row)
{
  int r;
  for(r=start_row;r<end_row;r++)
  {
    Row_clear(buf->rows[r]);
    Row_set_wrapped(buf->rows[r],NOT_WRAPPED);
  }

  return 0;
}

/**
 * clear cells in specified row in the buffer.
 * @param *buf reference to Buffer object to destruct
 * @param row row to clear cells in
 * @param start_col start column to clear
 * @param end_col end column to clear exclusive
 * @return 0 if succeeded else 1
 */
int VTScreenBuffer_clear_cells(VTScreenBuffer *buf, int row, int start_col,
                               int end_col)
{
  if(buf->rows[row])
  {
    return Row_clear_cells(buf->rows[row],start_col,end_col);
  }
  return 1;
}



/**
 * Resize buffer column
 * @param *buf reference to Buffer object to destruct
 * @param n_cols number of columns
 */
void VTScreenBuffer_resize(VTScreenBuffer *buf, int n_cols)
{
  int i;
  for(i=0;i<buf->num_rows;i++)
  {
    Row_resize(buf->rows[i],n_cols);
  }
  buf->num_cols = n_cols;
}

/**
 * Set specified range as (un)selected
 * @param *buf reference to Buffer object to destruct
 * @param scol start column
 * @param srow start row
 * @param ecol end column exclusive
 * @param erow end row exclusive
 * @return 0 if succeed, else 1
 */
int VTScreenBuffer_set_selection(VTScreenBuffer *buf, int scol, int srow,
                                 int ecol, int erow, int selected)
{
  int r;

  r = srow;

  if(scol < 0 || srow < 0 ||
     buf->num_rows <= erow || buf->num_cols <= ecol)
     return 1;
     
  
  if(srow == erow)
  {
    Row_set_selection(buf->rows[r],scol,ecol,selected);
  }
  else
  {
    Row_set_selection(buf->rows[r],scol,buf->num_cols,selected);
    for(r=r+1;r<erow;r++)
    {
      Row_set_selection(buf->rows[r],0,buf->num_cols,selected);
    }
    Row_set_selection(buf->rows[erow],0,ecol,selected);
  }
  return 0;
}
