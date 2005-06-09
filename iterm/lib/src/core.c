/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#if defined(HAVE_CONFIGH_H)
# include "../config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#define __USE_XOPEN
#include <wchar.h>
#undef __USE_XOPEN
#include <wctype.h>
#include <string.h>
#include "iterm/core.h"
#include "iterm/io.h"
#include "mbchar.h"
#include "screen_internal.h"
#include "vt100.h"

char SPLASH_MESSAGE[] = "\x1b[44;33;1m> Internationalized Terminal Emulator - Since 2002 <\x1b[m\n\n\r";

static char *CSI = "\033[";
static char *SS3 = "\033O";

static char *ltr_cursor[] = {"D","A","C","B"};
#ifdef BIDI
static char *rtl_cursor[] = {"C","A","D","B"};
#endif

static char *paging[] = {"1~","4~","5~","6~","2~","3~"};

static char *function[] = {"11","12","13","14","15","17",
                           "18","19","20","21","23","24",
                           "25","26","28","29","31","32"};

static char *keypadnum[] = {"0","1","2","3","4","5","6","7","8","9",
                            "-",",",".","\r"};

static char *keypadapl[] = {"p","q","r","s","t","u","v","w","x","y",
                            "m","l*","n","M"};

#define IsCursorKey(keysym) ((VTK_LEFT <= (keysym))  && ((keysym) <= VTK_DOWN))
#define IsPagingKey(keysym) ((VTK_HOME <= (keysym))  && ((keysym) <= VTK_DELETE))
#define IsFunctionKey(keysym) ((VTK_F1 <= (keysym))  && ((keysym) <= VTK_F18))
#define IsKeypadKey(keysym) ((VTK_KP_0 <= (keysym))  && ((keysym) <= VTK_KP_ENTER))

#define VTCore_call_callback(core) \
 if(core->exit_callback) core->exit_callback(core->screen->screen_view)

static int host_thread(void *arg)
{
  VTCore *core = (VTCore *)arg;
  int length = 0;
  unsigned char *bufp;
  Char mbchar;
  wchar_t wc;
  size_t mb_length;
  mbstate_t ps_back;
  int wc_width;
  int printable;

  length = core->host_io->read(core->host_io,
                               core->buffer+core->remain,
                               (BUFSIZ-core->remain));
  if(length < 0)
  {
    VTCore_call_callback(core);
    return 1;
  }
  else
  {
    length += core->remain;
    bufp = core->buffer;
    core->remain = length;
    while(core->remain > 0)
    {
      ps_back = core->ps;
      mb_length = mbrtowc(&wc,(char *)bufp,core->remain,&core->ps);
      if( mb_length == (size_t)-1 )
      {
            /* invalid.  asume 1 byte 1 char 1 column and not printable */
        mb_length = 1;
        wc_width = 1;
        printable = 0;
        core->ps = ps_back;
      }
      else if ( mb_length == (size_t)-2 )
      {
            /*  bytes shorted */
        if (MB_CUR_MAX > core->remain)
        {
          memmove(core->buffer,bufp,core->remain);
          core->ps = ps_back;
          break;
        }
        else /* MB_CUR_MAX < core->remain */
        {
            /* invalid.  asume 1 byte 1 char 1 column and not printable */
          mb_length = 1;
          wc_width = 1;
          printable = 0;
          core->ps = ps_back;
        }
      }
      else if ( mb_length == (size_t)0 )
      {
            /* \0 character found, assume  1 byte 1 char 1 column */
        mb_length = 1;
        wc_width = 1;
        printable = 0;
        memset(&core->ps,0,sizeof(core->ps));
        memset(&ps_back,0,sizeof(ps_back));
      }
      else
      {
            /* valid char */
        wc_width = wcwidth(wc);
        if(wc_width < 0)
        {
          printable = 0;
          wc_width = 1;
        }
        else
            printable = 1;
      }
      Char_init(mbchar,bufp,mb_length,wc_width,printable);
      VT100_parse(core->vt100,&mbchar);
      bufp += mb_length;
      core->remain -= mb_length;
          /*Thread_yield(core->host_thread);*/
    }
    VTScreen_flush(core->screen);
  }
  return 0;
}

