/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <locale.h>
#include <stdint.h>
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

/**** externel ****/
#include "iiimccf.h"
#include <iiimcf.h>
#include <iiimp-keycode.h>

int cur_col;
int cur_row;

/*
 *  Mapping: ASCII keychar --> IIIMF keycode
 */

static const int keychar_to_keycode[] = {
  // ascii code 00-0
	IIIMF_KEYCODE_UNDEFINED, 
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_TAB,	 

        // 10-19
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_CLEAR,
	IIIMF_KEYCODE_ENTER,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,

  	// 20-29
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_ESCAPE,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,

        // 30-39
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_UNDEFINED,
	IIIMF_KEYCODE_SPACE,
	IIIMF_KEYCODE_EXCLAMATION_MARK,
	IIIMF_KEYCODE_QUOTEDBL,
	IIIMF_KEYCODE_NUMBER_SIGN,
	IIIMF_KEYCODE_DOLLAR,
	IIIMF_KEYCODE_UNDEFINED, 		/* % */
	IIIMF_KEYCODE_AMPERSAND,
	IIIMF_KEYCODE_QUOTE,

	// 40-49
	IIIMF_KEYCODE_LEFT_PARENTHESIS,
	IIIMF_KEYCODE_RIGHT_PARENTHESIS,
	IIIMF_KEYCODE_ASTERISK,
	IIIMF_KEYCODE_PLUS,
	IIIMF_KEYCODE_COMMA,
	IIIMF_KEYCODE_MINUS,
	IIIMF_KEYCODE_PERIOD,
	IIIMF_KEYCODE_SLASH,
	IIIMF_KEYCODE_0,
	IIIMF_KEYCODE_1,

	// 50-59
	IIIMF_KEYCODE_2,
	IIIMF_KEYCODE_3,
	IIIMF_KEYCODE_4,
	IIIMF_KEYCODE_5,
	IIIMF_KEYCODE_6,
	IIIMF_KEYCODE_7,
	IIIMF_KEYCODE_8,
	IIIMF_KEYCODE_9,
	IIIMF_KEYCODE_COLON,
	IIIMF_KEYCODE_SEMICOLON,

	// 60-69
	IIIMF_KEYCODE_LESS,
	IIIMF_KEYCODE_EQUALS,
	IIIMF_KEYCODE_GREATER,
	IIIMF_KEYCODE_UNDEFINED,		/* ? */
	IIIMF_KEYCODE_AT,
	IIIMF_KEYCODE_A,
	IIIMF_KEYCODE_B,
	IIIMF_KEYCODE_C,
	IIIMF_KEYCODE_D,
	IIIMF_KEYCODE_E,

	// 70-79
	IIIMF_KEYCODE_F,
	IIIMF_KEYCODE_G,
	IIIMF_KEYCODE_H,
	IIIMF_KEYCODE_I,
	IIIMF_KEYCODE_J,
	IIIMF_KEYCODE_K,
	IIIMF_KEYCODE_L,
	IIIMF_KEYCODE_M,
	IIIMF_KEYCODE_N,
	IIIMF_KEYCODE_O,

	// 80-89
	IIIMF_KEYCODE_P,
	IIIMF_KEYCODE_Q,
	IIIMF_KEYCODE_R,
	IIIMF_KEYCODE_S,
	IIIMF_KEYCODE_T,
	IIIMF_KEYCODE_U,
	IIIMF_KEYCODE_V,
	IIIMF_KEYCODE_W,
	IIIMF_KEYCODE_X,
	IIIMF_KEYCODE_Y,

	// 90-99
	IIIMF_KEYCODE_Z,
	IIIMF_KEYCODE_OPEN_BRACKET,
	IIIMF_KEYCODE_BACK_SLASH,
	IIIMF_KEYCODE_CLOSE_BRACKET,
	IIIMF_KEYCODE_CIRCUMFLEX,
	IIIMF_KEYCODE_UNDERSCORE,
	IIIMF_KEYCODE_BACK_QUOTE,
	IIIMF_KEYCODE_A,
	IIIMF_KEYCODE_B,
	IIIMF_KEYCODE_C,

	// 100-109
	IIIMF_KEYCODE_D,
	IIIMF_KEYCODE_E,
	IIIMF_KEYCODE_F,
	IIIMF_KEYCODE_G,
	IIIMF_KEYCODE_H,
	IIIMF_KEYCODE_I,
	IIIMF_KEYCODE_J,
	IIIMF_KEYCODE_K,
	IIIMF_KEYCODE_L,
	IIIMF_KEYCODE_M,

	// 110-119
	IIIMF_KEYCODE_N,
	IIIMF_KEYCODE_O,
	IIIMF_KEYCODE_P,
	IIIMF_KEYCODE_Q,
	IIIMF_KEYCODE_R,
	IIIMF_KEYCODE_S,
	IIIMF_KEYCODE_T,
	IIIMF_KEYCODE_U,
	IIIMF_KEYCODE_V,
	IIIMF_KEYCODE_W,

	// 120-126
	IIIMF_KEYCODE_X,
	IIIMF_KEYCODE_Y,
	IIIMF_KEYCODE_Z,
	IIIMF_KEYCODE_BRACELEFT,
	IIIMF_KEYCODE_UNDEFINED,		/* | */
	IIIMF_KEYCODE_BRACERIGHT,
	IIIMF_KEYCODE_DELETE,		/* ~ */
	IIIMF_KEYCODE_BACK_SPACE,		/* BACKSPACE */
};


