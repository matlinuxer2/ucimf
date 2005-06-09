/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/**
 * @file tabs.h
 */	

#ifndef __TABS_H__
#define __TABS_H__

enum {
    BYTE = 8,
    MAX_TAB_BYTES = 40,
        /**< MAX Tab bytes */
    MAX_TAB_BITS = BYTE * MAX_TAB_BYTES
        /**< Max Tab bits, means max column size this class can handle */
};


typedef unsigned char TABS[MAX_TAB_BYTES];
/**< tabulation class */

void TABS_clear_all_tabs(TABS tabstop);
void TABS_clear_tab(TABS tabstop,int col);
void TABS_set_tab(TABS tabstop,int col);
void TABS_init_tabs(TABS tabstop);
int  TABS_get_next_tab(TABS tabstop, int col);
int TABS_get_prev_tab(TABS tabstop, int col);

#endif
