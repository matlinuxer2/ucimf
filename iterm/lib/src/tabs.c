/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <string.h>
#include "tabs.h"

/**
 * clear all tabstop.
 * @param tabstop TABS class
 */
void TABS_clear_all_tabs(TABS tabstop)
{
  memset(tabstop,0,sizeof(TABS));
}

/**
 * clear specified column's tabstop.
 * @param tabstop TABS class
 * @param col column to clear tabstop
 */
void TABS_clear_tab(TABS tabstop,int col)
{
      /* clear tabular set at position `col'. col should be positive */
  tabstop[col/8] &= ~(1<<(col%8));
}

/**
 * set tabstop at specified column.
 * @param tabstop TABS class
 * @param col column to set tabstop
 */
void TABS_set_tab(TABS tabstop,int col)
{
      /* set tabular at position `col'. col should be positive  */
  tabstop[(col/8)] |=  (1<<(col%8));
}

/**
 * initialize tabstops at each 8 columns
 * @param tabstop TABS class
 */
void TABS_init_tabs(TABS tabstop)
{
  int i;
  TABS_clear_all_tabs(tabstop);
      /* set tabular at each 8 columns */
  for(i=0;i<MAX_TAB_BITS;i+=8)
      TABS_set_tab(tabstop,i);
}

/**
 * find next tab stop.
 * @param tabstop TABS class
 * @param col search start column
 */
int TABS_get_next_tab(TABS tabstop, int col)
{
  int i;
  for(i=col+1;i<MAX_TAB_BITS;i++)
      if(tabstop[(i/8)] &  (1<<(i%8)))
          return i;
  return col;
}

/**
 * find prev tab stop.
 * @param tabstop TABS class
 * @param col search start column
 */
int TABS_get_prev_tab(TABS tabstop, int col)
{
  int i;
  int prev = 0;
  col = MAX_TAB_BITS < col ? MAX_TAB_BITS : col;
  for(i=0;i<col;i++)
      if(tabstop[(i/8)] &  (1<<(i%8)))
          prev = i;
  return prev;
}
