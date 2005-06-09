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
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "vt100.h"
#include "state.h"


#ifdef DEBUG
static char *state_names[] = {
    "GROUND_STATE",
    "ESC_STATE",
    "CSI_STATE",
    "IGN_STATE",
    "IES_STATE",
    "SCS_STATE",
    "SCR_STATE",
    "EIG_STATE",
    "DEC_STATE",
    "OSC_STATE",
#ifdef BIDI
    "BIDI_STATE",
#endif
    "NUM_STATE"
};

void default_func(void *obj,Char *ch, int *params,int n_params)
{
  int i;
  State *state = (State *)obj;
  fprintf(stderr,"WARNING: Not Implemented: %s -> %c(-> %s)\n",
          state->state_name,Char_get_first_byte(ch),
          state->sequence[Char_get_first_byte(ch)].next_state->state_name);
  for(i=0;i<n_params;i++)
  {
    if(params[i] > 0)
        fprintf(stderr," params[%d] = %d, ",i,params[i]);
  }
  fprintf(stderr,"\n");
}
#endif

#ifdef DEBUG
#define TO_GROUND_STATE(vt,st,ch)\
do { \
    State_register_char(&vt->all_states[st],\
                        &vt->all_states[GROUND_STATE],\
                        ch,\
                        default_func,\
                        &vt->all_states[st]);\
} while(0)
#else
#define TO_GROUND_STATE(vt,st,ch)\
do { \
    State_register_char(&vt->all_states[st],\
                        &vt->all_states[GROUND_STATE],\
                        ch,\
                        NULL,\
                        NULL);\
} while(0)
#endif

static void GroundState_init(VT100 *);
static void ESCState_init(VT100 *);
static void CSIState_init(VT100 *);
static void IGNState_init(VT100 *);
static void IESState_init(VT100 *);
static void SCSState_init(VT100 *);
static void SCRState_init(VT100 *);
static void EIGState_init(VT100 *);
static void DECState_init(VT100 *);
static void OSCState_init(VT100 *);
#ifdef BIDI
static void BIDIState_init(VT100 *);
#endif


/* VT100 method impl */

/** parse multibyte character
 * @param vt100 reference to VT100 object
 * @param ch reference to Char object to be parsed
 */
void VT100_parse(VT100 *vt100,Char *ch)
{
  State *prev_state;
  prev_state = vt100->current_state;
  vt100->current_state = State_parse(vt100->current_state,ch);
  if(prev_state != vt100->current_state && vt100->current_state->get_in)
      vt100->current_state->get_in(vt100->current_state,ch);
}
/* VT100 private method impl end */


/**
 * VT100 Object Constructor
 */
VT100 *VT100_new()
{
  VT100 *vt100;
  if((vt100 = (VT100 *)malloc(sizeof(VT100))) == NULL)
  {
    perror("VT100_new");
    return NULL;
  }
  GroundState_init(vt100);
  ESCState_init(vt100);
  CSIState_init(vt100);
  IGNState_init(vt100);
  IESState_init(vt100);
  SCSState_init(vt100);
  SCRState_init(vt100);
  EIGState_init(vt100);
  DECState_init(vt100);
  OSCState_init(vt100);
#ifdef BIDI
  BIDIState_init(vt100);
#endif

  vt100->current_state = &vt100->all_states[GROUND_STATE];
  return vt100;
}

/**
 * register escape sequence to VT100 object
 * @param vt100 reference to VT100 object
 * @param sequene NULL terminated character sequence
 * @param next_state_t next state vt100 transits after processing this sequence
 * @param func callback function when vt100 takes this sequence
 * @param object passed to callback function as a first parameter
 */
void VT100_register_sequence(VT100 *vt100,unsigned char *sequence,
                             state_t next_state_t, callback_func func,
                             void *object)
{
  State *state = &vt100->all_states[GROUND_STATE];
  State *next_state;
  int i;
  int len = strlen((char *)sequence)-1;

  for(i=0;i<len;i++)
  {
    if(state->sequence[sequence[i]].next_state != NULL)
        state = state->sequence[sequence[i]].next_state;
  }
  if(state == NULL)
      return;

  if( next_state_t == NO_MOVE)
      next_state = NULL;
  else
      next_state = &vt100->all_states[next_state_t];
  
  State_register_char(state,next_state,sequence[i],func,object);
}

