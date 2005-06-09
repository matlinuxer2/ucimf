/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html
*/
/**
 * @file screen.h
 * VTScreenView interface definition.
 */
#ifndef __VTSCREEN_H__
#define __VTSCREEN_H__

#define UK_CHARSET 'A'
  /**< Macro of UK Character set specfied by set_rendition */
#define ASCII_CHARSET 'B'
  /**< Macro of ASCII Character set specfied by set_rendition */
#define SPECIAL_GRAPHICS '0'
  /**< Macro of special graphics set specfied by set_rendition */

/** Type definition of On/Off */
enum {
    OFF,
    ON
};

/**
   Type definition of sending mouse position \n
   MOUSE_OFF - not send mouse cursor position \n
   X10_MOUSE - send X10 compatibility mouse cursor position \n
   VT200_MOUSE - send VT200  compatibility mouse cursor position \n
*/
enum {
    MOUSE_OFF,
    X10_MOUSE,
    VT200_MOUSE
};

/**
   Type definition of selecting character \n
   SELECTION_CHAR - Character oriented selection
   SELECTION_WORD - Word oriented selection
   SELECTION_LINE - Line oriented selection
*/
enum {
    SELECTION_CHAR,
    SELECTION_WORD,
    SELECTION_LINE
};

/**
 * @struct _VTScreenView
 * VTScreenView interface. \n
 * View of VTScreen.  User must create instance that implements these methods.
 * To short, this is a set of callback functions which are called by @link
 * VTCore VTCore @endlink
 */
typedef struct _VTScreenView
{
  void *object;
      /**< reference to platform dependent VTScreenView object*/
  void (*draw_text)(struct _VTScreenView *view,int col, int row,
                    char *mbstring, int length,int width);
      /**<
       * draw text in screen.
       * @param view reference to VTScreenView object
       * @param col start column to draw text
       * @param row start row to draw text
       * @param mbstring reference multibyte string
       * @param length length of multibyte string
       * @param width column width of multibyte string
       */

  void (*update_cursor_position)(struct _VTScreenView *view,int col, int row);
      /**<
       * set cursor position
       * @param view reference to VTScreenView object
       * @param col start column to draw text
       * @param row start row to draw text
       */

  void (*set_rendition)(struct _VTScreenView *view,
                        int bold, int blink,
                        int inverse, int underline,
                        int foreground, int background,
                        char charset);
      /**<
       * set graphic rendition of text.
       * @param view reference to VTScreenView object
       * @param bold bold text
       * @param inverse reverse text
       * @param underline underlined text
       * @param foreground foreground color number(1 - 9) may change
       * @param background background color number(1 - 9) may change
       * @param charset graphic character sets \n
       A - UK charset \n
       B - ASCII charset \n
       0 - Special Graphics \n
       1 - Alternate Rom character set \n
       2 - Alternate Rom Special Graphics
       */
  void (*clear_rect)(struct _VTScreenView *view,int s_col,
                     int s_row, int e_col, int e_row);
      /**<
       * clear specified rectangular.
       * @param view reference to VTScreenView object
       * @param s_col top left column
       * @param s_row top left row
       * @param e_col bottom right column
       * @param e_row bottom right row
       */
  void (*swap_video)(struct _VTScreenView *view);
      /**<
       * swap foreground and background color permanently.
       * @param view reference to VTScreenView object.
       *
       * After calling this, whole background color will be swaped.
       */
  
  void (*ring)(struct _VTScreenView *view);
      /**<
       * ring the bell
       * @param view reference to VTScreenView object
       */
  
  void (*resize_request)(struct _VTScreenView *view, int cols, int rows);
      /**<
       * request new size of the screen.
       * @param view reference to VTScreenView object
       * @param cols number of request columns
       * @param rows number of request rows
       * @param cols number of actual new columns
       * @param rows number of actual new rows
       *
       * ScreenView must return actual rows and columns for model
       * know if request had been accepted or not.
       */
  
  void (*notify_osc)(struct _VTScreenView *view, int type,
                     char *params, int n_params);
      /**<
       * notify OSC(operating system command).
       * @param view reference to VTScreenView object
       * @param type type of OSC
       * @param params parameter string
       * @param n_params length of parameter string
       *
       * type is usually(on xterm) decoded as \n
       * 0 - change icon & window title \n
       * 1 - change icon title \n
       * 2 - change window titile \n
       * 46 - change log file \n
       * 50 - change font \n
       */

  void (*update_scrollbar)(struct _VTScreenView *view, int max,
                           int top, int shown);
      /**<
       * notify scrollbar event.
       * @param view reference to VTScreenView object
       * @param max max length of scrollbar
       * @param top point of scrollbar 
       * @param shown length of scrollbar thumb
       */


  void (*scroll_view)(struct _VTScreenView *view, int dest_row,
                      int src_row, int num_line); 
      /**<
       * notify scrollbar event.
       * @param view reference to VTScreenView object
       * @param dest_row destination row position
       * @param src_row source row position
       * @param num_line number of line to bitblit
       */

} VTScreenView;
/**< typedefed struct of struct _VTScreenView */

#define VTScreenView_init(view) \
do \
{ \
  view->object = NULL; \
  view->draw_text = NULL; \
  view->clear_rect = NULL; \
  view->set_rendition = NULL; \
  view->swap_video = NULL; \
  view->resize_request = NULL; \
  view->update_cursor_position = NULL; \
  view->ring = NULL; \
  view->notify_osc = NULL; \
  view->update_scrollbar = NULL; \
  view->scroll_view = NULL; \
} while(0)
/**<
 * utility macro to initialize VTScreenView.
 * @param view reference to VTScreenView Object.
*/

#endif
