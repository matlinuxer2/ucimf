#include "imf.h"
#include <iostream>
using std::cout;

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
  char* result = "\0";
  return result;
}
