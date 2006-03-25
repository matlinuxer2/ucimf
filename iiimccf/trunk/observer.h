#include <vector>
#include "graphdev.h"
using namespace std;

class Text;
class Rectangle;
class BitMap;

class Observer{
  public:
    Observer();
    ~Observer();
    virtual void update()=NULL;
};

class Subject{
  public:
    Subject();
    ~Subject();
   
    void attach( Observer* obsr );
    void detach( Observer* obsr );
    void notify();
  private:
    vector<Observer*> observers;
};


class TrackPoint : public Subject{
  public:
    TrackPoint();
    ~TrackPoint();
    void get_position( int& x, int& y);
    void set_position( int x, int y);
  private:
    int x,y;
};

class Stts : public Observer{
  public:
    Stts();
    ~Stts();
    
    void update();
    void show();
    void hide();
  private:
    Rectangle* div;
    Text* title;
    BitMap tmp;
};
