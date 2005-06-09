/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#if defined(HAVE_CONFIGH_H)
# include "../config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iterm/screen.h"
#include "screen_internal.h"
#include "vt100.h"
#include "tabs.h"

#ifdef BIDI
#include "screen_visual_row.h"
#endif

/* set 1 for non-implementation warnig */
#if defined(DEBUG)
#define WARN(str) fprintf(stderr,"WARNING: %s is not implemented yet\n",str);
#else
#define WARN(str)
#endif

#define TEXT_POINT(scr,c,r) (((r)*scr->columns)+(c))

#define CURRENT_CHARSET(scr) (scr->current_SS ? scr->charset[scr->current_SS] : scr->charset[scr->current_LS])

#define CHECK_CURSOR_X(scr) (scr->cursor_x)<0?(scr->cursor_x)=0:(scr->cursor_x);(scr->cursor_x)>scr->columns-1?scr->cursor_x=scr->columns-1:(scr->cursor_x)
#define CHECK_CURSOR_Y(scr) (scr->cursor_y)<0?(scr->cursor_y)=0:(scr->cursor_y);(scr->cursor_y)>scr->rows-1?scr->cursor_y=scr->rows-1:(scr->cursor_y)

#define CHECK_CURSOR_Y_WITH_MARGIN(scr) (scr->cursor_y)<(scr->margin.top)?(scr->cursor_y)=(scr->margin.top):(scr->cursor_y);(scr->cursor_y)>scr->margin.bottom?scr->cursor_y=scr->margin.bottom:(scr->cursor_y)

#define SCROLL_UP(scr,amt) VTScreen_scroll_buffer(scr,amt)

#define HAS_DRAW_TEXT(scr) (scr->screen_view && scr->screen_view->draw_text)
#define TO_BUFFERED_ROW(scr,vrow) (scr->visual_start_row + (vrow))
#define GET_ASSOCIATED_ROW(scr,vrow) (VTScreenBuffer_get_row(scr->current_buffer,TO_BUFFERED_ROW(scr,vrow)))

#define ARRANGE_POINTS(scr,scol,srow,ecol,erow) \
do { \
  if(TEXT_POINT(scr,ecol,erow) < TEXT_POINT(scr,scol,srow)) { \
    int tmp_x, tmp_y; \
    tmp_x = scol; \
    tmp_y = srow; \
    scol = ecol; \
    srow = erow; \
    ecol = tmp_x; \
    erow = tmp_y; \
  } \
} while(0)


#define VTScreenView_draw_text(scr,col,row,mbs,len,width) \
   scr->screen_view->draw_text(scr->screen_view,col,row,mbs,len,width)

#define VTScreenView_clear_rect(scr,scol,srow,ecol,erow) \
   if((scr->screen_view && scr->screen_view->clear_rect)) \
    scr->screen_view->clear_rect(scr->screen_view,scol,srow,ecol,erow)

#define VTScreenView_set_rendition(scr,bold,blink,inverse,underline,foreground, background,charset) \
  if((scr->screen_view && scr->screen_view->set_rendition)) \
      scr->screen_view->set_rendition(scr->screen_view,bold,blink,inverse,underline,foreground,background,charset)

#define VTScreenView_update_rect(scr) (void)scr;

#define UPDATE_WHOLE_SCREEN(screen) \
   (screen->update_whole_screen = 1)

#define VTScreenView_notify_osc(scr,type,params,n_params) \
  if((scr->screen_view && scr->screen_view->notify_osc)) \
     scr->screen_view->notify_osc(scr->screen_view,type,params,n_params)
  
#define VTScreenView_ring(scr) \
  if((scr->screen_view && scr->screen_view->ring)) \
     scr->screen_view->ring(scr->screen_view)

#define VTScreenView_swap_video(scr) \
  if((scr->screen_view && scr->screen_view->swap_video)) \
     scr->screen_view->swap_video(scr->screen_view)

#define VTScreenView_update_scrollbar(scr) \
  if((scr->screen_view && scr->screen_view->update_scrollbar)) \
     scr->screen_view->update_scrollbar(scr->screen_view,\
                        scr->visual_start_row+scr->rows,\
                        scr->visual_start_row-scr->num_line_shifted,scr->rows)
                                        
#define VTScreenView_update_cursor_position(scr) \
     if((scr->screen_view && scr->screen_view->update_cursor_position)) \
        scr->screen_view->update_cursor_position(scr->screen_view,\
                                                 scr->cursor_x,\
                                                  scr->cursor_y+scr->num_line_shifted)

#define HAS_SCROLL_VIEW(scr) (scr->screen_view && scr->screen_view->scroll_view)
#define VTScreenView_scroll_view(scr,d,s,l) \
do { \
  if(scr->cursor_drawn) VTScreen_undraw_cursor(scr); \
  scr->screen_view->scroll_view(scr->screen_view,d,s,l); \
}while(0)

#define SCROLL_UP_VIEW(scr,l) \
do { \
      if(l == 0) \
          break;  \
      if(l > 0) { \
          VTScreenView_scroll_view(scr,scr->margin.top,scr->margin.top+l,scr->margin.bottom - scr->margin.top-l+1);\
          VTScreenView_clear_rect(scr,0,scr->margin.bottom-l+1,scr->columns,scr->margin.bottom-l+2); \
      } else {\
        VTScreenView_scroll_view(scr,scr->margin.top-l,scr->margin.top,scr->margin.bottom - scr->margin.top+l+1); \
          VTScreenView_clear_rect(scr,0,scr->margin.top,scr->columns,scr->margin.top-l); \
      } \
} while(0)

#define VTScreenView_resize_request(scr,w,h) \
     if((scr->screen_view && scr->screen_view->resize_request)) \
          scr->screen_view->resize_request(scr->screen_view,w,h)


#define DRAW_CURSOR(scr) \
do {\
 if(VTScreen_is_dectcem(scr))\
      VTScreen_draw_cursor(screen);\
} while(0)

#define  UNDRAW_CURSOR(scr) \
do {\
 if(scr->cursor_drawn)\
      VTScreen_undraw_cursor(screen);\
} while(0)



/* prototypes */
static void VTScreen_draw_cursor(VTScreen *screen);
static void VTScreen_undraw_cursor(VTScreen *screen);
static void VTScreen_move_cursor(VTScreen *screen, int col ,int row);
static void VTScreen_redraw_row(VTScreen *screen, int r, int scol, int ecol,
                                int with_clear);

static struct DECMODES default_decmodes = {OFF,ON,OFF,ON,OFF,ON,ON,ON,ON};
static struct ATTRIBUTES default_attributes = {OFF,OFF,OFF,OFF,0,0};
static struct SAVED_CURSOR default_saved_cursor = {0,0,
                                                   {OFF,OFF,OFF,OFF,0,0},
                                                   {ASCII_CHARSET,
                                                    ASCII_CHARSET,
                                                    ASCII_CHARSET,
                                                    ASCII_CHARSET},
                                                   0};

static char *separators[] = { " ","!","\"","#","$","%","&","'","(",")",
                              "*","+",",","-",".","/",":",";","<","=",
                              ">","?","@","[","\\","]","^","_","`","{",
                              "|","}","~",NULL};

static void  VTScreen_reset_selection(VTScreen *);


/* static constants */
  /* to insert empty cell */
static char blank[2] = " ";
  /* for alignment check */
static Char capital_E;

static void VTScreen_update_region(VTScreen *screen , int scol, int srow,
                                   int ecol,int erow)
{
  int i;
  for(i=srow;i<erow;i++)
  {
    Row_set_updated(GET_ASSOCIATED_ROW(screen,i),scol,ecol);
  }
}

/*
  This is only one way save all to alt and restore alt to all.
  But xterm can swap all and alt.  Do I need ?
*/
static void VTScreen_to_alt_buffer(VTScreen *screen)
{
  VTScreen_reset_selection(screen);
  VTScreenBuffer_copy(screen->all_buffer,screen->alt_buffer);
  UPDATE_WHOLE_SCREEN(screen);
}

static void VTScreen_from_alt_buffer(VTScreen *screen)
{
  VTScreen_reset_selection(screen);
  VTScreenBuffer_copy(screen->alt_buffer,screen->all_buffer);
  UPDATE_WHOLE_SCREEN(screen);
}


/*
  If interface implementes scroll_view, then use it
   otherwise redrawing scrolled region
*/
static void VTScreen_scroll_buffer(VTScreen *screen, int amount)
{
  VTScreen_reset_selection(screen);

  if(HAS_SCROLL_VIEW(screen))
  {
    screen->num_of_view_scroll += amount;
  }

  if(screen->margin.top > 0 || screen->margin.bottom < screen->rows - 1)
  {
    VTScreenBuffer_shift_row_with_margin(
        screen->current_buffer,
        TO_BUFFERED_ROW(screen,screen->margin.top),
        TO_BUFFERED_ROW(screen,screen->margin.bottom+1),
        amount);
    if(!HAS_SCROLL_VIEW(screen))
        VTScreen_update_region(screen,0,screen->margin.top,
                               screen->columns,screen->margin.bottom+1);
  }
  else
  {
    if(screen->current_buffer->num_rows <
       TO_BUFFERED_ROW(screen,screen->rows+amount))
    {
      screen->visual_start_row=screen->current_buffer->num_rows-screen->rows;
      VTScreenBuffer_shift_row(screen->current_buffer,amount);
    }
    else if (screen->visual_start_row + amount < 0)
    {
      screen->visual_start_row = 0;
      VTScreenBuffer_shift_row(screen->current_buffer,amount);
    }
    else
    {
      int i;
      screen->visual_start_row += amount;
      if(amount < 0)
      {
        for(i=0;i<-amount;i++)
        {
          VTScreenBuffer_clear_rows(screen->current_buffer,
                                    TO_BUFFERED_ROW(screen,i),
                                    TO_BUFFERED_ROW(screen,i+1));
        }
      }
      else
      {
        for(i=0;i<amount;i++)
        {
          VTScreenBuffer_clear_rows(screen->current_buffer,
                                    TO_BUFFERED_ROW(screen,screen->rows-i-1),
                                    TO_BUFFERED_ROW(screen,screen->rows-i));
        }
      }
    }
    if(!HAS_SCROLL_VIEW(screen))
        UPDATE_WHOLE_SCREEN(screen);
  }
}

#ifdef BIDI
void VTScreen_set_direction(VTScreen *screen, int dir)
{
  if(screen->vtlayout)
  {
    screen->vtlayout->set_global_direction(screen->vtlayout,dir);
    VTScreen_redraw(screen,0,0,
                    screen->columns,screen->rows,1);
  }
}

int VTScreen_is_direction_LTR(VTScreen *screen)
{
  if(screen->bidi_implicit && screen->vtlayout)
      return screen->vtlayout->is_direction_LTR(screen->vtlayout);
  else
      return 1;
}
#endif

