#include <ucimf.h>
#include "imf.h"


int ucimf_focus_on()
{
  Imf* imf = new Imf;
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

char* ucimf_process_stdin( char* buf )
{
  return imf->process_input( buf  );
}


void ucimf_cursor_position( int x, int y)
{
  imf->update_cursor_position(x, y);
}
