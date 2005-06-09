/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/**
 * @file screen.h
 */
#ifndef __VTSCREEN_INTERNAL_H__
#define __VTSCREEN_INTERNAL_H__
#if defined(HAVE_CONFIGH_H)
# include "../config.h"
#endif
#include "screen_buffer.h"
#include "tabs.h"
#include "vt100.h"
#include "iterm/screen.h"

#ifdef BIDI
#include "screen_visual_row.h"
#endif

enum {
    MAX_MODE_BYTES = 2,
    MAX_MODE_BITS = BYTE * MAX_MODE_BYTES
};

enum {
    DECCKM = 0, /**< DEC Cursor Key mode on/off */
    DECANM,     /**< ANSI/VT52 mode on/off */
    DECCOLM,    /**< DEC Column mode on/off */
    DECSCLM,    /**< DEC Scrolling mode on/off */
    DECSCNM,    /**< DEC Screen mode on/off */
    DECOM,      /**< DEC Original mode on/off */
    DECAWM,     /**< DEC Auto Wrap mode on/off */
    DECINLM,    /**< DEC Interlace mode on/off */
    DECTCEM    /**< DEC Text Cursor Enable Mode */
}; 

typedef unsigned char MODES[MAX_MODE_BYTES];

#define VTScreen_clear_modes(scr) \
       memset(scr->modes,0,sizeof(scr->decmodes))

#define VTScreen_set_mode(scr,mode) \
       (scr->modes[(mode/8)] |= (1<<(mode%8)))

#define VTScreen_reset_mode(scr,mode) \
       (scr->modes[(mode/8)] &= ~(1<<(mode%8)))

#define VTScreen_is_mode(scr,mode) \
       (scr->modes[(mode/8)] & (1<<(mode%8)))

/**
 * @struct _VTScreen
 * VTScreen class.
 * Model of the Screen.
 * This struct is typedefed to `Screen'.
 */
typedef struct _VTScreen
{
  struct _VTScreenView *screen_view;
      /**< reference to VTScreenView object */
  
  TABS tabstop;
      /**< store tabstops */

  unsigned int do_wrap : 1;
      /**< force to wrap flag */
#ifdef BIDI  
  unsigned int bidi_implicit : 1;
      /**< Implicit Bidi Data Handling or not*/
#endif  
  unsigned int linefeed : 1;
      /**< linefeed mode attribute flag */
  unsigned int insert : 1;
      /**< insert mode attribute flag */
  unsigned int deckpam : 1;
      /**< keypad application mode flag */
  
  struct DECMODES{
    unsigned int decckm : 1;
        /**< DEC Cursor Key mode on/off */
    unsigned int decanm : 1;
        /**< ANSI/VT52 mode on/off */
    unsigned int deccolm : 1;
        /**< DEC Column mode on/off */
    unsigned int decsclm : 1;
        /**< DEC Scrolling mode on/off */
    unsigned int decscnm : 1;
        /**< DEC Screen mode on/off */
    unsigned int decom : 1;
        /**< DEC Original mode on/off */
    unsigned int decawm : 1;
        /**< DEC Auto Wrap mode on/off */
    unsigned int decinlm : 1;
        /**< DEC Interlace mode on/off */
    unsigned int dectcem : 1;
        /**< DEC Text Cursor Enable Mode */
  } decmodes;
      /**< DEC private modes flags */

  struct SAVEMODES {
    struct DECMODES decmodes;
    unsigned int track_mouse_type : 1;
  } savemodes;
      /* xterm store modes flags */
  
  struct ATTRIBUTES {
    unsigned int bold:1;
        /**< current bold text attribute state */
    unsigned int inverse:1;
        /**< current revese text attribute state */
    unsigned int underline:1;
        /**< current underlined text attribute state */
    unsigned int blink:1;
        /**< current blink text attribute state*/
    unsigned int foreground:4;
        /**< current foreground color attribute state */
    unsigned int background:4;
        /**< current background color attribute state */
  } current_attr;
      /**< current text rendition flags */

  char charset[4];
      /** charcter set G0,G1,G2,G3 */

  int current_LS;
      /** current Locking Shift */
  int current_SS;
      /** current Single Shift */
  
  VTScreenBuffer *current_buffer;
      /**< current buffer to point all_buffer of alt_buffer */
  VTScreenBuffer *all_buffer;
      /**< all screen buffer, including history */
  VTScreenBuffer *alt_buffer;
      /**< alternative screen buffer, including history */
  int history_size;
      /**< history_size of buffer */
         
  int visual_start_row;
      /**< index to visual start row of buffer */

  struct MARGINE {
    int top;
        /**< index to end of top margin from visual_start_row */
    int bottom;
        /**< index to start of bottom margin from visual_start_row*/
  } margin;
      /**< top/bottom margin of scrolling area */
  
  int cursor_x;
      /**< current cursor column position */
  int cursor_y;
      /**< current cursor row  position */
  int columns;
      /**< number of visual column */
  int rows;
      /**< number of visual row */

  int drawn_cursor_x;
      /**< drawn cursor column position */
  int drawn_cursor_y;
      /**< drawn cursor row position */
  Row *drawn_cursor_row;
      /**< drawn cursor row */
  
  unsigned int cursor_drawn : 1;
      /**< if cursor is drawn or not */

  int num_line_shifted;
      /**< number of line shifted by scrollbar */

  unsigned int update_whole_screen : 1;
      /**< if we need to update whole screen or not */

  struct SELECTION {
    struct REGION {
      int scol;
      int srow;
      int ecol;
      int erow;
    } region;
    int type;
  } selection;
      /**< selected region  */

  struct SAVED_CURSOR {
    int cursor_x;
    int cursor_y;
    struct ATTRIBUTES attr;
    char charset[4];
    int ls;
  } saved_cursor;
      /**< saved cursor */

  struct TrackMouse {
    struct REGION region;
        /**< reporting region   */
    int type;
        /**< escape sequence type \n
           0 - unreport \n
           1 - X10 compatibility \n
           2 - DEC vt200 compatible
           3 - DEC vt200 Hilight Tracking 
         */
  } track_mouse;
      /**< for MouseTracking  */

  int num_of_view_scroll;
      /**< number of line to scroll view image */
    
#ifdef BIDI
  VisualRow *vrow;
  VTLayout *vtlayout;
#endif /* BIDI */
} VTScreen;
/**< typedefed struct of struct _VTScreen */

