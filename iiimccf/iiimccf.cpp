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

int iiimccf_scrn( int height, int width, int resolution )
{
  return 0;
}

int iiimccf_pos( int x, int y)
{
  return 0;
}

void iiimccf_refresh()
{

}