/**
 * VT100 Object Destructor
 */
void VT100_destroy(VT100 *vt100)
{
  int i;
  for(i=0;i<NUM_STATE;i++)
  {
    if(vt100->all_states[i].params)
      free(vt100->all_states[i].params);
  }
  free(vt100);
}
/* VT100 method impl */

/**************************************************/
void GroundState_init(VT100 *vt100)
{
  State_init(&vt100->all_states[GROUND_STATE],1);
  State_register_char(&vt100->all_states[GROUND_STATE],
                      &vt100->all_states[ESC_STATE],ESC,NULL,NULL);
#ifdef DEBUG
  strcpy(vt100->all_states[GROUND_STATE].state_name,"GROUND STATE");
#endif  
}

/**************************************************/
/* CSI State utility */
#ifdef BIDI
/* transit BIDI State */
static void CSIState_to_bidi_state(void *object, Char *ch,
                                   int *params, int n_params)
{
  State *csi_state = (State *)object;
  State *bidi_state = csi_state->sequence[' '].next_state;
  int i;
  int length = 2 < n_params ? 2 : n_params;
   bidi_state->params[1] = 0;
  for(i=0;i<length;i++)
  {
    bidi_state->params[i] = csi_state->params[i];
  }
  bidi_state->n_params = length;
}
#endif /* BIDI */

void CSIState_init(VT100 *vt100)
{
  int i;
  State_init(&vt100->all_states[CSI_STATE],10);
#ifdef DEBUG
  strcpy(vt100->all_states[CSI_STATE].state_name,"CSI STATE");
#endif  
  State_register_char(&vt100->all_states[CSI_STATE],
                      &vt100->all_states[ESC_STATE],ESC,NULL,NULL);

  State_register_char(&vt100->all_states[CSI_STATE],
                      &vt100->all_states[DEC_STATE],'?',NULL,NULL);

      /* store digits in params */
  for(i='0';i<='9';i++)
  {
    State_register_char(&vt100->all_states[CSI_STATE],
                        &vt100->all_states[CSI_STATE],i,State_store_digit,
                        &vt100->all_states[CSI_STATE]);
  }
      /* prepare next for param */
  State_register_char(&vt100->all_states[CSI_STATE],
                      &vt100->all_states[CSI_STATE],';',State_next_param,
                      &vt100->all_states[CSI_STATE]);
#ifdef BIDI  
  State_register_char(&vt100->all_states[CSI_STATE],
                      &vt100->all_states[BIDI_STATE],' ',
                      CSIState_to_bidi_state,
                      &vt100->all_states[CSI_STATE]);
  for(i='!';i<='/';i++)
#else  
  for(i=' ';i<='/';i++)
#endif      
  {
    State_register_char(&vt100->all_states[CSI_STATE],
                        &vt100->all_states[EIG_STATE],i,NULL,NULL);
  }

      /* to ground_state */
  for(i='E';i<='F';i++)
  {
    TO_GROUND_STATE(vt100,CSI_STATE,i);
  }
  TO_GROUND_STATE(vt100,CSI_STATE,'I');
  for(i='Q';i<='S';i++)
  {
    TO_GROUND_STATE(vt100,CSI_STATE,i);
  }
  for(i='U';i<='b';i++)
  {
    TO_GROUND_STATE(vt100,CSI_STATE,i);
  }
  TO_GROUND_STATE(vt100,CSI_STATE,'d');
  TO_GROUND_STATE(vt100,CSI_STATE,'e');
  for(i='i';i<='k';i++)
  {
    TO_GROUND_STATE(vt100,CSI_STATE,i);
  }
  for(i='o';i<='q';i++)
  {
    TO_GROUND_STATE(vt100,CSI_STATE,i);
  }
  for(i='s';i<='w';i++)
  {
    TO_GROUND_STATE(vt100,CSI_STATE,i);
  }
  for(i='y';i<=DEL;i++)
  {
    TO_GROUND_STATE(vt100,CSI_STATE,i);
  }
      
}

