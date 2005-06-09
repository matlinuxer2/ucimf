/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <sys/ioctl.h>

#include <iterm/unix/ttyio.h>
#include "fbiterm.h"

/* Initialize Terminal IO */
static TerminalIO *
init_io ()
{
  char *defaultShell = "/bin/sh";
  char *shell;
  char *program[] = { defaultShell, NULL };

  putenv ("TERM=iterm");
  shell = getenv ("SHELL");
  if (shell != NULL && shell[0] != '\0')
    program[0] = shell;

  return (TerminalIO *) TtyTerminalIO_new (pIterm->cols, pIterm->rows,
                                           *program, program);
}

/* Initialize VT machine */
int
InitVt ()
{
  /* create screen view object */
  pIterm->view = VTScreenView_new ();
  if (pIterm->view == NULL)
    {
      fprintf (stderr, "VTScreenView_new() failed.\n");
      return -1;
    }

  /* create terminal IO object */
  ioctl (0, TIOCSCTTY, 0);
  pIterm->tio = init_io (pIterm);
  if (pIterm->tio == NULL)
    {
      fprintf (stderr, "init_io() failed.\n");
      return -1;
    }

  /* create VTCore object */
  pIterm->vtcore_ptr =
    VTCore_new (pIterm->tio, pIterm->cols, pIterm->rows, 100);
  if (pIterm->vtcore_ptr == NULL)
    {
      fprintf (stderr, "VTCore_new() failed.\n");
      return -1;
    }

  /* set screen view object pointer to VTCore */
  VTCore_set_screen_view (pIterm->vtcore_ptr, pIterm->view);

  /* set exit callback function */
  VTCore_set_exit_callback (pIterm->vtcore_ptr, (void *) clean_up);
  return 0;
}
