#include "dummy.h"
#include "widget.h"

void DummyImf::refresh()
{
  Status* stts = Status::getInstance();
  stts->set_imf_name("Dummy");
  stts->set_im_name("none");
}

string DummyImf::process_input( const string& input)
{
  string result = input;
  return result;
}