/**************************************************/
void ESCState_init(VT100 *vt100)
{
  int i;
  State_init(&vt100->all_states[ESC_STATE],1);
#ifdef DEBUG
  strcpy(vt100->all_states[ESC_STATE].state_name,"ESC STATE");
#endif  
  State_register_char(&vt100->all_states[ESC_STATE],
                      &vt100->all_states[CSI_STATE],'[',NULL,NULL);

  State_register_char(&vt100->all_states[ESC_STATE],
                      &vt100->all_states[OSC_STATE],']',NULL,NULL);

  State_register_char(&vt100->all_states[ESC_STATE],
                      &vt100->all_states[IGN_STATE],'P',NULL,NULL);
  State_register_char(&vt100->all_states[ESC_STATE],
                      &vt100->all_states[IGN_STATE],'^',NULL,NULL);
  State_register_char(&vt100->all_states[ESC_STATE],
                      &vt100->all_states[IGN_STATE],'_',NULL,NULL);

  State_register_char(&vt100->all_states[ESC_STATE],
                      &vt100->all_states[SCS_STATE],'(',NULL,NULL);
  State_register_char(&vt100->all_states[ESC_STATE],
                      &vt100->all_states[SCS_STATE],')',NULL,NULL);
  State_register_char(&vt100->all_states[ESC_STATE],
                      &vt100->all_states[SCS_STATE],'*',NULL,NULL);
  State_register_char(&vt100->all_states[ESC_STATE],
                      &vt100->all_states[SCS_STATE],'+',NULL,NULL);

  State_register_char(&vt100->all_states[ESC_STATE],
                      &vt100->all_states[SCR_STATE],'#',NULL,NULL);

      /* to ground state */
  for(i='0';i<='6';i++)
  {
    TO_GROUND_STATE(vt100,ESC_STATE,i);
  }

  for(i='9';i<='<';i++) /* 9 : ; < */
  {
    TO_GROUND_STATE(vt100,ESC_STATE,i);
  }

  for(i='?';i<='C';i++) /* ? @ A B C */
  {
    TO_GROUND_STATE(vt100,ESC_STATE,i);
  }

  TO_GROUND_STATE(vt100,ESC_STATE,'G');

  for(i='I';i<='L';i++) /* I J K L */
  {
    TO_GROUND_STATE(vt100,ESC_STATE,i);
  }

  for(i='Q';i<='S';i++) /* Q S R */
  {
    TO_GROUND_STATE(vt100,ESC_STATE,i);
  }

  for(i='U';i<='Y';i++) /* U V W X Y */
  {
    TO_GROUND_STATE(vt100,ESC_STATE,i);
  }

  TO_GROUND_STATE(vt100,ESC_STATE,'\\');

  for(i='`';i<='c';i++) /* ` a b c */
  {
    TO_GROUND_STATE(vt100,ESC_STATE,i);
  }
  
  for(i='d';i<='k';i++) /* d e f g h i j k */
  {
    TO_GROUND_STATE(vt100,ESC_STATE,i);
  }

  for(i='p';i<='{';i++) /* p q r s t u v w x y z { */
  {
    TO_GROUND_STATE(vt100,ESC_STATE,i);
  }

  TO_GROUND_STATE(vt100,ESC_STATE,DEL);

  for(i=160;i<256;i++)
  {
    TO_GROUND_STATE(vt100,ESC_STATE,i);
  }
}

/**************************************************/
void IGNState_init(VT100 *vt100)
{
  int i;
  State_init(&vt100->all_states[IGN_STATE],1);
#ifdef DEBUG
  strcpy(vt100->all_states[IGN_STATE].state_name,"IGN STATE");
#endif  

  State_register_char(&vt100->all_states[IGN_STATE],
                      &vt100->all_states[IES_STATE],ESC,NULL,NULL);

  
  for(i=160;i<256;i++)
  {
    State_register_char(&vt100->all_states[IGN_STATE],
                        &vt100->all_states[GROUND_STATE],i,NULL,NULL);
  }
  State_register_char(&vt100->all_states[IGN_STATE],
                      &vt100->all_states[GROUND_STATE],0x18,NULL,NULL);
  State_register_char(&vt100->all_states[IGN_STATE],
                      &vt100->all_states[GROUND_STATE],0x1A,NULL,NULL);
}