/**
 * Scroll screen.
 * @param screen reference to VTScreen object.
 * @param top 0.0 to 1.0 value of screen point in screen buffer
 */
void VTScreen_scroll(VTScreen *screen, double top)
{
  int num = (int)(top * (TO_BUFFERED_ROW(screen,screen->rows)));
  int tmp;

  if(num == screen->visual_start_row  - screen->num_line_shifted)
      return;
  
  if(screen->visual_start_row < num )
      tmp = 0;
  else
      tmp =  screen->visual_start_row - num;

  if(screen->num_line_shifted == tmp)
      return;

  VTScreen_scroll_by_line(screen,tmp - screen->num_line_shifted);
}

/**
 * Scroll screen.
 * @param screen reference to VTScreen object.
 * @param num_line >0 for scroll up, <0 for scroll down
 */
void VTScreen_scroll_by_line(VTScreen *screen, int num_line)
{
  if(screen->visual_start_row < screen->num_line_shifted + num_line)
  {
    num_line = screen->visual_start_row - screen->num_line_shifted ;
    screen->num_line_shifted = screen->visual_start_row;
  }
  else
  {
    screen->num_line_shifted += num_line;
    if(screen->num_line_shifted < 0)
    {
      num_line -= screen->num_line_shifted;
      screen->num_line_shifted  = 0;
    }
  }
  if(num_line == 0)
      return;
  VTScreenView_update_scrollbar(screen);

  if(HAS_SCROLL_VIEW(screen) && num_line < screen->rows )
  {
    if(num_line > 0)
    {
      VTScreenView_scroll_view(screen,num_line,0,
                               screen->rows - num_line);
      VTScreen_redraw(screen,0,0,
                      screen->columns,num_line,1);
    }
    else
    {
      VTScreenView_scroll_view(screen,0,-num_line,
                               screen->rows + num_line);
      VTScreen_redraw(screen,0,screen->rows+num_line,
                      screen->columns,screen->rows,1);
    }
  }
  else
  {
    VTScreen_redraw(screen,
                    0,0,
                    screen->columns,screen->rows,1);
  }
}

/**
 * Set screen attribute as reverse video
 * @param screen reference to VTScreen object.
 * @param on if 1  reverse, 0 set normal.
 */
void VTScreen_set_reverse_video(VTScreen *screen, int on)
{
  VTScreen_set_decscnm(screen,on);
}

/**
 * set VTScreenView
 * @param screen reference to VTScreen object.
 * @param view reference to VTScreenView object.
 */
void VTScreen_set_view(VTScreen *screen, VTScreenView *view)
{
  screen->screen_view = view;
  UPDATE_WHOLE_SCREEN(screen);
  VTScreen_flush(screen);
}

/**
 * Set buffer attribute, that specified string is selected.
 * @param screen reference to VTScreen object.
 * @param scol start column.
 * @param srow start row
 * @param ecol end column inclusive.
 * @param erow end row inclusive.
 * @param selected if 1 select, 0 unselect. \n
 * NOTE. scol,srow/ecol,srow is not needed to coordinate.
 */
static void VTScreen_set_selection_internal(VTScreen *screen,
                                            int scol, int srow,
                                            int ecol, int erow, int selected)
{
  int r;
  Row *row;

  if(scol < 0 || srow < 0 ||
     screen->current_buffer->num_rows < erow ||
     screen->current_buffer->num_cols < ecol )
      return ;

  ARRANGE_POINTS(screen,scol,srow,ecol,erow);

  srow = TO_BUFFERED_ROW(screen,srow) - screen->num_line_shifted;
  erow =TO_BUFFERED_ROW(screen,erow) - screen->num_line_shifted;
  r =   srow;
  row = VTScreenBuffer_get_row(screen->current_buffer,r);

  if(srow == erow)
  {
#ifdef BIDI
    if(screen->bidi_implicit && screen->vtlayout)
    {
      row = VisualRow_get_visual_row(screen->vrow,row,screen->vtlayout);
      VisualRow_set_selection(screen->vrow,scol,ecol,selected);
    }
    else
#endif    
    Row_set_selection(row,scol,ecol,selected);
  }
  else
  {
#ifdef BIDI
    if(screen->bidi_implicit && screen->vtlayout)
    {
      row = VisualRow_get_visual_row(screen->vrow,row,screen->vtlayout);
      VisualRow_set_selection(screen->vrow,scol,
                              screen->current_buffer->num_cols,selected);
    }
    else
#endif 
    Row_set_selection(row,scol,
                      screen->current_buffer->num_cols,selected);
    for(r=r+1;r<erow;r++)
    {
      row = VTScreenBuffer_get_row(screen->current_buffer,r);
      Row_set_selection(row,0,
                        screen->current_buffer->num_cols,selected);
    }
    row = VTScreenBuffer_get_row(screen->current_buffer,erow);

#ifdef BIDI
    if(screen->bidi_implicit && screen->vtlayout)
    {
      VisualRow_get_visual_row(screen->vrow,row,screen->vtlayout);
      VisualRow_set_selection(screen->vrow,0,ecol,selected);
    }
    else
#endif
    Row_set_selection(row,0,ecol,selected);
  }
  
}

static void VTScreen_reset_selection(VTScreen *screen)
{
  if(screen->selection.region.scol < 0 ||
     screen->selection.region.ecol < 0 )
      return ;
  VTScreen_set_selection_internal(screen, 
                                  screen->selection.region.scol,
                                  screen->selection.region.srow,
                                  screen->selection.region.ecol,
                                  screen->selection.region.erow,
                                  0);

  ARRANGE_POINTS(screen,
                 screen->selection.region.scol,screen->selection.region.srow,
                 screen->selection.region.ecol,screen->selection.region.erow);

  VTScreen_update_region(screen,
                         screen->selection.region.scol,
                         screen->selection.region.srow,
                         screen->selection.region.ecol,
                         screen->selection.region.erow);

  screen->selection.region.scol = 
 screen->selection.region.ecol = -1;
}

/**
 * Clear previously selected string.
 * @param screen reference to VTScreen object.
 */
void VTScreen_clear_selection(VTScreen *screen)
{
  ARRANGE_POINTS(screen,
                 screen->selection.region.scol,screen->selection.region.srow,
                 screen->selection.region.ecol,screen->selection.region.erow);
  VTScreen_set_selection_internal(screen,
                                  screen->selection.region.scol,
                                  screen->selection.region.srow,
                                  screen->selection.region.ecol,
                                  screen->selection.region.erow,
                                  0);
  
  if(screen->selection.region.srow == screen->selection.region.erow)
      VTScreen_redraw(screen,
                      screen->selection.region.scol,
                      screen->selection.region.srow,
                      screen->selection.region.ecol+1,
                      screen->selection.region.erow+1,0);
  else
      VTScreen_redraw(screen,
                      0,screen->selection.region.srow,
                      screen->columns,screen->selection.region.erow+1,0);
  
  screen->selection.region.scol =
 screen->selection.region.ecol = -1;
}

static int is_separator(char *ch, char *dummy)
{
  char **sep;
  sep = separators;
  while(*sep != NULL)
  {
    if( strcmp(ch,*sep) == 0)
        return 1;
    sep++;
  }
  return 0;
}

static int is_not_same_char(char *ch, char *sep)
{
  return (strcmp(ch,sep) != 0);
}

static int VTScreen_extend_selection_left(VTScreen *screen, int c, int r)
{
  int col = c;
  Row *row = GET_ASSOCIATED_ROW(screen,r);
  char character[128]; /* enough ? */
  char first_char[128]; /* enough ? */
  int length,width;
  int precol = col;
  int (*until)(char *,char *);

#ifdef BIDI
    if(screen->bidi_implicit && screen->vtlayout)
    {
      row = VisualRow_get_visual_row(screen->vrow,row,screen->vtlayout);
    }
#endif    
  switch(screen->selection.type)
  {
    case SELECTION_LINE:
        return 0;
    case SELECTION_WORD:
        if(!Row_is_char_drawn(row,col))
            return col;
        while(!Row_is_multicolumn_start(row,col))
          col--;

        Row_compose_bytes(row,col,col+Row_get_cell_width(row,col),
                          first_char,sizeof(first_char),&length,&width,0);
        first_char[length] = '\0';
        if(is_separator(first_char,NULL))
            until = is_not_same_char;
        else
            until = is_separator;
      while(0 < col)
      {
        if(!Row_is_char_drawn(row,col))
            return precol;
        if(!Row_is_multicolumn_start(row,col))
        {
          col--;
          continue;
        }
        Row_compose_bytes(row,col,col+Row_get_cell_width(row,col),
                          character,sizeof(character),&length,&width,0);
        character[length] = '\0';
        if(until(character,first_char))
              return precol;
        precol = col;
        col--;
      }
      return col;
    default:
    case SELECTION_CHAR:
        return c;
  }
}

static int VTScreen_extend_selection_right(VTScreen *screen, int c, int r)
{
  int col = c;
  Row *row = GET_ASSOCIATED_ROW(screen,r);
  char character[128]; /* enough ? */
  char first_char[128]; /* enough ? */
  int length,width;
  int precol = col;
  int (*until)(char *,char *);
  
#ifdef BIDI
    if(screen->bidi_implicit && screen->vtlayout)
    {
      row = VisualRow_get_visual_row(screen->vrow,row,screen->vtlayout);
    }
#endif    
  switch(screen->selection.type)
  {
    case SELECTION_LINE:
        return screen->columns-1;
    case SELECTION_WORD:
          if(!Row_is_char_drawn(row,col))
              return col;
          while(!Row_is_multicolumn_start(row,col))
            col--;
          Row_compose_bytes(row,col,col+Row_get_cell_width(row,col),
                            first_char,sizeof(first_char),&length,&width,0);
          first_char[length] = '\0';
          if(is_separator(first_char,NULL))
              until = is_not_same_char;
          else
              until = is_separator;

        while(col < row->num_columns)
        {
          if(!Row_is_char_drawn(row,col))
              return col;
          if(!Row_is_multicolumn_start(row,col))
          {
            col++;
            continue;
          }
          Row_compose_bytes(row,col,col+Row_get_cell_width(row,col),
                            character,sizeof(character),&length,&width,0);
          character[length] = '\0';

          if(until(character,first_char))
                return precol;
          precol = col;
          col++;
        }
        return precol;
    default:
    case SELECTION_CHAR:
        return c;
  }
}


void VTScreen_start_selection(VTScreen *screen, int col, int row, int type)
{
  Row *r = GET_ASSOCIATED_ROW(screen,row);
  if(col < 0 || screen->columns < col ||
     row < 0 || screen->rows < row )
  {
    screen->selection.region.scol =
    screen->selection.region.ecol = -1;
    return;
  }
  if(Row_is_char_drawn(r,col) &&
     !Row_is_multicolumn_start(r,col))
      col--;
  screen->selection.type = type;
  screen->selection.region.scol = col;
  screen->selection.region.srow = row;
  screen->selection.region.ecol = -1; /* mark for extend_selection below */
  screen->selection.region.erow = row; 
  if(type == SELECTION_WORD || type == SELECTION_LINE)
  {
    screen->selection.region.scol =
 VTScreen_extend_selection_left(screen,col,row);
    VTScreen_extend_selection(screen,
                              VTScreen_extend_selection_right(screen,col,row),
                              row);
  }
}

