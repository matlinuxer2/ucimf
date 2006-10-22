#include <vector>
using std::vector;

/*
 * Basic Observer-Subject Model
 */

class Observer{
  public:
    virtual void update()=NULL;
};

class Subject{
  public:
    void attach( Observer* obsr );
    void detach( Observer* obsr );
    void notify();
  private:
    vector<Observer*> observers;
};

/*
 * Derived Object 
 */

class TrackPoint : public Subject{
  public:
    TrackPoint();
    void get_position( int& x, int& y);
    void set_position( int x, int y);
  private:
    int x,y;
};


class SwitchStatus : public Subject{
  public:
    SwitchStatus();
    bool get_screenstatus();
    void set_screenstatus( bool );
  private:
    bool switch_status;
    bool visible;
};

class CursorPosition : public Subject{
  public:
    static CursorPosition* getInstance();
    void get_position( int& x, int& y);
    void set_position( int x, int y);
    
  protected:
    CursorPosition();

  private:
    static CursorPosition *_instance;
    int x,y;
};

class ConsoleFocus : public Subject{
  public:
    static ConsoleFocus* getInstance();
    bool get_focus();
    void set_focus( bool );
  protected:
    ConsoleFocus();

  private:
    static ConsoleFocus *_instance;
    bool focus;
};

/*
class CurrentImfStatus : public Subject{
  public:
    static CurrentImfStatus* getInstance();

    char* get_imf_name();
    char* get_im_name();
    char* get_lang_name();
    void set_imf_name( char* );
    void set_im_name( char* );
    void set_lang_name( char* );
    
  protected:
    CurrentImfStatus();

  private:
    static CurrentImfStatus *_instance;
  
};

*/
