/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/kd.h>
#include <linux/keyboard.h>

#define BIDI
#include <iterm/core.h>

#include "iterm/vtlayout.h"
#include "fbiterm.h"

/* assigned key code */
enum
{
  SHIFT_PAGEUP = 198,
  SHIFT_PAGEDOWN,
  CTRL_F1,
  ALT_ENTER,
  ALT_ENTER_GR,
};

typedef struct _new_key
{
  int table;
  int key;
  int new_key;
}
new_key;

static new_key nkeys[] = {
  /* [table]        [key]         [new_key] */
  {(1 << KG_SHIFT), KEY_PAGEUP, K (KT_LATIN, SHIFT_PAGEUP)},	/* Shift + PageUp key */
  {(1 << KG_SHIFT), KEY_PAGEDOWN, K (KT_LATIN, SHIFT_PAGEDOWN)},	/* Shift + PageDown key */
  {(1 << KG_CTRL), KEY_F1, K (KT_LATIN, CTRL_F1)},	/* Ctrl + F1 */
  {(1 << KG_ALT), KEY_ENTER, K (KT_LATIN, ALT_ENTER)},	/* Alt + Enter */
  {(1 << KG_ALTGR), KEY_ENTER, K (KT_LATIN, ALT_ENTER)},	/* Alt + Enter (GR) */
};

int
keypress (unsigned char ch)
{
  int res = True;

  switch (ch)
  {
    case SHIFT_PAGEUP:
        VTCore_scroll_up (pIterm->vtcore_ptr, pIterm->rows / 2);
        break;
        
    case SHIFT_PAGEDOWN:
        VTCore_scroll_down (pIterm->vtcore_ptr, pIterm->rows / 2);
        break;
        
    case CTRL_F1:
    case ALT_ENTER:
    case ALT_ENTER_GR:
        if (VTCore_is_direction_LTR (pIterm->vtcore_ptr))
            VTCore_set_direction (pIterm->vtcore_ptr, VT_RTL);
        else
            VTCore_set_direction (pIterm->vtcore_ptr, VT_LTR);
        break;
    case '\r': /* enter key */
        VTCore_send_key(pIterm->vtcore_ptr,(VTK_CR));
        break;
    default:
      res = False;
      break;
    }
  return res;
}

/* handling for escape sequences of linux console */
int keypress2(unsigned char *buf, int length)
{
  int ret = False;

  if( 2 < length )
  {
    if(buf[0] == '\033' )
    {
          /*
            handling cursor keys
            3 bytes 
            CURSOR_UP    - "\x1b[A"
            CURSOR_DOWN  - "\x1b[B"
            CURSOR_RIGHT - "\x1b[C"
            CURSOR_LEFT  - "\x1b[D"
           */
      if(buf[1] == '['  && 'A' <=buf[2] && buf[2] <= 'D')
      {
        switch(buf[2])
        {
          case 'A':
              VTCore_send_key(pIterm->vtcore_ptr,VTK_UP);
              ret = True;
              break;
          case 'B':
              VTCore_send_key(pIterm->vtcore_ptr,VTK_DOWN);
              ret = True;
              break;
          case 'C':
              VTCore_send_key(pIterm->vtcore_ptr,VTK_RIGHT);
              ret = True;
              break;
          case 'D':
              VTCore_send_key(pIterm->vtcore_ptr,VTK_LEFT);
              ret = True;
              break;
          default:
              /* do nothing */;
        }
      }
      else if(buf[1] == 'O')
      {
            /*
              handling keypad keys
              3 bytes 
              KP0 "\x1b[Op"
              KP1 "\x1b[Oq"
              KP2 "\x1b[Or"
              KP3 "\x1b[Os"
              KP4 "\x1b[Ot"
              KP5 "\x1b[Ou"
              KP6 "\x1b[Ov"
              KP7 "\x1b[Ow"
              KP8 "\x1b[Ox"
              KP9 "\x1b[Oy"
              
              KEY_KPCOMMA  I don't know ;)
            */
        if('p' <= buf[2] && buf[2] <= 'y')
        {
          VTCore_send_key(pIterm->vtcore_ptr,VTK_KP_0 + buf[2] - 'p');
          ret = True;
        }
        else if(buf[2] == 'S') /*  KPMINUS "\x1b[OS" */
        {
          VTCore_send_key(pIterm->vtcore_ptr,VTK_KP_DASH);
          ret = True; 
        }
        else if(buf[2] == 'n') /* KPDOT   "\x1b[On" */
        {
          VTCore_send_key(pIterm->vtcore_ptr,VTK_KP_PERIOD);
          ret = True; 
        }
        else if(buf[2] == 'M') /* KPENTER "\x1b[OM" */
        {
          VTCore_send_key(pIterm->vtcore_ptr,VTK_KP_ENTER);
          ret = True; 
        }
      }
      else if( 3 < length )
      {
        if(buf[2] == '[')
        {
              /*
                handling cursor keys
                4 bytes
                FUNC1 "\x1b[[A"
                FUNC2 "\x1b[[B"
                FUNC3 "\x1b[[C"
                FUNC4 "\x1b[[D"
                FUNC5 "\x1b[[E"
                FUNC6 - 18 are same as iterm capabilities
              */
          if('A' <=buf[3] && buf[3] <= 'E')
          {
            VTCore_send_key(pIterm->vtcore_ptr,VTK_F1 + buf[3] - 'A');
            ret = True;
          }
        }
      }
    }
  }
  return ret ;
}

/* Initialize Input */
int
InitInput (void)
{
  int i;
  for (i = 0; i < sizeof (nkeys) / sizeof (new_key); i++)
  {
    struct kbentry entry;
    struct kbentry tmp;
    entry.kb_table = nkeys[i].table;
    entry.kb_index = nkeys[i].key;

        /* save current value */
    ioctl (0, KDGKBENT, &entry);
    tmp.kb_value = entry.kb_value;

    entry.kb_value = nkeys[i].new_key;
    ioctl (0, KDSKBENT, &entry);

        /* save current value */
    nkeys[i].new_key = tmp.kb_value;
  }
  return 0;
}

void restore_keys(void)
{
  int i;
  for (i = 0; i < sizeof (nkeys) / sizeof (new_key); i++)
  {
    struct kbentry entry;
    entry.kb_table = nkeys[i].table;
    entry.kb_index = nkeys[i].key;
    entry.kb_value = nkeys[i].new_key;
    ioctl (0, KDSKBENT, &entry);
  }

}
