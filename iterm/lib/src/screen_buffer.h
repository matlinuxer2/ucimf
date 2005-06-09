/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/**
 * @file screen_buffer.h
 */
#ifndef __SCREEN_BUFFER_H__
#define __SCREEN_BUFFER_H__
#include "screen_row.h"
#include "mbchar.h"

/**
 * @struct _VTScreenBuf
 * Buffer class.
 * Model of Buffer which holds Row.
 * This struct is typedefed to `ScreenBuffer'.
 */
typedef struct _VTScreenBuf
{
  Row **rows;
      /**< rows in buffer */
  int num_rows;
      /**< number of rows in buffer */
  int num_cols;
      /**< number of columns in row */
  unsigned char attr;
      /**< what's this ?? */
}VTScreenBuffer;
/**< typedefed struct of struct _VTScreenBuf */


VTScreenBuffer *VTScreenBuffer_new(int col, int row);
void VTScreenBuffer_destroy(VTScreenBuffer *buf);
int VTScreenBuffer_add_char(VTScreenBuffer *buf, int col, int row,
                            Char *mbchar,int bold, int blink,int inverse,
                            int underline,int foreground, int background,
                            char charset);
int VTScreenBuffer_copy(VTScreenBuffer *buf,VTScreenBuffer *dest);
int VTScreenBuffer_shift_row(VTScreenBuffer *buf, int num_line);
int VTScreenBuffer_shift_row_with_margin(VTScreenBuffer *buf,
                                         int shift_start_row,
                                         int shift_end_row,
                                         int num_line);
void VTScreenBuffer_resize(VTScreenBuffer *buf, int n_cols);

int VTScreenBuffer_clear_rows(VTScreenBuffer *buf, int start_row,
                               int end_row);
int VTScreenBuffer_clear_cells(VTScreenBuffer *buf, int row, int start_col,
                               int enc_col);
int VTScreenBuffer_set_selection(VTScreenBuffer *buf, int scol, int srow,
                                  int ecol, int erow, int selected);
/*
int VTScreenBuffer_compose_row_bytes(VTScreenBuffer *buf, int row ,
                                     int col_start, int col_end,
                                     char *str, int str_max,
                                     int *total_len, int *total_width);
int VTScreenBuffer_find_multicolum_char_start_column(VTScreenBuffer *buf,
                                                     int row,int col);
int VTScreenBuffer_insert_cells(VTScreenBuffer *buf, int row,
                                int col_start, int num);
int VTScreenBuffer_delete_cells(VTScreenBuffer *buf, int row,
                                int col_start, int num);
*/
/*
int VTScreenBuffer_is_char_drawn(VTScreenBuffer *buf, int row, int col);
int VTScreenBuffer_is_multicolumn_start(VTScreenBuffer *buf, int row, int col);
int VTScreenBuffer_is_bold(VTScreenBuffer *buf, int row, int col);
int VTScreenBuffer_is_inverse(VTScreenBuffer *buf, int row, int col);
int VTScreenBuffer_is_underline(VTScreenBuffer *buf, int row, int col);
int VTScreenBuffer_get_foreground(VTScreenBuffer *buf,int row,int col);
int VTScreenBuffer_get_background(VTScreenBuffer *buf,int row,int col);
*/

#define VTScreenBuffer_find_multicolum_char_start_column(buf,row,col) Row_find_multicolum_char_start_column(buf->rows[row],col)

#define VTScreenBuffer_compose_row_bytes(buf, row ,col_start, col_end, str,  str_len, total_len, total_width) Row_compose_bytes(buf->rows[row],col_start,col_end,str,str_len,total_len,total_width,0)

#define VTScreenBuffer_insert_cells(buf, row,col_start,num) Row_insert_cells(buf->rows[row],col_start,num)

#define VTScreenBuffer_delete_cells(buf,row,col_start,num) Row_delete_cells(buf->rows[row],col_start,num)

#define VTScreenBuffer_get_row(buf,r) (buf->rows[r])


#define VTScreenBuffer_is_char_drawn(buf, row, col) Row_is_char_drawn(buf->rows[row],col)
/**<
 * check if character of specified Cell is drawn or not
 * @param *buf reference to Buffer object to destruct
 * @param row row of Cell to check
 * @param col column of Cell to check
 * @return 1 if drawn else 0
 */

#define VTScreenBuffer_is_bold(buf, row, col) Row_is_bold(buf->rows[row],col)
/**<
 * check if attribute of specified Cell is bold or not
 * @param *buf reference to Buffer object to destruct
 * @param row row of Cell to check
 * @param col column of Cell to check
 * @return 1 if bold else 0
 */

#define VTScreenBuffer_is_blink(buf, row, col) Row_is_blink(buf->rows[row],col)
#define VTScreenBuffer_is_inverse(buf, row, col) Row_is_inverse(buf->rows[row],col)
/**<
 * check if attribute of specified Cell is reverse video or not
 * @param *buf reference to Buffer object to destruct
 * @param row row of Cell to check
 * @param col column of Cell to check
 * @return 1 if reverse video else 0
 */

#define VTScreenBuffer_is_underline(buf, row, col) Row_is_underline(buf->rows[row],col)
/**<
 * check if attribute of specified Cell is underlined or not
 * @param *buf reference to Buffer object to destruct
 * @param row row of Cell to check
 * @param col column of Cell to check
 * @return 1 if underlined else 0
 */

#define VTScreenBuffer_is_multicolumn_start(buf, row, col) Row_is_multicolumn_start(buf->rows[row],col)
/**<
 * check if character of specified Cell is start column of the multicolumn character or not
 * @param *buf reference to Buffer object to destruct
 * @param row row of Cell to check
 * @param col column of Cell to check
 * @return 1 if cell is multicolumn character's first cell else 0
 */

#define VTScreenBuffer_get_foreground(buf,row,col) Row_get_foreground(buf->rows[row],col)
/**<
 * get foreground attribute of specified Cell
 * @param *buf reference to Buffer object to destruct
 * @param row row of Cell to check
 * @param col column of Cell to check
 * @return value of foreground
 */

#define VTScreenBuffer_get_background(buf,row,col) Row_get_background(buf->rows[row],col)
/**<
 * get background attribute of specified Cell
 * @param *buf reference to Buffer object to destruct
 * @param row row of Cell to check
 * @param col column of Cell to check
 * @return value of foreground
 */


#define VTScreenBuffer_get_cell_width(buf,row,col) Row_get_cell_width(buf->rows[row],col)
/**<
 * check width of specified Cell
 * @param *buf reference to Buffer object to destruct
 * @param row row of Cell to check
 * @param col column of Cell to check
 * @return width of the character in the Cell,
 *        if this is not multicolumn start cell, returns 1
 */


#define VTScreenBuffer_get_charset(buf,row,col) Row_get_charset(buf->rows[row],col)

#define VTScreenBuffer_is_same_attributes(buf,col1,row1,col2, row2) \
  Row_is_same_attributes(buf->rows[row1],col1,buf->rows[row2],col2)

#endif /* __SCREEN_BUFFER_H__ */
