#include <vector>
#include "layer.h"
using std::vector;

class Stts : public Window{
  public:
    Stts();
    ~Stts();

  private:
    utf_string title;
};

class Prdt : public Window
{
  public:
    Prdt();
    ~Prdt();

    void append( const char* s);
    void draw();
    void clear();
    
  private:
    utf_string title;
};


class Lkc : public Window
{
  public:
    Lkc();
    ~Lkc();

    void append( const char* s );
    void draw();
    void clear();

  private:
    vector<utf_string> title;
};
