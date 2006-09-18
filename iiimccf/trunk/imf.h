class Imf{
  public:
    virtual char* process_input( char* buf )=0;
    virtual bool switch_lang( int )=0;
    virtual bool switch_im( int )=0;
    virtual bool switch_im_per_lang( int )=0;
};
