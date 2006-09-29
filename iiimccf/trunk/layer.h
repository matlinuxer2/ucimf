#include <vector>

typedef unsigned long Word;
struct CharBitMap;

class Window
{
  public:
    bool isVisible();
    void setShow();
    void setHide();
    void show();
    void hide();
   
    virtual void draw();
    virtual void draw_decration();

    int x();
    int y();
    int w();
    int h();
    bool position(int x, int y);
    void shift(int s_x, int s_y);
    void w( int new_width );
    void h( int new_height );
   
  protected:
    void push();
    void pop();

  private:
    bool visible;
    int cur_x, cur_y;
    int shift_x, shift_y;
    CharBitMap *window_fg, *screen_bg;
};


/*
 * Character Object Layer
 */

class utf_string 
{
  public:
    utf_string(){};
    ~utf_string(){};

    Word operator[]( const int& i);
    int size();
    void clear();
    void info();
    
    utf_string& operator=( const utf_string& a);
    utf_string& operator+( const utf_string& a);
    void utf16( const char* );
    void utf8( const char* );
    char* utf16();
    char* utf8();
  private:
    std::vector<Word> CharCodes;
  
};

