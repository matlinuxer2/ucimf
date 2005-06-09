/* This software is subject to the terms of the Common Public License
   You must accept the terms of this license to use this software.

   Copyright (C) 2002, International Business Machines Corporation
   and others.  All Rights Reserved.

   Further information about Common Public License Version 0.5 is obtained
   from url http://oss.software.ibm.com/developer/opensource/license-cpl.html */

/*
 * Function:
 *	TemplateAction
 *
 * Parameters:
 *	w	   - template widget
 *	event	   - event that caused this action
 *	params	   - parameters
 *	num_params - number of parameters
 *
 * Description:
 *	This function does nothing.
 */
/*ARGSUSED*/


#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xmu/Atoms.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include "VTP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "actions.h"
#include "xim.h"

static char selected_string[BUFSIZ];

void HandleKeys(Widget w, XEvent *event, String *params, Cardinal *num_params)
{
  VTWidget vt = (VTWidget)w;
#define MBSIZE  4096
  char mb[MBSIZE]; /* enough? */
  KeySym keysym;
  Status status;
  int length = -1;
  int ret = 0;

  if(vt->vt.ic)
      length = XmbLookupString(vt->vt.ic,(XKeyPressedEvent *)event,
                               mb,MBSIZE,&keysym,&status);
  else
      length = XLookupString((XKeyPressedEvent *)event,
                             mb,MBSIZE,
                             &keysym,NULL);
#undef MBSIZE
  if(keysym == XK_Return)
      VTCore_send_key(vt->vt.vtcore,VTK_CR);
  else if((keysym >= XK_Left) && (keysym <= XK_Down))
      ret = VTCore_send_key(vt->vt.vtcore,keysym-XK_Up+VTK_UP);
  else if((keysym >= XK_KP_Left) && (keysym <= XK_KP_Down))
      ret = VTCore_send_key(vt->vt.vtcore,keysym-XK_KP_Up+VTK_UP);
  else if(keysym == XK_Prior || keysym ==  XK_KP_Prior)
      ret = VTCore_send_key(vt->vt.vtcore,VTK_PAGE_UP);
  else if(keysym == XK_Next || keysym == XK_KP_Next)
      ret = VTCore_send_key(vt->vt.vtcore,VTK_PAGE_DOWN);
  else if(keysym == XK_Home || keysym == XK_KP_Home)
      ret = VTCore_send_key(vt->vt.vtcore,VTK_HOME);
  else if(keysym == XK_End || keysym == XK_KP_End)
      ret = VTCore_send_key(vt->vt.vtcore,VTK_END);
  else if(keysym == XK_Insert || keysym == XK_KP_Insert)
      ret = VTCore_send_key(vt->vt.vtcore,VTK_INSERT);
  else if(keysym == XK_Delete || keysym == XK_KP_Delete)
      ret = VTCore_send_key(vt->vt.vtcore,VTK_DELETE);
  else if(IsFunctionKey(keysym))
      ret = VTCore_send_key(vt->vt.vtcore,keysym-XK_F1+VTK_F1);
  else if(IsKeypadKey(keysym))
  {
    if(XK_KP_0 <= keysym && keysym <= XK_KP_9)
        ret = VTCore_send_key(vt->vt.vtcore,keysym-XK_KP_0+VTK_KP_0);
    else
    {
      switch(keysym)
      {
        case XK_KP_Separator:
            ret = VTCore_send_key(vt->vt.vtcore,VTK_KP_COMMA);
            break;
        case XK_KP_Enter:
            ret = VTCore_send_key(vt->vt.vtcore,VTK_KP_ENTER);
            break;
        case XK_KP_Decimal:
            ret = VTCore_send_key(vt->vt.vtcore,VTK_KP_PERIOD);
            break;
        case XK_KP_Subtract:
            ret = VTCore_send_key(vt->vt.vtcore,VTK_KP_DASH);
            break;
        default:
                /* send straight forward  */
            ret = VTCore_write(vt->vt.vtcore,mb,length);
            break;
      }
    }
  }
  else if(length > 0)
      ret = VTCore_write(vt->vt.vtcore,mb,length);

  if(ret < 0)
      exit(1);
}