VTScreen *VTScreen_new(int n_cols, int n_rows,int n_hist);
void VTScreen_init(VTScreen *screen,VT100 *vt100);
void VTScreen_destroy(VTScreen *screen); 
void VTScreen_redraw(VTScreen *screen, int start_col, int start_row,
                     int end_col, int end_row, int with_clear_rect);
void VTScreen_flush(VTScreen *screen);
void VTScreen_set_size(VTScreen *screen,int cols, int rows);
void VTScreen_start_selection(VTScreen *screen,int scol, int srow, int type);
void VTScreen_extend_selection(VTScreen *screen,int scol, int srow);
void VTScreen_clear_selection(VTScreen *screen);

void VTScreen_copy_buffer(VTScreen *screen, int scol, int srow,
                          int ecol, int erow, char *dest, int dest_size);
void VTScreen_copy_selected_buffer(VTScreen *screen,char *dest, int dest_size);
                                   
void VTScreen_set_reverse_video(VTScreen *screen, int on);
void VTScreen_scroll(VTScreen *screen, double top);
void VTScreen_scroll_by_line(VTScreen *screen, int num_line);
void VTScreen_set_view(VTScreen *screen, VTScreenView *view);


#define VTScreen_is_decckm(scr) (scr->decmodes.decckm == ON)
#define VTScreen_set_decckm(scr,flag) (scr->decmodes.decckm = flag)

#define VTScreen_is_decanm(scr) (scr->decmodes.decanm == ON)
#define VTScreen_set_decanm(scr,flag) (scr->decmodes.decanm = flag)

#define VTScreen_is_deccolm(scr) (scr->decmodes.deccolm == ON)
#define VTScreen_set_deccolm(scr,flag) (scr->decmodes.deccolm = flag)

#define VTScreen_is_decsclm(scr) (scr->decmodes.decsclm == ON)
#define VTScreen_set_decsclm(scr,flag) (scr->decmodes.decsclm = flag)

#define VTScreen_is_decscnm(scr) (scr->decmodes.decscnm == ON)
#define VTScreen_set_decscnm(scr,flag) (scr->decmodes.decscnm = flag)

#define VTScreen_is_decom(scr) (scr->decmodes.decom == ON)
#define VTScreen_set_decom(scr,flag) (scr->decmodes.decom = flag)

#define VTScreen_is_decawm(scr) (scr->decmodes.decawm == ON)
#define VTScreen_set_decawm(scr,flag) (scr->decmodes.decawm = flag)

#define VTScreen_is_decinlm(scr) (scr->decmodes.decinlm == ON)
#define VTScreen_set_decinlm(scr,flag) (scr->decmodes.decinlm = flag)

#define VTScreen_is_dectcem(scr) (scr->decmodes.dectcem == ON) 
#define VTScreen_set_dectcem(scr,flag) (scr->decmodes.dectcem = flag)


#ifdef BIDI
void VTScreen_set_direction(VTScreen *screen,int dir);
int VTScreen_is_direction_LTR(VTScreen *screen);
#endif /* BIDI */
#endif  /* __VTSCREEN_INTERNAL_H__ */