void VTScreen_extend_selection(VTScreen *screen, int col, int row)
{
  int scol,srow,ecol,erow;
  int start_point, end_point, current_point;
  Row *r;

  if(screen->selection.region.scol < 0 )
      return ;

  col = col < 0 ? 0 : col;
  row = row < 0 ? 0 : row;
  col = screen->columns < col ? screen->columns : col;
  row = screen->rows <= row ? screen->rows -1: row;
  
  r = GET_ASSOCIATED_ROW(screen,row);
  start_point = TEXT_POINT(screen,
                           screen->selection.region.scol,
                           screen->selection.region.srow);
  end_point = TEXT_POINT(screen,
                           screen->selection.region.ecol,
                           screen->selection.region.erow);

  if(screen->selection.type == SELECTION_WORD ||
     screen->selection.type == SELECTION_LINE)
  {
    current_point = TEXT_POINT(screen,col,row);
  
    if(start_point < current_point)
      col = VTScreen_extend_selection_right(screen,col,row) + 1;
    else if(current_point < start_point)
      col = VTScreen_extend_selection_left(screen,col,row);
  }

      /* adjust column position of multicolumn character  */
  if(Row_is_char_drawn(r,col) &&
     !Row_is_multicolumn_start(r,col) )
      col++;

  current_point = TEXT_POINT(screen,col,row);
  if(screen->selection.region.ecol < 0)
  {
    VTScreen_set_selection_internal(screen,
                                    screen->selection.region.scol,
                                    screen->selection.region.srow,
                                    col, row,ON);
  }
  else if( (start_point <= end_point && end_point < current_point) ||
           (current_point < end_point &&  end_point <= start_point) )
  {
    VTScreen_set_selection_internal(screen,
                                    screen->selection.region.ecol,
                                    screen->selection.region.erow,
                                    col,row,1);
  }
  else if( (start_point < current_point && current_point < end_point) ||
           (end_point < current_point && current_point < start_point) )
  {
    VTScreen_set_selection_internal(screen,
                                    screen->selection.region.ecol,
                                    screen->selection.region.erow,
                                    col,row,0);
  }
  else if( (current_point < start_point && start_point < end_point) ||
           ( end_point < start_point && start_point < current_point) )
  {
        /* the other way around */
    if(screen->selection.type == SELECTION_WORD ||
       screen->selection.type == SELECTION_LINE)
    {
          /* keep selection start point */
      if(start_point < current_point)
          screen->selection.region.scol =
      VTScreen_extend_selection_left(screen,
                                     screen->selection.region.scol -1,
                                     screen->selection.region.srow);
      else
          screen->selection.region.scol =
      VTScreen_extend_selection_right(screen,
                                      screen->selection.region.scol,
                                      screen->selection.region.srow) + 1;
    }

    VTScreen_set_selection_internal(screen,
                                    screen->selection.region.scol,
                                    screen->selection.region.srow,
                                    screen->selection.region.ecol,
                                    screen->selection.region.erow,OFF);

    VTScreen_set_selection_internal(screen,
                                    screen->selection.region.scol,
                                    screen->selection.region.srow,
                                    col,row,ON);
  }
  else
    return;

  scol = screen->selection.region.ecol;
  srow = screen->selection.region.erow;
  ecol = screen->selection.region.ecol = col;
  erow = screen->selection.region.erow = row;
  ARRANGE_POINTS(screen,scol,srow,ecol,erow);
  if(srow == erow)
      VTScreen_redraw_row(screen,srow,scol,ecol+1,0);
  else
      VTScreen_redraw(screen,0,srow,screen->columns,erow+1,0);
}

/**
 * Copy buffer.
 * @param screen reference to VTScreen object.
 * @param scol start column.
 * @param srow start row
 * @param ecol end column inclusive.
 * @param erow end row inclusive.
 * @param selected if 1 select, 0 unselect. \n
 * NOTE. scol,srow/ecol,srow is not needed to coordinate.
 */
void VTScreen_copy_buffer(VTScreen *screen, int scol, int srow,
                          int ecol, int erow, char *dest, int dest_size)
{
  int length;
  int width;
  Row *row;
  scol = scol < 0 ? 0 : scol;
  scol = screen->columns < scol ? screen->columns : scol;
  ecol = ecol < 0 ? 0 : ecol;
  ecol = screen->columns < ecol ? screen->columns : ecol;

  srow = srow < 0 ? 0 : srow;
  srow = screen->rows < srow ? screen->rows-1 : srow;
  erow = erow < 0 ? 0 : erow;
  erow = screen->rows < erow ? screen->rows-1 : erow;

  
  if(TEXT_POINT(screen,ecol,erow) < TEXT_POINT(screen,scol,srow))
  {
    int tmp_x, tmp_y;
    tmp_x = scol;
    tmp_y = srow;
    scol = ecol;
    srow = erow;
    ecol = tmp_x;
    erow = tmp_y;
  }

  row = GET_ASSOCIATED_ROW(screen,srow-screen->num_line_shifted);
  if(srow == erow) /* only single row */
  {
#ifdef BIDI
    if(screen->bidi_implicit && screen->vtlayout)
    {
      VisualRow_get_visual_row(screen->vrow,row,screen->vtlayout);
      VisualRow_copy_logical_buffer(screen->vrow,scol,ecol,
                                    dest,dest_size,&length,&width);
    }
    else
#endif    
        Row_compose_bytes(row,scol,ecol,dest,dest_size, &length,&width,0);
    if(dest_size == length)
        length--;
    dest[length] = '\0';
  }
  else /* multiple rows */
  {
    int shifted = TO_BUFFERED_ROW(screen,-screen->num_line_shifted);
    int r = shifted+srow;
    int total_length;

#ifdef  BIDI
    if(screen->bidi_implicit && screen->vtlayout)
    {
      VisualRow_get_visual_row(screen->vrow,row,screen->vtlayout);
      VisualRow_copy_logical_buffer(screen->vrow,scol,
                                    screen->current_buffer->num_cols,
                                    dest,dest_size,&length,&width);
    }
    else
#endif
    Row_compose_bytes(row,scol,screen->current_buffer->num_cols,
                      dest,dest_size,&length,&width,0);

    total_length = length;
    for(r=r+1;r<shifted+erow;r++)
    {
      if(!Row_is_wrapped(row) && total_length < dest_size)
      {
        dest[total_length] = '\n';
        total_length++;
      }
      row = VTScreenBuffer_get_row(screen->current_buffer,r);
      Row_compose_bytes(row,0,screen->current_buffer->num_cols,
                            dest+total_length,dest_size-total_length,
                            &length,&width,0);
      total_length+=length;
    }

    if(!Row_is_wrapped(row) && total_length < dest_size)
    {
      dest[total_length] = '\n';
      total_length++;
    }
    row = VTScreenBuffer_get_row(screen->current_buffer,r);
#ifdef  BIDI
    if(screen->bidi_implicit && screen->vtlayout)
    {
      VisualRow_get_visual_row(screen->vrow,row,screen->vtlayout);
      VisualRow_copy_logical_buffer(screen->vrow,0,ecol,
                                    dest+total_length,dest_size,&length,&width);

    }
    else
#endif    
    Row_compose_bytes(row,0,ecol,dest+total_length,
                      dest_size-total_length,&length,&width,0);
    total_length+=length;
    if(dest_size == total_length)
        total_length--;
    dest[total_length] = '\0';
  }
}

void VTScreen_copy_selected_buffer(VTScreen *screen, char *dest, int dest_size)
{
  VTScreen_copy_buffer(screen,
                       screen->selection.region.scol,
                       screen->selection.region.srow,
                       screen->selection.region.ecol,
                       screen->selection.region.erow,
                       dest,dest_size);
}

/**
 * re-setting screen size.
 * @param screen reference to VTScreen object
 * @param cols number of columns
 * @param rows number of rows
 */
void VTScreen_set_size(VTScreen *screen,int cols, int rows)
{
  int abs_cursor = screen->visual_start_row + screen->cursor_y;

  VTScreen_reset_selection(screen);
      /* try to keep cursor position */
  if(abs_cursor < rows)
  {
    screen->visual_start_row = 0 ;
    screen->cursor_y = abs_cursor;
  }
  else
  {
    if(screen->rows < rows)
    {
      screen->visual_start_row = screen->visual_start_row + screen->rows - rows;
      if(screen->visual_start_row < 0)
          screen->visual_start_row = 0;
      
      screen->cursor_y = abs_cursor - screen->visual_start_row;
    }
    else /* rows <= screen->rows */
    {
      if(rows < screen->cursor_y)
      {
        screen->cursor_y = rows - 1;
        screen->visual_start_row = abs_cursor - screen->cursor_y;
      }
    }
  }
  screen->rows = rows;
  CHECK_CURSOR_Y(screen);
  screen->margin.top = 0;
  screen->margin.bottom = rows-1;

  if(screen->columns == cols && rows < screen->current_buffer->num_rows )
      return;
  
  screen->columns = cols;
  VTScreenBuffer_resize(screen->all_buffer,cols);
  VTScreenBuffer_resize(screen->alt_buffer,cols);
  CHECK_CURSOR_X(screen);
}

static void VTScreen_flush_scroll(VTScreen *screen)
{
  UNDRAW_CURSOR(screen);
  if(screen->num_of_view_scroll &&
     abs(screen->num_of_view_scroll) <= screen->margin.bottom - screen->margin.top)
  {
    SCROLL_UP_VIEW(screen,screen->num_of_view_scroll);
  }
  screen->num_of_view_scroll = 0;
  VTScreenView_update_scrollbar(screen);
}

/**
 * flushing a buffer and notify ScreenView to redraw.
 * @param screen reference to VTScreen object
 * This function call after all data from Host has got empty.
 * Only this function calls redraw method of VTScreenView Object.
 */
void VTScreen_flush(VTScreen *screen)
{
  int i;
  Row *row;

  if(screen->num_line_shifted != 0)
  {
    screen->num_line_shifted = 0;
    UPDATE_WHOLE_SCREEN(screen);
  }

  if(screen->update_whole_screen)
  {
    VTScreen_redraw(screen,0,0,screen->columns,screen->rows,1);
    screen->update_whole_screen = 0;
    screen->num_of_view_scroll = 0;
    VTScreenView_update_scrollbar(screen);
  }
  else
  {
    VTScreen_flush_scroll(screen);
    for(i=0;i<screen->rows;i++)
    {
      row = GET_ASSOCIATED_ROW(screen,i);
      if(Row_is_updated(row))
      {
#ifdef BIDI    
        if(screen->bidi_implicit && screen->vtlayout)
            VTScreen_redraw_row(screen,i,0,screen->columns,1);
        else
#endif           
            VTScreen_redraw_row(screen,i,
                                Row_get_updated_first(row),
                                Row_get_updated_last(row),1);
        Row_clear_updated(row);
      }
    }
  }
  DRAW_CURSOR(screen);
}

