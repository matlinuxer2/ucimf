/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/**
 * @file state.h
 */
#ifndef __STATE_H__
#define __STATE_H__
#include "mbchar.h"

enum {
    N_TRANS = 256
};
  /**< Max number of translations, means byte*/

typedef void  (*callback_func)(void *,Char *,int *, int);
  /**< typedef of callback function */

/**
 * @struct _Handler
 * Handler class.
 * This class is for internal use only.
 * This struct is typedefed to `Handler'.
 */
typedef struct _Handler
{
  struct _State *next_state;
      /**< reference to next state */
  void *object;
      /**< reference to object of method */
  void (*update)(void *object,Char* ch, int* params,int n_params);
      /**< Handler method
       * @param object reference to registered object
       * @param ch reference to Char object which finally invokes this method
       * @param params reference to parameter array
       * @param n_params number of parameter
       */
} Handler;
/**< typedefed struct of struct _Handler */

/**
 * @struct _State
 * State class.
 * This class is for internal use only.
 * This struct is typedefed to `State'.
 */
typedef struct _State
{
  Handler sequence[N_TRANS];
      /**< handlers */
  Handler when_printable;
      /**< a hander for only printable character */
  int *params;
      /**< parameters */
  int n_params;
      /**< number of parameters */

  int max_params;
      /**< max number of parameters */
  
  void (*get_in)(struct _State *, Char *);
      /**< method called when state changed
       * @param state reference to State object
       * @param ch reference to Char object to kick the method
       */
  
      /*
  void (*get_out)(Char *ch);
      */
#ifdef DEBUG
  char state_name[32];
#endif
} State;
/**< typedefed struct of struct _State */

void State_register_char(State *state, State *next_state,
                         int c, callback_func func,void *obj);
State *State_parse(State *state,Char *ch);
void State_store_digit(void *object, Char *ch, int *params, int n_params);
void State_next_param(void *object, Char *ch, int *params, int n_params);
void State_init(State *state, int max_params);

#define ESC '\033'
#define DEL '\177'
#define DEFAULT_PARAM  -1

#endif
