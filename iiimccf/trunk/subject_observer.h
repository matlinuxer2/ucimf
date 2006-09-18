#include <vector>
using std::vector;


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

class TrackPoint : public Subject{
  public:
    TrackPoint();
    ~TrackPoint();
    void get_position( int& x, int& y);
    void set_position( int x, int y);
  private:
    int x,y;
};

class SwitchStatus : public Subject{
  public:
    SwitchStatus();
    ~SwitchStatus();
    bool get_screenstatus();
    void set_screenstatus( bool );
  private:
    bool switch_status;
    bool visible;
};
