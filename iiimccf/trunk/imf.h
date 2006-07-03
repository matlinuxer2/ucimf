class Imf{
  public:
    virtual char* process_keyevent(int keycode, int keychar, int modifier)=0;
    virtual bool switch_lang( char* )=0;
    virtual bool switch_im()=0;
    virtual bool switch_im_per_lang()=0;
    virtual void update_cursor_position(int x, int y )=0;
}
