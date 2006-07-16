#include <vector>
using std::vector;

typedef unsigned long Word;

struct CharBitMap;

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


class Window
{
  public:
    bool position(int x, int y);
    void shift(int s_x, int s_y);
    bool isVisible();
    void setShow();
    void setHide();
    void show();
    void hide();
    
    int x();
    int y();
    int w();
    int h();
    int c();
    void w( int new_width );
    void h( int new_height );
    void c( int new_color );
   
  protected:
    void push();
    void pop();

  private:
    int font_width, font_height, font_color;
    bool visible;
    int cur_x, cur_y;
    int shift_x, shift_y;
    CharBitMap window_fg, screen_bg;
}


/*
 * Character Object Layer
 */

class String 
{
  public:
    String(){};
    ~String(){};

    int size();
    void clear();
    Word operator[]( const int& i);
    String&  operator+( const String& a, const String& b);
    void append_utf8_char( const char& );
    void append_utf8_string( const char*  );
    void append_utf16_char( const Word& );
    void append_utf16_string( String );
    void info();
  private:
    vector<Word> CharCodes;
  
};

