#ifndef __Imf
#define __Imf

#include <string>
using namespace std;

class Imf{
  public:
    virtual string process_input( const string& input )=0;
    virtual void  switch_lang()=0;
    virtual void  switch_im()=0;
    virtual void  switch_im_per_lang()=0;
  protected:
    static Imf *current_imf;
    static void ChangeImf( Imf* );
};

class DummyImf : public Imf
{
  public: 
    DummyImf();
    ~DummyImf();
    string process_input( const string& input );
    void switch_lang(){};
    void switch_im(){};
    void switch_im_per_lang(){};
};

#endif
