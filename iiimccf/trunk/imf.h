#ifndef __Imf
#define __Imf

class Imf{
  public:
    virtual char* process_input( char* buf )=0;
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
    char* process_input( char* buf );
    void switch_lang(){};
    void switch_im(){};
    void switch_im_per_lang(){};
};

#endif
