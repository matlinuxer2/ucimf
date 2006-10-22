#include "window.h"
#include <vector>
#include <map>
using namespace std;

class Shift;

class Cwm 
{
  public:
    static Cwm* getInstance();

    void set_position( int new_x, int new_y );
    void set_focus( bool new_focus );
    bool get_focus(){ return _focus; };
    void windowChanged( Window* changed_win );
    void attachWindow( Window* new_win, Shift* new_shift );

    int xres(){ return _xres; };
    int yres(){ return _yres; };
    int cursor_x(){ return _cursor_x; };
    int cursor_y(){ return _cursor_y; };

  protected:
    Cwm();
    
  private:
    static Cwm* _instance;

    int _xres, _yres;
    int _cursor_x, _cursor_y;
    bool _focus;
    
    vector<Window*> wins;
    map<Window*,Shift*> cwmmap;
};


class Shift
{
  public:
    virtual void update( Window* )=0;

  protected:
    Cwm* cwm;
};

class StatusShift : Shift
{
  public:
    void update( Window* );

};

/*
class PreeditShift : Shift
{
  public:
    void update( Window* );

}

class LookupChoiceShift : Shift
{
  public:
    void update( Windows* );

}
*/
