#include <ucimf.h>
#include "imf.h"


int ucimf_focus_on()
{
}

int ucimf_focus_off()
{
}

int ucimf_change_im()
{
}

int ucimf_switch_imf( IMF_NAME )
{
}

char* ucimf_process_key( int keycode, int keychar, int modifier )
{
  return imf->process_keyevent( keycode, keychar, modifier );
}


void ucimf_cursor_position( int x, int y)
{
  imf->update_cursor_position(x, y);
}
