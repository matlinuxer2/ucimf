#include "openvanilla.h"

int main()
{
  OVImf* ovimf;
  ovimf = new OVImf;
  ovimf->process_keyevent('j','j',0);
  ovimf->lookupchoice->show();
  ovimf->process_keyevent('i','i',0);
  ovimf->lookupchoice->show();
  ovimf->process_keyevent('3','3',0);
  ovimf->lookupchoice->show();
  ovimf->preedit->send();
  return 1;
}