/* used for DECID and DA1 */
static void VTCore_report_device_attribute(VTCore *core)
{
      /* responce Advanced Video Option(AVO) */
  char *str = "\033[?1;2c";
  VTCore_write(core,str,strlen(str));
}

/* Identify Terminal(DEC private) ESC Z */
static void VTCore_DECID(void *object, Char *ch, int *params, int n_params)
{
      /* this is obsolete sequence */  
  VTCore_report_device_attribute((VTCore *)object);
}

/**** CSI State */
/* Device Attribute  ESC [ c */
static void VTCore_DA1(void *object, Char *ch, int *params, int n_params)
{
  if(params[0] <= 0)
      VTCore_report_device_attribute((VTCore *)object);
}

/* aka DSR(Device Status Report)  ESC [ n */
static void VTCore_CPR(void *object, Char *ch, int *params, int n_params)
{
  VTCore *core = (VTCore *)object;
  char str[256];

  if( params[0] == 5 )
  {
    sprintf(str,"\033[0n");
  }
  else if ( params[0] == 6)
  {
    sprintf(str,"\033[%d;%dR",
            core->screen->cursor_y+1,
            core->screen->cursor_x+1);
  }
  else
      return ;
  VTCore_write(core,str,strlen(str));
}
/* Request Terminal Parameters  ESC [ x */
static void VTCore_DECREQTPARM(void *object, Char *ch, int *params, int n_params)
{
  char str[256]; /* enough? */
  int sol;

  if(params[0] < 0)
      sol = 2;
  else if(params[0] < 2)
      sol = params[0] + 2;
  else
      return;
      /* ESC[<sol>;<parity>;<nbits>;<xspeed>;<rspeed>;<clkmul>;<flags>x */
      /* sol;no parity;eight bits;transmit 9600;receive 9600; ? ; ? */
  sprintf(str,"\033[%d;1;1;112;112;1;0x",sol);
  VTCore_write((VTCore *)object,str,strlen(str));
}
/**** CSI State end */


/**
 * initialize VTCore
 * @param screen reference to VTScreen object
 * @param vt100 reference to VT100 object
 * This method connects VTCore to VT100
 */
static void VTCore_init(VTCore *core, VT100 *vt100)
{

      /* CSI state */
  VT100_register_sequence(vt100,(unsigned char *)"\033[c", GROUND_STATE,
                          VTCore_DA1,core);
  VT100_register_sequence(vt100,(unsigned char *)"\033[n", GROUND_STATE,
                          VTCore_CPR,core);
  VT100_register_sequence(vt100,(unsigned char *)"\033[x",GROUND_STATE,
                          VTCore_DECREQTPARM,core);

      /* ESC state */
  VT100_register_sequence(vt100,(unsigned char *)"\033Z",GROUND_STATE,
                          VTCore_DECID,core);
  
}

/** method to write input data to Host.
 * @param core reference to VTCore object
 * @param mb reference to multibyte character
 * @param length of character to write
 */
int VTCore_write(struct _VTCore *core, char *mb, int length)
{
  int ret;
      /* write from both threads, so need protection */
  ret = core->host_io->write(core->host_io,mb,length);
  return ret;
}

/** method to request redraw to screen model.
 * @param core reference to VTCore object
 * @param scol top left column
 * @param srow top left row
 * @param ecol bottom right column exclusively
 * @param erow bottom right row exclusively
 */
void VTCore_redraw(VTCore *core, int scol, int srow,int ecol, int erow)
{
  VTScreen_redraw(core->screen,scol,srow,ecol,erow,1);
}

/** method to send specified key
 * @param core reference to VTCore object
 * @param type key type
 */
