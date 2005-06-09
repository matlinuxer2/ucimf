/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html
*/
/**
 * @file core.h
 * VTCore class definition.
 */
/** @page index
 * <h2>Internationalized Terminal Emulator Library</h2>
 * This is a portable library for internationalized terminal emulator.
 * All you need to make terminal emulator is to implements Callback functions,
 * like a drawing string on specfic column and row, or set fore/background
 * color and so on.
 */
#ifndef __VTCORE_H__
#define __VTCORE_H__
#include <stdio.h>
#include <wchar.h>
#include "io.h"
#include "screen.h"

enum {
    VTM_SHIFT = 0x0004,   /**< Shift  modifier */
    VTM_META  = 0x0008,   /**< Meta modifier */
    VTM_CONTROL = 0x0010,   /**< Control modifier */
    VTK_LEFT = 0xFF51,   /**< cursor key left */
    VTK_UP =   0xFF52,   /**< cursor key up */
    VTK_RIGHT = 0xFF53,   /**< cursor key right */
    VTK_DOWN = 0xFF54,   /**< cursor key down */
    VTK_HOME = 0xFF55,   /**< home key */
    VTK_END  = 0xFF56,   /**< end key */
    VTK_PAGE_UP = 0xFF57, /**< page up key */
    VTK_PAGE_DOWN = 0xFF58, /**< page down key */
    VTK_INSERT = 0xFF59,  /**< insert key */
    VTK_DELETE = 0xFF5A, /**< delete key */

    VTK_CR =  0xFF0D,   /**< Enter/Return Key */

    VTK_KP_0  =   0xFF00, /**< KeyPad 0 Key */
    VTK_KP_1  =   0xFF01, /**< KeyPad 1 Key */
    VTK_KP_2  =   0xFF02, /**< KeyPad 2 Key */
    VTK_KP_3  =   0xFF03, /**< KeyPad 3 Key */
    VTK_KP_4  =   0xFF04, /**< KeyPad 4 Key */
    VTK_KP_5  =   0xFF05, /**< KeyPad 5 Key */
    VTK_KP_6  =   0xFF06, /**< KeyPad 6 Key */
    VTK_KP_7  =   0xFF07, /**< KeyPad 7 Key */
    VTK_KP_8  =   0xFF08, /**< KeyPad 8 Key */
    VTK_KP_9  =   0xFF09, /**< KeyPad 9 Key */
    
    VTK_KP_DASH   = 0xFF0A, /**< KeyPad Dash/Minus/Hyphen Key */
    VTK_KP_COMMA  = 0xFF0B, /**< KeyPad Comma Key */
    VTK_KP_PERIOD = 0xFF0C, /**< KeyPad Period Key */
    VTK_KP_ENTER  = 0xFF0D, /**< KeyPad Enter/Return Key */
    
    VTK_KP_PF1  =   0xFF0E, /**< KeyPad PF1 Key */
    VTK_KP_PF2  =   0xFF0F, /**< KeyPad PF2 Key */
    VTK_KP_PF3  =   0xFF10, /**< KeyPad PF3 Key */
    VTK_KP_PF4  =   0xFF11, /**< KeyPad PF4 Key */
    
    VTK_F1   =      0x1000, /**< F1 Key */
    VTK_F2   =      0x1001, /**< F2 Key */
    VTK_F3   =      0x1002, /**< F3 Key */
    VTK_F4   =      0x1003, /**< F4 Key */
    VTK_F5   =      0x1004, /**< F5 Key */
    VTK_F6   =      0x1005, /**< F6 Key */
    VTK_F7   =      0x1006, /**< F7 Key */
    VTK_F8   =      0x1007, /**< F8 Key */
    VTK_F9   =      0x1008, /**< F9 Key */
    VTK_F10  =      0x1009, /**< F10 Key */
    VTK_F11  =      0x1010, /**< F11 Key */
    VTK_F12  =      0x1011, /**< F12 Key */
    VTK_F13  =      0x1012, /**< F13 Key */
    VTK_F14  =      0x1013, /**< F14 Key */
    VTK_F15  =      0x1014, /**< F15 Key */
    VTK_F16  =      0x1015, /**< F16 Key */
    VTK_F17  =      0x1016, /**< F17 Key */
    VTK_F18  =      0x1017 /**< F18 Key */
};


struct _VTScreen;
struct _VT100;
/**
 * @struct VTCore
 * VTCore class.  Model of Virtual Terminal \n
 * Users must manipulate thd Model through methods of this class. \n
 *
 */
typedef struct _VTCore
{
  struct _VTScreen *screen;
      /**< pointer of Screen object */
  TerminalIO *host_io;
      /**< pointer of terminalIO object, connected to Host */
  struct _VT100 *vt100;
      /**< pointer of VT100 object, a protocol machine */

  unsigned char buffer[BUFSIZ];
      /**< Input Stream Buffer */

  int remain;
      /**< Number of char remained in the buffer */

  mbstate_t ps;
      /**< State of buffer */

  char **cursor;
      /**< cursor table  */
  void (*exit_callback)(VTScreenView *view);
    /**< callback procedure when connection lose */
  
} VTCore ;
/**< typedefed struct of struct _VTCore  */

VTCore *VTCore_new(TerminalIO *io, int num_cols, int num_rows,
                   int num_history);

void VTCore_destroy(VTCore *core);
int VTCore_write(struct _VTCore *core, char *mb, int length);
int  VTCore_send_key(VTCore *core, int type);
void VTCore_redraw(VTCore *core, int scol, int srow,int ecol, int erow);
void VTCore_set_screen_view(VTCore *core, VTScreenView *view);
void VTCore_set_screen_size(VTCore *core, int width, int height);
void VTCore_start_selection(VTCore *core, int scol, int srow, int type);
void VTCore_extend_selection(VTCore *core, int scol, int srow);
void VTCore_copy_buffer(VTCore *core, int scol, int srow,
                        int ecol, int erow, char *dest, int dest_size);
void VTCore_copy_selected_buffer(VTCore *core, char *dest, int dest_size);
void VTCore_clear_selection(VTCore *core);
void VTCore_scroll(VTCore *core, double top);
void VTCore_set_exit_callback(VTCore *core, void (*proc)(VTScreenView *view));
void VTCore_set_reverse_video(VTCore *core, int on);
void VTCore_scroll_up(VTCore *core, int num_line);
void VTCore_scroll_down(VTCore *core, int num_line);
int VTCore_send_mouse_position(VTCore *core, int button, int modifiers,
                               int pressed, int col, int row);
void VTCore_dispatch(VTCore *core);
#ifdef BIDI
void VTCore_set_direction(VTCore *core, int dir);
int VTCore_is_direction_LTR(VTCore *core);
#endif
#endif

