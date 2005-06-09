/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <locale.h>
#include "fbiterm.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#ifdef HAVE_SELECT
#include <errno.h>
#include <iterm/unix/ttyio.h>
#endif

Iterm *pIterm;

void
exitFbiterm (int exitcode)
{
  if (pIterm->fbInitialized) {
    fb_end (pIterm);
  }
  if (pIterm->termInitialized) {
    /* set new keyboard control parameters for new terminal */
    tcsetattr (0, TCSAFLUSH, &(pIterm->ntio));
    change_to_text ();
  }
  if (pIterm->inputInitialized) {
    restore_keys();
  }
  exit (exitcode);
}

static void
catchSignal ()
{
  exitFbiterm (0);
}

static void
iterm_usage (void)
{
  fprintf (stdout, "Usage: iterm [ -a <fontfile> ] [ -m <fontfile> ] [ -v ]\n\
\n\
options:\n\
  -a <fontfile>\tascii text font\n\
  -m <fontfile>\tunicode text font\n\
  -v\t\tprint version information and exit\n\
  -h\t\tthis help message\n");

  exit (0);
}

int
main (int argc, char *argv[])
{
  unsigned char buf[BUFSIZ + 1], *ascfontname, *mbfontname;
  int i;

  setlocale (LC_ALL, "");

  /* allocate memory for Iterm structure */
  pIterm = (Iterm *) malloc (sizeof (Iterm));
  if (pIterm != NULL)
    {
      memset (pIterm, 0x0, sizeof (Iterm));
      pIterm->fb = (TermFb *) malloc (sizeof (TermFb));
      if (pIterm->fb == NULL)
	{
	  fprintf (stderr, "There is not enough memory available now.\n");
	  exit (1);
	}
      pIterm->asc_font = pIterm->mb_font = NULL;
      pIterm->termInitialized = pIterm->fbInitialized = pIterm->inputInitialized = 0;
    }
  else
    {
      fprintf (stderr, "There is not enough memory available now.\n");
      exit (1);
    }

  ascfontname = DefaultAsc;
  mbfontname = DefaultMB;
  while ((i = getopt (argc, argv, "a:m:hv")) != EOF)
    switch (i)
      {
      case 'a':
	if (optarg != NULL)
	  ascfontname = optarg;
	else
	  ascfontname = DefaultAsc;
	break;

      case 'm':
	if (optarg != NULL)
	  mbfontname = optarg;
	else
	  mbfontname = DefaultMB;
	break;

      case 'v':
	fprintf (stdout, "iterm %s\n", ITERM_VERSION);
	exit (0);
	break;

      case 'h':
      default:
	iterm_usage ();
	break;
      }

  if (optind < argc)
    iterm_usage ();

  /* initialize font/framebuffer/terminal/VT/input */
  if (InitFont (ascfontname, mbfontname) < 0)
    exitFbiterm (FONT_ERROR_EXIT);

  if (InitTerm () < 0)
    exitFbiterm (TERM_ERROR_EXIT);
  pIterm->termInitialized = 1;
  
  if (InitFb () < 0)
    exitFbiterm (FB_ERROR_EXIT);
  pIterm->fbInitialized = 1;
  
  if (InitVt () < 0)
    exitFbiterm (VT_ERROR_EXIT);

  if (InitInput () < 0)
    exitFbiterm (INPUT_ERROR_EXIT);
  pIterm->inputInitialized = 1;
  
  signal (SIGHUP, catchSignal);
  signal (SIGTERM, catchSignal);
  signal (SIGSEGV, catchSignal);

#ifdef HAVE_SELECT
  {
    int fd = TtyTerminalIO_get_associated_fd(pIterm->tio);
    int ret;
    int max = 0;
    fd_set rfds;
    struct timeval timeout;

    while (1)
      {
	FD_ZERO (&rfds);
	FD_SET (0, &rfds);
	FD_SET (fd, &rfds);
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;
	if (fd > max)
	  max = fd;

	ret = select (max + 1, &rfds, NULL, NULL, &timeout);
	if (ret == 0 || (ret < 0 && errno == EINTR))
	  continue;

	if (ret < 0)
	  perror ("select");

	if (FD_ISSET (0, &rfds))
	  {
	    ret = read (0, buf, sizeof (buf));
	    if (ret == 1)
            {
              int i;
              for (i = 0; i < ret; i++)
              {
                if (keypress (buf[i]) == False)
                    write (fd, &buf[i], 1);
              }
            }
            else if (ret > 1)
            {
              if (keypress2(buf,ret) == False)
                  write (fd, buf, ret);
            }
	  }
	else if (FD_ISSET (fd, &rfds))
	  {
	    if (ret > 0)
	      VTCore_dispatch(pIterm->vtcore_ptr);
	  }
      }
  }
#else
  {
    int ret;
    for (;;)
      if ((ret = read (0, buf, BUFSIZ)) > 0)
	VTCore_write (pIterm->vtcore_ptr, buf, ret);
  }
#endif
  return SUCCESS;
}