static void VTScreen_move_cursor(VTScreen *screen, int col ,int row)
{
  screen->cursor_x = col;
  screen->cursor_y = row;
  CHECK_CURSOR_X(screen);
  CHECK_CURSOR_Y(screen);
  screen->do_wrap = 0;
  VTScreen_reset_selection(screen);
}

static void VTScreen_draw_cursor(VTScreen *screen)
{
  int displayed_y = screen->cursor_y + screen->num_line_shifted;

  if(displayed_y < 0 || screen->rows <= displayed_y)
  {
    screen->cursor_drawn = 0;
    return ;
  }

  screen->cursor_drawn = 1;
  screen->drawn_cursor_x = screen->cursor_x;
  screen->drawn_cursor_y = displayed_y;

  VTScreenView_update_cursor_position(screen);  
  if(HAS_DRAW_TEXT(screen))
  {
    int length;
    int width;
    int x = screen->cursor_x;
    Row *row = GET_ASSOCIATED_ROW(screen,screen->cursor_y);
    screen->drawn_cursor_row = row;
#ifdef BIDI    
    if(screen->bidi_implicit && screen->vtlayout)
    {
        row = VisualRow_get_visual_row(screen->vrow,row,screen->vtlayout);
        x = VisualRow_translate_logical_position(screen->vrow,x);
    }
#endif
    if(Row_is_char_drawn(row,x))
    {
          /* character is already drawn*/
#define SIZE 16      
      char buffer[SIZE]; /* enough? */
          /* Skip part of multi column character */
      x = Row_find_multicolum_char_start_column(row,x);
      
      if(x < 0) /* something wrong */
          return;
      Row_compose_bytes(row,x,x+Row_get_cell_width(row,x),
                        buffer,SIZE,&length,&width,0);
#undef SIZE      
      
      VTScreenView_set_rendition(screen,
                                 Row_is_bold(row,x),
                                 Row_is_blink(row,x),
                                 !Row_is_inverse(row,x),
                                 Row_is_underline(row,x),
                                 0, /* default foreground */
                                 0, /* default background */
                                 Row_get_charset(row,x));

      VTScreenView_draw_text(screen,x,displayed_y,buffer,length,width);
    }
    else
    {
          /* set inverse */

      VTScreenView_set_rendition(screen,
                                 screen->current_attr.bold,
                                 screen->current_attr.blink,
                                 1,
                                 screen->current_attr.underline,
                                 screen->current_attr.foreground,
                                 screen->current_attr.background,
                                 ASCII_CHARSET);
      VTScreenView_draw_text(screen,x,displayed_y,blank,1,1);
    }
  }
}

static void VTScreen_undraw_cursor(VTScreen *screen)
{
  screen->cursor_drawn = 0;
  if( (screen->columns < screen->drawn_cursor_x) ||
      (screen->rows <= screen->drawn_cursor_y) ||
      (screen->drawn_cursor_x < 0) ||
      (screen->drawn_cursor_y < 0))
  {
    return ;
  }

  if(HAS_DRAW_TEXT(screen))
  {
    int length;
    int width;
    int x = screen->drawn_cursor_x;
    Row *row = screen->drawn_cursor_row;

        /*
    if(screen->margin.top <= screen->drawn_cursor_y &&
       screen->drawn_cursor_y <= screen->margin.bottom)
    {
      logical_y -= screen->num_of_view_scroll;
      if(logical_y < 0 || screen->rows <= logical_y)
          return;
    }
        */
#ifdef BIDI
    if(screen->bidi_implicit && screen->vtlayout)
    {
      row = VisualRow_get_visual_row(screen->vrow,row,screen->vtlayout);
      x = VisualRow_translate_logical_position(screen->vrow,x);
    }
#endif
    
    if(Row_is_char_drawn(row,x))
    {
#define SIZE 16
      char buffer[SIZE];
          /* Skip part of multi column character */
      x = Row_find_multicolum_char_start_column(row,x);
      if(x < 0 )
          return;
      Row_compose_bytes(row,x,x+Row_get_cell_width(row,x),
                        buffer,SIZE,&length,&width,0);
#undef  SIZE

      VTScreenView_set_rendition(screen,
                                 Row_is_bold(row,x),
                                 Row_is_blink(row,x),
                                 Row_is_inverse(row,x),
                                 Row_is_underline(row,x),
                                 Row_get_foreground(row,x),
                                 Row_get_background(row,x),
                                 Row_get_charset(row,x));
      VTScreenView_draw_text(screen,x,screen->drawn_cursor_y,
                             buffer,length,width);
    }
    else
    {
          /* set normal */
      VTScreenView_set_rendition(screen,
                                 screen->current_attr.bold,
                                 screen->current_attr.blink,
                                 0,
                                 screen->current_attr.underline,
                                 screen->current_attr.foreground,
                                 screen->current_attr.background,
                                 ASCII_CHARSET);
      VTScreenView_draw_text(screen,x,screen->drawn_cursor_y,blank,1,1);
    }
  } 
}

/**
 * Destructor of VTScreen object.
 * @param screen reference to VTScreen object
 */
void VTScreen_destroy(VTScreen *screen)
{
  if(screen)
  {
    VTScreenBuffer_destroy(screen->current_buffer);
    VTScreenBuffer_destroy(screen->alt_buffer);
#ifdef BIDI  
    if(screen->vrow)
        VisualRow_destroy(screen->vrow);
    if(screen->vtlayout)
        VTLayout_destroy(screen->vtlayout);
#endif  
    free(screen);
  }
}

void static VTScreen_redraw_row(VTScreen *screen, int displayed_r,
                                int start_col, int end_col,
                                int with_clear_rect)
{
  int c,start;
#define SIZE 4096
  char buffer[SIZE]; /* big enough? */
  int width;
  int length;
  Row *row;
  row = VTScreenBuffer_get_row(screen->current_buffer,
                               TO_BUFFERED_ROW(screen,displayed_r)
                               - screen->num_line_shifted);
#ifdef BIDI    
  if(screen->bidi_implicit && screen->vtlayout)
      row = VisualRow_get_visual_row(screen->vrow,row,screen->vtlayout);
#endif    
  if(row)
  {
    c=start_col;
    while( end_col < row->num_columns &&
          !Row_is_multicolumn_start(row,end_col))
    {
      end_col++; /* expand column for multi column char */
    }
    
    while(c<end_col)
    {
      start = c;
      if(Row_is_char_drawn(row,c))
      {
            /* adjust start column */
        if(!Row_is_multicolumn_start(row,c))
            start = Row_find_multicolum_char_start_column(row,c);
        
        if(start < 0)
            start = 0;

        do {
          c++; /* while character attributes are same */
        } while(c < end_col &&
                Row_is_same_attributes(row,c-1,row,c));
        Row_compose_bytes(row,start,c,buffer,SIZE,&length,&width,0); 
#undef SIZE
        VTScreenView_set_rendition(screen,
                                   Row_is_bold(row,start),
                                   Row_is_blink(row,start),
                                   Row_is_inverse(row,start),
                                   Row_is_underline(row,start),
                                   Row_get_foreground(row,start),
                                   Row_get_background(row,start),
                                   Row_get_charset(row,start));
        VTScreenView_draw_text(screen,start,displayed_r,
                               buffer,length,width);
      } else {
            /* if character is not in the cell, clear the cell */
        c++;
        if(with_clear_rect)
        {
          int i=0;
          while(c<end_col && !Row_is_char_drawn(row,c))
          {
            c++;
            i++;
          } 
          VTScreenView_clear_rect(screen,
                                  start,displayed_r,
                                  c, displayed_r+1);
        }
      }
    }
  }
}

/**
 * redraw specified rectangular.
 * @param screen reference to VTScreen object
 * @param start_col top left column
 * @param start_row top left row
 * @param end_col bottom right column
 * @param end_row bottom right row
 */
void VTScreen_redraw(VTScreen *screen, int start_col, int start_row,
                     int end_col, int end_row, int with_clear_rect)
{
  int displayed_r;

  start_col = start_col < 0 ? 0  : start_col;
  start_row = start_row < 0 ? 0 : start_row;
  end_col = screen->columns < end_col ? screen->columns : end_col;
  end_row = screen->rows < end_row ? screen->rows : end_row;

/*  UNDRAW_CURSOR(screen); */
      
  for(displayed_r=start_row; displayed_r<end_row; displayed_r++)
  {
    VTScreen_redraw_row(screen,displayed_r,start_col,end_col,with_clear_rect);
  }
  DRAW_CURSOR(screen);

      /* restore rendition (do I need ?)*/
   VTScreenView_set_rendition(screen,
                              screen->current_attr.bold,
                              screen->current_attr.blink,
                              screen->current_attr.inverse,
                              screen->current_attr.underline,
                              screen->current_attr.foreground,
                              screen->current_attr.background,
                              CURRENT_CHARSET(screen));
}

static void VTScreen_reset(VTScreen *screen)
{
      /* initializing */
  TABS_init_tabs(screen->tabstop);
  screen->visual_start_row = 0;
  screen->cursor_x = 0;
  screen->cursor_y = 0;
  
  screen->linefeed = 0;
#ifdef BIDI  
  screen->bidi_implicit = 1;
#endif  
  screen->insert = 0;
  screen->margin.top = 0;
  screen->margin.bottom = screen->rows - 1;
  
      /* do not force wrap */
  screen->do_wrap = 0;

      /* current attributes */
  screen->current_attr = default_attributes;
      /* DEC MODES */
  screen->decmodes = default_decmodes;
  screen->deckpam = 0;

      /* Saved Cursor */
  screen->saved_cursor = default_saved_cursor;

  screen->selection.region.scol =
 screen->selection.region.srow =
 screen->selection.region.ecol =
 screen->selection.region.erow = 0;
  screen->selection.type = 0;
  
  screen->drawn_cursor_x = 0;
  screen->drawn_cursor_y = 0;
  screen->cursor_drawn = 0;

  screen->num_line_shifted = 0;

  screen->charset[0] =
 screen->charset[1] =
 screen->charset[2] =
 screen->charset[3] = ASCII_CHARSET;

  screen->current_LS = 0;
  screen->current_SS = 0; /* no shift */

  screen->track_mouse.region.scol =
 screen->track_mouse.region.srow =
 screen->track_mouse.region.ecol =
 screen->track_mouse.region.erow =
 screen->track_mouse.type = 0;

  screen->num_of_view_scroll = 0;
}

/**
 * VTScreen Constructor.
 * @param n_cols number of columns
 * @param n_rows number of rows
 * @param n_hist number of history, shoud be n_rows<=
 */
