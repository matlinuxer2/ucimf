#include "iiimccf-int.h"

IIIMCCF* iiimccf=NULL;

int iiimccf_init()
{
  iiimccf = new IIIMCCF;
  return 0;
}

int iiimccf_exit()
{
  delete iiimccf;
  iiimccf = NULL;
  return 0;
}

int iiimccf_on()
{
  iiimccf->on();
  iiimccf->ims_set();
  return 0;
}

int iiimccf_off()
{
  iiimccf->off();
  return 0;
}

int iiimccf_proc( int keycode, int keychar, int modifier )
{
  iiimccf->proc( keycode, keychar, modifier );
  return 0;
}

int iiimccf_result( char* *buf_out )
{ 
  int buf_len;
  buf_len= iiimccf->result( buf_out ); 
  return buf_len;
}

void iiimccf_show_ims()
{
  iiimccf->ims_show();
}

void iiimccf_scrn( int height, int width, int resolution )
{
  iiimccf->scrn( height, width, resolution );
}

void iiimccf_pos( int x, int y)
{
  iiimccf->pos( x, y );
}

void iiimccf_refresh()
{
  iiimccf->refresh();
}