int keyinput_to_keyevent( char* buf, int buf_len, int* p_keycode, int* p_keychar, int* p_modifier )
{
  if( buf_len== 1 )
  {
    int ich = (int) buf[0];
    *p_keychar = ich;
    *p_keycode = keychar_to_keycode[ ich ];
    *p_modifier = 0; // default as zero.
  }
  else 
    if( buf_len == 3 && buf[0]==27 && buf[1]==91 )
  {
    switch( buf[2] )
    {
      case 65:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_UP;
	*p_modifier = 0; // default as zero.
	break;

      case 66:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_DOWN;
	*p_modifier = 0; // default as zero.
	break;
	
      case 67:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_RIGHT;
	*p_modifier = 0; // default as zero.
	break;
	
      case 68:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_LEFT;
	*p_modifier = 0; // default as zero.
	break;

      default:
	break;
    }
  }
  else 
    if( buf_len == 4 && buf[0]==27 && buf[1]==91 && buf[4]==126 )
  {
    switch( buf[3] )
    {
      case 53:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_PAGE_UP;
	*p_modifier = 0; // default as zero.
	break;
	
      case 54:
	*p_keychar = 0;
	*p_keycode = IIIMF_KEYCODE_PAGE_DOWN;
	*p_modifier = 0; // default as zero.
	break;
      default:
	break;
    }
  }
  else
  {
    *p_keychar = 0;
    *p_keycode = 0;
    *p_modifier = 0; // default as zero.
  }
  
  return 0;
}


int get_cursor_position()
{
  int pos_x, pos_y;
  int var=1; // vary with depth 8=1,16=2,24=3,32=4
  
  /*
  int col = pIterm->vtcore_ptr->screen->cursor_x;
  int row = pIterm->vtcore_ptr->screen->cursor_y;
  */

  pos_x = var * cur_col * (pIterm->asc_font->cell_width);
  pos_y = cur_row * (pIterm->asc_font->cell_height);
  //pos_y = cur_row * (pIterm->fb->line_length) * (pIterm->asc_font->cell_height);
  
  iiimccf_pos( pos_x, pos_y );
  
  return 0;
}



/******************/


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
    fd_set rfds;	// declare a file descriptor set
    struct timeval timeout;

    /* Argument for IIIMCF input switch. */
    int SWITCH_TO_IIIMCF= 0;
	
	/* Import some definition from input.h to here */
        //unsigned char *buf2;
	//int *buf2_len;
	//iiimccf = iiimccf_new( buf, &ret, buf2, buf2_len );
	iiimccf_init(); 
	
	
    while (1)
      {
	FD_ZERO (&rfds);	// zeroize
	FD_SET (0, &rfds);	// add a new fd STDIN
	FD_SET (fd, &rfds);	// add a new fd of "fd"
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;
	if (fd > max)
	  max = fd;

	ret = select (max + 1, &rfds, NULL, NULL, &timeout);
	if (ret == 0 || (ret < 0 && errno == EINTR))
	  continue;

	if (ret < 0)
	  perror ("select");

	if (FD_ISSET (0, &rfds))  // if STDIN(0) is in the fd_set
	  {
	    ret = read (0, buf, sizeof (buf));
	    
	    
	    /* "203" has been registed for "Ctrl-Space" in input.c */    	    
	    if( buf[0] == 203 )
	    {
	       if( SWITCH_TO_IIIMCF == 0 ){
		 SWITCH_TO_IIIMCF = 1;
		 iiimccf_on();
	       }else{
		 SWITCH_TO_IIIMCF = 0;
		 iiimccf_off();
	       }
	       continue;
	    }
	    
	    if( buf[0] == 204 && SWITCH_TO_IIIMCF == 1 )
	    {
	      iiimccf_change_ims();
	      continue;
	    }
	    
	    if (ret == 1 && !(SWITCH_TO_IIIMCF==1) )
            {
              int i;
              for (i = 0; i < ret; i++)
              {
                if ( keypress (buf[i]) == False )
                    write (fd, &buf[i], 1);
              }
            }
	    /* if SWITCH_TO_IIIMCF is on, then redirect the input to iiimcf*/
	    else if( SWITCH_TO_IIIMCF == 1 )
	    {
	        get_cursor_position();
	  
                int keycode, keychar, modifier;
                
		keyinput_to_keyevent( buf, ret, &keycode, &keychar, &modifier ); 
		iiimccf_proc( keycode, keychar, modifier );
		
		char* committed_buf;
		int committed_buf_len=0 ;
		committed_buf_len = iiimccf_result( &committed_buf );
		if ( committed_buf_len > 0 ){
		  write( fd, committed_buf, committed_buf_len );
		}else{
				continue;
		}
	    }
	    else if( ret > 1 )
	    {
              if (keypress2(buf,ret) == False)
                  write (fd, buf, ret);
	    }
			 
	  }
	  else if (FD_ISSET (fd, &rfds)) // if the foreign 'fd' is in the fd_set
	  {
	    if (ret > 0)
	    {
              if( SWITCH_TO_IIIMCF == 1 )
		;//iiimccf_pop(); // save the IIIMCCF display first
	      
	      VTCore_dispatch(pIterm->vtcore_ptr);
	      
	      if( SWITCH_TO_IIIMCF == 1 )
		;//iiimccf_push();  // then restore IIIMCCF display
	    }
	  }
	  
      }
	  
      /* Exit the iiimcf client */
      iiimccf_exit();
	  
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