void HandleString(Widget w, XEvent *e, String *params, Cardinal *n_params)
{
  int ret = 0;
  VTWidget vt = (VTWidget)w;
  if(*n_params > 0)
      ret = VTCore_write(vt->vt.vtcore,*params,strlen(*params));

  if(ret < 0)
      exit(1);
}

static int send_mouse_position(VTWidget vt, XButtonEvent *event)
{
  int height =  vt->core.height-VTGetStatusLineHeight((Widget)vt)-
 vt->core.border_width*2 -1; 
  int col;
  int row;
  int button = 0;
  int modifiers = 0;
  int pressed = 0;

  if(event->type == ButtonPress)
    pressed = 1;
  else if(event->type == ButtonRelease)
    pressed = 0;
  else
    return 0;

  if(height < event->y)
      return 0;
  col = event->x/vt->vt.cell_width_pixel;
  row = event->y/vt->vt.cell_height_pixel;

  if(event->state & ShiftMask)
    modifiers = VTM_SHIFT;
  if(event->state & ControlMask)
    modifiers |= VTM_CONTROL;
  if(event->state & Mod1Mask)
    modifiers |= VTM_META;

  button =  event->button - 1;

  return VTCore_send_mouse_position(vt->vt.vtcore,button,
                                    modifiers,pressed,col,row);
}

void HandleButtonDown(Widget w, XEvent *event,
                      String *params, Cardinal *num_params)
{
  send_mouse_position((VTWidget)w,(XButtonEvent *)event);
}

void HandleButtonUp(Widget w, XEvent *event,
                      String *params, Cardinal *num_params)
{
  send_mouse_position((VTWidget)w,(XButtonEvent *)event);
}


void show_selection(Widget w,XtPointer client_data, Atom *selection,
                    Atom *type, XtPointer value,unsigned long *length,
                    int *format)
{
  VTWidget vt = (VTWidget)w;
  if( *type == 0 || *length == 0 || value == NULL) /* No Compound Text */
  {
    int inbytes;
    char *string;
        /* Fetch from cutbuffer0 */
    string = XFetchBuffer(XtDisplay(w),&inbytes,0);
    if(inbytes > 0)
    {
      string[inbytes] = '\0';
      VTCore_write(vt->vt.vtcore,string,strlen(string));
    }
  }
  else
  {
    XTextProperty text_prop;
    char **string_list;
    int num_of_list;
    char *lag, *cp, *end;
    text_prop.value = (unsigned char *)value;
    text_prop.encoding = *type;
    text_prop.format = *format;
    text_prop.nitems = *length;

    XmbTextPropertyToTextList(XtDisplay(w),
                              &text_prop,&string_list,&num_of_list);

    while( num_of_list-- > 0 )
    {
      end = string_list[num_of_list] + strlen(string_list[num_of_list]);
      lag = string_list[num_of_list];
      for (cp = string_list[num_of_list]; cp != end; cp++)
      {
        if (*cp != '\n') continue;
        *cp = '\r';
        VTCore_write(vt->vt.vtcore,lag,cp-lag+1);
      lag = cp + 1;
      }
      if (lag != end)
          VTCore_write(vt->vt.vtcore,lag,cp-lag);
    }
    XFreeStringList(string_list);
  }
}

void HandleInsertSelection(Widget w, XEvent *event,
                           String *params, Cardinal *num_params)
{
  if(send_mouse_position((VTWidget)w,(XButtonEvent *)event))
      return;
  
  XtGetSelectionValue(w,XA_PRIMARY,XA_COMPOUND_TEXT(XtDisplay(w)),
                      show_selection,
                      NULL,event->xbutton.time);
}

void HandleStartSelection(Widget w, XEvent *event,
                          String *params, Cardinal *num_params)
{
  VTWidget vt = (VTWidget)w;
  if(send_mouse_position((VTWidget)w,(XButtonEvent *)event))
    return;

  VTCore_clear_selection(vt->vt.vtcore);
  if(event->type == ButtonPress)
  {
    int height =  vt->core.height-VTGetStatusLineHeight(w)-
 vt->core.border_width*2 -1; 
    int y = height < event->xbutton.y ? height : event->xbutton.y;
    static Time pre_t = 0;
    static int type = 0;
    int col;
    int row;
    
    col = event->xbutton.x/vt->vt.cell_width_pixel;
    row = y/vt->vt.cell_height_pixel;
    if(((XButtonEvent *)event)->time - pre_t < 500)
    {
      type++;
    }
    else
        type = 0;
    VTCore_start_selection(vt->vt.vtcore,col,row,type);
    pre_t = ((XButtonEvent *)event)->time;
  }
}

