/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html
*/
/**
 * @file io.h
 * @link TerminalIO IO Interface @endlink definition.
 */
#ifndef __TERMINAL_IO__
#define __TERMINAL_IO__

struct Concrete_IO;
/**< platform dependent IO object */

/**
 * @struct TerminalIO
 * IO interface, which includes read/write and tell_window_size
 * methods which depends on procol. \n
 * User must not manipulate the interface directory. \n
 * @link ttyio.h TtyTerminalIO @endlink is a class which  implements
 * this interface.
 */
typedef struct _TerminalIO
{
  struct Concrete_IO *concrete_io;
      /**< This is supposed to point platform independent structure */
  int (*read)(struct _TerminalIO *tio,void *buf, int size);
      /**< read method.
       * @param tio reference to TerminalIO object
       * @param buf reference to buffer to store data, caller must allocate memory
       * @param size size of buffer
       */
  int (*write)(struct _TerminalIO *tio,const void *buf, int size);
      /**< write method.
       * @param tio reference to TerminalIO object
       * @param buf reference to buffer to store data
       * @param size size of data to write
       */
  int (*tell_window_size)(struct _TerminalIO *tio,int width, int height);
      /**< inform window size to Host program protocol dependently
       * @param tio reference to TerminalIO object
       * @param width new width of window
       * @param height new height of window
       */
}TerminalIO;
/**< typedefed struct  of struct _TeminalIO */
#endif
