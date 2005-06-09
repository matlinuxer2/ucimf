/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/**
 * @file vt100.h
 */
#ifndef __VT100__
#define __VT100__
#if defined(HAVE_CONFIGH_H)
# include "../config.h"
#endif
#include <limits.h>
#include "state.h"

/** enumeration of states VT100 transits */
typedef enum
{
    GROUND_STATE,
    ESC_STATE,
    CSI_STATE,
    IGN_STATE,
    IES_STATE,
    SCS_STATE,
    SCR_STATE,
    EIG_STATE,
    DEC_STATE,
    OSC_STATE,
    BIDI_STATE,
    NUM_STATE,
    NO_MOVE
}state_t;

/**
 * @struct _VT100
 * VT100 class.
 * VT100 protocol machine
 * This struct is typedefed to `VT100'.
 */
typedef struct _VT100 {
  State all_states[NUM_STATE];
      /**< all of states which VT100 object moves to */
  State *current_state;
      /**< reference to current state */
}VT100;
/**< typedefed struct of struct _VT100 */


/** VT100 methods */
VT100 *VT100_new();
void VT100_parse(VT100 *vt100, Char *ch);
void VT100_register_sequence(VT100 *vt100,unsigned char *sequence,
                             state_t next_state, callback_func func,
                             void *obj);
void VT100_destroy(VT100 *vt100);
/** VT100 methods end */


#endif