void HandleExtendSelection(Widget w, XEvent *event,
                          String *params, Cardinal *num_params)
{
  VTWidget vt = (VTWidget)w;
  fflush(stdout);
  if(send_mouse_position((VTWidget)w,(XButtonEvent *)event))
    return ; 
  if((event->type == ButtonPress || event->type == MotionNotify))
  {

    int col,row;
    int x,y;
    int width = vt->core.width - vt->core.border_width*2 - 1 ;
    int height =  vt->core.height-VTGetStatusLineHeight(w)-
 vt->core.border_width*2 -1;

    x = event->xbutton.x < 0 ? 0 : event->xbutton.x;
    y = event->xbutton.y < 0 ? 0 : event->xbutton.y;
    x = width < x ? width + vt->vt.cell_width_pixel/2 : x;
    y = height < y ? height + vt->vt.cell_height_pixel : y; 

    col = x/vt->vt.cell_width_pixel;
    row = y/vt->vt.cell_height_pixel;
    VTCore_extend_selection(vt->vt.vtcore, col,row);
  }
}

static Boolean convert(Widget w, Atom *selection, Atom *target,
                       Atom *type_return, XtPointer *value_return,
                       unsigned long *length_return, int *format_return)
{
  if(*target == XA_COMPOUND_TEXT(XtDisplay(w)))
  {
    XTextProperty prop;

    *value_return = selected_string;

    if(XmbTextListToTextProperty(XtDisplay(w),(char **)value_return,1,
                                 XCompoundTextStyle,&prop) != Success)
        return False;

    *value_return = (XtPointer)prop.value;
    *length_return = prop.nitems;
    *type_return = *target;
    *format_return = 8;
    return True;
  }
  return False;
}

static void lose(Widget w, Atom *selection)
{
  VTWidget vt = (VTWidget)w; 
  VTCore_clear_selection(vt->vt.vtcore);
}

void HandleEndSelection(Widget w, XEvent *event,
                       String *params, Cardinal *num_params)
{
  VTWidget vt = (VTWidget)w;
  if(send_mouse_position((VTWidget)w,(XButtonEvent *)event))
     return ;
     
  if(event->type == ButtonRelease )
  {
    VTCore_copy_selected_buffer(vt->vt.vtcore,selected_string,BUFSIZ);
    XtOwnSelection(w,XA_PRIMARY,event->xmotion.time,convert,lose,NULL);
        /* Store to CutBuffer0 */
    XStoreBuffer(XtDisplay(w),selected_string,strlen(selected_string),0);
  }
}

static int params_to_line(VTWidget vt,String *params, Cardinal *num_params)
{
  int line;
  
  if(*num_params != 2)
      return 0;

  if( strncmp("line",params[1],4) == 0)
      line = atoi(*params);
  else if( strncmp("page",params[1],4) == 0)
      line = atoi(*params) * vt->vt.num_rows;
  else if( strncmp("halfpage",params[1],8) == 0)
      line = atoi(*params) * vt->vt.num_rows/2;
  else
      line = 0;

  return line;
}

void HandleScrollUp(Widget w, XEvent *event,
                    String *params, Cardinal *num_params)
{
  VTWidget vt = (VTWidget)w;
  if(*num_params > 0)
      VTCore_scroll_up(vt->vt.vtcore,params_to_line(vt,params,num_params));
}

void HandleScrollDown(Widget w, XEvent *event,
                    String *params, Cardinal *num_params)
{
  VTWidget vt = (VTWidget)w;
  if(*num_params > 0)
      VTCore_scroll_down(vt->vt.vtcore,params_to_line(vt,params,num_params));
}

#ifdef BIDI
#include "iterm/vtlayout.h"
void HandleSwitchDirection(Widget w, XEvent *event,
                    String *params, Cardinal *num_params)
{
  VTWidget vt = (VTWidget)w;
  if(VTCore_is_direction_LTR(vt->vt.vtcore))
      VTCore_set_direction(vt->vt.vtcore,VT_RTL);
  else
      VTCore_set_direction(vt->vt.vtcore,VT_LTR);

}
#endif
