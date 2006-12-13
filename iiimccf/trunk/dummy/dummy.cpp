#include "dummy.h"

char* DummyImf::name()
{
  return "Dummy";
}

string DummyImf::process_input( const string& input)
{
  string result = input;
  return result;
}

