/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

#include <stdio.h>
#include <stdlib.h>
#include "state.h"

/* storing digits */
void State_store_digit(void *object, Char *ch, int *params, int n_params)
{
  int tmp;
  State *state = (State *)object;
  tmp = params[state->n_params-1];
  if(tmp == DEFAULT_PARAM)
      tmp = 0;
  state->params[state->n_params-1] = 10 * tmp + (Char_get_first_byte(ch) - '0');
}

void State_next_param(void *object, Char *ch, int *params, int n_params)
{
  State *state = (State *)object;
  if(state->n_params < state->max_params )
      state->params[state->n_params++] = DEFAULT_PARAM;
}

/* State method impl */
void State_register_char(State *state, State *next_state,
                                int c, callback_func func, void *object)
{
  state->sequence[c].next_state = next_state;
  state->sequence[c].update = func;
  state->sequence[c].object = object;
}

/**< parse method
   * @param state reference to State object
   * @param ch reference to Char object to be parsed
   */
State *State_parse(State *state, Char *ch)
{
  State *next_state = state;

  if(Char_get_length(ch) == 1 &&
     (state->sequence[Char_get_first_byte(ch)].update != NULL ||
      state->sequence[Char_get_first_byte(ch)].next_state != NULL))
  {
    if (state->sequence[Char_get_first_byte(ch)].update != NULL)
        state->sequence[Char_get_first_byte(ch)].update(state->sequence[Char_get_first_byte(ch)].object,
                                          ch,state->params,state->n_params);
    if (state->sequence[Char_get_first_byte(ch)].next_state != NULL)
        next_state = state->sequence[Char_get_first_byte(ch)].next_state;
  }
  else if(Char_is_printable(ch) && state->when_printable.update)
  {
    state->when_printable.update(state->when_printable.object,ch,
                                 state->params,state->n_params);
    if(state->when_printable.next_state != NULL)
        next_state = state->when_printable.next_state;
  }
  
  return next_state;
}

static void State_get_in(State *state, Char *ch)
{
  state->n_params = 1;
  state->params[0] = DEFAULT_PARAM;
}

void State_init(State *state, int max_params)
{
  int i;
  state->get_in = State_get_in;
  state->when_printable.update = NULL;
  state->when_printable.next_state = state;

  for(i=0;i<N_TRANS;i++)
  {
    state->sequence[i].update = NULL;
    state->sequence[i].next_state = NULL;
  }

  state->max_params = max_params;
  if(0<max_params)
      state->params = (int *)calloc(sizeof(int),state->max_params);
  else
      state->params = NULL;
  state->n_params = 0;
#ifdef DEBUG
  memset(state->state_name,0,sizeof(state->state_name));
#endif  
}
/* State private method impl end*/

/* State Objects end*/