int  VTCore_send_key(VTCore *core, int type)
{
  char string[16]; /* enough? */

  if(type == VTK_CR)
  {
    if(core->screen->linefeed)
        strcpy(string,"\r\n");
    else
        strcpy(string,"\r");
  }
  else  if(IsCursorKey(type))

  {
    if(core->screen->decmodes.decckm)
        strcpy(string,SS3);
    else
        strcpy(string,CSI);
    strcat(string,core->cursor[type-VTK_LEFT]);
  }
  else if(IsPagingKey(type))
  {
    strcpy(string,CSI);
    strcat(string,paging[type-VTK_HOME]);
  }
  else if(IsFunctionKey(type))
  {
    strcpy(string,CSI);
    strcat(string,function[type-VTK_F1]);
    strcat(string,"~");
  }
  else if(IsKeypadKey(type))
  {
    if(core->screen->deckpam)
    {
      strcpy(string,SS3);
      strcat(string,keypadapl[type-VTK_KP_0]);
    }
    else
        strcpy(string,keypadnum[type-VTK_KP_0]);
  }
  else 
      string[0] = 0; /* do nothing */
  return VTCore_write(core,string,strlen(string));
}

/** method to set ScreenView to ScreenModel
 * @param core reference to VTCore object
 * @param view reference to ScreenView object
 */
void VTCore_set_screen_view(VTCore *core, VTScreenView *view)
{
  VTScreen_set_view(core->screen,view);
      
}

/** method to set Screen size
 * @param core reference to VTCore object
 * @param width new number of cols
 * @param height new number of rows
 */
void VTCore_set_screen_size(VTCore *core, int width, int height)
{
  VTScreen_set_size(core->screen,width,height);
  core->host_io->tell_window_size(core->host_io,width,height);
}

/**
 * set reverse video
 * @param core reference to VTCore object
 * @param on 0 will unset reverse video, 1 is opposite
 */
void VTCore_set_reverse_video(VTCore *core, int on)
{
  VTScreen_set_reverse_video(core->screen,on);
}

/**
 * set callback function when host thread finish.
 * @param core reference to VTCore object
 * @param proc reference to function to be called.
 * Function Prototype is void proc(VTScreenView *view), but may be changed ;-)
 */
void VTCore_set_exit_callback(VTCore *core, void (*proc)(VTScreenView *view))
{
  core->exit_callback = proc;
}

/**
 * Scroll buffer
 * @param core reference to VTCore object
 * @param top 0.0 is a beginning of buffer \n
 *        NOTE: top must be 0.0 - 1.0 
 */
void VTCore_scroll(VTCore *core, double top)
{
  VTScreen_scroll(core->screen,top);
}

/**
 * Scroll buffer
 * @param core reference to VTCore object
 * @param num_line number of line to scroll up
 */
void VTCore_scroll_up(VTCore *core, int num_line)
{
  VTScreen_scroll_by_line(core->screen,num_line);
}

/**
 * Scroll buffer
 * @param core reference to VTCore object
 * @param num_line number of line to scroll down
 */
void VTCore_scroll_down(VTCore *core, int num_line)
{
  VTScreen_scroll_by_line(core->screen,-num_line);
}


/**
 * Clear selection.
 * @param core reference to VTCore object
 */
void VTCore_clear_selection(VTCore *core)
{
  VTScreen_clear_selection(core->screen);
}

/**
 * Start selection.
 * @param core reference to VTCore object
 * @param col start column
 * @param row start row
 * @param type selection type
 */
void VTCore_start_selection(VTCore *core, int col, int row, int type)
{
  VTScreen_start_selection(core->screen,col,row,type);
}

/**
 * Extend selection.
 * @param core reference to VTCore object
 * @param col end column
 * @param row end row
 */
void VTCore_extend_selection(VTCore *core, int col, int row)
{
  VTScreen_extend_selection(core->screen,col,row);
}

/**
 * Copy multibyte characters from buffer. \n
 * Caller must allocate dest before calling this method
 * @param core reference to VTCore object
 * @param scol start column
 * @param srow start row
 * @param ecol end column inclusive
 * @param erow end row inclusive
 * @param dest destination
 * @param dest_size size of destination
 */
void VTCore_copy_buffer(VTCore *core, int scol, int srow,
                          int ecol, int erow, char *dest, int dest_size)
{
  VTScreen_copy_buffer(core->screen,scol,srow,ecol,erow,dest,dest_size);
}

/**
 * Copy multibyte characters from buffer. \n
 * Caller must allocate dest before calling this method and specfy
 * selection by @link VTCore::VTCore_start_selection VTCore_start_selection
 * @endlink/@link VTCore::VTCore_extend_selection VTCore_extend_selection
 * @endlink method.
 * @param core reference to VTCore object
 * @param dest destination
 * @param dest_size size of destination
 */
