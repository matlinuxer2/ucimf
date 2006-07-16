#include <vector>
#include "layer.h"
using std::vector;

class Stts : public Window, public Observer{
  public:
    Stts();
    ~Stts();
    void update();
  private:
    String title;
};

class Prdt : public Window, public Observer
{
  public:
    Prdt();
    ~Prdt();

    bool update();
    void append( const char* s);
    bool draw();
    
  private:
    String title;
};


class Lkc : public Window, public Observer
{
  public:
    Lkc();
    ~Lkc();

    bool update();
    void append( const char* s );
    bool draw();

  private:
    vector<String> title;
};
