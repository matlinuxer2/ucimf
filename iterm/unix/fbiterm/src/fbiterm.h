/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained 
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#ifndef __ITERM_H__
#define __ITERM_H__

#include <wchar.h>
#include <wctype.h>
#include <signal.h>
#include <termios.h>		/* for termios structure */
#include <signal.h>		/* for signal() */
#include <sys/kd.h>		/* for KDSETMODE/KD_GRAPHICS/KD_TEXT */
#include <linux/vt.h>		/* for VT_* definitions */
#include <sys/ioctl.h>		/* for ioctl */
#include <linux/fb.h>

/* for X font access */
#include "Xmd.h"
#include "fontxlfd.h"
#include "fontmisc.h"
#include "fontstruct.h"
#include "fntfil.h"
#include "bitmap.h"
#include "pcf.h"
#include "bdfint.h"

/* for VTcore access */
#include <iterm/core.h>		/* for VTCorem */
#include "VTScreenView.h"	/* for VTCore/VTScreenView */

#ifndef	False
#  define False 0
#endif
#ifndef	True
#  define True 1
#endif

#ifndef	min
#  define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef	max
#  define max(a,b) (((a)>(b))?(a):(b))
#endif

#define ITERM_VERSION "1.0.0"

#define FB_ENV    "FRAMEBUFFER"
#define FB_DEFDEV "/dev/fb0"

#define DefaultAsc	"/usr/lib/X11/fonts/misc/8x16.pcf.gz"
#define DefaultMB	"/usr/X11R6/lib/X11/fonts/misc/unifont.pcf.gz"
//#define DefaultMB	"/usr/share/fonts/unifont/unifont.pcf.gz"
#define DefaultFont	"/usr/local/share/iterm/fonts/8x16.pcf.gz"

enum ITERM_RETCODE
{
  ITERM_REPLACEFONT = 1,
};

enum ITERM_ERRCODE
{
  SUCCESS = 0,
  FONT_ERROR_EXIT = 1,
  TERM_ERROR_EXIT,
  FB_ERROR_EXIT,
  VT_ERROR_EXIT,
  INPUT_ERROR_EXIT,
};

/* drawing methods */
typedef void (*SetPaletteProc) ();
typedef void (*DrawTextProc) (int col, int row, char *mbstring, int length,
			      int width);
typedef void (*ClearRectProc) (int s_col, int s_row, int e_col, int e_row);
typedef void (*SetRenditionProc) (int bold, int blink, int inverse,
				  int underline, int foreground,
				  int background, char charset);

/* font related structure */
typedef struct _TermFont
{
  char *filename;		/* font file name */
  char *encoding_name;		/* font encoding name */
  FontRec *frec;		/* FontRec structure (defined by X11) */
  int cell_width;		/* cell width per character */
  int cell_height;		/* cell height per character */
}
TermFont;

/* frame buffer structure */
typedef struct _TermFb
{
  int fbfp;			/* frame buffer device descriptor */
  char *fbdev;			/* frame buffer device name */
  int line_length;		/* frame buffer line length */
  int smem_len;			/* frmae buffer memory length */
  unsigned char *buf;		/* frame buffer pointer */
  SetPaletteProc set_palette;	/* procedure for setting palette */
  DrawTextProc draw_text;	/* procedure for drawing text */
  ClearRectProc clear_rect;	/* procedure for clearing rectangle */
  SetRenditionProc set_rendition;	/* procedure for setting rendition */
}
TermFb;

/* iterm structure */
typedef struct _Iterm
{
  VTCore *vtcore_ptr;		/* VTCore pointer */
  VTScreenView *view;		/* VT screen view pointer */
  TerminalIO *tio;		/* terminal IO data pointer */
  TermFont *asc_font;		/* ascii font data pointer */
  TermFont *mb_font;		/* multi byte font data pointer */
  TermFb *fb;			/* frame buffer pointer */
  struct termios ntio;		/* for backup */
  int cell_width;		/* cell width per character */
  int cell_height;		/* cell height per character */
  int cols;			/* total columns (xres / cell_width) */
  int rows;			/* total rows (yres / cell_height) */
  int bold;			/* bold flag */
  int blink;			/* blink flag */
  int underline;		/* underline flag */
  char charset;			/* char area for distinguishing UK or special graph */
  int lock;			/* lock flag for the display control 
				   at the time of the change of a virtual terminal */
  int termInitialized ;         /* flag to check if terminal is initialized */
  int fbInitialized;            /* flag to check if framebuffer is initialized */
  int inputInitialized;        /* flag to check if input is initialized */

}
Iterm;


/* function prototypes */
int InitFont (char *ascfont, char *mbfont);
int InitFb ();
int InitTerm ();
int InitVt ();
int InitInput (void);
void exitFbiterm (int exitcode);

int get_char_len (char *src, mbstate_t * ps);
int keypress (unsigned char ch);
int keypress2 (unsigned char *buffer, int length);
void change_to_graphic (void);
void change_to_text (void);
void clean_up ();
void fb_end ();
void init_tty ();
void restore_keys();
void leave_vt ();
void return_vt ();
void setup_vt_handle (void);
TermFont *load_font (char *input_filename);

/* truecolor 8bit color */
void fb_true8_set_palette (void);
void fb_true8_clear_rect (int s_col, int s_row, int e_col, int e_row);
void fb_true8_set_rendition (int bold, int blink, int inverse, int underline,
			     int foreground, int background, char charset);
void fb_true8_draw_text (int col, int row, char *mbstring, int length,
			 int width);

/* truecolor 16bit color */
void fb_true16_set_palette (void);
void fb_true16_clear_rect (int s_col, int s_row, int e_col, int e_row);
void fb_true16_set_rendition (int bold, int blink, int inverse, int underline,
			      int foreground, int background, char charset);
void fb_true16_draw_text (int col, int row, char *mbstring, int length,
			  int width);

/* truecolor 24bit color */
void fb_true24_set_palette (void);
void fb_true24_clear_rect (int s_col, int s_row, int e_col, int e_row);
void fb_true24_set_rendition (int bold, int blink, int inverse, int underline,
			      int foreground, int background, char charset);
void fb_true24_draw_text (int col, int row, char *mbstring, int length,
			  int width);

/* truecolor 32bit color */
void fb_true32_set_palette (void);
void fb_true32_clear_rect (int s_col, int s_row, int e_col, int e_row);
void fb_true32_set_rendition (int bold, int blink, int inverse, int underline,
			      int foreground, int background, char charset);
void fb_true32_draw_text (int col, int row, char *mbstring, int length,
			  int width);

extern void VTCore_set_direction ();
extern int
wcwidth (wint_t __c)
  __THROW;
     extern unsigned char *get_glyph ();
     extern Iterm *pIterm;

#endif /* __ITERM_H__ */
