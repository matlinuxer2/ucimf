#include <iiimcf.h>
#include <iiimp.h>

using namespace std;

class Rectangle;
class Text;
class BitMap;

class Prdt
{
  public:
    Prdt();
    Prdt( IIIMCF_context context );
    ~Prdt(){};

    void info(); // for text-mode checking.

    bool update();
    bool position(int x, int y);
    bool isVisible();
    void shift();
    void empty();
    void setShow();
    void setHide();
    void show();
    void hide();
    
  private:

    IIIMCF_context context;
    vector<IIIMP_card16> buf_utf16;
    bool visible;
    int cur_x, cur_y;
    int shift_x, shift_y;
    int cur_pos;
    Text* prdt_text;
    Rectangle* rect;
    BitMap prdt_tmp;
};
