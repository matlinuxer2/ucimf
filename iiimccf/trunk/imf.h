enum IMF {
  IIIMF,
  OpenVanilla,
  SCIM
}

class Imf{
  public:
    static Imf* getInstance();
    virtual char* process_keyevent(int keycode, int keychar, int modifier)=NULL;
    virtual bool switch_imf( IMF )=NULL;
    virtual bool switch_lang( char* )=NULL;
    virtual bool switch_im()=NULL;
    virtual bool switch_im_per_lang()=NULL;
    virtual bool switch_im_custom()=NULL;
    virtual void cursor_position(int x, int y )=NULL;
  protect:
    Imf();
  private:
    static Imf* _instance;
}