VTScreen *VTScreen_new(int n_cols, int n_rows,int n_hist)
{
  VTScreen *screen;

  if((screen = malloc(sizeof(VTScreen))) == NULL)
  {
    perror("VTScreen_new");
    return NULL;
  }

  screen->history_size = n_hist;
  if((screen->all_buffer = VTScreenBuffer_new(n_cols,n_hist)) == NULL)
  {
    perror("VTScreen_new");
    free(screen);
    return NULL;
  }
  screen->current_buffer = screen->all_buffer;


  if((screen->alt_buffer = VTScreenBuffer_new(n_cols,n_hist)) == NULL)
  {
    perror("VTScreen_new");
    VTScreenBuffer_destroy(screen->all_buffer);
    free(screen);
    return NULL;
  }

  screen->screen_view = NULL;

  screen->rows = n_rows;
  screen->columns = n_cols;


#ifdef BIDI
  screen->vrow = VisualRow_new();
  screen->vtlayout = VTLayout_new();
#endif
      /* initializing */
  VTScreen_reset(screen);

  Char_init(capital_E,(unsigned char *)"E",1,1,1);
  
  return screen;
}


/* move cursor downwards setting cursor on specified column position
   scroll screen up if needed  */
static void VTScreen_index(VTScreen *screen,int column_position)
{
  int new_y;
  new_y = screen->cursor_y+1;
  if(screen->margin.bottom < new_y)
  {
    SCROLL_UP(screen,1);
    new_y = screen->margin.bottom;
  }
  VTScreen_move_cursor(screen,column_position,new_y);
}

/*
  printout.  store a character into buffer
  PRINTABLE character of Ground state
 */
static void VTScreen_write(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;

      /* do wrap if needed */
  if(VTScreen_is_decawm(screen) &&
     ( screen->do_wrap ||
       screen->columns < screen->cursor_x + Char_get_width(ch) ))
  {
    screen->do_wrap = 0;
    Row_set_wrapped(screen->current_buffer->rows[TO_BUFFERED_ROW(screen,screen->cursor_y)],WRAPPED);
    VTScreen_index(screen,0);
  }

  if(screen->insert)
  {
    VTScreenBuffer_insert_cells(screen->current_buffer,
                                TO_BUFFERED_ROW(screen,screen->cursor_y),
                                screen->cursor_x,
                                Char_get_width(ch));
  }
  VTScreenBuffer_add_char(screen->current_buffer,
                          screen->cursor_x,
                          TO_BUFFERED_ROW(screen,screen->cursor_y),
                          ch,
                          screen->current_attr.bold,
                          screen->current_attr.blink,
                          screen->current_attr.inverse,
                          screen->current_attr.underline,
                          screen->current_attr.foreground,
                          screen->current_attr.background,
                          CURRENT_CHARSET(screen));
  screen->current_SS = 0;

  screen->cursor_x += Char_get_width(ch);
  if(VTScreen_is_decawm(screen) &&  screen->columns <= screen->cursor_x)
  {
    CHECK_CURSOR_X(screen);
    screen->do_wrap = 1;
  }
}

/* common methods */
/* ring the bell ^G (anystate excepts OSC) */
static void VTScreen_BEL(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  VTScreenView_ring(screen);
}

/* Backspace ^H */
static void VTScreen_BS(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  VTScreen_move_cursor(object,screen->cursor_x -1 ,screen->cursor_y);
}

/* Carriage Return ^M  */
static void VTScreen_CR(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  VTScreen_move_cursor(screen,0,screen->cursor_y);
}

static int next_tab(VTScreen *screen, int col)
{
  int next;
  next= TABS_get_next_tab(screen->tabstop, col);
  if(VTScreen_is_decawm(screen) && screen->columns <= next)
  {
    if(screen->do_wrap)
    {
      next = 0;
      screen->cursor_y ++;
      screen->do_wrap = 0;
    }
    else
    {
      screen->do_wrap = 1;
    }
  }
  return next;
}
/* Horizontal Tab ^I or TAB */
static void VTScreen_HT(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int next;
  next = next_tab(screen,screen->cursor_x);
  VTScreen_move_cursor(screen,next,screen->cursor_y);
}

/* Shift IN ^O */
static void VTScreen_SI(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  screen->current_LS = 0;
}

/* Shift OUT ^N */
static void VTScreen_SO(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  screen->current_LS = 1;
}

  /* New Line ^J */
static void VTScreen_NL(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int new_x;

  new_x = screen->cursor_x;
  if(screen->linefeed)
      new_x = 0;
  VTScreen_index(screen,new_x);
}

/* Vertical Tab ^K */
static void VTScreen_VT(void *object, Char *ch, int *params, int n_params)
{
  VTScreen_index((VTScreen *)object,((VTScreen *)object)->cursor_x);
}

/* Form Feed ^L */
static void VTScreen_NP(void *object, Char *ch, int *params, int n_params)
{
  VTScreen_index((VTScreen *)object,((VTScreen *)object)->cursor_x);
}
/* End of common methods */

  /* ESC state */
/* Save Cursor ESC 7 */
static void VTScreen_DECSC(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  screen->saved_cursor.cursor_x = screen->cursor_x;
  screen->saved_cursor.cursor_y = screen->cursor_y;
  screen->saved_cursor.attr = screen->current_attr;
  memcpy(screen->saved_cursor.charset,screen->charset,4);
  screen->saved_cursor.ls = screen->current_LS;
}

/* Restore Cursor ESC 8 */
static void VTScreen_DECRC(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  screen->cursor_x = screen->saved_cursor.cursor_x;
  screen->cursor_y = screen->saved_cursor.cursor_y;
  screen->current_attr = screen->saved_cursor.attr;
  memcpy(screen->charset,screen->saved_cursor.charset,4);
  screen->current_LS = screen->saved_cursor.ls;
  CHECK_CURSOR_X(screen);
  CHECK_CURSOR_Y(screen); 
}

/* Keypad application mode  ESC = */
static void VTScreen_DECKPAM(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  screen->deckpam = 1;
}

/* Keypad number mode  ESC > */
static void VTScreen_DECKPNM(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  screen->deckpam = 0;
}

/* Index  ESC D */
static void VTScreen_IND(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  VTScreen_index((VTScreen *)object,screen->cursor_x);
}

/* Next Line  ESC E */
static void VTScreen_NEL(void *object, Char *ch, int *params, int n_params)
{
  VTScreen_index((VTScreen *)object,0);
}

/* ???  ESC F */
static void VTScreen_HP_BUGGY_LL(void *object, Char *ch, int *params, int n_params)
{
  WARN("VTScreen_HP_BUGGY_LL");
}

/* Horizontal Tab Set  ESC H */
static void VTScreen_HTS(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;

  TABS_set_tab(screen->tabstop,screen->cursor_x);
}

/* Reverse Index  ESC M */
static void VTScreen_RI(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int new_y;

  new_y = screen->cursor_y-1;
  if(new_y  < screen->margin.top)
  {
    SCROLL_UP(screen,-1);
    new_y = screen->margin.top;
  }
  VTScreen_move_cursor(screen,screen->cursor_x,new_y);
}

/* single shift 2 ESC N */
static void VTScreen_SS2(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  screen->current_SS = 2;
}

/* single shift 3 ESC O */
static void VTScreen_SS3(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  screen->current_SS = 3;
}

/* Identify Terminal(DEC private) ESC Z */
/* This escape sequence is handled in VTCore */

/* Reset to Initial State  ESC c */
static void VTScreen_RIS(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  VTScreen_reset(screen);
  VTScreenBuffer_clear_rows(screen->current_buffer,
                            TO_BUFFERED_ROW(screen,0),
                            TO_BUFFERED_ROW(screen,screen->rows));
}

/* ???  ESC m */
static void VTScreen_HP_MEM_LOCK(void *object, Char *ch, int *params, int n_params)
{
  WARN("VTScreen_HP_MEM_LOCK");
}

/* ???  ESC m */
static void VTScreen_HP_MEM_UNLOCK(void *object, Char *ch, int *params, int n_params)
{
  WARN("VTScreen_HP_MEM_LOCK");
}

/* Locking Shift 2  ESC n */
static void VTScreen_LS2(void *object, Char *ch, int *params, int n_params)
{
  WARN("VTScreen_LS2");
}

/* Locking Shift 3  ESC o */
static void VTScreen_LS3(void *object, Char *ch, int *params, int n_params)
{
  WARN("VTScreen_LS3");
}

/* Locking Shift 3 Right  ESC | */
static void VTScreen_LS3R(void *object, Char *ch, int *params, int n_params)
{
  WARN("VTScreen_LS3R");
}

/* Locking Shift 2 Right  ESC } */
static void VTScreen_LS2R(void *object, Char *ch, int *params, int n_params)
{
  WARN("VTScreen_LS2R");
}

/* Locking Shift 1 Right  ESC ~ */
static void VTScreen_LS1R(void *object, Char *ch, int *params, int n_params)
{
  WARN("VTScreen_LS1R");
}
/* End of ESC state */


/* request resizeing screen buffer */
static void VTScreen_resize(VTScreen *screen,int width, int height)
{
  VTScreen_reset_selection(screen);
  VTScreenView_resize_request( screen,width,height);
}

/* DEC state */
/* DEC SET mode   ESC [ ? h */
static void VTScreen_DECSET(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int i;
  char str[256];
  for(i=0;i<n_params;i++)
  {
    sprintf(str,"DECSET[%d]",params[i]);
    switch(params[i])
    {
      case 1:
          VTScreen_set_decckm(screen,ON);
          break;
      case 2: /* ansi VT52 modes*/
          WARN(str);
          VTScreen_set_decanm(screen,ON);
          break;
      case 3:
          if(!VTScreen_is_deccolm(screen))
          {
            VTScreen_set_deccolm(screen,ON);
            VTScreen_resize(screen,132,screen->rows);
          }
          break;
      case 4: /* jump scroll.  do we need?*/
          VTScreen_set_decsclm(screen,ON);
          break;
      case 5:
          if(!VTScreen_is_decscnm(screen))
          {
            VTScreen_set_decscnm(screen,ON);
            VTScreenView_swap_video(screen);
          }
          break;
      case 6:
          VTScreen_set_decom(screen,ON);
          VTScreen_move_cursor(screen,0,screen->margin.top);
          break;
      case 7:
          VTScreen_set_decawm(screen,ON);
          break;
      case 8: /* ignore auto repeat */
          break;
      case 9: /* X10 compatibility */
          screen->track_mouse.type = X10_MOUSE;
          break;
      case 25:
          VTScreen_set_dectcem(screen,ON);
          break;
      case 47:
          VTScreen_to_alt_buffer(screen);
          break;
      case 1000:
          screen->track_mouse.type = VT200_MOUSE;
          break;
      case 1049:
          VTScreen_to_alt_buffer(screen);
          VTScreen_DECSC(screen,NULL,NULL,0);
          break;
      default:
          /* do nothing */;
          WARN(str);
          break;
    }
  }
}

