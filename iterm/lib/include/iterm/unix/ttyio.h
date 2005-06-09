/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html
*/
/**
 * @file ttyio.h
 * TtyIO Object definition, which implementers @link TerminalIO IO Interface
 * @endlink .
 */
#ifndef __TTY__
#define __TTY__
#include <termios.h>
#include <sys/types.h>
#include "iterm/io.h"

/**
 * TTYIO  Constructor. \n
 * Meaings of programe_name and argv are same as execvp(3)
 * @param width width of window
 * @param height height of window
 * @param program_name name of program
 * @param argv argments for program terminated by NULL
 * @return reference to ttyio object
 */
TerminalIO* TtyTerminalIO_new(int width,int height,
                                char *program_name, char *argv[]);

/**
 * TTYIO Destructor.
 * @param tio reference to ttyio object
 */
void TtyTerminalIO_destroy(TerminalIO *tio);

/**
 * Obtain associated file descriptor. \n
 * This is a method only for TtyTerminalIO.
 * @param tio reference to ttyio object
 */
int TtyTerminalIO_get_associated_fd(TerminalIO *tio);
#endif
