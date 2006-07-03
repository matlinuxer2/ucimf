#include <vector>
using std::vector;

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


class Window
{
  public:
    bool position(int x, int y);
    bool isVisible();
    void shift();
    void setShow();
    void setHide();
    void show();
    void hide();

  private:
    bool visible;
    int cur_x, cur_y;
    int shift_x, shift_y;
    BitMap tmp;
}

class Stts : public Window, public Observer{
  public:
    Stts();
    ~Stts();
    void update();
  private:
    Rectangle* div;
    Text* title;
};

class Prdt : Window
{
  public:
    Prdt();
    ~Prdt(){};

    void info(); // for text-mode checking.
    bool update();
    bool draw();
    void empty();
    
  private:
    IIIMCF_context context;
    vector<IIIMP_card16> buf_utf16;
    Text* prdt_text;
    Rectangle* rect;
};


class Lkc : Window
{
  public:
    Lkc();
    ~Lkc(){};

    void info(); // for text-mode checking.
    bool update();
    bool draw();
    void empty();

  private:

    IIIMCF_context context;
    int cur_idx;
    Text* lkc_text;
    Rectangle* rect;
};