/* DEC RESET mode  ESC [ ? l */
static void VTScreen_DECRST(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int i;
  char str[256];

  for(i=0;i<n_params;i++)
  {
    sprintf(str,"DECRST[%d]",params[i]);
    switch(params[i])
    {
      case 1:
          VTScreen_set_decckm(screen,OFF);
          break;
      case 2: /* ansi VT52 modes*/
          WARN(str);
          VTScreen_set_decanm(screen,OFF);
          break;
      case 3:
          if(VTScreen_is_deccolm(screen))
          {
            VTScreen_resize(screen,80,screen->rows);
            VTScreen_set_deccolm(screen,OFF);
          }
          break;
      case 4:  /* jump scroll.  do we need?*/
          VTScreen_set_decsclm(screen,OFF);
          break;
      case 5:
          if(VTScreen_is_decscnm(screen))
          {
            VTScreen_set_decscnm(screen,OFF);
            VTScreenView_swap_video(screen);
          }
          break;
      case 6:
          VTScreen_set_decom(screen,OFF);
          VTScreen_move_cursor(screen,0,0);
          break;
      case 7:
          VTScreen_set_decawm(screen,OFF);
          break;
      case 8: /* ignore auto repeat */
          break;
      case 9: /* X10 compatibility */
          screen->track_mouse.type = MOUSE_OFF;
          break;
      case 25:
          VTScreen_set_dectcem(screen,OFF);
          break;
      case 47:
          VTScreen_from_alt_buffer(screen);
          break;
      case 1000:
          screen->track_mouse.type = MOUSE_OFF;
          break;
      case 1049:
          VTScreen_from_alt_buffer(screen);
          VTScreen_DECRC(screen,NULL,NULL,0);
          break;
      default:
          /* do nothing */;
          WARN(str);
          break;
    }
  }
}

/* ???  ESC [ ? r */
static void VTScreen_XTERM_RESTORE(void *object, Char *ch, int *params, int n_params)
{
  int i;
  char str[256];
  VTScreen *screen = (VTScreen *)object;

  for(i=0;i<n_params;i++)
  {
    sprintf(str,"VTScreen_XTERM_RESTORE[%d]",params[i]);
    switch(params[i])
    {
      case 1:
          VTScreen_set_decckm(screen,screen->savemodes.decmodes.decckm);
          break;
      case 2: /* ansi VT52 modes */
          VTScreen_set_decanm(screen,screen->savemodes.decmodes.decanm);
          break;
      case 3:
          VTScreen_set_deccolm(screen,screen->savemodes.decmodes.deccolm);
          if(VTScreen_is_deccolm(screen))
              VTScreen_resize(screen,132,screen->rows);
          break;
      case 4:  /* jump scroll.  do we need?*/
          VTScreen_set_decsclm(screen,screen->savemodes.decmodes.decsclm);
          break;
      case 5:
          if(screen->savemodes.decmodes.decscnm != screen->decmodes.decscnm)
              VTScreenView_swap_video(screen);
          VTScreen_set_decscnm(screen,screen->savemodes.decmodes.decscnm);
          break;
      case 6:
          VTScreen_set_decom(screen,screen->savemodes.decmodes.decom);
          VTScreen_move_cursor(screen,0,screen->margin.top);
          break;
      case 7:
          VTScreen_set_decawm(screen,screen->savemodes.decmodes.decawm);
          break;
      case 25:
          VTScreen_set_dectcem(screen,screen->savemodes.decmodes.dectcem);
          break;
      case 1000: /* VT200 MOUSE */
      case 1001: /* VT200 Highlight mouse */
          screen->savemodes.track_mouse_type = screen->track_mouse.type;
          break;
      default:
          /* do nothing */;
          WARN(str);
          break;
    }
  }
}

/* ???  ESC [ ? s */
static void VTScreen_XTERM_SAVE(void *object, Char *ch, int *params, int n_params)
{
  int i;
  char str[256];
  VTScreen *screen = (VTScreen *)object;

  for(i=0;i<n_params;i++)
  {
    sprintf(str,"VTScreen_XTERMSAVE[%d]",params[i]);
    switch(params[i])
    {
      case 1:
          screen->savemodes.decmodes.decckm = VTScreen_is_decckm(screen);
          break;
      case 2: /* ansi VT52 modes */
          screen->savemodes.decmodes.decanm = VTScreen_is_decanm(screen);
          break;
      case 3:
          screen->savemodes.decmodes.deccolm = VTScreen_is_deccolm(screen);
          break;
      case 4:  /* jump scroll.  do we need?*/
          screen->savemodes.decmodes.decsclm = VTScreen_is_decsclm(screen);
          break;
      case 5:
          screen->savemodes.decmodes.decscnm = VTScreen_is_decscnm(screen);
          break;
      case 6:
          screen->savemodes.decmodes.decom = VTScreen_is_decom(screen);
          break;
      case 7:
          screen->savemodes.decmodes.decawm = VTScreen_is_decawm(screen);
          break;
      case 25:
          screen->savemodes.decmodes.dectcem = VTScreen_is_dectcem(screen);
          break;
      case 1000: /* VT200 MOUSE */
      case 1001: /* VT200 Highlight mouse */
          screen->savemodes.track_mouse_type = screen->track_mouse.type;
          break;
      default:
          /* do nothing */;
          WARN(str);
          break;
    }
  }
}
/* End of DEC state */

/* CSI state */
  /* Cursor Back  ESC [ D */
static void VTScreen_CUB(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int n, new_x;

  if((n = params[0]) < 1)
      n = 1;
  new_x = screen->cursor_x - n;
  VTScreen_move_cursor(screen,new_x,screen->cursor_y);
}

  /* Cursor Down  ESC [ B */
static void VTScreen_CUD(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int n;

  if((n = params[0]) < 1)
      n = 1;
  screen->cursor_y += n;
  CHECK_CURSOR_Y_WITH_MARGIN(screen);
  VTScreen_move_cursor(screen,screen->cursor_x,screen->cursor_y);
}

  /* Cursor Forward  ESC [ C */
static void VTScreen_CUF(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int n,new_x;

  if((n = params[0]) < 1)
      n = 1;

  new_x = screen->cursor_x + n;
  VTScreen_move_cursor(screen,new_x,screen->cursor_y);
}

  /* Cursor Up  ESC [ A */
static void VTScreen_CUU(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int n;

  if((n = params[0]) < 1)
      n = 1;
  screen->cursor_y -= n;
  CHECK_CURSOR_Y_WITH_MARGIN(screen);
  VTScreen_move_cursor(screen,screen->cursor_x,screen->cursor_y );
}

/* Cursor Next Line ESC [ E */
static void VTScreen_CNL(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int n;

  if((n = params[0]) < 1)
      n = 1;
  screen->cursor_y += n;
  screen->cursor_x = 0;
  VTScreen_move_cursor(screen,screen->cursor_x,screen->cursor_y );
}

/* Cursor Preceding Line ESC [ F */
static void VTScreen_CPL(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int n;

  if((n = params[0]) < 1)
      n = 1;
  screen->cursor_y -= n;
  screen->cursor_x = 0;
  VTScreen_move_cursor(screen,screen->cursor_x,screen->cursor_y );
}

  /* Cursor Position  ESC [ H / ESC H f */
static void VTScreen_CUP(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int lines,columns;

  if((lines = params[0]) < 1)
      lines = 1;
  if(n_params < 2 || (columns = params[1]) == -1)
      columns = 1;

      /* CUP protocol's index starts from 1, but our index starts from 0 */
  screen->cursor_y = lines - 1;
  screen->cursor_x = columns -1;
  if(VTScreen_is_decom(screen))
  {
    CHECK_CURSOR_Y_WITH_MARGIN(screen);
  }
  else
  {
    CHECK_CURSOR_Y(screen);
  }
  CHECK_CURSOR_X(screen);
  
  VTScreen_move_cursor(screen,screen->cursor_x,screen->cursor_y);
}
  /* Character Position Absolute ESC [ ' */
  /* also support Cursor Character Absolute ESC [ G */
static void VTScreen_HPA(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int columns[2];

  columns[0] = screen->cursor_y + 1;
  if((columns[1] = params[0]) < 1)
      columns[1] = 1;

  VTScreen_CUP(object,ch,columns,2);
}
  
  /* Line Position Absolute ESC [ d */
static void VTScreen_VPA(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int lines[2];

  lines[1] = screen->cursor_x + 1;
  if((lines[0] = params[0]) < 1)
      lines[0] = 1;

  VTScreen_CUP(object,ch,lines,2);
}

#ifdef BIDI
#define CLEAR_LINE(scr,row,scol,ecol)  \
do {\
   VTScreenBuffer_clear_cells(scr->current_buffer, \
                              scr->visual_start_row+row,scol,ecol); \
   if(scr->vtlayout) \
     VTScreen_update_region(scr,0,row,scr->columns,row+1); \
} while(0)
#else
#define CLEAR_LINE(scr,row,scol,ecol)  \
do {\
   VTScreenBuffer_clear_cells(scr->current_buffer, \
                              scr->visual_start_row+row,scol,ecol); \
} while(0)
#endif
#define CLEAR_DISP(scr,srow,erow) \
do {\
   VTScreenBuffer_clear_rows(scr->current_buffer, \
                             scr->visual_start_row + srow, \
                             scr->visual_start_row + erow); \
} while(0)

/* Erase in Display  ESC [ J */
static void VTScreen_ED(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;

  switch(params[0])
  {
    default:
    case 0:
            /* erase from active position to end of line */
        CLEAR_LINE(screen,screen->cursor_y,screen->cursor_x,screen->columns);
        CLEAR_DISP(screen,screen->cursor_y+1,screen->rows);
        break;
    case 1:
            /* erase from start of line to active position */
        CLEAR_LINE(screen,screen->cursor_y,0,screen->cursor_x+1);
        CLEAR_DISP(screen,0,screen->cursor_y);
        break;
    case 2:
        /* erase whole screen */
        CLEAR_DISP(screen,0,screen->rows);
        break;
  }
}

/* Erase in Line  ESC [ K */
static void VTScreen_EL(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;

  switch(params[0])
  {
    default:
    case 0:
            /* erase from cursor to end of line */
        CLEAR_LINE(screen,screen->cursor_y,screen->cursor_x,screen->columns);
        break;
    case 1: /* erase from start of line to cursor inclusive */
        CLEAR_LINE(screen,screen->cursor_y,0,screen->cursor_x+1);
        break;
    case 2: /* erase whole line */
        CLEAR_LINE(screen,screen->cursor_y,0,screen->columns);
        break;
  }
}

  /* Erase Character ESC [ X */
static void VTScreen_ECH(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int n;

  if((n = params[0]) < 1)
      n = 1;

  CLEAR_LINE(screen,screen->cursor_y,screen->cursor_x,screen->cursor_x+n);
}

