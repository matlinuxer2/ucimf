#include <vector>
#include "layer.h"
using std::vector;

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
