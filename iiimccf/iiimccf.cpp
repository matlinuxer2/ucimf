#include "iiimccf-int.h"

IIIMCCF* iiimccf=NULL;

int iiimccf_init()
{
  iiimccf = new IIIMCCF;
}

int iiimccf_exit()
{
  delete iiimccf;
  iiimccf = NULL;
}

int iiimccf_on()
{
  iiimccf->on();
}

int iiimccf_off()
{
  iiimccf->off();
}

void iiimccf_refresh()
{

}