/* Insert Line(VT102)  ESC [ L */
static void VTScreen_IL(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int amount = params[0];

  if(amount <= 0)
      amount = 1;
  VTScreen_reset_selection(screen);
#define USE_SCROLL_VIEW 0
  /*
    When received IL/DL request, use scroll_view(bitblt) or not
    basically without using scroll_view can buffer the scroll, so
    it's faster
   */
#if USE_SCROLL_VIEW 
  if(HAS_SCROLL_VIEW(screen))
      VTScreen_flush(screen);
#endif
  VTScreenBuffer_shift_row_with_margin(
      screen->current_buffer,
      TO_BUFFERED_ROW(screen,screen->cursor_y),
      TO_BUFFERED_ROW(screen,screen->margin.bottom+1),
      -amount);
  if(screen->margin.bottom+1 - screen->cursor_y < amount)
      amount = screen->margin.bottom+1 - screen->cursor_y;
#if USE_SCROLL_VIEW
  if(HAS_SCROLL_VIEW(screen))
  {
    VTScreenView_scroll_view(screen,screen->cursor_y+amount,screen->cursor_y,
                             screen->margin.bottom+1-screen->cursor_y-amount);
  }
  else
#endif      
   VTScreen_update_region(screen,
                          0,screen->cursor_y,
                          screen->columns,screen->rows);
}

/* Delete Line(VT102)  ESC [ M */
static void VTScreen_DL(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int amount = params[0];

  if(amount <= 0)
      amount = 1;
  VTScreen_reset_selection(screen);
#if USE_SCROLL_VIEW
  if(HAS_SCROLL_VIEW(screen))
      VTScreen_flush(screen);
#endif  
  VTScreenBuffer_shift_row_with_margin(
      screen->current_buffer,
      TO_BUFFERED_ROW(screen,screen->cursor_y),
      TO_BUFFERED_ROW(screen,screen->margin.bottom+1),
      amount);
  if(screen->margin.bottom+1 - screen->cursor_y < amount)
      amount = screen->margin.bottom+1 - screen->cursor_y;
#if USE_SCROLL_VIEW
  if(HAS_SCROLL_VIEW(screen))
  {
    VTScreenView_scroll_view(screen,screen->cursor_y,screen->cursor_y+amount,
                             screen->margin.bottom+1-screen->cursor_y-amount);
  } 
  else
#endif      
  VTScreen_update_region(screen,
                         0,screen->cursor_y,
                         screen->columns,screen->rows);
}

/* Delete Character(VT102)  ESC [ @ */
static void VTScreen_ICH(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int amount = params[0];

  if(amount <= 0)
      amount = 1;
  VTScreenBuffer_insert_cells(screen->current_buffer,
                              TO_BUFFERED_ROW(screen,+screen->cursor_y),
                               screen->cursor_x,
                               amount);
}

/* Delete Character(VT102)  ESC [ P */
static void VTScreen_DCH(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int amount = params[0];

  if(amount <= 0)
      amount = 1;
  VTScreenBuffer_delete_cells(screen->current_buffer,
                              TO_BUFFERED_ROW(screen,screen->cursor_y),
                              screen->cursor_x,
                              amount);
}

/* Cursor Forward Tabulation ESC [ I */
static void VTScreen_CHT(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int i;
  int n;
  int next;

  if( (n = params[0]) < 0)
      n = 1;

  next = screen->cursor_x;
  for(i=0;i<n;i++)
  {
    next = next_tab(screen,next);
  }
  VTScreen_move_cursor(screen,next,screen->cursor_y);
}

/* TrackMouse  ESC [ T */
static void VTScreen_TrackMouse(void *object, Char *ch, int *params, int n_params)
{
  WARN("VTScreen_TrackMouse");
}


/* Device Attribute  ESC [ c */
/* This is handled by VTCore */

/* Tabulation Clear  ESC [ g */
static void VTScreen_TBC(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int n = 0;
  if((n = params[0]) < 0)
    n = 0;

  switch(n)
  {
    case 0:
        TABS_clear_tab(screen->tabstop,screen->cursor_x);
        break;
    case 3:
        TABS_clear_all_tabs(screen->tabstop);
        break;
    default:
        /* do nothing */;
  }
}

/* Cursor backward Tabulation   ESC [ Z */
static void VTScreen_CBT(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int n;
  int prev = screen->cursor_x;
  if( (n = params[0]) < 0 )
      n = 1;
  while(n)
  {
    prev = TABS_get_prev_tab(screen->tabstop,prev);
    n--;
  }
  VTScreen_move_cursor(screen,prev,screen->cursor_y);
}

/* ANSI SET mode  ESC [ h */
static void VTScreen_SET(void *object, Char *ch, int *params, int n_params)
{
  int i;
  char str[256];
  VTScreen *screen = (VTScreen *)object;

  for(i=0;i<n_params;i++)
  {
    sprintf(str,"SET[%d]",params[i]);
    switch(params[i])
    {
      default:
          WARN(str);
          break;
      case 4: /* insert mode IRM */
          screen->insert = 1;
          break;
#ifdef BIDI
      case 8:
          screen->bidi_implicit = 1;
          break;
#endif          
      case 20: /* LineFeed Mode */
          screen->linefeed = 1;
          break;
    }
  }
}

/* ANSI RESET mode  ESC [ l */
static void VTScreen_RST(void *object, Char *ch, int *params, int n_params)
{
  int i;
  char str[256];
  VTScreen *screen = (VTScreen *)object;

  for(i=0;i<n_params;i++)
  {
    sprintf(str,"RST[%d]",params[i]);
    switch(params[i])
    {
      case 4: /* insert mode IRM */
          screen->insert = 0;
          break;
#ifdef BIDI
      case 8:
          screen->bidi_implicit = 0;
          break;
#endif          
      case 20: /* LineFeed Mode */
          screen->linefeed = 0;
          break;
      default:
          /* do nothing */;
          WARN(str);
          break;
    }
  }
}

/* Set(Select) Graphic Rendition  ESC [ m */
static void VTScreen_SGR(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int i;
  char str[256];

  for(i=0;i<n_params;i++)
  {
    sprintf(str,"SGR[%d]",params[i]);
    switch(params[i])
    {
      case -1: /* default value of argument */
      case 0:
              /* reset attribute */
          screen->current_attr.bold = 
      screen->current_attr.blink =
      screen->current_attr.inverse =
      screen->current_attr.underline =
      screen->current_attr.foreground =
      screen->current_attr.background = 0;
          break;
      case 1: /* Bold */
          screen->current_attr.bold = 1;
          break;
      case 4: /* Underline */
          screen->current_attr.underline = 1;
          break;
      case 5: /* Blink */
          screen->current_attr.blink = 1;
          break;
      case 7: /* Inverse */
          screen->current_attr.inverse = 1;
          break;
      case 24: /* Not Underline */
          screen->current_attr.underline = 0;
          break;
      case 25: /* Not Blink */
          screen->current_attr.blink = 0;
          break;
      case 27: /* Not Inverse */
          screen->current_attr.inverse = 0;
          break;
      case 30: /* black */
      case 31: /* red */
      case 32: /* green */
      case 33: /* yellow */
      case 34: /* blue */
      case 35: /* magenta */
      case 36: /* cyan */
      case 37: /* white */
          screen->current_attr.foreground = params[i] - 29; 
          break;
      case 39: /* default color */
          screen->current_attr.foreground = 0;
          break;
      case 40: /* black background */
      case 41: /* red background */
      case 42: /* green background */
      case 43: /* yellow background */
      case 44: /* blue background */
      case 45: /* magenta background */
      case 46: /* cyan background */
      case 47: /* white background */
          screen->current_attr.background = params[i] - 39;
          break;
      case 49: /* default color */
          screen->current_attr.background = 0;
          break;

      default:
          WARN(str);
          /* do nothing */;
    }
  }
}

/* aka DSR(Device Status Report)  ESC [ n */
/* This escape sequence is handled in VTCore. */
   
/* Set Top Bottom Margin  ESC [ r */
static void VTScreen_DECSTBM(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int top;
  int bottom;

  if((top=params[0]) < 1)
      top = 1;
  if(n_params < 2 || (bottom = params[1]) < 1 ||
     bottom > screen->rows ||
     top > bottom)
      bottom = screen->rows;
  top--;
  bottom--;
  if(screen->num_of_view_scroll)
      VTScreen_flush_scroll(screen);

  if( (top  < bottom ) &&
      (bottom < screen->rows ) &&
      (0<=top) )
  {
    screen->margin.top = top;
    screen->margin.bottom = bottom;
  }
  if(VTScreen_is_decom(screen))
      VTScreen_move_cursor(screen,0,screen->margin.top);
  else
      VTScreen_move_cursor(screen,0,0);
}

/* Request Terminal Parameters  ESC [ x */
/* This escape sequence is handled in VTCore */

#ifdef BIDI
/* Start Reverse String ESC [ ]*/
static void VTScreen_SRS(void *object, Char *ch, int *params, int n_params)
{
  WARN("SRS");
}
#endif
/* End of CSI state */

/* SCR state */
/* Screen Alignment Display  ESC # 8 */
static void VTScreen_DECALN(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;

  int y,x;

  for(y=0;y<screen->rows;y++)
  {
    for(x=0;x<screen->columns;x++)
    {
      VTScreenBuffer_add_char(screen->current_buffer,x,
                              TO_BUFFERED_ROW(screen,y),&capital_E,0,0,0,0,0,0,
                              ASCII_CHARSET);
    }
  }
}
/* End of SCR state */

/* SCS state */
/* Graphic character set ESC [()*+] [AB012] */
static void VTScreen_GSETS(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  switch((char)params[0])
  {
    case '(':
        screen->charset[0] = Char_get_first_byte(ch);
        break;
    case ')':
        screen->charset[1] = Char_get_first_byte(ch);
        break;
    case '*':
        screen->charset[2] = Char_get_first_byte(ch);
        break;
    case '+':
        screen->charset[3] = Char_get_first_byte(ch);
        break;
    default:
        screen->charset[0] = screen->charset[1] =
    screen->charset[2] = screen->charset[3] = ASCII_CHARSET;
        screen->current_LS = 0;
        screen->current_SS = 0;
        fprintf(stderr,"This can't be happen: ESC [ %c %c\n",
                (char)params[0],Char_get_first_byte(ch));
  }
}
/* End of SCS state */

/* OSC state */
/* Operating System Command  ESC ] <num> ; <printables> ^G */
static void VTScreen_OSC(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;

  ((char *)params)[n_params-1] = '\0'; /* terminate string */
  VTScreenView_notify_osc(screen,((char *)params)[0],
                          &((char *)params)[1],n_params-2);

}
/* End of OSC state */