/**************************************************/
void IESState_init(VT100 *vt100)
{
  int i;
  State_init(&vt100->all_states[IES_STATE],1);

#ifdef DEBUG
  strcpy(vt100->all_states[IES_STATE].state_name,"IES STATE");
#endif  
  for(i=0;i<128;i++)
  {
    State_register_char(&vt100->all_states[IES_STATE],
                        &vt100->all_states[IGN_STATE],i,NULL,NULL);
  }

  for(i=160;i<256;i++)
  {
    State_register_char(&vt100->all_states[IES_STATE],
                        &vt100->all_states[GROUND_STATE],i,NULL,NULL);
  }
}

/**************************************************/
/* SCS State utility */
void SCSState_get_in(State *state, Char *ch)
{
  state->params[0] = Char_get_first_byte(ch);
}

void SCSState_init(VT100 *vt100)
{
  int i;
  State_init(&vt100->all_states[SCS_STATE],1);
  vt100->all_states[SCS_STATE].get_in = SCSState_get_in;
#ifdef DEBUG
  strcpy(vt100->all_states[SCS_STATE].state_name,"SCS STATE");
#endif  

  State_register_char(&vt100->all_states[SCS_STATE],
                      &vt100->all_states[ESC_STATE],ESC,NULL,NULL);
  
  for(i=' ';i<='/';i++)
  {
    State_register_char(&vt100->all_states[SCS_STATE],
                        &vt100->all_states[EIG_STATE],i,NULL,NULL);
  }

      /* to ground_state */
  for(i='3';i<='@';i++)
  {
    TO_GROUND_STATE(vt100,SCS_STATE,i);
  }
  for(i='C';i<=DEL;i++)
  {
    TO_GROUND_STATE(vt100,SCS_STATE,i);
  }
  for(i=160;i<256;i++)
  {
    TO_GROUND_STATE(vt100,SCS_STATE,i);
  }
}

/**************************************************/
void SCRState_init(VT100 *vt100)
{
  int i;
  
  State_init(&vt100->all_states[SCR_STATE],1);

#ifdef DEBUG
  strcpy(vt100->all_states[SCR_STATE].state_name,"SCR STATE");
#endif  
  State_register_char(&vt100->all_states[SCR_STATE],
                      &vt100->all_states[ESC_STATE],ESC,NULL,NULL);

  for(i=' ';i<='/';i++)
  {
    State_register_char(&vt100->all_states[SCR_STATE],
                        &vt100->all_states[EIG_STATE],i,NULL,NULL);
  }

      /* to ground state */
  for(i='0';i<='7';i++)
  {
    TO_GROUND_STATE(vt100,SCR_STATE,i);
  }
  for(i='9';i<=DEL;i++)
  {
    TO_GROUND_STATE(vt100,SCR_STATE,i);
  }
  for(i=160;i<256;i++)
  {
    TO_GROUND_STATE(vt100,SCR_STATE,i);
  }
  
}

/**************************************************/
void EIGState_init(VT100 *vt100)
{
  int i;
  State_init(&vt100->all_states[EIG_STATE],1);

#ifdef DEBUG
  strcpy(vt100->all_states[EIG_STATE].state_name,"EIG STATE");
#endif  
  State_register_char(&vt100->all_states[EIG_STATE],
                      &vt100->all_states[ESC_STATE],ESC,NULL,NULL);

      /* to ground state */
  for(i='0';i<=DEL;i++)
  {
    TO_GROUND_STATE(vt100,EIG_STATE,i);
  }
  for(i=160;i<256;i++)
  {
    TO_GROUND_STATE(vt100,EIG_STATE,i);
  }
  
}

/**************************************************/
void DECState_init(VT100 *vt100)
{
  int i;
  State_init(&vt100->all_states[DEC_STATE],10);

#ifdef DEBUG
  strcpy(vt100->all_states[DEC_STATE].state_name,"DEC STATE");
#endif  
  State_register_char(&vt100->all_states[DEC_STATE],
                      &vt100->all_states[ESC_STATE],ESC,NULL,NULL);

      /* store digits in params */
  for(i='0';i<='9';i++)
  {
    State_register_char(&vt100->all_states[DEC_STATE],
                        &vt100->all_states[DEC_STATE],i,State_store_digit,
                        &vt100->all_states[DEC_STATE]);
  }
      /* prepare next for param */
  State_register_char(&vt100->all_states[DEC_STATE],
                      &vt100->all_states[DEC_STATE],';',State_next_param,
                      &vt100->all_states[DEC_STATE]);

  for(i=' ';i<='/';i++)
  {
    State_register_char(&vt100->all_states[DEC_STATE],
                        &vt100->all_states[EIG_STATE],i,NULL,NULL);
  }

      /* to ground_state */
  for(i='<';i<='g';i++)
  {
    TO_GROUND_STATE(vt100,DEC_STATE,i);
  }
  for(i='i';i<='k';i++)
  {
    TO_GROUND_STATE(vt100,DEC_STATE,i);
  }
  for(i='m';i<='q';i++)
  {
    TO_GROUND_STATE(vt100,DEC_STATE,i);
  }
  for(i='t';i<=DEL;i++)
  {
    TO_GROUND_STATE(vt100,DEC_STATE,i);
  }
  for(i=160;i<256;i++)
  {
    TO_GROUND_STATE(vt100,DEC_STATE,i);
  }
  
}

