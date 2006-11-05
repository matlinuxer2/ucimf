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

string DummyImf::process_input( const string& input  )
{
  string result = input;
  return result;
}