#ifdef BIDI
/* BIDI State */
/* Select Presentation Direction  ESC [ Ps1 ; Ps2 <space> S */
static void VTScreen_SPD(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;

  if(screen->vtlayout == NULL)
      return; 

  switch(params[0])
  {
    case -1:
    case 0:
        if(params[1] == 1 || n_params == 1)
          VTScreen_set_direction(screen,VT_LTR);
        UPDATE_WHOLE_SCREEN(screen);
        break;
    case 3:
        if(params[1] == 1 || n_params == 1)
          VTScreen_set_direction(screen,VT_RTL);
        UPDATE_WHOLE_SCREEN(screen);
        break;
  }
}

/* Select Alternative Presentation Variantsq  ESC [ Ps ... <space> ] */
static void VTScreen_SAPV(void *object, Char *ch, int *params, int n_params)
{
  VTScreen *screen = (VTScreen *)object;
  int i;
  char str[256];

  if(screen->vtlayout == NULL)
      return ;

  for(i=0;i<n_params;i++)
  {
    sprintf(str,"SAPV[%d]",params[i]);
    switch(params[i])
    {
      case 0:
      case 1:
          if(screen->vtlayout->set_numerals)
              screen->vtlayout->set_numerals(screen->vtlayout,VT_NOMINAL);
          break;
      case 2:
          if(screen->vtlayout->set_numerals)
              screen->vtlayout->set_numerals(screen->vtlayout,VT_NATIONAL);
          break;
      default:
          WARN(str);
    }
  }
}


/* End of BIDI State */
#endif

/**
 * initialize VTScreen.
 * @param screen reference to VTScreen object
 * @param vt100 reference to VT100 object
 * This method connects VTScreen to VT100
 */
void VTScreen_init(VTScreen *screen,VT100 *vt100)
{
      /* Ground state */
  vt100->all_states[GROUND_STATE].when_printable.update = VTScreen_write;
  vt100->all_states[GROUND_STATE].when_printable.object = screen;

      /* OSC state */
  VT100_register_sequence(vt100,(unsigned char *)"\033]\a", GROUND_STATE, VTScreen_OSC,screen);
  
      /* SCS state */
  VT100_register_sequence(vt100,(unsigned char *)"\033(0", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033(1", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033(2", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033(A", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033(B", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033)0", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033)1", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033)2", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033)A", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033)B", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033*0", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033*1", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033*2", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033*A", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033*B", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+0", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+1", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+2", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+A", GROUND_STATE, VTScreen_GSETS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+B", GROUND_STATE, VTScreen_GSETS,screen);
  
      /* SCR state */
  VT100_register_sequence(vt100,(unsigned char *)"\033#8", GROUND_STATE, VTScreen_DECALN,screen);
  
      /* CSI state */
  VT100_register_sequence(vt100,(unsigned char *)"\033[A", GROUND_STATE, VTScreen_CUU,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[B", GROUND_STATE, VTScreen_CUD,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[C", GROUND_STATE, VTScreen_CUF,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[D", GROUND_STATE, VTScreen_CUB,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[E", GROUND_STATE, VTScreen_CNL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[F", GROUND_STATE, VTScreen_CPL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[H", GROUND_STATE, VTScreen_CUP,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[I", GROUND_STATE, VTScreen_CHT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[G", GROUND_STATE, VTScreen_HPA,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[`", GROUND_STATE, VTScreen_HPA,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[J", GROUND_STATE, VTScreen_ED,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[K", GROUND_STATE, VTScreen_EL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[L", GROUND_STATE, VTScreen_IL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[M", GROUND_STATE, VTScreen_DL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[@", GROUND_STATE, VTScreen_ICH,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[P", GROUND_STATE, VTScreen_DCH,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[T", GROUND_STATE, VTScreen_TrackMouse,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[X", GROUND_STATE, VTScreen_ECH,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[Z", GROUND_STATE, VTScreen_CBT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[d", GROUND_STATE, VTScreen_VPA,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[f", GROUND_STATE, VTScreen_CUP,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[g", GROUND_STATE, VTScreen_TBC,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[h", GROUND_STATE, VTScreen_SET,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[l", GROUND_STATE, VTScreen_RST,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[m", GROUND_STATE, VTScreen_SGR,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[r",GROUND_STATE,VTScreen_DECSTBM,screen);
#ifdef BIDI
  VT100_register_sequence(vt100,(unsigned char *)"\033[[",GROUND_STATE,VTScreen_SRS,screen);
#endif

      /* These are handled in VTCore */
/*  VT100_register_sequence(vt100,"\033[n", GROUND_STATE, VTScreen_CPR,screen);
    VT100_register_sequence(vt100,"\033[c", GROUND_STATE, VTScreen_DA1,screen);

    VT100_register_sequence(vt100,"\033[x",GROUND_STATE,VTScreen_DECREQTPARM,screen); */

      /* DEC state */
  VT100_register_sequence(vt100,(unsigned char *)"\033[?h",GROUND_STATE,VTScreen_DECSET,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?l",GROUND_STATE,VTScreen_DECRST,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?r",GROUND_STATE,VTScreen_XTERM_RESTORE,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?s",GROUND_STATE,VTScreen_XTERM_SAVE,screen);
  
      /* ESC state */
  VT100_register_sequence(vt100,(unsigned char *)"\0337",GROUND_STATE,VTScreen_DECSC,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\0338",GROUND_STATE,VTScreen_DECRC,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033=",GROUND_STATE,VTScreen_DECKPAM,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033>",GROUND_STATE,VTScreen_DECKPNM,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033D",GROUND_STATE,VTScreen_IND,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033E",GROUND_STATE,VTScreen_NEL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033F",GROUND_STATE,VTScreen_HP_BUGGY_LL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033H",GROUND_STATE,VTScreen_HTS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033M",GROUND_STATE,VTScreen_RI,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033N",GROUND_STATE,VTScreen_SS2,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033O",GROUND_STATE,VTScreen_SS3,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033c",GROUND_STATE,VTScreen_RIS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033l",GROUND_STATE,VTScreen_HP_MEM_LOCK,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033m",GROUND_STATE,VTScreen_HP_MEM_UNLOCK,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033n",GROUND_STATE,VTScreen_LS2,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033o",GROUND_STATE,VTScreen_LS3,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033|",GROUND_STATE,VTScreen_LS3R,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033}",GROUND_STATE,VTScreen_LS2R,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033~",GROUND_STATE,VTScreen_LS1R,screen);
      /* This is handled in VTCore */
/* VT100_register_sequence(vt100,(unsigned char *)"\033Z",GROUND_STATE,VTScreen_DECID,screen);*/

#ifdef BIDI
  VT100_register_sequence(vt100,(unsigned char *)"\033[ S",GROUND_STATE,VTScreen_SPD,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[ ]",GROUND_STATE,VTScreen_SAPV,screen);
#endif
  
      /* BEL */
  VT100_register_sequence(vt100,(unsigned char *)"\a", NO_MOVE, VTScreen_BEL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033\a", NO_MOVE, VTScreen_BEL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[\a", NO_MOVE, VTScreen_BEL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?\a", NO_MOVE, VTScreen_BEL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033#\a", NO_MOVE, VTScreen_BEL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033# \a", NO_MOVE, VTScreen_BEL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+\a", NO_MOVE, VTScreen_BEL,screen);

      /* BS */
  VT100_register_sequence(vt100,(unsigned char *)"\b", NO_MOVE, VTScreen_BS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033\b", NO_MOVE, VTScreen_BS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[\b", NO_MOVE, VTScreen_BS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?\b", NO_MOVE, VTScreen_BS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033#\b", NO_MOVE, VTScreen_BS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033# \b", NO_MOVE, VTScreen_BS,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+\b", NO_MOVE, VTScreen_BS,screen);

      /* HT */
  VT100_register_sequence(vt100,(unsigned char *)"\t", NO_MOVE, VTScreen_HT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033\t", NO_MOVE, VTScreen_HT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[\t", NO_MOVE, VTScreen_HT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?\t", NO_MOVE, VTScreen_HT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033#\t", NO_MOVE, VTScreen_HT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033# \t", NO_MOVE, VTScreen_HT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+\t", NO_MOVE, VTScreen_HT,screen);

      /* SO */
  VT100_register_sequence(vt100,(unsigned char *)"\016", NO_MOVE, VTScreen_SO,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033\016", NO_MOVE, VTScreen_SO,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[\016", NO_MOVE, VTScreen_SO,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?\016", NO_MOVE, VTScreen_SO,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033#\016", NO_MOVE, VTScreen_SO,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033# \016", NO_MOVE, VTScreen_SO,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+\016", NO_MOVE, VTScreen_SO,screen);

      /* SI */
  VT100_register_sequence(vt100,(unsigned char *)"\017", NO_MOVE, VTScreen_SI,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033\017", NO_MOVE, VTScreen_SI,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[\017", NO_MOVE, VTScreen_SI,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?\017", NO_MOVE, VTScreen_SI,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033#\017", NO_MOVE, VTScreen_SI,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033# \017", NO_MOVE, VTScreen_SI,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+\017", NO_MOVE, VTScreen_SI,screen);

      /* CR */
  VT100_register_sequence(vt100,(unsigned char *)"\r", GROUND_STATE, VTScreen_CR,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033\r", GROUND_STATE, VTScreen_CR,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[\r", GROUND_STATE, VTScreen_CR,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?\r", GROUND_STATE, VTScreen_CR,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033#\r",  GROUND_STATE, VTScreen_CR,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033# \r", GROUND_STATE, VTScreen_CR,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+\r", GROUND_STATE, VTScreen_CR,screen);

      /* NL(LF) */
  VT100_register_sequence(vt100,(unsigned char *)"\n", GROUND_STATE, VTScreen_NL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033\n", GROUND_STATE, VTScreen_NL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[\n", GROUND_STATE, VTScreen_NL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?\n", GROUND_STATE, VTScreen_NL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033#\n",  GROUND_STATE, VTScreen_NL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033#\n", GROUND_STATE, VTScreen_NL,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+\n", GROUND_STATE, VTScreen_NL,screen);

      /* VT */
  VT100_register_sequence(vt100,(unsigned char *)"\v", GROUND_STATE, VTScreen_VT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033\v", GROUND_STATE, VTScreen_VT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[\v", GROUND_STATE, VTScreen_VT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?\v", GROUND_STATE, VTScreen_VT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033#\v",  GROUND_STATE, VTScreen_VT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033# \v", GROUND_STATE, VTScreen_VT,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+\v", GROUND_STATE, VTScreen_VT,screen);

      /* NP(FF) */
  VT100_register_sequence(vt100,(unsigned char *)"\f", GROUND_STATE, VTScreen_NP,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033\f", GROUND_STATE, VTScreen_NP,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[\f", GROUND_STATE, VTScreen_NP,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033[?\f", GROUND_STATE, VTScreen_NP,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033#\f",  GROUND_STATE, VTScreen_NP,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033# \f", GROUND_STATE, VTScreen_NP,screen);
  VT100_register_sequence(vt100,(unsigned char *)"\033+\f", GROUND_STATE, VTScreen_NP,screen);
}