/**************************************************/
/* OSC State utilities */
static void OSCState_get_in_osc1(State *state, Char *ch)
{
  int i;
  state->n_params = 1;
  state->params[0] = DEFAULT_PARAM;

      /* set storing digit functin */
  for(i='0';i<='9';i++)
  {
    State_register_char(state,state,i,State_store_digit,state);
  }
#ifdef DEBUG  
  state->when_printable.object = state;
  state->when_printable.update = default_func;
#else
  state->when_printable.update = NULL;
#endif  
  state->when_printable.object = state;
  state->when_printable.next_state = state;
}

/* storing pribale characters */
static void OSCState_store_printable(void *object, Char *ch,
                                     int *params, int n_params)
{
  State *state = (State *)object;
  if(state->n_params + Char_get_length(ch) < state->max_params)
  {
    int i;
    for(i=0;i<Char_get_length(ch);i++)
    {
      memcpy(&((char *)state->params)[state->n_params-1],
             Char_get_string(ch),Char_get_length(ch));
    }
    state->n_params += Char_get_length(ch);
  }
}

/* transit osc2 state */
static void to_osc2(void *object, Char *ch, int *params, int n_params)
{
  int i;
  State *state = (State *)object;

  state->n_params++;
      /* unset storing digit function */
  for(i='0';i<='9';i++)
  {
    State_register_char(state,NULL,i,NULL,NULL);
  }
  state->when_printable.update = OSCState_store_printable;
  state->when_printable.next_state = state;
  state->when_printable.object = state;
}

void OSCState_init(VT100 *vt100)
{
  int i;
  State_init(&vt100->all_states[OSC_STATE],1024);

#ifdef DEBUG
  strcpy(vt100->all_states[OSC_STATE].state_name,"OSC STATE");
#endif
  vt100->all_states[OSC_STATE].get_in = OSCState_get_in_osc1;

      /* to ground_state */
  for(i=0;i<256;i++)
  {
    if(!isprint(i))
        State_register_char(&vt100->all_states[OSC_STATE],
                            &vt100->all_states[GROUND_STATE],i,NULL,NULL);
  }

      /* store digits in params */
  for(i='0';i<='9';i++)
  {
    State_register_char(&vt100->all_states[OSC_STATE],
                        &vt100->all_states[OSC_STATE],i,State_store_digit,
                        &vt100->all_states[OSC_STATE]);
  }
      /* prepare next for param */
  State_register_char(&vt100->all_states[OSC_STATE],
                      &vt100->all_states[OSC_STATE],';',to_osc2,
                      &vt100->all_states[OSC_STATE]);

}

#ifdef BIDI
/**************************************************/
void BIDIState_init(VT100 *vt100)
{
  int i;
  State_init(&vt100->all_states[BIDI_STATE],8);

#ifdef DEBUG
  strcpy(vt100->all_states[BIDI_STATE].state_name,"BIDI STATE");
#endif
  vt100->all_states[BIDI_STATE].get_in = NULL;

      /* to ign_state */
  for(i=0;i<256;i++)
  {
    if(!isprint(i))
        State_register_char(&vt100->all_states[BIDI_STATE],
                            &vt100->all_states[IGN_STATE],i,NULL,NULL);
  }
  State_register_char(&vt100->all_states[BIDI_STATE],
                      &vt100->all_states[IES_STATE],ESC,NULL,NULL);
  TO_GROUND_STATE(vt100,BIDI_STATE,ESC-3);
  TO_GROUND_STATE(vt100,BIDI_STATE,ESC-1);
}
#endif /* BIDI */
