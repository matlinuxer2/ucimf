#include "imf.h"

Imf* Imf::current_imf = 0;

void Imf::ChangeImf( Imf* imf )
{
}
  

DummyImf::DummyImf()
{
  current_imf = this;
}

DummyImf::~DummyImf()
{
  current_imf = 0;
}

char* DummyImf::process_input( char* buf )
{
  //char *result="\033\13320;20H [DummyImf]"; 
  char *result="[DummyImf]"; 
  return result;
}
