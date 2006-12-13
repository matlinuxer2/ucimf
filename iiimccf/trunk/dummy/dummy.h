#include "imf.h"

class DummyImf : public Imf
{
  public:
    char* name();
    string process_input( const string& input );
    void  switch_lang(){};
    void  switch_im(){};
    void  switch_im_per_lang(){};
};

extern "C" Imf* createImf(){ return new DummyImf; }
extern "C" void destroyImf( Imf* imf ){ delete imf; }