void VTCore_copy_selected_buffer(VTCore *core, char *dest, int dest_size)
{
  VTScreen_copy_selected_buffer(core->screen,dest,dest_size);
}

static void set_splash(VTCore *core,unsigned char *string, int length)
{
  Char mbchar;
  int i=0;

  while(i<length)
  {
    Char_init(mbchar,&string[i],1,1,1);
    VT100_parse(core->vt100,&mbchar);
    i++;
  }
}

/**
 * Constructor of VTCore object.
 * @param io reference to TerminalIO object which VTCore connect to
 * @param num_cols number of columns
 * @param num_rows number of rows
 * @param num_history number of saved rows (not included initial num rows)
 */
VTCore *VTCore_new(TerminalIO *io, int num_cols, int num_rows, int num_history)
{
  VTCore *core;

  if((core = malloc(sizeof(VTCore))) == NULL)
  {
    perror("VTCore_new");
    return NULL;
  }

  if((core->vt100 = VT100_new()) == NULL)
  {
    perror("VTCore_new");
    free(core);
    return NULL;
  }

  memset(&core->ps,0,sizeof(core->ps));
  core->remain = 0;
  core->exit_callback = NULL;
  core->screen = VTScreen_new(num_cols,num_rows,num_history+num_rows);
  VTScreen_init(core->screen,core->vt100);
  VTCore_init(core,core->vt100);
  set_splash(core,(unsigned char *)SPLASH_MESSAGE,strlen(SPLASH_MESSAGE));
  core->cursor = ltr_cursor;
  
  core->host_io = io;

  return core;
}

/**
 * Destructor  of VTCore object.
 * @param core reference to VTCore object
 */
void VTCore_destroy(VTCore *core)
{
  if(core)
  {
    if(core->screen)
        VTScreen_destroy(core->screen);
    if(core->vt100)
        VT100_destroy(core->vt100);
    free(core);
  }
}

/**
 * Dispatch host event;
 * This method doesn't return until host sends bytes.
 * Caller must detect host event before calling this method.
 * @param core reference to VTCore object
 */
void VTCore_dispatch(VTCore *core)
{
  host_thread(core);
}

/**
 * Send mouse cursor position
 * This should be called when
 * @param core reference to VTScreen object.
 * @param button button number 0 =<
 * @param modifiers modifiers of key(VTM_SHIFT, VTM_META or VTM_CONTROL)
 * @param pressed 1 if this button is pressed, 0 if released
 * @param col mouse column
 * @param row mouse row
 * @return if send to the host return 1, else 0.
 *  Caller should NOT do anything after this returns 1;
 */
int VTCore_send_mouse_position(VTCore *core, int button, int modifiers,
                               int pressed,  int col, int row)
{
  char string[8];
  strncpy(string,"\033[M",3);
  switch(core->screen->track_mouse.type)
  {
    case X10_MOUSE: /* X10 compatible  */
        if(modifiers == 0 && pressed != 0)
        {
          string[3] = ' ' + button;
          string[4] = ' ' + col + 1;
          string[5] = ' ' + row + 1;
          string[6] = '\0';
          VTCore_write(core,string,strlen(string));
          return 1;
        }
        break;
    case VT200_MOUSE: /* VT200 compatible */
        if(modifiers == 0 || modifiers == VTM_CONTROL)
        {
          string[3] = ' ' + modifiers + (pressed ? button : 3);
          string[4] = ' ' + col + 1;
          string[5] = ' ' + row + 1;
          string[6] = '\0';
          VTCore_write(core,string,strlen(string));
          return 1;
        }
        return 0;
    case 3:
        return 0;
    default:
	/* do nothing */;
  }
  return 0;
}

#ifdef BIDI
void VTCore_set_direction(VTCore *core, int dir)
{
  VTScreen_set_direction(core->screen,dir);
  if(VTScreen_is_direction_LTR(core->screen))
      core->cursor = ltr_cursor;
  else
      core->cursor = rtl_cursor;
}

int VTCore_is_direction_LTR(VTCore *core)
{
  int ret;
  ret = VTScreen_is_direction_LTR(core->screen);
  return ret;
}

#endif
