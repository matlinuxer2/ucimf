#include <ucimf.h>
#include "imf.h"


int ucimf_init()
{
  Imf imf;
  imf->init();
}

int ucimf_exit()
{
  // deallocate memory 
  Imf imf;
  imf->exit();
}

int ucimf_focus_on()
{
  // prepare for process keyevent
}

int ucimf_focus_off()
{
  // stop process keyevent
}

char* ucimf_process_key( int keycode, int keychar, int modifier )
{

}


enum IMF{ IIIMF, OVIME, SCIM };

int ucimf_switch_imf( IMF );

void ucimf_change_im();

void ucimf_pos( int x, int y);
