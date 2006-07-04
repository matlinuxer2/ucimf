class Imf{
  public:
    virtual char* process_input( char* buf )=0;
    virtual bool switch_lang( char* lang )=0;
    virtual bool switch_im()=0;
    virtual bool switch_im_per_lang()=0;
    virtual void update_cursor_position(int x, int y )=0;
}
